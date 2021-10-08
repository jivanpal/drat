(argument_volume)=

# {argument}`volume` / {argument}`volume-name`

## Description

The {argument}`volume` argument allows you to specify the volume you want to
work with by its index within the container specified by {argument}`container`.
Volume indexing starts at 1, so `1` is the first volume, `2` is the second
volume, etc., and an index of `0` refers to the container itself, as opposed to
an actual volume.

Alternatively, you can specify a volume name via the {argument}`volume-name`
argument. The specified volume name must match the name of precisely one of the
volumes within the container, else the name cannot be resolved to a specific
volume and an error will be reported, along with a list of the volumes and
their indexes, if possible. [Unicode canonical equivalence](https://unicode.org/reports/tr15/#Canon_Compat_Equivalence)
is used to compare strings for equality.

## Example usage

- `--volume 0`
- `--volume 1`
- `--volume 5`
- `--volume-name Preboot`
- `--volume-name 'macOS - Data'`
