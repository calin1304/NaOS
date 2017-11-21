ENTRY(_start);
OUTPUT_FORMAT(elf32-i386);

SECTIONS {
    .text 0x100000 : {
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
