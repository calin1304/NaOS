ARCH := i386-elf
CC := $(ARCH)-gcc
LD := $(ARCH)-ld
AS := $(ARCH)-as
AR := $(ARCH)-ar

SYSROOT=${abspath ../sysroot}

CFLAGS_INCLUDES := \
	-Iinclude \
	-Ilibc/include \
	-I../libc/include \
	-I$(SYSROOT)/include \
	-I$(SYSROOT)/usr/include

# `-mgeneral-regs-only` is used because compiler complains that no 80387 instructions are
# allowed in exception service routines.
CFLAGS := \
	-Wall \
	-Wextra \
	-ffreestanding \
	-std=c99 \
	-mno-red-zone \
	-mgeneral-regs-only \
	-ffreestanding \
	$(CFLAGS_INCLUDES)

SRCDIR := src
OBJ_DIR := _build

SOURCES := $(shell find $(SRCDIR) -type f -name '*.c')
OBJECTS := $(patsubst $(SRCDIR)/%.c, $(OBJ_DIR)/%.o, $(SOURCES))

OBJ_DIRS := $(sort $(dir $(OBJECTS)))

LIBS := -lgcc
LDFLAGS := -nostdlib
TARGET := kernel.elf

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

