#!/bin/sh

#set up enviormental variables
. ./config.sh

for DIR in $PROJECTS; do
    cd $DIR && make build && cd ..
done;

#check if build is good
./is_multiboot.sh $PROJECT_NAME.kernel

