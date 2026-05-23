#include "os_concepts.h"
#include "vga.h"
#include "idt.h"

void print_number_os(unsigned int value) {
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

void show_scheduler_demo() {
    print("Scheduler demo:\n");
    print("Algorithm: Round-robin simulation\n");
    print("\n");
    print("Task 1: running\n");
    print("Task 2: waiting\n");
    print("Task 3: ready\n");
    print("\n");
    print("Switching task...\n");
    print("Task 1: ready\n");
    print("Task 2: running\n");
    print("Task 3: waiting\n");
    print("\n");
    print("This demonstrates basic CPU time sharing.\n");
}

void show_task_status() {
    unsigned int ticks = get_timer_ticks();
    unsigned int current_task = (ticks / 300) % 3;

    print("Timer-based task status:\n");
    print("Timer ticks: ");
    print_number_os(ticks);
    print("\n");

    print("Scheduling method: round-robin simulation\n");

    if (current_task == 0) {
        print("Current task: Task 1\n");
        print("Task 1: running\n");
        print("Task 2: ready\n");
        print("Task 3: waiting\n");
    }
    else if (current_task == 1) {
        print("Current task: Task 2\n");
        print("Task 1: waiting\n");
        print("Task 2: running\n");
        print("Task 3: ready\n");
    }
    else {
        print("Current task: Task 3\n");
        print("Task 1: ready\n");
        print("Task 2: waiting\n");
        print("Task 3: running\n");
    }
}

void show_syscall_demo() {
    print("System call demo:\n");
    print("\n");
    print("User program requests a service from the kernel.\n");
    print("Example: print text to the screen.\n");
    print("\n");
    print("User mode request -> Kernel handles request -> Output returned\n");
    print("\n");
    print("MiniOS does not have real user mode yet.\n");
    print("This is an educational syscall simulation.\n");
}

void show_interrupts_demo() {
    print("Interrupts demo:\n");
    print("\n");
    print("An interrupt is a signal that gets the CPU's attention.\n");
    print("Example: keyboard input or timer tick.\n");
    print("\n");
    print("Keyboard mode: interrupt-driven IRQ1 input\n");
    print("Timer mode: interrupt-driven PIT timer\n");
    print("Software interrupt: int 0x80 enabled\n");
}