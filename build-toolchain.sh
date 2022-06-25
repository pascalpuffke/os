#!/bin/bash

BUILD_DIR=gnu
TARGET=i686-elf

BINUTILS_VERSION=2.38
GCC_VERSION=12.1.0

if [ ! -d "$BUILD_DIR" ]; then
    echo -e "\e[36mCreating build directory: \e[0m$BUILD_DIR... "
    mkdir -p "$BUILD_DIR/bin"
fi

cd $BUILD_DIR

build_binutils() {
    echo -e "\e[36mBuilding \e[0mbinutils... "

    if [ ! -f "binutils-$BINUTILS_VERSION.tar.xz" ]; then
        local url="https://www.artfiles.org/gnu.org/binutils/binutils-$BINUTILS_VERSION.tar.xz"
        local signature_url="https://www.artfiles.org/gnu.org/binutils/binutils-$BINUTILS_VERSION.tar.xz.sig"

        echo -e "    \e[36mDownloading \e[0mbinutils-$BINUTILS_VERSION... "
        wget $url
        wget $signature_url

        echo -ne "    \e[36mVerifying \e[0mbinutils-$BINUTILS_VERSION signature... "
        gpg --verify binutils-$BINUTILS_VERSION.tar.xz.sig binutils-$BINUTILS_VERSION.tar.xz
        if [ $? -ne 0 ]; then
            echo -e "\e[31mfailed with return code $?.\e[0m"
            exit 1
        fi
        echo "succeeded."
    fi

    if [ ! -d "binutils-$BINUTILS_VERSION" ]; then
        echo -e "    \e[36mExtracting \e[0mbinutils-$BINUTILS_VERSION... "
        tar xf binutils-$BINUTILS_VERSION.tar.xz
    fi

    cd binutils-$BINUTILS_VERSION
    echo -e "    \e[36mConfiguring \e[0mbinutils-$BINUTILS_VERSION for target $TARGET... "
    ./configure --target=$TARGET --prefix=`pwd`/../bin --with-sysroot --disable-nls --disable-werror
    make -j$(nproc)
    make install
    cd ..
}

build_gcc() {
    echo -e "\e[36mBuilding \e[0mgcc... "

    if [ ! -f "gcc-$GCC_VERSION.tar.xz" ]; then
        local url="https://www.artfiles.org/gnu.org/gcc/gcc-$GCC_VERSION/gcc-$GCC_VERSION.tar.xz"
        local signature_url="https://www.artfiles.org/gnu.org/gcc/gcc-$GCC_VERSION/gcc-$GCC_VERSION.tar.xz.sig"

        echo -e "    \e[36mDownloading \e[0mgcc-$GCC_VERSION... "
        wget $url
        wget $signature_url

        echo -ne "    \e[36mVerifying \e[0mgcc-$GCC_VERSION signature... "
        gpg --verify gcc-$GCC_VERSION.tar.xz.sig gcc-$GCC_VERSION.tar.xz
        if [ $? -ne 0 ]; then
            echo -e "\e[31mfailed with return code $?.\e[0m"
            exit 1
        fi
        echo "succeeded."
    fi

    if [ ! -d "gcc-$GCC_VERSION" ]; then
        echo -e "    \e[36mExtracting \e[0mgcc-$GCC_VERSION... "
        tar xf gcc-$GCC_VERSION.tar.xz
    fi

    cd gcc-$GCC_VERSION
    echo -e "    \e[36mConfiguring \e[0mgcc-$GCC_VERSION for target $TARGET... "
    ./configure --target=$TARGET --prefix=`pwd`/../bin --disable-nls --enable-languages=c,c++ --without-headers
    make -j$(nproc) all-gcc
    make -j$(nproc) all-target-libgcc
    make install-gcc
    make install-target-libgcc
    cd ..
}

build_binutils
build_gcc
