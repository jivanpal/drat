(command_explore-fs-tree)=

# {drat-command}`explore-fs-tree`

## Description

The {drat-command}`explore-fs-tree` command provides an interface via which you
can explore a filesystem B-tree (or subtree). You specify the B-tree either:

- directly, by specifying the block address of a B-tree (root) node via
  {argument}`fs`; or

- indirectly, by specifying sufficient info for the block address of a B-tree
  root node to be determined. That is, specify either:

  - the block address of a volume superblock via {argument}`fs`; or

  - a volume via {argument}`volume` or {argument}`volume-name`, optionally
    along with a maximum transaction ID via {argument}`max-xid` (the most
    recent filesystem tree with a transaction ID that doesn't exceed the one
    specified will be explored).

You must also specify an object map so that the Virtual object IDs used in the
filesystem tree to refer to other nodes within it can be resolved to block
addresses in order to locate the nodes. You specify the object map either
directly or indirectly as described in {drat-command}`explore-omap-tree`.

Direct and indirect specifications may be mixed, in which case direct
specifications take precedence. In other words, if a volume is specified via
{argument}`volume` or {argument}`volume-name`, the implied choice of volume
superblock can be overriden via {argument}`fs`, and the implied choice of object
map can be overridden via {argument}`omap`.

If both the filesystem tree and object map are unspecified, the default is
`--volume 1`; that is, the most recent filesystem tree of the first volume
will be explored using the corresponding object map. If one or the other is
specified, but not both, that's an error.

When the command is run, Drat will display the entries within the specified
B-tree (root) node, each of which is a key given by an (FSOID, FSRT) pair.
You will then be prompted to choose an entry by entering its index. Drat will
then read the corresponding child node and display its entries. You can repeat
this process of entry selection until you select an entry in a leaf node, after
which Drat will display info about the corresponding record in the tree. At any
point, you can enter `..` to go back to the parent node, or enter nothing to
exit Drat.

When viewing a non-leaf node, an extra set of columns, *Target child node*, is
shown, which displays the Virtual *OID* of the child node that the entry points
to, as well as the the *Block address* that the object map resolves that OID to,
and the *Actual OID* that appears in the data contained in that block. Following
each *Actual OID* is a checkbox `( )`, which will be checked `(X)` if that value
does not match the value in the *OID* column, indicating that the mapping is
bad. A well-formed object map will not result in any checkboxes being checked.

When viewing a leaf node, human-readable descriptions of the record type are
shown in the *FSRT* column, and extra data is shown for convenience based on the
record type. For example, if the record is a dentry, then the corresponding
inode's FSOID, type, and name are shown.

## Example usage and output

```
$ drat --container /dev/disk2s2 explore-fs-tree --fs 0xd297a --omap 0xd0146

Opening file at `/dev/disk0s2` in read-only mode ... OK.

Finding object mappings tree via block address: 0xd0146
Reading block 0xd0146 ... validating ... OK.
This is a B-tree root node for an object map.

Finding filesystem tree via block address: 0xd297a
Reading block 0xd297a ... validating ... OK.
This is a B-tree root node for a filesystem.
Height of tree is 4.
Node has 13 entries, as follows:
+-------+------------------+-----------------------------------------+
|       |           |      | Target child node                       |
| Index |     FSOID | FSRT |----------+---------------+--------------+
|       |           |      |      OID | Block address |   Actual OID |
+-------+-----------+------+----------+---------------+--------------+
|     0 |       0x1 | 0x9  |   0xdd29 |       0xed993 |   0xdd29 ( ) |
|     1 |   0xaf3a4 | 0x5  |  0x2f77e |       0xe0a41 |  0x2f77e ( ) |
|     2 |  0xa43cf2 | 0x3  | 0x243fa9 |       0xe2645 | 0x243fa9 ( ) |
|     3 |  0xd20da9 | 0x8  | 0x38715f |       0xd6fb4 | 0x38715f ( ) |
|     4 |  0xf4ff0c | 0x8  | 0x41fe57 |       0xe3780 | 0x41fe57 ( ) |
|     5 | 0x11f65da | 0x3  | 0x4f9bec |       0xdd44b | 0x4f9bec ( ) |
|     6 | 0x1314268 | 0x9  | 0x5629f5 |       0xe5a00 | 0x5629f5 ( ) |
|     7 | 0x1428c41 | 0x3  | 0x5cce9b |       0xeb99e | 0x5cce9b ( ) |
|     8 | 0x1499c33 | 0x4  | 0x5e8b16 |       0xebeec | 0x5e8b16 ( ) |
|     9 | 0x153f3be | 0x4  | 0x620615 |       0xdb928 | 0x620615 ( ) |
|    10 | 0x15f71df | 0x3  | 0x6627d0 |       0xd5e71 | 0x6627d0 ( ) |
|    11 | 0x1678e4e | 0x3  | 0x67e91f |       0xda643 | 0x67e91f ( ) |
|    12 | 0x16e1f95 | 0x3  | 0x690b02 |       0xd5c8f | 0x690b02 ( ) |
+-------+-----------+------+----------+---------------+--------------+
Current path: (_, _, _, _)
Choose an entry [.., 0-12]: 1

Child node has Virtual OID 0x2f77e, maps to block 0xe0a41.
Reading block 0xe0a41 ... validating ... OK.
Node has 94 entries, as follows:
+-------+-----------------+-----------------------------------------+
|       |          |      | Target child node                       |
| Index |    FSOID | FSRT |----------+---------------+--------------+
|       |          |      |      OID | Block address |   Actual OID |
+-------+----------+------+----------+---------------+--------------+
|     0 |  0xaf3a4 | 0x5  |  0x18ef1 |       0xd894c |  0x18ef1 ( ) |
|     1 |  0xaf9d9 | 0x8  |  0x18f71 |       0xdfe02 |  0x18f71 ( ) |
|     2 |  0xafcf1 | 0x3  |  0xdb001 |       0xe1c51 |  0xdb001 ( ) |
|     3 |  0xaffcc | 0x5  |  0x18ff2 |       0xe1c57 |  0x18ff2 ( ) |
|     4 |  0xb02b1 | 0x5  |  0xdb096 |       0xd8bd5 |  0xdb096 ( ) |
|     5 |  0xb06fe | 0x3  |  0x19070 |       0xd85aa |  0x19070 ( ) |
|     6 |  0xb0d79 | 0x4  |  0x190e4 |       0xd6ecd |  0x190e4 ( ) |
|     7 |  0xcb602 | 0x4  |  0x1975d |       0xd1b20 |  0x1975d ( ) |
|     8 |  0xce5e4 | 0x3  |  0x19a56 |       0xe3903 |  0x19a56 ( ) |
|     9 |  0xd7090 | 0x4  |  0x1cf1a |       0xe48b1 |  0x1cf1a ( ) |
|    10 |  0xd76b2 | 0x3  |  0x1cf35 |       0xe0160 |  0x1cf35 ( ) |
|    11 |  0xd96d8 | 0x9  |  0x1a7b7 |       0xe0167 |  0x1a7b7 ( ) |
|    12 |  0xda81f | 0x3  |  0x1a8af |       0xe0177 |  0x1a8af ( ) |
|    13 |  0xdb811 | 0x9  |  0x1e15b |       0xe01b0 |  0x1e15b ( ) |
|    14 |  0xdd711 | 0x4  |  0x22fbf |       0xdfbd3 |  0x22fbf ( ) |
|    15 |  0xddd50 | 0x3  |  0x1eb31 |       0xd9473 |  0x1eb31 ( ) |
|    16 |  0xde2f0 | 0x3  |  0x1ecaf |       0xe01cb |  0x1ecaf ( ) |
|    17 |  0xdf371 | 0x3  |  0x1f100 |       0xe01f0 |  0x1f100 ( ) |
|    18 |  0xe2866 | 0x3  |  0x1f4a1 |       0xdd298 |  0x1f4a1 ( ) |
|    19 | 0x143365 | 0x3  |  0x22bf3 |       0xd2e56 |  0x22bf3 ( ) |
|    20 | 0x1478f9 | 0x5  |  0x236c8 |       0xd54f0 |  0x236c8 ( ) |
|    21 | 0x1595c1 | 0x3  |  0x2c6eb |       0xd4c43 |  0x2c6eb ( ) |
|    22 | 0x15a089 | 0x9  |  0x2c78a |       0xe05f4 |  0x2c78a ( ) |
|    23 | 0x15af6b | 0x9  |  0x2cb92 |       0xe0654 |  0x2cb92 ( ) |
|    24 | 0x1799fd | 0x3  |  0x31de9 |       0xda18e |  0x31de9 ( ) |
|    25 | 0x179fcd | 0x3  |  0x31f0d |       0xdfca9 |  0x31f0d ( ) |
|    26 | 0x17a00a | 0x3  |  0x31e6c |       0xdf530 |  0x31e6c ( ) |
|    27 | 0x17a7c8 | 0x3  |  0x31ef9 |       0xda57f |  0x31ef9 ( ) |
|    28 | 0x1823fc | 0x9  |  0x32ddd |       0xe1075 |  0x32ddd ( ) |
|    29 | 0x182924 | 0x3  |  0x32e55 |       0xd2f36 |  0x32e55 ( ) |
|    30 | 0x183213 | 0x3  |  0x32f01 |       0xd89df |  0x32f01 ( ) |
|    31 | 0x1d683c | 0x4  |  0x5623a |       0xdf38d |  0x5623a ( ) |
|    32 | 0x22adf7 | 0x4  |  0x8e042 |       0xda2bc |  0x8e042 ( ) |
|    33 | 0x287b71 | 0x3  |  0x943cb |       0xe015b |  0x943cb ( ) |
|    34 | 0x2b7e2f | 0x4  |  0xb1dd7 |       0xe29e8 |  0xb1dd7 ( ) |
|    35 | 0x2d4ed2 | 0x6  |  0xb4769 |       0xda2c4 |  0xb4769 ( ) |
|    36 | 0x2e49cf | 0x3  |  0xbb071 |       0xd2541 |  0xbb071 ( ) |
|    37 | 0x2e4b7e | 0x9  |  0xba460 |       0xd250b |  0xba460 ( ) |
|    38 | 0x2ea142 | 0x8  |  0xbb558 |       0xe4aef |  0xbb558 ( ) |
|    39 | 0x2eb39d | 0x3  |  0xbbe75 |       0xdd2b8 |  0xbbe75 ( ) |
|    40 | 0x2eb86f | 0x3  |  0xbc079 |       0xd0355 |  0xbc079 ( ) |
|    41 | 0x2f7f6d | 0x3  |  0xc05f4 |       0xe76a6 |  0xc05f4 ( ) |
|    42 | 0x2f840b | 0x4  |  0xc0668 |       0xe74c2 |  0xc0668 ( ) |
|    43 | 0x2f89c5 | 0x4  |  0xc06de |       0xe73e0 |  0xc06de ( ) |
|    44 | 0x2f8f79 | 0x4  |  0xc0750 |       0xd9c21 |  0xc0750 ( ) |
|    45 | 0x2f956b | 0x4  |  0xc07ce |       0xe78c1 |  0xc07ce ( ) |
|    46 | 0x2f9af8 | 0x3  |  0xc0843 |       0xd2774 |  0xc0843 ( ) |
|    47 | 0x32815a | 0x3  |  0xc11b2 |       0xd394f |  0xc11b2 ( ) |
|    48 | 0x333c07 | 0x3  |  0xd0fd0 |       0xdf268 |  0xd0fd0 ( ) |
|    49 | 0x37fb42 | 0x3  |  0xe94a8 |       0xd9978 |  0xe94a8 ( ) |
|    50 | 0x3c4c65 | 0x3  |  0xffb5f |       0xda92e |  0xffb5f ( ) |
|    51 | 0x43e4d8 | 0x3  | 0x120fdb |       0xdfa52 | 0x120fdb ( ) |
|    52 | 0x47457f | 0x3  | 0x13ac2d |       0xdfbc9 | 0x13ac2d ( ) |
|    53 | 0x47491b | 0x9  | 0x13ac9b |       0xd78c1 | 0x13ac9b ( ) |
|    54 | 0x4afc54 | 0x8  | 0x13be67 |       0xd5ba8 | 0x13be67 ( ) |
|    55 | 0x4f0f95 | 0x3  | 0x160f9d |       0xdbfd5 | 0x160f9d ( ) |
|    56 | 0x4f168d | 0x3  | 0x161026 |       0xd526f | 0x161026 ( ) |
|    57 | 0x4f28d2 | 0x3  | 0x1610ae |       0xe5380 | 0x1610ae ( ) |
|    58 | 0x57c70b | 0x3  | 0x1843ab |       0xd9b9d | 0x1843ab ( ) |
|    59 | 0x57cba2 | 0x3  | 0x184a2b |       0xd9f1c | 0x184a2b ( ) |
|    60 | 0x5d8aca | 0x9  | 0x1a476d |       0xe630d | 0x1a476d ( ) |
|    61 | 0x5df9f8 | 0x9  | 0x1a53a9 |       0xe4202 | 0x1a53a9 ( ) |
|    62 | 0x5f4745 | 0x3  | 0x1a4f80 |       0xe43a2 | 0x1a4f80 ( ) |
|    63 | 0x5fdd5c | 0x3  | 0x1a538d |       0xe1940 | 0x1a538d ( ) |
|    64 | 0x6080bc | 0x3  | 0x1a5b6d |       0xd6b14 | 0x1a5b6d ( ) |
|    65 | 0x649eb8 | 0x9  | 0x1b82aa |       0xdf7a5 | 0x1b82aa ( ) |
|    66 | 0x66912c | 0x4  | 0x1bf13d |       0xe3927 | 0x1bf13d ( ) |
|    67 | 0x6b9ddc | 0x3  | 0x25c9fa |       0xd7e9f | 0x25c9fa ( ) |
|    68 | 0x72baa8 | 0x3  | 0x1fef96 |       0xdcdb8 | 0x1fef96 ( ) |
|    69 | 0x744bc1 | 0x3  | 0x20a607 |       0xd3c88 | 0x20a607 ( ) |
|    70 | 0x744dce | 0x3  | 0x20b9ca |       0xdd729 | 0x20b9ca ( ) |
|    71 | 0x744e02 | 0x8  | 0x209e5a |       0xd80ee | 0x209e5a ( ) |
|    72 | 0x7451fc | 0x3  | 0x209ec3 |       0xef591 | 0x209ec3 ( ) |
|    73 | 0x745a15 | 0x9  | 0x20a00a |       0xe5107 | 0x20a00a ( ) |
|    74 | 0x745ee1 | 0x8  | 0x20a08b |       0xe44dd | 0x20a08b ( ) |
|    75 | 0x746977 | 0x3  | 0x20a15b |       0xed8c4 | 0x20a15b ( ) |
|    76 | 0x7828dc | 0x3  | 0x216ad0 |       0xd81e7 | 0x216ad0 ( ) |
|    77 | 0x7ce9f1 | 0x3  | 0x22b142 |       0xd9644 | 0x22b142 ( ) |
|    78 | 0x803df5 | 0x3  | 0x2310fe |       0xdcab4 | 0x2310fe ( ) |
|    79 | 0x84846b | 0x9  | 0x25129a |       0xd413e | 0x25129a ( ) |
|    80 | 0x86d1b5 | 0x3  | 0x257c99 |       0xd7480 | 0x257c99 ( ) |
|    81 | 0x87b65b | 0x9  | 0x25bd0d |       0xdd6aa | 0x25bd0d ( ) |
|    82 | 0x88b49e | 0x3  | 0x25ca62 |       0xea7e0 | 0x25ca62 ( ) |
|    83 | 0x8fd89b | 0x9  | 0x2710ad |       0xe27aa | 0x2710ad ( ) |
|    84 | 0x8fdb17 | 0x4  | 0x2712c9 |       0xd1936 | 0x2712c9 ( ) |
|    85 | 0x8fdb44 | 0x3  | 0x271117 |       0xd6d76 | 0x271117 ( ) |
|    86 | 0x8fe224 | 0x3  | 0x2711a9 |       0xd518c | 0x2711a9 ( ) |
|    87 | 0x8fe3d4 | 0x4  | 0x27121e |       0xd5be7 | 0x27121e ( ) |
|    88 | 0x8feaff | 0x8  | 0x2712a2 |       0xd470d | 0x2712a2 ( ) |
|    89 | 0x94bf69 | 0x3  | 0x2818d9 |       0xd4c80 | 0x2818d9 ( ) |
|    90 | 0x995688 | 0x3  | 0x28875a |       0xdf433 | 0x28875a ( ) |
|    91 | 0x9cbb60 | 0x9  | 0x28be2b |       0xde6bd | 0x28be2b ( ) |
|    92 | 0x9e8db6 | 0x3  | 0x2a1ecf |       0xe0039 | 0x2a1ecf ( ) |
|    93 | 0xa21883 | 0x3  | 0x2a728c |       0xd9965 | 0x2a728c ( ) |
+-------+----------+------+----------+---------------+--------------+
Current path: (1, _, _, _)
Choose an entry [.., 0-93]: 30

Child node has Virtual OID 0x32f01, maps to block 0xd89df.
Reading block 0xd89df ... validating ... OK.
Node has 65 entries, as follows:
+-------+-----------------+-----------------------------------------+
|       |          |      | Target child node                       |
| Index |    FSOID | FSRT |----------+---------------+--------------+
|       |          |      |      OID | Block address |   Actual OID |
+-------+----------+------+----------+---------------+--------------+
|     0 | 0x183213 | 0x3  |  0x32efd |       0xd6494 |  0x32efd ( ) |
|     1 | 0x183bf7 | 0x3  |  0x32f89 |       0xd128c |  0x32f89 ( ) |
|     2 | 0x185464 | 0x3  |  0x33457 |       0xd9748 |  0x33457 ( ) |
|     3 | 0x18552b | 0x6  |  0x33564 |       0xe115b |  0x33564 ( ) |
|     4 | 0x187983 | 0x3  |  0x37adb |       0xd8f96 |  0x37adb ( ) |
|     5 | 0x18a6a4 | 0x6  |  0x3b3a0 |       0xe9e4c |  0x3b3a0 ( ) |
|     6 | 0x18a962 | 0x9  |  0x3b3ac |       0xde0dd |  0x3b3ac ( ) |
|     7 | 0x18a964 | 0x9  |  0x3b3f6 |       0xd32e1 |  0x3b3f6 ( ) |
|     8 | 0x18a96f | 0x3  |  0x3b3f9 |       0xe308d |  0x3b3f9 ( ) |
|     9 | 0x18a971 | 0x3  |  0x3b3f7 |       0xe3c1f |  0x3b3f7 ( ) |
|    10 | 0x18a985 | 0x3  |  0x3b3f8 |       0xe5deb |  0x3b3f8 ( ) |
|    11 | 0x18a999 | 0x3  |  0x3b3fc |       0xe57c0 |  0x3b3fc ( ) |
|    12 | 0x18ba67 | 0x9  |  0x3d295 |       0xd10e7 |  0x3d295 ( ) |
|    13 | 0x18ba6b | 0x3  |  0x3d293 |       0xd10e5 |  0x3d293 ( ) |
|    14 | 0x18ba80 | 0x3  |  0x3d294 |       0xd10e6 |  0x3d294 ( ) |
|    15 | 0x18ba93 | 0x4  |  0x3d296 |       0xe5a2d |  0x3d296 ( ) |
|    16 | 0x18e7c3 | 0x3  |  0x3d2ca |       0xea935 |  0x3d2ca ( ) |
|    17 | 0x18e7cb | 0x4  |  0x3d79b |       0xeae76 |  0x3d79b ( ) |
|    18 | 0x18e827 | 0x6  |  0x3d7b4 |       0xea93e |  0x3d7b4 ( ) |
|    19 | 0x18e8fb | 0x9  |  0x3d7b6 |       0xd13a6 |  0x3d7b6 ( ) |
|    20 | 0x18e8fb | 0x9  | 0x209c65 |       0xea942 | 0x209c65 ( ) |
|    21 | 0x18e91d | 0x3  |  0x3d7b9 |       0xeaa15 |  0x3d7b9 ( ) |
|    22 | 0x18e96c | 0x4  |  0x3d7c1 |       0xe4931 |  0x3d7c1 ( ) |
|    23 | 0x18e98f | 0x4  |  0x3d7ce |       0xe2c98 |  0x3d7ce ( ) |
|    24 | 0x1918c7 | 0x8  |  0x3ddca |       0xe6e13 |  0x3ddca ( ) |
|    25 | 0x19196b | 0x9  |  0x3de4d |       0xe3073 |  0x3de4d ( ) |
|    26 | 0x191973 | 0x4  |  0x3de50 |       0xdde67 |  0x3de50 ( ) |
|    27 | 0x191975 | 0x4  |  0x3de4e |       0xe3074 |  0x3de4e ( ) |
|    28 | 0x191989 | 0x4  |  0x3de4f |       0xe33b0 |  0x3de4f ( ) |
|    29 | 0x19199f | 0x3  |  0x3de51 |       0xeb69d |  0x3de51 ( ) |
|    30 | 0x191b1c | 0x3  |  0x3de81 |       0xd0831 |  0x3de81 ( ) |
|    31 | 0x191d0a | 0x6  |  0x3de9c |       0xda6a8 |  0x3de9c ( ) |
|    32 | 0x192004 | 0x6  |  0x3df11 |       0xd250c |  0x3df11 ( ) |
|    33 | 0x192170 | 0x3  |  0x3df2c |       0xd60aa |  0x3df2c ( ) |
|    34 | 0x193c56 | 0x8  |  0x42396 |       0xd1306 |  0x42396 ( ) |
|    35 | 0x197686 | 0x3  |  0x43f58 |       0xd08db |  0x43f58 ( ) |
|    36 | 0x197cd9 | 0x9  |  0x44247 |       0xde436 |  0x44247 ( ) |
|    37 | 0x19bacf | 0x9  |  0x446f7 |       0xe00c7 |  0x446f7 ( ) |
|    38 | 0x19badc | 0x9  |  0x44a7e |       0xe86f1 |  0x44a7e ( ) |
|    39 | 0x19baed | 0x6  |  0x44a7f |       0xd2a15 |  0x44a7f ( ) |
|    40 | 0x1a1150 | 0x3  |  0x453b0 |       0xd1322 |  0x453b0 ( ) |
|    41 | 0x1a2b18 | 0x3  |  0x4819d |       0xd15c1 |  0x4819d ( ) |
|    42 | 0x1a71f9 | 0x6  |  0x49491 |       0xf239e |  0x49491 ( ) |
|    43 | 0x1a71fe | 0x4  |  0x4c416 |       0xd10f9 |  0x4c416 ( ) |
|    44 | 0x1a720e | 0x3  |  0x4c417 |       0xd10fb |  0x4c417 ( ) |
|    45 | 0x1a7221 | 0x4  |  0x4c419 |       0xe6c90 |  0x4c419 ( ) |
|    46 | 0x1a7761 | 0x3  |  0x4c481 |       0xdefda |  0x4c481 ( ) |
|    47 | 0x1ae04f | 0x3  |  0x547cb |       0xdbf66 |  0x547cb ( ) |
|    48 | 0x1ae061 | 0x6  |  0x54c62 |       0xda59d |  0x54c62 ( ) |
|    49 | 0x1b2e00 | 0x3  |  0x55c2c |       0xd1737 |  0x55c2c ( ) |
|    50 | 0x1b33aa | 0x4  |  0x57d8b |       0xd6332 |  0x57d8b ( ) |
|    51 | 0x1b38a9 | 0x3  |  0x57e66 |       0xda582 |  0x57e66 ( ) |
|    52 | 0x1b3d5e | 0x3  |  0x57e67 |       0xe0759 |  0x57e67 ( ) |
|    53 | 0x1b5399 | 0x4  |  0x58248 |       0xe20e8 |  0x58248 ( ) |
|    54 | 0x1bb366 | 0x3  |  0x5d1dd |       0xd027e |  0x5d1dd ( ) |
|    55 | 0x1bf693 | 0x8  |  0x62440 |       0xea807 |  0x62440 ( ) |
|    56 | 0x1d401c | 0x6  |  0x658b6 |       0xd7efd |  0x658b6 ( ) |
|    57 | 0x1d4029 | 0x8  |  0x65f22 |       0xea6ef |  0x65f22 ( ) |
|    58 | 0x1d4037 | 0x8  |  0x65f23 |       0xd7fc2 |  0x65f23 ( ) |
|    59 | 0x1d4299 | 0x3  |  0x65f29 |       0xdf4a7 |  0x65f29 ( ) |
|    60 | 0x1d45a8 | 0x4  |  0x65f82 |       0xddb2a |  0x65f82 ( ) |
|    61 | 0x1d45ae | 0x3  |  0x65fae |       0xea73e |  0x65fae ( ) |
|    62 | 0x1d45b9 | 0x4  |  0x65faf |       0xde792 |  0x65faf ( ) |
|    63 | 0x1d45d2 | 0x8  |  0x65fb1 |       0xe961e |  0x65fb1 ( ) |
|    64 | 0x1d5e9c | 0x3  |  0x67303 |       0xe852e |  0x67303 ( ) |
+-------+----------+------+----------+---------------+--------------+
Current path: (1, 30, _, _)
Choose an entry [.., 0-64]: 17

Child node has Virtual OID 0x3d79b, maps to block 0xeae76.
Reading block 0xeae76 ... validating ... OK.
Node has 30 entries, as follows:
+-------+----------+------------------+
| Index |    FSOID | FSRT             |
+-------+----------+------------------+
|     0 | 0x18e7cb | 0x4, xattr       |
|     1 | 0x18e7cc | 0x3, inode       |
|     2 | 0x18e7cc | 0x4, xattr       |
|     3 | 0x18e7cc | 0x6, dstream     |
|     4 | 0x18e7cc | 0x8, file extent |
|     5 | 0x18e7cd | 0x3, inode       |
|     6 | 0x18e7cd | 0x4, xattr       |
|     7 | 0x18e7cd | 0x6, dstream     |
|     8 | 0x18e7cd | 0x8, file extent |
|     9 | 0x18e825 | 0x3, inode       |
|    10 | 0x18e825 | 0x4, xattr       |
|       |          |                  |+-------------+----------------+--------------------+
|       |          |                  || Inode FSOID | Inode type     | Inode name         |
|       |          |                  |+-------------+----------------+--------------------+
|    11 | 0x18e825 | 0x9, dentry      ||   0x169da65 | 0x4, directory | 83bc0baa3881f265_0 |
|    12 | 0x18e825 | 0x9, dentry      ||   0x169da72 | 0x4, directory | d3c975fa1a9b20f6_0 |
|    13 | 0x18e825 | 0x9, dentry      ||   0x169da6b | 0x4, directory | df7b00085ba675ef_0 |
|    14 | 0x18e825 | 0x9, dentry      ||    0x18e828 | 0x4, directory | index-dir          |
|    15 | 0x18e825 | 0x9, dentry      ||   0x169da6f | 0x4, directory | f3faf2ae921d9bf5_0 |
|    16 | 0x18e825 | 0x9, dentry      ||   0x169da71 | 0x4, directory | 41a6c187467c47e1_0 |
|    17 | 0x18e825 | 0x9, dentry      ||   0x169da6c | 0x4, directory | 9b7fc9e7916c2c7c_0 |
|    18 | 0x18e825 | 0x9, dentry      ||   0x169da73 | 0x4, directory | a20bc29b43958149_0 |
|    19 | 0x18e825 | 0x9, dentry      ||   0x169da70 | 0x4, directory | 9bffce5feb2725f3_0 |
|    20 | 0x18e825 | 0x9, dentry      ||   0x169da67 | 0x4, directory | fd78b4182656630a_0 |
|    21 | 0x18e825 | 0x9, dentry      ||   0x169da66 | 0x4, directory | fc39fc87aa0a667a_0 |
|    22 | 0x18e825 | 0x9, dentry      ||   0x169da6a | 0x4, directory | 083f607b0ffe8511_0 |
|    23 | 0x18e825 | 0x9, dentry      ||   0x169da64 | 0x4, directory | d4e4cca6484c1eb2_0 |
|    24 | 0x18e825 | 0x9, dentry      ||   0x169da68 | 0x4, directory | c15c8b176db7917c_0 |
|    25 | 0x18e825 | 0x9, dentry      ||    0x18e827 | 0x4, directory | index              |
|    26 | 0x18e825 | 0x9, dentry      ||   0x169da69 | 0x4, directory | 1b12bbc9cb4a5267_0 |
|    27 | 0x18e825 | 0x9, dentry      ||   0x169da74 | 0x4, directory | a28d674c00625ded_0 |
|       |          |                  |+-------------+----------------+--------------------+
|    28 | 0x18e827 | 0x3, inode       |
|    29 | 0x18e827 | 0x4, xattr       |
+-------+----------+------------------+
Current path: (1, 30, 17, _)
Choose an entry [.., 0-29]: 13

Record data for entry 13:
  METADATA:
    - Key size:           31 bytes
    - Value size:         18 bytes
    - ID and type field:  0x900000000018e825
  KEY:
    - Virtual OID:    0x18e825
    - Object type:    Directory entry
  VALUE:
    - Dentry name length:     19 UTF-8 bytes (including terminating NULL (U+0000) byte)
    - Dentry name hash:       0x06afe3
    - Dentry name:            df7b00085ba675ef_0
    - Dentry Virtual OID:     0x169da6b
    - Time added:             Wed Mar  3 15:17:33 2021
    - Dentry type:            Regular file
    - No. extended fields:    0
Current path: (1, 30, 17, 31)
Choose an entry [..]: 
```
