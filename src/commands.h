#ifndef DRAT_COMMANDS_H
#define DRAT_COMMANDS_H

#define ARRAY_SIZE(a)   (sizeof(a) / sizeof(*(a)))

typedef int main_function(int argc, const char** argv);

typedef struct {
    const char*     command_name;
    main_function*  command_main;
} drat_command_t;

/**
 * Function prototypes; functions implementations will be
 * linked into main program via compiled object files.
 */
main_function main__explore_fs_tree;
main_function main__explore_omap_tree;
main_function main__inspect;
main_function main__list_raw;
main_function main__list;
main_function main__modify;
main_function main__read;
main_function main__recover_raw;
main_function main__recover;
main_function main__resolver;
main_function main__search_last_btree_node;
main_function main__search;

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
