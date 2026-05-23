#include "vga.h"

void kernel_main() {
    clear_screen();

    print("Welcome to MiniOS\n");
    print("Simple educational kernel project\n");
    print("\n");
    print("Status: Boot successful\n");
    print("Display: VGA text mode working\n");

    while (1) {
    }
}