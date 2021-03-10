#include <stdio.h>
#include <stdbool.h>
#include <argp.h>

#include "commands.h"

static void print_usage(bool is_error) {
    fprintf(
        is_error ? stderr : stdout,

        "Usage: drat <command>\n"
        "\n"
        "List of commands:\n"
        "   explore-fs-tree         Explore filesystem B-tree\n"
        "   explore-omap-tree       Explore object map B-tree\n"
        "   inspect                 Inspect APFS partition\n"
        "   list-raw                List directory contents or file info based on its filesystem OID\n"
        "   list                    List directory contents or file info based on its filepath\n"
        // "   modify                  Modify structures on disk to resolve problems\n"
        "   read                    Read a block and display information about it\n"
        "   recover-raw             Recover a file based on its filesystem OID\n"
        "   recover                 Recover a file based on its filepath\n"
        "   resolver                Check if given Virtual OIDs resolve to given Physical OIDs\n"
        "   search-last-btree-node  Search the partition for B-tree nodes, reporting the Physical OID of the last one discovered\n"
        "   search                  Search the partition for blocks with certain features/properties\n"
        "   version                 Display Drat's version info\n"
    );
}

int main(int argc, char** argv) {
    char* cmd_name = argv[1];
    if (!cmd_name) {
        print_usage(false);
        return 0;
    }

    command_function* cmd = get_command_function(cmd_name);
    if (!cmd) {
        fprintf(stderr, "Unrecognised command `%s`.\n\n", cmd_name);
        print_usage(true);
        return -1;
    }

    return cmd(argc - 1, argv + 1);
}
