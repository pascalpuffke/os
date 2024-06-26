#!/bin/bash

# Can you tell I have no idea what a makefile is?
# NOTE that this script assumes a working i686-elf cross compiler is present.

CROSS_PREFIX=gnu/bin

ASSEMBLER=$CROSS_PREFIX/i686-elf-as
LINKER=$CROSS_PREFIX/i686-elf-ld
CXX=$CROSS_PREFIX/i686-elf-g++

# Either make an ISO or run the raw multiboot kernel in qemu
USE_ISO=1

WARNINGS="-Wall -Wextra -Wshadow -Wold-style-cast -Woverloaded-virtual -Wpedantic -Wconversion -Wsign-conversion -Wimplicit-fallthrough"

if [ ! -d "build" ]; then
	mkdir build
fi

boot() {
	echo -ne "\e[36mBuilding \e[0mboot... "

	$ASSEMBLER src/boot.S -o build/boot.o
	if [ $? -ne 0 ]; then
		echo -e "\e[31mBuild failed with code $?.\e[0m"
		exit 1
	fi
}

libraries() {
	local libc_flags="-Iinclude -ffreestanding -O3 $WARNINGS -Wno-unused-function -std=c++23 -fno-exceptions -fno-rtti"

	echo -e "\e[36mBuilding \e[0mlibc... "

	if [ ! -d "build/libc" ]; then
		mkdir -p build/libc
	fi

	for file in $(find src/libraries/libc -name "*.cpp"); do
		echo -e "    \e[36mCompiling \e[0m$file... "
		$CXX -c $file -o build/libc/$(basename $file .cpp).o $libc_flags
		if [ $? -ne 0 ]; then
			echo -e "\e[31mBuild failed with code $?.\e[0m"
			exit 1
		fi
	done
}

kernel() {
	local kernel_flags="-Iinclude -ffreestanding -O3 $WARNINGS -Wno-unused-function -std=c++23 -fno-exceptions -fno-rtti"

	echo -e "\e[36mBuilding \e[0mkernel... "

	for file in $(find src/kernel -name "*.cpp"); do
		echo -e "    \e[36mCompiling \e[0m$file... "
		$CXX -c $file -o build/$(basename $file .cpp).o $kernel_flags
		if [ $? -ne 0 ]; then
			echo -e "\e[31mBuild failed with return code $?.\e[0m"
			exit 1
		fi
	done
}

link() {
	echo -ne "\e[36mLinking\e[0m... "

	local linker_flags="-ffreestanding -O2 -nostdlib -lgcc"

	local objects=""
	for file in $(find build/ -name "*.o"); do
		objects="$objects $file"
	done

	echo -e "\e[90m$CXX -T src/linker.ld -o build/kernel.bin $objects $linker_flags\e[0m"
	$CXX -T src/linker.ld -o build/kernel.bin $objects $linker_flags
	if [ $? -ne 0 ]; then
		echo -e "\e[31mLinking failed with return code $?.\e[0m"
		exit 1
	fi
}

verify_multiboot() {
	echo -ne "\e[36mVerifying \e[0mmultiboot... "

	grub-file --is-x86-multiboot build/kernel.bin
	if [ $? -ne 0 ]; then
		echo -e "\e[31mfailed with return code $?.\e[0m"
		exit 1
	fi
	echo "succeeded."
}

make_iso() {
	echo -e "\e[36mMaking \e[0mISO..."

	if [ ! -d "iso" ]; then
		echo -e "    \e[36mCreating \e[0mISO directory..."
		mkdir -p iso/boot/grub
	fi

	if [ ! -f "iso/grub.cfg" ]; then
		echo -e "    \e[36mgrub.cfg not found, creating\e[0m..."
		echo "menuentry \"Kernel\" {" >>iso/grub.cfg
		echo "	multiboot /boot/kernel.bin" >>iso/grub.cfg
		echo "}" >>iso/grub.cfg
	fi

	cp build/kernel.bin iso/boot/kernel.bin
	cp iso/grub.cfg iso/boot/grub/grub.cfg
	grub-mkrescue -o kernel.iso iso
}

run_qemu() {
	local qemu_flags="-serial stdio -m 128M --enable-kvm -cpu host -vga std"

	if [ $USE_ISO -eq 0 ]; then
		echo -e "    \e[36mRunning \e[0mQEMU with raw multiboot kernel..."
		qemu-system-i386 $qemu_flags -kernel build/kernel.bin
	else
		echo -e "    \e[36mRunning \e[0mQEMU with ISO..."
		qemu-system-i386 $qemu_flags -cdrom kernel.iso
	fi
}

build() {
	boot
	libraries
	kernel
	link
	verify_multiboot
}

build

if [ "$1" == "run" ]; then
	if [ $USE_ISO -eq 1 ]; then
		make_iso
		run_qemu
	else
		run_qemu
	fi
fi
