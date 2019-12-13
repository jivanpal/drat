#!/bin/bash

IFS=$'\n'

for f in $(find ./RECOVERY -type f); do
  echo -n "- \`$f\` -- "
  ./rtn "$f"
done
