#ifndef DRAT_COMMANDS_H
#define DRAT_COMMANDS_H

#include <stddef.h>
#include <string.h>

#include <drat/asize.h>

typedef int command_function(int argc, char** argv);

typedef struct {
    const char*         name;
    command_function*   function;
    const char*         description;
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
command_function cmd_timeline;
command_function cmd_modify;
command_function cmd_read;
command_function cmd_recover_raw;
command_function cmd_recover;
command_function cmd_resolver;
command_function cmd_search_last_btree_node;
command_function cmd_search;
command_function cmd_version;

static drat_command_t drat_commands[] = {
    { "explore-fs-tree"         , cmd_explore_fs_tree           , "Explore filesystem B-tree" },
    { "explore-omap-tree"       , cmd_explore_omap_tree         , "Explore object map B-tree" },
    { "inspect"                 , cmd_inspect                   , "Inspect APFS partition" },
    { "list-raw"                , cmd_list_raw                  , "List directory contents or file info based on its filesystem OID" },
    { "list"                    , cmd_list                      , "List directory contents or file info based on its filepath" },
    { "timeline"                , cmd_timeline                  , "Parse the whole filesystem recursively and build a MACB timeline" },
    // { "modify"                  , cmd_modify                    , "Modify structures on disk to resolve problems" },
    { "read"                    , cmd_read                      , "Read a block and display information about it" },
    { "recover-raw"             , cmd_recover_raw               , "Recover a file based on its filesystem OID" },
    { "recover"                 , cmd_recover                   , "Recover a file based on its filepath" },
    { "resolver"                , cmd_resolver                  , "Check if given Virtual OIDs resolve to given Physical OIDs" },
    { "search-last-btree-node"  , cmd_search_last_btree_node    , "Search the partition for B-tree nodes, reporting the Physical OID of the last one discovered" },
    { "search"                  , cmd_search                    , "Search the partition for blocks with certain features/properties" },
    { "version"                 , cmd_version                   , "Display Drat's version number along with legal info (copyright, warranty, and license)" },
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

    for (size_t i = 0; i < ARRAY_SIZE(drat_commands); i++) {
        if (strcmp(drat_commands[i].name, command_name) == 0) {
            return drat_commands[i].function;
        }
    }

    return NULL;
}

#endif // DRAT_COMMANDS_H
