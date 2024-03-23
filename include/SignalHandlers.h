#ifndef SIGNALHANDLERS_H
#define SIGNALHANDLERS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>


void father_handler(int sig);
void child_sigint_handler(int sig);

#endif

