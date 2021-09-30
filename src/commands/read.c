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
    paddr_t block_addr;
} options_t;

#define DRAT_ARG_KEY_BLOCK      (DRAT_GLOBAL_ARGS_LAST_KEY - 1)

#define DRAT_ARG_ERR_INVALID_BLOCK  (DRAT_GLOBAL_ARGS_LAST_ERR - 1)
#define DRAT_ARG_ERR_NO_BLOCK       (DRAT_GLOBAL_ARGS_LAST_ERR - 2)

static const struct argp_option argp_options[] = {
    // char* name,  int key,            char* arg,      int flags,  char* doc
    { "block",      DRAT_ARG_KEY_BLOCK, "block addr",   0,          "Block address" },
    {0}
};

static error_t argp_parser(int key, char* arg, struct argp_state* state) {
    options_t* options = state->input;

    switch (key) {
        case DRAT_ARG_KEY_BLOCK:
            if (!parse_number(&options->block_addr, arg)) {
                return DRAT_ARG_ERR_INVALID_BLOCK;
            }
            break;
        case ARGP_KEY_END:
            if (options->block_addr == -1) {
                return DRAT_ARG_ERR_NO_BLOCK;
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
        "Usage:   %1$s %2$s --container <container> --block <block address>\n"
        "Example: %1$s %2$s --container /dev/disk0s2 --block 0x3af2\n",
        globals.program_name,
        globals.command_name
    );
}

int cmd_read(int argc, char** argv) {
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
            case DRAT_ARG_ERR_INVALID_BLOCK:
                fprintf(stderr, "%s: option `--block`" INVALID_HEX_STRING, globals.program_name);
                break;
            case DRAT_ARG_ERR_NO_BLOCK:
                fprintf(stderr, "%s: option `--block` is mandatory.\n", globals.program_name);
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

    printf("Reading block %#"PRIx64" ... ", options.block_addr);
    obj_phys_t* block = malloc(globals.block_size);
    if (read_blocks(block, options.block_addr, 1) == 0) {
        return EX_NOINPUT;
    }
    printf("validating ... ");
    if (is_cksum_valid(block)) {
        printf("OK.\n");
    } else {
        printf("FAILED.\nThe specified block may contain file-system data or be free space.\n");
    }
    printf("\n");
    
    printf("Details of block %#"PRIx64":\n", options.block_addr);
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

    free(block);
    close_container();
    return 0;
}
