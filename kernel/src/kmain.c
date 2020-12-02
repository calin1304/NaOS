#include <stdint.h>

#include "defines.h"
#include "idt.h"
#include "gdt.h"
#include "console.h"
#include "io.h"
#include "pic.h"
#include "clock.h"
#include "utils.h"
#include <scheduler.h>
#include <process/process.h>
#include "multiboot.h"
#include <drivers/ata/ata.h>
#include <fs/tar.h>

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

void t0()
{
    char *s = "t0";
    while (1) {
        for (int i = 0; i < 100000000; ++i);
        __asm__("mov $0, %eax");
        __asm__("mov %0, %%ebx" : : "m"(s) : "%ebx");
        __asm__("int $0x80");
    }
}

void t1()
{
    char *s = "t1";
    while (1) {
        for (int i = 0; i < 100000000; ++i);
        __asm__("mov $0, %eax");
        __asm__("mov %0, %%ebx" : : "m"(s) : "%ebx");
        __asm__("int $0x80");
    }
}

void enter_userspace()
{
    install_tss(0x10, malloc(PAGE_SIZE) + PAGE_SIZE);
    __asm__ __volatile__("cli");
    // Setup userspace data segments
    __asm__ __volatile__("mov $0x23, %ax\n\t"
                         "mov %ax, %ds\n\t"
                         "mov %ax, %es\n\t"
                         "mov %ax, %fs\n\t"
                         "mov %ax, %gs");
    // Setup userspace stack
    __asm__ __volatile__("mov %esp, %eax\n\t"
                         "push $0x23\n\t"
                         "push %eax");
    __asm__ __volatile__("pushf\n\t");
    __asm__ __volatile__("pop %eax\n\t"
                         "or $0x200, %eax\n\t" // Enable IF in EFLAGS
                         "push %eax"); 
    __asm__ __volatile__("push $0x1b"); // Push userspace code segment selector
    __asm__ __volatile__("push $1f\n\t"
                         "iret\n\t"
                         "1:");
}

#include <elf.h>

void* elf_load(char *p)
{
    // FILE *f = fopen(path, "r");

    // Process *process = malloc(sizeof(Process));
    // process->id = get_next_availablePID();
    // process->state = PROCESS_PAUSED;
    // process->pdir = pmm_alloc_block();
    // vmm_map_page(process->pdir, process->pdir);
    // memset(process->pdir, 0, sizeof(PDirectory));
    // vmm_identity_map(process->pdir, 0x0, 1024);
    
    // uint8_t *buffer = malloc(f->size);
    // fread(buffer, sizeof(uint8_t), f->size, f);
    Elf32Header *elf_header = p;
    Elf32ProgramHeader *elfProgramHeader = p + elf_header->e_phoff;
    Elf32SectionHeader *elfSectionHeader = p + elf_header->e_shoff;
    for (int i = 0; i < elf_header->e_shnum; ++i) {
        if (elfSectionHeader[i].sh_addr != 0) {
            uint8_t *sec = malloc(4096);
            vmm_map(sec, elfSectionHeader[i].sh_addr);
            // if (!pdir_vaddr_is_mapped(process->pdir, elfSectionHeader[i].sh_addr)) {
            //      sec = pmm_alloc_block();
            //     pdir_map_page(process->pdir, sec, (void*)elfSectionHeader[i].sh_addr);
            // } else {
            //     sec = (uint8_t*)pdir_get_paddr(process->pdir, elfSectionHeader[i].sh_addr);
            // }
            uint8_t *dat = p + elfSectionHeader[i].sh_offset;
            memcpy(elfSectionHeader[i].sh_addr, dat, elfSectionHeader[i].sh_size);
        }
    }
    int (*entry)() = elf_header->e_entry;
    return entry;
}

void *get_loaded_module(const char *s, multiboot_info_t *mbt)
{
    multiboot_module_t *modules = (multiboot_module_t *)mbt->mods_addr;
    for (int i = 0; i < mbt->mods_count; ++i) {
        if (!strcmp(modules[i].cmdline, s)) {
            // size_t mod_size = modules[i].mod_end - modules[i].mod_start;
            // ret = malloc(mod_size);
            // memcpy(ret, modules[i].mod_start, mod_size);
            return modules[i].mod_start;
        }
    }
    return 0;
}

void *find_modules_end(multiboot_info_t *mbt)
{
    multiboot_module_t *modules = (multiboot_module_t *)mbt->mods_addr;
    return modules[mbt->mods_count-1].mod_end;
}

void kmain(multiboot_info_t *mbt, unsigned int magic)
{
    // Make a NULL stack frame to signal backtrace to stop
    __asm__ __volatile__("movl $0, -4(%ebp)");

    uint16_t count = 1193180 / 100;
    outb(0x43, 0x36);
    outb(0x40, count & 0xffff);
    outb(0x40, count >> 8);
    clock_init(&clock);
    
    gdt_init();
    
    init_pics(0x20, 0x28);
    outb(PIC1_DATA, 0xFC); // Umask interrupts

    void *grub_mods_end = find_modules_end(mbt);

    idt_init();
    pmm_init(grub_mods_end, mbt);
    vmm_init();

    console_init();
    printf("kmain addr: %p\n", kmain);
    print_multiboot_info(mbt);
    for (;;);
    /* I think modules loaded by grub are not mapped in vspace so map them */
    tar_header_t *initrd = get_loaded_module("/boot/naos.initrd", mbt);
    char *init = tar_open(initrd, "initrd/init");
    LOG("init loaded at %p\n", init)
    // void (*entry)() = elf_load(init);
    // LOG("%p", entry);
    
    ATADevice atadev[4];
    ata_init_device(&atadev[0], ATA_PRIMARY_BUS_IO_BASE, 
                     ATA_PRIMARY_BUS_CONTROL_BASE, 0);
    ata_init_device(&atadev[1], ATA_PRIMARY_BUS_IO_BASE, 
                     ATA_PRIMARY_BUS_CONTROL_BASE, 1);
    ata_init_device(&atadev[2], ATA_SECONDARY_BUS_IO_BASE, 
                     ATA_SECONDARY_BUS_CONTROL_BASE, 0);
    ata_init_device(&atadev[3], ATA_SECONDARY_BUS_IO_BASE, 
                     ATA_SECONDARY_BUS_CONTROL_BASE, 0);
    Process p0 = create_process(t0);
    Process p1 = create_process(t1);
    scheduler_add(&p0);
    scheduler_add(&p1);
    enter_userspace();
    scheduler_start();
    for(;;);
}
