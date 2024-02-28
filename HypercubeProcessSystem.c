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
    init_signal_handlers();
    init_pipes(n);
    create_hypercube_processes(n);
    
    wait_for_children(n);

    return 0;
}
