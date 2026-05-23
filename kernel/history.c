#include "history.h"
#include "vga.h"

#define HISTORY_SIZE 10
#define COMMAND_SIZE 128

static char command_history[HISTORY_SIZE][COMMAND_SIZE];
static int history_count = 0;
static int history_nav_index = 0;

void copy_string_history(char *destination, const char *source, int max_size) {
    int i = 0;

    while (source[i] != '\0' && i < max_size - 1) {
        destination[i] = source[i];
        i++;
    }

    destination[i] = '\0';
}

int string_equals_history(const char *a, const char *b) {
    int i = 0;

    while (a[i] != '\0' && b[i] != '\0') {
        if (a[i] != b[i]) {
            return 0;
        }
        i++;
    }

    return a[i] == '\0' && b[i] == '\0';
}

void print_number_history(int value) {
    char buffer[16];
    int i = 0;

    if (value == 0) {
        print_char('0');
        return;
    }

    while (value > 0) {
        buffer[i] = '0' + (value % 10);
        value = value / 10;
        i++;
    }

    for (int j = i - 1; j >= 0; j--) {
        print_char(buffer[j]);
    }
}

void history_add(const char *command) {
    if (command[0] == '\0') {
        return;
    }

    // Avoid saving the same command twice in a row
    if (history_count > 0) {
        if (string_equals_history(command_history[history_count - 1], command)) {
            history_nav_index = history_count;
            return;
        }
    }

    if (history_count < HISTORY_SIZE) {
        copy_string_history(command_history[history_count], command, COMMAND_SIZE);
        history_count++;
    } else {
        for (int i = 1; i < HISTORY_SIZE; i++) {
            copy_string_history(command_history[i - 1], command_history[i], COMMAND_SIZE);
        }

        copy_string_history(command_history[HISTORY_SIZE - 1], command, COMMAND_SIZE);
    }

    history_nav_index = history_count;
}

void history_show() {
    if (history_count == 0) {
        print("Command history is empty.\n");
        return;
    }

    print("Command history:\n");

    for (int i = 0; i < history_count; i++) {
        print_number_history(i + 1);
        print(". ");
        print(command_history[i]);
        print("\n");
    }
}

int history_get_previous(char *buffer, int buffer_size) {
    if (history_count == 0) {
        return 0;
    }

    if (history_nav_index > 0) {
        history_nav_index--;
    }

    copy_string_history(buffer, command_history[history_nav_index], buffer_size);
    return 1;
}

int history_get_next(char *buffer, int buffer_size) {
    if (history_count == 0) {
        return 0;
    }

    if (history_nav_index < history_count - 1) {
        history_nav_index++;
        copy_string_history(buffer, command_history[history_nav_index], buffer_size);
        return 1;
    }

    history_nav_index = history_count;
    buffer[0] = '\0';
    return 1;
}

void history_reset_navigation() {
    history_nav_index = history_count;
}