#ifndef DRAT_COMMANDS_H
#define DRAT_COMMANDS_H

#include <stddef.h>
#include <string.h>

#define ARRAY_SIZE(a)   (sizeof(a) / sizeof(*(a)))

typedef int command_function(int argc, char** argv);

typedef struct {
    const char*         name;
    command_function*   function;
} drat_command_t;

/**
 * Function prototypes; function implementations are
 * contained within the respective command's source file.
 */
command_function cmd_explore_fs_tree;
command_function cmd_explore_omap_tree;
command_function cmd_inspect;
command_function cmd_list_raw;
command_function cmd_list;
command_function cmd_modify;
command_function cmd_read;
command_function cmd_recover_raw;
command_function cmd_recover;
command_function cmd_resolver;
command_function cmd_search_last_btree_node;
command_function cmd_search;

static drat_command_t commands[] = {
    { "explore-fs-tree"         , cmd_explore_fs_tree         },
    { "explore-omap-tree"       , cmd_explore_omap_tree       },
    { "inspect"                 , cmd_inspect                 },
    { "list-raw"                , cmd_list_raw                },
    { "list"                    , cmd_list                    },
    { "modify"                  , cmd_modify                  },
    { "read"                    , cmd_read                    },
    { "recover-raw"             , cmd_recover_raw             },
    { "recover"                 , cmd_recover                 },
    { "resolver"                , cmd_resolver                },
    { "search-last-btree-node"  , cmd_search_last_btree_node  },
    { "search"                  , cmd_search                  },
};

/**
 * Given the name of a command `command_name`, return a pointer to the
 * corresponding function `command_main` for that command.
 * 
 * RETURN VALUE:
 *      Return pointer to function `command_main`, where `command_main` has the
 *      type signature `int command_main(int argc, char** argv)`.
 *      If `command_name` is `NULL` or an unrecognised command, return `NULL`.
 */
command_function* get_command_function(char* command_name) {
    if (!command_name) {
        return NULL;
    }

    for (size_t i = 0; i < ARRAY_SIZE(commands); i++) {
        if (strcmp(commands[i].name, command_name) == 0) {
            return commands[i].function;
        }
    }

    return NULL;
}

#endif // DRAT_COMMANDS_H
