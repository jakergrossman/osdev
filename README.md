# osdev (denton)
operating system dev fun

## compilation
denton uses docker to build a cross compilation environment:

```
Usage: make [subcommand]

Subcommands:
  sdk              create and import cross-compilation docker image
  setup            initialize meson build directory
  compile          compile operating system
  iso              create ISO image
  clean            delete build objects
  distclean        delete build directory
  qemu-<ARCH>      run operating system with qemu-system-<ARCH>
  help             show this help text
```

From zero to QEMU on the build machine:
```sh
QEMU=qemu-system-i386 make sdk qemu
```