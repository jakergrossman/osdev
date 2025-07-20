# osdev (denton)
operating system dev fun

## requirements
- docker

## compilation
denton uses a Makefile wrapper to:
- build a Docker image with Meson and a cross compiler
- build the operating system using the cross compiler with Meson

```
Usage: make [subcommand]

Subcommands:
  sdk              create and import cross-compilation docker image
  setup            initialize meson build directory
  compile          compile operating system
  iso              create ISO image
  clean            delete build objects
  distclean        delete build directory
  qemu             run operating system with qemu-system-<ARCH>
  help             show this help text
```

From zero to QEMU on the build machine:
```sh
QEMU=qemu-system-i386 make sdk qemu
```