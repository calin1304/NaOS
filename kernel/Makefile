CFLAGS := $(CFLAGS) -I./ -I./include -I./libk/include -g -mno-80387
SRCDIR := src
BUILDDIR := .obj
SOURCES := $(shell find $(SRCDIR) -type f -name '*.c')
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:%.c=%.o))
LIBS := -lgcc -lk
TARGET := ker.bin
LDFLAGS := -nostdlib -L./libk

.PHONY: all
all: libk $(TARGET)

$(TARGET): $(OBJECTS) $(BUILDDIR)/boot.o
	$(CC) -o $@ -T linker.ld $(LDFLAGS) $^ $(LIBS)

$(BUILDDIR)/boot.o: $(SRCDIR)/boot.asm
	$(AS) $(AS_FLAGS) $^ -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/%.c $(BUILDDIR)
	$(CC) -c -o $@ $(CFLAGS) $<

$(BUILDDIR):
	mkdir $@

.PHONY: libk
libk:
	$(MAKE) -C libk

.PHONY: clean
clean:
	$(MAKE) -C libk clean
	rm -f $(BUILDDIR)/*.o
	rm -f $(OBJECTS)
	rm -f $(TARGET)
	rm -f boot.o
