#include "ProcessCommunication.h"


#define ENABLE_DISPLAY_DESCRIPTION 0
#define ENABLE_DISPLAY_TOKEN_JOURNEY 0


int nb_pipes = 0;
int nb_processes = 0;
int **pipe_fds = NULL;
pid_t *child_pids = NULL;
int * pipe_ids_list = NULL;
int continue_communication = 1;



void init_pipes(int n) {
  nb_pipes = (1 << n) * n;
  pipe_fds = (int **) calloc(nb_pipes, sizeof(int *));
  if(pipe_fds == NULL)
  {
    fprintf(stderr, "Pipe allocation failed 1");
    exit(EXIT_FAILURE);
  }
  if(atexit(free_pipes) != 0)
  {
    fprintf(stderr, "atexit() error for free_pipes\n");
    exit(EXIT_FAILURE);
  }

  if(ENABLE_DISPLAY_DESCRIPTION)
    printf("Tubes :");

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

    if(ENABLE_DISPLAY_DESCRIPTION)
      printf(" [%d-%d]", pipe_fds[i][0], pipe_fds[i][1]);
  }
  if(ENABLE_DISPLAY_DESCRIPTION)
    printf("\n");
}

void create_hypercube_processes(int n) {
  nb_processes = 1 << n;

  child_pids = (pid_t *) malloc(nb_processes * sizeof(pid_t));
  if(child_pids == NULL)
  {
    fprintf(stderr, "child_pids allocation failed");
    exit(EXIT_FAILURE);
  }
  if(atexit(free_child_pids) != 0)
  {
    fprintf(stderr, "atexit() error for free_child_pids\n");
    exit(EXIT_FAILURE);
  }

  for (int id_process = 0; id_process < nb_processes; id_process++) {
    pid_t pid = fork();
    if (pid == -1) {
      perror("Failed to fork");
      exit(EXIT_FAILURE);
    } else if (pid == 0) { // childs

      signal(SIGINT, child_sigint_handler);
      
      pipe_ids_list = (int *) malloc(2*n * sizeof(int));
      if(pipe_ids_list == NULL)
      {
        fprintf(stderr, "pipe_ids_list allocation failed");
        exit(EXIT_FAILURE);
      }
      if(atexit(free_pipe_ids_list) != 0)
      {
        fprintf(stderr, "atexit() error for free_pipe_ids_list\n");
        exit(EXIT_FAILURE);
      }
      
      for(int i = 0; i < n; i++) {
        int other_p = id_process ^ (1 << i);

        if(ENABLE_DISPLAY_DESCRIPTION)
          printf("id : %d <--> other id : %d\t\t", id_process, other_p);
  
        pipe_ids_list[2*i] = pipe_fds[id_process * n + i][0];
        pipe_ids_list[2*i+1] = pipe_fds[other_p * n + i][1];
        close(pipe_fds[id_process * n + i][1]);
        close(pipe_fds[other_p * n + i][0]);
        if(ENABLE_DISPLAY_DESCRIPTION)
          printf("id %d : \t read in: %d\t write in: %d\n", id_process, pipe_ids_list[2*i], pipe_ids_list[2*i+1]);
      }

      for (int i = 0; i < nb_pipes; i++) {
        if(!isInTab(pipe_fds[i][0], pipe_ids_list, 2*n) && !isInTab(pipe_fds[i][1], pipe_ids_list, 2*n)) {
          close(pipe_fds[i][0]);
          close(pipe_fds[i][1]);
        }
      }


      token_journey(id_process, pipe_ids_list, n);


      // close pipes
      for(int i = 0; i < 2*n; i++) {
        close(pipe_ids_list[i]);
      }
      exit(0);

    } else { // father
      child_pids[id_process] = pid;
    }
  }

  // gestion des signaux recu par le pere
  signal(SIGUSR1, father_handler);
  signal(SIGINT, father_handler);

  // close all pipes
  for (int i = 0; i < nb_pipes; i++) {
    close(pipe_fds[i][0]);
    close(pipe_fds[i][1]);
  }
}


void token_journey(int id_process, int *pipe_ids_list, int n) {
    char file_name[256];
    int file;
    fd_set readfds;
    int position_bit_dif;
    struct timeval timeBefore, timeNow;
    int token = 0;
    timeBefore.tv_sec = 0;
    timeBefore.tv_usec = 0;
    

    sprintf(file_name, "%d.txt", id_process);
    file = open(file_name, O_CREAT | O_WRONLY | O_TRUNC, 0666);
    if (file == -1) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }
    
    srand(time(NULL) + id_process);

    sleep(1);
    
    if (id_process == 0) {
        gettimeofday(&timeBefore, NULL);
        token++;
        position_bit_dif = rand() % n;
        dprintf(file, "Starting token: %d\n", token);

        if(ENABLE_DISPLAY_TOKEN_JOURNEY) {
          printf("\nEnvoyé par : %d", id_process);
          printf("\nToken = %d\n", token);
          printf("rand : %d\t dest id : %d\t tube desc : %d\n", position_bit_dif, id_process ^ (1 << position_bit_dif), pipe_ids_list[2*position_bit_dif+1]);
        }
        
        if (write(pipe_ids_list[2*position_bit_dif+1], &token, sizeof(token)) == -1) {
            perror("Write to pipe failed");
            exit(EXIT_FAILURE);
        }
    }

    suseconds_t msec = 0;
      
    int nfds = set_readfds(n, &readfds);
      
    while(select(nfds+1, &readfds, NULL, NULL, NULL) > 0 && continue_communication) {

      sleep(1);

      if(ENABLE_DISPLAY_TOKEN_JOURNEY)
        printf("\nRecu par : %d", id_process);

      for(int i = 0; i < n; i++)
      {
        if(FD_ISSET(pipe_ids_list[2*i], &readfds))
        {
          if(read(pipe_ids_list[2*i], &token, sizeof(token)) != sizeof(token))
          {
            perror("Read from pipe failed");
            exit(EXIT_FAILURE);
          }
        }
      }
      token++;

      if(ENABLE_DISPLAY_TOKEN_JOURNEY)
        printf("\nToken = %d\n", token);
      
      if(timeBefore.tv_sec == 0) // first reception of the token
      {
        gettimeofday(&timeBefore, NULL);
        dprintf(file, "First reception -> Token: %d\n", token);
      }
      else {
        gettimeofday(&timeNow, NULL);
        msec = (timeNow.tv_sec - timeBefore.tv_sec)*1000000 + timeNow.tv_usec - timeBefore.tv_usec;
        dprintf(file, "Token: %d, Time between 2 receptions of the token : %ld\n", token, msec);
        timeBefore = timeNow;
      }

      position_bit_dif = rand() % n;
      if(ENABLE_DISPLAY_TOKEN_JOURNEY)
        printf("rand : %d\t dest id : %d\t tube desc : %d\n", position_bit_dif, id_process ^ (1 << position_bit_dif), pipe_ids_list[2*position_bit_dif+1]);
        
      if (write(pipe_ids_list[2*position_bit_dif+1], &token, sizeof(int)) == -1) {
        printf("Error processus %d\n", id_process);
        perror("Write to pipe failed");
        exit(EXIT_FAILURE);
      }
      msec = 0;
      
      nfds = set_readfds(n, &readfds);
        
    }

    close(file);
}


int set_readfds(int n, fd_set *readfds) {
  int nfds = 0;
  FD_ZERO(readfds);

  for(int i = 0; i < n; i++)
  {
    FD_SET(pipe_ids_list[2*i], readfds);
    nfds = nfds > pipe_ids_list[2*i] ? nfds : pipe_ids_list[2*i];
  }
  return nfds;
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

void free_pipe_ids_list()
{
  free(pipe_ids_list);
}

