#ifndef TEXT_BUFFER_H
#define TEXT_BUFFER_H

void text_buffer_start_edit();
void text_buffer_handle_input(const char *input);
void text_buffer_show();
void text_buffer_clear();
void text_buffer_info();
int text_buffer_is_editing();

#endif