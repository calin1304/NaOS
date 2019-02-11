ARCH := i686-elf
CC := $(ARCH)-gcc
LD := $(ARCH)-ld
AS := $(ARCH)-as

PREFIX=/usr

CFLAGS=-Wall -Wextra -std=c99 -Iinclude -I$(SYSROOT)$(PREFIX)/include

OBJS=\
string/memcpy.o \
string/memset.o \
string/strchr.o \
string/strcmp.o \
string/strcpy.o \
string/strlen.o \
string/strncmp.o \
string/strstr.o \
string/strtok.o \
ctype/isalpha.o \
ctype/tolower.o \
ctype/toupper.o \
stdio/printf.o \
stdio/putchar.o \
stdio/puts.o \
stdio/sprintf.o \
stdio/vprintf.o \
stdio/vsprintf.o \
stdio/fopen.o \
stdio/fread.o \
stdlib/malloc.o \
stdlib/free.o \

LIBK_CFLAGS := $(CFLAGS) -ffreestanding -mno-red-zone -D__is_libk
LIBK_OBJS=$(OBJS:.o=.libk.o)

BINARIES=libk.a

.PHONY: all clean install-headers #install install-headers install-libs

all: $(BINARIES)

libk.a: $(LIBK_OBJS)
	$(AR) rcs $@ $^

$(LIBK_OBJS): %.libk.o: %.c
	$(CC) -c $(LIBK_CFLAGS) $< -o $@

install-headers:
	cp -r include/* $(SYSROOT)$(PREFIX)/include

clean:
	rm -f $(BINARIES) *.a
	rm -f $(OBJS) $(LIBK_OBJS)

