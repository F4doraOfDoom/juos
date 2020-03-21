#!/bin/sh

. ./config.sh silent
. ./scripts/iso.sh

DEBUG_ARGS=""
RUN_DEBUGGER=""
if [ "$2" = "DEBUG" ]; then
    objcopy --only-keep-debug $PROJECT_NAME.kernel $PROJECT_NAME.sym
    DEBUG_ARGS="-s -S"
    # basically just connect gdb to QEMU, load symbols, stop at main
    DEBUGGER_ARGS="$DEBUGGER -ex 'target remote localhost:1234' -ex 'symbol-file $PROJECT_NAME.sym' -ex 'b kernel_main' -ex 'continue'"
    # not compatible with other terminels. since it is local to my machine
    RUN_DEBUGGER="termite -e \"$DEBUGGER_ARGS\""
fi

$EMULATOR -cdrom juos.iso $DEBUG_ARGS -hda $PROJECT_NAME.storage & -m 4G
echo RUN DEBUGGER $RUN_DEBUGGER
sleep 0.2
eval $RUN_DEBUGGER