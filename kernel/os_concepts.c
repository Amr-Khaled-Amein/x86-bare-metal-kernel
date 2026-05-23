#include "os_concepts.h"
#include "vga.h"

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
    print("Current keyboard mode: polling\n");
    print("Future improvement: real interrupt-driven keyboard input.\n");
}