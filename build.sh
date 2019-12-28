#!/bin/sh

#set up enviormental variables
. ./config.sh

export KERNEL_BUILD_ARGS="$1"

echo $KERNEL_BUILD_ARGS

for DIR in $PROJECTS; do
    cd $DIR && make build && cd ..
done;

#check if build is good
./scripts/is_multiboot.sh $PROJECT_NAME.kernel

#create symbols
objcopy --only-keep-debug juos.kernel juos.sym

#create storage
qemu-img create -f raw $PROJECT_NAME.storage 4G

