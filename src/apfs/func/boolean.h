/**
 * Boolean functions to check whether a given condition holds.
 */

#ifndef APFS_FUNC_BOOLEAN_H
#define APFS_FUNC_BOOLEAN_H

#include <stdbool.h>
#include "../struct/object.h"

/**
 * Determine whether a given APFS object is of the Physical storage type.
 */
bool is_physical(obj_phys_t* obj) {
    return (obj->o_type & OBJ_STORAGETYPE_MASK) == OBJ_PHYSICAL;
}

/**
 * Determine whether a given APFS object is of the Ephemeral storage type.
 */
bool is_ephemeral(obj_phys_t* obj) {
    return (obj->o_type & OBJ_STORAGETYPE_MASK) == OBJ_EPHEMERAL;
}

/**
 * Determine whether a given APFS object is of the Virtual storage type.
 */
bool is_virtual(obj_phys_t* obj) {
    return (obj->o_type & OBJ_STORAGETYPE_MASK) == OBJ_VIRTUAL;
}

/**
 * Determine whether a given APFS block is a container superblock based on the
 * type flag in its header.
 */
bool is_nx_superblock(obj_phys_t* obj) {
    return (obj->o_type & OBJECT_TYPE_MASK) == OBJECT_TYPE_NX_SUPERBLOCK;
}

/**
 * Determine whether a given APFS block is a checkpoint mapping block based on
 * the type flag in its header.
 */
bool is_checkpoint_map_phys(obj_phys_t* obj) {
    return (obj->o_type & OBJECT_TYPE_MASK) == OBJECT_TYPE_CHECKPOINT_MAP;
}

/**
 * Determine whether a given APFS object is the root node of a B-tree.
 */
bool is_btree_node_phys_root(obj_phys_t* obj) {
    return (obj->o_type & OBJECT_TYPE_MASK) == OBJECT_TYPE_BTREE;
}

/**
 * Determine whether a given APFS object is a non-root node of a B-tree.
 */
bool is_btree_node_phys_non_root(obj_phys_t* obj) {
    return (obj->o_type & OBJECT_TYPE_MASK) == OBJECT_TYPE_BTREE_NODE;
}

/**
 * Determine whether a given APFS object is a B-tree node (regardless of
 * whether it is a root node or otherwise).
 */
bool is_btree_node_phys(obj_phys_t* obj) {
    return is_btree_node_phys_root(obj) || is_btree_node_phys_non_root(obj);
}

/**
 * Determine whether a given APFS object has the subtype corresponding to an
 * object map tree.
 */
bool is_omap_tree(obj_phys_t* obj) {
    return obj->o_subtype == OBJECT_TYPE_OMAP;
}

/**
 * Determine whether a given APFS object has the subtype corresponding to a
 * file-system records tree.
 */
bool is_fs_tree(obj_phys_t* obj) {
    return obj->o_subtype == OBJECT_TYPE_FSTREE;
}

#endif // APFS_FUNC_BOOLEAN_H
