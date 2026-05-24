#include "process.h"
#include "vga.h"

#define MAX_PROCESSES 8
#define NAME_LENGTH 16
#define STACK_SIZE 4096

typedef unsigned int uint32_t;

typedef enum {
    PROCESS_READY,
    PROCESS_RUNNING,
    PROCESS_BLOCKED,
    PROCESS_KILLED
} process_state_t;

typedef struct {
    unsigned int pid;
    char name[NAME_LENGTH];
    process_state_t state;

    uint32_t esp;
    uint32_t stack_bottom;
    uint32_t stack_top;

    unsigned int priority;
    unsigned int counter;
    unsigned int runs;
} process_t;

static process_t processes[MAX_PROCESSES];
static unsigned char process_stacks[MAX_PROCESSES][STACK_SIZE];
static int last_running_process_index = -1;
static unsigned int next_pid = 1;
static int scheduler_enabled = 0;
static int current_process_index = -1;
static unsigned int kernel_esp = 0;

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

void process_print_hex_digit(unsigned int digit) {
    if (digit < 10) {
        print_char('0' + digit);
    } else {
        print_char('A' + digit - 10);
    }
}

void process_print_hex(unsigned int value) {
    print("0x");

    for (int i = 28; i >= 0; i -= 4) {
        process_print_hex_digit((value >> i) & 0xF);
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

int find_free_process_slot() {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].state == PROCESS_KILLED) {
            return i;
        }
    }

    return -1;
}

int find_next_ready_process(int start_index) {
    int best_index = -1;
    unsigned int best_priority = 0;

    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].state == PROCESS_READY ||
            processes[i].state == PROCESS_RUNNING) {
            if (processes[i].priority > best_priority) {
                best_priority = processes[i].priority;
            }
        }
    }

    if (best_priority == 0) {
        return -1;
    }

    for (int offset = 1; offset <= MAX_PROCESSES; offset++) {
        int index = (start_index + offset) % MAX_PROCESSES;

        if (index < 0) {
            index += MAX_PROCESSES;
        }

        if ((processes[index].state == PROCESS_READY ||
             processes[index].state == PROCESS_RUNNING) &&
             processes[index].priority == best_priority) {
            best_index = index;
            break;
        }
    }

    return best_index;
}

void task_counter() {
    volatile unsigned int local = 0;

    while (1) {
        local++;

        if (local % 1000000 == 0) {
            if (current_process_index >= 0) {
                processes[current_process_index].counter++;
            }
        }
    }
}

void task_logger() {
    volatile unsigned int local = 0;

    while (1) {
        local++;

        if (local % 1000000 == 0) {
            if (current_process_index >= 0) {
                processes[current_process_index].counter += 2;
            }
        }
    }
}

void task_worker() {
    volatile unsigned int local = 0;

    while (1) {
        local++;

        if (local % 1000000 == 0) {
            if (current_process_index >= 0) {
                processes[current_process_index].counter += 5;
            }
        }
    }
}

void process_exit_trap() {
    while (1) {
    }
}

uint32_t create_initial_stack(int index, void (*entry_point)()) {
    uint32_t *stack = (uint32_t *) (process_stacks[index] + STACK_SIZE);

    /*
        This stack must match what irq0 restores:

        popa restores:
            EDI, ESI, EBP, ignored ESP, EBX, EDX, ECX, EAX

        iret restores:
            EIP, CS, EFLAGS
    */

    *(--stack) = 0x202;                  // EFLAGS: interrupts enabled
    *(--stack) = 0x08;                   // CS: kernel code segment
    *(--stack) = (uint32_t) entry_point; // EIP: task entry point

    *(--stack) = 0; // EAX
    *(--stack) = 0; // ECX
    *(--stack) = 0; // EDX
    *(--stack) = 0; // EBX
    *(--stack) = 0; // Original ESP placeholder
    *(--stack) = 0; // EBP
    *(--stack) = 0; // ESI
    *(--stack) = 0; // EDI

    return (uint32_t) stack;
}

void process_init() {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        processes[i].pid = 0;
        processes[i].name[0] = '\0';
        processes[i].state = PROCESS_KILLED;
        processes[i].esp = 0;
        processes[i].stack_bottom = (uint32_t) &process_stacks[i][0];
        processes[i].stack_top = (uint32_t) (&process_stacks[i][STACK_SIZE]);
        processes[i].priority = 0;
        processes[i].counter = 0;
        processes[i].runs = 0;
    }

    next_pid = 1;
    scheduler_enabled = 0;
    current_process_index = -1;
    last_running_process_index = -1;
}

void process_run(const char *name) {
    void (*entry_point)() = 0;
    unsigned int priority = 1;

    if (process_string_equals(name, "counter")) {
        entry_point = task_counter;
        priority = 1;
    }
    else if (process_string_equals(name, "logger")) {
        entry_point = task_logger;
        priority = 2;
    }
    else if (process_string_equals(name, "worker")) {
        entry_point = task_worker;
        priority = 3;
    }
    else {
        print("Unknown process type. Use: run counter, run logger, or run worker\n");
        return;
    }

    int slot = find_free_process_slot();

    if (slot == -1) {
        print("Process table is full.\n");
        return;
    }

    processes[slot].pid = next_pid;
    processes[slot].state = PROCESS_READY;
    processes[slot].counter = 0;
    processes[slot].runs = 0;
    processes[slot].priority = priority;
    processes[slot].esp = create_initial_stack(slot, entry_point);

    process_copy_name(processes[slot].name, name);

    print("Started process ");
    process_print_number(next_pid);
    print(": ");
    print(name);
    print("\n");

    print("Stack top: ");
    process_print_hex(processes[slot].stack_top);
    print(", initial ESP: ");
    process_print_hex(processes[slot].esp);
    print("\n");

    next_pid++;
}

void process_list() {
    int found = 0;

    print("PID   NAME        PRIO   STATE      RUNS      COUNTER\n");
    print("-----------------------------------------------------\n");

    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].state != PROCESS_KILLED) {
            print_padded_number_process(processes[i].pid, 6);
            print_padded_text_process(processes[i].name, 12);
            print_padded_number_process(processes[i].priority, 7);

            if (processes[i].state == PROCESS_BLOCKED) {
                print_padded_text_process("blocked", 11);
            }
            else if (scheduler_enabled && i == last_running_process_index) {
                print_padded_text_process("last-run", 11);
            }
            else {
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
        if (processes[i].state != PROCESS_KILLED && processes[i].pid == pid) {
            processes[i].state = PROCESS_KILLED;

            if (current_process_index == i) {
                current_process_index = find_next_ready_process(i);
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

unsigned int process_schedule(unsigned int current_esp) {
    if (!scheduler_enabled) {
        return current_esp;
    }

    if (current_process_index == -1) {
        kernel_esp = current_esp;
    } else if (processes[current_process_index].state != PROCESS_KILLED) {
        processes[current_process_index].esp = current_esp;
        processes[current_process_index].state = PROCESS_READY;
    }

    int next_process = find_next_ready_process(current_process_index);

    if (next_process == -1) {
        current_process_index = -1;

        if (kernel_esp != 0) {
            return kernel_esp;
        }

        return current_esp;
    }

    if (current_process_index != -1 && next_process <= current_process_index) {
        current_process_index = -1;

        if (kernel_esp != 0) {
            return kernel_esp;
        }

        return current_esp;
    }

    current_process_index = next_process;
    last_running_process_index = next_process;
    processes[current_process_index].state = PROCESS_RUNNING;
    processes[current_process_index].runs++;

    return processes[current_process_index].esp;
}

void process_start_scheduler() {
    if (scheduler_enabled) {
        print("Scheduler is already running.\n");
        return;
    }

    if (find_next_ready_process(-1) == -1) {
        print("No active processes to schedule.\n");
        return;
    }

    scheduler_enabled = 1;
    current_process_index = -1;
    last_running_process_index = -1;
    kernel_esp = 0;

    print("Preemptive scheduler started.\n");
    print("PIT timer will now force task switches automatically.\n");
}

void process_scheduler_status() {
    print("Scheduler status:\n");

    if (scheduler_enabled) {
        print("State: running\n");
    } else {
        print("State: stopped\n");
    }

    print("Scheduling method: preemptive round-robin\n");
    print("Context switch source: PIT timer IRQ0\n");
    print("Register saving: assembly pusha/popa\n");
    print("Task stacks: isolated per-process kernel stacks\n");

    if (current_process_index != -1 &&
        processes[current_process_index].state != PROCESS_KILLED) {
        print("Current process: ");
        process_print_number(processes[current_process_index].pid);
        print(" (");
        print(processes[current_process_index].name);
        print(")\n");

        print("Current ESP: ");
        process_print_hex(processes[current_process_index].esp);
        print("\n");
    } else {
        print("Current process: none\n");
    }
}

void process_stop_scheduler() {
    scheduler_enabled = 0;
    current_process_index = -1;

    print("Preemptive scheduler stopped.\n");
}