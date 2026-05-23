#ifndef IDT_H
#define IDT_H

void idt_init();
void trigger_syscall_interrupt();
void syscall_interrupt_handler();
void timer_interrupt_handler();
unsigned int get_timer_ticks();

#endif