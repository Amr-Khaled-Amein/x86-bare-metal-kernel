#include "process.h"
#include "vga.h"

#define MAX_PROCESSES 8
#define NAME_LENGTH 16

typedef struct {
    unsigned int pid;
    char name[NAME_LENGTH];
    int active;
    unsigned int counter;
} process_t;

static process_t processes[MAX_PROCESSES];
static unsigned int next_pid = 1;

void process_copy_name(char *dest, const char *src) {
    int i = 0;

    while (src[i] != '\0' && i < NAME_LENGTH - 1) {
        dest[i] = src[i];
        i++;
    }

    dest[i] = '\0';
}

int process_string_equals(const char *a, const char *b) {
    int i = 0;

    while (a[i] != '\0' && b[i] != '\0') {
        if (a[i] != b[i]) {
            return 0;
        }
        i++;
    }

    return a[i] == '\0' && b[i] == '\0';
}

void process_print_number(unsigned int value) {
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

void process_init() {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        processes[i].pid = 0;
        processes[i].active = 0;
        processes[i].counter = 0;
        processes[i].name[0] = '\0';
    }

    next_pid = 1;
}

void process_run(const char *name) {
    if (!process_string_equals(name, "counter") &&
        !process_string_equals(name, "logger")) {
        print("Unknown process type. Use: run counter or run logger\n");
        return;
    }

    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (!processes[i].active) {
            processes[i].pid = next_pid;
            processes[i].active = 1;
            processes[i].counter = 0;
            process_copy_name(processes[i].name, name);

            print("Started process ");
            process_print_number(next_pid);
            print(": ");
            print(name);
            print("\n");

            next_pid++;
            return;
        }
    }

    print("Process table is full.\n");
}

void print_spaces(int count) {
    for (int i = 0; i < count; i++) {
        print(" ");
    }
}

int string_length_process(const char *text) {
    int length = 0;

    while (text[length] != '\0') {
        length++;
    }

    return length;
}

int number_length_process(unsigned int value) {
    int length = 0;

    if (value == 0) {
        return 1;
    }

    while (value > 0) {
        value = value / 10;
        length++;
    }

    return length;
}

void print_padded_text(const char *text, int width) {
    int length = string_length_process(text);

    print(text);

    if (length < width) {
        print_spaces(width - length);
    }
}

void print_padded_number(unsigned int value, int width) {
    int length = number_length_process(value);

    process_print_number(value);

    if (length < width) {
        print_spaces(width - length);
    }
}

void process_list() {
    int found = 0;

    print("PID   NAME        STATE      COUNTER\n");
    print("------------------------------------\n");

    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].active) {
            print_padded_number(processes[i].pid, 6);
            print_padded_text(processes[i].name, 12);
            print_padded_text("running", 11);
            process_print_number(processes[i].counter);
            print("\n");

            found = 1;
        }
    }

    if (!found) {
        print("No active processes.\n");
    }
}

void process_kill(unsigned int pid) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].active && processes[i].pid == pid) {
            processes[i].active = 0;

            print("Killed process ");
            process_print_number(pid);
            print(".\n");
            return;
        }
    }

    print("Process not found: ");
    process_print_number(pid);
    print("\n");
}

void process_tick() {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].active) {
            processes[i].counter++;
        }
    }
}