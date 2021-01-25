#ifndef APFS_FUNC_BTREE_H
#define APFS_FUNC_BTREE_H

#include "../struct/btree.h"
#include "../struct/omap.h"

omap_val_t* get_btree_phys_omap_val(btree_node_phys_t* root_node, oid_t oid, xid_t max_xid);

/**
 * Custom data structure used to store a full file-system record (i.e. a single
 * key–value pair from a file-system root tree) alongside each other for easier
 * data access and manipulation.
 * 
 * One can make use of an instance of this datatype by determining the strctures
 * contained within its `data` field by appealing to the `obj_id_and_type` field
 * of the `j_key_t` structure for the record, which is guaranteed to exist and
 * start at `data[0]`. That is, a pointer to this instance of `j_key_t` can be
 * obtained with `j_key_t* record_header = record->data`, where `record` is an
 * instance of this type, `j_rec_t`.
 * 
 * key_len: Length of the file-system record's key-part, in bytes.
 * 
 * val_len: Length of the file-system record's value-part, in bytes.
 * 
 * data:    Array of `key_len + val_len` bytes of data, of which,
 *          index 0 (inclusive) through `key_len` (exclusive) contain the
 *          key-part data, and index `key_len` (inclusive) through
 *          `key_len + val_len` (exclusive) contain the value-part data.
 */
typedef struct {
    uint16_t    key_len;
    uint16_t    val_len;
    char        data[];
} j_rec_t;

void free_j_rec_array(j_rec_t** records_array);

j_rec_t** get_fs_records(btree_node_phys_t* vol_omap_root_node, btree_node_phys_t* vol_fs_root_node, oid_t oid, xid_t max_xid);

#endif // APFS_FUNC_BTREE_H
