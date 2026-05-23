#include "keyboard.h"
#include "ports.h"
#include "vga.h"
#include "shell.h"
#include "text_buffer.h"
#include "history.h"

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define INPUT_BUFFER_SIZE 128

static int shift_pressed = 0;
static int input_length = 0;
static int extended_scancode = 0;
static char input_buffer[INPUT_BUFFER_SIZE];

static char scancode_to_ascii[128] = {
    0,  27, '1', '2', '3', '4', '5', '6',
    '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y',
    'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h',
    'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b',
    'n', 'm', ',', '.', '/', 0,
    '*', 0, ' '
};

char apply_shift(char c) {
    if (c >= 'a' && c <= 'z') {
        return c - 32;
    }

    switch (c) {
        case '1': return '!';
        case '2': return '@';
        case '3': return '#';
        case '4': return '$';
        case '5': return '%';
        case '6': return '^';
        case '7': return '&';
        case '8': return '*';
        case '9': return '(';
        case '0': return ')';
        case '-': return '_';
        case '=': return '+';
        case '[': return '{';
        case ']': return '}';
        case ';': return ':';
        case '\'': return '"';
        case ',': return '<';
        case '.': return '>';
        case '/': return '?';
        case '\\': return '|';
        case '`': return '~';
        default: return c;
    }
}

void clear_current_input() {
    while (input_length > 0) {
        vga_backspace();
        input_length--;
    }

    input_buffer[0] = '\0';
}

void load_command_into_input(const char *command) {
    clear_current_input();

    int i = 0;

    while (command[i] != '\0' && i < INPUT_BUFFER_SIZE - 1) {
        input_buffer[i] = command[i];
        print_char(command[i]);
        i++;
    }

    input_buffer[i] = '\0';
    input_length = i;
}

void keyboard_loop() {
    while (1) {
        unsigned char status = inb(KEYBOARD_STATUS_PORT);

        if (status & 1) {
            unsigned char scancode = inb(KEYBOARD_DATA_PORT);

            if (scancode == 0xE0) {
                extended_scancode = 1;
                continue;
            }

            if (extended_scancode) {
                extended_scancode = 0;

                if (!text_buffer_is_editing()) {
                    char history_command[INPUT_BUFFER_SIZE];

                    // Up arrow
                    if (scancode == 0x48) {
                        if (history_get_previous(history_command, INPUT_BUFFER_SIZE)) {
                            load_command_into_input(history_command);
                        }
                        continue;
                    }

                    // Down arrow
                    if (scancode == 0x50) {
                        if (history_get_next(history_command, INPUT_BUFFER_SIZE)) {
                            load_command_into_input(history_command);
                        }
                        continue;
                    }
                }

                continue;
            }

            if (scancode == 0x2A || scancode == 0x36) {
                shift_pressed = 1;
                continue;
            }

            if (scancode == 0xAA || scancode == 0xB6) {
                shift_pressed = 0;
                continue;
            }

            if (scancode > 127) {
                continue;
            }

            char key = scancode_to_ascii[scancode];

            if (key == '\b') {
                if (input_length > 0) {
                    vga_backspace();
                    input_length--;
                    input_buffer[input_length] = '\0';
                }
                continue;
            }

            if (key == '\n') {
                print_newline();

                input_buffer[input_length] = '\0';

                if (text_buffer_is_editing()) {
                    text_buffer_handle_input(input_buffer);
                    print("MiniOS> ");
                } else {
                    shell_handle_command(input_buffer);
                    history_reset_navigation();
                }

                input_length = 0;
                input_buffer[0] = '\0';

                continue;
            }

            if (key != 0) {
                if (shift_pressed) {
                    key = apply_shift(key);
                }

                if (input_length < INPUT_BUFFER_SIZE - 1) {
                    print_char(key);
                    input_buffer[input_length] = key;
                    input_length++;
                    input_buffer[input_length] = '\0';
                }
            }
        }
    }
}