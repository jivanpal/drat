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
        "   inspect     Inspect APFS partition\n"
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
