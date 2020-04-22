#!/bin/bash

export ARCH=$(./scripts/arch.sh)
export TOOLS=$(./scripts/cross_tools_location.sh)
export HOST=$(./scripts/host.sh)

export DEBUGGER=$(./scripts/debugger.sh)
export EMULATOR=qemu-system-$ARCH
export CC=$TOOLS$HOST-g++
export AS=$TOOLS$HOST-as

#TODO add -O2 when not debugging
export CFLAGS="-Wall -Wextra -g -D__arch_$ARCH -std=c++2a -fmessage-length=0 -Werror -Wwrite-strings -Winit-self -Wcast-align -Wpointer-arith -Wstrict-aliasing -Wformat=2 -Wmissing-include-dirs -Wno-unused-parameter -Wuninitialized"

export CPPFLAGS="-fno-threadsafe-statics"
export LIBC=$PWD/libc
export LIBCXX=$PWD/libstdcxx

export INCLUDE=$LIBC/include
export CXXINCLUDE=$LIBCXX
export KINCLUDE=$PWD/kernel/include/
export AINCLUDE=$PWD/kernel/include/$ARCH/include

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
    echo "DEBUGGER          " $DEBUGGER
    echo "************\n"
fi