SOURCES := bootloader.asm
TARGET := bootloader

$(TARGET):
	$(ASM) -o $(TARGET) -f bin bootloader.asm

.PHONY: clean
clean:
	rm -f $(TARGET)