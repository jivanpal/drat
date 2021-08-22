#ifndef APFS_FS_H
#define APFS_FS_H

/**
 * Structures and related items as defined in
 * §7 Volumes
 */

#include <stdint.h>
#include <apfs/crypto.h>    // wrapped_meta_crypto_state_t
#include <apfs/general.h>   // uuid_t
#include <apfs/object.h>    // obj_phys_t, oid_t, xid_t

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
    obj_phys_t  apfs_o;

    uint32_t    apfs_magic;
    uint32_t    apfs_fs_index;

    uint64_t    apfs_features;
    uint64_t    apfs_readonly_compatible_features;
    uint64_t    apfs_incompatible_features;

    uint64_t    apfs_unmount_time;

    uint64_t    apfs_fs_reserve_block_count;
    uint64_t    apfs_fs_quota_block_count;
    uint64_t    apfs_fs_alloc_count;
    
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
    uint16_t    reserved;

    xid_t       apfs_root_to_xid;
    oid_t       apfs_er_state_oid;

// Fields introduced in revision 2020-05-15

    // Fields supported on macOS 10.13.3+
    uint64_t    apfs_cloneinfo_id_epoch;
    uint64_t    apfs_cloneinfo_xid;

    // Fields supported on macOS 10.15+
    oid_t       apfs_snap_meta_ext_oid;
    uuid_t      apfs_volume_group_id;

// Fields introduced in revision 2020-06-22
    
    // Fields supported on macOS 11+
    oid_t       apfs_integrity_meta_oid;
    oid_t       apfs_fext_tree_oid;
    uint32_t    apfs_fext_tree_type;
    
    uint32_t    reserved_type;
    oid_t       reserved_oid;
} apfs_superblock_t;

/** Volume Flags **/

#define APFS_FS_UNENCRYPTED             0x00000001LL
#define APFS_FS_RESERVED_2              0x00000002LL
#define APFS_FS_RESERVED_4              0x00000004LL
#define APFS_FS_ONEKEY                  0x00000008LL
#define APFS_FS_SPILLEDOVER             0x00000010LL
#define APFS_FS_RUN_SPILLOVER_CLEANER   0x00000020LL
#define APFS_FS_ALWAYS_CHECK_EXTENTREF  0x00000040LL
#define APFS_FS_RESERVED_80             0x00000080LL
#define APFS_FS_RESERVED_100            0x00000100LL

#define APFS_FS_FLAGS_VALID_MASK      ( \
      APFS_FS_UNENCRYPTED               \
    | APFS_FS_RESERVED_2                \
    | APFS_FS_RESERVED_4                \
    | APFS_FS_ONEKEY                    \
    | APFS_FS_SPILLEDOVER               \
    | APFS_FS_RUN_SPILLOVER_CLEANER     \
    | APFS_FS_ALWAYS_CHECK_EXTENTREF    \
    | APFS_FS_RESERVED_80               \
    | APFS_FS_RESERVED_100              \
)

#define APFS_FS_CRYPTOFLAGS   ( \
      APFS_FS_UNENCRYPTED       \
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

#define APFS_VOLUME_ENUM_SHIFT      6

#define APFS_VOL_ROLE_DATA          ( 1 << APFS_VOLUME_ENUM_SHIFT)  // = 0x0040 --- formerly defined explicitly as `0x0040`
#define APFS_VOL_ROLE_BASEBAND      ( 2 << APFS_VOLUME_ENUM_SHIFT)  // = 0x0080 --- formerly defined explicitly as `0x0080`

// Roles supported since revision 2020-05-15 --- macOS 10.15+, iOS 13+
#define APFS_VOL_ROLE_UPDATE        ( 3 << APFS_VOLUME_ENUM_SHIFT)  // = 0x00c0
#define APFS_VOL_ROLE_XART          ( 4 << APFS_VOLUME_ENUM_SHIFT)  // = 0x0100
#define APFS_VOL_ROLE_HARDWARE      ( 5 << APFS_VOLUME_ENUM_SHIFT)  // = 0x0140
#define APFS_VOL_ROLE_BACKUP        ( 6 << APFS_VOLUME_ENUM_SHIFT)  // = 0x0180
#define APFS_VOL_ROLE_RESERVED_7    ( 7 << APFS_VOLUME_ENUM_SHIFT)  // = 0x01c0 --- spec also uses the name `APFS_VOL_ROLE_SIDECAR`, but that could be an error
#define APFS_VOL_ROLE_RESERVED_8    ( 8 << APFS_VOLUME_ENUM_SHIFT)  // = 0x0200 --- formerly named `APFS_VOL_ROLE_RESERVED_200`
#define APFS_VOL_ROLE_ENTERPRISE    ( 9 << APFS_VOLUME_ENUM_SHIFT)  // = 0x0240
#define APFS_VOL_ROLE_RESERVED_10   (10 << APFS_VOLUME_ENUM_SHIFT)  // = 0x0280
#define APFS_VOL_ROLE_PRELOGIN      (11 << APFS_VOLUME_ENUM_SHIFT)  // = 0x02c0

/** Optional Volume Feature Flags **/

#define APFS_FEATURE_DEFRAG_PRERELEASE          0x00000001LL
#define APFS_FEATURE_HARDLINK_MAP_RECORDS       0x00000002LL
#define APFS_FEATURE_DEFRAG                     0x00000004LL
#define APFS_FEATURE_STRICTATIME                0x00000008LL
#define APFS_FEATURE_VOLGRP_SYSTEM_INO_SPACE    0x00000010LL

#define APFS_SUPPORTED_FEATURES_MASK  (     \
      APFS_FEATURE_DEFRAG                   \
    | APFS_FEATURE_DEFRAG_PRERELEASE        \
    | APFS_FEATURE_HARDLINK_MAP_RECORDS     \
    | APFS_FEATURE_STRICTATIME              \
    | APFS_FEATURE_VOLGRP_SYSTEM_INO_SPACE  \
)

/** Read-Only Comaptible Volume Feature Flags **/

#define APFS_SUPPORTED_ROCOMPAT_MASK    0x0ULL

/** Incompatible Volume Feature Flags **/

#define APFS_INCOMPAT_CASE_INSENSITIVE          0x00000001LL
#define APFS_INCOMPAT_DATALESS_SNAPS            0x00000002LL
#define APFS_INCOMPAT_ENC_ROLLED                0x00000004LL
#define APFS_INCOMPAT_NORMALIZATION_INSENSITIVE 0x00000008LL
#define APFS_INCOMPAT_INCOMPLETE_RESTORE        0x00000010LL
#define APFS_INCOMPAT_SEALED_VOLUME             0x00000020LL
#define APFS_INCOMPAT_RESERVED_40               0x00000040LL

#define APFS_SUPPORTED_INCOMPAT_MASK          ( \
      APFS_INCOMPAT_CASE_INSENSITIVE            \
    | APFS_INCOMPAT_DATALESS_SNAPS              \
    | APFS_INCOMPAT_ENC_ROLLED                  \
    | APFS_INCOMPAT_NORMALIZATION_INSENSITIVE   \
    | APFS_INCOMPAT_INCOMPLETE_RESTORE          \
    | APFS_INCOMPAT_SEALED_VOLUME               \
    | APFS_INCOMPAT_RESERVED_40                 \
)

#endif // APFS_FS_H
