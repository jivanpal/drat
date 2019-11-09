/**
 * A set of functions used to interact with APFS B-trees.
 */

#ifndef APFS_FUNC_BTREE_H
#define APFS_FUNC_BTREE_H

#include <stdbool.h>

#include "../struct/general.h"
#include "../struct/btree.h"
#include "../struct/j.h"
#include "../io.h"

#include "../string/omap.h"
#include "../string/j.h"

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
    // Create a copy of the root node to use as the current node we're working with
    btree_node_phys_t* node = malloc(nx_block_size);
    if (!node) {
        fprintf(stderr, "\nABORT: get_btree_phys_omap_val: Could not allocate sufficient memory for `node`.\n");
        exit(-1);
    }
    memcpy(node, root_node, nx_block_size);

    // Pointers to areas of the node
    char* toc_start = (char*)(node->btn_data) + node->btn_table_space.off;
    char* key_start = toc_start + node->btn_table_space.len;
    char* val_end   = (char*)node + nx_block_size - sizeof(btree_info_t);

    // We'll need access to the B-tree info after discarding our copy of the root node
    btree_info_t* bt_info = malloc(sizeof(btree_info_t));
    if (!bt_info) {
        fprintf(stderr, "\nABORT: get_btree_phys_omap_val: Could not allocate sufficient memory for `bt_info`.\n");
        exit(-1);
    }
    memcpy(bt_info, val_end, sizeof(btree_info_t));

    // Descend the B-tree to find the target key–value pair
    while (true) {
        if (!(node->btn_flags & BTNODE_FIXED_KV_SIZE)) {
            fprintf(stderr, "\nNOTE: Object map B-trees don't have variable size keys and values ... do they?\n");
            
            free(bt_info);
            free(node);
            return NULL;
        }

        // TOC entries are instances of `kvoff_t`
        kvoff_t* toc_entry = toc_start;

        // Find the correct TOC entry, i.e. the last TOC entry whose:
        // - OID doesn't exceed the given OID; or
        // - OID matches the given OID, and XID doesn't exceed the given XID
        for (uint32_t i = 0;    i < node->btn_nkeys;    i++, toc_entry++) {
            omap_key_t* key = key_start + toc_entry->k;
            if (key->ok_oid > oid) {
                toc_entry--;
                break;
            }
            if (key->ok_oid == oid) {
                if (key->ok_xid > max_xid) {
                    toc_entry--;
                    break;
                }
                if (key->ok_xid == max_xid) {
                    break;
                }
            }
        }

        // `toc_entry` now points to the correct TOC entry to use; or
        // it points before `toc_start` if the desired (OID, XID) pair
        // does not exist in this B-tree.
        if ((char*)toc_entry < toc_start) {
            free(bt_info);
            free(node);
            return NULL;
        }

        // If this is a leaf node, return the object map value
        if (node->btn_flags & BTNODE_LEAF) {
            // If the object doesn't have the specified OID, then no sufficient
            // object with that OID exists in the B-tree.
            omap_key_t* key = key_start + toc_entry->k;
            if (key->ok_oid != oid) {
                free(bt_info);
                free(node);
                return NULL;
            }

            omap_val_t* val = val_end - toc_entry->v;

            omap_val_t* return_val = malloc(sizeof(omap_val_t));
            if (!return_val) {
                fprintf(stderr, "\nABORT: get_btree_phys_omap_val: Could not allocate sufficient memory for `return_val`.\n");
                exit(-1);
            }
            memcpy(return_val, val, sizeof(omap_val_t));
            
            free(bt_info);
            free(node);
            return return_val;
        }

        // Else, read the corresponding child node into memory and loop
        paddr_t* child_node_addr = val_end - toc_entry->v;
        
        if (read_blocks(node, *child_node_addr, 1) != 1) {
            fprintf(stderr, "\nABORT: get_btree_phys_omap_val: Failed to read block 0x%llx.\n", *child_node_addr);
            exit(-1);
        }

        if (!is_cksum_valid(node)) {
            fprintf(stderr, "\nABORT: get_btree_phys_omap_val: Checksum of node at block 0x%llx did not validate.\n", *child_node_addr);
            exit(-1);
        }

        toc_start = (char*)(node->btn_data) + node->btn_table_space.off;
        key_start = toc_start + node->btn_table_space.len;
        val_end   = (char*)node + nx_block_size;    // Always dealing with non-root node here
    }
}

/**
 * Custom datatype to store the key and value for a file-system record
 * alongside each other for easier data access and manipulation.
 */
typedef struct {
    uint16_t    key_len;
    uint16_t    val_len;
    char        data[];
} j_key_val_t;

/**
 * NOTE: to future me: It is the caller's responsibility to determine which
 * datatypes are contained within the instance of `j_key_val_t` returned by
 * this function, using the `obj_id_and_type` field of the `j_key_t` instance
 * at the beginning of `j_key_val_t::data`.
 */
j_key_val_t* get_btree_virt_fs_val(btree_node_phys_t* vol_omap_root_node, btree_node_phys_t* vol_fs_root_node, oid_t oid, xid_t max_xid) {
    // Create a copy of the root node to use as the current node we're working with
    btree_node_phys_t* node = malloc(nx_block_size);
    if (!node) {
        fprintf(stderr, "\nABORT: get_btree_virt_fs_val: Could not allocate sufficient memory for `node`.\n");
        exit(-1);
    }
    memcpy(node, vol_fs_root_node, nx_block_size);

    // Pointers to areas of the node
    char* toc_start = (char*)(node->btn_data) + node->btn_table_space.off;
    char* key_start = toc_start + node->btn_table_space.len;
    char* val_end   = (char*)node + nx_block_size - sizeof(btree_info_t);

    // We may need access to the B-tree info after discarding our copy of the root node
    btree_info_t* bt_info = malloc(sizeof(btree_info_t));
    if (!bt_info) {
        fprintf(stderr, "\nABORT: get_btree_virt_fs_val: Could not allocate sufficient memory for `bt_info`.\n");
        exit(-1);
    }
    memcpy(bt_info, val_end, sizeof(btree_info_t));

    // Descend the B-tree to find the target key–value pair
    while (true) {
        if (node->btn_flags & BTNODE_FIXED_KV_SIZE) {
            fprintf(stderr, "\nNOTE: File-system root B-trees don't have fixed size keys and values ... do they?\n");
            
            free(bt_info);
            free(node);
            return NULL;
        }

        // TOC entries are instances of `kvloc_t`
        kvloc_t* toc_entry = toc_start;

        // Find the correct TOC entry, i.e. the last TOC entry whose:
        // - OID doesn't exceed the given OID; or
        // - OID matches the given OID, and XID doesn't exceed the given XID
        for (uint32_t i = 0;    i < node->btn_nkeys;    i++, toc_entry++) {
            j_key_t* key = key_start + toc_entry->k.off;
            oid_t record_oid = key->obj_id_and_type & OBJ_ID_MASK;

            if (record_oid > oid) {
                toc_entry--;
                break;
            }

            if (record_oid == oid) {
                // if (key->ok_xid > max_xid) {
                //     toc_entry--;
                //     break;
                // }
                // if (key->ok_xid == max_xid) {
                //     break;
                // }
                break;
            }
        }

        // `toc_entry` now points to the correct TOC entry to use; or
        // it points before `toc_start` if no records with the
        // desired OID exist in this B-tree.
        if ((char*)toc_entry < toc_start) {
            free(bt_info);
            free(node);
            return NULL;
        }

        // If this is a leaf node, return the object map value
        if (node->btn_flags & BTNODE_LEAF) {
            j_key_t* key = key_start + toc_entry->k.off;
            oid_t record_oid = key->obj_id_and_type & OBJ_ID_MASK;

            // If the object doesn't have the specified OID, then
            // no record with that OID exists in the B-tree.
            if (record_oid != oid) {
                free(bt_info);
                free(node);
                return NULL;
            }

            char* val = val_end - toc_entry->v.off;

            j_key_val_t* return_obj = malloc(sizeof(j_key_val_t) + toc_entry->k.len + toc_entry->v.len);
            if (!return_obj) {
                fprintf(stderr, "\nABORT: get_btree_virt_fs_val: Could not allocate sufficient memory for `return_obj`.\n");
                exit(-1);
            }
            
            return_obj->key_len = toc_entry->k.len;
            return_obj->val_len = toc_entry->v.len;
            memcpy(return_obj->data,                        key,  return_obj->key_len);
            memcpy(return_obj->data + return_obj->key_len,  val,  return_obj->val_len);
            
            free(bt_info);
            free(node);
            return return_obj;
        }

        // Else, read the corresponding child node into memory and loop
        oid_t* child_node_virt_oid = val_end - toc_entry->v.off;
        omap_val_t* child_node_omap_val = get_btree_phys_omap_val(vol_omap_root_node, *child_node_virt_oid, max_xid);
        if (!child_node_omap_val) {
            fprintf(stderr, "get_btree_virt_fs_val: Need to descend to node with Virtual OID 0x%llx, but the file-system object map lists no objects with this Virtual OID.\n", *child_node_virt_oid);
            
            free(bt_info);
            free(node);
            return NULL;
        }
        
        if (read_blocks(node, child_node_omap_val->ov_paddr, 1) != 1) {
            fprintf(stderr, "\nABORT: get_btree_virt_fs_val: Failed to read block 0x%llx.\n", child_node_omap_val->ov_paddr);
            exit(-1);
        }

        if (!is_cksum_valid(node)) {
            fprintf(stderr, "\nABORT: get_btree_virt_fs_val: Checksum of node at block 0x%llx did not validate.\n", child_node_omap_val->ov_paddr);
            exit(-1);
        }

        toc_start = (char*)(node->btn_data) + node->btn_table_space.off;
        key_start = toc_start + node->btn_table_space.len;
        val_end   = (char*)node + nx_block_size;    // Always dealing with non-root node here
    }
}

#endif // APFS_FUNC_BTREE_H
