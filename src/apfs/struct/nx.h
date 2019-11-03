/**
 * Structures and related items as defined in
 * §5 "Container"
 */

#ifndef APFS_STRUCT_NX_H
#define APFS_STRUCT_NX_H

#include <stdint.h>
#include "general.h"    // for `prange_t`, `uuid_t`
#include "object.h"     // for `obj_phys_t`, `oid_t`, `xid_t`

/** `nx_counter_id_t` --- forward declared for `nx_superblock_t` **/

typedef enum {
    NX_CNTR_OBJ_CKSUM_SET   = 0,
    NX_CNTR_OBJ_CKSUM_FAIL  = 1,

    NX_NUM_COUNTERS = 32,
} nx_counter_id_t;

/** `nx_superblock_t` **/

#define NX_MAGIC                        'BSXN'
#define NX_MAX_FILE_SYSTEMS             100

#define NX_EPH_INFO_COUNT               4
#define NX_EPH_MIN_BLOCK_COUNT          8
#define NX_MAX_FILE_SYSTEM_EPH_STRUCTS  4
#define NX_TX_MIN_CHECKPOINT_COUNT      4
#define NX_EPH_INFO_VERSION             1

typedef struct nx_superblock {
    obj_phys_t  nx_o;
    uint32_t    nx_magic;
    uint32_t    nx_block_size;
    uint64_t    nx_block_count;

    uint64_t    nx_features;
    uint64_t    nx_readonly_compatible_features;
    uint64_t    nx_incompatible_features;

    uuid_t      nx_uuid;

    oid_t       nx_next_oid;
    xid_t       nx_next_xid;

    uint32_t    nx_xp_desc_blocks;
    uint32_t    nx_xp_data_blocks;
    paddr_t     nx_xp_desc_base;
    paddr_t     nx_xp_data_base;
    uint32_t    nx_xp_desc_next;
    uint32_t    nx_xp_data_next;
    uint32_t    nx_xp_desc_index;
    uint32_t    nx_xp_desc_len;
    uint32_t    nx_xp_data_index;
    uint32_t    nx_xp_data_len;

    oid_t       nx_spaceman_oid;
    oid_t       nx_omap_oid;
    oid_t       nx_reaper_oid;

    uint32_t    nx_test_type;

    uint32_t    nx_max_file_systems;
    oid_t       nx_fs_oid[NX_MAX_FILE_SYSTEMS];
    uint64_t    nx_counters[NX_NUM_COUNTERS];
    prange_t    nx_blocked_out_prange;
    oid_t       nx_evict_mapping_tree_oid;
    uint64_t    nx_flags;
    paddr_t     nx_efi_jumpstart;
    uuid_t      nx_fusion_uuid;
    prange_t    nx_keylocker;
    uint64_t    nx_ephemeral_info[NX_EPH_INFO_COUNT];

    oid_t       nx_test_oid;

    oid_t       nx_fusion_mt_oid;
    oid_t       nx_fusion_wbc_oid;
    prange_t    nx_fusion_wbc;
} nx_superblock_t;

/** Container Flags **/

#define NX_RESERVED_1       0x00000001LL
#define NX_RESERVED_2       0x00000002LL
#define NX_CRYPTO_SW        0x00000004LL

/** Optional Container Feature Flags **/

#define NX_FEATURE_DEFRAG           0x0000000000000001ULL
#define NX_FEATURE_LCFD             0x0000000000000002ULL
#define NX_SUPPORTED_FEATURES_MASK  (NX_FEATURE_DEFRAG | NX_FEATURE_LCFD)

/** Read-Only Compatible Container Feature Flags **/

#define NX_SUPPORTED_ROCOMPAT_MASK  0x0ULL

/** Incompatible Container Feature Flags **/

#define NX_INCOMPAT_VERSION1        0x0000000000000001ULL
#define NX_INCOMPAT_VERSION2        0x0000000000000002ULL
#define NX_INCOMPAT_FUSION          0x0000000000000100ULL
#define NX_SUPPORTED_INCOMPAT_MASK  (NX_INCOMPAT_VERSION2 | NX_INCOMPAT_FUSION)

/** Block and Container Size **/

#define NX_MINIMUM_BLOCK_SIZE       4096        // =    4 Ki
#define NX_DEFAULT_BLOCK_SIZE       4096        // =    4 Ki
#define NX_MAXIMUM_BLOCK_SIZE       65536       // =   64 Ki
#define NX_MINIMUM_CONTAINER_SIZE   1048576     // = 1024 Ki = 1 Mi

/** `checkpoint_mapping_t` **/

typedef struct checkpoint_mapping {
    uint32_t    cpm_type;
    uint32_t    cpm_subtype;
    uint32_t    cpm_size;
    uint32_t    cpm_pad;
    oid_t       cpm_fs_oid;
    oid_t       cpm_oid;
    oid_t       cpm_paddr;
} checkpoint_mapping_t;

/** `checkpoint_map_phys_t` **/

typedef struct checkpoint_map_phys {
    obj_phys_t              cpm_o;
    uint32_t                cpm_flags;
    uint32_t                cpm_count;
    checkpoint_mapping_t    cpm_map[];
} checkpoint_map_phys_t;

/** Checkpoint Flags **/

#define CHECKPOINT_MAP_LAST 0x00000001

/** `evict_mapping_val_t` **/

typedef struct evict_mapping_val {
    paddr_t     dst_paddr;
    uint64_t    len;
} __attribute__((packed))   evict_mapping_val_t;

#endif // APFS_STRUCT_NX_H
