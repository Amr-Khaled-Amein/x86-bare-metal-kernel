#include "vga.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY 0xB8000

static int cursor_row = 0;
static int cursor_col = 0;
static unsigned char text_color = 0x0F;

void clear_screen() {
    char *video_memory = (char *) VGA_MEMORY;

    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        video_memory[i * 2] = ' ';
        video_memory[i * 2 + 1] = text_color;
    }

    cursor_row = 0;
    cursor_col = 0;
}

void print_newline() {
    cursor_col = 0;
    cursor_row++;

    if (cursor_row >= VGA_HEIGHT) {
        cursor_row = 0;
    }
}

void print(const char *message) {
    char *video_memory = (char *) VGA_MEMORY;

    for (int i = 0; message[i] != '\0'; i++) {
        if (message[i] == '\n') {
            print_newline();
            continue;
        }

        int index = (cursor_row * VGA_WIDTH + cursor_col) * 2;

        video_memory[index] = message[i];
        video_memory[index + 1] = text_color;

        cursor_col++;

        if (cursor_col >= VGA_WIDTH) {
            print_newline();
        }
    }
}