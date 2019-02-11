export ARCH := i686-elf
export CC := $(ARCH)-gcc
export LD := $(ARCH)-ld
export AS := $(ARCH)-as
export CFLAGS := -Wall -Wextra -I ${abspath .} -ffreestanding -std=c99 -mno-red-zone

KERNEL := kernel/ker.bin
ISODIR := isodir

.PHONY: all
all: kernel initrd iso

.PHONY: iso
iso:	
	cp -f $(KERNEL) $(ISODIR)/boot/
	mkisofs -R -b boot/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o bootcd.iso isodir

.PHONY: initrd
initrd:
	tar -c -f $(ISODIR)/boot/naos.initrd initrd/*

.PHONY: kernel
kernel:
	$(MAKE) -C kernel

.PHONY: clean
clean:
	$(MAKE) -C kernel clean

