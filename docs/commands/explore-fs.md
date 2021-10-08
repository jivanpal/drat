(command_explore-fs)=

# {drat-command}`explore-fs`

## Description

The {drat-command}`explore-fs` command provides an interface via which you can
explore a volume's filesystem. That is, you can interactively navigate through
the filesystem, viewing the filesystem records for any filesystem object. When
you run {drat-command}`explore-fs`, you must specify:

- a filesystem tree in the same manner as {drat-command}`explore-fs-tree`; and

- an object map in the same manner as {drat-command}`explore-fs-tree`; and

- an entry point, which is either:

  - an absolute filesystem path via {argument}`path`, like `/`,
    `/Users/john`, `/Users/john/Documents/my document.txt`, or `/..`; or

  - a filesystem object ID via {argument}`fsoid`, like `0x7563`.

If no filesystem tree is specified, the default is `--volume 1`.
If no entrypoint is specified, the default is `--path /`.

When the command is run, Drat will display the filesystem records of the entry
point in the order they appear in the filesystem B-tree, which should be by
FSRT, then by name for xattrs and dentries. In order to navigate the filesystem,
you will be prompted to enter:

- a record index;
- `/` followed by a dentry name;
- `@` followed by an xattr name; or
- `..`, which denotes the parent filesystem object.

At any time, you can enter nothing to exit Drat.

## Example usage and output

```
$ drat explore-fs --container /dev/disk0s2 --volume 1 --path /Users/john

Opening file at `/dev/disk0s2` in read-only mode ... OK.

Determining block size ... 4096 bytes.

Finding volume 0:
- Finding most recent container superblock:
  - Reading block 0x0 ... validating ... OK.
  - Loading checkpoint descriptor area ... OK.
  - Searching checkpoint descriptor area ... found most recent container superblock at index 2, its XID is 0x629c38.
- Finding container's object mappings tree:
  - Container's object map has Physical OID 0xd25cd.
  - Reading block 0xd25cd ... validating ... OK.
  - Container's object mappings tree has Physical OID 0xdb5ce.
  - Reading block 0xdb5ce ... validating ... OK.
- Finding volume 0's superblock:
  - Volume 0 has Virtual OID 0x402, maps to block 0xd8e59.
  - Reading block 0xd8e59 ... validating ... OK.
  - Volume name: macOS - Data

Finding volume's object mappings tree:
- Volume's object map has Physical OID 0xd66c6.
- Reading block 0xd66c6 ... validating ... OK.
- Volume's object mappings tree has Physical OID 0xd196a.
- Reading block 0xd196a ... validating ... OK.

Finding volume's filesystem tree:
- Filesystem tree has Virtual OID 0x404, maps to block 0xcfcf3.
- Reading block 0xcfcf3 ... validating ... OK.

Navigating to path `/Users/john` ... its FSOID is 0x7563.
Finding records for FSOID 0x7563 ... OK.
Filesystem object has 25 records, as follows:
+-------+-------------+
| Index | FSRT        |
+-------+-------------+
|     0 | 0x3, inode  | Parent FSOID = 0x558b
|     1 | 0x4, xattr  | Name = com.apple.system.Security
|       |             +-------------+-------------------+---------------------+
|       |             | Inode FSOID | Inode type        | Inode name          |
|       |             +-------------+-------------------+---------------------+
|     2 | 0x9, dentry |    0x1476c2 | 0x4, directory    | .config             |
|     3 | 0x9, dentry |      0x7564 | 0x4, directory    | Music               |
|     4 | 0x9, dentry |    0x5a288d | 0x8, regular file | .DS_Store           |
|     5 | 0x9, dentry |      0x759a | 0x8, regular file | .CFUserTextEncoding |
|     6 | 0x9, dentry |     0x37239 | 0x4, directory    | bin                 |
|     7 | 0x9, dentry |     0x3723d | 0x8, regular file | .zshrc              |
|     8 | 0x9, dentry |     0x9cf93 | 0x4, directory    | .local              |
|     9 | 0x9, dentry |      0x7566 | 0x4, directory    | Pictures            |
|    10 | 0x9, dentry |   0x16f5f01 | 0x8, regular file | .zsh_history        |
|    11 | 0x9, dentry |      0x7568 | 0x4, directory    | Desktop             |
|    12 | 0x9, dentry |      0x756a | 0x4, directory    | Library             |
|    13 | 0x9, dentry |    0x182e7e | 0x4, directory    | .cups               |
|    14 | 0x9, dentry |     0x36fc6 | 0x4, directory    | .ssh                |
|    15 | 0x9, dentry |      0x7592 | 0x4, directory    | Movies              |
|    16 | 0x9, dentry |     0x3297d | 0x4, directory    | Applications        |
|    17 | 0x9, dentry |   0x16f2b3e | 0x4, directory    | .Trash              |
|    18 | 0x9, dentry |   0x153fffe | 0x8, regular file | .zcompdump          |
|    19 | 0x9, dentry |      0x7594 | 0x4, directory    | Documents           |
|    20 | 0x9, dentry |    0xaf5a6c | 0x4, directory    | .zcompcache         |
|    21 | 0x9, dentry |     0xc2297 | 0x8, regular file | .Xauthority         |
|    22 | 0x9, dentry |      0x7596 | 0x4, directory    | Downloads           |
|    23 | 0x9, dentry |     0xc12c9 | 0x4, directory    | .cache              |
|    24 | 0x9, dentry |    0x144d51 | 0x8, regular file | .aliases            |
+-------+-------------+-------------+-------------------+---------------------+
Current path: /Users/john
Choose a record [.., 0-24, /<inode name>, @<xattr name>]: 4

Finding records for FSOID 0x5a288d ... OK.
Filesystem object has 5 records, as follows:
+-------+------------------+
| Index | FSRT             |
+-------+------------------+
|     0 | 0x3, inode       | Parent FSOID = 0x7563
|     1 | 0x4, xattr       | Name = com.apple.FinderInfo
|     2 | 0x6, dstream     | Reference count = 1
|       |                  +-----------------+----------------+---------------+
|       |                  | Logical address | Length (bytes) | Block address |
|       |                  +-----------------+----------------+---------------+
|     3 | 0x8, file extent |               0 |           4096 |     0x359223c |
|     4 | 0x8, file extent |          0x1000 |           8192 |     0x2c69bcd |
+-------+------------------+-----------------+----------------+---------------+
Current path: /Users/john/.DS_Store
Choose a record [.., 0-4, @<xattr name>]: ..

Finding records for FSOID 0x7563 ... OK
Object has 25 records, as follows:
+-------+-------------+
| Index | FSRT        |
+-------+-------------+
|     0 | 0x3, inode  | Parent FSOID = 0x558b
|     1 | 0x4, xattr  | Name = com.apple.system.Security
|       |             +-------------+-------------------+---------------------+
|       |             | Inode FSOID | Inode type        | Inode name          |
|       |             +-------------+-------------------+---------------------+
|     2 | 0x9, dentry |    0x1476c2 | 0x4, directory    | .config             |
|     3 | 0x9, dentry |      0x7564 | 0x4, directory    | Music               |
|     4 | 0x9, dentry |    0x5a288d | 0x8, regular file | .DS_Store           |
|     5 | 0x9, dentry |      0x759a | 0x8, regular file | .CFUserTextEncoding |
|     6 | 0x9, dentry |     0x37239 | 0x4, directory    | bin                 |
|     7 | 0x9, dentry |     0x3723d | 0x8, regular file | .zshrc              |
|     8 | 0x9, dentry |     0x9cf93 | 0x4, directory    | .local              |
|     9 | 0x9, dentry |      0x7566 | 0x4, directory    | Pictures            |
|    10 | 0x9, dentry |   0x16f5f01 | 0x8, regular file | .zsh_history        |
|    11 | 0x9, dentry |      0x7568 | 0x4, directory    | Desktop             |
|    12 | 0x9, dentry |      0x756a | 0x4, directory    | Library             |
|    13 | 0x9, dentry |    0x182e7e | 0x4, directory    | .cups               |
|    14 | 0x9, dentry |     0x36fc6 | 0x4, directory    | .ssh                |
|    15 | 0x9, dentry |      0x7592 | 0x4, directory    | Movies              |
|    16 | 0x9, dentry |     0x3297d | 0x4, directory    | Applications        |
|    17 | 0x9, dentry |   0x16f2b3e | 0x4, directory    | .Trash              |
|    18 | 0x9, dentry |   0x153fffe | 0x8, regular file | .zcompdump          |
|    19 | 0x9, dentry |      0x7594 | 0x4, directory    | Documents           |
|    20 | 0x9, dentry |    0xaf5a6c | 0x4, directory    | .zcompcache         |
|    21 | 0x9, dentry |     0xc2297 | 0x8, regular file | .Xauthority         |
|    22 | 0x9, dentry |      0x7596 | 0x4, directory    | Downloads           |
|    23 | 0x9, dentry |     0xc12c9 | 0x4, directory    | .cache              |
|    24 | 0x9, dentry |    0x144d51 | 0x8, regular file | .aliases            |
+-------+-------------+-------------+-------------------+---------------------+
Current path: /Users/john
Choose a record [.., 0-24, /<inode name>, @<xattr name>]: /Library

Finding records for FSOID 0x756a ... OK.
Object has 70 records, as follows:
+-------+-------------+
| Index | FSRT        |
+-------+-------------+
|     0 | 0x3, inode  | Parent FSOID = 0x7563
|     1 | 0x4, xattr  | Name = com.apple.FinderInfo
|     2 | 0x4, xattr  | Name = com.apple.system.Security
|       |             +-------------+-------------------+-------------------------+
|       |             | Inode FSOID | Inode type        | Inode name              |
|       |             +-------------+-------------------+-------------------------+
|     3 | 0x9, dentry |      0x756b | 0x4, directory    | Application Support     |
|     4 | 0x9, dentry |      0x81a6 | 0x4, directory    | Maps                    |
|     5 | 0x9, dentry |      0x7980 | 0x4, directory    | Assistant               |
|     6 | 0x9, dentry |      0x756c | 0x4, directory    | Autosave Information    |
|     7 | 0x9, dentry |      0x8952 | 0x4, directory    | Saved Application State |
|     8 | 0x9, dentry |      0x8044 | 0x4, directory    | IdentityServices        |
|     9 | 0x9, dentry |      0x8b74 | 0x4, directory    | WebKit                  |
|    10 | 0x9, dentry |    0x14308d | 0x4, directory    | Developer               |
|    11 | 0x9, dentry |    0x5299e3 | 0x4, directory    | Mozilla                 |
|    12 | 0x9, dentry |      0x7964 | 0x4, directory    | Calendars               |
|    13 | 0x9, dentry |      0x756d | 0x4, directory    | Preferences             |
|    14 | 0x9, dentry |      0x8946 | 0x4, directory    | studentd                |
|    15 | 0x9, dentry |      0x7751 | 0x4, directory    | Messages                |
|    16 | 0x9, dentry |      0x8ff5 | 0x4, directory    | HomeKit                 |
|    17 | 0x9, dentry |    0x2f777a | 0x4, directory    | DES                     |
|    18 | 0x9, dentry |      0x75ef | 0x4, directory    | Keychains               |
|    19 | 0x9, dentry |      0x8093 | 0x4, directory    | Sharing                 |
|    20 | 0x9, dentry |      0x7571 | 0x4, directory    | ColorPickers            |
|    21 | 0x9, dentry |    0x1538ca | 0x4, directory    | ColorSync               |
|    22 | 0x9, dentry |      0x782e | 0x4, directory    | Application Scripts     |
|    23 | 0x9, dentry |      0x7572 | 0x4, directory    | Assistants              |
|    24 | 0x9, dentry |      0x759d | 0x8, regular file | .localized              |
|    25 | 0x9, dentry |      0x776f | 0x4, directory    | Mail                    |
|    26 | 0x9, dentry |     0x99335 | 0x4, directory    | UIKitSystem             |
|    27 | 0x9, dentry |      0x7573 | 0x4, directory    | Compositions            |
|    28 | 0x9, dentry |    0x846f5e | 0x4, directory    | GameKit                 |
|    29 | 0x9, dentry |     0x323ad | 0x4, directory    | Google                  |
|    30 | 0x9, dentry |      0x8bb2 | 0x4, directory    | LanguageModeling        |
|    31 | 0x9, dentry |      0x7575 | 0x4, directory    | Favorites               |
|    32 | 0x9, dentry |      0x8e95 | 0x4, directory    | Passes                  |
|    33 | 0x9, dentry |      0x759e | 0x4, directory    | FontCollections         |
|    34 | 0x9, dentry |      0x7577 | 0x4, directory    | Sounds                  |
|    35 | 0x9, dentry |      0x82f4 | 0x4, directory    | com.apple.internal.ck   |
|    36 | 0x9, dentry |      0x7578 | 0x4, directory    | Printers                |
|    37 | 0x9, dentry |      0x768a | 0x4, directory    | SyncedPreferences       |
|    38 | 0x9, dentry |      0x7579 | 0x4, directory    | Audio                   |
|    39 | 0x9, dentry |      0x7582 | 0x4, directory    | Keyboard Layouts        |
|    40 | 0x9, dentry |      0x7583 | 0x4, directory    | Logs                    |
|    41 | 0x9, dentry |      0x7584 | 0x4, directory    | Internet Plug-Ins       |
|    42 | 0x9, dentry |     0x96d34 | 0x4, directory    | FrontBoard              |
|    43 | 0x9, dentry |      0x9220 | 0x4, directory    | News                    |
|    44 | 0x9, dentry |      0x767d | 0x4, directory    | Accounts                |
|    45 | 0x9, dentry |      0x7783 | 0x4, directory    | Safari                  |
|    46 | 0x9, dentry |      0x7585 | 0x4, directory    | Colors                  |
|    47 | 0x9, dentry |      0x8cce | 0x4, directory    | MediaStream             |
|    48 | 0x9, dentry |      0x7586 | 0x4, directory    | PreferencePanes         |
|    49 | 0x9, dentry |      0x9bce | 0x4, directory    | Mobile Documents        |
|    50 | 0x9, dentry |      0x7666 | 0x4, directory    | Suggestions             |
|    51 | 0x9, dentry |     0x324a7 | 0x4, directory    | LaunchAgents            |
|    52 | 0x9, dentry |      0x7fc8 | 0x4, directory    | Group Containers        |
|    53 | 0x9, dentry |      0x8b86 | 0x4, directory    | Dictionaries            |
|    54 | 0x9, dentry |      0x76b8 | 0x4, directory    | Containers              |
|    55 | 0x9, dentry |      0x767b | 0x4, directory    | PersonalizationPortrait |
|    56 | 0x9, dentry |      0x7587 | 0x4, directory    | Fonts                   |
|    57 | 0x9, dentry |      0x8b7f | 0x4, directory    | KeyboardServices        |
|    58 | 0x9, dentry |      0x7628 | 0x4, directory    | Metadata                |
|    59 | 0x9, dentry |      0x7588 | 0x4, directory    | Screen Savers           |
|    60 | 0x9, dentry |      0x8078 | 0x4, directory    | CallServices            |
|    61 | 0x9, dentry |      0x7589 | 0x4, directory    | Spelling                |
|    62 | 0x9, dentry |      0x9cea | 0x4, directory    | SafariSafeBrowsing      |
|    63 | 0x9, dentry |      0x760f | 0x4, directory    | Reminders               |
|    64 | 0x9, dentry |      0x758a | 0x4, directory    | Input Methods           |
|    65 | 0x9, dentry |      0x9bea | 0x4, directory    | Cookies                 |
|    66 | 0x9, dentry |      0x758c | 0x4, directory    | Services                |
|    67 | 0x9, dentry |    0x294760 | 0x4, directory    | Keyboard                |
|    68 | 0x9, dentry |      0x881f | 0x4, directory    | CoreFollowUp            |
|    69 | 0x9, dentry |      0x758d | 0x4, directory    | Caches                  |
+-------+-------------+-------------+-------------------+-------------------------+
Current path: /Users/john/Library
Choose a record [.., 0-69, /<inode name>, @<xattr name>]: 
```
