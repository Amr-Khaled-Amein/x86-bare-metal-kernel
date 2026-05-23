#ifndef PROCESS_H
#define PROCESS_H

void process_init();
void process_run(const char *name);
void process_list();
void process_kill(unsigned int pid);
void process_tick();

void process_start_scheduler();
void process_stop_scheduler();
void process_scheduler_status();

#endif