# Changelog for v0.1.0 (2021-01-26)

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
