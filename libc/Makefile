CFLAGS := -ffreestanding -nostdlib -m32 -Wall -Wextra -I ..
AR := ar
BUILDDIR := .obj
SRCDIR := .
SOURCES := $(shell find $(SRCDIR) -type f -name '*.c')
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:%.c=%.o))
TARGET := libc.a

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(AR) rcs $@ $(OBJECTS)

$(BUILDDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) -c -o $@ $(CFLAGS) $<

.PHONY: clean
clean:
	rm -f $(OBJECTS)
	rm -f $(TARGET)