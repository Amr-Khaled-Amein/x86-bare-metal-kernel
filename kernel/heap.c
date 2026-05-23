#include "heap.h"
#include "vga.h"

#define HEAP_START 0x200000
#define HEAP_SIZE  0x10000
#define MAX_BLOCKS 32

typedef struct {
    unsigned int id;
    unsigned int address;
    unsigned int size;
    int used;
} heap_block_t;

static unsigned int heap_start = HEAP_START;
static unsigned int heap_current = HEAP_START;
static unsigned int heap_end = HEAP_START + HEAP_SIZE;

static heap_block_t blocks[MAX_BLOCKS];
static unsigned int block_count = 0;
static unsigned int next_block_id = 1;

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

void heap_init() {
    heap_current = heap_start;
    block_count = 0;
    next_block_id = 1;

    for (int i = 0; i < MAX_BLOCKS; i++) {
        blocks[i].id = 0;
        blocks[i].address = 0;
        blocks[i].size = 0;
        blocks[i].used = 0;
    }
}

void *kmalloc(unsigned int size) {
    if (size == 0) {
        return 0;
    }

    // First try to reuse a free block
    for (unsigned int i = 0; i < block_count; i++) {
        if (!blocks[i].used && blocks[i].size >= size) {
            blocks[i].used = 1;
            return (void *) blocks[i].address;
        }
    }

    // Otherwise reserve new heap memory
    if (heap_current + size >= heap_end) {
        return 0;
    }

    if (block_count >= MAX_BLOCKS) {
        return 0;
    }

    void *allocated_address = (void *) heap_current;

    blocks[block_count].id = next_block_id;
    blocks[block_count].address = heap_current;
    blocks[block_count].size = size;
    blocks[block_count].used = 1;

    block_count++;
    next_block_id++;

    heap_current += size;

    return allocated_address;
}

void allocate_custom_block(unsigned int size) {
    if (size == 0) {
        print("Usage: alloc SIZE\n");
        return;
    }

    unsigned int old_block_count = block_count;
    void *block = kmalloc(size);

    if (block == 0) {
        print("Allocation failed: heap is full or block table is full.\n");
        return;
    }

    // Check whether this was a reused block
    for (unsigned int i = 0; i < block_count; i++) {
        if (blocks[i].address == (unsigned int) block && blocks[i].used) {
            if (block_count == old_block_count) {
                print("Reused free block ID ");
            } else {
                print("Allocated block ID ");
            }

            print_decimal(blocks[i].id);
            print(" at address: ");
            print_hex(blocks[i].address);
            print(" (");
            print_decimal(size);
            print(" bytes requested)\n");
            return;
        }
    }
}

void allocate_demo_block() {
    allocate_custom_block(64);
}

void free_latest_block() {
    if (block_count == 0) {
        print("No blocks to free.\n");
        return;
    }

    for (int i = block_count - 1; i >= 0; i--) {
        if (blocks[i].used) {
            blocks[i].used = 0;

            print("Freed block ID ");
            print_decimal(blocks[i].id);
            print(".\n");

            return;
        }
    }

    print("No used blocks to free.\n");
}

void free_block_by_id(unsigned int id) {
    if (id == 0) {
        print("Usage: free ID\n");
        return;
    }

    for (unsigned int i = 0; i < block_count; i++) {
        if (blocks[i].id == id) {
            if (!blocks[i].used) {
                print("Block ID ");
                print_decimal(id);
                print(" is already free.\n");
                return;
            }

            blocks[i].used = 0;

            print("Freed block ID ");
            print_decimal(id);
            print(".\n");
            return;
        }
    }

    print("Block ID not found: ");
    print_decimal(id);
    print("\n");
}

void show_heap_blocks() {
    if (block_count == 0) {
        print("No heap blocks allocated yet.\n");
        return;
    }

    print("Heap blocks:\n");

    for (unsigned int i = 0; i < block_count; i++) {
        print("Block ");
        print_decimal(blocks[i].id);
        print(": ");

        print_decimal(blocks[i].size);
        print(" bytes, ");

        if (blocks[i].used) {
            print("used, ");
        } else {
            print("free, ");
        }

        print("address ");
        print_hex(blocks[i].address);
        print("\n");
    }
}

void show_heap_info() {
    unsigned int active_bytes = 0;
    unsigned int free_tracked_bytes = 0;

    for (unsigned int i = 0; i < block_count; i++) {
        if (blocks[i].used) {
            active_bytes += blocks[i].size;
        } else {
            free_tracked_bytes += blocks[i].size;
        }
    }

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

    print("Active allocated memory: ");
    print_decimal(active_bytes);
    print(" bytes\n");

    print("Reusable freed memory: ");
    print_decimal(free_tracked_bytes);
    print(" bytes\n");

    print("Total reserved memory: ");
    print_decimal(heap_current - heap_start);
    print(" bytes\n");

    print("Tracked blocks: ");
    print_decimal(block_count);
    print("\n");
}