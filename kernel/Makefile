CC=gcc
ASM=nasm
CFLAGS=-fno-builtin \
		 -fno-stack-protector \
		 -nostartfiles \
		 -ffreestanding \
		 -nostdlib \
		 -m32 \
		 -Wall -Wextra -I include

OBJECTS=.obj/stage2.o \
		.obj/kmain.o \
		.obj/console.o \
		.obj/idt.o \
		.obj/string.o \
		.obj/io.o .obj/pic.o .obj/gdt.o .obj/clock.o .obj/malloc.o .obj/ata.o .obj/fat12.o
TARGET=kernel

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJECTS)
	ld -m elf_i386 -T script.ld $^	

.obj/stage2.o: src/stage2.asm .obj
	$(ASM) -o $@ -f elf32 $<

.obj/%.o: src/%.c .obj
	$(CC) -c -o $@ $(CFLAGS) $<

.obj:
	mkdir $@

.PHONY: clean
clean:
	rm -f .obj/*
	rmdir .obj
	rm -f $(TARGET)