#!/bin/sh

. ./config.sh silent

ISO_ROOT="isodir"

mkdir -p $ISO_ROOT
mkdir -p $ISO_ROOT/boot
mkdir -p $ISO_ROOT/boot/grub

cp "$PROJECT_NAME".kernel $ISO_ROOT/boot/"$PROJECT_NAME".kernel
cat > $ISO_ROOT/boot/grub/grub.cfg << EOF
menuentry "$PROJECT_NAME" {
	multiboot /boot/$PROJECT_NAME.kernel
}
EOF

grub-mkrescue -o $PROJECT_NAME.iso $ISO_ROOT