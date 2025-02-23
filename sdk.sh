#!/bin/bash

set -e

usage() {
	cat << EOF
Usage: $0 [subcmd] [options]

Subcommands:
  setup:	Build the docker image
  build:	Build the operating system
  shell:	Run the docker image interactively
EOF
	exit $1
}

TAG=osdev
CACHE=

while getopts ":hc" o; do
	case "${o}" in
		h)
			usage 0
			;;
		c)
			CACHE="--no-cache"
			;;
		t)
			TAG="${OPTARG}"
			;;
	esac
done
shift $((OPTIND - 1))

subcmd="$1"
shift 1

case "${subcmd}" in
	docker)
		docker build -t "$TAG" . ${CACHE} $@
		;;
	setup)
		echo "Build docker image ($TAG)..."
		docker run -it \
			-v .:/src \
			--user="$(id -u):$(id -g)" \
			-t "${TAG}" \
			'meson setup --cross-file=i686-elf_meson.txt build --wipe'

		;;
	build)
		set -x
		docker run -it \
			-v .:/src \
			--user="$(id -u):$(id -g)" \
			-t "${TAG}" \
			'meson compile -C build'
		;;
	shell)
		echo "Entering SDK shell..."
		set -x

		docker run -it \
			-v .:/src \
			--user="$(id -u):$(id -g)" \
			-t "${TAG}" \
			sh
		;;
	help)
		usage 0
		;;
	*)
		usage 1
		;;
esac

