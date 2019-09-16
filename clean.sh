#!/bin/sh
. ./config.sh

rm $PROJECT_NAME.kernel
rm $PROJECT_NAME.iso
rm -rf isodir/

for DIR in $PROJECTS; do
    cd $DIR && make clean && cd ..
done;