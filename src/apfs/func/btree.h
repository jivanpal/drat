/**
 * A set of functions used to interact with APFS B-trees.
 */

#ifndef APFS_FUNC_BTREE_H
#define APFS_FUNC_BTREE_H

#include <stdbool.h>

#include "../struct/btree.h"
#include "../io.h"

/**
 * Given a B-tree that uses Physical OIDs to refer to its child nodes, and
 * given a key for that B-tree, get the associated value.
 * 
 * btn:     A pointer to a B-tree root node. It is the caller's responsibility
 *      to ensure that `btn` uses Physical OIDs to refer to its child nodes.
 *      If it does not, behaviour is undefined.
 * 
 * key:     The key to search for in the node `btn`.
 * 
 * RETURN VALUE:
 *      The value associated with the given key in the given B-tree.
 */
uint64_t btree_physical_get_val(btree_node_phys_t* btn, uint64_t key) {
    btree_node_phys_t* current_node = malloc(nx_block_size);
    if (!current_node) {
        fprintf(stderr, "\nABORT: btree_physical_get_val: Could not allocate sufficient memory for `current_node`.\n");
        exit(-1);
    }
    
    memcpy(current_node, btn, nx_block_size);

    char* toc_start = (char*)(btn->btn_data) + btn->btn_table_space.off;
    char* key_start = toc_start + btn->btn_table_space.len;
    char* val_end   = (char*)btn + nx_block_size - sizeof(btree_info_t);

    printf("TOC offset:         0x%lx\n", sizeof(btree_node_phys_t) + btn->btn_table_space.off);
    printf("Key area offset:    0x%lx\n", sizeof(btree_node_phys_t) + btn->btn_table_space.off + btn->btn_table_space.len);
    printf("Value area offset (end): ... \n");

    bool fixed_kv_size = btn->btn_flags & BTNODE_FIXED_KV_SIZE;
    
    char* prev_key = NULL;
    char* this_key = NULL;

    if (fixed_kv_size) {
        // TOC entries are instances of `kvoff_t`
        kvoff_t* toc_entry = toc_start;

        btree_info_t* bt_info = val_end;

        printf("Key size:   %u bytes\n",    bt_info->bt_fixed.bt_key_size);
        printf("Value size: %u bytes\n",    bt_info->bt_fixed.bt_val_size);

        unsigned char* key = malloc(bt_info->bt_fixed.bt_key_size);
        if (!key) {
            fprintf(stderr, "\nABORT: btree_physical_get_val: Could not allocate sufficient memory for `key` in fixed-size case.\n");
            exit(-1);
        }

        unsigned char* val = malloc(bt_info->bt_fixed.bt_val_size);
        if (!val) {
            fprintf(stderr, "\nABORT: btree_physical_get_val: Could not allocate sufficient memory for `val` in fixed-size case.\n");
            exit(-1);
        }

        for (uint32_t i = 0; i < btn->btn_nkeys; i++, toc_entry++) {
            memcpy(key,  key_start + toc_entry->k,  bt_info->bt_fixed.bt_key_size);
            memcpy(val,  val_end   - toc_entry->v,  bt_info->bt_fixed.bt_val_size);

            printf("Key–value pair #%u:\n", i);
            printf("- Key's offset in key area:                         0x%x\n", toc_entry->k);
            printf("- Value's reverse offset from end of value area:    0x%x\n", toc_entry->v);

            printf("- Key data (hex):   ");
            // Going through array `key` in reverse accounts for little-endian byte order
            for (uint32_t j = bt_info->bt_fixed.bt_key_size; j > 0; /* nothing */) {
                printf("%02x", key[--j]);
                // Print data in 4-nibble (2-byte) chunks
                if (j % 2 == 0) {
                    printf(" ");
                }
            }
            printf("\n");

            printf("- Value data (hex): ");
            for (uint32_t j = bt_info->bt_fixed.bt_val_size; j > 0; /* nothing */) {
                printf("%02x", val[--j]);
                if (j % 2 == 0) {
                    printf(" ");
                }
            }
            printf("\n");
        }

        free(key);
        free(val);
    } else {
        // TOC entries are instances of `kvloc_t`
        kvloc_t* toc_entry = toc_start;

        for (uint32_t i = 0; i < btn->btn_nkeys; i++, toc_entry++) {
            unsigned char* key = malloc(toc_entry->k.len);
            if (!key) {
                fprintf(stderr, "\nABORT: btree_physical_get_val: Could not allocate sufficient memory for `key` in variable-size case.\n");
                exit(-1);
            }

            unsigned char* val = malloc(toc_entry->v.len);
            if (!val) {
                fprintf(stderr, "\nABORT: btree_physical_get_val: Could not allocate sufficient memory for `val` in variable-size case.\n");
                exit(-1);
            }

            memcpy(key,  key_start + toc_entry->k.off,  toc_entry->k.len);
            memcpy(val,  val_end   - toc_entry->v.off,  toc_entry->v.len);
            
            printf("Key–value pair #%u:\n", i);
            printf("- Key's offset in key area:                         0x%x\n", toc_entry->k.off);
            printf("- Value's reverse offset from end of value area:    0x%x\n", toc_entry->v.off);
            printf("- Key length:       %u bytes\n",    toc_entry->k.len);
            printf("- Value length:     %u bytes\n",    toc_entry->v.len);

            printf("- Key data (hex):   ");
            for (uint32_t j = toc_entry->k.len; j > 0; /* nothing */) {
                if (j % 2 == 0) {
                    printf(" ");
                }
                printf("%02x", key[--j]);
            }
            printf("\n");

            printf("- Value data (hex): ");
            for (uint32_t j = toc_entry->v.len; j > 0; /* nothing */) {
                printf("%02x", val[--j]);
                if (j % 2 == 0) {
                    printf(" ");
                }
            }
            printf("\n");

            free(key);
            free(val);
        }
    }

    // while(btn->btn_level != 0) {
        // Go through node to find child node corresponding to chosen key.
    // }

    return 0;
}

#endif // APFS_FUNC_BTREE_H
