/**
 * Functions that generate nicely formatted strings representing data found in
 * File-system-related objects, such as `apfs_superblock_t`.
 */

#ifndef APFS_STRING_J_H
#define APFS_STRING_J_H

#include <stdbool.h>
#include <time.h>
#include "../struct/j.h"
#include "../struct/xf.h"

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

void print_j_key(j_key_t* key) {
    printf("Object ID:      0x%llx\n",  key->obj_id_and_type & OBJ_ID_MASK);
    printf("Object type:    %s\n",      j_key_type_to_string(
            (key->obj_id_and_type & OBJ_TYPE_MASK) >> OBJ_TYPE_SHIFT
    ));
}

void print_j_drec_hashed_key(j_drec_hashed_key_t* key) {
    print_j_key(key);   // `key` equals `&(key->hdr)`
    printf("\n");

    // 10-bit value; next smallest datatype is 16 bits
    uint16_t name_len = key->name_len_and_hash & J_DREC_LEN_MASK;
    // 22-bit value; next smallest datatype is 32 bits
    uint32_t name_hash = (key->name_len_and_hash & J_DREC_HASH_MASK) >> J_DREC_HASH_SHIFT;
    // TODO: validate the hash?
    
    printf("Directory name length:  %u UTF-8 bytes (including terminating NULL (U+0000) byte)\n",  name_len);
    printf("Directory name hash:    0x%06x\n",          name_hash);
    printf("Directory name:         ### %s ####\n",     key->name);
}

char* drec_val_to_type_string(j_drec_val_t* val) {
    switch (val->flags & DREC_TYPE_MASK) {
        case DT_UNKNOWN:
            return "Unknown";
        case DT_FIFO:
            return "Named pipe (FIFO / queue)";
        case DT_CHR:
            return "Character-special file";
        case DT_DIR:
            return "Directory";
        case DT_BLK:
            return "Block-special file";
        case DT_REG:
            return "Regular file";
        case DT_LNK:
            return "Symbolic link";
        case DT_SOCK:
            return "Socket";
        case DT_WHT:
            return "Whiteout";
        default:
            return "Unrecognised type";
    }
}

void print_j_drec_val(j_drec_val_t* val, bool has_xfields) {
    printf("File ID:                0x%llx\n", val->file_id);

    // timestamp converted from nanoseconds since
    // Unix epoch to seconds since Unix epoch.
    // Newline '\n' is added by result of `ctime()`.
    time_t timestamp = val->date_added / 1000000000;
    printf("Time added:             %s",    ctime(&timestamp));

    printf("Dentry type:            %s\n",  drec_val_to_type_string(val));

    printf("No. extended fields:    ");
    if (!has_xfields) {
        printf("0\n");
        return;
    }
    printf("%u\n", ((xf_blob_t*)(val->xfields))->xf_num_exts);
    
    // TODO: Print actual details of extended fields/attributes
}

#endif // APFS_STRING_J_H
