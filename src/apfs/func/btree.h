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
 * root_node:   A pointer to the root node of an object map B-tree that uses
 *      Physical OIDs to refer to its child nodes.
 *      It is the caller's responsibility to ensure that `root_node` satisfies
 *      these criteria; if it does not, behaviour is undefined.
 * 
 * oid:         The OID of the object search for in the object map B-tree
 *      `root_node`. That is, the result returned will pertain to an object
 *      found in the object map that has the given OID.
 * 
 * max_xid:     The highest XID to consider for the given OID. That is, the
 *      result returned will pertain to the single object found in the object
 *      map that has the given OID, and also whose XID is the highest among all
 *      objects in the object map that have the same OID but whose XIDs do not
 *      exceed `max_xid`.
 * 
 * RETURN VALUE:
 *      A pointer to an object map value corresponding to the unique object
 *      whose OID and XID satisfy the criteria described above for the
 *      parameters `oid` and `max_xid`. If no object exists with the given OID,
 *      a NULL pointer is returned.
 *      This pointer must be freed when it is no longer needed.
 */
omap_val_t* get_btree_phys_omap_val(btree_node_phys_t* root_node, oid_t oid, xid_t max_xid) {
    // Pointers to areas of the node
    char* toc_start = (char*)(root_node->btn_data) + root_node->btn_table_space.off;
    char* key_start = toc_start + root_node->btn_table_space.len;
    char* val_end   = (char*)root_node + nx_block_size - sizeof(btree_info_t);

    // Create a copy of the root node to use as the current node we're working with
    btree_node_phys_t* node = malloc(nx_block_size);
    if (!node) {
        fprintf(stderr, "\nABORT: get_btree_phys_omap_val: Could not allocate sufficient memory for `current_node`.\n");
        exit(-1);
    }
    memcpy(node, root_node, nx_block_size);

    // We'll need access to the B-tree info after discarding our copy of the root node
    btree_info_t* bt_info = malloc(sizeof(btree_info_t));
    if (!bt_info) {
        fprintf(stderr, "\nABORT: get_btree_phys_omap_val: Could not allocate sufficient memory for `bt_info`.\n");
        exit(-1);
    }
    memcpy(bt_info, val_end, sizeof(btree_info_t));

    // This variable will store the highest XID encountered which doesn't exceed `max_xid`.
    xid_t target_xid;
    
    // Descend the B-tree to find the target key–value pair
    while (true) {
        target_xid = 0;

        if (!(node->btn_flags & BTNODE_FIXED_KV_SIZE)) {
            fprintf(stderr, "\nNOTE: Object map B-trees don't have variable size keys and values ... do they?\n");
            return NULL;
        }

        // TOC entries are instances of `kvoff_t`
        kvoff_t* toc_entry = toc_start;


        // For the given OID, determine the highest XID that doesn't exceed `max_xid`
        for (uint32_t i = 0;    i < node->btn_nkeys;    i++, toc_entry++) {
            omap_key_t* key = key_start + toc_entry->k;
            if (key->ok_oid == oid  &&  key->ok_xid <= max_xid) {
                assert(key->ok_xid > target_xid);
                target_xid = key->ok_xid;
            }
        }


        // If `xid` remained unchanged, then we didn't encounter an object
        // with the given OID; return NULL
        if (target_xid == 0) {
            return NULL;
        }

        // We now have the target XID, and can find the corresponding child node or object map value
        toc_entry = toc_start;
        for (uint32_t i = 0;    i < node->btn_nkeys;    i++, toc_entry++) {
            omap_key_t* key = key_start + toc_entry->k;

            if (key->ok_oid == oid  &&  key->ok_xid == target_xid) {
                // We have found the desired key–value pair; break from this
                // for-loop, and `toc_entry` will point to the correct TOC entry
                break;
            }
        }

        // If this is a leaf node, return the object map value
        if (node->btn_flags & BTNODE_LEAF) {
            omap_val_t* val = val_end - toc_entry->v;

            omap_val_t* return_val = malloc(sizeof(omap_val_t));
            memcpy(return_val, val, sizeof(omap_val_t));
            
            return return_val;
        }

        // Else, read the correspoinding child node into memory and loop
        paddr_t* child_node_addr = val_end - toc_entry->v;
        
        if (read_blocks(node, *child_node_addr, 1) != 1) {
            fprintf(stderr, "\nABORT: get_btree_phys_omap_val: Failed to read block 0x%llx.\n", *child_node_addr);
            exit(-1);
        }
    }
}

#endif // APFS_FUNC_BTREE_H
