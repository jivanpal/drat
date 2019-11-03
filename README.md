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

Opening file at `/dev/disk0s2`.
Opened file successfully.

- Details for block 0x0000000000003af2
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
$ sudo ./bin/apfs-inspect /dev/disk0s2

Opening file at `/dev/disk0s2`.
Opened file successfully.

Simulating a mount of the APFS container.

Reading block 0x0 into buffer ... OK.
Validating block checksum ... OK.

- Details of block 0x0:
Stored checksum:  0x8e71ca425670c5f1
OID:              0x0000000000000001
XID:              0x000000000000b79b
Storage type:     Ephemeral
Type flags:       (none)
Type:             Container superblock
Subtype:          Invalid type

Preparing to read container superblocks from the checkpoint descriptor area:
- Allocating memory for array of container superblocks ... OK.
Locating the checkpoint descriptor area:
- The area is contiguous.
- The address of its first block is 0x0000000000000001.
Reading blocks from the checkpoint descriptor area into memory:
- Read 64 container superblocks into memory so far. Allocating more memory for more container superblocks ... OK.
- Read 128 container superblocks into memory so far. Allocating more memory for more container superblocks ... OK.
- Reached the end of the checkpoint descriptor area. Found 140 container superblocks and successfully read them into memory.
Searching array for the container superblock with the highest XID:
- Block 0xf6 has the highest XID.

- Details of block 0xf6:
Stored checksum:  0xed0e18abf7b1cee1
OID:              0x0000000000000001
XID:              0x000000000000c3b7
Storage type:     Ephemeral
Type flags:       (none)
Type:             Container superblock
Subtype:          Invalid type
----
Number of blocks in corresponding checkpoint:   2
desc_index:     0xf4
```
