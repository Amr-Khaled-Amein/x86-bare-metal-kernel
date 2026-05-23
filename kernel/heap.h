#ifndef HEAP_H
#define HEAP_H

void heap_init();
void *kmalloc(unsigned int size);
void show_heap_info();
void allocate_demo_block();
void free_latest_block();
void show_heap_blocks();

#endif