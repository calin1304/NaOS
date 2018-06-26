export ARCH := i686-elf
export CC := $(ARCH)-gcc
export LD := $(ARCH)-ld
export AS := $(ARCH)-as
export CFLAGS := -Wall -Wextra -I ${abspath .} -ffreestanding

KERNEL := kernel/ker.bin
ISODIR := isodir

.PHONY: all
all: kernel iso

.PHONY: iso
iso:	
	cp -f $(KERNEL) $(ISODIR)/
	mkisofs -R -b boot/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o bootcd.iso isodir

.PHONY: kernel
kernel:
	$(MAKE) -C kernel

.PHONY: clean
clean:
	$(MAKE) -C kernel clean

