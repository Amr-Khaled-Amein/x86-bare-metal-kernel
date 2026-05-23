#include "keyboard.h"
#include "ports.h"
#include "vga.h"
#include "shell.h"
#include "text_buffer.h"
#include "history.h"

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define PIC1_COMMAND 0x20

#define INPUT_BUFFER_SIZE 128
#define SCANCODE_QUEUE_SIZE 64

static int shift_pressed = 0;
static int input_length = 0;
static int extended_scancode = 0;

static char input_buffer[INPUT_BUFFER_SIZE];

static volatile unsigned char scancode_queue[SCANCODE_QUEUE_SIZE];
static volatile int queue_head = 0;
static volatile int queue_tail = 0;

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

void enqueue_scancode(unsigned char scancode) {
    int next_tail = (queue_tail + 1) % SCANCODE_QUEUE_SIZE;

    if (next_tail != queue_head) {
        scancode_queue[queue_tail] = scancode;
        queue_tail = next_tail;
    }
}

int dequeue_scancode(unsigned char *scancode) {
    if (queue_head == queue_tail) {
        return 0;
    }

    *scancode = scancode_queue[queue_head];
    queue_head = (queue_head + 1) % SCANCODE_QUEUE_SIZE;

    return 1;
}

void keyboard_interrupt_handler() {
    unsigned char status = inb(KEYBOARD_STATUS_PORT);

    if (status & 1) {
        unsigned char scancode = inb(KEYBOARD_DATA_PORT);
        enqueue_scancode(scancode);
    }

    // Send End of Interrupt signal to PIC
    outb(PIC1_COMMAND, 0x20);
}

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

void process_scancode(unsigned char scancode) {
    if (scancode == 0xE0) {
        extended_scancode = 1;
        return;
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
                return;
            }

            // Down arrow
            if (scancode == 0x50) {
                if (history_get_next(history_command, INPUT_BUFFER_SIZE)) {
                    load_command_into_input(history_command);
                }
                return;
            }
        }

        return;
    }

    if (scancode == 0x2A || scancode == 0x36) {
        shift_pressed = 1;
        return;
    }

    if (scancode == 0xAA || scancode == 0xB6) {
        shift_pressed = 0;
        return;
    }

    if (scancode > 127) {
        return;
    }

    char key = scancode_to_ascii[scancode];

    if (key == '\b') {
        if (input_length > 0) {
            vga_backspace();
            input_length--;
            input_buffer[input_length] = '\0';
        }
        return;
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

        return;
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

void keyboard_loop() {
    unsigned char scancode;

    while (1) {
        if (dequeue_scancode(&scancode)) {
            process_scancode(scancode);
        } else {
            __asm__ volatile ("hlt");
        }
    }
}