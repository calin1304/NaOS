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
				.obj/io.o .obj/pic.o .obj/gdt.o .obj/clock.o

floppy: build/floppy.img build/stage1.bin build/stage2.bin
	mount /dev/loop0 build/floppy_mount
	cp build/stage2.bin build/floppy_mount
	umount /dev/loop0
	dd if=build/stage1.bin of=build/floppy.img seek=0 count=1 conv=notrunc

build/floppy.img:
	dd if=/dev/zero of=build/floppy.img bs=1024 count=1440
	mkfs.vfat build/floppy.img
	
# iso: build/stage1.bin build/stage2.bin
# 	dd if=/dev/zero of=build/floppy.img bs=1024 count=1440
# 	dd if=build/stage1.bin of=build/floppy.img seek=0 count=1 conv=notrunc
# 	cp build/floppy.img build/iso
# 	cp build/stage2.bin build/iso
# 	genisoimage -quiet -V "MYOS" -input-charset iso8859-1 -o build/myos.iso -b floppy.img build/iso/

build/stage1.bin: src/stage1.asm
	$(ASM) -o $@ -f bin $< 

build/stage2.bin: $(OBJECT_FILES)
	ld -m elf_i386 -T script.ld $^

.obj/stage2.o: src/stage2.asm
	$(ASM) -o $@ -f elf32 $<

# .obj/kmain.o: src/kmain.c
	# $(CC) -c -o $@ $(CC_FLAGS) $<

.obj/%.o: src/%.c
	$(CC) -c -o $@ $(CC_FLAGS) $<

clean:
	rm .obj/*
	rm build/stage2.bin