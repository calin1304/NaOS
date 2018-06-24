CFLAGS := $(CFLAGS) -I./ -I./include -I./libk/include -g -mno-80387
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

kernel: $(OBJECTS) boot.o
	$(CC) -o $@ -T linker.ld $(LDFLAGS) $^ $(LIBS)

boot.o: boot.asm
	$(AS) $(AS_FLAGS) $^ -o $@

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
