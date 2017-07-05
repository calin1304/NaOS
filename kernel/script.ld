ENTRY(_start);
OUTPUT_FORMAT(binary);
OUTPUT(kernel);

SECTIONS {
    .text 0x7e00 : {
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
