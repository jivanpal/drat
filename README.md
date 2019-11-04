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

```
$ sudo ./bin/apfs-inspect /dev/disk2s2

Opening file at `/dev/disk0s2` in read-only mode ... OK.
Simulating a mount of the APFS container.
Validating checksum of block 0x0 ... OK.

- Details of block 0x0:
Stored checksum:    0x8a15fcbe5b2c44c2
OID:                0x1
XID:                0xd90b
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
UUID:               0xee88fe9d0853449d9743805152656b47
Next OID:           0x13b10
Next XID:           0xd90c
Space manager OID:  0x400
Object map OID:     0x1a2dbd

Locating the checkpoint descriptor area:
- Its length is 280 blocks.
- It is contiguous.
- The address of its first block is 0x1.
Loading the checkpoint descriptor area into memory ... OK.
Locating the most recent well-formed container superblock in the checkpoint descriptor area:
- It lies at index 91 within the checkpoint descriptor area.
- The corresponding checkpoint starts at index 90 within the checkpoint descriptor area, and spans 2 blocks.

- Details of this container superblock:

Stored checksum:    0x3bf498f4a969bf35
OID:                0x1
XID:                0xef2a
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
UUID:               0xee88fe9d0853449d9743805152656b47
Next OID:           0x13e08
Next XID:           0xef2b
Space manager OID:  0x400
Object map OID:     0x1a3425

Loading the corresponding checkpoint:

- Details of each block in this checkpoint:

Stored checksum:    0x89c104e6b63c773f
OID:                0x5b
XID:                0xef2a
Storage type:       Physical
Type flags:         (none)
Type:               Checkpoint map
Subtype:            (none/invalid)
Flags:
- Last checkpoint-mapping block in the correspondng checkpoint.
Number of mappings: 5

Stored checksum:    0x3bf498f4a969bf35
OID:                0x1
XID:                0xef2a
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
UUID:               0xee88fe9d0853449d9743805152656b47
Next OID:           0x13e08
Next XID:           0xef2b
Space manager OID:  0x400
Object map OID:     0x1a3425

- Details of each checkpoint-mapping in this checkpoint:

Ephemeral object ID:    0x400
- Address within checkpoint data area:  0xd60
- Object type:                          Space manager
- Object subtype:                       (none/invalid)
- Object size:                          4096 bytes
- Virtual OID of associated volume:     0x0
Ephemeral object ID:    0x401
- Address within checkpoint data area:  0xd61
- Object type:                          Container reaper
- Object subtype:                       (none/invalid)
- Object size:                          4096 bytes
- Virtual OID of associated volume:     0x0
Ephemeral object ID:    0x403
- Address within checkpoint data area:  0xd62
- Object type:                          B-tree (root node)
- Object subtype:                       Space manager free-space queue
- Object size:                          4096 bytes
- Virtual OID of associated volume:     0x0
Ephemeral object ID:    0x405
- Address within checkpoint data area:  0xd63
- Object type:                          B-tree (root node)
- Object subtype:                       Space manager free-space queue
- Object size:                          4096 bytes
- Virtual OID of associated volume:     0x0
Ephemeral object ID:    0xf10f
- Address within checkpoint data area:  0xd64
- Object type:                          Container reaper list
- Object subtype:                       (none/invalid)
- Object size:                          4096 bytes
- Virtual OID of associated volume:     0x0
END: All done.
```
