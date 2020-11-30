#ifndef DRAT_COMMANDS_H
#define DRAT_COMMANDS_H

#include "commands/inspect.h"

#define ARRAY_SIZE(a)   (sizeof(a) / sizeof(*(a)))

typedef int main_function(int argc, const char** argv);

typedef struct {
    const char*     command_name;
    main_function*  command_main;
} drat_command_t;

static drat_command_t commands[] = {
    { "inspect"     , main_inspect  },
};

main_function* get_command_main(char* command_name) {
    if (!command_name) {
        return NULL;
    }

    for (int i = 0; i < ARRAY_SIZE(commands); i++) {
        if (strcmp(commands[i].command_name, command_name) == 0) {
            return &(commands[i].command_main);
        }
    }
}

#endif // DRAT_COMMANDS_H
