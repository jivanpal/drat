(command_create-index)=

# {drat-command}`create-index`

The {drat-command}`create-index` command will scan the APFS container and create
an index of the blocks within the container, such as the location of blocks of
certain types or blocks holding data on certain objects. This index takes the
form of a SQLite 3 database, and can be passed to {drat-command}`search` to make
search operations much quicker by greatly reducing the size of the search space.

## Example usage and output

```
$ drat --container /dev/disk0s2 create-index drat-index.db
```

## Schema

```{todo} Describe the database schema used by the index.
```
