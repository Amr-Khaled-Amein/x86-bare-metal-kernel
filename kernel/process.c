#include "process.h"
#include "vga.h"

#define MAX_PROCESSES 8
#define NAME_LENGTH 16

typedef struct {
    unsigned int pid;
    char name[NAME_LENGTH];
    int active;
    unsigned int counter;
    unsigned int runs;
} process_t;

static process_t processes[MAX_PROCESSES];
static unsigned int next_pid = 1;

static int scheduler_enabled = 0;
static int current_process_index = -1;

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

void print_spaces_process(int count) {
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

void print_padded_text_process(const char *text, int width) {
    int length = string_length_process(text);

    print(text);

    if (length < width) {
        print_spaces_process(width - length);
    }
}

void print_padded_number_process(unsigned int value, int width) {
    int length = number_length_process(value);

    process_print_number(value);

    if (length < width) {
        print_spaces_process(width - length);
    }
}

int find_next_active_process(int start_index) {
    for (int offset = 1; offset <= MAX_PROCESSES; offset++) {
        int index = (start_index + offset) % MAX_PROCESSES;

        if (processes[index].active) {
            return index;
        }
    }

    return -1;
}

void process_init() {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        processes[i].pid = 0;
        processes[i].active = 0;
        processes[i].counter = 0;
        processes[i].runs = 0;
        processes[i].name[0] = '\0';
    }

    next_pid = 1;
    scheduler_enabled = 0;
    current_process_index = -1;
}

void process_run(const char *name) {
    if (!process_string_equals(name, "counter") &&
        !process_string_equals(name, "logger") &&
        !process_string_equals(name, "worker")) {
        print("Unknown process type. Use: run counter, run logger, or run worker\n");
        return;
    }

    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (!processes[i].active) {
            processes[i].pid = next_pid;
            processes[i].active = 1;
            processes[i].counter = 0;
            processes[i].runs = 0;
            process_copy_name(processes[i].name, name);

            print("Started process ");
            process_print_number(next_pid);
            print(": ");
            print(name);
            print("\n");

            if (current_process_index == -1) {
                current_process_index = i;
            }

            next_pid++;
            return;
        }
    }

    print("Process table is full.\n");
}

void process_list() {
    int found = 0;

    print("PID   NAME        STATE      RUNS      COUNTER\n");
    print("----------------------------------------------\n");

    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].active) {
            print_padded_number_process(processes[i].pid, 6);
            print_padded_text_process(processes[i].name, 12);

            if (scheduler_enabled && i == current_process_index) {
                print_padded_text_process("running", 11);
            } else {
                print_padded_text_process("ready", 11);
            }

            print_padded_number_process(processes[i].runs, 10);
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

            if (current_process_index == i) {
                current_process_index = find_next_active_process(i);
            }

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

void run_process_behavior(process_t *process) {
    process->runs++;

    if (process_string_equals(process->name, "counter")) {
        process->counter += 1;
    }
    else if (process_string_equals(process->name, "logger")) {
        process->counter += 2;
    }
    else if (process_string_equals(process->name, "worker")) {
        process->counter += 5;
    }
}

void process_tick() {
    if (!scheduler_enabled) {
        return;
    }

    if (current_process_index == -1) {
        current_process_index = find_next_active_process(0);
        return;
    }

    if (!processes[current_process_index].active) {
        current_process_index = find_next_active_process(current_process_index);
        return;
    }

    run_process_behavior(&processes[current_process_index]);

    current_process_index = find_next_active_process(current_process_index);
}

void process_start_scheduler() {
    if (scheduler_enabled) {
        print("Scheduler is already running.\n");
        return;
    }

    if (find_next_active_process(0) == -1) {
        print("No active processes to schedule.\n");
        return;
    }

    scheduler_enabled = 1;

    if (current_process_index == -1 || !processes[current_process_index].active) {
        current_process_index = find_next_active_process(0);
    }

    print("Cooperative scheduler started.\n");
}

void process_stop_scheduler() {
    scheduler_enabled = 0;
    print("Cooperative scheduler stopped.\n");
}

void process_scheduler_status() {
    print("Scheduler status:\n");

    if (scheduler_enabled) {
        print("State: running\n");
    } else {
        print("State: stopped\n");
    }

    if (current_process_index != -1 && processes[current_process_index].active) {
        print("Current process: ");
        process_print_number(processes[current_process_index].pid);
        print(" (");
        print(processes[current_process_index].name);
        print(")\n");
    } else {
        print("Current process: none\n");
    }

    print("Scheduling method: round-robin over active process table\n");
}