/**
 * Structures and related items as defined in
 * §9 "File-System Constants"
 */

#ifndef APFS_STRUCT_J_CONST_H
#define APFS_STRUCT_J_CONST_H

#include <stdint.h>

/** `j_obj_types` **/

typedef enum {
    APFS_TYPE_ANY           = 0,
    
    APFS_TYPE_SNAP_METADATA = 1,
    APFS_TYPE_EXTENT        = 2,
    APFS_TYPE_INODE         = 3,
    APFS_TYPE_XATTR         = 4,
    APFS_TYPE_SIBLING_LINK  = 5,
    APFS_TYPE_DSTREAM_ID    = 6,
    APFS_TYPE_CRYPTO_STATE  = 7,
    APFS_TYPE_FILE_EXTENT   = 8,
    APFS_TYPE_DIR_REC       = 9,
    APFS_TYPE_DIR_STATS     = 10,
    APFS_TYPE_SNAP_NAME     = 11,
    APFS_TYPE_SIBLING_MAP   = 12,
    
    APFS_TYPE_MAX_VALID     = 12,
    APFS_TYPE_MAX           = 15,
    
    APFS_TYPE_INVALID       = 15,
} j_obj_types;

/** `j_obj_kinds` **/

typedef enum {
    APFS_KIND_ANY           = 0,
    APFS_KIND_NEW           = 1,
    APFS_KIND_UPDATE        = 2,
    APFS_KIND_DEAD          = 3,
    APFS_KIND_UPDATE_RECENT = 4,
    
    APFS_KIND_INVALID       = 255,
} j_obj_kinds;

/** `j_inode_flags` **/

typedef enum {
    INODE_IS_APFS_PRIVATE           = 0x00000001,
    INODE_MAINTAIN_DIR_STATS        = 0x00000002,
    INODE_DIR_STATS_ORIGIN          = 0x00000004,
    INODE_PROT_CLASS_EXPLICIT       = 0x00000008,
    INODE_WAS_CLONED                = 0x00000010,
    INODE_FLAG_UNUSED               = 0x00000020,
    INODE_HAS_SECURITY_EA           = 0x00000040,
    INODE_BEING_TRUNCATED           = 0x00000080,
    INODE_HAS_FINDER_INFO           = 0x00000100,
    INODE_IS_SPARSE                 = 0x00000200,
    INODE_WAS_EVER_CLONED           = 0x00000400,
    INODE_ACTIVE_FILE_TRIMMED       = 0x00000800,
    INODE_PINNED_TO_MAIN            = 0x00001000,
    INODE_PINNED_TO_TIER2           = 0x00002000,
    INODE_HAS_RSRC_FORK             = 0x00004000,
    INODE_NO_RSRC_FORK              = 0x00008000,
    INODE_ALLOCATION_SPILLEDOVER    = 0x00010000,

    INODE_INHERITED_INTERNAL_FLAGS  = INODE_MAINTAIN_DIR_STATS,
    
    INODE_CLONED_INTERNAL_FLAGS     = ( \
          INODE_HAS_RSRC_FORK           \
        | INODE_NO_RSRC_FORK            \
        | INODE_HAS_FINDER_INFO         \
    ),
} j_inode_flags;

#define APFS_VALID_INTERNAL_INODE_FLAGS   ( \
      INODE_IS_APFS_PRIVATE                 \
    | INODE_MAINTAIN_DIR_STATS              \
    | INODE_DIR_STATS_ORIGIN                \
    | INODE_PROT_CLASS_EXPLICIT             \
    | INODE_WAS_CLONED                      \
    | INODE_HAS_SECURITY_EA                 \
    | INODE_BEING_TRUNCATED                 \
    | INODE_HAS_FINDER_INFO                 \
    | INODE_IS_SPARSE                       \
    | INODE_WAS_EVER_CLONED                 \
    | INODE_ACTIVE_FILE_TRIMMED             \
    | INODE_PINNED_TO_MAIN                  \
    | INODE_PINNED_TO_TIER2                 \
    | INODE_HAS_RSRC_FORK                   \
    | INODE_NO_RSRC_FORK                    \
    | INODE_ALLOCATION_SPILLEDOVER          \
)

#define APFS_INODE_PINNED_MASK  (INODE_PINNED_TO_MAIN | INODE_PINNED_TO_TIER2)

/** `j_xattr_flags` **/

typedef enum {
    XATTR_DATA_STREAM       = 0x00000001,
    XATTR_DATA_EMBEDDED     = 0x00000002,
    XATTR_FILE_SYSTEM_OWNED = 0x00000004,
    XATTR_RESERVED_8        = 0x00000008,
} j_xattr_flags;

/** `dir_rec_flags` **/

typedef enum {
    DREC_TYPE_MASK  = 0x000f,
    RESERVED_10     = 0x0010,
} dir_rec_flags;

/** Inode Numbers **/

#define INVALID_INO_NUM     0
#define ROOT_DIR_PARENT     1
#define ROOT_DIR_INO_NUM    2
#define PRIV_DIR_INO_NUM    3
#define SNAP_DIR_INO_NUM    6

#define MIN_USER_INO_NUM    16

/** Extended Attributes Constants **/

#define XATTR_MAX_EMBEDDED_SIZE     3804    // = 3 Ki + 732
#define SYMLINK_EA_NAME             "com.apple.fs.symlink"

/** File-System Object Constants **/

#define OWNING_OBJ_ID_INVALID   ~0ULL
#define OWNING_OBJ_ID_UNKNOWN   ~1ULL

#define JOBJ_MAX_KEY_SIZE       832
#define JOBJ_MAX_VALUE_SIZE     3808    // = 3 Ki + 736

#define MIN_DOC_ID              3

/** File Extent Constants **/

#define FEXT_CRYPTO_ID_IS_TWEAK     0x01

/** File Modes **/

typedef uint16_t    mode_t;

#define S_IFMT      0170000

#define S_IFIFO     0010000
#define S_IFCHR     0020000
#define S_IFDIR     0040000
#define S_IFBLK     0060000
#define S_IFREG     0100000
#define S_IFLNK     0120000
#define S_IFSOCK    0140000
#define S_IFWHT     0160000

/** Directory Entry File Types **/

#define DT_UNKNOWN  0
#define DT_FIFO     1
#define DT_CHR      2
#define DT_DIR      4
#define DT_BLK      6
#define DT_REG      8
#define DT_LNK      10
#define DT_SOCK     12
#define DT_WHT      14

#endif // APFS_STRUCT_J_CONST_H
