#include "SignalHandlers.h"


extern int nb_processes;
extern pid_t *child_pids;
extern int continue_communication;

volatile sig_atomic_t suspend_flag = 0;

void father_handler(int sig) {

  signal(sig, father_handler);
  switch(sig) {

    case SIGUSR1:
      suspend_flag = !suspend_flag;
      if (suspend_flag) {
        // Logique pour suspendre les processus
        for(int i = 0; i < nb_processes; i++)
          kill(child_pids[i], SIGSTOP);
      } else {
        // Logique pour reprendre les processus
        for(int i = 0; i < nb_processes; i++)
          kill(child_pids[i], SIGCONT);
      }
      break;

    case SIGINT:
      for(int i = 0; i < nb_processes; i++)
        kill(child_pids[i], SIGINT);
      break;
  }
}

void child_sigint_handler(int sig) {
  // Logique pour terminer proprement tous les processus
  signal(sig, child_sigint_handler);
  continue_communication = 0;
}


