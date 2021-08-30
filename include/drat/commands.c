#include "commands.h"

const drat_command_t commands[] = {
    { "explore-fs-tree"         , cmd_explore_fs_tree           , "Explore filesystem B-tree" },
    { "explore-omap-tree"       , cmd_explore_omap_tree         , "Explore object map B-tree" },
    { "inspect"                 , cmd_inspect                   , "Inspect APFS partition" },
    { "list-raw"                , cmd_list_raw                  , "List directory contents or file info based on its filesystem OID" },
    { "list"                    , cmd_list                      , "List directory contents or file info based on its filepath" },
    // { "modify"                  , cmd_modify                    , "Modify structures on disk to resolve problems" },
    { "read"                    , cmd_read                      , "Read a block and display information about it" },
    { "recover-raw"             , cmd_recover_raw               , "Recover a file based on its filesystem OID" },
    { "recover"                 , cmd_recover                   , "Recover a file based on its filepath" },
    { "resolver"                , cmd_resolver                  , "Check if given Virtual OIDs resolve to given Physical OIDs" },
    { "search-last-btree-node"  , cmd_search_last_btree_node    , "Search the partition for B-tree nodes, reporting the Physical OID of the last one discovered" },
    { "search"                  , cmd_search                    , "Search the partition for blocks with certain features/properties" },
    { "version"                 , cmd_version                   , "Display Drat's version number along with legal info (copyright, warranty, and license)" },
    {0}
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

    for (const drat_command_t* command = commands; command->name; command++) {
        if (strcmp(command->name, command_name) == 0) {
            return command->function;
        }
    }

    return NULL;
}
