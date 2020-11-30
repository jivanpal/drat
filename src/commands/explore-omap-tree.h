#include <stdio.h>
#include <sys/errno.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "apfs/io.h"
#include "apfs/struct/general.h"

#include "apfs/func/boolean.h"
#include "apfs/func/cksum.h"

#include "apfs/string/object.h"
#include "apfs/string/nx.h"
#include "apfs/string/btree.h"
#include "apfs/string/omap.h"
#include "apfs/string/fs.h"

/**
 * Print usage info for this program.
 */
void print_usage(char* program_name) {
    printf("Usage:   %s <container> <root node address>\nExample: %s /dev/disk0s2 0x3af2\n\n", program_name, program_name);
}

int main(int argc, char** argv) {
    printf("\n");

    // Extrapolate CLI arguments, exit if invalid
    if (argc != 3) {
        printf("Incorrect number of arguments.\n");
        print_usage(argv[0]);
        return 1;
    }
    
    char* nx_path = argv[1];
    
    paddr_t root_node_block_addr;
    bool parse_success = sscanf(argv[2], "0x%llx", &root_node_block_addr);
    if (!parse_success) {
        parse_success = sscanf(argv[2], "%llu", &root_node_block_addr);
    }
    if (!parse_success) {
        printf("%s is not a valid block address.\n", argv[2]);
        print_usage(argv[0]);
        printf("\n");
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

    // Read the specified root node
    printf("Reading block 0x%llx ... ", root_node_block_addr);
    btree_node_phys_t* root_node = malloc(nx_block_size);
    if (!root_node) {
        fprintf(stderr, "\nABORT: Could not allocate sufficient memory for `root_node`.\n");
        return -1;
    }

    if (read_blocks(root_node, root_node_block_addr, 1) == 0) {
        printf("\nEND: Block index %s does not exist in `%s`.\n", argv[2], nx_path);
        return 0;
    }

    printf("validating ... ");
    if (is_cksum_valid(root_node)) {
        printf("OK.\n");
    } else {
        printf("FAILED.\n");
    }
    printf("\n");

    // Allocate space for the current working node,
    // then copy the root node to this space.
    btree_node_phys_t* node = malloc(nx_block_size);
    if (!node) {
        fprintf(stderr, "\nABORT: Could not allocate sufficient memory for `node`.\n");
        return -1;
    }
    memcpy(node, root_node, nx_block_size);

    // Pointers to areas of the node
    char* toc_start = (char*)node->btn_data + node->btn_table_space.off;
    char* key_start = toc_start + node->btn_table_space.len;
    char* val_end   = (char*)node + nx_block_size;
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

        printf("\nNode has %u entries, as follows:\n", node->btn_nkeys);
        kvoff_t* toc_entry = toc_start;
        // Print mapped block's details if explroing a leaf node
        if (node->btn_flags & BTNODE_LEAF) {
            obj_phys_t* block = malloc(nx_block_size);
            if (!block) {
                fprintf(stderr, "\nABORT: Could not allocate sufficient memory for `block`.\n");
                return -1;
            }
            for (uint32_t i = 0;    i < node->btn_nkeys;    i++, toc_entry++) {
                omap_key_t* key = key_start + toc_entry->k;
                omap_val_t* val = val_end   - toc_entry->v;
                if (read_blocks(block, val->ov_paddr, 1) != 1) {
                    fprintf(stderr, "\nABORT: read_blocks: Error reading block %#llx.\n", val->ov_paddr);
                    return -1;
                }

                printf(
                    "- %3u:"
                    "  OID = %#9llx"
                    "  ||  XID = %#9llx"
                    "  ||  Target block = %#9llx"
                    "  ||  Target's actual OID = %#9llx (%s)"
                    "  ||  Target's actual XID = %#9llx (%s)\n", 
                    
                    i,
                    key->ok_oid,
                    key->ok_xid,
                    val->ov_paddr,
                    block->o_oid,   (block->o_oid == key->ok_oid ? "YES  " : "   NO"),
                    block->o_xid,   (block->o_xid == key->ok_xid ? "YES  " : "   NO")
                );
            }
            free(block);
        } else {
            for (uint32_t i = 0;    i < node->btn_nkeys;    i++, toc_entry++) {
                omap_key_t* key = key_start + toc_entry->k;

                printf(
                    "- %3u:"
                    "  OID = %#9llx"
                    "  ||  XID = %#9llx\n",
                    
                    i,
                    key->ok_oid,
                    key->ok_xid
                );
            }
        } 
        
        uint32_t entry_index;
        printf("Choose an entry [0-%u]: ", node->btn_nkeys - 1);
        scanf("%u", &entry_index);
        while (entry_index >= node->btn_nkeys) {
            printf("Invalid choice; choose an entry [0-%u]: ", node->btn_nkeys - 1);
            scanf("%u", &entry_index);
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

        printf("Child node resides at adress 0x%llx. Reading ... ", *child_node_addr);
        if (read_blocks(node, *child_node_addr, 1) != 1) {
            fprintf(stderr, "\nABORT: Failed to read block 0x%llx.\n", *child_node_addr);
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
        val_end   = (char*)node + nx_block_size;    // Always dealing with non-root node here
    }
    
    return 0;
}
