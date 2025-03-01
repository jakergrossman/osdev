#!/bin/sh

set -e

# Usage: usage EXIT_CODE
# Example: usage 0
usage() {
    if [ "$1" -eq 0 ]; then
        FILE=/dev/stdout
    else
        FILE=/dev/stderr
    fi
    cat << EOF
Usage: $0 [options] [subcmd]

Options:
  -c        pass --no-cache to docker build
  -t        set the tag parameter for the docker image
  -h        Print this help and exit
  -v        Print verbosely

Subcommands:
  docker:   Build the docker image
  setup:    Setup/clean the Meson build system
  build:    Build the operating system
  shell:    Run the docker image interactively
  sysroot:  Generate a sysroot in the build directory
  iso:      Generate an ISO image from the generated sysroot
  qemu:     Run the generated ISO with qemu-system-i386
  help:     Print this help and exit
EOF
> $FILE
    exit $1
}

check_docker() {
    if [ -z "$(docker images -q "${TAG}" 2> /dev/null)" ]; then
        echo "ERROR: docker image "${TAG}" not build!" 2>&1
        echo "ERROR: needs '$0 docker' to be run from the build machine" 2>&1
        usage 1
    fi
}

# Usage: if in_docker ; then echo "IM IN"; fi
in_docker() {
    test -n "${__OSDEV_ENV__}"
}

# Usage: sdk_exec COMMAND [ARGS...]
# Run COMMAND, ensuring it is run with
# in the Docker cross-compilation environment.
sdk_exec() {
        if in_docker ; then
            $@
        else
            check_docker
            docker run -it \
                -v .:/src \
                --user="$(id -u):$(id -g)" \
                -t "${TAG}" \
                $@
        fi
}

TAG=osdev
CACHE=
VERBOSE=0

while getopts ":hc:t:v" o; do
    case "${o}" in
        v)
            VERBOSE=$((VERBOSE+1))
            ;;
        c)
            CACHE="--no-cache"
            ;;
        t)
            TAG="${OPTARG}"
            ;;
        h)
            usage 0
            ;;
    esac
done
shift $((OPTIND - 1))

if [ "$#" -eq 0 ]; then
    echo "Missing subcommand" 2>&1
    usage 1
fi

subcmd="$1"
shift 1

if [ "$VERBOSE" -gt 0 ]; then
    set -x
fi

case "${subcmd}" in
    docker)
        docker build -t "$TAG" . ${CACHE} $@
        ;;

    setup)
        sdk_exec meson setup --cross-file=i686-elf build --wipe $@
        ;;

    build)
        sdk_exec meson compile -C build $@ \
            $([ "${VERBOSE}" -gt 0 ] && echo "-v")
        ;;

    shell)
        if in_docker ; then
            echo "Cannot exec shell from within Docker environment" 2>&1
            usage 1
        fi

        sdk_exec sh $@
        ;;

    sysroot)
        sdk_exec meson install -C build --destdir sysroot $@
        ;;

    iso)
        if in_docker ; then
            echo "TODO: ISO build is unsupported in the docker environment" 2>&1
            usage 1
        fi

        if [ ! -d "build/sysroot" ]; then
            echo "sysroot has not been setup" 2>&1
            usage 1
        fi

        scripts/iso.sh build/sysroot build/denton.iso

        ;;

    qemu)
        if [ ! -f "build/denton.iso" ]; then
            echo "ISO has not been generated" 2>&1
            usage 1
        fi

        qemu-system-i386 \
            -no-reboot \
            -no-shutdown \
            -serial stdio \
            -vga virtio \
            -D qemu.log \
            -cdrom build/denton.iso
        ;;

    help)
        usage 0
        ;;

    *)
        echo "Unknown subcommand: $1" 2>&1
        usage 1
        ;;
esac
