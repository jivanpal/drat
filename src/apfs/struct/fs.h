/**
 * Structures and related items as defined in
 * §7 "Volumes"
 */

#ifndef APFS_STRUCT_FS_H
#define APFS_STRUCT_FS_H

#include "general.h"    // for `uuid_t`
#include "object.h"     // for `obj_phys_t`, `oid_t`, `xid_t`
#include "crypto.h"

/** `apfs_modified_by_t` --- forward declared for `apfs_superblock_t` **/

#define APFS_MODIFIED_NAMELEN   32

typedef struct {
    uint8_t     id[APFS_MODIFIED_NAMELEN];
    uint64_t    timestamp;
    xid_t       last_xid;
} apfs_modified_by_t;

/** `apfs_superblock_t` **/

#define APFS_MAGIC          'BSPA'
#define APFS_MAX_HIST       8
#define APFS_VOLNAME_LEN    256

typedef struct {
    obj_phys_t      apfs_o;

    uint32_t        apfs_magic;
    uint32_t        apfs_fs_index;

    uint64_t    apfs_features;
    uint64_t    apfs_readonly_compatible_features;
    uint64_t    apfs_incompatible_features;

    uint64_t    apfs_unmount_time;

    uint64_t    apfs_fs_reserve_block_count;
    uint64_t    apfs_fs_quota_block_count;
    uint64_t    apfs_fs_alloc_count;
    
    // TODO: define `wrapped_meta_crypto_state_t` :: crypto.h ?
    wrapped_meta_crypto_state_t     apfs_meta_crypto;

    uint32_t    apfs_root_tree_type;
    uint32_t    apfs_extentref_tree_type;
    uint32_t    apfs_snap_meta_tree_type;
    
    oid_t       apfs_omap_oid;
    oid_t       apfs_root_tree_oid;
    oid_t       apfs_extentref_tree_oid;
    oid_t       apfs_snap_meta_tree_oid;
    
    xid_t       apfs_revert_to_xid;
    oid_t       apfs_revert_to_sblock_oid;

    uint64_t    apfs_next_obj_id;
    
    uint64_t    apfs_num_files;
    uint64_t    apfs_num_directories;
    uint64_t    apfs_num_symlinks;
    uint64_t    apfs_num_other_fsobjects;
    uint64_t    apfs_num_snapshots;

    uint64_t    apfs_total_block_alloced;
    uint64_t    apfs_total_blocks_freed;

    uuid_t      apfs_vol_uuid;
    uint64_t    apfs_last_mod_time;

    uint64_t    apfs_fs_flags;

    apfs_modified_by_t      apfs_formatted_by;
    apfs_modified_by_t      apfs_modified_by[APFS_MAX_HIST];

    uint8_t     apfs_volname[APFS_VOLNAME_LEN];
    uint32_t    apfs_next_doc_id;

    uint16_t    apfs_role;
    uint16_t    apfs_reserved;

    xid_t       apfs_root_to_xid;
    oid_t       apfs_er_state_oid;
} apfs_superblock_t;

/** Volume Flags **/

#define APFS_FS_UNENCRYPTED             0x00000001LL
#define APFS_FS_RESERVED_2              0x00000002LL
#define APFS_FS_RESERVED_4              0x00000004LL
#define APFS_FS_ONEKEY                  0x00000008LL
#define APFS_FS_SPILLEDOVER             0x00000010LL
#define APFS_FS_RUN_SPILLOVER_CLEANER   0x00000020LL
#define APFS_FS_ALWAYS_CHECK_EXTENTREF  0x00000040LL

#define APFS_FS_FALGS_VALID_MASK      ( \
      APFS_FS_UNENCRYPTED               \
    | APFS_FS_RESERVED_2                \
    | APFS_FS_RESERVED_4                \
    | APFS_FS_ONEKEY                    \
    | APFS_FS_SPILLEDOVER               \
    | APFS_FS_RUN_SPILLOVER_CLEANER     \
    | APFS_FS_ALWAYS_CHECK_EXTENTREF    \
)

#define APFS_FS_CRYPTOFLAGS   ( \
      APFS_FS_UNENCRYPTED       \
    | APFS_FS_RESERVED_2        \
    | APFS_FS_ONEKEY            \
)

/** Volume Roles **/

#define APFS_VOL_ROLE_NONE          0x0000

#define APFS_VOL_ROLE_SYSTEM        0x0001
#define APFS_VOL_ROLE_USER          0x0002
#define APFS_VOL_ROLE_RECOVERY      0x0004
#define APFS_VOL_ROLE_VM            0x0008

#define APFS_VOL_ROLE_PREBOOT       0x0010
#define APFS_VOL_ROLE_INSTALLER     0x0020
#define APFS_VOL_ROLE_DATA          0x0040
#define APFS_VOL_ROLE_BASEBAND      0x0080

#define APFS_VOL_ROLE_RESERVED_200  0x0200

/** Optional Volume Feature Flags **/

#define APFS_FEATURE_DEFRAG_PRERELEASE      0x00000001LL
#define APFS_FEATURE_HARDLINK_MAP_RECORDS   0x00000002LL
#define APFS_FEATURE_DEFRAG                 0x00000004LL

#define APFS_SUPPORTED_FEATURES_MASK  ( \
      APFS_FEATURE_DEFRAG               \
    | APFS_FEATURE_DEFRAG_PRERELEASE    \
    | APFS_FEATURE_HARDLINK_MAP_RECORDS \
)

/** Read-Only Comaptible Volume Feature Flags **/

#define APFS_SUPPORTED_ROCOMPAT_MASK    0x0ULL

/** Incompatible Volume Feature Flags **/

#define APFS_INCOMPAT_CASE_INSENSITIVE          0x000000001LL
#define APFS_INCOMPAT_DATALESS_SNAPS            0x000000002LL
#define APFS_INCOMPAT_ENC_ROLLED                0x000000004LL
#define APFS_INCOMPAT_NORMALIZATION_INSENSITIVE 0x000000008LL

#define APFS_SUPPORTED_INCOMPAT_MASK          ( \
      APFS_INCOMPAT_CASE_INSENSITIVE            \
    | APFS_INCOMPAT_DATALESS_SNAPS              \
    | APFS_INCOMPAT_ENC_ROLLED                  \
    | APFS_INCOMPAT_NORMALIZATION_INSENSITIVE   \
)

#endif // APFS_STRUCT_FS_H
