#include "ProcessCommunication.h"
#include "Utilities.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int pipe_fds[MAX_PROCESSES][2];
pid_t child_pids[MAX_PROCESSES];

void init_pipes(int n) {
  int nb_pipes = (1 << n) * n;
  for (int i = 0; i < nb_pipes; i++) {
    if (pipe(pipe_fds[i]) == -1) {
      perror("Pipe initialization failed");
      exit(EXIT_FAILURE);
    }
  }
}

void create_hypercube_processes(int n) {
  int nb_processes = 1 << n;
  for (int i = 0; i < nb_processes; i++) {
    pid_t pid = fork();
    if (pid == -1) {
      perror("Failed to fork");
      exit(EXIT_FAILURE);
    } else if (pid == 0) {
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
    } else {
      child_pids[i] = pid;
    }
  }
  for (int i = 0; i < nb_processes; i++) {
    close(pipe_fds[i][0]);
    close(pipe_fds[i][1]);
  }
}

void wait_for_children(int n) {
  int nb_processes = 1 << n;
  for (int i = 0; i < nb_processes; i++) {
    int status;
    waitpid(child_pids[i], &status, 0);
  }
}
