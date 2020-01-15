#!/bin/bash

#set up enviormental variables
. ./config.sh

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

export SCRIPT_ARGS="$1"
export KERNEL_BUILD_ARGS="$2"
export LIBC_BUILD_ARGS="$3"

echo $KERNEL_BUILD_ARGS


mkdir -p bin/kernel bin/libc bin/arch bin/drivers

for DIR in $PROJECTS; do
    cd $DIR && make build && cd ..
done;

#check if build is good
./scripts/is_multiboot.sh $PROJECT_NAME.kernel

#create symbols
objcopy --only-keep-debug juos.kernel juos.sym

#create storage
if list_include_item $SCRIPT_ARGS "NEW_STORAGE"; then
    echo "CREATING NEW QEMU IMG FOR STORAGE"
    qemu-img create -f raw $PWD/$PROJECT_NAME.storage 1M
fi
