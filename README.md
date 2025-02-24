# osdev (denton)
x86 operating system dev fun

## manual build

```sh
# building i686-elf cross-compile docker toolchain (build machine)
docker build . -t osdev

# enter interactive cross compile environment (build machine)
docker run --user=$(id -u):$(id -g) -it -v .:/src osdev sh

# compiling (cross environment)
meson setup --cross-file=i686-elf build
meson compile -C build

# sysroot generation (cross environment)
meson install -C build --destdir=sysroot

# ISO generation (build machine)
scripts/iso.sh build/sysroot os.iso

# Run QEMU (build machine)
qemu-system-i386 -no-reboot -no-shutdown -cdrom os.iso
```

## script build
A script `sdk.sh` is provided for convenience. Most subcommands
can be executed inside or outside of the Docker VM and the appropriate
action is taken accordingly.

```sh
Usage: sdk.sh [options] [subcmd]

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
```
