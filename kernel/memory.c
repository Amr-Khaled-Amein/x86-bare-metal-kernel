#include "memory.h"
#include "vga.h"

void show_memory_info() {
    print("Memory information:\n");
    print("Kernel load address: 1 MB\n");
    print("VGA text memory: 0xB8000\n");
    print("Architecture: x86 32-bit\n");
    print("Memory model: Freestanding kernel space\n");
    print("Paging: Not enabled yet\n");
    print("Heap allocator: Not implemented yet\n");
}