#include <stdint.h>

#include "defines.h"
#include "idt.h"
#include "gdt.h"
#include "console.h"
#include "io.h"
#include "pic.h"
#include "clock.h"

#include "multiboot.h"

#include <stdio.h>
#include <stdlib.h>


void kmain(multiboot_info_t *mbt, unsigned int magic)
{
    // Make a NULL stack frame to signal backtrace to stop
    asm volatile("movl $0, -4(%ebp)");

    uint16_t count = 1193180 / 100;
    outb(0x43, 0x36);
    outb(0x40, count & 0xffff);
    outb(0x40, count >> 8);
    clock_init(&clock);
    
    gdt_init();
    
    init_pics(0x20, 0x28);
    outb(PIC1_DATA, 0xFC); // Umask interrupts
    
    idt_init();
    pmm_init(mbt);
    vmm_init();

    console_init(&console);
    puts("\n[#] Kernel end");
    for(;;);
}
