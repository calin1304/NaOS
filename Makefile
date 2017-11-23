export TARGET := i686-elf
export CC := $(TARGET)-gcc
export LD := $(TARGET)-ld
export ASM := nasm
export CFLAGS := -Wall -Wextra -I ${abspath .} -ffreestanding

BOOTLOADER := bootloader/bootloader
KERNEL := kernel/ker.bin
DISK_MOUNT_LOCATION := build/floppy_mnt

.PHONY: all
all: bootloader kernel

.PHONY: floppy
floppy: build build/floppy.img
	mount /dev/loop0 $(DISK_MOUNT_LOCATION)
	cp $(KERNEL) $(DISK_MOUNT_LOCATION)
	cp res/welcome $(DISK_MOUNT_LOCATION)
	cp apps/* $(DISK_MOUNT_LOCATION)
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

.PHONY: clean
clean:
	$(MAKE) -C bootloader clean
	$(MAKE) -C kernel clean

build/floppy.img:
	dd if=/dev/zero of=build/floppy.img bs=1024 count=1440
	mkfs.vfat build/floppy.img
