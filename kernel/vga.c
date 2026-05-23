#include "vga.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY 0xB8000

#define TERMINAL_ROWS 200

typedef struct {
    char character;
    unsigned char color;
} terminal_cell_t;

static terminal_cell_t terminal_buffer[TERMINAL_ROWS][VGA_WIDTH];

static int cursor_row = 0;
static int cursor_col = 0;

static int view_start_row = 0;

static unsigned char text_color = 0x0F;

int max_view_start() {
    int max = cursor_row - VGA_HEIGHT + 1;

    if (max < 0) {
        return 0;
    }

    if (max > TERMINAL_ROWS - VGA_HEIGHT) {
        return TERMINAL_ROWS - VGA_HEIGHT;
    }

    return max;
}

void render_screen() {
    char *video_memory = (char *) VGA_MEMORY;

    for (int row = 0; row < VGA_HEIGHT; row++) {
        for (int col = 0; col < VGA_WIDTH; col++) {
            int source_row = view_start_row + row;
            int index = (row * VGA_WIDTH + col) * 2;

            video_memory[index] = terminal_buffer[source_row][col].character;
            video_memory[index + 1] = terminal_buffer[source_row][col].color;
        }
    }
}

void vga_follow_bottom() {
    view_start_row = max_view_start();
    render_screen();
}

void clear_terminal_buffer() {
    for (int row = 0; row < TERMINAL_ROWS; row++) {
        for (int col = 0; col < VGA_WIDTH; col++) {
            terminal_buffer[row][col].character = ' ';
            terminal_buffer[row][col].color = text_color;
        }
    }
}

void clear_screen() {
    clear_terminal_buffer();

    cursor_row = 0;
    cursor_col = 0;
    view_start_row = 0;

    render_screen();
}

void shift_terminal_up() {
    for (int row = 1; row < TERMINAL_ROWS; row++) {
        for (int col = 0; col < VGA_WIDTH; col++) {
            terminal_buffer[row - 1][col] = terminal_buffer[row][col];
        }
    }

    for (int col = 0; col < VGA_WIDTH; col++) {
        terminal_buffer[TERMINAL_ROWS - 1][col].character = ' ';
        terminal_buffer[TERMINAL_ROWS - 1][col].color = text_color;
    }

    cursor_row = TERMINAL_ROWS - 1;
}

void print_newline() {
    cursor_col = 0;
    cursor_row++;

    if (cursor_row >= TERMINAL_ROWS) {
        shift_terminal_up();
    }

    vga_follow_bottom();
}

void print_char_colored(char c, unsigned char color) {
    if (c == '\n') {
        print_newline();
        return;
    }

    terminal_buffer[cursor_row][cursor_col].character = c;
    terminal_buffer[cursor_row][cursor_col].color = color;

    cursor_col++;

    if (cursor_col >= VGA_WIDTH) {
        print_newline();
    } else {
        vga_follow_bottom();
    }
}

void print_char(char c) {
    print_char_colored(c, text_color);
}

void print_colored(const char *message, unsigned char color) {
    for (int i = 0; message[i] != '\0'; i++) {
        print_char_colored(message[i], color);
    }
}

void print(const char *message) {
    print_colored(message, text_color);
}

void vga_backspace() {
    if (cursor_col > 0) {
        cursor_col--;
    } else if (cursor_row > 0) {
        cursor_row--;
        cursor_col = VGA_WIDTH - 1;
    }

    terminal_buffer[cursor_row][cursor_col].character = ' ';
    terminal_buffer[cursor_row][cursor_col].color = text_color;

    vga_follow_bottom();
}

void vga_scroll_up() {
    if (view_start_row > 0) {
        view_start_row--;
        render_screen();
    }
}

void vga_scroll_down() {
    int max = max_view_start();

    if (view_start_row < max) {
        view_start_row++;
        render_screen();
    }
}