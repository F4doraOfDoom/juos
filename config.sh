#!/bin/sh

export ARCH=$(./scripts/arch.sh)
export TOOLS=$(./scripts/cross_tools_location.sh)
export HOST=$(./scripts/host.sh)

export EMULATOR=qemu-system-$ARCH
export CC=$TOOLS$HOST-g++
export AS=$TOOLS$HOST-as

#TODO add -O2 when not debugging
export CFLAGS="-Wall -Wextra -g -D__arch_$ARCH -std=c++1z"
export CPPFLAGS=""
export LIBC=$PWD/libc
export INCLUDE=$LIBC/include
export KINCLUDE=$PWD/kernel/include
export AINCLUDE=$PWD/arch/$ARCH/include

export ROOT=$PWD
export PROJECTS="libc kernel" #important to build libc first

export PROJECT_NAME="juos"

if [ "$1" = "silent" ]; then
    echo ""
else
    echo "***CONFIG***"
    echo "Using arch:       " $ARCH
    echo "Tools from:       " $TOOLS
    echo "Host is:          " $HOST
    echo "Compiler:         " $CC
    echo "CFLAGS:           " $CFLAGS
    echo "CPPFLAGS          " $CPPFLAGS
    echo "************\n"
fi