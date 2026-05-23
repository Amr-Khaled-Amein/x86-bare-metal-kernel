#ifndef HEAP_H
#define HEAP_H

void heap_init();

void *kmalloc(unsigned int size);
void show_heap_info();
void allocate_demo_block();
void allocate_custom_block(unsigned int size);
void free_latest_block();
void free_block_by_id(unsigned int id);
void show_heap_blocks();

#endif