#include <stdio.h>
#include <sys/errno.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <sysexits.h>

#include <apfs/object.h>
#include <apfs/nx.h>
#include <apfs/omap.h>
#include <apfs/fs.h>
#include <apfs/j.h>
#include <apfs/dstream.h>
#include <apfs/sibling.h>
#include <apfs/snap.h>

#include <drat/globals.h>
#include <drat/argp.h>
#include <drat/strings.h>
#include <drat/io.h>

#include <drat/func/boolean.h>
#include <drat/func/cksum.h>
#include <drat/func/btree.h>

#include <drat/string/object.h>
#include <drat/string/nx.h>
#include <drat/string/omap.h>
#include <drat/string/btree.h>
#include <drat/string/fs.h>
#include <drat/string/j.h>

typedef struct {
    int64_t omap_root_addr;
    size_t  num_oids;
    int64_t oids[];
} options_t;

#define DRAT_ARG_KEY_OMAP   (DRAT_GLOBAL_ARGS_LAST_KEY - 1)
#define DRAT_ARG_KEY_OIDS   (DRAT_GLOBAL_ARGS_LAST_KEY - 2)

#define DRAT_ARG_ERR_INVALID_OMAP   (DRAT_GLOBAL_ARGS_LAST_ERR - 1)
#define DRAT_ARG_ERR_INVALID_OID    (DRAT_GLOBAL_ARGS_LAST_ERR - 2)
#define DRAT_ARG_ERR_NO_OMAP        (DRAT_GLOBAL_ARGS_LAST_ERR - 3)
#define DRAT_ARG_ERR_NO_OIDS        (DRAT_GLOBAL_ARGS_LAST_ERR - 4)

static const struct argp_option argp_options[] = {
    // char* name,  int key,            char* arg,          int flags,      char* doc
    
    // TODO: Handle `--omap`
    // { "omap",       DRAT_ARG_KEY_OMAP,  "omap tree addr",   0,              "Block address of object map B-tree" },

    { "oids",       DRAT_ARG_KEY_OIDS,  "oid[,...]",        0,              "Comma-delimited list of OIDs" },
    { "oid",        DRAT_ARG_KEY_OIDS,  "oid",              OPTION_ALIAS,   "An OID" },
    {0}
};

static error_t argp_parser(int key, char* arg, struct argp_state* state) {
    options_t** options_p = state->input;
    options_t* options = *options_p;

    switch (key) {
        case DRAT_ARG_KEY_OMAP:
            if (!parse_number(&options->omap_root_addr, arg)) {
                return DRAT_ARG_ERR_INVALID_OMAP;
            }
            break;

        case DRAT_ARG_KEY_OIDS: {
            /*
             * NOTE: Need to enclose this case in a block `{}` to prevent the
             * following error when compiling on Linux: "a label can only be
             * part of a statement and a declaration is not a statement". This
             * practice also mitigates the possibility of names being declared
             * multiple times in the same variable scope.
             */
            char* option = arg;
            char* const tokens[] = {0};
            char* value = NULL;
            
            while (*option) {
                switch (getsubopt_posix(&option, tokens, &value)) {
                    case -1: {  // Need brace to prevent compile-time error on Linux
                        int64_t oid = -1;
                        if (!parse_number(&oid, value) || oid == -1) {
                            return DRAT_ARG_ERR_INVALID_OID;
                        }
                        options->num_oids++;
                        options = *options_p = realloc(options, sizeof(options_t) + options->num_oids * sizeof(options->oids[0]));
                        options->oids[options->num_oids - 1] = oid;
                        break;
                    }
                    default:
                        // This should never happen since `tokens` is empty
                        assert(false);
                        print_arg_parse_error();
                        break;
                }
            }
            break;
        }

        case ARGP_KEY_END:
            // TODO: Handle `--omap`
            // if (options->omap_root_addr == -1) {
            //     return DRAT_ARG_ERR_NO_OMAP;
            // }
            
            if (options->num_oids == 0) {
                return DRAT_ARG_ERR_NO_OIDS;
            }
            // fall through
        default:
            return ARGP_ERR_UNKNOWN;
    }

    return 0;
}

// TODO: Perhaps factor out from all commands
static const struct argp argp = {
    &argp_options,  // struct argp_option* options
    &argp_parser,   // argp_parser_t parser
    0,              // char* args_doc
    0,              // char* doc
    &argp_children  // struct argp_child* children
};

/**
 * Print usage info for this program.
 */
static void print_usage(FILE* stream) {
    fprintf(
        stream,
        "Usage:   %1$s %2$s --container <container> --volume <volume> --oids <oid>[,...]\n"
        "Example: %1$s %2$s --container /dev/disk0s2 --volume 0 --oids 0x247,0x11f3,0x10a\n",
        globals.program_name,
        globals.command_name
    );
}

int cmd_resolver(int argc, char** argv) {
    if (argc == 2) {
        // Command was specified with no other arguments
        print_usage(stdout);
        return 0;
    }

    // Set placeholder values so that the parser can identify whether the user
    // has set mandatory options or not
    globals.volume = -1;
    options_t* options = malloc(sizeof(options_t));
    options->omap_root_addr = -1;
    options->num_oids = 0;

    bool usage_error = true;
    error_t parse_result = argp_parse(&argp, argc, argv, ARGP_IN_ORDER, 0, &options);
    if (!print_global_args_error(parse_result)) {
        switch (parse_result) {
            case 0:
                usage_error = false;
                break;
            case DRAT_ARG_ERR_INVALID_OMAP:
                fprintf(stderr, "%s: option `--omap`" INVALID_HEX_STRING, globals.program_name);
                break;
            case DRAT_ARG_ERR_INVALID_OID:
                fprintf(stderr, "%s: option `--oids`" INVALID_HEX_STRING, globals.program_name);
                break;
            case DRAT_ARG_ERR_NO_OMAP:
                fprintf(stderr, "%s: option `--omap` is mandatory.\n", globals.program_name);
                break;
            case DRAT_ARG_ERR_NO_OIDS:
                fprintf(stderr, "%s: option `--oids` is mandatory.\n", globals.program_name);
                break;
            default:
                print_arg_parse_error();
                return EX_SOFTWARE;
        }
    }
    if (usage_error) {
        print_usage(stderr);
        return EX_USAGE;
    }

    // TODO: Perhaps handle other return values and factor out
    if (open_container() != 0) {
        return EX_NOINPUT;
    }
    printf("\n");
    
    setbuf(stdout, NULL);

    printf("Finding most recent container superblock:\n");

    printf("- Reading block 0x0 ... ");
    nx_superblock_t* nxsb = malloc(globals.block_size);
    if (!nxsb) {
        fprintf(stderr, "ABORT: Not enough memory to create `nxsb`.\n");
        return EX_OSERR;
    }
    if (read_blocks(nxsb, 0, 1) != 1) {
        fprintf(stderr, "ABORT: Failed to read block 0.\n");
        return EX_IOERR;
    }
    printf("validating ... ");
    if (!is_cksum_valid(nxsb)) {
        printf("FAILED.\n");
        printf("WARNING: Block 0 did not validate.\n");
    } else {
        printf("OK.\n");
    }

    if (!is_nx_superblock(nxsb)) {
        printf("WARNING: Block 0 should be a container superblock, but has the wrong object type.\n\n");
    }
    if (nxsb->nx_magic != NX_MAGIC) {
        printf(
            "WARNING: Block 0 %s a container superblock but doesn't have the correct magic number.\n",
            is_nx_superblock(nxsb) ? "is" : "should be"
        );
    }

    printf("- Loading checkpoint descriptor area ... ");
    
    uint32_t xp_desc_blocks = nxsb->nx_xp_desc_blocks & ~(1 << 31);
    char (*xp_desc)[globals.block_size] = malloc(xp_desc_blocks * globals.block_size);
    if (!xp_desc) {
        fprintf(stderr, "ABORT: Not enough memory for %"PRIu32" blocks.\n", xp_desc_blocks);
        return EX_OSERR;
    }

    if (nxsb->nx_xp_desc_blocks >> 31) {
        // TODO: implement case when xp_desc area is not contiguous
        printf("END: Checkpoint descriptor area is a B-tree, but we haven't implemented handling of this case yet.\n\n");
        return 0;
    } else {
        size_t blocks_read = read_blocks(xp_desc, nxsb->nx_xp_desc_base, xp_desc_blocks);
        if (blocks_read != xp_desc_blocks) {
            printf("FAILED.\n");
            fprintf(stderr, "\nABORT: Failed to read all blocks in the checkpoint descriptor area; only successfully read %zu blocks.\n", blocks_read);
            return EX_IOERR;
        } else {
            printf("OK.\n");
        }
    }

    printf("- Searching checkpoint descriptor area ... ");
    
    uint32_t i_latest_nx = 0;
    xid_t xid_latest_nx = 0;

    for (uint32_t i = 0; i < xp_desc_blocks; i++) {
        if (!is_cksum_valid(xp_desc[i])) {
            printf("WARNING: Checkpoint descriptor block %"PRIu32" failed validation.\n", i);
            continue;
        }
        
        if (is_nx_superblock(xp_desc[i])) {
            if ( ((nx_superblock_t*)xp_desc[i])->nx_magic  !=  NX_MAGIC ) {
                printf("WARNING: Checkpoint descriptor block %"PRIu32" is a container superblock but doesn't have the correct magic number.\n", i);
                continue;
            }

            xid_t nxsb_xid = ((nx_superblock_t*)xp_desc[i])->nx_o.o_xid;

            if ( (nxsb_xid > xid_latest_nx) && (nxsb_xid <= (xid_t)(globals.max_xid)) ) {
                i_latest_nx = i;
                xid_latest_nx = nxsb_xid;
            }
        } else if (!is_checkpoint_map_phys(xp_desc[i])) {
            printf("WARNING: Checkpoint descriptor block %"PRIu32" is neither a container superblock nor a checkpoint map.\n", i);
            continue;
        }
    }

    if (xid_latest_nx == 0) {
        printf("END: Didn't find any container superblock with maximum XID %#"PRIx64".\n", globals.max_xid);
        return 0;
    }
    
    // Don't need block 0 anymore; overwrite `nxsb` with the latest container superblock.
    memcpy(nxsb, xp_desc[i_latest_nx], sizeof(nx_superblock_t));
    // Don't need the checkpoint descriptor anymore
    free(xp_desc);
    
    printf("found most recent container superblock at index %"PRIu32", its XID is %#"PRIx64".\n", i_latest_nx, nxsb->nx_o.o_xid);

    printf("Finding container's omap tree:\n");
    
    printf("- Container's omap has Physical OID %#"PRIx64".\n", nxsb->nx_omap_oid);

    printf("- Reading block %#"PRIx64" ... ", nxsb->nx_omap_oid);
    omap_phys_t* nx_omap = malloc(globals.block_size);
    if (read_blocks(nx_omap, nxsb->nx_omap_oid, 1) != 1) {
        fprintf(stderr, "ABORT: Not enough memory for `nx_omap`.\n");
        return EX_OSERR;
    }
    printf("validating ... ");
    printf(is_cksum_valid(nx_omap) ? "OK.\n" : "FAILED.\n");
    
    if ((nx_omap->om_tree_type & OBJ_STORAGETYPE_MASK) != OBJ_PHYSICAL) {
        printf("  - END: Container omap B-tree is not a Physical object, and thus cannot be located.\n");
        return 0;
    }
    printf("- Container's omap tree has Physical OID %#"PRIx64".\n", nx_omap->om_tree_oid);

    printf("- Reading block %#"PRIx64" ... ", nx_omap->om_tree_oid);
    /**
     * The B-tree of the omap to search. If we are interested in a volume rather
     * than the container itself, then we will overwrite this with the volume's
     * omap B-tree later.
     */
    btree_node_phys_t* omap_btree = malloc(globals.block_size);
    if (!omap_btree) {
        fprintf(stderr, "ABORT: Not enough memory for `omap_btree`.\n");
        return EX_OSERR;
    }
    if (read_blocks(omap_btree, nx_omap->om_tree_oid, 1) != 1) {
        fprintf(stderr, "ABORT: Failed to read block %#"PRIx64".\n", nx_omap->om_tree_oid);
        return EX_IOERR;
    }
    printf("validating ...");
    printf(is_cksum_valid(omap_btree) ? "OK.\n" : "FAILED.\n");

    if (globals.volume != 0) {
        printf("\n");

        // TODO: Handle `--volume-name` as well as `--volume` here.
        printf("Finding volume %"PRId64"'s superblock:\n", globals.volume);
        oid_t apsb_oid = nxsb->nx_fs_oid[globals.volume - 1];
        if (apsb_oid == 0) {
            printf("  - END: Volume %"PRId64" does not exist (apparent Virtual OID is 0).\n", globals.volume);
            return 0;
        }
        printf("- Volume %"PRId64" has Virtual OID %#"PRIx64" ... ", globals.volume, apsb_oid);

        // Get the block address
        omap_entry_t* fs_entry = get_btree_phys_omap_entry(omap_btree, apsb_oid, nxsb->nx_o.o_xid);
        if (!fs_entry) {
            // TODO: Need better handling of this case; look at the previous transaction.
            printf("END: No objects with Virtual OID %#"PRIx64" and maximum XID %#"PRIx64" exist in `omap_btree`.\n", apsb_oid, nxsb->nx_o.o_xid);
            return 0;
        }
        
        printf("maps to block %#"PRIx64" with XID %#"PRIx64".\n", fs_entry->val.ov_paddr, fs_entry->key.ok_xid);

        // Read the block
        printf("- Reading block %#"PRIx64" ... ", fs_entry->val.ov_paddr);
        apfs_superblock_t* apsb = malloc(globals.block_size);
        if (!apsb) {
            fprintf(stderr, "ABORT: Not enough memory for `apsb`.\n");
            return EX_OSERR;
        }
        if (read_blocks(apsb, fs_entry->val.ov_paddr, 1) != 1) {
            fprintf(stderr, "ABORT: Failed to read block %#"PRIx64".\n", fs_entry->val.ov_paddr);
            return EX_IOERR;
        }
        printf("validating ... ");
        printf(is_cksum_valid(apsb) ? "OK.\n" : "FAILED.\n");
        printf("- Volume name: %s\n", apsb->apfs_volname);

        printf("Finding volume's omap tree:\n");

        printf("- Volume's omap has Physical OID %#"PRIx64".\n", apsb->apfs_omap_oid);

        printf("- Reading block %#"PRIx64" ... ", apsb->apfs_omap_oid);
        omap_phys_t* fs_omap = malloc(globals.block_size);
        if (!fs_omap) {
            fprintf(stderr, "ABORT: Not enough memory for `fs_omap`.\n");
            return EX_OSERR;
        }
        if (read_blocks(fs_omap, apsb->apfs_omap_oid, 1) != 1) {
            fprintf(stderr, "ABORT: Failed to read block %#"PRIx64".\n", apsb->apfs_omap_oid);
            return EX_IOERR;
        }
        printf("validating ... ");
        printf(is_cksum_valid(fs_omap) ? "OK.\n" : "FAILED.\n");

        if ((fs_omap->om_tree_type & OBJ_STORAGETYPE_MASK) != OBJ_PHYSICAL) {
            printf("- END: Volume's omap tree is not a Physical object, and thus cannot be located.\n");
            return 0;
        }
        printf("- Volume's omap tree has Physical OID %#"PRIx64".\n", fs_omap->om_tree_oid);

        printf("- Reading block %#"PRIx64" ... ", fs_omap->om_tree_oid);
        btree_node_phys_t* fs_omap_btree = malloc(globals.block_size);
        if (!fs_omap_btree) {
            fprintf(stderr, "ABORT: Not enough memory for `fs_omap_btree`.\n");
            return EX_OSERR;
        }
        if (read_blocks(fs_omap_btree, fs_omap->om_tree_oid, 1) != 1) {
            fprintf(stderr, "ABORT: Failed to read block %#"PRIx64".\n", fs_omap->om_tree_oid);
            return EX_IOERR;
        }
        printf("validating ... ");
        printf(is_cksum_valid(fs_omap_btree) ? "OK.\n" : "FAILED.\n");

        /*
         * We want to search the volume omap B-tree rather than the container
         * omap B-tree; overwrite `omap_tree` accordingly.
         */
        memcpy(omap_btree, fs_omap_btree, globals.block_size);

        free(fs_omap_btree);
        free(fs_omap);
        free(apsb);
        free(fs_entry);
    }

    printf("\n");
    
    for (size_t i = 0; i < options->num_oids; i++) {
        oid_t oid = (oid_t)(options->oids[i]);
        printf("%#"PRIx64" -> ", oid);
        
        omap_entry_t* omap_entry = get_btree_phys_omap_entry(omap_btree, oid, globals.max_xid);
        if (!omap_entry) {
            printf("WARNING: Could not resolve %#"PRIx64": no such entry in omap tree.\n", oid);
            continue;
        }
        printf("%#"PRIx64" with XID %#"PRIx64"\n", omap_entry->val.ov_paddr, omap_entry->key.ok_xid);

        free(omap_entry);
    }

    // Closing statements; de-allocate all memory, close all file descriptors.
    free(omap_btree);
    free(nx_omap);
    free(nxsb);
    close_container();
    free(options);

    return 0;
}
