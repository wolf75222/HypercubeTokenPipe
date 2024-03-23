#ifndef PROCESSCOMMUNICATION_H
#define PROCESSCOMMUNICATION_H

#define MAX_PROCESSES 1024

#include "Utilities.h"
#include "SignalHandlers.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <dirent.h>
#include <fcntl.h>

void delete_previous_files();

void init_pipes(int n);
void create_hypercube_processes(int n);
void token_journey(int id_process, int *pipe_ids_list, int n);
int set_readfds(int n, fd_set *readfds);
void wait_for_children(int n);

void free_pipes();
void free_child_pids();
void free_pipe_ids_list();
#endif
