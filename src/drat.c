#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#include <apfs/object.h>    // xid_t

#include <drat/commands.h>
#include <drat/strings.h>
#include <drat/arguments.h>

static void print_usage() {
    printf(
        "Usage: %s [global options] <command> [options]\n"
        "\n"
        "List of commands:\n",

        globals.program_name
    );

    for (const drat_command_t* command = commands; command->name; command++) {
        printf("  %-22s  %s\n", command->name, command->description);
    }
}

int main(int argc, char** argv) {
    globals.program_name = strrchr(argv[0], '/');
    if (!globals.program_name) {
        globals.program_name = argv[0];
    } else {
        globals.program_name++;
    }
    
    if (argc == 1) {
        // No command was specified; print usage on stdout
        printf(
            VERSION_STRING "\n"
            COPYRIGHT_STRING "\n"
            URL_STRING "\n"
            "\n"
        );
        print_usage(false);
        return 0;
    }
    
    command_info_t command = {
        false,  // seen
        NULL,   // name
        NULL,   // function
    };
    
    error_t parse_result = argp_parse(&argp_command, argc, argv, ARGP_IN_ORDER, 0, &command);
    if ( parse_result != 0 || (command.seen && !command.name) ) {
        fprintf(stderr, "ERROR: Couldn't parse arguments to determine command.\n");
        return -1;
    }

    if (!command.seen) {
        // Options were specified, but no command. Complain on stderr and exit.
        fprintf(stderr, "%s: no command specified; run `%s` without any arguments to see a list of commands.\n", globals.program_name, globals.program_name);
        return 0;
    }

    command.function = get_command_function(command.name);
    if (!command.function) {
        fprintf(
            stderr,
            "%s: unrecognized command `%s`; run `%s` without any arguments to see a list of commands.\n",
            globals.program_name,
            command.name,
            globals.program_name
        );
        return -1;
    }

    return command.function(argc, argv);
}
