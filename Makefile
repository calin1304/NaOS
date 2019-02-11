export ARCH := i686-elf
export CC := $(ARCH)-gcc
export LD := $(ARCH)-ld
export AS := $(ARCH)-as
export CFLAGS := -Wall -Wextra -I ${abspath .} -ffreestanding -std=c99 -mno-red-zone

KERNEL := kernel/ker.bin
ISODIR := isodir

.PHONY: all iso initrd kernel clean
all: libc kernel initrd iso

iso:	
	cp -f $(KERNEL) $(ISODIR)/boot/
	mkisofs -R -b boot/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o bootcd.iso isodir

initrd:
	tar -c -f $(ISODIR)/boot/naos.initrd initrd/*

libc:
	$(MAKE) -C libc

kernel:
	$(MAKE) -C kernel

clean:
	$(MAKE) -C kernel clean

