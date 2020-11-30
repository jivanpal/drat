#ifndef DRAT_COMMANDS_H
#define DRAT_COMMANDS_H

#include "commands/explore-fs-tree.h"
#include "commands/explore-omap-tree.h"
#include "commands/inspect.h"
#include "commands/list-raw.h"
#include "commands/list.h"
#include "commands/modify.h"
#include "commands/read.h"
#include "commands/recover-raw.h"
#include "commands/recover.h"
#include "commands/resolver.h"
#include "commands/search-last-btree-node.h"
#include "commands/search.h"

#define ARRAY_SIZE(a)   (sizeof(a) / sizeof(*(a)))

typedef int main_function(int argc, const char** argv);

typedef struct {
    const char*     command_name;
    main_function*  command_main;
} drat_command_t;

static drat_command_t commands[] = {
    { "explore-fs-tree"         , main__explore_fs_tree         },
    { "explore-omap-tree"       , main__explore_omap_tree       },
    { "inspect"                 , main__inspect                 },
    { "list-raw"                , main__list_raw                },
    { "list"                    , main__list                    },
    { "modify"                  , main__modify                  },
    { "read"                    , main__read                    },
    { "recover-raw"             , main__recover_raw             },
    { "recover"                 , main__recover                 },
    { "resolver"                , main__resolver                },
    { "search-last-btree-node"  , main__search_last_btree_node  },
    { "search"                  , main__search                  },
};

main_function* get_command_main(char* command_name) {
    if (!command_name) {
        return NULL;
    }

    for (size_t i = 0; i < ARRAY_SIZE(commands); i++) {
        if (strcmp(commands[i].command_name, command_name) == 0) {
            return &(commands[i].command_main);
        }
    }

    return NULL;
}

#endif // DRAT_COMMANDS_H
