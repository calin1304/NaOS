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

