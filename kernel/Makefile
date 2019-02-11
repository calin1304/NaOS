CFLAGS := $(CFLAGS) -I./ -I./include -I./libk/include -I../libc/include -mno-80387
SRCDIR := src
BUILDDIR := .obj
SOURCES := $(shell find $(SRCDIR) -type f -name '*.c')
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:%.c=%.o))
LIBS := -lgcc -lk
LIBDIR=../libc/
TARGET := ker.bin
LDFLAGS := -nostdlib -L$(LIBDIR)

.PHONY: all clean
all: $(TARGET)

$(TARGET): $(OBJECTS) $(BUILDDIR)/boot.o
	$(CC) -o $@ -T linker.ld $(LDFLAGS) $^ $(LIBS)

$(BUILDDIR)/boot.o: $(SRCDIR)/boot.asm
	$(AS) $(AS_FLAGS) $^ -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/%.c $(BUILDDIR)
	$(CC) -c -o $@ $(CFLAGS) $<

$(BUILDDIR):
	mkdir $@

clean:
	rm -f $(BUILDDIR)/*.o
	rm -f $(OBJECTS)
	rm -f $(TARGET)
	rm -f boot.o
