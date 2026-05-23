#include "shell.h"
#include "vga.h"
#include "memory.h"
#include "os_concepts.h"

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

void shell_handle_command(const char *command) {
    if (string_equals(command, "help")) {
        print("Available commands:\n");
        print("help       - Show available commands\n");
        print("clear      - Clear the screen\n");
        print("about      - Show information about MiniOS\n");
        print("version    - Show MiniOS version\n");
        print("memory     - Show basic memory information\n");
        print("scheduler  - Show scheduler simulation\n");
        print("syscall    - Show system call simulation\n");
        print("interrupts - Show interrupt explanation\n");
    }
    else if (string_equals(command, "clear")) {
        clear_screen();
    }
    else if (string_equals(command, "about")) {
        print("MiniOS is a small educational x86 kernel project.\n");
        print("It is built in C and Assembly and runs in QEMU.\n");
    }
    else if (string_equals(command, "version")) {
        print("MiniOS version 0.1\n");
    }
    else if (string_equals(command, "memory")) {
        show_memory_info();
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