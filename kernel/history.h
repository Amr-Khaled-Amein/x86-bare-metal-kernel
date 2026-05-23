#ifndef HISTORY_H
#define HISTORY_H

void history_add(const char *command);
void history_show();
int history_get_previous(char *buffer, int buffer_size);
int history_get_next(char *buffer, int buffer_size);
void history_reset_navigation();

#endif