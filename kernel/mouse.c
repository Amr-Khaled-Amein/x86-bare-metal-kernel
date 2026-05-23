#include "mouse.h"
#include "ports.h"
#include "vga.h"

#define MOUSE_DATA_PORT   0x60
#define MOUSE_STATUS_PORT 0x64
#define PIC1_COMMAND     0x20
#define PIC2_COMMAND     0xA0

static unsigned char mouse_packet[4];
static int packet_index = 0;
static int packet_size = 3;

static volatile int scroll_up_pending = 0;
static volatile int scroll_down_pending = 0;

void mouse_wait_write() {
    while (inb(MOUSE_STATUS_PORT) & 0x02) {
    }
}

void mouse_wait_read() {
    while (!(inb(MOUSE_STATUS_PORT) & 0x01)) {
    }
}

void mouse_write(unsigned char value) {
    mouse_wait_write();
    outb(MOUSE_STATUS_PORT, 0xD4);

    mouse_wait_write();
    outb(MOUSE_DATA_PORT, value);
}

unsigned char mouse_read() {
    mouse_wait_read();
    return inb(MOUSE_DATA_PORT);
}

void mouse_command(unsigned char command) {
    mouse_write(command);
    mouse_read(); // ACK
}

void mouse_command_with_data(unsigned char command, unsigned char data) {
    mouse_write(command);
    mouse_read(); // ACK

    mouse_write(data);
    mouse_read(); // ACK
}

void mouse_init() {
    // Enable auxiliary mouse device
    mouse_wait_write();
    outb(MOUSE_STATUS_PORT, 0xA8);

    // Enable mouse IRQ12 in PS/2 controller config
    mouse_wait_write();
    outb(MOUSE_STATUS_PORT, 0x20);

    unsigned char status = mouse_read();
    status = status | 0x02;
    status = status & ~0x20;

    mouse_wait_write();
    outb(MOUSE_STATUS_PORT, 0x60);

    mouse_wait_write();
    outb(MOUSE_DATA_PORT, status);

    // Try to enable mouse wheel mode: 200, 100, 80 sample-rate sequence
    mouse_command_with_data(0xF3, 200);
    mouse_command_with_data(0xF3, 100);
    mouse_command_with_data(0xF3, 80);

    // Get mouse ID
    mouse_write(0xF2);
    mouse_read(); // ACK

    unsigned char mouse_id = mouse_read();

    if (mouse_id == 3) {
        packet_size = 4; // IntelliMouse wheel packet
    } else {
        packet_size = 3;
    }

    // Enable packet streaming
    mouse_command(0xF4);
}

void mouse_interrupt_handler() {
    unsigned char status = inb(MOUSE_STATUS_PORT);

    if (!(status & 0x01)) {
        outb(PIC2_COMMAND, 0x20);
        outb(PIC1_COMMAND, 0x20);
        return;
    }

    // If bit 5 is NOT set, this is keyboard data, not mouse data
    if (!(status & 0x20)) {
        outb(PIC2_COMMAND, 0x20);
        outb(PIC1_COMMAND, 0x20);
        return;
    }

    unsigned char data = inb(MOUSE_DATA_PORT);

    if (packet_index == 0 && !(data & 0x08)) {
        outb(PIC2_COMMAND, 0x20);
        outb(PIC1_COMMAND, 0x20);
        return;
    }

    mouse_packet[packet_index] = data;
    packet_index++;

    if (packet_index >= packet_size) {
        packet_index = 0;

        if (packet_size == 4) {
            signed char wheel = (signed char) mouse_packet[3];

            if (wheel > 0) {
                scroll_up_pending += 3;
            } else if (wheel < 0) {
                scroll_down_pending += 3;
            }
        }
    }

    outb(PIC2_COMMAND, 0x20);
    outb(PIC1_COMMAND, 0x20);
}

void mouse_process_scroll() {
    while (scroll_up_pending > 0) {
        vga_scroll_up();
        scroll_up_pending--;
    }

    while (scroll_down_pending > 0) {
        vga_scroll_down();
        scroll_down_pending--;
    }
}