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

void print_char(char c) {
    char *video_memory = (char *) VGA_MEMORY;

    if (c == '\n') {
        print_newline();
        return;
    }

    int index = (cursor_row * VGA_WIDTH + cursor_col) * 2;

    video_memory[index] = c;
    video_memory[index + 1] = text_color;

    cursor_col++;

    if (cursor_col >= VGA_WIDTH) {
        print_newline();
    }
}

void print(const char *message) {
    for (int i = 0; message[i] != '\0'; i++) {
        print_char(message[i]);
    }
}

void vga_backspace() {
    char *video_memory = (char *) VGA_MEMORY;

    if (cursor_col > 0) {
        cursor_col--;
    } else if (cursor_row > 0) {
        cursor_row--;
        cursor_col = VGA_WIDTH - 1;
    }

    int index = (cursor_row * VGA_WIDTH + cursor_col) * 2;
    video_memory[index] = ' ';
    video_memory[index + 1] = text_color;
}