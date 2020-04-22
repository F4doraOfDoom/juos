#!/bin/bash
. ./config.sh

CLEAN_ARGS="$1"

function list_include_item {
  local list="$1"
  local item="$2"
  if [[ $list =~ (^|[[:space:]])"$item"($|[[:space:]]) ]] ; then
    # yes, list include item
    result=0
  else
    result=1
  fi
  return $result
}


rm $PROJECT_NAME.kernel
rm $PROJECT_NAME.iso
rm $PROJECT_NAME.sym
rm -rf isodir/
rm -rf bin/

if list_include_item $CLEAN_ARGS "CLEAN_STORAGE"; then
    echo "CLEANING STORAGE"
    rm $PROJECT_NAME.storage
fi