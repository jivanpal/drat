#include <stdio.h>

#include <drat/strings.h>

int cmd_version(int argc, char** argv) {
    if (argc != 1) {
        fprintf(stderr, "The `about` command doesn't take any arguments.\n");
        return 1;
    }

    printf(
        VERSION_STRING "\n"
        COPYRIGHT_STRING "\n"
        URL_STRING "\n"
        "\n"
        WARRANTY_AND_LICENSE_STRING
    );
    
    return 0;
}
