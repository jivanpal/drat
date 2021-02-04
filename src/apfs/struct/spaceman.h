#ifndef APFS_STRUCT_SPACEMAN_H
#define APFS_STRUCT_SPACEMAN_H

/**
 * Structures and related items as defined in
 * §17 "Space Manager"
 */

#include <stdint.h>
#include "general.h"    // paddr_t
#include "object.h"     // obj_phys_t, xid_t

/** `chunk_info_t` **/

typedef struct {
    xid_t       ci_xid;     // Spec says `uint64_t`, but I assume it is meant to be `xid_t`; they're equivalent, anyway
    uint64_t    ci_addr;
    uint32_t    ci_block_count;
    uint32_t    ci_free_count;
    paddr_t     ci_bitmap_addr;
} chunk_info_t;

/** `chunk_info_block_t` **/

typedef struct {
    obj_phys_t      cib_o;
    uint32_t        cib_index;
    uint32_t        cib_chunk_info_count;
    chunk_info_t    cib_chunk_info[];
} chunk_info_block_t;

/** `cib_addr_block` **/

typedef struct {
    obj_phys_t  cab_o;
    uint32_t    cab_index;
    uint32_t    cab_cib_count;
    paddr_t     cab_cib_addr[];
} cib_addr_block_t;

/** `spaceman_free_queue_key_t` --- forward declared for `spaceman_free_queue_entry_t` **/

typedef struct {
    xid_t       sfqk_xid;
    paddr_t     sfqk_paddr;
} spaceman_free_queue_key_t;

/** `spaceman_free_queue_val_t` --- forward declared for `spaceman_free_queue_entry_t` **/

typedef uint64_t spaceman_free_queue_val_t;

/** `spaceman_free_queue_entry_t` **/

typedef struct {
    spaceman_free_queue_key_t   sfqe_key;
    spaceman_free_queue_val_t   sfqe_count;
} spaceman_free_queue_entry_t;

/** `spaceman_free_queue_t` **/

typedef struct {
    uint64_t    sfq_count;
    oid_t       sfq_tree_oid;
    xid_t       sfq_oldest_xid;
    uint16_t    sfq_tree_node_limit;
    uint16_t    sfq_pad16;
    uint32_t    sfq_pad32;
    uint64_t    sfq_reserved;
} spaceman_free_queue_t;

/** `spaceman_device_t` **/

typedef struct {
    uint64_t    sm_block_count;
    uint64_t    sm_chunk_count;
    uint32_t    sm_cib_count;
    uint32_t    sm_cab_count;
    uint64_t    sm_free_count;
    uint32_t    sm_addr_offset;
    uint32_t    sm_reserved;
    uint64_t    sm_reserved2;
} spaceman_device_t;

/** `spaceman_allocation_zone_boundaries_t` **/

typedef struct {
    uint64_t    saz_zone_start;
    uint64_t    saz_zone_end;
} spaceman_allocation_zone_boundaries_t;

/** `spaceman_allocation_zone_info_phys_t` **/

#define SM_ALLOCZONE_INVALID_END_BOUNDARY       0
#define SM_ALLOCZONE_NUM_PREVIOUS_BOUNDARIES    7

typedef struct {
    spaceman_allocation_zone_boundaries_t
        saz_current_boundaries;
    spaceman_allocation_zone_boundaries_t
        saz_previous_boundaries[SM_ALLOCZONE_NUM_PREVIOUS_BOUNDARIES];
    uint16_t    saz_zone_id;
    uint16_t    saz_previous_boundary_index;
    uint32_t    saz_reserved;
} spaceman_allocation_zone_info_phys_t;

/** `smdev` --- forward declared for `spaceman[_datazone_info]_phys_t` **/

enum smdev {
    SD_MAIN     = 0,
    SD_TIER2    = 1,
    SD_COUNT    = 2,
};

/** `spaceman_datazone_info_phys_t` **/

#define SM_DATAZONE_ALLOCZONE_COUNT     8

typedef struct {
    spaceman_allocation_zone_info_phys_t
        sdz_allocation_zones[SD_COUNT][SM_DATAZONE_ALLOCZONE_COUNT];
} spaceman_datazone_info_phys_t;

/** `sfq` --- forward declared for `spaceman_phys_t` **/

enum sfq {
    SFQ_IP      = 0,
    SFQ_MAIN    = 1,
    SFQ_TIER2   = 2,
    SFQ_COUNT   = 3,
};

/** `spaceman_phys_t` **/

typedef struct {
    obj_phys_t  sm_o;
    uint32_t    sm_block_size;
    uint32_t    sm_blocks_per_chunk;
    uint32_t    sm_chunks_per_cib;
    uint32_t    sm_cibs_per_cab;

    spaceman_device_t   sm_dev[SD_COUNT];

    uint32_t    sm_flags;
    uint32_t    sm_ip_bm_tx_multiplier;
    uint64_t    sm_ip_block_count;
    uint32_t    sm_ip_bm_size_in_blocks;
    uint32_t    sm_ip_bm_block_count;
    paddr_t     sm_ip_bm_base;
    paddr_t     sm_ip_base;
    uint64_t    sm_fs_reserve_block_count;
    uint64_t    sm_fs_reserve_alloc_count;

    spaceman_free_queue_t   sm_fq[SFQ_COUNT];

    uint16_t    sm_ip_bm_free_head;
    uint16_t    sm_ip_bm_free_tail;
    uint32_t    sm_ip_bm_xid_offset;
    uint32_t    sm_ip_bitmap_offset;
    uint32_t    sm_ip_bm_free_next_offset;
    uint32_t    sm_version;
    uint32_t    sm_struct_size;

    spaceman_datazone_info_phys_t   sm_datazone;
} spaceman_phys_t;

#define SM_FLAG_VERSIONED   0x00000001

/** Chunk Info Block Constants **/

#define CI_COUNT_MASK           0x000fffff
#define CI_COUNT_RESERVED_MASK  0xfff00000

/** Internal-Pool Bitmap **/

#define SPACEMAN_IP_BM_TX_MULTIPLIER    16
#define SPACEMAN_IP_BM_INDEX_INVALID    0xfff
#define SPACEMAN_IP_BM_BLOCK_COUNT_MAX  0xffe

#endif // APFS_STRUCT_SPACEMAN_H
