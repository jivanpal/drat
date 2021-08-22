#ifndef APFS_BTREE_H
#define APFS_BTREE_H

/**
 * Structures and related items as defined in
 * §14 B-Trees
 */

#include <stdint.h>
#include <apfs/object.h>    // obj_phys_t

/** `nloc_t` --- forward declared for `btree_node_phys_t` **/

typedef struct {
    uint16_t    off;
    uint16_t    len;
} nloc_t;

/** `btree_node_phys_t` **/

typedef struct {
    obj_phys_t  btn_o;
    uint16_t    btn_flags;
    uint16_t    btn_level;
    uint32_t    btn_nkeys;
    nloc_t      btn_table_space;
    nloc_t      btn_free_space;
    nloc_t      btn_key_free_list;
    nloc_t      btn_val_free_list;
    uint64_t    btn_data[];
} btree_node_phys_t;

/** `btree_info_fixed_t` **/

typedef struct {
    uint32_t    bt_flags;
    uint32_t    bt_node_size;
    uint32_t    bt_key_size;
    uint32_t    bt_val_size;
} btree_info_fixed_t;

/** `btree_info_t` **/

typedef struct {
    btree_info_fixed_t  bt_fixed;
    uint32_t            bt_longest_key;
    uint32_t            bt_longest_val;
    uint64_t            bt_key_count;
    uint64_t            bt_node_count;
} btree_info_t;

/** `btn_index_node_val_t` **/

#define BTREE_NODE_HASH_SIZE_MAX    64

typedef struct {
    oid_t       binv_child_oid;
    uint8_t     binv_child_hash[BTREE_NODE_HASH_SIZE_MAX];
} btn_index_node_val_t;

/** `kvloc_t` **/

typedef struct {
    nloc_t  k;
    nloc_t  v;
} kvloc_t;

/** `kvoff_t` **/

typedef struct {
    uint16_t    k;
    uint16_t    v;
} kvoff_t;

/** B-Tree Flags **/

#define BTREE_UINT64_KEYS           0x00000001
#define BTREE_SEQUENTIAL_INSERT     0x00000002
#define BTREE_ALLOW_GHOSTS          0x00000004
#define BTREE_EPHEMERAL             0x00000008
#define BTREE_PHYSICAL              0x00000010
#define BTREE_NONPERSISTENT         0x00000020
#define BTREE_KV_NONALIGNED         0x00000040
#define BTREE_HASHED                0x00000080
#define BTREE_NOHEADER              0x00000100

/** B-Tree Table of Contents Constants **/

#define BTREE_TOC_ENTRY_INCREMENT   8
#define BTREE_TOC_ENTRY_MAX_UNUSED  (2 * BTREE_TOC_ENTRY_INCREMENT)

/** B-Tree Node Flags **/

#define BTNODE_ROOT                 0x0001
#define BTNODE_LEAF                 0x0002

#define BTNODE_FIXED_KV_SIZE        0x0004
#define BTNODE_HASHED               0x0008
#define BTNODE_NOHEADER             0x0010

#define BTNODE_CHECK_KOFF_INVAL     0x8000

/** B-Tree Node Constants **/

#define BTREE_NODE_SIZE_DEFAULT     4096    // = 4 Ki
#define BTREE_NODE_MIN_ENTRY_COUNT  4

#endif // APFS_BTREE_H
