(argument_max-xid)=

# {argument}`max-xid`

## Description

The {argument}`max-xid` argument allows you to specify the maximum transaction
ID that should be considered when working with the APFS container. That is, any
objects with a transaction ID that exceeds the one specified will be ignored,
as if you are at the point in the time when the transaction with the specified
ID occurred, and thus have no knowledge of any future transactions. A value of
`-1` means "no maximum transaction ID", and is functionally equivalent to
a value of `0xffffffffffffffff`, which is the highest possible transaction ID,
since transaction IDs are 64-bit numbers.
