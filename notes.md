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

  - [ ] OMAP_VALID_FLAGS

# §7 Volumes

  ## `apfs_superblock_t`

  - [ ] apfs_cloneinfo_id_epoch
  - [ ] apfs_cloneinfo_xid
  - [ ] apfs_snap_meta_ext_oid
  - [ ] apfs_volume_group_id
  - [ ] apfs_integrity_meta_oid
  - [ ] apfs_fext_tree_oid
  - [ ] apfs_fext_tree_type
  - [ ] reserved_type
  - [ ] reserved_oid

  ## Volume Flags

  - [ ] APFS_FS_RESERVED_80
  - [ ] APFS_FS_RESERVED_100

  ## Volume Roles

  - [ ] APFS_VOL_ROLE_UPDATE
  - [ ] APFS_VOL_ROLE_XART
  - [ ] APFS_VOL_ROLE_HARDWARE
  - [ ] APFS_VOL_ROLE_BACKUP
  - [ ] APFS_VOL_ROLE_RESERVED_7
  - [ ] APFS_VOL_ROLE_RESERVED_8
  - [ ] APFS_VOL_ROLE_ENTERPRISE
  - [ ] APFS_VOL_ROLE_RESERVED_10
  - [ ] APFS_VOL_ROLE_PRELOGIN

  ## Optional Volume Feature Flags

  - [ ] APFS_FEATURE_STRICTATIME
  - [ ] APFS_FEATURE_VOLGRP_SYSTEM_INO_SPACE

  ## Incompatible Volume Feature Flags

  - [ ] APFS_INCOMPAT_INCOMPLETE_RESTORE
  - [ ] APFS_INCOMPAT_SEALED_VOLUME

# §8 File-System Objects

  ## `j_key_t`

  - [x] SYSTEM_OBJ_ID_MARK

  ## `j_inode_val_t`

  - [x] uncompressed_size

# §9 File-System Constants

  ## `j_obj_types`

  - [x] APFS_TYPE_FILE_INFO
  
  ## `j_inode_flags`

  - [ ] INODE_FAST_PROMOTE
  - [ ] INODE_HAS_UNCOMPRESSED_SIZE
  - [ ] INODE_IS_PURGEABLE
  - [ ] INODE_WANTS_TO_BE_PURGEABLE
  - [ ] INODE_IS_SYNC_ROOT
  - [ ] INODE_SNAPSHOT_COW_EXEMPTION

  ## Inode Numbers

  - [ ] PURGEABLE_DIR_INO_NUM
  - [ ] UNIFIED_ID_SPACE_MARK
  
  ## Extended Attributes Constants

  - [ ] FIRMLINK_EA_NAME
  - [ ] APFS_COW_EXEMPT_COUNT_NAME

# §11 Extended Fields

  ## Extended-Field Types

  - [ ] INO_EXT_TYPE_PURGEABLE_FLAGS
  - [ ] INO_EXT_TYPE_ORIG_SYNC_ROOT_ID

# §13 Snapshot Metadata

  ## `snap_meta_ext_obj_phys_t`

  - [ ] *New datatype* `snap_meta_ext_obj_phys_t`

  ## `snap_meta_ext_t`

  - [ ] *New datatype* `snap_meta_ext_t`

# §14 B-Trees

  ## `btn_index_node_val_t`

  - [ ] *New datatype* `btn_index_node_val_t`

  ## B-Tree Flags

  - [ ] BTREE_HASHED
  - [ ] BTREE_NOHEADER

  ## B-Tree Node Flags

  - [ ] BTNODE_HASHED
  - [ ] BTNODE_NOHEADER

# §15 Encryption

  ## Protection Classes

  - [ ] PROTECTION_CLASS_M

  ## Encryption Identifiers

  - [ ] APFS_UNASSIGNED_CRYPTO_ID

  ## Keybag Tags

  - [ ] KB_TAG_WRAPPING_M_KEY
  - [ ] KB_TAG_VOLUME_M_KEY

# §16 Sealed Volumes

- [ ] *New chapter* Sealed Volumes

# §17 Space Manager

  ## `spaceman_free_queue_entry_t`

  - [ ] *New datatype* `spaceman_free_queue_entry_t`
