#!/bin/bash

if [ $# -ne 2 ] ; then
    echo "Incorrect number of arguments"
    echo "Usage: $0 <path to filepath list> <path to recovery directory>"
    exit 1
fi

echo "Started recovery on $(date "+%Y-%m-%d") at $(date "+%H:%M:%S")"

pathlist="$1"
rec_dir="$2"

mkdir -p "$rec_dir"

while IFS= read -r filepath; do
    # Replace consecutive slashes (e.g. `///`) with single slash `/`
    backup_path=$(echo "${rec_dir}/${filepath}" | sed -E 's:/+:/:g')
    
    mkdir -p "$(dirname "$backup_path")"

    # Don't overwrite existing files
    if [ -f "$backup_path" ] ; then
        echo "ALREADY EXISTS: $filepath"
        continue
    fi
    
    echo -n "- $filepath ... "
    ./bin/apfs-recover /dev/disk2s2 0 "$filepath" > "$backup_path" 2> /dev/null
    if [ $? -eq 0 ] ; then
        echo "OK."
    else
        echo "FAILED."
    fi
done < <(
    # Remove comments (`#` and subsequent characters on each line),
    # then remove leading whitespace,
    # then remove trailing whitespace,
    # then remove empty lines
    cat "$pathlist" \
    | sed -E '
        s/#.*$//
        s/^ +//
        s/ +$//
        s/^ *$//' \
    | grep -v '^ *$'
)

echo "Finished recovery on $(date "+%Y-%m-%d") at $(date "+%H:%M:%S")"

echo -e "\nALL DONE!\n"
