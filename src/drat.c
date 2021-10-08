#include <stdio.h>
#include <stdbool.h>
#include <argp.h>

#include "commands.h"
#include "legal.h"

static void print_usage(bool is_error) {
    fprintf(
        is_error ? stderr : stdout,
        
        "Usage: drat <command>\n"
        "\n"
        "List of commands:\n"
    );

    for (size_t i = 0; i < ARRAY_SIZE(drat_commands); i++) {
        fprintf(
            is_error ? stderr : stdout,
            "   %-22s  %s\n",
            drat_commands[i].name,
            drat_commands[i].description
        );
    }
}

int main(int argc, char** argv) {
    char* cmd_name = argv[1];
    if (!cmd_name) {
        printf(VERSION_AND_COPYRIGHT_STRING "\n");
        print_usage(false);
        return 0;
    }

    command_function* cmd = get_command_function(cmd_name);
    if (!cmd) {
        fprintf(stderr, "Unrecognised command `%s`.\n\n", cmd_name);
        print_usage(true);
        return -1;
    }

    return cmd(argc - 1, argv + 1);
}
