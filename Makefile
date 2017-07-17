export CC := gcc
export ASM := nasm
export CFLAGS := -fno-builtin \
		 -fno-stack-protector \
		 -nostartfiles \
		 -ffreestanding \
		 -nostdlib \
		 -Wall -Wextra -I ${abspath .} \
		 -m32 
export LDFLAGS := -L $(abspath ./libk)

OBJECT_FILES := .obj/stage2.o \
				.obj/kmain.o \
				.obj/console.o \
				.obj/idt.o \
				.obj/string.o \
				.obj/io.o .obj/pic.o .obj/gdt.o .obj/clock.o .obj/malloc.o .obj/ata.o .obj/fat12.o

BOOTLOADER := bootloader/bootloader
KERNEL := kernel/kernel

.PHONY: all
all: bootloader libk kernel

.PHONY: floppy
floppy: build build/floppy.img
	mount /dev/loop0 build/floppy_mount
	cp $(KERNEL) build/floppy_mount
	cp res/welcome build/floppy_mount
	cp app build/floppy_mount
	cp init build/floppy_mount
	umount /dev/loop0
	dd if=$(BOOTLOADER) of=build/floppy.img seek=0 count=1 conv=notrunc

build:
	@mkdir -p build/floppy_mount

.PHONY: bootloader
bootloader:
	$(MAKE) -C bootloader

.PHONY: kernel
kernel:
	$(MAKE) -C kernel

.PHONY: libk
libk:
	$(MAKE) -C libk

.PHONY: clean
clean:
	$(MAKE) -C bootloader clean
	$(MAKE) -C libk clean
	$(MAKE) -C kernel clean

build/floppy.img:
	dd if=/dev/zero of=build/floppy.img bs=1024 count=1440
	mkfs.vfat build/floppy.img
