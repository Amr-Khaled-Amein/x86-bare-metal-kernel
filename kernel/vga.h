#ifndef VGA_H
#define VGA_H

void clear_screen();
void print(const char *message);
void print_colored(const char *message, unsigned char color);
void print_char(char c);
void print_char_colored(char c, unsigned char color);
void print_newline();
void vga_backspace();

#endif