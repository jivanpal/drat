#include <stdio.h>
#include <sys/errno.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "apfs/io.h"
#include "apfs/func/boolean.h"
#include "apfs/func/cksum.h"
#include "apfs/func/btree.h"

#include "apfs/struct/object.h"
#include "apfs/struct/nx.h"
#include "apfs/struct/omap.h"
#include "apfs/struct/fs.h"

#include "apfs/struct/j.h"
#include "apfs/struct/dstream.h"
#include "apfs/struct/sibling.h"
#include "apfs/struct/snap.h"

#include "apfs/string/object.h"
#include "apfs/string/nx.h"
#include "apfs/string/omap.h"
#include "apfs/string/btree.h"
#include "apfs/string/fs.h"
#include "apfs/string/j.h"

#include "commands.h"

void print_usage() {
    fprintf(stderr,
        "Usage: drat <command>\n"
        "\n"
        "List of commands:\n"
        "   explore-fs-tree         Explore filesystem B-tree"
        "   explore-omap-tree       Explore object map B-tree"
        "   inspect                 Inspect APFS partition"
        "   list-raw                List directory contents or file info based on its filesystem OID"
        "   list                    List directory contents or file info based on its filepath"
        "   modify                  Modify structures on disk to resolve problems"
        "   read                    Read a block and display information about it"
        "   recover-raw             Recover a file based on its filesystem OID"
        "   recover                 Recover a file bsaed on its filepath"
        "   resolver                Check if given Virtual OIDs resolve to given Physical OIDs"
        "   search-last-btree-node  Search the partition for B-tree nodes, reporting the Physical OID of the last one discovered"
        "   search                  Search the partition for blocks with certain features/properties"
    );
}

int main(int argc, char** argv) {
    if (!argv[1]) {
        print_usage();
        return -1;
    }

    main_function* command_main = get_command_main(argv[1]);
    if (!command_main) {
        fprintf(stderr, "Unrecognised command `%s`.\n", argv[1]);
        return -1;
    }

    return command_main(argc - 1, argv + 1);
}
