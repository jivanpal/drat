(argument_volume)=

# {argument}`volume` / {argument}`volume-name`

## Description

The {argument}`volume` argument allows you to specify the volume you want to
work with by its index within the container specified by {argument}`container`.
Indexes are zero-based, so `0` is the first volume, `1` is the second volume,
etc.. An index of `-1` refers to the container itself, as opposed to any actual
volume.

Alternatively, you can specify a volume name via the {argument}`volume-name`
argument. The specified volume name must match the name of precisely one of the
volumes within the container, else the name cannot be resolved to a specific
volume and an error will be reported, along with a list of the volumes and
their indexes, if possible. [Unicode canonical equivalence](https://unicode.org/reports/tr15/#Canon_Compat_Equivalence)
is used to compare strings for equality.

## Example usage

- `--volume -1`
- `--volume 0`
- `--volume-name Preboot`
- `--volume-name 'macOS - Data'`
