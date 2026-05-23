#include "idt.h"
#include "vga.h"
#include "ports.h"

#define IDT_ENTRIES 256

#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

#define PIT_COMMAND  0x43
#define PIT_CHANNEL0 0x40

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
extern void irq0();
extern void irq1();
extern void irq12();

static struct idt_entry idt[IDT_ENTRIES];
static struct idt_ptr idtp;

static unsigned int timer_ticks = 0;

void idt_set_gate(int number, uint32_t base, uint16_t selector, uint8_t flags) {
    idt[number].base_low = base & 0xFFFF;
    idt[number].selector = selector;
    idt[number].zero = 0;
    idt[number].flags = flags;
    idt[number].base_high = (base >> 16) & 0xFFFF;
}

void pic_remap() {
    outb(PIC1_COMMAND, 0x11);
    outb(PIC2_COMMAND, 0x11);

    outb(PIC1_DATA, 0x20);
    outb(PIC2_DATA, 0x28);

    outb(PIC1_DATA, 0x04);
    outb(PIC2_DATA, 0x02);

    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);

    outb(PIC1_DATA, 0xFC);

    outb(PIC2_DATA, 0xFF);
}

void timer_init() {
    unsigned int frequency = 100;
    unsigned int divisor = 1193180 / frequency;

    outb(PIT_COMMAND, 0x36);
    outb(PIT_CHANNEL0, divisor & 0xFF);
    outb(PIT_CHANNEL0, (divisor >> 8) & 0xFF);
}

void idt_init() {
    idtp.limit = sizeof(struct idt_entry) * IDT_ENTRIES - 1;
    idtp.base = (uint32_t) &idt;

    for (int i = 0; i < IDT_ENTRIES; i++) {
        idt_set_gate(i, 0, 0, 0);
    }

    pic_remap();

    idt_set_gate(32, (uint32_t) irq0, 0x08, 0x8E);
    idt_set_gate(33, (uint32_t) irq1, 0x08, 0x8E);
    idt_set_gate(128, (uint32_t) isr128, 0x08, 0x8E);

    idt_load((uint32_t) &idtp);

    timer_init();

    __asm__ volatile ("sti");
}

void trigger_syscall_interrupt() {
    __asm__ volatile ("int $0x80");
}

void syscall_interrupt_handler() {
    print("Interrupt 0x80 received by the kernel.\n");
    print("This demonstrates real IDT-based software interrupt handling.\n");
}

void timer_interrupt_handler() {
    timer_ticks++;
    outb(PIC1_COMMAND, 0x20);
}

unsigned int get_timer_ticks() {
    return timer_ticks;
}