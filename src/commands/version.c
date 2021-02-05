#include <stdio.h>

int cmd_version(int argc, char** argv) {
    if (argc != 1) {
        fprintf(stderr, "The `version` command doesn't take any arguments.\n");
        return 1;
    }

    printf("Drat version 0.1.2\n");
    
    return 0;
}
