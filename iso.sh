#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/zepos.kernel isodir/boot/zepos.kernel
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "zepos" {
	multiboot /boot/zepos.kernel
}
EOF
grub-mkrescue -o zepos.iso isodir
