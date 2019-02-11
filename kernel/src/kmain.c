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
#include <fs/tar.h>

#include <libk/stdio.h>
#include <libk/stdlib.h>

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

void* find_module(const char *filename, multiboot_info_t *mbt)
{
    void *ret = NULL;
    multiboot_module_t *modules = (multiboot_module_t *)mbt->mods_addr;
    for (int i = 0; i < mbt->mods_count; ++i) {
        if (!strcmp(modules[i].cmdline, filename)) {
            LOG("Found module at %p", modules[i].mod_start);
            size_t mod_size = modules[i].mod_end - modules[i].mod_start;
            LOG("Module size %d bytes", mod_size);
            ret = malloc(mod_size);
            LOG("Moving module to %p", ret);
            memcpy(ret, modules[i].mod_start, mod_size);
            LOG("Moved module to %p", ret);
            // ret = modules[i].mod_start;
            break;
        }
    }
    return ret;
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
    
    idt_init();
    pmm_init(mbt);
    vmm_init();

    console_init(&console);
    print_multiboot_info(mbt);
    /* I think modules loaded by grub are not mapped in vspace so map them */
    tar_header_t *initrd = find_module("/boot/naos.initrd", mbt);
    char *init = tar_open(initrd, "initrd/init");
    LOG("%s", init);

    Process p0 = create_process(t0);
    Process p1 = create_process(t1);
    scheduler_add(&p0);
    scheduler_add(&p1);
    enter_userspace();
    scheduler_start();
    for(;;);
}
