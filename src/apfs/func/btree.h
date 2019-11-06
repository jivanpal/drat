/**
 * A set of functions used to interact with APFS B-trees.
 */

#ifndef APFS_FUNC_BTREE_H
#define APFS_FUNC_BTREE_H

#include <stdbool.h>

#include "../struct/general.h"
#include "../struct/btree.h"
#include "../io.h"

#include "../string/omap.h"

/**
 * Get the latest version of an object, up to a given XID, from an object map
 * B-tree that uses Physical OIDs to refer to its child nodes.
 * 
 * btn:     A pointer to the root node of an object map B-tree that uses
 *      Physical OIDs to refer to its child nodes.
 *      It is the caller's responsibility to ensure that `btn` satisfies
 *      these criteria; if it does not, behaviour is undefined.
 * 
 * oid:     The OID of the object search for in the object map B-tree `btn`.
 *      That is, the result returned will pertain to an object found in the
 *      object map that has the given OID.
 * 
 * max_xid: The highest XID to consider for the given OID. That is, the result
 *      returned will pertain to the single object found in the object map that
 *      has the given OID, and also whose XID is the highest among all objects
 *      in the object map that have the same OID but whose XIDs do not exceed
 *      `max_xid`.
 * 
 * RETURN VALUE:
 *      A pointer to an object map value corresponding to the unique object
 *      whose OID and XID satisfy the criteria described above for the
 *      parameters `oid` and `max_xid`.
 *      This pointer must be freed when it is no longer needed.
 */
omap_val_t* get_btree_phys_omap_val(btree_node_phys_t* btn, oid_t oid, xid_t max_xid) {
    btree_node_phys_t* current_node = malloc(nx_block_size);
    if (!current_node) {
        fprintf(stderr, "\nABORT: btree_physical_get_val: Could not allocate sufficient memory for `current_node`.\n");
        exit(-1);
    }
    
    memcpy(current_node, btn, nx_block_size);

    printf("Location offsets relative to the start of this block:\n");
    printf("- Table of contents:    0x%lx\n",   sizeof(btree_node_phys_t) + btn->btn_table_space.off);
    printf("- Keys area:            0x%lx\n",   sizeof(btree_node_phys_t) + btn->btn_table_space.off + btn->btn_table_space.len);
    printf("- End of values area:   0x%lx\n",   nx_block_size - sizeof(btree_info_t));

    char* toc_start = (char*)(btn->btn_data) + btn->btn_table_space.off;
    char* key_start = toc_start + btn->btn_table_space.len;
    char* val_end   = (char*)btn + nx_block_size - sizeof(btree_info_t);

    bool fixed_kv_size = btn->btn_flags & BTNODE_FIXED_KV_SIZE;
    
    if (fixed_kv_size) {
        // TOC entries are instances of `kvoff_t`
        kvoff_t* toc_entry = toc_start;

        btree_info_t* bt_info = val_end;

        printf("Key size:   %u bytes\n",  bt_info->bt_fixed.bt_key_size);
        printf("Value size: %u bytes\n",  bt_info->bt_fixed.bt_val_size);

        omap_key_t* key = malloc(bt_info->bt_fixed.bt_key_size);
        if (!key) {
            fprintf(stderr, "\nABORT: get_btree_phys_omap_val: Could not allocate sufficient memory for `key` in fixed-size case.\n");
            exit(-1);
        }

        omap_val_t* val = malloc(bt_info->bt_fixed.bt_val_size);
        if (!val) {
            fprintf(stderr, "\nABORT: get_btree_phys_omap_val: Could not allocate sufficient memory for `val` in fixed-size case.\n");
            exit(-1);
        }

        for (uint32_t i = 0; i < btn->btn_nkeys; i++, toc_entry++) {
            memcpy(key,  key_start + toc_entry->k,  bt_info->bt_fixed.bt_key_size);
            memcpy(val,  val_end   - toc_entry->v,  bt_info->bt_fixed.bt_val_size);

            printf("Key-value pair #%u:\n", i);
            
            printf("- Key data:\n");
            print_omap_key(key);

            printf("- Value data:\n");
            print_omap_val(val);
        }

        free(key);
        free(val);
    } else {
        fprintf(stderr, "\nbtree_phys_omap_get_val: Ability to handle trees with non-fixed size for keys and values is not yet implemented.\n");

        // // TOC entries are instances of `kvloc_t`
        // kvloc_t* toc_entry = toc_start;

        // for (uint32_t i = 0; i < btn->btn_nkeys; i++, toc_entry++) {
        //     unsigned char* key = malloc(toc_entry->k.len);
        //     if (!key) {
        //         fprintf(stderr, "\nABORT: btree_physical_get_val: Could not allocate sufficient memory for `key` in variable-size case.\n");
        //         exit(-1);
        //     }

        //     unsigned char* val = malloc(toc_entry->v.len);
        //     if (!val) {
        //         fprintf(stderr, "\nABORT: btree_physical_get_val: Could not allocate sufficient memory for `val` in variable-size case.\n");
        //         exit(-1);
        //     }
            
        //     memcpy(key,  key_start + toc_entry->k.off,  toc_entry->k.len);
        //     memcpy(val,  val_end   - toc_entry->v.off,  toc_entry->v.len);
            
        //     printf("Key–value pair #%u:\n", i);
        //     printf("- Key's offset in key area:                         0x%x\n", toc_entry->k.off);
        //     printf("- Value's reverse offset from end of value area:    0x%x\n", toc_entry->v.off);
        //     printf("- Key length:       %u bytes\n",    toc_entry->k.len);
        //     printf("- Value length:     %u bytes\n",    toc_entry->v.len);

        //     printf("- Key data (hex):   ");
        //     for (uint32_t j = toc_entry->k.len; j > 0; /* nothing */) {
        //         if (j % 2 == 0) {
        //             printf(" ");
        //         }
        //         printf("%02x", key[--j]);
        //     }
        //     printf("\n");

        //     printf("- Value data (hex): ");
        //     for (uint32_t j = toc_entry->v.len; j > 0; /* nothing */) {
        //         printf("%02x", val[--j]);
        //         if (j % 2 == 0) {
        //             printf(" ");
        //         }
        //     }
        //     printf("\n");

        //     free(key);
        //     free(val);
        // }
    }

    // while(btn->btn_level != 0) {
        // Go through node to find child node corresponding to chosen key.
    // }

    return NULL;
}

#endif // APFS_FUNC_BTREE_H
