/**
 * Functions that generate nicely formatted strings representing data found in
 * B-tree nodes, i.e. instances of `btree_node_phys_t`.
 */

#include "btree.h"

#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <drat/io.h>    // nx_block_size
#include <drat/func/boolean.h>
#include <drat/string/common.h>
#include <drat/string/object.h>

/**
 * Get a human-readable list of the flags that are set on a
 * given B-tree node.
 * 
 * btn:     A pointer to the B-tree node in question.
 * 
 * RETURN VALUE:
 *      A pointer to the first character of the string. The caller must free
 *      this pointer when it is no longer needed.
 */
char* get_btn_flags_string(btree_node_phys_t* btn) {
    struct u64_string_mapping flags[] = {
        { BTNODE_ROOT,              "Root node" },
        { BTNODE_LEAF,              "Leaf node" },
        { BTNODE_FIXED_KV_SIZE,     "Fixed size for keys and values" },
        { BTNODE_HASHED,            "Contains child node hashes" },
        { BTNODE_NOHEADER,          "Doesn't have an object header" },
        { BTNODE_CHECK_KOFF_INVAL,  "In transient state (key offsets are invalid) --- should never appear on disk" },
    };

    // Initialise result buffer as empty string
    const size_t bufsize = 2048;
    char* result_string = malloc(bufsize);
    if (!result_string) {
        fprintf(stderr, "\nERROR: %s: Couldn't create buffer `result_string`.\n", __func__);
        return NULL;
    }
    *result_string = '\0';

    size_t bytes_written = 0;
    for (size_t i = 0; i < ARRAY_SIZE(flags); i++) {
        if (btn->btn_flags & flags[i].value) {
            bytes_written += snprintf(
                result_string + bytes_written,
                bufsize - bytes_written,
                "- %s\n",
                flags[i].string
            );
            
            if (bytes_written > bufsize - 1) {
                // Exhausted buffer; return early.
                fprintf(stderr, "\nERROR: %s: Buffer `result_string` too small for entire result.\n", __func__);
                return result_string;
            }
        }
    }

    if (bytes_written == 0) {
        // No flags set; use default string.
        snprintf(result_string, bufsize, "- No flags are set.\n");
    }

    // Truncate buffer
    result_string = realloc(result_string, strlen(result_string) + 1);

    return result_string;
}

/**
 * Get a human-readable list of the flags that are set on a
 * given B-tree info object (an instance of `btree_info_t`; or equivalently for
 * this purpose, an instance of `btree_info_fixed_t`). The list is indented by
 * two spaces.
 * 
 * bt_info:     A pointer to the B-tree info object in question.
 * 
 * RETURN VALUE:
 *      A pointer to the first character of the string. The caller must free
 *      this pointer when it is no longer needed.
 */
char* get_bt_info_flags_string(btree_info_t* bt_info) {
    char* default_string = "  - No flags are set\n";
    size_t default_string_len = strlen(default_string);
    
    uint8_t NUM_FLAGS = 9;
    
    uint16_t flag_constants[] = {
        BTREE_UINT64_KEYS,
        BTREE_SEQUENTIAL_INSERT,
        BTREE_ALLOW_GHOSTS,
        BTREE_EPHEMERAL,
        BTREE_PHYSICAL,
        BTREE_NONPERSISTENT,
        BTREE_KV_NONALIGNED,
        BTREE_HASHED,
        BTREE_NOHEADER,
    };

    char* flag_strings[] = {
        "Keys are 64-bit values --- optimisations operations if possible",
        "This B-tree is currently undergoing a series of sequential inserts --- optimise operations if possible",
        "Ghosts (keys without values) are allowed",
        "Child nodes are referred to using Ephemeral OIDs",
        "Child nodes are referred to using Physical OIDs",
        "This B-tree does not persist across unmounts",
        "8-byte alignment of keys and values is not required",
        "Non-leaf nodes store a hash of their child nodes",
        "Nodes don't have object headers",
    };

    size_t max_mem_required = 0;
    for (uint8_t i = 0; i < NUM_FLAGS; i++) {
        max_mem_required += strlen(flag_strings[i]) + 5;
        // `+ 5` accounts for prepending "  - " and appending "\n" to each string
    }
    if (max_mem_required < default_string_len) {
        max_mem_required = default_string_len;
    }
    max_mem_required++; // Account for terminating NULL byte

    char* result_string = malloc(max_mem_required);
    if (!result_string) {
        fprintf(stderr, "\nABORT: get_bt_info_flags_string: Could not allocate sufficient memory for `result_string`.\n");
        exit(-1);
    }

    char* cursor = result_string;
    for (uint8_t i = 0; i < NUM_FLAGS; i++) {
        if (bt_info->bt_fixed.bt_flags & flag_constants[i]) {
            memcpy(cursor, "  - ", 4);
            cursor += 4;

            size_t flag_string_len = strlen(flag_strings[i]);
            memcpy(cursor, flag_strings[i], flag_string_len);
            cursor += flag_string_len;
            
            *cursor++  = '\n';
        }
    }

    // If no flags set, nothing is added to string, so use deafult string
    if (cursor == result_string) {
        memcpy(cursor, default_string, default_string_len);
        cursor += default_string_len;
    }

    *cursor = '\0';

    // De-allocate excess memory
    result_string = realloc(result_string, strlen(result_string) + 1);
    return result_string;
}

/**
 * Print a nicely formatted string describing the data contained in an instance
 * of `btree_info_t`.
 */
void print_btree_info(btree_info_t* bt_info) {
    printf("Info relating to the entire B-tree:\n");
    
    char* flags_string = get_bt_info_flags_string(bt_info);
    printf("- Flags:\n%s", flags_string);
    free(flags_string);

    printf("- Node size:                %u bytes\n",    bt_info->bt_fixed.bt_node_size);
    printf("- Key size:                 %u bytes\n",    bt_info->bt_fixed.bt_key_size);
    printf("- Value size:               %u bytes\n",    bt_info->bt_fixed.bt_val_size);
    printf("\n");


    printf("- Length of longest key:    %u bytes\n",    bt_info->bt_longest_key);
    printf("- Length of longest value:  %u bytes\n",    bt_info->bt_longest_val);
    printf("- Number of keys:           %" PRIu64 "\n",        bt_info->bt_key_count);
    printf("- Number of nodes:          %" PRIu64 "\n",        bt_info->bt_node_count);
}

/**
 * Print a nicely formatted string describing the data contained in a B-tree,
 * including the data in its header. If the given B-tree is a root node, data
 * relating to the entire tree that it is the root node of will also be printed.
 */
void print_btree_node_phys(btree_node_phys_t* btn) {
    print_obj_phys(btn);    // `btn` equals `&(btn->btn_o)`.

    char* flags_string = get_btn_flags_string(btn);
    printf("Flags:\n%s", flags_string);
    free(flags_string);

    printf("Number of child levels:         %u\n",  btn->btn_level);
    printf("Number of keys in this node:    %u\n",  btn->btn_nkeys);
    
    printf("Location of table of contents:\n");
    printf("- Offset from start of node data area:  0x%x = %u\n",   btn->btn_table_space.off,   btn->btn_table_space.off);
    printf("- Length (bytes):                       0x%x = %u\n",   btn->btn_table_space.len,   btn->btn_table_space.len);

    printf("Location of key–value shared free space:\n");
    printf("- Offset from start of keys area:       0x%x = %u\n",   btn->btn_free_space.off,    btn->btn_free_space.off);
    printf("- Length (bytes):                       0x%x = %u\n",   btn->btn_free_space.len,    btn->btn_free_space.len);

    if (is_btree_node_phys_root(btn)) {
        printf("\n");
        print_btree_info((char*)btn + nx_block_size - sizeof(btree_info_t));
    }
}
