export ARCH := i686-elf
export CC := $(ARCH)-gcc
export LD := $(ARCH)-ld
export AS := $(ARCH)-as

export SYSROOT=${abspath ./sysroot}

export CFLAGS := -Wall -Wextra -ffreestanding -std=c99 -mno-red-zone

KERNEL := kernel/ker.bin
ISODIR := isodir

.PHONY: all iso initrd libc kernel install-headers clean
all: libc kernel initrd iso

iso:	
	cp -f $(KERNEL) $(ISODIR)/boot/
	mkisofs -R -b boot/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o bootcd.iso isodir

initrd:
	tar -c -f $(ISODIR)/boot/naos.initrd initrd/*

libc:
	$(MAKE) -C libc

kernel:
	cd kernel && stack runhaskell Shakefile.hs


install-headers:
	$(MAKE) -C libc install-headers
	$(MAKE) -C kernel install-headers

clean:
	$(MAKE) -C libc clean
	$(MAKE) -C kernel clean

