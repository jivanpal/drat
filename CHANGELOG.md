# Drat changelog

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
