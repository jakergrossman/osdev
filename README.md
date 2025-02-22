# osdev
x86 operating system dev fun

```sh
# building i686-elf cross-compile docker toolchain
docker build . -t osdev

# running cross compile environment
docker run --user=$(id -u):$(id -g) -it -v .:/src osdev sh

# compiling
meson setup --cross-file=i686-elf_meson.txt build
meson compile -C build
```
