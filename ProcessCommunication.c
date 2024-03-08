#include "ProcessCommunication.h"
#include "Utilities.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int nb_pipes = 0;
int nb_processes = 0;
int **pipe_fds = NULL;
pid_t *child_pids = NULL;

void init_pipes(int n) {
  nb_pipes = (1 << n) * n;
  pipe_fds = (int **) malloc(nb_pipes * sizeof(int *));
  if(pipe_fds == NULL)
  {
    perror("Pipe allocation failed 1");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < nb_pipes; i++) {
    pipe_fds[i] = (int *) malloc(2 * sizeof(int));
    if(pipe_fds[i] == NULL)
    {
      perror("Pipe allocation failed 2");
      exit(EXIT_FAILURE);
    }
    if (pipe(pipe_fds[i]) == -1) {
      perror("Pipe initialization failed");
      exit(EXIT_FAILURE);
    }
  }
}

void create_hypercube_processes(int n) {
  nb_processes = 1 << n;

  child_pids = (pid_t *) malloc(nb_processes * sizeof(pid_t));
  if(child_pids == NULL)
  {
    perror("child_pids allocation failed 2");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < nb_processes; i++) {
    pid_t pid = fork();
    if (pid == -1) {
      perror("Failed to fork");
      exit(EXIT_FAILURE);
    } else if (pid == 0) { // childs
      for (int j = 0; j < nb_processes; j++) {
        for (int k = 0; k < 2; k++) {
          if (j != i) {
            close(pipe_fds[j][k]);
          }
        }
      }
      int bitDiffPos;
      for (int j = 0; j < nb_processes; j++) {
        if (difference1bit(i, j, &bitDiffPos)) {
          if (i < j) {
            close(pipe_fds[j][0]);
          } else {
            close(pipe_fds[j][1]);
          }
        } else {
          close(pipe_fds[j][0]);
          close(pipe_fds[j][1]);
        }
      }
      exit(0);
    } else { // father
      child_pids[i] = pid;
    }
  }

  
  for (int i = 0; i < nb_pipes; i++) {
    close(pipe_fds[i][0]);
    close(pipe_fds[i][1]);
  }
}

void wait_for_children(int n) {
  for (int i = 0; i < nb_processes; i++) {
    int status;
    waitpid(child_pids[i], &status, 0);
  }
}

void free_pipes()
{
  for(int i = 0; i < nb_pipes; i++)
  {
    free(pipe_fds[i]);
    pipe_fds[i] = NULL;
  }
  free(pipe_fds);
  pipe_fds = NULL;
}

void free_child_pids()
{
  free(child_pids);
}


