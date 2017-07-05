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
#include "kernel/include/paging.h"


#include "libc/include/stdio.h"

extern Console console;
extern Clock clock;

extern uint32_t enablePaging(uint32_t pageDirectoryAddress);

#define PAGE_TABLES_COUNT 1

PageDirectoryEntry pageDirectory[1024] __attribute__((aligned(4096)));
PageTable pageTables[PAGE_TABLES_COUNT] __attribute__((aligned(4096)));

void initPaging()
{
    for (int i = 0; i < 1024; ++i) {
        pageDirectory[i] = 0x2;
    }
    for (int j = 0; j < PAGE_TABLES_COUNT; ++j) {
        for (int i = 0; i < 1024; ++i) {
            pageTables[j].entries[i] = ((i + 1024*j) * 0x1000) | 3;
        }
    }
    for (int i = 0 ; i < PAGE_TABLES_COUNT; ++i) {
        pageDirectory[i] = ((unsigned int)&(pageTables[i])) | 3;
    }
    
    enablePaging((uint32_t)pageDirectory);
}

void main(void) 
{
    initPaging();

    uint16_t count = 1193180 / 100;
    outb(0x43, 0x36);
    outb(0x40, count & 0xffff);
    outb(0x40, count >> 8);
    clock_init(&clock);
    
    gdt_init();
    
    init_pics(0x20, 0x28);
    outb(PIC1_DATA, 0xFC); // Umask interrupts
    
    idt_init();
    
    heap_initialize();
    console_init(&console);
    
    struct FAT12RootEntry *root = (struct FAT12RootEntry *)malloc(sizeof(struct FAT12RootEntry)*16);
    uint8_t *fat = (uint8_t*)malloc(sizeof(uint8_t)*128);
    
    ata_read_lba(19, 1, (uint16_t*)root);
    ata_read_lba(1, 1, (uint16_t*)fat);

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
    puts("\n[#] Kernel end");
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
