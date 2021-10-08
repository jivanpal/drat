(argument_container)=

# {argument}`container`

## Description

The {argument}`container` argument is used to specify the file (whether a
regular file or not) that represents the APFS partition/container to work on.
For example, this can be:

- a device special file that represents:

  - an APFS partition (e.g. `/dev/disk2s3`, `/dev/sdb3`); or
  
  - a synthesized disk that represents a mounted APFS container
    (e.g. `/dev/disk3`, `/dev/sdc`); or

- a disk image, such as:

  - a copy of an APFS partition made with the likes of {command}`dd`;

  - an APFS filesystem image created with the likes of {command}`newfs_apfs`,
    {command}`hdiutil` or via {app}`Disk Utility`.

This argument is mandatory for all commands, else Drat will not know what APFS
container you want it to work on.
