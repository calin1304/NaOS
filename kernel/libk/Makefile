CFLAGS := $(CFLAGS) -I ../include
AR := $(ARCH)-ar
BUILDDIR := .obj
SRCDIR := src
SOURCES := $(shell find $(SRCDIR) -type f -name '*.c')
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:%.c=%.o))
TARGET := libk.a

$(TARGET): $(OBJECTS)
	$(AR) rcs $@ $(OBJECTS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c $(BUILDDIR)
	$(CC) -c -o $@ $(CFLAGS) $<

$(BUILDDIR):
	@mkdir $@

.PHONY: clean
clean:
	rm -f $(OBJECTS)
	rm -f $(TARGET)
