OUTPUT_FORMAT(binary);

SECTIONS {
    .text 0x10000 : {
        start.o (.text);
        entry.o (.text);
        * (.text)
    }
    .rodata BLOCK(4K) : ALIGN(4K) {
        * (.rodata)
    }
    .data BLOCK(4K) : ALIGN(4K) {
        * (.data)
    }
    .bss BLOCK(4K) : ALIGN(4K) {
        * (.bss)
    }
}
