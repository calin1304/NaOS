CPPFLAGS=-fno-builtin \
		 -fno-stack-protector \
		 -nostartfiles \
		 -ffreestanding \
		 -nostdlib \
		 -m32 \
		 -Wall -Wextra # -Werror
	
floppy: build/floppy.img build/stage1.bin build/stage2.bin
	mount /dev/loop0 build/floppy_mount
	cp build/stage2.bin build/floppy_mount
	umount /dev/loop0
	dd if=build/stage1.bin of=build/floppy.img seek=0 count=1 conv=notrunc

build/floppy.img:
	dd if=/dev/zero of=build/floppy.img bs=1024 count=1440
	mkfs.vfat build/floppy.img
	
iso: build/stage1.bin build/stage2.bin
	dd if=/dev/zero of=build/floppy.img bs=1024 count=1440
	dd if=build/stage1.bin of=build/floppy.img seek=0 count=1 conv=notrunc
	cp build/floppy.img build/iso
	cp build/stage2.bin build/iso
	genisoimage -quiet -V "MYOS" -input-charset iso8859-1 -o build/myos.iso -b floppy.img build/iso/

build/stage1.bin: src/stage1.asm
	nasm -o $@ -f bin $< 

build/stage2.bin: obj/stage2.o obj/kmain.o
	# nasm -o build/stage2.bin -i src/ -f bin src/stage2.asm
	ld -m elf_i386 -T script.ld obj/stage2.o obj/kmain.o

obj/stage2.o: src/stage2.asm
	nasm -o $@ -f elf32 $<

obj/kmain.o: src/kmain.cpp
	g++ -c -o $@ $(CPPFLAGS) $<

debug:
	python3 ./build.py
	qemu-system-i386 -s -S build/build.img

all:
	python3 ./build.py
	qemu-system-i386 -drive format=raw,file=build/build.img