#!/bin/sh

export ARCH=$(./arch.sh)
export TOOLS=$(./cross_tools_location.sh)
export HOST=$(./host.sh)

export CC=$TOOLS$HOST-g++
export CFLAGS="-O2 -Wall -Wextra -O2"
export CPPFLAGS=""

export PROJECTS="kernel libc"

echo "***CONFIG***"
echo "Using arch:       " $ARCH
echo "Tools from:       " $TOOLS
echo "Host is:          " $HOST
echo "Compiler:         " $CC
echo "CFLAGS:           " $CFLAGS
echo "CPPFLAGS          " $CPPFLAGS
echo "************\n"
