#!/bin/sh

#set up enviormental variables
. ./config.sh

for DIR in $PROJECTS; do
    echo "Bulding:      " $DIR
    cd $DIR && make build
done;