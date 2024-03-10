#include "SignalHandlers.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

volatile sig_atomic_t suspend_flag = 0;

void handle_sigusr1(int sig) {
  suspend_flag = !suspend_flag;
  if (suspend_flag) {
    // Logique pour suspendre les processus
  } else {
    // Logique pour reprendre les processus
  }
}

void handle_sigint(int sig) {
  // Logique pour terminer proprement tous les processus
  exit(0);
}

void init_signal_handlers() {
  struct sigaction sa_usr1, sa_int;
  sa_usr1.sa_handler = handle_sigusr1;
  sigemptyset(&sa_usr1.sa_mask);
  sa_usr1.sa_flags = 0;
  if (sigaction(SIGUSR1, &sa_usr1, NULL) == -1) {
    perror("Error setting SIGUSR1 handler");
    exit(EXIT_FAILURE);
  }

  sa_int.sa_handler = handle_sigint;
  sigemptyset(&sa_int.sa_mask);
  sa_int.sa_flags = 0;
  if (sigaction(SIGINT, &sa_int, NULL) == -1) {
    perror("Error setting SIGINT handler");
    exit(EXIT_FAILURE);
  }
}
