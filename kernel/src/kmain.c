#include <stdint.h>

#include "defines.h"
#include "idt.h"
#include "gdt.h"
#include "console.h"
#include "io.h"
#include "pic.h"
#include "clock.h"
#include "malloc.h"
#include "fat12.h"
#include "ata.h"

extern Console console;
extern Clock clock;

void main(void) 
{
    clock_init(&clock);    
    console_init(&console);
    console_printf(&console, "[#] Console initialized\n");
    
    console_printf(&console, "[#] Initializing GDT\n");
    gdt_init();
    
    console_printf(&console, "[#] Initializing PICs and remapping IRQs to %x-%x\n", 0x20, 0x28+7);
    init_pics(0x20, 0x28);
    outb(PIC1_DATA, 0xFC);
    
    console_printf(&console, "[#] Initializing IDT\n");
    idt_init();

    console_printf(&console, "[%x] Kernel is running\n", clock.ticks);

    heap_initialize();
    console_printf(&console, "[#] Heap initialized\n");

    uint16_t count = 1193180 / 100;
    outb(0x43, 0x36);
    outb(0x40, count & 0xffff);
    outb(0x40, count >> 8);

    struct FAT12RootEntry *root = (struct FAT12RootEntry *)0x1000;
    uint8_t *fat = (uint8_t*)0x2500;
    
    ata_read_lba(19, 1, (uint16_t*)root);
    console_printf(&console, "[#] First sector of Root directory loaded at %x\n", root);
    ata_read_lba(1, 1, (uint16_t*)fat);
    console_printf(&console, "[#] First sector of FAT loaded at %x\n", fat);

    struct FAT12RootEntry *f = fat12_find_file_root_entry("WELCOME ");
    if (!f) {
        console_printf(&console, "Welcome message not found\n");
    } else {
        uint16_t *dst = 0x3000;
        fat12_load_file(f, dst);
        uint8_t *s = (uint8_t*)dst;
        for (int i = 0; i < f->filesize; ++i) {
            console_printf(&console, "%c", s[i]);
        }
    }
    console_printf(&console, "\nKernel end\n");
    
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
