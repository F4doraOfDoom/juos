#!/bin/sh

if grub-file --is-x86-multiboot $1; then
    echo IS MUTLIBOOT
else
    echo NOT MUTLIBOOT
fi;