#ifndef IDT_H
#define IDT_H

void idt_init();
void trigger_syscall_interrupt();
void syscall_interrupt_handler();
unsigned int timer_interrupt_handler(unsigned int current_esp);
unsigned int get_timer_ticks();

#endif