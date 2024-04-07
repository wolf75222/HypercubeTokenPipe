#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "ProcessCommunication.h"
#include "SignalHandlers.h"
#include "Utilities.h"



// 0 : no ----- 1 : yes


int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <dimension>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int n = atoi(argv[1]);

    if(n == 0) {
        printf("delete_previous_files...\n");
        delete_previous_files();
        exit(0);
    }
    
    printf("HypercubeProcessSystem: %d\n", getpid());
    printf("Dimension: %d\n", n);
    printf("delete_previous_files...\n");
    delete_previous_files();
    printf("init_pipes...\n");
    init_pipes(n);
    printf("create_hypercube_processes...\n");
    create_hypercube_processes(n);
    
    wait_for_children(n);

    exit(0);
}

