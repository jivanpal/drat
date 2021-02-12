#include <stdio.h>
#include <sys/errno.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "../apfs/io.h"
#include "../apfs/struct/general.h"

#include "../apfs/func/boolean.h"
#include "../apfs/func/cksum.h"

#include "../apfs/string/object.h"
#include "../apfs/string/nx.h"
#include "../apfs/string/btree.h"
#include "../apfs/string/omap.h"
#include "../apfs/string/fs.h"

static void print_usage(int argc, char** argv) {
    fprintf(
        argc == 1 ? stdout : stderr,
        
        "Usage:   %s <container> <address>\n"
        "Example: %s /dev/disk0s2 0x3af2\n",
        
        argv[0],
        argv[0]
    );
}

int cmd_read(int argc, char** argv) {
    if (argc == 1) {
        print_usage(argc, argv);
        return 0;
    }

    // Extrapolate CLI arguments, exit if invalid
    if (argc != 3) {
        fprintf(stderr, "Incorrect number of arguments.\n");
        print_usage(argc, argv);
        return 1;
    }
    char* nx_path = argv[1];
    paddr_t nx_block_addr = 0x0;
    bool parse_success = sscanf(argv[2], "0x%" PRIx64 "", &nx_block_addr);
    if (!parse_success) {
        parse_success = sscanf(argv[2], "%" PRIu64 "", &nx_block_addr);
    }
    if (!parse_success) {
        printf("%s is not a valid block address.\n", argv[2]);
        print_usage(argc, argv);
        return 1;
    }
    
    // Open (device special) file corresponding to an APFS container, read-only
    printf("Opening file at `%s` in read-only mode ... ", nx_path);
    nx = fopen(nx_path, "rb");
    if (!nx) {
        fprintf(stderr, "\nABORT: main: ");
        report_fopen_error();
        printf("\n");
        return -errno;
    }
    printf("OK.\n\n");

    printf("Reading block 0x%" PRIx64 " ... ", nx_block_addr);
    obj_phys_t* block = malloc(nx_block_size);
    if (read_blocks(block, nx_block_addr, 1) == 0) {
        printf("\nEND: Block index %s does not exist in `%s`.\n", argv[2], nx_path);
        goto cleanup;
    }
    printf("validating ... ");
    if (is_cksum_valid(block)) {
        printf("OK.\n");
    } else {
        printf("FAILED.\nThe specified block may contain file-system data or be free space.\n");
    }
    printf("\n");
    
    printf("Details of block 0x%" PRIx64 ":\n", nx_block_addr);
    printf("--------------------------------------------------------------------------------\n");
    switch (block->o_type & OBJECT_TYPE_MASK) {
        case OBJECT_TYPE_NX_SUPERBLOCK:
            print_nx_superblock(block);
            break;
        case OBJECT_TYPE_BTREE:
        case OBJECT_TYPE_BTREE_NODE:
            print_btree_node_phys(block);
            break;
        case OBJECT_TYPE_OMAP:
            print_omap_phys(block);
            break;
        case OBJECT_TYPE_CHECKPOINT_MAP:
            print_checkpoint_map_phys(block);
            break;
        case OBJECT_TYPE_FS:
            print_apfs_superblock(block);
            break;
        default:
            print_obj_phys(block);
            printf("\n--- This tool cannot currently display more details about this type of block ---\n");
            break;
    }
    printf("--------------------------------------------------------------------------------\n");
    printf("\nEND: All done.\n");

cleanup:
    free(block);
    fclose(nx);
    return 0;
}
