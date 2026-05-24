#include "gdt.h"
#include "vga.h"

typedef unsigned int   uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char  uint8_t;

#define GDT_ENTRIES 6

#define KERNEL_CODE_SELECTOR 0x08
#define KERNEL_DATA_SELECTOR 0x10
#define USER_CODE_SELECTOR   0x1B
#define USER_DATA_SELECTOR   0x23
#define TSS_SELECTOR         0x28

struct gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed));

struct gdt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

struct tss_entry {
    uint32_t prev_tss;
    uint32_t esp0;
    uint32_t ss0;
    uint32_t esp1;
    uint32_t ss1;
    uint32_t esp2;
    uint32_t ss2;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t es;
    uint32_t cs;
    uint32_t ss;
    uint32_t ds;
    uint32_t fs;
    uint32_t gs;
    uint32_t ldt;
    uint16_t trap;
    uint16_t iomap_base;
} __attribute__((packed));

extern void gdt_flush(uint32_t gdt_ptr_address);
extern void tss_flush();

static struct gdt_entry gdt[GDT_ENTRIES];
static struct gdt_ptr gp;
static struct tss_entry tss;

static unsigned char kernel_tss_stack[4096];

void memory_set_gdt(unsigned char *dest, unsigned char value, unsigned int length) {
    for (unsigned int i = 0; i < length; i++) {
        dest[i] = value;
    }
}

void gdt_set_gate(int number, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity) {
    gdt[number].base_low = base & 0xFFFF;
    gdt[number].base_middle = (base >> 16) & 0xFF;
    gdt[number].base_high = (base >> 24) & 0xFF;

    gdt[number].limit_low = limit & 0xFFFF;
    gdt[number].granularity = (limit >> 16) & 0x0F;

    gdt[number].granularity |= granularity & 0xF0;
    gdt[number].access = access;
}

void write_tss(int number, uint16_t ss0, uint32_t esp0) {
    uint32_t base = (uint32_t) &tss;
    uint32_t limit = base + sizeof(struct tss_entry);

    gdt_set_gate(number, base, limit, 0xE9, 0x00);

    memory_set_gdt((unsigned char *) &tss, 0, sizeof(struct tss_entry));

    tss.ss0 = ss0;
    tss.esp0 = esp0;

    tss.cs = USER_CODE_SELECTOR;
    tss.ss = USER_DATA_SELECTOR;
    tss.ds = USER_DATA_SELECTOR;
    tss.es = USER_DATA_SELECTOR;
    tss.fs = USER_DATA_SELECTOR;
    tss.gs = USER_DATA_SELECTOR;

    tss.iomap_base = sizeof(struct tss_entry);
}

void tss_set_kernel_stack(unsigned int stack) {
    tss.esp0 = stack;
}

void gdt_init() {
    gp.limit = sizeof(struct gdt_entry) * GDT_ENTRIES - 1;
    gp.base = (uint32_t) &gdt;

    gdt_set_gate(0, 0, 0, 0, 0);

    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Ring 0 code
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Ring 0 data

    gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // Ring 3 code
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // Ring 3 data

    write_tss(5, KERNEL_DATA_SELECTOR, (uint32_t) (kernel_tss_stack + 4096));

    gdt_flush((uint32_t) &gp);
    tss_flush();

    print("GDT: Ring 0/Ring 3 segments loaded\n");
    print("TSS: Kernel stack for user-mode interrupts loaded\n");
}