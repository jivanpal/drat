/**
 * Structures and related items as defined in
 * §8 "File-System Objects"
 */

#include <stdint.h>

/** `j_key_t` **/

typedef struct j_key {
    uint64_t    obj_id_and_type;
} __attribute__((packed))   j_key_t;

#define OBJ_ID_MASK     0x0fffffffffffffffULL
#define OBJ_TYPE_MASK   0xf000000000000000ULL
#define OBJ_TYPE_SHIFT  60

/** `j_inode_key_t` **/

typedef struct j_inode_key {
    j_key_t     hdr;
} __attribute__((packed))   j_inode_key_t;

/** `j_inode_val_t` **/

typedef struct j_inode_val {
    uint64_t        parent_id;
    uint64_t        private_id;
    
    uint64_t        create_time;
    uint64_t        mod_time;
    uint64_t        change_time;
    uint64_t        access_time;
    
    uint64_t        internal_flags;

    union {
        int32_t     nchildren;
        int32_t     nlink;
    };

    cp_key_class_t  default_protection_class;
    uint32_t        write_generation_counter;
    uint32_t        bsd_flags;
    uid_t           owner;
    gid_t           group;
    mode_t          mode;
    uint16_t        pad1;
    uint64_t        pad2;
    uint8_t         xfields[];
} __attribute__((packed))   j_inode_val_t;

typedef uint32_t    uid_t;
typedef uint32_t    gid_t;

/** `j_drec_key_t` **/

typedef struct j_drec_key {
    j_key_t     hdr;
    uint16_t    name_len;
        /*
         * TODO: Above may be `uint32_t len_name_and_hash`?
         * Spec contradicts itself; investigate this --
         * Likely a typo inherited from `j_drec_hashed_key_t` definition.
         */
    uint8_t     name[0];
} __attribute__((packed))   j_drec_key_t;

/** `j_drec_hashed_key_t` **/

typedef struct j_drec_hashed_key {
    j_key_t     hdr;
    uint32_t    name_len_and_hash;
    uint8_t     name[0];
} __attribute__((packed))   j_drec_hashed_key_t;

#define J_DREC_LEN_MASK     0x000003ff
#define J_DREC_HASH_MASK    0xfffff400
#define J_DREC_HASH_SHIFT   10

/** `j_drec_val_t` **/

typedef struct j_drec_val {
    uint64_t    file_id;
    uint64_t    date_added;
    uint64_t    flags;
    uint8_t     xfields[];
} __attribute__((packed))   j_drec_val_t;

/** `j_dir_stats_key_t` **/

typedef struct j_dir_stats_key {
    j_key_t     hdr;
} __attribute__((packed))   j_dir_stats_key_t;

/** `j_dir_stats_val` **/

typedef struct j_dir_stats_val {
    uint64_t    num_children;
    uint64_t    total_size;
    uint64_t    chained_key;
    uint64_t    gen_count;
} __attribute__((packed))   j_dir_stats_val_t;

/** `j_xattr_key_t` **/

typedef struct j_xattr_key {
    j_key_t     hdr;
    uint16_t    name_len;
    uint8_t     name[0];
} __attribute__((packed))   j_xattr_key_t;

/** `j_xattr_val_t` **/

typedef struct j_xattr_val {
    uint16_t    flags;
    uint16_t    xdata_len;
    uint8_t     xdata[0];
} __attribute__((packed))   j_xattr_val_t;
