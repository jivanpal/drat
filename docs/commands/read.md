(command_read)=

# {command}`read`

## Description

The {command}`read` command prints a nicely formatted, human-readable
description of a given block in an APFS container.

## Example usage and output

```
$ drat --container /dev/disk2s2 read --block 0x3af2

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
