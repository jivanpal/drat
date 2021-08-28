#include "xf.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>

#include <apfs/xf.h>
#include <apfs/object.h>    // oid_t, xid_t
#include <apfs/dstream.h>   // j_dstream_t
#include <apfs/j.h>         // j_dir_stats_val_t
#include <apfs/general.h>   // uuid_t

#include <drat/asize.h>

#include <drat/func/xf.h>

#include <drat/string/common.h>
#include <drat/string/dstream.h>
#include <drat/string/j.h>

/**
 * Get a human-readable string describing the type of a given xfield.
 * 
 * x_type:
 *  The `x_type` field of an instance of `x_field_t`.
 * 
 * RETURN VALUE:
 *  A C-string. This pointer needn't be freed by the caller.
 */
char* x_type_to_string(uint8_t x_type) {
    switch (x_type) {
        case INO_EXT_TYPE_SNAP_XID:             return "Snapshot XID";
        case INO_EXT_TYPE_DELTA_TREE_OID:       return "Snapshot extent delta tree Virtual OID";
        case INO_EXT_TYPE_DOCUMENT_ID:          return "Document ID";
        case INO_EXT_TYPE_NAME:                 return "Item name";
        case INO_EXT_TYPE_PREV_FSIZE:           return "Previous file size";
        case INO_EXT_TYPE_RESERVED_6:           return "Reserved field 6";
        case INO_EXT_TYPE_FINDER_INFO:          return "Finder info";
        case INO_EXT_TYPE_DSTREAM:              return "Data stream";
        case INO_EXT_TYPE_RESERVED_9:           return "Reserved field 9";
        case INO_EXT_TYPE_DIR_STATS_KEY:        return "Directory statistics";
        case INO_EXT_TYPE_FS_UUID:              return "Filesystem auto-mount UUID";
        case INO_EXT_TYPE_RESERVED_12:          return "Reserved field 12";
        case INO_EXT_TYPE_SPARSE_BYTES:         return "Number of sparse bytes in data stream";
        case INO_EXT_TYPE_RDEV:                 return "ID of special device";
        case INO_EXT_TYPE_PURGEABLE_FLAGS:      return "Flags for purgeable file";
        case INO_EXT_TYPE_ORIG_SYNC_ROOT_ID:    return "Inode number of file's original sync-root";
        default:                                return "(unknown)";
    }
}

/**
 * Get a human-readable string that lists the type flags that are set on a given
 * xfield.
 * 
 * x_flags:
 *  The `x_flags` field of an instance of `x_field_t`.
 * 
 * RETURN VALUE:
 *  A C-string. The caller must free this pointer when it is no longer needed.
 */
char* get_x_flags_string(uint8_t x_flags) {
    enum_string_mapping_t flags[] = {
        { XF_DATA_DEPENDENT,    "Dependent on file data" },
        { XF_DO_NOT_COPY,       "Do not copy" },
        { XF_RESERVED_4,        "Reserved flag 3 (0x04)" },
        { XF_CHILDREN_INHERIT,  "Inherited by children" },
        { XF_USER_FIELD,        "Set in userspace" },
        { XF_SYSTEM_FIELD,      "Set by system (in kernel space)" },
        { XF_RESERVED_40,       "Reserved flag 7 (0x40)" },
        { XF_RESERVED_80,       "Reserved flag 8 (0x80)" },
    };

    return get_flags_enum_string(flags, ARRAY_SIZE(flags), x_flags, true);
}

void print_x_field(x_field_t* xfield) {
    char* flags_string = get_x_flags_string(xfield->x_flags);

    printf("Type:   %s\n",              x_type_to_string(xfield->x_type));
    printf("Size:   %"PRIu16" bytes\n", xfield->x_size);
    printf("Flags:  %s\n",              flags_string);

    free(flags_string);
}

void print_xf_pair(xf_pair_t* xf_pair) {
    printf("== KEY ==\n");
    print_x_field(xf_pair);

    printf("== VALUE ==\n");
    
    // Avoid having to rewrite this expression a bazillion times
    void* value = &(xf_pair->value);

    /*
     * xfield types whose values are of unknown data type (e.g. reserved types,
     * Finder data, purgeable flags) appear together at the bottom of this list.
     */
    switch(xf_pair->key.x_type) {
        case INO_EXT_TYPE_SNAP_XID:
            xid_t* snap_xid = value;
            printf("Snapshot XID:   %#"PRIx64"\n", *snap_xid);
            break;
        case INO_EXT_TYPE_DELTA_TREE_OID:
            oid_t* delta_tree_oid = value;
            printf("Delta tree Virtual OID: %#"PRIx64"\n", *delta_tree_oid);
            break;
        case INO_EXT_TYPE_DOCUMENT_ID:
            uint32_t* document_id = value;
            printf("Document ID:    %#"PRIx32"\n", *document_id);
            break;
        case INO_EXT_TYPE_NAME:
            char* name = value;
            printf("Item name: %s\n", name);
            break;
        case INO_EXT_TYPE_PREV_FSIZE:
            uint64_t* size = value;
            printf("Previous file size: %"PRIu64" bytes\n", *size);
            break;
        case INO_EXT_TYPE_DSTREAM:
            j_dstream_t* dstream = value;
            print_j_dstream(dstream);
            break;
        case INO_EXT_TYPE_DIR_STATS_KEY:
            j_dir_stats_val_t* dir_stats = value;
            print_j_dir_stats_val(dir_stats);
            break;
        case INO_EXT_TYPE_FS_UUID:
            uuid_t* uuid = value;
            // TODO: Factor out into `print_uuid()`, see if UUIDs are printed elsewhere; factor those, too
            printf("Filesystem UUID:    %#016"PRIx64"%016"PRIx64"\n", *(uint64_t*)(uuid+8), *(uint64_t*)uuid);
            break;
        case INO_EXT_TYPE_SPARSE_BYTES:
            uint64_t* bytes = value;
            printf("Number of sparse bytes: %"PRIu64" bytes\n", *bytes);
            break;
        case INO_EXT_TYPE_RDEV:
            uint32_t* rdev = value;
            printf("Device ID:  %#"PRIx32"\n", *rdev);
            break;
        case INO_EXT_TYPE_ORIG_SYNC_ROOT_ID:
            uint64_t* inode_number = value;
            printf("Inode number:   %#"PRIx64"\n", *inode_number);
            break;

        // xfield types whose value is of unknown type
        case INO_EXT_TYPE_RESERVED_6:
        case INO_EXT_TYPE_RESERVED_9:
        case INO_EXT_TYPE_RESERVED_12:
        case INO_EXT_TYPE_FINDER_INFO:
        case INO_EXT_TYPE_PURGEABLE_FLAGS:
        default:
            printf("Unknown value type.\n");
            // TODO: Print hexdump of value based on size of field
            break;
    }
}

/**
 * Given an array of xfields as returned by a call to `get_xf_pairs_array()`,
 * print a human-readable description of the data contained within.
 * 
 * xf_pairs_array: The array to print a description of.
 */
void print_xf_pairs_array(xf_pair_t** xf_pairs_array) {
    for (xf_pair_t** cursor = xf_pairs_array; *cursor; cursor++) {
        print_xf_pair(*cursor);
        printf("\n");
    }
}
