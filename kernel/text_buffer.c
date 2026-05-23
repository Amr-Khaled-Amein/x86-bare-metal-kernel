#include "text_buffer.h"
#include "vga.h"

#define TEXT_BUFFER_SIZE 512

static char saved_text[TEXT_BUFFER_SIZE];
static int saved_length = 0;
static int editing_mode = 0;

void text_buffer_start_edit() {
    editing_mode = 1;

    print("Text editor mode enabled.\n");
    print("Type text and press ENTER to save:\n");
}

void text_buffer_handle_input(const char *input) {
    saved_length = 0;

    while (input[saved_length] != '\0' && saved_length < TEXT_BUFFER_SIZE - 1) {
        saved_text[saved_length] = input[saved_length];
        saved_length++;
    }

    saved_text[saved_length] = '\0';
    editing_mode = 0;

    print("Text saved to RAM buffer.\n");
}

void text_buffer_show() {
    if (saved_length == 0) {
        print("Buffer is empty.\n");
        return;
    }

    print("Saved buffer:\n");
    print(saved_text);
    print("\n");
}

void text_buffer_clear() {
    for (int i = 0; i < TEXT_BUFFER_SIZE; i++) {
        saved_text[i] = '\0';
    }

    saved_length = 0;
    editing_mode = 0;

    print("RAM text buffer cleared.\n");
}

void text_buffer_info() {
    print("Text buffer information:\n");
    print("Storage: RAM\n");
    print("Maximum size: 512 bytes\n");
    print("Current used: ");

    char buffer[16];
    int value = saved_length;
    int i = 0;

    if (value == 0) {
        print_char('0');
    } else {
        while (value > 0) {
            buffer[i] = '0' + (value % 10);
            value = value / 10;
            i++;
        }

        for (int j = i - 1; j >= 0; j--) {
            print_char(buffer[j]);
        }
    }

    print(" bytes\n");
}

int text_buffer_is_editing() {
    return editing_mode;
}