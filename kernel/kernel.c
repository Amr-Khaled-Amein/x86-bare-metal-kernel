#include "vga.h"
#include "keyboard.h"
#include "shell.h"
#include "heap.h"
#include "idt.h"
#include "minifs.h"

void kernel_main() {
    clear_screen();

    heap_init();
    minifs_init();
    idt_init();

    print("========================================\n");
    print("              MiniOS\n");
    print("========================================\n");
    print("\n");
    print("Welcome to MiniOS Kernel\n");
    print("\n");
    print("Status: Boot successful\n");
    print("Mode: 32-bit protected mode\n");
    print("Display: VGA text mode working\n");
    print("Keyboard: PS/2 keyboard polling enabled\n");
    print("Shell: Basic command handling enabled\n");
    print("Heap: Simple kernel heap initialized\n");
    print("IDT: Interrupt Descriptor Table initialized\n");
    print("Timer: PIT timer interrupt enabled\n");
    print("RAM App: Text buffer application available\n");
    print("Keyboard: IRQ1 interrupt-driven input enabled\n");
    print("MiniFS: In-memory file system initialized\n");
    print("\n");
    print("----------------------------------------\n");
    print("System initialization complete.\n");
    print("----------------------------------------\n");
    print("\n");

    shell_start();

    keyboard_loop();

    while (1) {
    }
}