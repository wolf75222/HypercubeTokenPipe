#ifndef PROCESSCOMMUNICATION_H
#define PROCESSCOMMUNICATION_H

#define MAX_PROCESSES 1024

void init_pipes(int n);
void create_hypercube_processes(int n);
void wait_for_children(int n);

void free_pipes();
void free_child_pids();
#endif
