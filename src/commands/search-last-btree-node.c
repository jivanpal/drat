#include <stdio.h>
#include <sys/errno.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "../apfs/io.h"
#include "../apfs/func/boolean.h"
#include "../apfs/func/cksum.h"
#include "../apfs/func/btree.h"

#include "../apfs/struct/object.h"
#include "../apfs/struct/nx.h"
#include "../apfs/struct/omap.h"
#include "../apfs/struct/fs.h"

#include "../apfs/struct/j.h"
#include "../apfs/struct/dstream.h"
#include "../apfs/struct/sibling.h"
#include "../apfs/struct/snap.h"

#include "../apfs/string/object.h"
#include "../apfs/string/nx.h"
#include "../apfs/string/omap.h"
#include "../apfs/string/btree.h"
#include "../apfs/string/fs.h"
#include "../apfs/string/j.h"

/**
 * Print usage info for this program.
 */
static void print_usage(int argc, char** argv) {
    fprintf(
        argc == 1 ? stdout : stderr,
        
        "Usage:   %s <container>\n"
        "Example: %s /dev/disk0s2\n",

        argv[0],
        argv[0]
    );
}

int cmd_search_last_btree_node(int argc, char** argv) {
    if (argc == 1) {
        print_usage(argc, argv);
        return 0;
    }
    
    setbuf(stdout, NULL);

    // Extrapolate CLI arguments, exit if invalid
    if (argc != 2) {
        fprintf(stderr, "Incorrect number of arguments.\n");
        print_usage(argc, argv);
        return 1;
    }
    nx_path = argv[1];
    
    // Open (device special) file corresponding to an APFS container, read-only
    printf("Opening file at `%s` in read-only mode ... ", nx_path);
    nx = fopen(nx_path, "rb");
    if (!nx) {
        fprintf(stderr, "\nABORT: ");
        report_fopen_error();
        printf("\n");
        return -errno;
    }
    printf("OK.\n");

    obj_phys_t* block = malloc(nx_block_size);
    if (!block) {
        fprintf(stderr, "\nABORT: Could not allocate sufficient memory for `block`.\n");
        return -1;
    }

    printf("Reading block 0x0 to obtain block count ... ");
    if (read_blocks(block, 0x0, 1) != 1) {
        printf("FAILED.\n");
        return -1;
    }
    printf("OK.\n");

    uint64_t num_blocks = ((nx_superblock_t*)block)->nx_block_count;
    printf("The specified device has %" PRIu64 " = %#" PRIx64 " blocks. Commencing search:\n\n", num_blocks, num_blocks);

    uint64_t num_matches = 0;
    uint64_t first_match_addr = 0;
    uint64_t last_match_addr = 0;

    printf("First match: %#" PRIx64 "\n", first_match_addr);
    printf("Last match:  %#" PRIx64 "\n", last_match_addr);

    for (uint64_t addr = 0xa5e3b; addr < 0x13adf2; addr++) {
        printf("\rReading %#" PRIx64 " ...", addr);

        if (read_blocks(block, addr, 1) != 1) {
            if (feof(nx)) {
                printf("Reached end of file; ending search.\n");
                break;
            }

            assert(ferror(nx));
            printf("- An error occurred whilst reading block %#" PRIx64 ".\n", addr);
            continue;
        }

        /** Search criteria for dentries of items with certain names **/
        if (   is_cksum_valid(block)
            && is_btree_node_phys(block)
        ) {
            btree_node_phys_t* node = block;
            
            if (node->btn_flags & BTNODE_LEAF) {
                num_matches++;
                last_match_addr = addr;
                if (first_match_addr == 0) {
                    first_match_addr = addr;
                }
                // Move to start of line,
                // clear whole line (which reads "Reading 0x..."),
                // move up one line,
                // clear whole line (which reads "Last match: 0x..."),
                // move up one line,
                // clear whole line (which reads "First match: 0x...")
                printf("\r\033[2K\033[A\033[2K\033[A\033[2K");
                printf("First match: %#" PRIx64 "\n", first_match_addr);
                printf("Last match:  %#" PRIx64 "\n", last_match_addr);
            }
        }
    }

    printf("\n\nFinished search; found %" PRIu64 " results.\n\n", num_matches);
    
    return 0;
}
