#ifndef PROCESS_H
#define PROCESS_H

void process_init();
void process_run(const char *name);
void process_list();
void process_kill(unsigned int pid);
unsigned int process_schedule(unsigned int current_esp);
void process_start_scheduler();
void process_stop_scheduler();
void process_scheduler_status();
void process_sem_wait(unsigned int pid);
void process_sem_signal();
void process_sem_status();

#endif