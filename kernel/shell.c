#include "shell.h"
#include "vga.h"
#include "memory.h"
#include "os_concepts.h"
#include "heap.h"
#include "idt.h"
#include "text_buffer.h"
#include "history.h"

int string_equals(const char *a, const char *b) {
    int i = 0;

    while (a[i] != '\0' && b[i] != '\0') {
        if (a[i] != b[i]) {
            return 0;
        }
        i++;
    }

    return a[i] == '\0' && b[i] == '\0';
}

void shell_start() {
    print("MiniOS Shell\n");
    print("Type 'help' to see available commands.\n");
    print("\n");
    print("MiniOS> ");
}

void print_number(unsigned int value) {
    char buffer[16];
    int i = 0;

    if (value == 0) {
        print_char('0');
        return;
    }

    while (value > 0) {
        buffer[i] = '0' + (value % 10);
        value = value / 10;
        i++;
    }

    for (int j = i - 1; j >= 0; j--) {
        print_char(buffer[j]);
    }
}

void shell_handle_command(const char *command) {
    if (command[0] != '\0') {
        history_add(command);
    }
    if (string_equals(command, "help")) {
        print("Available commands:\n");
        print("help       - Show available commands\n");
        print("clear      - Clear the screen\n");
        print("about      - Show information about MiniOS\n");
        print("version    - Show MiniOS version\n");
        print("memory     - Show basic memory information\n");
        print("alloc      - Allocate 64 bytes from kernel heap\n");
        print("free       - Free latest allocated heap block\n");
        print("blocks     - Show tracked heap blocks\n");
        print("heap       - Show heap memory information\n");
        print("scheduler  - Show scheduler simulation\n");
        print("syscall    - Show system call simulation\n");
        print("interrupts - Show interrupt explanation\n");
        print("int80      - Trigger software interrupt 0x80\n");
        print("ticks      - Show system timer ticks\n");
        print("tasks      - Show timer-based task status\n");
        print("edit       - Write text into RAM buffer\n");
        print("show       - Show saved RAM text buffer\n");
        print("clearbuf   - Clear saved RAM text buffer\n");
        print("bufinfo    - Show RAM text buffer information\n");
        print("history    - Show command history\n");

    }
    else if (string_equals(command, "clear")) {
        clear_screen();
    }
    else if (string_equals(command, "about")) {
        print("MiniOS is a small educational x86 kernel project.\n");
        print("It is built in C and Assembly and runs in QEMU.\n");
    }
    else if (string_equals(command, "version")) {
        print("MiniOS version 0.2\n");
    }
    else if (string_equals(command, "memory")) {
        show_memory_info();
    }
    else if (string_equals(command, "heap")) {
        show_heap_info();
    }
    else if (string_equals(command, "free")) {
        free_latest_block();
    }
    else if (string_equals(command, "blocks")) {
        show_heap_blocks();
    }
    else if (string_equals(command, "alloc")) {
        allocate_demo_block();
    }
    else if (string_equals(command, "scheduler")) {
        show_scheduler_demo();
    }
    else if (string_equals(command, "syscall")) {
        show_syscall_demo();
    }
    else if (string_equals(command, "interrupts")) {
        show_interrupts_demo();
    }
    else if (string_equals(command, "int80")) {
        print("Triggering interrupt 0x80...\n");
        trigger_syscall_interrupt();
    }
    else if (string_equals(command, "ticks")) {
        print("System ticks: ");
        print_number(get_timer_ticks());
        print("\n");
    }
    else if (string_equals(command, "tasks")) {
        show_task_status();
    }
    else if (string_equals(command, "edit")) {
        text_buffer_start_edit();
    }
    else if (string_equals(command, "show")) {
        text_buffer_show();
    }
    else if (string_equals(command, "clearbuf")) {
        text_buffer_clear();
    }
    else if (string_equals(command, "bufinfo")) {
        text_buffer_info();
    }
    else if (string_equals(command, "history")) {
        history_show();
    }
    else if (command[0] == '\0') {
        // Empty command: do nothing
    }
    else {
        print("Unknown command: ");
        print(command);
        print("\n");
        print("Type 'help' to see available commands.\n");
    }

    print("MiniOS> ");
}