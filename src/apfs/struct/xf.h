#ifndef APFS_STRUCT_XF_H
#define APFS_STRUCT_XF_H

/**
 * Structures and related items as defined in
 * §11 "Extended Fields"
 */

#include <stdint.h>

typedef struct {
    uint16_t    xf_num_exts;
    uint16_t    xf_used_data;
    uint8_t     xf_data[];
} xf_blob_t;

typedef struct {
    uint8_t     x_type;
    uint8_t     x_flags;
    uint16_t    x_size;
} x_field_t;

/** Extended-Field Types **/

#define DREC_EXT_TYPE_SIBLING_ID        1

#define INO_EXT_TYPE_SNAP_XID           1
#define INO_EXT_TYPE_DELTA_TREE_OID     2
#define INO_EXT_TYPE_DOCUMENT_ID        3
#define INO_EXT_TYPE_NAME               4
#define INO_EXT_TYPE_PREV_FSIZE         5
#define INO_EXT_TYPE_RESERVED_6         6
#define INO_EXT_TYPE_FINDER_INFO        7
#define INO_EXT_TYPE_DSTREAM            8
#define INO_EXT_TYPE_RESERVED_9         9
#define INO_EXT_TYPE_DIR_STATS_KEY      10
#define INO_EXT_TYPE_FS_UUID            11
#define INO_EXT_TYPE_RESERVED_12        12
#define INO_EXT_TYPE_SPARSE_BYTES       13
#define INO_EXT_TYPE_RDEV               14
#define INO_EXT_TYPE_PURGEABLE_FLAGS    15
#define INO_EXT_TYPE_ORIG_SYNC_ROOT_ID  16

/** Extended-Field Flags **/

#define XF_DATA_DEPENDENT       0x0001
#define XF_DO_NOT_COPY          0x0002
#define XF_RESERVED_4           0x0004
#define XF_CHILDREN_INHERIT     0x0008
#define XF_USER_FIELD           0x0010
#define XF_SYSTEM_FIELD         0x0020
#define XF_RESERVED_40          0x0040
#define XF_RESERVED_80          0x0080

#endif // APFS_STRUCT_XF_H
