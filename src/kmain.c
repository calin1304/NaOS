#include <stdint.h>

#include "defines.h"
#include "idt.h"
#include "gdt.h"
#include "console.h"
#include "io.h"
#include "pic.h"
#include "clock.h"

extern Console console;
extern Clock clock;

void main(void) 
{
    // clock_init(&clock);    
    // console_init(&console);
    // console_printf(&console, "[#] Console initialized\n");
    
    // console_printf(&console, "[#] Initializing GDT\n");
    gdt_init();
    
    // console_printf(&console, "[#] Initializing PICs and remapping IRQs to %x-%x\n", 0x20, 0x28+7);
    init_pics(0x20, 0x28);
    outb(PIC1_DATA, 0xFC);
    
    // console_printf(&console, "[#] Initializing IDT\n");
    idt_init();

    // console_printf(&console, "[%x] Kernel is running\n", clock.ticks);


    int n = 200;
    int m = 320;
    uint8_t *vga = (uint8_t*)0xA0000;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            vga[i*m + j] = 1;
        }
    }

    for (int i = n/3; i < ((n/3) << 1); ++i) {
        for (int j = m/3; j < ((m/3) << 1); ++j) {
            vga[i*m + j] = 2;
        }
    }
    for(;;);
}
