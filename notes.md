# §3 Objects

  ## Object Types

  - [x] OBJECT_TYPE_ER_RECOVERY_BLOCK
  - [x] OBJECT_TYPE_SNAP_META_EXT
  - [x] OBJECT_TYPE_INTEGRITY_META
  - [x] OBJECT_TYPE_FEXT_TREE
  - [x] OBJECT_TYPE_RESERVED_20
  - [x] OBJECT_TYPE_MEDIA_KEYBAG

# §5 Container

  ## `nx_superblock_t`

  - [x] nx_newest_mounted_version
  - [x] nx_mkb_locker

# §6 Object Maps

  ## Object Map Flags

  - [x] OMAP_VALID_FLAGS

# §7 Volumes

  ## `apfs_superblock_t`

  - [x] apfs_cloneinfo_id_epoch
  - [x] apfs_cloneinfo_xid
  - [x] apfs_snap_meta_ext_oid
  - [x] apfs_volume_group_id
  - [x] apfs_integrity_meta_oid
  - [x] apfs_fext_tree_oid
  - [x] apfs_fext_tree_type
  - [x] reserved_type
  - [x] reserved_oid

  ## Volume Flags

  - [x] APFS_FS_RESERVED_80
  - [x] APFS_FS_RESERVED_100

  ## Volume Roles

  - [x] APFS_VOL_ROLE_UPDATE
  - [x] APFS_VOL_ROLE_XART
  - [x] APFS_VOL_ROLE_HARDWARE
  - [x] APFS_VOL_ROLE_BACKUP
  - [x] APFS_VOL_ROLE_RESERVED_7
  - [x] APFS_VOL_ROLE_RESERVED_8
  - [x] APFS_VOL_ROLE_ENTERPRISE
  - [x] APFS_VOL_ROLE_RESERVED_10
  - [x] APFS_VOL_ROLE_PRELOGIN

  ## Optional Volume Feature Flags

  - [x] APFS_FEATURE_STRICTATIME
  - [x] APFS_FEATURE_VOLGRP_SYSTEM_INO_SPACE

  ## Incompatible Volume Feature Flags

  - [x] APFS_INCOMPAT_INCOMPLETE_RESTORE
  - [x] APFS_INCOMPAT_SEALED_VOLUME

# §8 File-System Objects

  ## `j_key_t`

  - [x] SYSTEM_OBJ_ID_MARK

  ## `j_inode_val_t`

  - [x] uncompressed_size

# §9 File-System Constants

  ## `j_obj_types`

  - [x] APFS_TYPE_FILE_INFO
  
  ## `j_inode_flags`

  - [x] INODE_FAST_PROMOTE
  - [x] INODE_HAS_UNCOMPRESSED_SIZE
  - [x] INODE_IS_PURGEABLE
  - [x] INODE_WANTS_TO_BE_PURGEABLE
  - [x] INODE_IS_SYNC_ROOT
  - [x] INODE_SNAPSHOT_COW_EXEMPTION

  ## Inode Numbers

  - [x] PURGEABLE_DIR_INO_NUM
  - [x] UNIFIED_ID_SPACE_MARK
  
  ## Extended Attributes Constants

  - [x] FIRMLINK_EA_NAME
  - [x] APFS_COW_EXEMPT_COUNT_NAME

# §11 Extended Fields

  ## Extended-Field Types

  - [x] INO_EXT_TYPE_PURGEABLE_FLAGS
  - [x] INO_EXT_TYPE_ORIG_SYNC_ROOT_ID

# §13 Snapshot Metadata

  ## `snap_meta_ext_obj_phys_t`

  - [x] *New datatype* `snap_meta_ext_obj_phys_t`

  ## `snap_meta_ext_t`

  - [x] *New datatype* `snap_meta_ext_t`

# §14 B-Trees

  ## `btn_index_node_val_t`

  - [x] *New datatype* `btn_index_node_val_t`

  ## B-Tree Flags

  - [x] BTREE_HASHED
  - [x] BTREE_NOHEADER

  ## B-Tree Node Flags

  - [x] BTNODE_HASHED
  - [x] BTNODE_NOHEADER

# §15 Encryption

  ## Protection Classes

  - [x] PROTECTION_CLASS_M

  ## Encryption Identifiers

  - [x] APFS_UNASSIGNED_CRYPTO_ID

  ## Keybag Tags

  - [x] KB_TAG_WRAPPING_M_KEY
  - [x] KB_TAG_VOLUME_M_KEY

# §16 Sealed Volumes

- [ ] *New chapter* Sealed Volumes

# §17 Space Manager

  ## `spaceman_free_queue_entry_t`

  - [x] *New datatype* `spaceman_free_queue_entry_t`
