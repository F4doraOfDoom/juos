#!/bin/sh

#set up enviormental variables
. ./config.sh

for DIR in $PROJECTS; do
    cd $DIR && make build && cd ..
done;

