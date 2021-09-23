#ifndef DRAT_COMMANDS_H
#define DRAT_COMMANDS_H

#include <stddef.h>
#include <string.h>

#include <drat/asize.h>

typedef int command_function(int argc, char** argv);

/**
 * Function prototypes; function implementations are
 * contained within the respective command's source file.
 */
command_function cmd_explore_fs_tree;
command_function cmd_explore_omap_tree;
command_function cmd_inspect;
command_function cmd_list;
command_function cmd_modify;
command_function cmd_read;
command_function cmd_recover_raw;
command_function cmd_recover;
command_function cmd_resolver;
command_function cmd_search_last_btree_node;
command_function cmd_search;
command_function cmd_version;

typedef struct {
    const char*         name;
    command_function*   function;
    const char*         description;
} drat_command_t;

extern const drat_command_t commands[];

command_function* get_command_function(char* command_name);

#endif // DRAT_COMMANDS_H
