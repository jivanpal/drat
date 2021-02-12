/**
 * A set of functions used to interact with APFS B-trees.
 */

#include "btree.h"

#include <inttypes.h>
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
    btree_info_t* bt_info = (char*)root_node + nx_block_size - sizeof(btree_info_t);
    
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

    // Descend the B-tree to find the target key–value pair
    while (true) {
        if (!(node->btn_flags & BTNODE_FIXED_KV_SIZE)) {
            // TODO: Handle this case
            fprintf(stderr, "\nget_btree_phys_omap_val: Object map B-trees don't have variable size keys and values ... do they?\n");
            
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
         *      the last entry.
         * (c) it points to the correct entry to descend.
         */
        
        // Handle case (a)
        if ((char*)toc_entry < toc_start) {
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

        // If this is a leaf node, return the object map entry
        if (node->btn_flags & BTNODE_LEAF) {
            // If the object doesn't have the specified OID or its XID exceeds
            // the specifed maximum, then no matching object exists in the B-tree.
            omap_key_t* key = key_start + toc_entry->k;
            if (key->ok_oid != oid || key->ok_xid > max_xid) {
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
            
            free(node);
            return omap_entry;
        }

        // Else, read the corresponding child node into memory and loop
        paddr_t* child_node_addr = val_end - toc_entry->v;
        
        if (read_blocks(node, *child_node_addr, 1) != 1) {
            fprintf(stderr, "\nABORT: get_btree_phys_omap_val: Failed to read block 0x%" PRIx64 ".\n", *child_node_addr);
            exit(-1);
        }

        if (!is_cksum_valid(node)) {
            fprintf(stderr, "\nget_btree_phys_omap_val: Checksum of node at block 0x%" PRIx64 " did not validate. Proceeding anyway as if it did.\n", *child_node_addr);
        }

        toc_start = (char*)(node->btn_data) + node->btn_table_space.off;
        key_start = toc_start + node->btn_table_space.len;
        val_end   = (char*)node + nx_block_size;    // Always dealing with non-root node here
    }
}

/**
 * Free memory allocated for a file-system records array that
 * was created by a call to `get_fs_records()`.
 * 
 * records_array:   A pointer to an array of pointers to instances of `j_rec_t`,
 *                  as returned by a call to `get_fs_records()`.
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
 *      A pointer to the root node of the object map B-tree of the APFS volume
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
 *      of file-system rescords returned by this function. To consider all XIDs,
 *      specify `~0` or `-1`.
 * 
 * RETURN VALUE:
 *      A pointer to the head of an array of pointers to instances of `j_rec_t`.
 *      Each such instance of `j_rec_t` describes a file-system record relating
 *      to the file-system object whose Virtual OID is `oid`. The length of this
 *      array is not explicitly returned to the caller, but the last pointer in
 *      the array returned will be a NULL pointer.
 * 
 *      When the data in the array is no longer needed, it is the caller's
 *      responsibility to free the associated memory by passing the pointer
 *      that was returned by this function to `free_j_rec_array()`.
 */
j_rec_t** get_fs_records(btree_node_phys_t* vol_omap_root_node, btree_node_phys_t* vol_fs_root_node, oid_t oid, xid_t max_xid) {
    btree_info_t* bt_info = (char*)vol_fs_root_node + nx_block_size - sizeof(btree_info_t);

    uint16_t tree_height = vol_fs_root_node->btn_level + 1;

    /**
     * `desc_path` describes the path we have taken to descend down the file-
     * system root tree. Since these B+ trees do not contain pointers to their
     * siblings, this info is needed in order to easily walk the tree after we
     * find the first record with the given OID.
     * 
     * The value of `desc_path[i]` is the index of the key
     * chosen `i` levels beneath the root level, out of the keys within the
     * node that key was chosen from. The length of `desc_path` is the height
     * of the tree.
     * 
     * For example, suppose the height of the tree is 4, and:
     * 
     * 1. we descend to the 1st child (index 0) of the root node (level 3); then
     * 2. we descend to the 3rd child (index 2) of that level 2 node; then
     * 3. we descend to the 2nd child (index 1) of that level 1 node; then
     * 4. we are currently looking at the 4th entry (index 3) of that leaf node;
     * 
     * then `desc_path` will be equal to the array `{0, 2, 1, 3}`.
     */
    uint32_t desc_path[tree_height];

    /**
     * Let `node` be the working node (the FS tree node that we're currently
     * looking at), and copy the FS tree's root node there. We do this so that
     * when we look at child nodes later, we can copy them to `node` without
     * losing access to the root node, an instance of which will still be
     * present as `vol_fs_root_node`.
     */
    btree_node_phys_t* node = malloc(nx_block_size);
    if (!node) {
        fprintf(stderr, "\nABORT: get_fs_records: Could not allocate sufficient memory for `node`.\n");
        exit(-1);
    }
    memcpy(node, vol_fs_root_node, nx_block_size);

    // Pointers to areas of the working node
    char* toc_start = (char*)(node->btn_data) + node->btn_table_space.off;
    char* key_start = toc_start + node->btn_table_space.len;
    char* val_end   = (char*)node + nx_block_size - sizeof(btree_info_t);

    /**
     * DESCENT LOOP
     * Descend the tree to find the first record in the tree with the desired OID.
     */
    for (uint16_t i = 0; i < tree_height; i++) {
        if (node->btn_flags & BTNODE_FIXED_KV_SIZE) {
            // TODO: Handle this case
            fprintf(stderr, "\nget_fs_records: File-system root B-trees don't have fixed size keys and values ... do they?\n");
            exit(-1);
        }

        // TOC entries are instances of `kvloc_t`
        kvloc_t* toc_entry = toc_start;

        /**
         * Determine which entry in this node we should descend; we break from
         * this loop once we have determined the right entry or looked at all
         * of them.
         */
        for (desc_path[i] = 0;    desc_path[i] < node->btn_nkeys;    desc_path[i]++, toc_entry++) {
            j_key_t* key = key_start + toc_entry->k.off;
            oid_t record_oid = key->obj_id_and_type & OBJ_ID_MASK;

            /** Handle leaf nodes **/

            if (node->btn_flags & BTNODE_LEAF) {
                if (record_oid == oid) {
                    /**
                     * This is the first matching record, and `desc_path`
                     * now describes the path to it in the tree.
                     */
                    break;
                }

                if (record_oid > oid) {
                    /**
                     * If a record with the desired OID existed, we would've
                     * encountered it by now, so no such records exist.
                     */
                    free(node);
                    return NULL;
                }

                assert(record_oid < oid);
                continue;   // Look at the next record
            }

            /** Handle non-leaf nodes **/
            assert(!(node->btn_flags & BTNODE_LEAF));

            if (record_oid >= oid) {
                /**
                 * We've encountered the first entry in this non-leaf node
                 * whose key states an OID that is greater than or equal to the
                 * desired OID. Thus, if this *isn't* the first entry in this
                 * node, we descend the previous entry, as a record with the
                 * desired OID may exist in that sub-tree.
                 */
                if (desc_path[i] != 0) {
                    desc_path[i]--;
                    toc_entry--;
                    break;
                }

                /**
                 * However, if this *is* the first entry in this node, we only
                 * descend it if its key's stated OID matches the desired OID;
                 * else it exceeds the desired OID, and thus no records with the
                 * desired OID exist *in the whole tree*.
                 */
                if (record_oid == oid) {
                    break;
                }
                
                free(node);
                return NULL;
            }

            assert(record_oid < oid);
            // Implicit `continue`; look at the next entry.
        }

        /**
         * One of the following is now true about `toc_entry`:
         * 
         * (a) it points directly after the last TOC entry, in which case:
         *      (i)  if this is a leaf node, we're looking at it because the
         *              first record in the *next* leaf node has the desired
         *              OID, or no records with the desired OID exist in the
         *              whole tree. We just break from the descent loop, and the
         *              walk loop will handle the current value of `desc_path`
         *              correctly.
         *      (ii) if this is a non-leaf node, we should descend the last 
         *              entry.
         * (b) it points to the correct entry to descend.
         */


        /**
         * If this is a leaf node, then we have finished descending the tree,
         * and `desc_path` describes the path to the first record with the
         * desired OID. We break from this while-loop (the descent loop) and
         * enter the next while-loop (the walk loop), which should behave
         * correctly based on the vale of `desc_path`.
         * 
         * This handles case (a)(i) above, and also case (b) when we're looking
         * at a leaf node.
         */
        if (node->btn_flags & BTNODE_LEAF) {
            break;
        }

        /** Convert case (a)(ii) to case (b) */
        if (toc_entry - (kvloc_t*)toc_start == node->btn_nkeys) {
            desc_path[i]--;
            toc_entry--;
        }

        /**
         * Else, read the corresponding child node into memory and loop.
         * This handles case (b) when we're looking at a non-leaf node.
         */

        oid_t* child_node_virt_oid = val_end - toc_entry->v.off;
        omap_entry_t* child_node_omap_entry = get_btree_phys_omap_entry(vol_omap_root_node, *child_node_virt_oid, max_xid);
        if (!child_node_omap_entry) {
            fprintf(stderr, "\nABORT: get_fs_records: Need to descend to node with Virtual OID 0x%" PRIx64 ", but the volume object map lists no objects with this Virtual OID.\n", *child_node_virt_oid);
            exit(-1);
        }
        
        if (read_blocks(node, child_node_omap_entry->val.ov_paddr, 1) != 1) {
            fprintf(stderr, "\nABORT: get_fs_records: Failed to read block 0x%" PRIx64 ".\n", child_node_omap_entry->val.ov_paddr);
            exit(-1);
        }

        // `node` is now the child node we will scan on next loop

        if (!is_cksum_valid(node)) {
            fprintf(stderr, "\nABORT: get_fs_records: Checksum of node at block 0x%" PRIx64 " did not validate.\n", child_node_omap_entry->val.ov_paddr);
            exit(-1);
        }

        toc_start = (char*)(node->btn_data) + node->btn_table_space.off;
        key_start = toc_start + node->btn_table_space.len;
        val_end   = (char*)node + nx_block_size;    // Always dealing with non-root node here
    }

    // Initialise the array of records which will be returned to the caller
    size_t num_records = 0;
    j_rec_t** records = malloc(sizeof(j_rec_t*));
    if (!records) {
        fprintf(stderr, "\nABORT: get_fs_records: Could not allocate sufficient memory for `records`.\n");
        exit(-1);
    }
    records[0] = NULL;

    /**
     * WALK LOOP
     * 
     * Now that we've found the first record with the given OID, walk along the
     * tree to get the rest of the records with that OID.
     * 
     * We do so by following `desc_path`, which describes the descent path to a
     * record in the tree, and then adjusting the value of `desc_path` so that
     * it refers to the next record in tree, so that when we loop, we visit
     * that next record.
     */
    while (true) {
        #ifdef DEBUG
        // Print the contents of `desc_path`
        fprintf(stderr, "(");
        for (i = 0; i < tree_height - 1; i++) {
            fprintf(stderr, "%u, ", desc_path[i]);
        }
        fprintf(stderr, "%u)\n", desc_path[i]);
        #endif

        // Reset working node and pointers to the root node
        memcpy(node, vol_fs_root_node, nx_block_size);
        toc_start = (char*)(node->btn_data) + node->btn_table_space.off;
        key_start = toc_start + node->btn_table_space.len;
        val_end   = (char*)node + nx_block_size - sizeof(btree_info_t);

        // Descend to the record described by `desc_path`.
        for (uint16_t i = 0; i < tree_height; i++) {
            if (node->btn_flags & BTNODE_FIXED_KV_SIZE) {
                // TODO: Handle this case
                fprintf(stderr, "\nget_fs_records: File-system root B-trees don't have fixed size keys and values ... do they?\n");
                
                free(node);
                free_j_rec_array(records);
                return NULL;
            }

            /**
             * If `desc_path[i]` isn't a valid entry index in this node, that
             * means we've already looked at all the entries in this node, and
             * should look at the next node on this level.
             */
            if (desc_path[i] >= node->btn_nkeys) {
                /**
                 * If this is a root node, then there are no other nodes on this
                 * level; we've gone through the whole tree, return the results.
                 */
                if (node->btn_flags & BTNODE_ROOT) {
                    free(node);
                    return records;
                }
                
                /**
                 * Else, we adjust the value of `desc_path` so that it refers
                 * to the leftmost descendant of the next node on this level.
                 * We then break from this for-loop so that we loop inside the
                 * while-loop (the walk loop), which will result in us making
                 * a new descent from the root based on the new value of
                 * `desc_path`.
                 */
                desc_path[i - 1]++;
                for (uint16_t j = i; j < tree_height; j++) {
                    desc_path[j] = 0;
                }
                break;
            }

            /**
             * Handle leaf nodes:
             * The entry we're looking at is the next record, so add it to the
             * records array, then adjust `desc_path` and loop.
             */
            if (node->btn_flags & BTNODE_LEAF) {
                // TOC entries are instances of `kvloc_t`.
                // Walk along this leaf node
                for (
                    kvloc_t* toc_entry = (kvloc_t*)toc_start + desc_path[i];
                    desc_path[i] < node->btn_nkeys;
                    desc_path[i]++, toc_entry++
                ) {
                    j_key_t* key = key_start + toc_entry->k.off;
                    oid_t record_oid = key->obj_id_and_type & OBJ_ID_MASK;

                    if (record_oid != oid) {
                        // This record doesn't have the right OID, so we must have
                        // found all of the relevant records; return the results
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
                    memcpy(records[num_records]->data,                                  key,  records[num_records]->key_len);
                    memcpy(records[num_records]->data + records[num_records]->key_len,  val,  records[num_records]->val_len);

                    num_records++;

                    records = realloc(records, (num_records + 1) * sizeof(j_rec_t*));
                    if (!records) {
                        fprintf(stderr, "\nABORT: get_fs_records: Could not allocate sufficient memory for `records`.\n");
                        exit(-1);
                    }
                    records[num_records] = NULL;
                }

                /**
                 * We've run off the end of this leaf node, and `desc_path` now
                 * refers to the first record of the next leaf node.
                 * Loop so that we correctly make a new descent to that record
                 * from the root node.
                 */
                break;
            }

            /**
             * Handle non-leaf nodes:
             * Read the child node that this entry points to, then loop.
             */
            assert(!(node->btn_flags & BTNODE_LEAF));

            // We look at the TOC entry corresponding to the child node we need
            // to descend to.

            // TOC entries are instances of `kvloc_t`.
            kvloc_t* toc_entry = (kvloc_t*)toc_start + desc_path[i];

            oid_t* child_node_virt_oid = val_end - toc_entry->v.off;
            omap_entry_t* child_node_omap_entry = get_btree_phys_omap_entry(vol_omap_root_node, *child_node_virt_oid, max_xid);
            if (!child_node_omap_entry) {
                fprintf(stderr, "\nABORT: get_fs_records: Need to descend to node with Virtual OID 0x%" PRIx64 " and maximum XID 0x%" PRIx64 ", but the volume object map lists no such objects.\n", *child_node_virt_oid, max_xid);
                exit(-1);
            }
            
            if (read_blocks(node, child_node_omap_entry->val.ov_paddr, 1) != 1) {
                fprintf(stderr, "\nABORT: get_fs_records: Failed to read block 0x%" PRIx64 ".\n", child_node_omap_entry->val.ov_paddr);
                exit(-1);
            }

            // `node` is now the child node that we will examine on next loop

            if (!is_cksum_valid(node)) {
                fprintf(stderr, "\nABORT: get_fs_records: Checksum of node at block 0x%" PRIx64 " did not validate.\n", child_node_omap_entry->val.ov_paddr);
                exit(-1);
            }

            toc_start = (char*)(node->btn_data) + node->btn_table_space.off;
            key_start = toc_start + node->btn_table_space.len;
            val_end   = (char*)node + nx_block_size;    // Always dealing with non-root node here
        }
    }
}
