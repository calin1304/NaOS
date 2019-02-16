PREFIX=/usr
CFLAGS_INCLUDES := -Iinclude -I$(SYSROOT)/include -I$(SYSROOT)/usr/include
CFLAGS := $(CFLAGS) $(CFLAGS_INCLUDES) -ffreestanding -D__is_libk
SRCDIR := src
SOURCES := $(shell find $(SRCDIR) -type f -name '*.c')
OBJECTS := $(SOURCES:%.c=%.o)
LIBS := -lgcc -lk
LIBDIR=../libc/
TARGET := ker.bin
LDFLAGS := -nostdlib -L$(LIBDIR)

.PHONY: all clean install-headers
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) -o $@ -T linker.ld $(LDFLAGS) $^ $(LIBS)

install-headers:
	cp -r include/* $(SYSROOT)/include

clean:
	-rm -f $(OBJECTS) $(TARGET)
