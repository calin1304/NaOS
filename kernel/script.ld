ENTRY(_start);
OUTPUT_FORMAT(binary);
OUTPUT(kernel);

SECTIONS {
    .text 0x10000 : {
        .obj/stage2.o (.text);
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
