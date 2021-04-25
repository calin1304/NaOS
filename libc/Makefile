PREFIX=/usr

CFLAGS=-Wall \
	   -Wextra \
	   -std=c99 \
	   -Iinclude \
	   -I$(SYSROOT)/include \
	   -I$(SYSROOT)/usr/include \
	   -I../kernel/include

SOURCES := $(shell find . -type f -name '*.c')
OBJECTS := $(SOURCES:%.c=%.o)

LIBK_CFLAGS := $(CFLAGS) -ffreestanding -mno-red-zone -D__is_libk
LIBK_OBJS := $(OBJECTS:%.o=%.libk.o)

BINARIES=libk.a

.PHONY: all clean install-headers #install install-headers install-libs

all: $(BINARIES)

libk.a: $(LIBK_OBJS)
	$(AR) rcs $@ $^

$(LIBK_OBJS): %.libk.o: %.c
	$(CC) -c $(LIBK_CFLAGS) $< -o $@

install-headers:
	cp -r include/* $(SYSROOT)/include

clean:
	-rm -f $(BINARIES) *.a
	-rm -f $(OBJECTS) $(LIBK_OBJS)

