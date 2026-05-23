#include "vga.h"
#include "keyboard.h"

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
    print("Keyboard: PS/2 keyboard polling enabled\n");
    print("\n");
    print("----------------------------------------\n");
    print("Type on the keyboard. MiniOS will echo it.\n");
    print("----------------------------------------\n");
    print("\n");
    print("MiniOS> ");

    keyboard_loop();

    while (1) {
    }
}