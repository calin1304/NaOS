ENTRY(_start);
OUTPUT_FORMAT(binary);
OUTPUT(build/stage2.bin);

SECTIONS {
    .text 0x7e00 : {
        .obj/stage2.o (.text);
        * (.text)
    }
    .data : {
        * (.data)
    }
    .rodata : {
        * (.rodata)
    }
    .bss : {
        * (.bss)
    }
}
