global idt_load
global isr128
global irq0

extern syscall_interrupt_handler
extern timer_interrupt_handler

idt_load:
    mov eax, [esp + 4]
    lidt [eax]
    ret

isr128:
    pusha
    call syscall_interrupt_handler
    popa
    iret

irq0:
    pusha
    call timer_interrupt_handler
    popa
    iret