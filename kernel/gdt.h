#ifndef GDT_H
#define GDT_H

void gdt_init();
void tss_set_kernel_stack(unsigned int stack);

#endif