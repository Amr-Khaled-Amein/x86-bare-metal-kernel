global idt_load
global isr128

extern syscall_interrupt_handler

idt_load:
    mov eax, [esp + 4]
    lidt [eax]
    ret

isr128:
    pusha
    call syscall_interrupt_handler
    popa
    iret