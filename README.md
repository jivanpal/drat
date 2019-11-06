# APFS Tools

This is a set of open-source tools for analysing [APFS (Apple File System)](https://en.wikipedia.org/wiki/Apple_File_System)
partitions. Their creation was inspired by a [personal data loss incident](https://apple.stackexchange.com/questions/373718)
and [Jonathan Levin's](https://twitter.com/Morpheus______) `fsleuth` tool, which
he demonstrated in [this lecture](http://docs.macsysadmin.se/2018/video/Day4Session2.mp4).

This toolset is currently in development, and is being implemented with reference
to [Apple's official APFS specification (PDF)](https://developer.apple.com/support/downloads/Apple-File-System-Reference.pdf).
A copy of this spec as it appeared on 2019-10-31, namely the version published
on 2019-02-07, is included in this repository as `reference.pdf` for archival
purposes, particularly in case the online version of the document changes.

All of the tools currently included operate in a read-only fashion, as they are
intended to be used in situations involving data recovery or data forensics; but
this is subject to change.

## Build instructions

### Required software

- `gcc` — tested with GCC 9.2.0, installed via [Homebrew](https://brew.sh) (Homebrew GCC 9.2.0_1)
- `make` — tested with GNU Make 3.81, as included in Xcode Command Line Tools 11.0.0.0.1.1567737322 for macOS Catalina 10.15 (19A603)

Compilation and execution of this toolset has been tested on macOS Catalina 10.15 (19A603).

### Compilation

- Ensure that `gcc` is in your `$PATH`, then run `make` from the directory where
  the `Makefile` resides. A `bin` directory will be created, in which the
  compiled binaries will be stored.
- You can run `make <toolname>`, e.g. `make apfs-read`, to compile only that
  tool.
- Run `make clean` to remove the compiled binaries (`bin` directory) and object
  files (`obj` directory).

## Tool descriptions

### `apfs-read`

This tool prints out a nicely formatted, human-readable description of a given
block in a given APFS container. This tool assumes an APFS block size of 4096
bytes.

#### Usage

`apfs-read <container> <address>`
- `<container>` — The device file to read.
- `<address>` — The address of the block to read within `<container>`.
    This value can be specified as a hexadecimal value prefixed with `0x`,
    or as a decimal value.

#### Example usage

- `apfs-read /dev/disk0s2 0x0000000000003af2`
- `apfs-read /dev/disk1 15090`
- `apfs-read dump.bin 0x3af2`

#### Example output

```
$ sudo ./bin/apfs-read /dev/disk0s2 0x3af2

Opening file at `/dev/disk0s2` in read-only mode ... OK.

- Details of block 0x3af2:
Stored checksum:  0x8fb5c6176f907f7e
OID:              0x0000000000000403
XID:              0x000000000000bd46
Storage type:     Ephemeral
Type flags:       (none)
Type:             B-tree (root node)
Subtype:          Space manager free-space queue
```

### `apfs-inspect`

This tool inspect an APFS container by simulating the process of mounting it,
printing information on what it determines about the container during the
process.

#### Usage

`apfs-inspect <container>`
- `<container>` — The device file to inspect.

#### Example usage

- `apfs-inspect /dev/disk0s2`
- `apfs-inspect /dev/disk1`
- `apfs-inspect dump.bin`

#### Example output

<details>
<summary>Click to show/hide example output</summary>

```
$ sudo ./bin/apfs-inspect /dev/disk2s2

Opening file at `/dev/disk0s2` in read-only mode ... OK.
Simulating a mount of the APFS container.
Validating checksum of block 0x0 ... OK.

Details of block 0x0:
--------------------------------------------------------------------------------
Stored checksum:    0x6d3b3ae87c3b1736
OID:                0x1
XID:                0x137ba
Storage type:       Ephemeral
Type flags:         (none)
Type:               Container superblock
Subtype:            (none/invalid)
Magic string:       NXSB
Block size:         4096 bytes
Block count:        122045436
Supported features:
- The volumes in this container support defragmentation.
Supported read-only compatible features:
- No read-only compatible feature flags are set.
Backward-incompatible features:
- This container uses APFS version 1, as implemented in macOS 10.12.
UUID:       0xee88fe9d0853449d9743805152656b47
Next OID:                       0x1961d
Next XID:                       0x137bb
Space manager Ephemeral OID:    0x400
Object map Physical OID:        0x1a63be
Reaper Ephemeral OID:           0x401
Other flags:
- Reserved flag 1
--------------------------------------------------------------------------------

Locating the checkpoint descriptor area:
- Its length is 280 blocks.
- It is contiguous.
- The address of its first block is 0x1.
Loading the checkpoint descriptor area into memory ... OK.
Locating the most recent well-formed container superblock in the checkpoint descriptor area:
- It lies at index 203 within the checkpoint descriptor area.

Details of this container superblock:
--------------------------------------------------------------------------------
Stored checksum:    0x759fc6ac73d3d530
OID:                0x1
XID:                0x14a20
Storage type:       Ephemeral
Type flags:         (none)
Type:               Container superblock
Subtype:            (none/invalid)
Magic string:       NXSB
Block size:         4096 bytes
Block count:        122045436
Supported features:
- The volumes in this container support defragmentation.
Supported read-only compatible features:
- No read-only compatible feature flags are set.
Backward-incompatible features:
- This container uses APFS version 1, as implemented in macOS 10.12.
UUID:       0xee88fe9d0853449d9743805152656b47
Next OID:                       0x1992d
Next XID:                       0x14a21
Space manager Ephemeral OID:    0x400
Object map Physical OID:        0x1a6dc8
Reaper Ephemeral OID:           0x401
Other flags:
- Reserved flag 1
--------------------------------------------------------------------------------
- The corresponding checkpoint starts at index 202 within the checkpoint descriptor area, and spans 2 blocks.

Loading the corresponding checkpoint ... OK.

Details of each block in this checkpoint:
--------------------------------------------------------------------------------
Stored checksum:    0x8ddda7aeb21ec651
OID:                0xcb
XID:                0x14a20
Storage type:       Physical
Type flags:         (none)
Type:               Checkpoint map
Subtype:            (none/invalid)
Flags:
- Last checkpoint-mapping block in the correspondng checkpoint.
Number of mappings: 5
--------------------------------------------------------------------------------
Stored checksum:    0x759fc6ac73d3d530
OID:                0x1
XID:                0x14a20
Storage type:       Ephemeral
Type flags:         (none)
Type:               Container superblock
Subtype:            (none/invalid)
Magic string:       NXSB
Block size:         4096 bytes
Block count:        122045436
Supported features:
- The volumes in this container support defragmentation.
Supported read-only compatible features:
- No read-only compatible feature flags are set.
Backward-incompatible features:
- This container uses APFS version 1, as implemented in macOS 10.12.
UUID:       0xee88fe9d0853449d9743805152656b47
Next OID:                       0x1992d
Next XID:                       0x14a21
Space manager Ephemeral OID:    0x400
Object map Physical OID:        0x1a6dc8
Reaper Ephemeral OID:           0x401
Other flags:
- Reserved flag 1
--------------------------------------------------------------------------------

Details of each checkpoint-mapping in this checkpoint:
--------------------------------------------------------------------------------
Ephemeral OID:                      0x400
Logical block address on disk:      0x3120
Object type:                        Space manager
Object subtype:                     (none/invalid)
Object size:                        4096 bytes
Associated volume OID (virtual):    0x0
--------------------------------------------------------------------------------
Ephemeral OID:                      0x401
Logical block address on disk:      0x3121
Object type:                        Container reaper
Object subtype:                     (none/invalid)
Object size:                        4096 bytes
Associated volume OID (virtual):    0x0
--------------------------------------------------------------------------------
Ephemeral OID:                      0x403
Logical block address on disk:      0x3122
Object type:                        B-tree (root node)
Object subtype:                     Space manager free-space queue
Object size:                        4096 bytes
Associated volume OID (virtual):    0x0
--------------------------------------------------------------------------------
Ephemeral OID:                      0x405
Logical block address on disk:      0x3123
Object type:                        B-tree (root node)
Object subtype:                     Space manager free-space queue
Object size:                        4096 bytes
Associated volume OID (virtual):    0x0
--------------------------------------------------------------------------------
Ephemeral OID:                      0xf10f
Logical block address on disk:      0x3124
Object type:                        Container reaper list
Object subtype:                     (none/invalid)
Object size:                        4096 bytes
Associated volume OID (virtual):    0x0
--------------------------------------------------------------------------------
- There are 5 checkpoint-mappings in this checkpoint.

Reading the Ephemeral objects used by this checkpoint ... OK.
Validating the Ephemeral objects ... OK.

Details of the Ephemeral objects:
--------------------------------------------------------------------------------
Stored checksum:    0xfee4a5aa72b9b458
OID:                0x400
XID:                0x14a20
Storage type:       Ephemeral
Type flags:         (none)
Type:               Space manager
Subtype:            (none/invalid)
--------------------------------------------------------------------------------
Stored checksum:    0x8d1fafc7b2dd002b
OID:                0x401
XID:                0x14a20
Storage type:       Ephemeral
Type flags:         (none)
Type:               Container reaper
Subtype:            (none/invalid)
--------------------------------------------------------------------------------
Stored checksum:    0x02364e0be3653baf
OID:                0x403
XID:                0x14a20
Storage type:       Ephemeral
Type flags:         (none)
Type:               B-tree (root node)
Subtype:            Space manager free-space queue
--------------------------------------------------------------------------------
Stored checksum:    0x8772b0eb8cd606bc
OID:                0x405
XID:                0x14a20
Storage type:       Ephemeral
Type flags:         (none)
Type:               B-tree (root node)
Subtype:            Space manager free-space queue
--------------------------------------------------------------------------------
Stored checksum:    0xf7914643085005a5
OID:                0xf10f
XID:                0x14a20
Storage type:       Ephemeral
Type flags:         (none)
Type:               Container reaper list
Subtype:            (none/invalid)
--------------------------------------------------------------------------------

The container superblock states that the container object map has Physical OID 0x1a6dc8.
Loading the container object map ... OK.
Validating the container object map ... OK.

Details of the container object map:
--------------------------------------------------------------------------------
Stored checksum:    0x171fdd0a28a9fd34
OID:                0x1a6dc8
XID:                0x14a20
Storage type:       Physical
Type flags:         (none)
Type:               Object map
Subtype:            (none/invalid)
Flags:
- No snapshot support
Object mappings tree:
- Storage type:         Physical
- Type flags:           (none)
- Type:                 B-tree (root node)
- Object ID:            0x1a6dc9
Snapshots tree:
- Storage type:         Physical
- Type flags:           (none)
- Type:                 B-tree (root node)
- Object ID:            0x0
- Number of snapshots:  0 snapshots
- Latest snapshot XID:  0x0
In-progress revert:
- Minimum XID:          0x0
- Maximum XID:          0x0
--------------------------------------------------------------------------------

Reading the root node of the container object map B-tree ... OK.
Validating the root node of the container object map B-tree ... OK.

Details of the container object map B-tree:
--------------------------------------------------------------------------------
Stored checksum:    0x501de44e5ed6f257
OID:                0x1a6dc9
XID:                0x14a20
Storage type:       Physical
Type flags:         (none)
Type:               B-tree (root node)
Subtype:            Object map
Flags:                          Root node, Leaf node, Fixed size for keys and values
Number of child levels:         0
Number of keys in this node:    5
Location of table of contents:
- Offset from start of node data area:  0x0 = 0
- Length (bytes):                       0x1c0 = 448
Location of key–value shared free space:
- Offset from start of keys area:       0x60 = 96
- Length (bytes):                       0xd20 = 3360

Info relating to the entire B-tree:
- Flags:
  - This B-tree is currently undergoing a series of sequential inserts --- optimise operations if possible
  - Child nodes are referred to using Physical OIDs
- Node size:                4096 bytes
- Key size:                 16 bytes
- Value size:               16 bytes

- Length of longest key:    16 bytes
- Length of longest value:  16 bytes
- Number of keys:           5
- Number of nodes:          1
--------------------------------------------------------------------------------

The container superblock lists 5 APFS volumes, whose superblocks have the following Virtual OIDs:
- 0x402
- 0x408
- 0x40a
- 0x40c
- 0x480

Reading the APFS volume superblocks ... OK.
Validating the APFS volume superblocks ... OK.

Details of these volume superblocks:
--------------------------------------------------------------------------------
Stored checksum:    0x37c61805a0c1ffb8
OID:                0x402
XID:                0x14a20
Storage type:       Virtual
Type flags:         (none)
Type:               APFS volume
Subtype:            (none/invalid)
--------------------------------------------------------------------------------
Stored checksum:    0x6524cf9633537bcd
OID:                0x408
XID:                0x10798
Storage type:       Virtual
Type flags:         (none)
Type:               APFS volume
Subtype:            (none/invalid)
--------------------------------------------------------------------------------
Stored checksum:    0x8078e4e99f134fce
OID:                0x40a
XID:                0x142b4
Storage type:       Virtual
Type flags:         (none)
Type:               APFS volume
Subtype:            (none/invalid)
--------------------------------------------------------------------------------
Stored checksum:    0xa10536a53af6a40d
OID:                0x40c
XID:                0x13fe2
Storage type:       Virtual
Type flags:         (none)
Type:               APFS volume
Subtype:            (none/invalid)
--------------------------------------------------------------------------------
Stored checksum:    0xc6db0e59b5943640
OID:                0x480
XID:                0x1079b
Storage type:       Virtual
Type flags:         (none)
Type:               APFS volume
Subtype:            (none/invalid)
--------------------------------------------------------------------------------
END: All done.
```
</details>
