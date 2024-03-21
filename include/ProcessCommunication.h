#ifndef PROCESSCOMMUNICATION_H
#define PROCESSCOMMUNICATION_H

#define MAX_PROCESSES 1024

void delete_previous_files();

void init_pipes(int n);
void create_hypercube_processes(int n);
void token_journey(int id_process, int *pipe_ids_list, int n);
void wait_for_children(int n);

void free_pipes();
void free_child_pids();
void free_pipe_ids_list();
#endif
