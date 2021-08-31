#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <sysexits.h>

#include <apfs/object.h>    // xid_t

#include <drat/commands.h>
#include <drat/globals.h>
#include <drat/argp.h>
#include <drat/strings.h>

static void print_usage(FILE* stream) {
    fprintf(
        stream,
        "Usage: %s [global options] <command> [options]\n\nList of commands:\n",
        globals.program_name
    );

    for (const drat_command_t* command = commands; command->name; command++) {
        fprintf(stream, "  %-22s  %s\n", command->name, command->description);
    }
}

int main(int argc, char** argv) {
    setbuf(stdout, NULL);
    
    globals.program_name = strrchr(argv[0], '/');
    if (!globals.program_name) {
        globals.program_name = argv[0];
    } else {
        globals.program_name++;
    }
    
    // For the unlikely event that `print_arg_parse_error()` gets
    // called before we invoke a command.
    globals.command_name = globals.program_name;
    
    if (argc == 1) {
        // No arguments were specified
        printf("\nDrat is a diagnostic and data recovery tool for APFS (Apple File System) partitions.\n\n");
        print_usage(stdout);
        return 0;
    }
    
    if (argp_parse(&argp_command, argc, argv, ARGP_IN_ORDER, 0, 0)) {
        print_arg_parse_error();
        return -1;
    }

    if (!globals.command_name) {
        // Options were specified, but no command
        fprintf(
            stderr,
            "%s: no command specified; run `%s` without any arguments to see a list of commands.\n",
            globals.program_name,
            globals.program_name
        );
        return EX_USAGE;
    }

    command_function* command = get_command_function(globals.command_name);
    if (!command) {
        // Specified command is inavlid
        fprintf(
            stderr,
            "%s: unrecognized command `%s`; run `%s` without any arguments to see a list of commands.\n",
            globals.program_name,
            globals.command_name,
            globals.program_name
        );
        return EX_USAGE;
    }

    return command(argc, argv);
}
