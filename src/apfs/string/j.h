/**
 * Functions that generate nicely formatted strings representing data found in
 * File-system-related objects, such as `apfs_superblock_t`.
 */

#ifndef APFS_STRING_J_H
#define APFS_STRING_J_H

#include "../struct/j.h"

char* j_key_type_to_string(uint8_t j_key_type) {
    switch (j_key_type) {
        case APFS_TYPE_SNAP_METADATA:
            return "Snapshot metadata";
        case APFS_TYPE_EXTENT:
            return "Physical extent record";
        case APFS_TYPE_INODE:
            return "Inode";
        case APFS_TYPE_XATTR:
            return "Extended attribute";
        case APFS_TYPE_SIBLING_LINK:
            return "Sibling link (mapping from an inode to hard links that target it)";
        case APFS_TYPE_DSTREAM_ID:
            return "Data stream";
        case APFS_TYPE_CRYPTO_STATE:
            return "Per-file encryption state";
        case APFS_TYPE_FILE_EXTENT:
            return "Physical extent record for a file";
        case APFS_TYPE_DIR_REC:
            return "Directory entry";
        case APFS_TYPE_DIR_STATS:
            return "Directory information/statistics";
        case APFS_TYPE_SNAP_NAME:
            return "Snapshot name";
        case APFS_TYPE_SIBLING_MAP:
            return "Sibling map (mapping from hard link to inode)";
        case APFS_TYPE_INVALID:
            return "Invalid type";
        default:
            return "Unknown type";
    }
}

void print_j_key(j_key_t* j_key) {
    printf("Object ID:      0x%llx\n",  j_key->obj_id_and_type & OBJ_ID_MASK);
    printf("Object type:    %s\n",      j_key_type_to_string(
            (j_key->obj_id_and_type & OBJ_TYPE_MASK) >> OBJ_TYPE_SHIFT
    ));
}

#endif // APFS_STRING_J_H
