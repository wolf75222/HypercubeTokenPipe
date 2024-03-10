#include "ProcessCommunication.h"
#include "Utilities.h"
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

int nb_pipes = 0;
int nb_processes = 0;
int **pipe_fds = NULL;
pid_t *child_pids = NULL;

// Delete files created during previous execution
void delete_previous_files() 
{
    DIR *directory;
    struct dirent *de;

    directory = opendir("./");
    if(directory == NULL)
    {
      fprintf(stderr, "Unable to open current directory\n");
      exit(EXIT_FAILURE);
    }
    while((de = readdir(directory)) != NULL)
    {
        if(hasToBeUnlink(de->d_name))
        {
            unlink(de->d_name);
        }
    }
    closedir(directory);
}

void init_pipes(int n) {
  nb_pipes = (1 << n) * n;
  pipe_fds = (int **) malloc(nb_pipes * sizeof(int *));
  if(pipe_fds == NULL)
  {
    fprintf(stderr, "Pipe allocation failed 1");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < nb_pipes; i++) {
    pipe_fds[i] = (int *) malloc(2 * sizeof(int));
    if(pipe_fds[i] == NULL)
    {
      fprintf(stderr, "Pipe allocation failed 2");
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
    fprintf(stderr, "child_pids allocation failed");
    exit(EXIT_FAILURE);
  }

  for (int id_process = 0; id_process < nb_processes; id_process++) {
    pid_t pid = fork();
    if (pid == -1) {
      perror("Failed to fork");
      exit(EXIT_FAILURE);
    } else if (pid == 0) { // childs
      fd_set readfds;
      int nfds = 0;
      FD_ZERO(&readfds);
      int * pipe_ids_list = (int *) malloc(2*n * sizeof(int));
      if(pipe_ids_list == NULL)
      {
        fprintf(stderr, "pipe_ids_list allocation failed");
        exit(EXIT_FAILURE);
      }
      
      for(int i = 0; i < n; i++) {
        int other_p = id_process ^ (1 << i);
        
        if(id_process < other_p) {
          pipe_ids_list[2*i] = (id_process & other_p) * n + i;
          pipe_ids_list[2*i+1] = (id_process | other_p) * n + i;
          close(pipe_fds[pipe_ids_list[2*i]][1]); // close write
          close(pipe_fds[pipe_ids_list[2*i+1]][0]); // close read
        }
        else {
          pipe_ids_list[2*i] = (id_process | other_p) * n + i;
          pipe_ids_list[2*i+1] = (id_process & other_p) * n + i;
          close(pipe_fds[pipe_ids_list[2*i]][1]);
          close(pipe_fds[pipe_ids_list[2*i+1]][0]);
        }
        FD_SET(pipe_fds[pipe_ids_list[2*i]][0], &readfds);
        nfds = maximum(nfds, pipe_fds[pipe_ids_list[2*i]][0]);
      }
      for (int i = 0; i < nb_pipes; i++) {
        if(!isInTab(i, pipe_ids_list, 2*n)) {
          close(pipe_fds[i][0]);
          close(pipe_fds[i][1]);
        }
      }


      token_journey(id_process, pipe_ids_list, n);


      // close pipes
      for(int i = 0; i < n; i++) {
        int other_p = id_process ^ i;
        if(id_process < other_p) {
          close(pipe_fds[pipe_ids_list[2*i]][0]);
          close(pipe_fds[pipe_ids_list[2*i+1]][1]);
        }
        else {
          close(pipe_fds[pipe_ids_list[2*i]][1]);
          close(pipe_fds[pipe_ids_list[2*i+1]][0]);
        }
      }
      free(pipe_ids_list);
      exit(0);
    } else { // father
      child_pids[id_process] = pid;
      // close all pipes
      for (int i = 0; i < nb_pipes; i++) {
        close(pipe_fds[i][0]);
        close(pipe_fds[i][1]);
      }
    }
  }

  
  for (int i = 0; i < nb_pipes; i++) {
    close(pipe_fds[i][0]);
    close(pipe_fds[i][1]);
  }
}


void token_journey(int id_process, int *pipe_fds, int n) {
    char file_name[256];
    int file;
    
    sprintf(file_name, "%d.txt", id_process);
    file = open(file_name, O_CREAT | O_WRONLY | O_TRUNC, 0666);
    if (file == -1) {
        perror("Failed to open a file");
        exit(EXIT_FAILURE);
    }
    srand(time(NULL) + id_process); 
    if (id_process == 0) {
        int token = 0; 
        int pipe_index = rand() % n; 
        dprintf(file, "Starting token: %d\n", token); 
        
        
        if (write(pipe_fds[2 * pipe_index + 1], &token, sizeof(token)) == -1) {
            perror("Write to pipe failed");
            exit(EXIT_FAILURE);
        }
    } else {
        
        int token;
        if (read(pipe_fds[2 * (id_process % n)], &token, sizeof(token)) == -1) {
            perror("Read from pipe failed");
            exit(EXIT_FAILURE);
        }
        
        dprintf(file, "Received token: %d by process %d\n", token, id_process); 
        
        token++; 
        
        int next_pipe_index = rand() % n;
        dprintf(file, "Passing token: %d to next\n", token); 
        
        
        if (write(pipe_fds[2 * next_pipe_index + 1], &token, sizeof(token)) == -1) {
            perror("Write to pipe failed");
            exit(EXIT_FAILURE);
        }
    }

    close(file); 
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


