#include "ProcessCommunication.h"
#include "SignalHandlers.h"
#include "Utilities.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <dimension>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int n = atoi(argv[1]);

    int atex = atexit(free_pipes);
    if(atex != 0)
    {
        fprintf(stderr, "atexit() error 1\n");
        exit(EXIT_FAILURE);
    }
    atex = atexit(free_child_pids);
    if(atex != 0)
    {
        fprintf(stderr, "atexit() error 2\n");
        exit(EXIT_FAILURE);
    }
    
    printf("HypercubeProcessSystem: %d\n", getpid());
    printf("Dimension: %d\n", n);
    printf("Initializing...\n");
    init_signal_handlers();
    printf("delete_previous_files...\n");
    delete_previous_files();
    printf("init_pipes...\n");
    init_pipes(n);
    printf("create_hypercube_processes...\n");
    create_hypercube_processes(n);
    
    wait_for_children(n);

    return 0;
}

