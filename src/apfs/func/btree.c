/**
 * A set of functions used to interact with APFS B-trees.
 */

#include "btree.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cksum.h"
#include "../struct/j.h"    // j_key_t
#include "../io.h"          // nx_block_size, read_blocks()

/**
 * Get the latest version of an object, up to a given XID, from an object map
 * B-tree that uses Physical OIDs to refer to its child nodes.
 * 
 * root_node:   A pointer to the root node of an object map B-tree that uses
 *      Physical OIDs to refer to its child nodes.
 *      It is the caller's responsibility to ensure that `root_node` satisfies
 *      these criteria; if it does not, behaviour is undefined.
 * 
 * oid:         The Virtual OID of the object to search for in the object map
 *      B-tree whose root node is `root_node`. That is, the result returned will
 *      pertain to an object found in the object map that has the specified OID.
 * 
 * max_xid:     The highest XID to consider for the given OID. That is, the
 *      result returned will pertain to the single object found in the object
 *      map that has the given OID, and also whose XID is the highest among all
 *      objects in the object map that have the same OID but whose XIDs do not
 *      exceed `max_xid`. To consider all XIDs (that is, to just return the
 *      object with the highest XID among those objects with the specified OID),
 *      you can specify `~0` or `-1`.
 * 
 * RETURN VALUE:
 *      A pointer to an object map entry (key/value pair) corresponding to the
 *      unique object whose OID and XID satisfy the criteria described above for
 *      the parameters `oid` and `max_xid`. If no such entry exists, a NULL
 *      pointer is returned. The key-part is included so that the caller see
 *      the XID of the returned entry.
 *      This pointer must be freed when it is no longer needed.
 */
omap_entry_t* get_btree_phys_omap_entry(btree_node_phys_t* root_node, oid_t oid, xid_t max_xid) {
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

    // We'll need access to the B-tree info after discarding our copy of the
    // root node, so create a copy of this info
    btree_info_t* bt_info = malloc(sizeof(btree_info_t));
    if (!bt_info) {
        fprintf(stderr, "\nABORT: get_btree_phys_omap_val: Could not allocate sufficient memory for `bt_info`.\n");
        exit(-1);
    }
    memcpy(bt_info, val_end, sizeof(btree_info_t));

    // Descend the B-tree to find the target key–value pair
    while (true) {
        if (!(node->btn_flags & BTNODE_FIXED_KV_SIZE)) {
            // TODO: Handle this case
            fprintf(stderr, "\nget_btree_phys_omap_val: Object map B-trees don't have variable size keys and values ... do they?\n");
            
            free(bt_info);
            free(node);
            return NULL;
        }

        // TOC entries are instances of `kvoff_t`
        kvoff_t* toc_entry = toc_start;

        /**
         * Find the correct TOC entry, i.e. the last TOC entry whose:
         * - OID doesn't exceed the given OID; or
         * - OID matches the given OID, and XID doesn't exceed the given XID
         */
        for (uint32_t i = 0;    i < node->btn_nkeys;    i++, toc_entry++) {
            omap_key_t* key = key_start + toc_entry->k;
            if (key->ok_oid > oid  ||  (key->ok_oid == oid && key->ok_xid > max_xid)) {
                toc_entry--;
                break;
            }
        }

        /**
         * One of the following is now true about `toc_entry`:
         * 
         * (a) it points before `toc_start` if no matching records exist
         *      in this B-tree.
         * (b) it points directly after the last TOC entry if we should descend
         *      the last node.
         * (c) it points to the correct entry to descend.
         */
        
        // Handle case (a)
        if ((char*)toc_entry < toc_start) {
            free(bt_info);
            free(node);
            return NULL;
        }

        // Convert case (b) into case (c)
        if (toc_entry - (kvoff_t*)toc_start == node->btn_nkeys) {
            toc_entry--;
        }

        // Handle case (c)

        #ifdef DEBUG
        fprintf(stderr, "\n- get_btree_phys_omap_val: Picked entry %lu\n", toc_entry - (kvoff_t*)toc_start);
        #endif

        // If this is a leaf node, return the object map value
        if (node->btn_flags & BTNODE_LEAF) {
            // If the object doesn't have the specified OID or its XID exceeds
            // the specifed maximum, then no matching object exists in the B-tree.
            omap_key_t* key = key_start + toc_entry->k;
            if (key->ok_oid != oid || key->ok_xid > max_xid) {
                free(bt_info);
                free(node);
                return NULL;
            }

            omap_val_t* val = val_end - toc_entry->v;

            omap_entry_t* omap_entry = malloc(sizeof(omap_entry_t));
            if (!omap_entry) {
                fprintf(stderr, "\nABORT: get_btree_phys_omap_val: Could not allocate sufficient memory for `omap_entry`.\n");
                exit(-1);
            }

            memcpy(&(omap_entry->key), key, sizeof(omap_key_t));
            memcpy(&(omap_entry->val), val, sizeof(omap_val_t));
            
            free(bt_info);
            free(node);
            return omap_entry;
        }

        // Else, read the corresponding child node into memory and loop
        paddr_t* child_node_addr = val_end - toc_entry->v;
        
        if (read_blocks(node, *child_node_addr, 1) != 1) {
            fprintf(stderr, "\nABORT: get_btree_phys_omap_val: Failed to read block 0x%llx.\n", *child_node_addr);
            exit(-1);
        }

        if (!is_cksum_valid(node)) {
            fprintf(stderr, "\nget_btree_phys_omap_val: Checksum of node at block 0x%llx did not validate. Proceeding anyway as if it did.\n", *child_node_addr);
        }

        toc_start = (char*)(node->btn_data) + node->btn_table_space.off;
        key_start = toc_start + node->btn_table_space.len;
        val_end   = (char*)node + nx_block_size;    // Always dealing with non-root node here
    }
}

/**
 * Free memory allocated for a file-system records array that
 * was created by a call to `get_btree_virt_fs_records()`.
 * 
 * records_array:   A pointer to an array of pointers to instances of `j_rec_t`,
 *                  as returned by a call to `get_btree_virt_fs_records()`.
 */
void free_j_rec_array(j_rec_t** records_array) {
    if (!records_array) {
        return;
    }

    for (j_rec_t** cursor = records_array; *cursor; cursor++) {
        free(*cursor);
    }
    free(records_array);
}

/**
 * Get an array of all the file-system records with a given Virtual OID from a
 * given file-system root tree.
 * 
 * vol_omap_root_node:
 *      A pointer to the root node of the object map tree of the APFS volume
 *      which the given file-system root tree belongs to. This is needed in
 *      order to resolve the Virtual OIDs of objects listed in the file-system
 *      root tree to their respective block addresses within the APFS container,
 *      so that we can actually find the structures on disk.
 * 
 * vol_fs_root_node:
 *      A pointer to the root node of the file-system root tree.
 * 
 * oid:
 *      The Virtual OID of the desired records to fetch.
 * 
 * max_xid:
 *      The maximum XID to consider for a file-system object/entry. That is,
 *      no object whose XID exceeds this value will be present in the array
 *      of file-system rescords returned by this function.
 * 
 * RETURN VALUE:
 *      A pointer to the head of an array of pointers to instances of `j_rec_t`.
 *      Each such instance of `j_rec_t` describes a file-system record relating
 *      to the file-system object whose Virtual OID is `oid`. The length of this
 *      array is not explicitly returned to the caller, but the last pointer in
 *      the array returned will be a NULL pointer.
 * 
 *      When the data in the array is no longer needed, the pointer that was
 *      returned by this function should be passed to `free_j_rec_array()`
 *      in order to free the memory allocated by this function via internal
 *      calls to `malloc()` and `realloc()`.
 */
j_rec_t** get_fs_records(btree_node_phys_t* vol_omap_root_node, btree_node_phys_t* vol_fs_root_node, oid_t oid, xid_t max_xid) {

    /**
     * `desc_path` describes the path we have taken to descend down the file-
     * system root tree. The value of `desc_path[i]` is the index of the key
     * chosen `i` levels beneath the root level, out of the keys within the
     * node that key was chosen from.
     *
     * Since these B+ trees do not contain pointers to their siblings, this
     * info is needed in order to easily walk the tree after we find the first
     * record with the given OID.
     */
    uint32_t desc_path[vol_fs_root_node->btn_level + 1];
    uint16_t i = 0;     // `i` keeps tracks of how many descents we've made from the root node.
    
    // Initialise the array of records which will be returned to the caller
    size_t num_records = 0;
    j_rec_t** records = malloc(sizeof(j_rec_t*));
    if (!records) {
        fprintf(stderr, "\nABORT: get_fs_records: Could not allocate sufficient memory for `records`.\n");
        exit(-1);
    }
    records[0] = NULL;

    // Create a copy of the root node to use as the current node we're working with
    btree_node_phys_t* node = malloc(nx_block_size);
    if (!node) {
        fprintf(stderr, "\nABORT: get_fs_records: Could not allocate sufficient memory for `node`.\n");
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
        fprintf(stderr, "\nABORT: get_fs_records: Could not allocate sufficient memory for `bt_info`.\n");
        exit(-1);
    }
    memcpy(bt_info, val_end, sizeof(btree_info_t));

    // Find the first (leftmost/least) record in the tree with the given OID
    while (true) {
        if (node->btn_flags & BTNODE_FIXED_KV_SIZE) {
            fprintf(stderr, "\nget_fs_records: File-system root B-trees don't have fixed size keys and values ... do they?\n");
            
            free(bt_info);
            free(node);
            free_j_rec_array(records);
            return NULL;
        }

        // TOC entries are instances of `kvloc_t`
        kvloc_t* toc_entry = toc_start;

        // Determine which entry in this node we should descend
        for (desc_path[i] = 0;    desc_path[i] < node->btn_nkeys;    desc_path[i]++, toc_entry++) {
            j_key_t* key = key_start + toc_entry->k.off;
            oid_t record_oid = key->obj_id_and_type & OBJ_ID_MASK;

            if (record_oid == oid) {
                if (node->btn_flags & BTNODE_LEAF) {
                    // This is the first entry in this leaf node with the given
                    // OID, and thus the first record in the whole tree with
                    // the given OID
                    break;
                }

                // A record with the given OID may exist as a descendant of the
                // previous entry; backtrack so we descend that entry ...
                if (desc_path[i] != 0) {
                    desc_path[i]--;
                    toc_entry--;
                }
                // ... unless this is the first entry in the node, in which
                // case, backtracking is impossible, and this is the entry we
                // should descend
                break;
            }

            if (record_oid > oid) {
                if (node->btn_flags & BTNODE_LEAF) {
                    // If a record with the given OID exists in this leaf node,
                    // we would've encountered it by now. Hence, this leaf node
                    // contains no entries with the given OID, and so no record
                    // with the given OID exists in the whole tree
                    free(bt_info);
                    free(node);
                    free_j_rec_array(records);
                    return NULL;
                }

                // We just passed the entry we want to descend; backtrack
                desc_path[i]--;
                toc_entry--;
                break;
            }
        }

        /**
         * One of the following is now true about `toc_entry`:
         * (a) it points to the correct entry to descend.
         * (b) it points before `toc_start` if no records with the desired OID
         *      exist in this B-tree.
         * (c) it points directly after the last TOC entry; in which case:
         *      (i)  if this is a leaf node, we looked into it because the first
         *              entry of the next node has the desired OID. As such,
         *              don't adjust `desc_path`; it's logical value already
         *              targets the next node, and this iwll be handled by the
         *              next while loop.
         *      (ii) if this is a non-leaf node, we should descend the last 
         *              entry.
         */
        if ((char*)toc_entry < toc_start) {
            free(bt_info);
            free(node);
            free_j_rec_array(records);
            return NULL;
        }
        // If this is a leaf node, then it contains the first record with the
        // given OID, and `desc_path` desribes the path taken to reach this
        // node (unless it describes an out of bounds path, as described in
        // case (c)(i) above). Break from the while-loop so that we can walk
        // along the tree to get the rest of the records with the given OID.
        if (node->btn_flags & BTNODE_LEAF) {
            break;
        }
        if (toc_entry - (kvloc_t*)toc_start == node->btn_nkeys) {
            desc_path[i]--;
            toc_entry--;
        }

        // Else, read the corresponding child node into memory and loop
        oid_t* child_node_virt_oid = val_end - toc_entry->v.off;
        omap_val_t* child_node_omap_val = get_btree_phys_omap_val(vol_omap_root_node, *child_node_virt_oid, max_xid);
        if (!child_node_omap_val) {
            fprintf(stderr, "get_fs_records: Need to descend to node with Virtual OID 0x%llx, but the file-system object map lists no objects with this Virtual OID.\n", *child_node_virt_oid);
            
            free(bt_info);
            free(node);
            free_j_rec_array(records);
            return NULL;
        }
        
        if (read_blocks(node, child_node_omap_val->ov_paddr, 1) != 1) {
            fprintf(stderr, "\nABORT: get_fs_records: Failed to read block 0x%llx.\n", child_node_omap_val->ov_paddr);
            exit(-1);
        }

        if (!is_cksum_valid(node)) {
            fprintf(stderr, "\nABORT: get_fs_records: Checksum of node at block 0x%llx did not validate.\n", child_node_omap_val->ov_paddr);
            exit(-1);
        }

        toc_start = (char*)(node->btn_data) + node->btn_table_space.off;
        key_start = toc_start + node->btn_table_space.len;
        val_end   = (char*)node + nx_block_size;    // Always dealing with non-root node here

        i++;
    }

    /*
     * Now that we've found the first record with the given OID, walk along the
     * tree to get the rest of the records with that OID.
     * 
     * We do so by following `desc_path`, which describes the path to the the
     * next leaf-node entry in order, and then adjusting the values in
     * `desc_path` so that our next descent from the root takes us to the next
     * unvisited leaf-node entry in order.
     * 
     * TODO: This procedure could be optimised by walking along leaf nodes
     * directly rather than making a new descent from the root just to find the
     * sibling of an entry in a leaf node --- HINT: make `desc_path` one entry
     * shorter
     */
    while (true) {
        #ifdef DEBUG
        fprintf(stderr, "(");
        for (i = 0; i < vol_fs_root_node->btn_level; i++) {
            fprintf(stderr, "%u, ", desc_path[i]);
        }
        fprintf(stderr, "%u)\n", desc_path[i]);
        
        if (vol_fs_root_node->btn_level == 3) {

            if ( desc_path[0] == 5
                && desc_path[1] == 12
                && desc_path[2] == 63
                // && desc_path[3] == 0
            ) {
                fprintf(stderr, "Skip!");
                desc_path[2]++;
                desc_path[3] = 0;
                continue;
            }

            if ( desc_path[0] == 5
                && desc_path[1] == 12
                && desc_path[2] == 77
                // && desc_path[3] == 0
            ) {
                fprintf(stderr, "Skip!");
                desc_path[2]++;
                desc_path[3] = 0;
                continue;
            }

            if ( desc_path[0] == 5
                && desc_path[1] == 12
                && desc_path[2] == 78
                // && desc_path[3] == 0
            ) {
                fprintf(stderr, "Skip!");
                desc_path[2]++;
                desc_path[3] = 0;
                continue;
            }
        }
        #endif

        // Reset current node and pointers to the root node
        memcpy(node, vol_fs_root_node, nx_block_size);
        toc_start = (char*)(node->btn_data) + node->btn_table_space.off;
        key_start = toc_start + node->btn_table_space.len;
        val_end   = (char*)node + nx_block_size - sizeof(btree_info_t);

        for (i = 0; i <= vol_fs_root_node->btn_level; i++) {
            if (node->btn_flags & BTNODE_FIXED_KV_SIZE) {
                fprintf(stderr, "\nget_fs_records: File-system root B-trees don't have fixed size keys and values ... do they?\n");
                
                free(bt_info);
                free(node);
                free_j_rec_array(records);
                return NULL;
            }

            if (desc_path[i] >= node->btn_nkeys) {
                // We've already gone through the last entry in this node.

                if (node->btn_flags & BTNODE_ROOT) {
                    // We've gone through the whole tree; return the results
                    free(node);
                    free(bt_info);
                    return records;
                }
                
                // Prep `desc_path` to take us to the leftmost descendant of
                // this node's sibling; then break from this for-loop so that
                // we loop inside the while-loop, i.e. make a new descent from
                // the root.
                desc_path[i - 1]++;
                for (uint16_t j = i; j <= vol_fs_root_node->btn_level; j++) {
                    desc_path[j] = 0;
                }
                break;
            }

            // TOC entries are instances of `kvloc_t`; look at the entry
            // described by `desc_path`
            kvloc_t* toc_entry = (kvloc_t*)toc_start + desc_path[i];

            // If this is a leaf node, we have the next
            // record; add it to the records array
            if (node->btn_flags & BTNODE_LEAF) {
                j_key_t* key = key_start + toc_entry->k.off;
                oid_t record_oid = key->obj_id_and_type & OBJ_ID_MASK;

                if (record_oid != oid) {
                    // This record doesn't have the right OID, so we must have
                    // found all of the relevant records; return the results
                    free(bt_info);
                    free(node);
                    return records;
                }

                char* val = val_end - toc_entry->v.off;

                records[num_records] = malloc(sizeof(j_rec_t) + toc_entry->k.len + toc_entry->v.len);
                if (!records[num_records]) {
                    fprintf(stderr, "\nABORT: get_fs_records: Could not allocate sufficient memory for `records[%lu]`.\n", num_records);
                    exit(-1);
                }
                
                records[num_records]->key_len = toc_entry->k.len;
                records[num_records]->val_len = toc_entry->v.len;
                memcpy(
                    records[num_records]->data,
                    key,
                    records[num_records]->key_len
                );
                memcpy(
                    records[num_records]->data + records[num_records]->key_len,
                    val,
                    records[num_records]->val_len
                );

                num_records++;
                records = realloc(records, (num_records + 1) * sizeof(j_rec_t*));
                if (!records) {
                    fprintf(stderr, "\nABORT: get_fs_records: Could not allocate sufficient memory for `records`.\n");
                }
                records[num_records] = NULL;

                // Prep `desc_path` for the next descent from the root node,
                // then actually start the next descent from the root node.
                desc_path[i]++;
                break;
            }

            // Else, read the corresponding child node into memory and loop
            oid_t* child_node_virt_oid = val_end - toc_entry->v.off;
            omap_val_t* child_node_omap_val = get_btree_phys_omap_val(vol_omap_root_node, *child_node_virt_oid, max_xid);
            if (!child_node_omap_val) {
                fprintf(stderr, "get_fs_records: Need to descend to node with Virtual OID 0x%llx, but the file-system object map lists no objects with this Virtual OID.\n", *child_node_virt_oid);
                
                free(bt_info);
                free(node);
                free_j_rec_array(records);
                return NULL;
            }
            
            if (read_blocks(node, child_node_omap_val->ov_paddr, 1) != 1) {
                fprintf(stderr, "\nABORT: get_fs_records: Failed to read block 0x%llx.\n", child_node_omap_val->ov_paddr);
                exit(-1);
            }

            if (!is_cksum_valid(node)) {
                fprintf(stderr, "\nABORT: get_fs_records: Checksum of node at block 0x%llx did not validate.\n", child_node_omap_val->ov_paddr);
                exit(-1);
            }

            toc_start = (char*)(node->btn_data) + node->btn_table_space.off;
            key_start = toc_start + node->btn_table_space.len;
            val_end   = (char*)node + nx_block_size;    // Always dealing with non-root node here
        }
    }
}
