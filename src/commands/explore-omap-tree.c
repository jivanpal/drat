#include <stdio.h>
#include <sys/errno.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <sysexits.h>

#include <apfs/general.h>

#include <drat/strings.h>
#include <drat/globals.h>
#include <drat/argp.h>
#include <drat/io.h>

#include <drat/func/boolean.h>
#include <drat/func/cksum.h>

#include <drat/string/object.h>
#include <drat/string/nx.h>
#include <drat/string/btree.h>
#include <drat/string/omap.h>
#include <drat/string/fs.h>

typedef struct {
    paddr_t omap_root_addr;
} options_t;

#define DRAT_ARG_KEY_OMAP   (DRAT_GLOBAL_ARGS_LAST_KEY - 1)

#define DRAT_ARG_ERR_INVALID_OMAP   (DRAT_GLOBAL_ARGS_LAST_ERR - 1)
#define DRAT_ARG_ERR_NO_OMAP        (DRAT_GLOBAL_ARGS_LAST_ERR - 2)

static const struct argp_option argp_options[] = {
    // char* name,  int key,            char* arg,          int flags,  char* doc
    { "omap",       DRAT_ARG_KEY_OMAP,  "omap tree addr",   0,          "Block address of object map B-tree" },
    {0}
};

static error_t argp_parser(int key, char* arg, struct argp_state* state) {
    options_t* options = state->input;

    switch (key) {
        case DRAT_ARG_KEY_OMAP:
            if (!parse_number(&options->omap_root_addr, arg)) {
                return DRAT_ARG_ERR_INVALID_OMAP;
            }
            break;
        case ARGP_KEY_END:
            if (options->omap_root_addr == -1) {
                return DRAT_ARG_ERR_NO_OMAP;
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

static void print_usage(FILE* stream) {
    fprintf(
        stream,
        "Usage:   %1$s %2$s --container <container> --omap <omap tree root node address>\n"
        "Example: %1$s %2$s --container /dev/disk0s2 --omap 0x3af2\n",
        globals.program_name,
        globals.command_name
    );
}

int cmd_explore_omap_tree(int argc, char** argv) {
    if (argc == 2) {
        // Command was specified with no other arguments
        print_usage(stdout);
        return 0;
    }

    // Set placeholder values so that the parser can identify whether the user
    // has set mandatory options or not
    options_t options = {-1};

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
            case DRAT_ARG_ERR_NO_OMAP:
                fprintf(stderr, "%s: option `--omap` is mandatory.\n", globals.program_name);
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

    // Read the specified root node
    printf("Reading block %#"PRIx64" ... ", options.omap_root_addr);
    btree_node_phys_t* omap_root_node = malloc(globals.block_size);
    if (!omap_root_node) {
        fprintf(stderr, "\nABORT: Could not allocate sufficient memory for `omap_root_node`.\n");
        return -1;
    }

    if (read_blocks(omap_root_node, options.omap_root_addr, 1) == 0) {
        return EX_NOINPUT;
    }

    printf("validating ... ");
    if (is_cksum_valid(omap_root_node)) {
        printf("OK.\n");
    } else {
        printf("FAILED.\n");
    }
    printf("\n");

    // Allocate space for the current working node,
    // then copy the root node to this space.
    btree_node_phys_t* node = malloc(globals.block_size);
    if (!node) {
        fprintf(stderr, "\nABORT: Could not allocate sufficient memory for `node`.\n");
        return -1;
    }
    memcpy(node, omap_root_node, globals.block_size);

    // Pointers to areas of the node
    char* toc_start = (char*)node->btn_data + node->btn_table_space.off;
    char* key_start = toc_start + node->btn_table_space.len;
    char* val_end   = (char*)node + globals.block_size;
    if (node->btn_flags & BTNODE_ROOT) {
        val_end -= sizeof(btree_info_t);
    }

    // // Copy the B-tree info somewhere else for easy referencing
    // btree_info_t* bt_info = malloc(sizeof(btree_info_t));
    // if (!bt_info) {
    //     fprintf(stderr, "\nABORT: Could not allocate sufficient memory for `bt_info`.\n");
    //     return -1;
    // }
    // memcpy(bt_info, val_end, sizeof(btree_info_t));

    // Descend the tree
    while (true) {
        if (!(node->btn_flags & BTNODE_FIXED_KV_SIZE)) {
            fprintf(stderr, "\nObject map trees don't have variable size keys and values ... do they?\n");
            return 0;
        }

        printf("\nNode has %"PRIu32" entries, as follows:\n", node->btn_nkeys);
        kvoff_t* toc_entry = toc_start;
        // Print mapped block's details if explroing a leaf node
        if (node->btn_flags & BTNODE_LEAF) {
            obj_phys_t* block = malloc(globals.block_size);
            if (!block) {
                fprintf(stderr, "\nABORT: Could not allocate sufficient memory for `block`.\n");
                return -1;
            }
            for (uint32_t i = 0;    i < node->btn_nkeys;    i++, toc_entry++) {
                omap_key_t* key = key_start + toc_entry->k;
                omap_val_t* val = val_end   - toc_entry->v;
                if (read_blocks(block, val->ov_paddr, 1) != 1) {
                    fprintf(stderr, "\nABORT: read_blocks: Error reading block %#"PRIx64".\n", val->ov_paddr);
                    return -1;
                }

                printf(
                    "- %3"PRIu32":"
                    "  OID = %#9"PRIx64""
                    "  ||  XID = %#9"PRIx64""
                    "  ||  Target block = %#9"PRIx64""
                    "  ||  Target's actual OID = %#9"PRIx64" (%s)"
                    "  ||  Target's actual XID = %#9"PRIx64" (%s)\n", 
                    
                    i,
                    key->ok_oid,
                    key->ok_xid,
                    val->ov_paddr,
                    block->o_oid,   block->o_oid == key->ok_oid ? "YES  " : "   NO",
                    block->o_xid,   block->o_xid == key->ok_xid ? "YES  " : "   NO"
                );
            }
            free(block);
        } else {
            for (uint32_t i = 0;    i < node->btn_nkeys;    i++, toc_entry++) {
                omap_key_t* key = key_start + toc_entry->k;
                omap_val_t* val = val_end - toc_entry->v;

                printf(
                    "- %3"PRIu32":"
                    "  OID = %#9"PRIx64""
                    "  ||  XID = %#9"PRIx64""
                    "  ||  Target child node = %#9"PRIx64"\n",
                    
                    i,
                    key->ok_oid,
                    key->ok_xid,
                    val->ov_paddr
                );
            }
        } 
        
        uint32_t entry_index;
        printf("Choose an entry [0-%"PRIu32"]: ", node->btn_nkeys - 1);
        scanf("%"SCNu32"", &entry_index);
        while (entry_index >= node->btn_nkeys) {
            printf("Invalid choice; choose an entry [0-%"PRIu32"]: ", node->btn_nkeys - 1);
            scanf("%"SCNu32"", &entry_index);
        }

        toc_entry = (kvoff_t*)toc_start + entry_index;

        // If this is a leaf node, output the object map value
        if (node->btn_flags & BTNODE_LEAF) {
            omap_key_t* key = key_start + toc_entry->k;
            omap_val_t* val = val_end   - toc_entry->v;

            printf("KEY:\n");
            print_omap_key(key);
            printf("\nVALUE:\n");
            print_omap_val(val);
            return 0;
        }

        // Else, read the corresponding child node into `node` and loop
        paddr_t* child_node_addr = val_end - toc_entry->v;

        printf("Child node resides at address %#"PRIx64". Reading ... ", *child_node_addr);
        if (read_blocks(node, *child_node_addr, 1) != 1) {
            fprintf(stderr, "\nABORT: Failed to read block %#"PRIx64".\n", *child_node_addr);
            return -1;
        }

        printf("validating ... ");
        if (is_cksum_valid(node)) {
            printf("OK.\n");
        } else {
            printf("FAILED.\n");
        }

        toc_start = (char*)(node->btn_data) + node->btn_table_space.off;
        key_start = toc_start + node->btn_table_space.len;
        val_end   = (char*)node + globals.block_size;   // Always dealing with non-root node here
    }
    
    return 0;
}
