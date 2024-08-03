#!/bin/sh
set -e
. ./iso.sh

# qemu-system-$(./target-triplet-to-arch.sh $HOST) -s -S -M q35,accel=tcg -d int -monitor stdio -cdrom zepos.iso 
qemu-system-$(./target-triplet-to-arch.sh $HOST) "$@" -M q35,accel=tcg -d int -monitor stdio -cdrom zepos.iso 
