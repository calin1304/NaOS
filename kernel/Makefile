PREFIX=/usr
CFLAGS_INCLUDES := \
	-Iinclude \
	-Ilibc/include \
	-I../libc/include \
	-I$(SYSROOT)/include \
	-I$(SYSROOT)/usr/include
CFLAGS := $(CFLAGS) $(CFLAGS_INCLUDES) -ffreestanding # -D__is_libk

SRCDIR := src
OBJ_DIR := _build

SOURCES := $(shell find $(SRCDIR) -type f -name '*.c')
OBJECTS := $(patsubst $(SRCDIR)/%.c, $(OBJ_DIR)/%.o, $(SOURCES))
OBJ_DIRS := $(sort $(dir $(OBJECTS)))


LIBS := -lgcc -lk
LIBDIR=../libc/
TARGET := kernel.elf
LDFLAGS := -nostdlib -L$(LIBDIR)

.PHONY: all clean install-headers
all: $(OBJ_DIRS) $(TARGET)

$(OBJ_DIRS):
	mkdir -p $@

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ -T linker.ld $(LDFLAGS) $^ $(LIBS)

$(OBJ_DIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

install-headers:
	cp -r include/* $(SYSROOT)/include

clean:
	-rm -f $(OBJECTS) $(TARGET)

