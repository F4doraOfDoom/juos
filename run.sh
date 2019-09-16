#!/bin/sh

. ./config.sh silent

if [ "$1" = "iso" ]; then 
    . ./scripts/iso.sh
    $EMULATOR -cdrom juos.iso
elif [ "$1" = "bin" ]; then
    $EMULATOR -kernel $PROJECT_NAME.kernel
fi