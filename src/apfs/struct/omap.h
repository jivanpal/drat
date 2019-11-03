/**
 * Structures and related items as defined in
 * §6 "Object Maps"
 */

#ifndef APFS_STRUCT_OMAP_H
#define APFS_STRUCT_OMAP_H

#include <stdint.h>
#include "general.h"    // for `paddr_t`
#include "object.h"     // for `obj_phys_t`, `oid_t`, `xid_t`

/** `omap_phys_t` **/

typedef struct omap_phys {
    obj_phys_t  om_o;
    uint32_t    om_flags;
    uint32_t    om_snap_count;
    uint32_t    om_tree_type;
    uint32_t    om_snapshot_tree_type;
    oid_t       om_tree_oid;
    oid_t       om_snapshot_tree_oid;
    xid_t       om_most_recent_snap;
    xid_t       om_pending_revert_min;
    xid_t       om_pending_revert_max;
} omap_phys_t;

/** `omap_key_t` **/

typedef struct omap_key {
    oid_t   ok_oid;
    xid_t   ok_xid;
} omap_key_t;

/** `omap_val_t` **/

typedef struct omap_val {
    uint32_t    ov_flags;
    uint32_t    ov_size;
    paddr_t     ov_paddr;
} omap_val_t;

/** `omap_snapshot_t` **/

typedef struct omap_snapshot {
    uint32_t    oms_flags;
    uint32_t    oms_pad;
    oid_t       oms_oid;
} omap_snapshot_t;

/** Object Map Value Flags **/

#define OMAP_VAL_DELETED            0x00000001
#define OMAP_VAL_SAVED              0x00000002
#define OMAP_VAL_ENCRYPTED          0x00000004
#define OMAP_VAL_NOHEADER           0x00000008
#define OMAP_VAL_CRYPTO_GENERATION  0x00000010

/** Snapshot Flags **/

#define OMAP_SNAPSHOT_DELETED       0x00000001
#define OMAP_SNAPSHOT_REVERTED      0x00000002

/** Object Map Flags **/

#define OMAP_MANUALLY_MANAGED   0x00000001
#define OMAP_ENCRYPTING         0x00000002
#define OMAP_DECRYPTING         0x00000004
#define OMAP_KEYROLLING         0x00000008
#define OMAP_CRYPTO_GENERATION  0x00000010

/** Object Map Constants **/

#define OMAP_MAX_SNAP_COUNT     UINT32_MAX

/** Object Map Reaper Phases **/

#define OMAP_REAP_PHASE_MAP_TREE        1
#define OMAP_REAP_PHASE_SNAPSHOT_TREE   2

#endif // APFS_STRUCT_OMAP_H
