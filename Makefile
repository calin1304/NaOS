export ARCH := i686-elf
export CC := $(ARCH)-gcc
export LD := $(ARCH)-ld
export ASM := nasm
export CFLAGS := -Wall -Wextra -I ${abspath .} -ffreestanding

KERNEL := kernel/ker.bin
ISODIR := isodir

.PHONY: all
all: kernel iso

.PHONY: iso
iso:	
	cp -f $(KERNEL) $(ISODIR)/boot/
	grub-mkrescue -o naos.iso $(ISODIR)

.PHONY: kernel
kernel:
	$(MAKE) -C kernel

.PHONY: clean
clean:
	$(MAKE) -C kernel clean

