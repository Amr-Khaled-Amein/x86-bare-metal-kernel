#include "heap.h"
#include "vga.h"

#define HEAP_START 0x200000
#define HEAP_SIZE  0x10000

static unsigned int heap_start = HEAP_START;
static unsigned int heap_current = HEAP_START;
static unsigned int heap_end = HEAP_START + HEAP_SIZE;

void heap_init() {
    heap_current = heap_start;
}

void *kmalloc(unsigned int size) {
    if (heap_current + size >= heap_end) {
        return 0;
    }

    void *allocated_address = (void *) heap_current;
    heap_current += size;

    return allocated_address;
}

void print_hex_digit(unsigned int digit) {
    if (digit < 10) {
        print_char('0' + digit);
    } else {
        print_char('A' + digit - 10);
    }
}

void print_hex(unsigned int value) {
    print("0x");

    for (int i = 28; i >= 0; i -= 4) {
        unsigned int digit = (value >> i) & 0xF;
        print_hex_digit(digit);
    }
}

void print_decimal(unsigned int value) {
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

void show_heap_info() {
    print("Heap information:\n");

    print("Heap start: ");
    print_hex(heap_start);
    print("\n");

    print("Heap current: ");
    print_hex(heap_current);
    print("\n");

    print("Heap end: ");
    print_hex(heap_end);
    print("\n");

    print("Heap size: ");
    print_decimal(HEAP_SIZE);
    print(" bytes\n");

    print("Heap used: ");
    print_decimal(heap_current - heap_start);
    print(" bytes\n");

    print("Heap free: ");
    print_decimal(heap_end - heap_current);
    print(" bytes\n");
}

void allocate_demo_block() {
    unsigned int size = 64;
    void *block = kmalloc(size);

    if (block == 0) {
        print("Allocation failed: heap is full.\n");
        return;
    }

    print("Allocated 64 bytes at address: ");
    print_hex((unsigned int) block);
    print("\n");
}