#!/bin/sh
. ./config.sh

for DIR in $PROJECTS; do
    cd $DIR && make clean && cd ..
done;