#include "vga.h"

void kernel_main() {
    clear_screen();

    print("========================================\n");
    print("              MiniOS\n");
    print("========================================\n");
    print("\n");
    print("Welcome to MiniOS Kernel\n");
    print("\n");
    print("Status: Boot successful\n");
    print("Mode: 32-bit protected mode\n");
    print("Display: VGA text mode working\n");
    print("Environment: QEMU emulator\n");
    print("\n");
    print("----------------------------------------\n");
    print("System initialization complete.\n");
    print("----------------------------------------\n");
    print("\n");
    print("Next step: keyboard input and shell\n");

    while (1) {
    }
}