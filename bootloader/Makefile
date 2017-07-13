SOURCES := bootloader.asm
TARGET := bootloader

$(TARGET): $(SOURCES)
	$(ASM) -o $(TARGET) -f bin $<

.PHONY: clean
clean:
	rm -f $(TARGET)