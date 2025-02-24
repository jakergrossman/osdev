#!/bin/sh

usage() {
	cat << EOF
Usage: $0 SYSROOT_PATH ISO_PATH

Create a bootable ISO from a compiled sysroot
EOF
> $1
	exit $2
}

if [ "$#" -lt 2 ]; then
	usage /dev/stderr 1
fi

SYSROOT_PATH=$1
ISO_PATH=$2
shift 2

grub-mkrescue -o "${ISO_PATH}" "${SYSROOT_PATH}"
