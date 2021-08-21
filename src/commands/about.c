#include <stdio.h>

#include "../legal.h"

int cmd_about(int argc, char** argv) {
    if (argc != 1) {
        fprintf(stderr, "The `about` command doesn't take any arguments.\n");
        return 1;
    }

    printf(VERSION_AND_COPYRIGHT_STRING "\n" WARRANTY_AND_LICENSE_STRING);
    
    return 0;
}
