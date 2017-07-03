export CC := gcc
export ASM := nasm
export CFLAGS := -fno-builtin \
		 -fno-stack-protector \
		 -nostartfiles \
		 -ffreestanding \
		 -nostdlib \
		 -m32 \
		 -Wall -Wextra -I ${abspath .}
export LDFLAGS := -L $(abspath "./libc")

OBJECT_FILES := .obj/stage2.o \
				.obj/kmain.o \
				.obj/console.o \
				.obj/idt.o \
				.obj/string.o \
				.obj/io.o .obj/pic.o .obj/gdt.o .obj/clock.o .obj/malloc.o .obj/ata.o .obj/fat12.o

BOOTLOADER := bootloader/bootloader
KERNEL := kernel/kernel

.PHONY: all
all: bootloader libc kernel

.PHONY: floppy
floppy: build/floppy.img
	mount /dev/loop0 build/floppy_mount
	cp $(KERNEL) build/floppy_mount
	cp res/welcome.txt build/floppy_mount
	umount /dev/loop0
	dd if=$(BOOTLOADER) of=build/floppy.img seek=0 count=1 conv=notrunc

.PHONY: bootloader
bootloader:
	$(MAKE) -C bootloader

.PHONY: kernel
kernel:
	$(MAKE) -C kernel

.PHONY: libc
libc:
	$(MAKE) -C libc

.PHONY: clean
clean:
	$(MAKE) -C bootloader clean
	$(MAKE) -C libc clean
	$(MAKE) -C kernel clean

build/floppy.img:
	dd if=/dev/zero of=build/floppy.img bs=1024 count=1440
	mkfs.vfat build/floppy.img
