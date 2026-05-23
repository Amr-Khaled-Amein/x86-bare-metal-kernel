ASM = nasm
CC = gcc
LD = ld

CFLAGS = -m32 -ffreestanding -fno-pie -fno-stack-protector -nostdlib -nostdinc -Ikernel
LDFLAGS = -m elf_i386 -T linker.ld

all: build/kernel.bin

build/boot.o: boot/boot.asm
	$(ASM) -f elf32 boot/boot.asm -o build/boot.o

build/interrupt_stubs.o: kernel/interrupt_stubs.asm
	$(ASM) -f elf32 kernel/interrupt_stubs.asm -o build/interrupt_stubs.o

build/kernel.o: kernel/kernel.c
	$(CC) $(CFLAGS) -c kernel/kernel.c -o build/kernel.o

build/vga.o: kernel/vga.c
	$(CC) $(CFLAGS) -c kernel/vga.c -o build/vga.o

build/ports.o: kernel/ports.c
	$(CC) $(CFLAGS) -c kernel/ports.c -o build/ports.o

build/keyboard.o: kernel/keyboard.c
	$(CC) $(CFLAGS) -c kernel/keyboard.c -o build/keyboard.o

build/shell.o: kernel/shell.c
	$(CC) $(CFLAGS) -c kernel/shell.c -o build/shell.o

build/memory.o: kernel/memory.c
	$(CC) $(CFLAGS) -c kernel/memory.c -o build/memory.o

build/os_concepts.o: kernel/os_concepts.c
	$(CC) $(CFLAGS) -c kernel/os_concepts.c -o build/os_concepts.o

build/heap.o: kernel/heap.c
	$(CC) $(CFLAGS) -c kernel/heap.c -o build/heap.o

build/idt.o: kernel/idt.c
	$(CC) $(CFLAGS) -c kernel/idt.c -o build/idt.o

build/text_buffer.o: kernel/text_buffer.c
	$(CC) $(CFLAGS) -c kernel/text_buffer.c -o build/text_buffer.o

build/history.o: kernel/history.c
	$(CC) $(CFLAGS) -c kernel/history.c -o build/history.o

build/minifs.o: kernel/minifs.c
	$(CC) $(CFLAGS) -c kernel/minifs.c -o build/minifs.o

build/process.o: kernel/process.c
	$(CC) $(CFLAGS) -c kernel/process.c -o build/process.o

build/kernel.bin: build/boot.o build/interrupt_stubs.o build/kernel.o build/vga.o build/ports.o build/keyboard.o build/shell.o build/memory.o build/os_concepts.o build/heap.o build/idt.o build/text_buffer.o build/history.o build/minifs.o build/process.o
	$(LD) $(LDFLAGS) build/boot.o build/interrupt_stubs.o build/kernel.o build/vga.o build/ports.o build/keyboard.o build/shell.o build/memory.o build/os_concepts.o build/heap.o build/idt.o build/text_buffer.o build/history.o build/minifs.o build/process.o -o build/kernel.bin

run: all
	qemu-system-i386 -kernel build/kernel.bin

clean:
	rm -f build/*.o build/*.bin