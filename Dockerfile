FROM alpine:latest AS base

LABEL maintainer="jak <ahoy@jakergrossman.com>"
LABEL description="i686-elf GCC+binutils toolchain for osdev"

ARG ARCH="aarch64"

ENV ARCH="${ARCH}"
ENV TARGET="${ARCH}-elf" \
    BINUTILS_VERSION="2.44" \
    GCC_VERSION="14.2.0"\
    PKGDIR="/pkg" \
    PREFIX="/usr/local"

RUN apk add --no-cache --virtual .deps \
    curl file gcc make musl-dev build-base coreutils git \
    mpc1-dev gmp-dev mpfr-dev meson grub mtools xorriso

FROM base AS build

WORKDIR /usr/local/src/
ADD https://ftp.gnu.org/gnu/binutils/binutils-$BINUTILS_VERSION.tar.gz \
    binutils-$BINUTILS_VERSION.tar.gz
RUN tar xf binutils-$BINUTILS_VERSION.tar.gz
ADD https://ftp.gnu.org/gnu/gcc/gcc-$GCC_VERSION/gcc-$GCC_VERSION.tar.gz \
    gcc-$GCC_VERSION.tar.gz
RUN tar xf gcc-$GCC_VERSION.tar.gz

RUN mkdir build-binutils \
    && cd build-binutils \
    && ../binutils-$BINUTILS_VERSION/configure \
        --target=$TARGET --enable-multiroot --with-sysroot --disable-nls \
    && make --silent -j$(nproc) \
    && make --silent DESTDIR="$PKGDIR" install

# ensure binutils is available for GCC build
ENV PATH="${PKGDIR}/usr/local/bin:${PATH}"
RUN mkdir build-gcc \
    && cd build-gcc \
    && ../gcc-$GCC_VERSION/configure \
        --target=$TARGET --disable-nls --enable-languages=c \
        --without-headers --disable-werror --enable-multilib \
    && make -j$(nproc) all-gcc \
    && make -j$(nproc) all-target-libgcc \
    && make --silent DESTDIR="$PKGDIR" install-gcc \
    && make --silent DESTDIR="$PKGDIR" install-target-libgcc

FROM base AS sdk

ENV __OSDEV_ENV__=1

COPY --from=build "$PKGDIR"/usr/local /usr/local
COPY meson-cross/$TARGET.cross /usr/local/share/meson/cross/$TARGET
WORKDIR /src
#ENTRYPOINT ["/bin/sh", "-c"]
