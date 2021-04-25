export ARCH := i386-elf
export CC := $(ARCH)-gcc
export LD := $(ARCH)-ld
export AS := $(ARCH)-as
export AR := $(ARCH)-ar

export SYSROOT=${abspath ./sysroot}

# `-mgeneral-regs-only` is used because compiler complains that no 80387 instructions are
# allowed in exception service routines.
export CFLAGS := \
	-Wall \
	-Wextra \
	-ffreestanding \
	-std=c99 \
	-mno-red-zone \
	-mgeneral-regs-only \
	-Ikernel/include

KERNEL := kernel/ker.bin
ISODIR := isodir

.PHONY: all iso initrd kernel install-headers clean
all: kernel initrd iso

iso:
	cp -f $(KERNEL) $(ISODIR)/boot/
	mkisofs -R -b boot/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o bootcd.iso isodir

initrd:
	tar -c -f $(ISODIR)/boot/naos.initrd initrd/*

kernel:
	$(MAKE) -C kernel

install-headers:
	$(MAKE) -C kernel install-headers

clean:
	$(MAKE) -C kernel clean

