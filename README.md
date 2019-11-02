# APFS Dump

This is an open-source tool (to the extent permissible by law) for reading
[APFS (Apple File System)](https://en.wikipedia.org/wiki/Apple_File_System)
partitions, inspired by a [personal data loss incident](https://apple.stackexchange.com/questions/373718)
and [Jonathan Levin's](https://twitter.com/Morpheus______) `fsleuth` tool, which
he demonstrated in [this lecture](http://docs.macsysadmin.se/2018/video/Day4Session2.mp4).

This tool is currently in development, and is being implemented with reference
to [Apple's official APFS specification (PDF)](https://developer.apple.com/support/downloads/Apple-File-System-Reference.pdf).
A copy of this spec as it appeared on 2019-10-31, namely the version published
on 2019-02-07, is included in this repository in case the online version of the
document changes.

## Build instructions

Ensure that `gcc` is in your `$PATH`, then run `make` from the directory where
the `Makefile` resides. Run `make clean` to remove the compiled binaries and
object files.
