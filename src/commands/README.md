This directory contains one `.c` source file for each command which Drat
supports. To add support for new commands, we use a system similar to that of
`git`, where new commands are declared in `src/commands.h` once they are
implemented. To add a new command `my-command`, do the following:

1. Create a file `src/commands/my-command.c` where your command will be defined.
    It should contain at least the following two functions:

    1. `cmd_my_command()`, whose type signature is the same as that of a C
        `main()` function, e.g.

        ```c
        int cmd_my_command(int argc, char** argv) {
            // function body
        }
        ```

        This function will be passed `argv` without the name of the executable, and
        `argc` equal to the size of that array, e.g. if the user runs
        `drat my-command arg1 arg2`, then in the scope of `cmd_my_command()`:

        - `argv` is equal to the array `{"my-command", "arg1", "arg2"}`; and
        - `argc` is equal to `3`.

    2. `static void print_usage(int argc, char** argv)`, which will output a
        description of the command's usage. It should be passed the same `argc`
        and `argv` which are passwd to `cmd_my_command()`. If no arguments are
        supplied to the command, the usage message should be output on `stdout`,
        else it should be output on `stderr` along with any info about what the
        user did wrong / why the arguments are invalid.
        
        See other commands' source files for examples. 

2. Add a corresponding function prototype and command entry to `src/commands.h`,
    keeping the lists in alphabetical order, e.g.

    ```c
    /**
     * Function prototypes; function implementations are
     * contained within the respective command's source file.
     */
    // ...
    command_function cmd_my_command();
    // ...

    static drat_command_t commands[] = {
        // ...
        { "my-command"      , cmd_my_command    },
        // ...
    };
    ```

3. Add a short description of your command's purpose to the list in
    `print_usage()` in `src/drat.c`. This will be output in the list of commands
    shown to the user when they run Drat without specifying a command. Please
    keep the list in alphabetical order.
