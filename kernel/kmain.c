#include <stdint.h>

#include "defines.h"
#include "idt.h"
#include "gdt.h"
#include "console.h"
#include "io.h"
#include "pic.h"
#include "clock.h"
#include "malloc.h"
#include "fs/fat12/fat12.h"
#include "ata.h"


#include "libc/include/stdio.h"

extern Console console;
extern Clock clock;

void main(void) 
{
    clock_init(&clock);    
    console_init(&console);
    printf("[#] Console initialized\n");
    
    printf("[#] Initializing GDT\n");
    gdt_init();
    
    printf("[#] Initializing PICs and remapping IRQs to %x-%x\n", 0x20, 0x28+7);
    init_pics(0x20, 0x28);
    outb(PIC1_DATA, 0xFC);
    
    printf("[#] Initializing IDT\n");
    idt_init();

    printf("[%x] Kernel is running\n", clock.ticks);

    heap_initialize();
    printf("[#] Heap initialized\n");

    uint16_t count = 1193180 / 100;
    outb(0x43, 0x36);
    outb(0x40, count & 0xffff);
    outb(0x40, count >> 8);

    // struct FAT12RootEntry *root = (struct FAT12RootEntry *)0x1000;
    // uint8_t *fat = (uint8_t*)0x2500;
    struct FAT12RootEntry *root = (struct FAT12RootEntry *)malloc(sizeof(struct FAT12RootEntry)*16);
    uint8_t *fat = (uint8_t*)malloc(sizeof(uint8_t)*128);
    
    ata_read_lba(19, 1, (uint16_t*)root);
    printf("[#] First sector of Root directory loaded at %x\n", root);
    ata_read_lba(1, 1, (uint16_t*)fat);
    printf("[#] First sector of FAT loaded at %x\n", fat);

    struct FAT12RootEntry *f = fat12_find_file_root_entry(root, "WELCOME ");
    if (!f) {
        printf("Welcome message not found\n");
    } else {
        uint16_t *dst = 0x3000;
        fat12_load_file(fat, f, dst);
        uint8_t *s = (uint8_t*)dst;
        for (unsigned int i = 0; i < f->filesize; ++i) {
            printf("%c", s[i]);
        }
    }
    puts("\nKernel end\n");
    // int n = 200;
    // int m = 320;
    // uint8_t *vga = (uint8_t*)0xA0000;
    // for (int i = 0; i < n; ++i) {
    //     for (int j = 0; j < m; ++j) {
    //         vga[i*m + j] = 1;
    //     }
    // }

    // for (int i = n/3; i < ((n/3) << 1); ++i) {
    //     for (int j = m/3; j < ((m/3) << 1); ++j) {
    //         vga[i*m + j] = 2;
    //     }
    // }
    for(;;);
}
