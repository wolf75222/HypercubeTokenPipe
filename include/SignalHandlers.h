#ifndef SIGNALHANDLERS_H
#define SIGNALHANDLERS_H

#include <signal.h>

void handle_sigusr1(int sig);
void handle_sigint(int sig);
// void init_signal_handlers();

#endif

