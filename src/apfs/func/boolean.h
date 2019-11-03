/**
 * Boolean functions to check whether a given condition holds.
 */

#ifndef APFS_FUNC_BOOLEAN_H
#define APFS_FUNC_BOOLEAN_H

#include "../struct/object.h"

/**
 * Determine whether a given APFS block is a container superblock based on the
 * type flag in its header.
 */
char is_nx_superblock(obj_phys_t* obj) {
    return (obj->o_type & OBJECT_TYPE_MASK) == OBJECT_TYPE_NX_SUPERBLOCK;
}

/**
 * Determine whether a given APFS block is a checkpoint mapping block based on
 * the type flag in its header.
 */
char is_checkpoint_map_phys(obj_phys_t* obj) {
    return (obj->o_type & OBJECT_TYPE_MASK) == OBJECT_TYPE_CHECKPOINT_MAP;
}

#endif // APFS_FUNC_BOOLEAN_H
