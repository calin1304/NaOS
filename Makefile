CC=gcc
ASM=nasm
CC_FLAGS=-fno-builtin \
		 -fno-stack-protector \
		 -nostartfiles \
		 -ffreestanding \
		 -nostdlib \
		 -m32 \
		 -Wall -Wextra -I ./include

OBJECT_FILES=.obj/stage2.o \
				.obj/kmain.o \
				.obj/console.o \
				.obj/idt.o \
				.obj/string.o \
				.obj/io.o .obj/pic.o .obj/gdt.o .obj/clock.o .obj/malloc.o .obj/ata.o .obj/fat12.o

floppy: bootloader build/floppy.img build/stage2.bin
	mount /dev/loop0 build/floppy_mount
	cp build/stage2.bin build/floppy_mount
	cp res/welcome.txt build/floppy_mount
	umount /dev/loop0
	dd if=bootloader/bootloader of=build/floppy.img seek=0 count=1 conv=notrunc

build/floppy.img:
	dd if=/dev/zero of=build/floppy.img bs=1024 count=1440
	mkfs.vfat build/floppy.img

.PHONY: bootloader
bootloader:
	$(MAKE) -C bootloader

build/stage2.bin: $(OBJECT_FILES)
	ld -m elf_i386 -T script.ld $^

.obj/stage2.o: src/stage2.asm
	$(ASM) -o $@ -f elf32 $<

.obj/%.o: src/%.c
	$(CC) -c -o $@ $(CC_FLAGS) $<

.PHONY: clean
clean:
	$(MAKE) -C bootloader clean
	rm .obj/*
	rm build/stage2.bin