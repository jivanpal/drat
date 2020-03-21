#!/bin/bash

IFS=$'\n'

if [ $# -ne 1 ] ; then
  2>&1 echo "Usage: $0 <target directory>"
  exit 1
fi

for f in $(find "$1" -type f); do
  echo -n "- \`$f\` -- "
  ./rtn "$f"
done
