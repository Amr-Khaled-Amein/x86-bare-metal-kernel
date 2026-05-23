#ifndef MINIFS_H
#define MINIFS_H

void minifs_init();

void fs_list_files();
void fs_create_file(const char *name);
void fs_write_file(const char *name, const char *content);
void fs_read_file(const char *name);
void fs_delete_file(const char *name);
void fs_info();

#endif