CFLAGS := $(CFLAGS) -I./ -I./include -I./libk/include -g
SRCDIR := .
BUILDDIR := .obj
SOURCES := $(shell find $(SRCDIR) -type f -name '*.c')
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:%.c=%.o))
LIBS := -lgcc -lk
TARGET := ker.bin
LDFLAGS := -nostdlib -L./libk

.PHONY: all
all: $(TARGET)

$(TARGET): libk kernel
	mv kernel $@

kernel: $(BUILDDIR)/isr.o $(OBJECTS) boot.o
	$(CC) -o $@ -T linker.ld $(LDFLAGS) $^ $(LIBS)

boot.o: boot.asm
	nasm -felf32 boot.asm -o boot.o

$(BUILDDIR)/isr.o: $(SRCDIR)/isr.asm
	@mkdir -p $(dir $@)
	$(ASM) -o $@ -f elf32 $<

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) -c -o $@ $(CFLAGS) $<

.PHONY: libk
libk:
	$(MAKE) -C libk

.PHONY: clean
clean:
	$(MAKE) -C libk clean
	$(MAKE) -C entry clean
	rm -f .obj/*.o
	rm -f $(OBJECTS)
	rm -f kernel
	rm -f boot.o
