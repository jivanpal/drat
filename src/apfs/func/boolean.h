/**
 * Boolean functions to check whether a given condition holds.
 */

/**
 * Determine whether a given APFS block is a container superblock.
 */
char is_nx_superblock(obj_phys_t* obj) {
    return (obj->o_type & OBJECT_TYPE_MASK) == OBJECT_TYPE_NX_SUPERBLOCK;
}

char is_checkpoint_map_phys(obj_phys_t* obj) {
    return (obj->o_type & OBJECT_TYPE_MASK) == OBJECT_TYPE_CHECKPOINT_MAP;
}
