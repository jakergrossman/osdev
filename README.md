# osdev (denton)
x86 operating system dev fun

## compilation
denton uses docker to build a cross compilation environment

Use the makefile [`osdev.mk`](osdev.mk) to create the build
environment and compile the operating system:

```
Usage: osdev.mk [subcommand]

Subcommands:
  sdk              create cross-compilation SDK
  setup            initialize meson build directory
  compile          compile operating system
  iso              create ISO image
  qemu             run operating system with qemu
  clean            meson clean
  distclean        delete bin directory
  help             show this help text
```

From zero to QEMU on the build machine:
```sh
QEMU=qemu-system-i386 make sdk qemu
```