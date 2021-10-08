(command_resolve-virtual-oids)=

# {drat-command}`resolve-virtual-oids`

## Description

The {drat-command}`resolve-virtual-oids` command takes one or more Virtual OIDs and
uses one or more object maps to resolve each Virtual OID to one or more block
addresses. It can also be used to programatically check whether a given Virtual
OID maps to a given block address, or do such checks in a batch — that is,
check whether a specific set of Virtual OIDs maps directly to a specific
set of block addresses.

## Example usage and output

```
$ drat --container /dev/disk2s2 resolve-virtual-oids --volume 1 --oid 0x247
0x247 -> 0x1167a

$ drat --container /dev/disk2s2 resolve-virtual-oids --block 0x1d623e --oid 0x247
0x247 -> 0x1167a

$ drat --container /dev/disk2s2 resolve-virtual-oids --volume 1 --oids 0x247,0x11f3,0x10a
0x247  -> 0x1167a
0x11f3 -> 0x20ad6
0x10a  -> 0x17d59
```

```
$ drat --container /dev/disk2s2 resolve-virtual-oids --volume 1 --oids 0x247,0x11f3 --expected-addresses 0x1167a,0x20ad6,0x17d59
0x247  -> 0x1167a ? OK
0x11f3 -> 0x20ad6 ? OK
0x10a  -> 0x17d59 ? OK

$ echo $?
0
```

```
$ drat --container /dev/disk2s2 resolve-virtual-oids --volume 1 --oids 0x247,0x11f3 --expected-addresses 0x1167a,0x123,0x17d59
0x247  -> 0x1167a ? OK
0x11f3 -> 0x123   ? FAILED ; 0x11f3 -> 0x20ad6
0x10a  -> 0x17d59 ? OK

$ echo $?
1
```