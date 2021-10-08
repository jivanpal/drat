# Drat changelog

## v0.1.3 (2021-08-29)

### Functional changes

- Linux support added (tested under Ubuntu 20.04.3). From this version onwards,
  binaries for Linux on x86-64 will also be provided for versioned releases.
  Some more general code portability provisions have also been made.

- `drat inspect` now automatically detects the APFS block size, based on the
  block size value provided by the container superblock found at block 0.

- Extended fields (xfields) are now supported.

- `drat recover` now gets the actual file size and thus outputs files of the
  correct size rather than a multiple of the APFS block size. The tools in
  `/supplemental-tools` have thus been removed. ([Issue #2](https://github.com/jivanpal/drat/issues/2)).

- UUIDs are now printed in standard format.

- Fixed bug in `get_nx_incompatible_features_string()` and `get_nx_flags_string()`
  that caused these functions to analyse the `nx_features` field instead.

- `drat version` now outputs legal info (copyright notice, warranty, and
  licence).

### Other changes

- Refactored libraries:

  - `/include` has been added to the compilation include path to avoid use of
    relative paths.

  - `/src/apfs/struct` has moved to `/include/apfs`, and can thus be included
    as `<apfs>`. This library contains headers that define all data structures
    detailed in Apple's APFS specfication.

  - The remainder of `/src/apfs` has moved to `/include/drat`, and can thus be
    included as `<drat>`. This library contains functionality specific to Drat:
    
    - Headers in `<drat/func>` define functions that facilitate common
      operations on data strctures provided by `<apfs>`;

    - Headers in `<drat/string>` define functions that produce human-readable
      strings/output detailing the data contained in data structures provided
      by `<apfs>`; and

    - Headers in `<drat>` define miscellaneous functions.

- Refactored code:

  - String generation functions that operate on enum fields have had their
    shared code factored into `<drat/string/common.h>` as
    `get_single_enum_string()` and `get_flags_enum_string()`.

  - Use of `ctime()` for printing timestamps has been factored into
    `get_apfs_timestamp_string()`.

- Documentation for v0.2.x has been drafted in `/docs` using
  [Sphinx](https://www.sphinx-doc.org/en/master/).

## v0.1.2 (2021-02-05)

- Renamed `src/apfs/struct/const.h` to `src/apfs/struct/jconst.h`

- Updated symbol/struct definitions in `src/apfs/struct` to align with the
  latest APFS spec revision (2020-06-22). This included adding a new file
  `sealed.h` with the definitions contained in the new chapter of the spec,
  §16 Sealed Volumes. A detailed list of changes follows:

  - §3 Objects — `object.h`

    - §3.1 Object Types

      - `OBJECT_TYPE_ER_RECOVERY_BLOCK`
      - `OBJECT_TYPE_SNAP_META_EXT`
      - `OBJECT_TYPE_INTEGRITY_META`
      - `OBJECT_TYPE_FEXT_TREE`
      - `OBJECT_TYPE_RESERVED_20`
      - `OBJECT_TYPE_MEDIA_KEYBAG`

  - §5 Container — `nx.h`

    - §5.2 `nx_superblock_t`

      - `nx_newest_mounted_version`
      - `nx_mkb_locker`

  - §6 Object Maps — `omap.h`

    - §6.7 Object Map Flags

      - `OMAP_VALID_FLAGS`

  - §7 Volumes — `fs.h`

    - §7.1 `apfs_superblock_t`

      - `apfs_cloneinfo_id_epoch`
      - `apfs_cloneinfo_xid`
      - `apfs_snap_meta_ext_oid`
      - `apfs_volume_group_id`
      - `apfs_integrity_meta_oid`
      - `apfs_fext_tree_oid`
      - `apfs_fext_tree_type`
      - `reserved_type`
      - `reserved_oid`

    - §7.3 Volume Flags

      - `APFS_FS_RESERVED_80`
      - `APFS_FS_RESERVED_100`
      - `APFS_FS_FLAGS_VALID_MASK` (name corrected from erroneous `APFS_FS_FALGS_VALID_MASK`)

    - §7.4 Volume Roles

      - `APFS_VOL_ROLE_UPDATE`
      - `APFS_VOL_ROLE_XART`
      - `APFS_VOL_ROLE_HARDWARE`
      - `APFS_VOL_ROLE_BACKUP`
      - `APFS_VOL_ROLE_RESERVED_7`
      - `APFS_VOL_ROLE_RESERVED_8` (name changed from `APFS_VOL_ROLE_RESERVED_200`)
      - `APFS_VOL_ROLE_ENTERPRISE`
      - `APFS_VOL_ROLE_RESERVED_10`
      - `APFS_VOL_ROLE_PRELOGIN`

    - §7.5 Optional Volume Feature Flags

      - `APFS_FEATURE_STRICTATIME`
      - `APFS_FEATURE_VOLGRP_SYSTEM_INO_SPACE`

    - §7.7 Incompatible Volume Feature Flags

      - `APFS_INCOMPAT_INCOMPLETE_RESTORE`
      - `APFS_INCOMPAT_SEALED_VOLUME`

  - §8 File-System Objects — `j.h`

    - §8.1 `j_key_t`

      - `SYSTEM_OBJ_ID_MARK`

    - §8.3 `j_inode_val_t`

      - `uncompressed_size`

  - §9 File-System Constants — `jconst.h`

    - §9.1 `j_obj_types`

      - `APFS_TYPE_FILE_INFO`

    - §9.3 `j_inode_flags`

      - `INODE_FAST_PROMOTE`
      - `INODE_HAS_UNCOMPRESSED_SIZE`
      - `INODE_IS_PURGEABLE`
      - `INODE_WANTS_TO_BE_PURGEABLE`
      - `INODE_IS_SYNC_ROOT`
      - `INODE_SNAPSHOT_COW_EXEMPTION`

    - §9.6 Inode Numbers

      - `PURGEABLE_DIR_INO_NUM`
      - `UNIFIED_ID_SPACE_MARK`

    - §9.7 Extended Attributes Constants

      - `FIRMLINK_EA_NAME`
      - `APFS_COW_EXEMPT_COUNT_NAME`

  - §11 Extended Fields — `xf.h`

    - §11.3 Extended-Field Types

      - `INO_EXT_TYPE_PURGEABLE_FLAGS`
      - `INO_EXT_TYPE_ORIG_SYNC_ROOT_ID`

  - §13 Snapshot Metadata — `snap.h`

    - *New datatype*: §13.6 `snap_meta_ext_obj_phys_t`
    - *New datatype*: §13.7 `snap_meta_ext_t`

  - §14 B-Trees — `btree.h`

    - *New datatype*: §14.4 `btn_index_node_val_t`

    - §14.8 B-Tree Flags

      - `BTREE_HASHED`
      - `BTREE_NOHEADER`

    - §14.10 B-Tree Node Flags

      - `BTNODE_HASHED`
      - `BTNODE_NOHEADER`

  - §15 Encryption — `crypto.h`

    - §15.7 Protection Classes

      - `PROTECTION_CLASS_M`

    - §15.8 Encryption Identifiers

      - `APFS_UNASSIGNED_CRYPTO_ID`

    - §15.12 Keybag Tags

      - `KB_TAG_WRAPPING_M_KEY`
      - `KB_TAG_VOLUME_M_KEY`

  - *New chapter*: §16 Sealed Volumes — `sealed.h`

    - *New datatype*: §16.1 `integrity_meta_phys_t`

    - §16.2 Integrity Metadata Version Constants

      - `INTEGRITY_META_VERSION_INVALID`
      - `INTEGRITY_META_VERSION_1`
      - `INTEGRITY_META_VERSION_2`
      - `INTEGRITY_META_VERSION_HIGHEST`

    - §16.3 Integrity Metadata Flags

      - `APFS_SEAL_BROKEN`

    - §16.4 `apfs_hash_type_t`

      - `APFS_HASH_INVALID`
      - `APFS_HASH_SHA256`
      - `APFS_HASH_SHA512_256`
      - `APFS_HASH_SHA384`
      - `APFS_HASH_SHA512`
      - `APFS_HASH_MIN`
      - `APFS_HASH_MAX`
      - `APFS_HASH_DEFAULT`

    - *New datatype*: §16.5 `fext_tree_key_t`

    - *New datatype*: §16.6 `fext_tree_val_t`

    - *New datatype*: §16.7 `j_file_info_key_t`

      - `J_FILE_INFO_LBA_MASK`
      - `J_FILE_INFO_TYPE_MASK`
      - `J_FILE_INFO_TYPE_SHIFT`

    - *New datatype*: §16.8 `j_file_info_val_t`

    - §16.9 `j_obj_file_info_type`

      - `APFS_FILE_INFO_DATA_HASH`

    - *New datatype*: §16.10 `j_file_data_hash_val_t`

  - §17 Space Manager — `spaceman.h`

    - *New datatype*: §17.4 `spaceman_free_queue_entry_t`

- Updated existing functions in `src/apfs/string` to handle new symbols:

  - In `btree.c`:
    - `get_btn_flags_string()`
    - `get_bt_info_flags_string()`

  - In `fs.c`:
    - `get_apfs_features_string()`
    - `get_apfs_incompatible_features_string()`
    - `get_apfs_fs_flags_string()`
    - `get_apfs_role_string()`
    - `print_apfs_superblock()`

  - In `j.c`
    - `get_j_inode_internal_flags()`
    - `print_j_inode_val()`

  - In `nx.c`:
    - `get_nx_flags_string()`
    - `print_nx_superblock()`

  - In `object.c`:
    - `get_o_type_string()`
    - `get_o_subtype_string()`

## v0.1.1 (2021-02-02)

- Fixed bug in `get_o_type_string()` where the "Unknown type" output did not
  correctly display the hex value of the type field
  (`o_type & OBJECT_TYPE_MASK`).

- Re-implemented (and hopefully fixed all bugs in) the B-tree routines in
  `src/apfs/func/btree.c`. Of particular note:

  - `get_btree_phys_omap_val()` is deprecated and replaced with
    `get_btree_phys_omap_entry()`, which allows the caller to see the XID of
    the returned entry.

  - `get_fs_records()` is now more efficient, as it no longer makes multiple
    descents from the root node just to access multiple records in the same leaf
    node. Instead, we now just walk along the leaf node, making new descents
    only when we need to access a different node.

## v0.1.0 (2021-01-26)

Initial release, basically the same as the prior `apfs-tools` commands, but packaged into a single `drat` command. Recognised commands and their functionality are highly subject to change for now. The current list of commands is:

```
List of commands:
   explore-fs-tree         Explore filesystem B-tree
   explore-omap-tree       Explore object map B-tree
   inspect                 Inspect APFS partition
   list-raw                List directory contents or file info based on its filesystem OID
   list                    List directory contents or file info based on its filepath
   read                    Read a block and display information about it
   recover-raw             Recover a file based on its filesystem OID
   recover                 Recover a file bsaed on its filepath
   resolver                Check if given Virtual OIDs resolve to given Physical OIDs
   search-last-btree-node  Search the partition for B-tree nodes, reporting the Physical OID of the last one discovered
   search                  Search the partition for blocks with certain features/properties
   version                 Display Drat's version info
```
