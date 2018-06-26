CFLAGS := $(CFLAGS) -I ../include
AR := $(ARCH)-ar
BUILDDIR := .obj
SRCDIR := .
SOURCES := $(shell find $(SRCDIR) -type f -name '*.c')
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:%.c=%.o))
TARGET := libk.a

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(AR) rcs $@ $(OBJECTS)

$(BUILDDIR)/%.o: %.c $(BUILDDIR)
	$(CC) -c -o $@ $(CFLAGS) $<

$(BUILDDIR):
	@mkdir $@

.PHONY: clean
clean:
	rm -f $(OBJECTS)
	rm -f $(TARGET)
