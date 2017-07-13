CFLAGS := $(CFLAGS) -I./include
SRCDIR := .
BUILDDIR := .obj
SOURCES := $(shell find $(SRCDIR) -type f -name '*.c')
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:%.c=%.o))
LIBS := -lc
TARGET := kernel
LDFLAGS := -L "../libc"

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJECTS) $(BUILDDIR)/stage2.o
	ld -m elf_i386 -T script.ld $(LDFLAGS) $^ $(LIBS)

$(BUILDDIR)/stage2.o: $(SRCDIR)/stage2.asm
	@mkdir -p $(dir $@)
	$(ASM) -o $@ -f elf32 $<

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) -c -o $@ $(CFLAGS) $<

.PHONY: clean
clean:
	rm -f .obj/*.o
	rm -f $(OBJECTS)
	rm -f $(TARGET)