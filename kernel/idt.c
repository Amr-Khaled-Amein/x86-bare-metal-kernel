#include "idt.h"
#include "vga.h"

#define IDT_ENTRIES 256

typedef unsigned int   uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char  uint8_t;

struct idt_entry {
    uint16_t base_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t flags;
    uint16_t base_high;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

extern void idt_load(uint32_t idt_ptr_address);
extern void isr128();

static struct idt_entry idt[IDT_ENTRIES];
static struct idt_ptr idtp;

void idt_set_gate(int number, uint32_t base, uint16_t selector, uint8_t flags) {
    idt[number].base_low = base & 0xFFFF;
    idt[number].selector = selector;
    idt[number].zero = 0;
    idt[number].flags = flags;
    idt[number].base_high = (base >> 16) & 0xFFFF;
}

void idt_init() {
    idtp.limit = sizeof(struct idt_entry) * IDT_ENTRIES - 1;
    idtp.base = (uint32_t) &idt;

    for (int i = 0; i < IDT_ENTRIES; i++) {
        idt_set_gate(i, 0, 0, 0);
    }

    // Interrupt 0x80: software interrupt / syscall-style interrupt
    idt_set_gate(128, (uint32_t) isr128, 0x08, 0x8E);

    idt_load((uint32_t) &idtp);
}

void trigger_syscall_interrupt() {
    __asm__ volatile ("int $0x80");
}

void syscall_interrupt_handler() {
    print("Interrupt 0x80 received by the kernel.\n");
    print("This demonstrates real IDT-based software interrupt handling.\n");
}