(command_explore-omap-tree)=

# {drat-command}`explore-omap-tree`

## Description

The {drat-command}`explore-omap-tree` command provides an interface via which you
can explore an object map B-tree (or subtree). You specify the B-tree either:

- directly, by specifying the block address of a B-tree (root) node via
  {argument}`omap`; or

- indirectly, by specifying sufficient info for the block address of a B-tree
  root node to be determined. That is, specify either:

  - the block address of an object map (a data structure of type
    {datatype}`omap_phys_t`), volume superblock, or container superblock via
    {argument}`omap`; or

  - a volume or the container itself via {argument}`volume` or
    {argument}`volume-name`, optionally along with a maximum transaction ID via
    {argument}`max-xid` (the most recent object map with a transaction ID that
    doesn't exceed the one specified will be explored).

If no such arguments are specified, Drat assumes `--volume -1 --max-xid -1`.
That is, it will explore the most recent container object map B-tree.

When the command is run, Drat will display the entries within the specified
B-tree (root) node, each of which is a key given by an (OID, XID) pair. You
will then be prompted to choose an entry by specifying its index and pressing
ENTER. Drat will then read the corresponding child node and display its
entries. You can repeat this process of entry selection until you select an
entry in a leaf node, after which Drat will display info about the
corresponding record in the tree. Pressing ENTER at this point will exit Drat.
At any point, you can specify `..` instead of a valid entry index to go back to
the parent node.

When viewing a non-leaf node, an extra column, *Target child node*, is shown
for convenience, which displays the address of the child node that the entry
points to.

When viewing a leaf node, some extra columns are shown for convenience.
The *Target block* column shows what block address the given (OID,&nbsp;XID)
pair maps to. The *Target's actual OID* and *Target's actual XID* columns
display the OID and XID that appear in the actual data contained in that block.
Following each of these values appears a checkbox, `( )`. If the actual OID/XID
does not match the expected OID/XID (the OID/XID which the object map had
mapped in the first place to the address of the block being examined), then the
checkbox will be filled with an X, `(X)`, to highlight the fact that this
mapping is bad. A proper object map will have no such filled checkboxes.

## Example usage

```
$ drat --container /dev/disk2s2 explore-omap-tree

$ drat --container partition-dump.bin explore-omap-tree --volume 2

$ drat --container /dev/disk2s2 explore-omap-tree --volume-name Preboot --max-xid 0x56f2e00

$ drat --container partition-dump.bin explore-omap-tree --omap 0xd1395
```

## Example output

```
$ drat --container partition-dump.bin explore-omap-tree --omap 0xd1395

Opening file at `partition-dump.bin` in read-only mode ... OK.

Reading block 0xd1395 ... validating ... OK.
This is a B-tree root node.
Height of tree is 3.
Node has 11 entries, as follows:
+-------+----------+----------+-------------------+
| Index |      OID |      XID | Target child node |
+-------+----------+----------+-------------------+
|     0 |    0x404 | 0x620846 |    0x100000000012 |
|     1 |  0x1a62b |   0x1ac2 |           0xd8384 |
|     2 | 0x1a42d1 | 0x5fe4fc |           0xd3dff |
|     3 | 0x3a3770 | 0x615572 |           0xd6d35 |
|     4 | 0x421d21 | 0x305a6f |           0xd7c77 |
|     5 | 0x52f2b1 | 0x4034b2 |           0x8ffff |
|     6 | 0x56cae8 | 0x49bb2b |           0x80028 |
|     7 | 0x5ea464 | 0x5ef837 |           0xd6445 |
|     8 | 0x636ea0 | 0x5e61d0 |           0xd30cc |
|     9 | 0x66b83b | 0x62031d |           0xd3dec |
|    10 | 0x67fea8 | 0x5fe60c |           0xd6ac4 |
+-------+----------+----------+-------------------+
Current path: (_, _, _)
Choose an entry [0-10]: 1

Child node is block 0xd663a.
Reading block 0xd663a ... validating ... OK.
Node has 91 entries, as follows:
+-------+----------+----------+-------------------+
| Index |      OID |      XID | Target child node |
+-------+----------+----------+-------------------+
|     0 |  0x1a62b |   0x1ac2 |           0xd6e95 |
|     1 |  0x1a921 |   0x1a97 |           0x80188 |
|     2 |  0x1abbf |   0x1ad0 |           0x80470 |
|     3 |  0x1adae |   0x1ad0 |           0xd6e47 |
|     4 |  0x1ae09 |   0x1a97 |           0xd6e4a |
|     5 |  0x1ae78 |   0x1a97 |           0xd6e53 |
|     6 |  0x1af64 | 0x5d5d2b |           0x80368 |
|     7 |  0x1b1dc |   0x1d3c |           0xd6e91 |
|     8 |  0x1dd86 |   0x1ad0 |           0xe2803 |
|     9 |  0x1ee21 |   0x1acb |           0xeb460 |
|    10 |  0x22bc3 |   0x1d38 |           0xe4433 |
|    11 |  0x22c80 | 0x222913 |           0x80420 |
|    12 |  0x28c5c | 0x3b52cd |           0x80230 |
|    13 |  0x2c70f |   0x6a29 |           0x803e0 |
|    14 |  0x2c930 | 0x5881b1 |           0xd6a38 |
|    15 |  0x2cc08 | 0x620345 |           0xd5df1 |
|    16 |  0x31de5 |  0x4b5fc |           0xdb797 |
|    17 |  0x31e20 |  0x4b5fc |           0xcfe3e |
|    18 |  0x31e90 |  0x4b5fc |           0xd5533 |
|    19 |  0x31efe |  0x4b5fc |           0x800f0 |
|    20 |  0x32d0b | 0x615572 |           0xd6029 |
|    21 |  0x32e1b |   0xaac2 |           0xe0a07 |
|    22 |  0x32e60 | 0x5d5d2a |           0xd672a |
|    23 |  0x32efd | 0x3c0651 |           0xe74f4 |
|    24 |  0x4c481 | 0x5fa6c4 |           0x80460 |
|    25 |  0x799da | 0x61d90c |           0x80450 |
|    26 |  0x92a7f | 0x3c0550 |           0x803f0 |
|    27 |  0xb137a | 0x34a74c |           0x800e8 |
|    28 |  0xb7074 | 0x61da8a |           0xcfd16 |
|    29 |  0xbb0e4 |  0x4c663 |           0xd55d1 |
|    30 |  0xbb413 |  0x4ce5e |           0xe029f |
|    31 |  0xbb4d3 |  0x4ce45 |           0xd5adb |
|    32 |  0xbbcc3 |  0x4c947 |           0xda3c0 |
|    33 |  0xbbffa |  0x4ce72 |           0x80238 |
|    34 |  0xbc03b |  0x4ce45 |           0xe6aee |
|    35 |  0xbda1c | 0x3c066c |           0xd0084 |
|    36 |  0xc060b |  0x4b5fc |           0x80468 |
|    37 |  0xc0648 |  0x4b5fc |           0xe71a1 |
|    38 |  0xc06a6 |  0x4b5fc |           0x80170 |
|    39 |  0xc06e7 | 0x31a219 |           0xe2b32 |
|    40 |  0xc0726 |  0x4b5fc |           0xeb5db |
|    41 |  0xc0784 | 0x31a219 |           0xe3b9a |
|    42 |  0xc07c3 |  0x4b5fc |           0x80030 |
|    43 |  0xc0808 | 0x31a219 |           0x800c8 |
|    44 |  0xc0916 | 0x5d6438 |           0xe6e7e |
|    45 |  0xd0f7e |  0x5a086 |           0xeac65 |
|    46 |  0xd1162 |  0xa3f40 |           0xd111e |
|    47 |  0xda823 | 0x25b862 |           0x803b8 |
|    48 |  0xdaa44 | 0x25a034 |           0xe21da |
|    49 |  0xdaab5 | 0x25a034 |           0xeaa39 |
|    50 |  0xdab25 | 0x25a034 |           0xda0ff |
|    51 |  0xdab95 | 0x25a034 |           0xd5eea |
|    52 |  0xdac07 | 0x25a034 |           0xe46f7 |
|    53 |  0xdac78 | 0x25a034 |           0x800b8 |
|    54 |  0xdad85 | 0x25a034 |           0x80370 |
|    55 |  0xdae19 | 0x25a034 |           0xea541 |
|    56 |  0xdaec9 | 0x25a034 |           0xcfe68 |
|    57 |  0xdafbf | 0x25a034 |           0x80440 |
|    58 |  0xdb02c | 0x25b862 |           0xd5a49 |
|    59 |  0xdb104 | 0x25a034 |           0xe6d17 |
|    60 |  0xdb20b | 0x25a034 |           0xd035b |
|    61 |  0xdb377 | 0x25a034 |           0xea989 |
|    62 |  0xdb423 | 0x25a034 |           0xe6ea3 |
|    63 |  0xdb4fd | 0x25a034 |           0xeabd7 |
|    64 |  0xdb570 | 0x25a034 |           0xec05d |
|    65 |  0xdb5e0 | 0x25a034 |           0xd6019 |
|    66 |  0xdb6c2 | 0x25a034 |           0xe156f |
|    67 |  0xdb736 | 0x25a034 |           0x80180 |
|    68 |  0xdb838 | 0x25a034 |           0xd3174 |
|    69 |  0xdb8ca | 0x25a034 |           0xdc724 |
|    70 |  0xdb965 | 0x25a034 |           0xd9424 |
|    71 |  0xdb9d3 | 0x25a034 |           0xe5395 |
|    72 |  0xdba43 | 0x25a034 |           0xeb231 |
|    73 |  0xdbab5 | 0x25a034 |           0xda65d |
|    74 |  0xdbb23 | 0x25a034 |           0xd4503 |
|    75 |  0xdbc3d | 0x25a034 |           0x80160 |
|    76 |  0xdbdde | 0x25a034 |           0xd6e8b |
|    77 |  0xe9997 | 0x5ef8a2 |           0x80418 |
|    78 |  0xff747 | 0x5d5d22 |           0x80348 |
|    79 | 0x11c3f8 | 0x19f5ed |           0xd555d |
|    80 | 0x13ac36 | 0x4bb20d |           0x80280 |
|    81 | 0x13ac78 | 0x4bbe9d |           0xd671c |
|    82 | 0x13fb9f | 0x61d911 |           0xe0310 |
|    83 | 0x160f7d |  0xae209 |           0xd55ef |
|    84 | 0x160fc8 |  0xae209 |           0x80448 |
|    85 | 0x160fca |  0xae209 |           0xd7871 |
|    86 | 0x161038 |  0xae209 |           0xda1eb |
|    87 | 0x1610ea | 0x5ff4f4 |           0x80360 |
|    88 | 0x177bbe | 0x2245b7 |           0xd98d3 |
|    89 | 0x1849d0 | 0x34a74c |           0xea81d |
|    90 | 0x1856f7 | 0x174fe3 |           0xda035 |
+-------+----------+----------+-------------------+
Current path: (1, _, _)
Choose an entry [0-90]: 40

Child node is block 0xe6ea3.
Reading block 0xe6ea3 ... validating ... OK.
Node has 88 entries, as follows:
+-------+---------+----------+--------------+---------------------+---------------------+
| Index |     OID |      XID | Target block | Target's actual OID | Target's actual XID |
+-------+---------+----------+--------------+---------------------+---------------------+
|     0 | 0xc0726 |  0x4b5fc |      0xd73e8 |         0xc0726 ( ) |         0x4b5fc ( ) |
|     1 | 0xc0727 | 0x3185ab |      0xde42e |         0xc0727 ( ) |        0x3185ab ( ) |
|     2 | 0xc0728 |  0x4b5fc |      0xd73d7 |         0xc0728 ( ) |         0x4b5fc ( ) |
|     3 | 0xc0729 | 0x31a219 |      0xec455 |         0xc0729 ( ) |        0x31a219 ( ) |
|     4 | 0xc072a |  0x4b5fc |      0xd73d9 |         0xc072a ( ) |         0x4b5fc ( ) |
|     5 | 0xc072b |  0x4b5fc |      0xd73d3 |         0xc072b ( ) |         0x4b5fc ( ) |
|     6 | 0xc072c |  0x4b5fc |      0xd73d2 |         0xc072c ( ) |         0x4b5fc ( ) |
|     7 | 0xc072d |  0x4b5fc |      0xd73d4 |         0xc072d ( ) |         0x4b5fc ( ) |
|     8 | 0xc072e | 0x31a219 |      0xe6f1a |         0xc072e ( ) |        0x31a219 ( ) |
|     9 | 0xc072f |  0x4942b |      0xd41eb |         0xc072f ( ) |         0x4942b ( ) |
|    10 | 0xc0730 |  0x4b5fc |      0xd73ce |         0xc0730 ( ) |         0x4b5fc ( ) |
|    11 | 0xc0731 |  0x4b5fc |      0xd73cd |         0xc0731 ( ) |         0x4b5fc ( ) |
|    12 | 0xc0732 |  0x4942b |      0xd41fe |         0xc0732 ( ) |         0x4942b ( ) |
|    13 | 0xc0733 | 0x31a219 |      0xe6f13 |         0xc0733 ( ) |        0x31a219 ( ) |
|    14 | 0xc0734 |  0x4b5fc |      0xd73ca |         0xc0734 ( ) |         0x4b5fc ( ) |
|    15 | 0xc0735 |  0x4b5fc |      0xd73cb |         0xc0735 ( ) |         0x4b5fc ( ) |
|    16 | 0xc0736 |  0x4b5fc |      0xd73c9 |         0xc0736 ( ) |         0x4b5fc ( ) |
|    17 | 0xc0737 | 0x31a219 |      0xe6f10 |         0xc0737 ( ) |        0x31a219 ( ) |
|    18 | 0xc0738 |  0x4b5fc |      0xd73c6 |         0xc0738 ( ) |         0x4b5fc ( ) |
|    19 | 0xc0739 |  0x4b5fc |      0xd73c5 |         0xc0739 ( ) |         0x4b5fc ( ) |
|    20 | 0xc073a |  0x4b1fd |      0xd4d0b |         0xc073a ( ) |         0x4b1fd ( ) |
|    21 | 0xc073b | 0x31a219 |      0xe6f04 |         0xc073b ( ) |        0x31a219 ( ) |
|    22 | 0xc073c |  0x4b5fc |      0xd73c1 |         0xc073c ( ) |         0x4b5fc ( ) |
|    23 | 0xc073d |  0x4b5fc |      0xd73c0 |         0xc073d ( ) |         0x4b5fc ( ) |
|    24 | 0xc073f | 0x31a219 |      0xe6f02 |         0xc073f ( ) |        0x31a219 ( ) |
|    25 | 0xc0741 |  0x4b5fc |      0xd73bd |         0xc0741 ( ) |         0x4b5fc ( ) |
|    26 | 0xc0742 |  0x4b5fc |      0xd73bc |         0xc0742 ( ) |         0x4b5fc ( ) |
|    27 | 0xc0743 |  0x4b5fc |      0xd73be |         0xc0743 ( ) |         0x4b5fc ( ) |
|    28 | 0xc0744 | 0x31a219 |      0xe6f00 |         0xc0744 ( ) |        0x31a219 ( ) |
|    29 | 0xc0745 |  0x4b5fc |      0xd73ba |         0xc0745 ( ) |         0x4b5fc ( ) |
|    30 | 0xc0746 |  0x4b5fc |      0xd73b9 |         0xc0746 ( ) |         0x4b5fc ( ) |
|    31 | 0xc0747 |  0x4942b |      0xd4296 |         0xc0747 ( ) |         0x4942b ( ) |
|    32 | 0xc0748 | 0x31a219 |      0xe6ef9 |         0xc0748 ( ) |        0x31a219 ( ) |
|    33 | 0xc0749 |  0x4b5fc |      0xd73b7 |         0xc0749 ( ) |         0x4b5fc ( ) |
|    34 | 0xc074a |  0xa4f9d |      0xdb89b |         0xc074a ( ) |         0xa4f9d ( ) |
|    35 | 0xc074b |  0x4942b |      0xd429c |         0xc074b ( ) |         0x4942b ( ) |
|    36 | 0xc074c | 0x31a219 |      0xec59c |         0xc074c ( ) |        0x31a219 ( ) |
|    37 | 0xc074d |  0x4b5fc |      0xd73b4 |         0xc074d ( ) |         0x4b5fc ( ) |
|    38 | 0xc074e |  0x4b5fc |      0xd73b3 |         0xc074e ( ) |         0x4b5fc ( ) |
|    39 | 0xc074f |  0x4942b |      0xd42b4 |         0xc074f ( ) |         0x4942b ( ) |
|    40 | 0xc0750 |  0x8f0aa |      0xd9c21 |         0xc0750 ( ) |         0x8f0aa ( ) |
|    41 | 0xc0751 | 0x31a219 |      0xe6ef3 |         0xc0751 ( ) |        0x31a219 ( ) |
|    42 | 0xc0752 |  0x4b5fc |      0xd73b1 |         0xc0752 ( ) |         0x4b5fc ( ) |
|    43 | 0xc0753 |  0x7f795 |      0xd8648 |         0xc0753 ( ) |         0x7f795 ( ) |
|    44 | 0xc0754 |  0x4942b |      0xd42bf |         0xc0754 ( ) |         0x4942b ( ) |
|    45 | 0xc0755 | 0x31a219 |      0xec598 |         0xc0755 ( ) |        0x31a219 ( ) |
|    46 | 0xc0758 |  0x4b5fc |      0xd73ad |         0xc0758 ( ) |         0x4b5fc ( ) |
|    47 | 0xc0759 |  0x4b5fc |      0xd73ac |         0xc0759 ( ) |         0x4b5fc ( ) |
|    48 | 0xc075a |  0x4942c |      0xd438a |         0xc075a ( ) |         0x4942c ( ) |
|    49 | 0xc075c | 0x31a219 |      0xe6eda |         0xc075c ( ) |        0x31a219 ( ) |
|    50 | 0xc075d | 0x178060 |      0xe6955 |         0xc075d ( ) |        0x178060 ( ) |
|    51 | 0xc075e |  0x7f795 |      0xd8699 |         0xc075e ( ) |         0x7f795 ( ) |
|    52 | 0xc075f |  0x4942d |      0xd4512 |         0xc075f ( ) |         0x4942d ( ) |
|    53 | 0xc0760 | 0x31a219 |      0xe6ed4 |         0xc0760 ( ) |        0x31a219 ( ) |
|    54 | 0xc0761 |  0x4b5fc |      0xd73a5 |         0xc0761 ( ) |         0x4b5fc ( ) |
|    55 | 0xc0762 |  0x7f795 |      0xd86d2 |         0xc0762 ( ) |         0x7f795 ( ) |
|    56 | 0xc0763 |  0x4942d |      0xd455c |         0xc0763 ( ) |         0x4942d ( ) |
|    57 | 0xc0764 | 0x31a219 |      0xe6ec0 |         0xc0764 ( ) |        0x31a219 ( ) |
|    58 | 0xc0765 |  0x4b5fc |      0xd7389 |         0xc0765 ( ) |         0x4b5fc ( ) |
|    59 | 0xc0766 |  0x84819 |      0xdbee0 |         0xc0766 ( ) |         0x84819 ( ) |
|    60 | 0xc0768 | 0x31a219 |      0xe6eb9 |         0xc0768 ( ) |        0x31a219 ( ) |
|    61 | 0xc0769 |  0x4b5fc |      0xd7385 |         0xc0769 ( ) |         0x4b5fc ( ) |
|    62 | 0xc076a |  0x4b5fc |      0xd7386 |         0xc076a ( ) |         0x4b5fc ( ) |
|    63 | 0xc076b |  0x4b5fc |      0xd7384 |         0xc076b ( ) |         0x4b5fc ( ) |
|    64 | 0xc076c | 0x31a219 |      0xe6eab |         0xc076c ( ) |        0x31a219 ( ) |
|    65 | 0xc076d |  0xf1ab8 |      0xd319b |         0xc076d ( ) |         0xf1ab8 ( ) |
|    66 | 0xc076e |  0x4b5fc |      0xd7378 |         0xc076e ( ) |         0x4b5fc ( ) |
|    67 | 0xc076f |  0x4b5fc |      0xd7370 |         0xc076f ( ) |         0x4b5fc ( ) |
|    68 | 0xc0770 |  0x4b5fc |      0xd7379 |         0xc0770 ( ) |         0x4b5fc ( ) |
|    69 | 0xc0771 | 0x31a219 |      0xe6ea9 |         0xc0771 ( ) |        0x31a219 ( ) |
|    70 | 0xc0772 |  0x4b5fc |      0xd736e |         0xc0772 ( ) |         0x4b5fc ( ) |
|    71 | 0xc0773 |  0x4b5fc |      0xd736d |         0xc0773 ( ) |         0x4b5fc ( ) |
|    72 | 0xc0774 |  0x4942e |      0xd4662 |         0xc0774 ( ) |         0x4942e ( ) |
|    73 | 0xc0775 | 0x2fb2c5 |      0xec704 |         0xc0775 ( ) |        0x2fb2c5 ( ) |
|    74 | 0xc0776 | 0x31a219 |      0xe6ea6 |         0xc0776 ( ) |        0x31a219 ( ) |
|    75 | 0xc0777 |  0x4b5fc |      0xd736b |         0xc0777 ( ) |         0x4b5fc ( ) |
|    76 | 0xc0778 |  0x7f795 |      0xd875f |         0xc0778 ( ) |         0x7f795 ( ) |
|    77 | 0xc0779 |  0x4942e |      0xd466e |         0xc0779 ( ) |         0x4942e ( ) |
|    78 | 0xc077a | 0x31a219 |      0xe6ea5 |         0xc077a ( ) |        0x31a219 ( ) |
|    79 | 0xc077b |  0x4b5fc |      0xd7367 |         0xc077b ( ) |         0x4b5fc ( ) |
|    80 | 0xc077c |  0x84819 |      0xdb73f |         0xc077c ( ) |         0x84819 ( ) |
|    81 | 0xc077d |  0x4942e |      0xd467a |         0xc077d ( ) |         0x4942e ( ) |
|    82 | 0xc077e | 0x31a219 |      0xe6ea4 |         0xc077e ( ) |        0x31a219 ( ) |
|    83 | 0xc077f |  0x4b5fc |      0xd735c |         0xc077f ( ) |         0x4b5fc ( ) |
|    84 | 0xc0780 | 0x31a219 |      0xe6e8e |         0xc0780 ( ) |        0x31a219 ( ) |
|    85 | 0xc0781 |  0x4942e |      0xd4681 |         0xc0781 ( ) |         0x4942e ( ) |
|    86 | 0xc0782 |  0x4b5fc |      0xd7356 |         0xc0782 ( ) |         0x4b5fc ( ) |
|    87 | 0xc0783 |  0x4b5fc |      0xd7352 |         0xc0783 ( ) |         0x4b5fc ( ) |
+-------+---------+----------+--------------+---------------------+---------------------+
Current path: (1, 40, _)
Choose an entry [0-87, ..]: 31

Record data for entry 31:
  KEY:
    - OID:              0xc0747
    - XID:              0x4942b
  VALUE:
    - Object size:      4096 bytes
    - Object address:   0xd4296
Current path: (1, 40, 31)
Choose an entry [ENTER, ..]: 
```
