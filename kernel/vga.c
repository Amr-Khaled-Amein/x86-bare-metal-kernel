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

void scroll_screen() {
    char *video_memory = (char *) VGA_MEMORY;

    // Move every line one row up
    for (int row = 1; row < VGA_HEIGHT; row++) {
        for (int col = 0; col < VGA_WIDTH; col++) {
            int from = (row * VGA_WIDTH + col) * 2;
            int to = ((row - 1) * VGA_WIDTH + col) * 2;

            video_memory[to] = video_memory[from];
            video_memory[to + 1] = video_memory[from + 1];
        }
    }

    // Clear the last line
    int last_row = VGA_HEIGHT - 1;

    for (int col = 0; col < VGA_WIDTH; col++) {
        int index = (last_row * VGA_WIDTH + col) * 2;
        video_memory[index] = ' ';
        video_memory[index + 1] = text_color;
    }

    cursor_row = VGA_HEIGHT - 1;
    cursor_col = 0;
}

void print_newline() {
    cursor_col = 0;
    cursor_row++;

    if (cursor_row >= VGA_HEIGHT) {
        scroll_screen();
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