global user_task_entry

user_task_entry:
    mov ax, 0x23
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

.user_loop:
    mov eax, 1
    int 0x80

    mov ecx, 5000000

.delay:
    loop .delay

    jmp .user_loop