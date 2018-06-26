#include <stdint.h>

#include "defines.h"
#include "idt.h"
#include "gdt.h"
#include "console.h"
#include "io.h"
#include "pic.h"
#include "clock.h"
#include "utils.h"

#include "multiboot.h"

#include <stdio.h>
#include <stdlib.h>

void print_multiboot_info(multiboot_info_t *mbt)
{
    if (GET_BIT(mbt->flags, 0)) {
        printf("Lower memory size: %d KB\n", mbt->mem_lower);
        printf("Upper memory size: %d KB\n", mbt->mem_upper);
    }

    // Device the OS was booted from
    if (GET_BIT(mbt->flags, 1)) {
        printf("boot_device: %x\n", mbt->boot_device);
    }

    // Check for arguments to kernel
    if (GET_BIT(mbt->flags, 2)) {
        printf("Address of command line: %x\n", mbt->cmdline);
    }

    // Check for loaded modules
    if (GET_BIT(mbt->flags, 3)) {
        printf("Loaded modules: %d\n", mbt->mods_count);
    }

    // Check for a.out symbol table or ELF kernel header table
    if (GET_BIT(mbt->flags, 4)) {
        printf("Symbol table availalbe at %p\n", mbt->u.aout_sym.addr);
    } else if (GET_BIT(mbt->flags, 5)) {
        printf("ELF Kernel header table availalbe at %p\n", mbt->u.elf_sec.addr);
    }
    
    // Check for memory map
    if (GET_BIT(mbt->flags, 6)) {
        printf("Memory map available at %p\n", mbt->mmap_addr);
        printf("Memory map length: %d\n", mbt->mmap_length);
        multiboot_memory_map_t *mmap = (multiboot_memory_map_t*)mbt->mmap_addr;
        while (mmap < mbt->mmap_addr + mbt->mmap_length) {
            printf("Size: %d, Type: %d, Addr :%p, Length: %x\n", mmap->size, mmap->type, mmap->addr_low, mmap->len_low);
            mmap = (multiboot_memory_map_t*)((uint32_t)mmap + mmap->size + sizeof(mmap->size));
        }
    }
}

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
    print_multiboot_info(mbt);
    puts("\n[#] Kernel end");
    for(;;);
}
