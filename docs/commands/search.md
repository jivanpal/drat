(command_search)=

# {drat-command}`search`

The {drat-command}`search` command allows you to scan the filesystem for
particular data, such as blocks of particular types or with particular IDs, or
blocks containing specific data, such as object maps nodes containing particular
mappings, or filesystem records for particular FSOIDs or item names.

Performing a search involves specifying one or more search criteria/parameters,
ecah of which has its own flag — these are listed below. Some flags are only
defined/make sense in certain contexts, so if that context is not explicitly
specified, it is implied; or if a conflicting contxt is specified, an error
occurs. For example, searching for particular object mappings only makes sense
if we're looking at blocks whose type is a B-tree (`btree` or `btree-node`) and
whose subtype is an object map (`omap`). Thus, specifying `--omap-key-oid 0x123`
implies `--type btree --subtype omap`. Specifying conflicting
contexts, as in `drat search --type fs --omap-key-oid 0x123`, results in an
error.

The list of values passed to a parameter can be comma-delimited
(e.g. `--type omap,fs`), which indicates that any of the values should match
(logical OR); or plus-delimited (e.g. `--type btree-leaf+omap-tree`), which
indicates that all of the values should match (logical AND).

Drat can create an index of the filesystem in advance in order to make searching
quicker (see {drat-command}`create-index`). This index is stored in a file, and
you pass it to Drat with {argument}`index`. If no index is specified or if the
index doesn't contain required data, Drat will scan through the filesystem
itself to get this data.

## Search parameters

### Top-level parameters

A list of top-level search parameters follows:

- `--oid` — The object ID. Use `--storage-type` to filter to Physical OIDs,
  Ephemeral OIDs, or Virtual OIDs.

- `--xid` — The transaction ID. This value is matched exactly. For a wider
  search, specify a comma-delimited list of IDs (e.g. `--xid 0x1,0x3,0x1b`) or
  a range in the form `[min xid]-[max xid]` (e.g. `--xid 0x1b-0x2c`), or a
  combination of these (e.g. `--xid 0x1-0x3,0x5,0x1b-0x2c`).

- `--storage-type` — The object storage type. Valid values are `physical`,
  `ephemeral`, and `virtual`.

- `--type` — The object type/subtype. See *Object types* below for a list of
  valid keyword values. This can also be a numeric value (decimal or
  hexadecimal), which can be useful in order to search for reserved types
  (e.g. `0x20`, `24`), or types that this version of Drat is not aware of. Some
  of the supported keyword values actually reference *subtypes*, but all of the
  currently defined subtypes imply a (super-)type of `btree`, so in practice
  there is no need for a separate search parameter. If, for whatever reason, you
  want to specify a hexadecimal subtype, you can use a slash `/` to delimit it
  from the (super-)type. For example:

  - `--type 0x20` is an object with (super-)type `0x20`.
  - `--type 0x20/0x30` is an object with (super-)type `0x20` and subtype `0x30`.
  - `--type /0x30` is an object with subtype `0x30` and any (super-)type.

### Object types

A table of valid keyword values for the `--type` parameter follows:

| Keyword                       | Equivalent value                  | Description   |
| :--                           | :--                               | :--           |
| `invalid`                     | `0x0`                             | The "invalid" object type |
| `test`                        | `0xff`                            | The reserved "testing" object type |
| `nxsb`                        | `0x1`                             | Container superblock |
| `btree`                       | `btree-root,btree-nonroot`        | Any B-tree node |
| `btree-root`                  | `0x2`                             | B-tree root node |
| `btree-nonroot`               | `0x3`                             | B-tree non-root node |
| `btree-leaf`                  | `--type btree --btree-flags leaf` | B-tree leaf node |
| `spaceman`                    | `0x5`                             | Spaceman (space manager) block |
| `spaceman-cab`                | `0x6`                             | Spaceman chunk-info address block |
| `spaceman-cib`                | `0x7`                             | Spaceman chunk-info block |
| `spaceman-bitmap`             | `0x8`                             | Spaceman free-space bitmap |
| `spaceman-free-queue-tree`    | `btree/0x9`                       | Spaceman free-space queue |
| `extent-list-tree`            | `btree/0xa`                       | Extents-list tree |
| `omap`                        | `0xb`                             | Omap (object map) |
| `omap-tree`                   | `btree/0xb`                       | Any omap tree node |
| `omap-tree-leaf`              | `btree-leaf/0xb`                  | Omap tree leaf node |
| `xp-map`                      | `0xc`                             | Checkpoint map |
| `fs`                          | `0xd`                             | Volume superblock |
| `fs-tree`                     | `btree/0xe`                       | Any filesystem records tree node |
| `fs-tree-leaf`                | `btree-leaf/0xe`                  | Filesystem records tree leaf node |
| `block-ref-tree`              | `btree/0xf`                       | Extent reference tree |
| `snap-meta-tree`              | `btree/0x10`                      | Volume snapshot metadata tree |
| `reaper`                      | `0x11`                            | Reaper |
| `reaper-list`                 | `0x12`                            | Reaper list |
| `omap-snapshot-tree`          | `btree/0x13`                      | Omap snapshot tree |
| `efi-jumpstart`               | `0x14`                            | EFI jumpstart block |
| `fusion-middle-tree`          | `btree/0x15`                      | Fusion drive middle-tree (tracks HDD blocks that are cached on SSD) |
| `fusion-wbc`                  | `0x16`                            | Fusion drive write-back cache state |
| `fusion-wbc-list`             | `0x17`                            | Fusion drive write-back cache list |
| `er-state`                    | `0x18`                            | Encryption rolling state |
| `gbitmap`                     | `0x19`                            | General-purpose bitmap |
| `gbitmap-tree`                | `btree/0x1a`                      | B-tree of general-purpose bitmaps |
| `gbitmap-block`               | `0x1b`                            | Block containing a general-purpose bitmap |
| `er-recovery-block`           | `0x1c`                            | Encryption-rolling recovery block |
| `snap-meta-ext`               | `0x1d`                            | Additional snapshot metadata |
| `integrity-meta`              | `0x1e`                            | Integrity metadata object |
| `fext-tree`                   | `btree/0x1f`                      | B-tree of file extents |
| `container-keybag`            | `0x6b657973` (`'keys'`)           | Container keybag |
| `volume-keybag`               | `0x72656373` (`'recs'`)           | Volume keybag |
| `media-keybag`                | `0x6d6b6579` (`'mkey'`)           | Media keybag |

### Context-specific parameters

The following sections list search parameters that are only valid in certain contexts.

#### B-tree nodes (`--type btree`)

```{todo} Describe this
```

#### B-tree root nodes (`--type btree-root`)

```{todo} Describe this
```

#### Omap trees (`--type omap-tree[-leaf]`)

The following `--omap-key-*` parameters can be used with `--type omap-tree` in
general, but it's usually only useful to use them with `--type omap-tree-leaf`,
since you're probably intersted in finding particular object mappings (pointers
to objects), not pointers to other omap tree nodes:

- `--omap-key-oid` — A Virtual OID that makes up an (OID, XID) key-pair.

- `--omap-key-xid` — An XID that makes up an (OID, XID) key-pair.

The following `--omap-val-*` parameters can only be used with
`--type omap-tree-leaf`:

- `--omap-val-paddr` — The block address that some (OID, XID) key-pair maps to.

#### Filesystem trees (`--type fs-tree[-leaf]`)

The following parameters can be used with `--type fs-tree` in general, but it's
usually only useful to use them with `--type fs-tree-leaf`, since you're
probably interested in finding particular filesystem records, not the
Virtual OIDs of other filesystem tree nodes:

- `--fsoid` — Filesystem object ID (a.k.a inode number). The ID of a filesystem
  object whose records you wish to search for.

- `--fsrt` — Filesystem record type. The type of filesystem records that you
  wish to search for. See *Filesystem record types* below for a list of valid
  keyword values.

##### Filesystem record types

| Keyword           | Equivalent value  | Description   |
| :--               | :--               | :--           |
| `any`             | `0x0`             | Any type |
| `snap-meta`       | `1`               | Snapshot metadata |
| `extent`          | `2`               | Physical extent record |
| `inode`           | `3`               | Inode |
| `xattr`           | `4`               | Extended attribute |
| `sibling-link`    | `5`               | Sibling link; a mapping from target inode to source hard links which point to that target |
| `dstream`         | `6`               | Data stream |
| `crypto`          | `7`               | Per-file encryption state |
| `file-extent`     | `8`               | File extent record |
| `dentry`          | `9`               | Dentry (directory entry) |
| `dir-stats`       | `10`, `0xa`       | Directory information/statistics |
| `snap-name`       | `11`, `0xb`       | Snapshot name |
| `sibling-map`     | `12`, `0xc`       | Sibling map; a mapping from hard link to target inode |
| `file-info`       | `13`, `0xd`       | Additional info about file data |
| `invalid`         | `15`, `0xf`       | The "invalid" record type |

##### Record-specific parameters

This section describes parameters specific to each record type. They can only be
used in conjunction with `--type fs-tree-leaf`.

###### Dentries (`--fsrt dentry`)

The following search parameters can only be used in conjunction with
`--fsrt dentry`, since they are specific to dentries (directory entry records):

- `--dentry-name` — The name of the item. Be careful to escape any commas in the
  name with backslash `\`, else they will be interpreted as logical OR
  operators. For example, compare `--dentry-name 'Martian, The'` and
  `--dentry-name 'Martian\, The'`: the latter will search for items named
  `Martian, The`, whereas the former will search for items named `Martian` or
  ` The` (note also the leading space since leading and trailing whitespace is
  not ignored).

- `--dentry-fsoid` — The FSOID (a.k.a. inode number, file ID) of the item.
