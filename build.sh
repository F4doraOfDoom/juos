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

objcopy --only-keep-debug juos.kernel juos.sym

