#!/bin/bash

if [ $# -ne 1 ] ; then
  echo "Incorrect arguments; usage: $0 <filelist>"
  exit 1
fi

while IFS= read -r filepath; do
  rm -fv "/Users/jivan/Desktop/RECOVERY/$filepath"
done < <(
    # Remove comments (`#` and subsequent characters on each line),
    # then remove leading whitespace,
    # then remove trailing whitespace,
    # then remove empty lines
    cat "$1" \
    | sed -E '
        s/#.*$//
        s/^ +//
        s/ +$//
        s/^ *$//' \
    | grep -v '^ *$'
)

echo -e "\nALL DONE!\n"
