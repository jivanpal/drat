#ifndef APFS_FUNC_BOOLEAN_H
#define APFS_FUNC_BOOLEAN_H

#include <stdbool.h>
#include "../struct/object.h"

bool is_physical                (obj_phys_t* obj);
bool is_ephemeral               (obj_phys_t* obj);
bool is_virtual                 (obj_phys_t* obj);
bool is_nx_superblock           (obj_phys_t* obj);
bool is_checkpoint_map_phys     (obj_phys_t* obj);
bool is_btree_node_phys_root    (obj_phys_t* obj);
bool is_btree_node_phys_non_root(obj_phys_t* obj);
bool is_btree_node_phys         (obj_phys_t* obj);
bool is_omap_tree               (obj_phys_t* obj);
bool is_fs_tree                 (obj_phys_t* obj);

#endif // APFS_FUNC_BOOLEAN_H
