#include "print-fs-records.h"

#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>

#include <apfs/object.h>    // constants
#include <apfs/j.h>         // j_key_t, j_dir_stats_*
#include <apfs/snap.h>      // j_snap_*
#include <apfs/sibling.h>   // j_sibling_*
#include <apfs/dstream.h>   // j_phys_ext_*, j_dstream_id_*, j_file_extent_*
#include <apfs/crypto.h>    // j_crypto_*

#include <drat/io.h>        // nx_block_size
#include <drat/string/j.h>  // drec_val_to_short_type_string()

char *get_file_name(j_rec_t** file_record) {
    j_rec_t* fs_rec = *file_record; // Only one record gets passed
    j_key_t* hdr = fs_rec->data;
    j_inode_val_t* val = fs_rec->data + fs_rec->key_len;
    switch ((hdr->obj_id_and_type & OBJ_TYPE_MASK) >> OBJ_TYPE_SHIFT) {
        case APFS_TYPE_INODE: {
            char* itemname = get_item_name(val->xfields, fs_rec->val_len);
            if (itemname == NULL) {
                printf("No Xfields found\n");
                return NULL;
            }
            return itemname;
        } break;
        default: {
            fprintf(stderr, "Not an inode...\n");
            return NULL;
        }
    }
    return NULL;
}

void print_fs_records(j_rec_t** fs_records) {
    size_t num_records = 0;

    for (j_rec_t** fs_rec_cursor = fs_records; *fs_rec_cursor; fs_rec_cursor++) {
        num_records++;
        j_rec_t* fs_rec = *fs_rec_cursor;

        j_key_t* hdr = fs_rec->data;
        fprintf(stderr, "- ");

        switch ( (hdr->obj_id_and_type & OBJ_TYPE_MASK) >> OBJ_TYPE_SHIFT ) {
            /*
             * NOTE: Need to enclose each case in a block `{}` since the
             * names `key` and `val` are potentially declared multiple times
             * in the same variable scope (though in practice it is not a
             * concern since every `case` here ends in a `break`.)
             * 
             * This practice also prevents the following error when compiling
             * on Linux: "a label can only be part of a statement and a
             * declaration is not a statement".
             */
            case APFS_TYPE_SNAP_METADATA: {
                j_snap_metadata_key_t* key = fs_rec->data;
                j_snap_metadata_val_t* val = fs_rec->data + fs_rec->key_len;
                fprintf(stderr, "SNAP METADATA");
            } break;
            case APFS_TYPE_EXTENT: {
                j_phys_ext_key_t* key = fs_rec->data;
                j_phys_ext_val_t* val = fs_rec->data + fs_rec->key_len;
                fprintf(stderr, "EXTENT");
            } break;
            case APFS_TYPE_INODE: {
                j_inode_key_t* key = fs_rec->data;
                j_inode_val_t* val = fs_rec->data + fs_rec->key_len;
                fprintf(stderr, "INODE");
                print_j_inode_info(val, fs_rec->val_len);
            } break;
            case APFS_TYPE_XATTR: {
                j_xattr_key_t* key = fs_rec->data;
                j_xattr_val_t* val = fs_rec->data + fs_rec->key_len;
                fprintf(stderr, "XATTR"
                    " || name = %s",
                    
                    key->name
                );
            } break;
            case APFS_TYPE_SIBLING_LINK: {
                j_sibling_key_t* key = fs_rec->data;
                j_sibling_val_t* val = fs_rec->data + fs_rec->key_len;
                fprintf(stderr, "SIBLING LINK");
            } break;
            case APFS_TYPE_DSTREAM_ID: {
                j_dstream_id_key_t* key = fs_rec->data;
                j_dstream_id_val_t* val = fs_rec->data + fs_rec->key_len;
                fprintf(stderr, "DSTREAM ID "
                    " || file ID = %#8"PRIx64
                    " || ref. count = %"PRIu32,

                    key->hdr.obj_id_and_type & OBJ_ID_MASK,
                    val->refcnt
                );
            } break;
            case APFS_TYPE_CRYPTO_STATE: {
                j_crypto_key_t* key = fs_rec->data;
                j_crypto_val_t* val = fs_rec->data + fs_rec->key_len;
                fprintf(stderr, "CRYPTO STATE");
            } break;
            case APFS_TYPE_FILE_EXTENT: {
                j_file_extent_key_t* key = fs_rec->data;
                j_file_extent_val_t* val = fs_rec->data + fs_rec->key_len;

                uint64_t extent_length_bytes = val->len_and_flags & J_FILE_EXTENT_LEN_MASK;
                uint64_t extent_length_blocks = extent_length_bytes / nx_block_size;

                fprintf(stderr, "FILE EXTENT"
                    " || file ID = %#8"PRIx64
                    " || log. addr. = %#10"PRIx64
                    " || length = %8"PRIu64" B = %#10"PRIx64" B = %5"PRIu64" blocks = %#7"PRIx64" blocks"
                    " || phys. block = %#10"PRIx64,

                    key->hdr.obj_id_and_type & OBJ_ID_MASK,
                    key->logical_addr,
                    extent_length_bytes, extent_length_bytes, extent_length_blocks, extent_length_blocks,
                    val->phys_block_num
                );
            } break;
            case APFS_TYPE_DIR_REC: {
                // Apple's spec inorrectly says to use `j_drec_key_t`; see NOTE in <apfs/j.h>
                // iOS 12 dmgs keys are of j_drec_key_t type (pre 2019)
                j_drec_hashed_key_t*    key = fs_rec->data;
                //j_drec_key_t*           key = fs_rec->data;
                j_drec_val_t*           val = fs_rec->data + fs_rec->key_len;
                
                fprintf(stderr, "DIR REC"
                    " || %s"
                    " || target ID = %#8"PRIx64
                    " || name = %s",

                    drec_val_to_short_type_string(val),
                    val->file_id,
                    key->name
                );
            } break;
            case APFS_TYPE_DIR_STATS: {
                j_dir_stats_key_t* key = fs_rec->data;
                // Apple's spec incorrectly says to use `j_drec_val_t`; see NOTE in <apfs/jconst.h>
                j_dir_stats_val_t* val = fs_rec->data + fs_rec->key_len;
                fprintf(stderr, "DIR STATS");
            } break;
            case APFS_TYPE_SNAP_NAME: {
                j_snap_name_key_t* key = fs_rec->data;
                j_snap_name_val_t* val = fs_rec->data + fs_rec->key_len;
                fprintf(stderr, "SNAP NAME");
            } break;
            case APFS_TYPE_SIBLING_MAP: {
                j_sibling_map_key_t* key = fs_rec->data;
                j_sibling_map_val_t* val = fs_rec->data + fs_rec->key_len;
                fprintf(stderr, "SIBLING MAP");
            } break;
            case APFS_TYPE_INVALID:
                fprintf(stderr, "INVALID");
                break;
            default:
                fprintf(stderr, "(unknown)");
                break;
        }

        fprintf(stderr, "\n");
    }

    fprintf(stderr, "\n");
}
