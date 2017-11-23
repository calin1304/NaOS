CFLAGS := $(CFLAGS) -I./include -I./libk/include -g
SRCDIR := .
BUILDDIR := .obj
SOURCES := $(shell find $(SRCDIR) -type f -name '*.c')
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:%.c=%.o))
LIBS := -lgcc -lk
TARGET := ker.bin
LDFLAGS := -nostdlib -L./libk #-Map kernel.map

.PHONY: all
all: $(TARGET)

$(TARGET): libk entry kernel
	./mkimg.py -e entry/entry -f kernel -o $(TARGET)

kernel: $(OBJECTS) $(BUILDDIR)/stage2.o
	$(CC) -o $@ -T script.ld $(LDFLAGS) $^ $(LIBS)
	./mksymlist.py kernel symbols
	#objcopy --only-keep-debug $@ -o kernel.dbg
	objcopy --strip-debug $@

$(BUILDDIR)/stage2.o: $(SRCDIR)/stage2.asm
	@mkdir -p $(dir $@)
	$(ASM) -o $@ -f elf32 $<

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) -c -o $@ $(CFLAGS) $<

.PHONY: libk
libk:
	$(MAKE) -C libk

.PHONY: entry
entry:
	$(MAKE) -C entry

.PHONY: clean
clean:
	$(MAKE) -C libk clean
	$(MAKE) -C entry clean
	rm -f .obj/*.o
	rm -f $(OBJECTS)
	rm -f kernel
