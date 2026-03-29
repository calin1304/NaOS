PREFIX=/usr
CFLAGS_INCLUDES := \
	-Iinclude \
	-Ilibc/include \
	-I../libc/include \
	-I$(SYSROOT)/include \
	-I$(SYSROOT)/usr/include
CFLAGS := $(CFLAGS) $(CFLAGS_INCLUDES) -ffreestanding

INCLUDE_DIR := include
SRCDIR := src
OBJ_DIR := _build

SOURCES := $(shell find $(SRCDIR) -type f -name '*.c')
OBJECTS := $(patsubst $(SRCDIR)/%.c, $(OBJ_DIR)/%.o, $(SOURCES))
OBJ_DIRS := $(sort $(dir $(OBJECTS)))

LIBS := -lgcc
TARGET := _build/kernel.elf
LDFLAGS := -nostdlib

.PHONY: all format check clean install-headers
all: $(OBJ_DIRS) $(TARGET)

$(OBJ_DIRS):
	mkdir -p $@

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ -T linker.ld $(LDFLAGS) $^ $(LIBS)

$(OBJ_DIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

install-headers:
	cp -r include/* $(SYSROOT)/include

format: $(SOURCES)
	indent $(SOURCES) -linux

check:
	cppcheck -I $(INCLUDE_DIR) $(SRCDIR)

clean:
	-rm -f $(OBJECTS) $(TARGET)

