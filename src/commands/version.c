#include <stdio.h>
#include <sysexits.h>

#include <drat/globals.h>
#include <drat/strings.h>

int cmd_version(int argc, char** argv) {
    if (argc > 2) {
        fprintf(stderr, "The `%s` command doesn't take any arguments.\n", globals.command_name);
        return EX_USAGE;
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
