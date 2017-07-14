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
#include "kernel/include/memory.h"
#include "kernel/include/pmm.h"
#include "kernel/include/vmm.h"
#include "kernel/include/elf.h"

#include "libk/include/stdio.h"

extern Console console;
extern Clock clock;

void enter_userspace()
{
    __asm__(
        "cli\n"
        "movw $0x23, %ax\n"
        "movw %ax, %ds\n"
        "movw %ax, %es\n"
        "movw %ax, %fs\n"
        "movw %ax, %gs\n"
        "pushl $0x23\n"
        "push %esp\n"
        "pushfl\n"
        "pushl $0x1b\n"
        "lea (a), %eax\n"
        "pushl %eax\n"
        "iretl\n"
        "a: addl $4, %esp\n"
    );
    // const char msg[] = "hello, world from user space";
    // asm("movl $0, %eax");
    // asm("movl %0, %%ebx" : : "m"(msg) : "%ebx");
    // asm("int $0x80");
    printf("welcome to userspace\n");
    // asm("hlt");
}

void kmain(struct MemoryMapInfo* mminfo, uint16_t mmentries)
{
    uint16_t count = 1193180 / 100;
    outb(0x43, 0x36);
    outb(0x40, count & 0xffff);
    outb(0x40, count >> 8);
    clock_init(&clock);
    
    gdt_init();
    
    init_pics(0x20, 0x28);
    outb(PIC1_DATA, 0xFC); // Umask interrupts
    
    idt_init();

    console_init(&console);

    pmm_init(mminfo, mmentries, 0x20000);
    vmm_init();
    
    for (uint16_t i = 0; i < mmentries; ++i) {
        printf("Start address: %x -> ", mminfo[i].base);
        printf("length: %x ", mminfo[i].length);
        if (mminfo[i].type == 0x1) {
            printf("available memory\n");
        } else if (mminfo[i].type == 0x2) {
            printf("reserved memory\n");
        } else if (mminfo[i].type == 0x3) {
            printf("ACPI reclaim memory\n");
        } else if (mminfo[i].type == 0x4) {
            printf("ACPI NVS memory\n");
        }
    }

    struct FAT12RootEntry *root = (struct FAT12RootEntry *)pmm_alloc_block();
    if (!root) {
        printf("Error on block physical allocation\n");
    }
    uint8_t *fat = (uint8_t*)pmm_alloc_block();
    if (!fat) {
        printf("Error on block physical allocation\n");
    }

    ata_read_lba(19, 1, (uint16_t*)root);
    ata_read_lba(1, 1, (uint16_t*)fat);

    struct FAT12RootEntry *f = fat12_find_file_root_entry(root, "APP     ");
    printf("Filesize: %d bytes\n", f->filesize);
    uint8_t *buffer = (uint8_t*)pmm_alloc_block();
    if (!buffer) {
        printf("Error on block physical allocation\n");
    }
    fat12_load_file(fat, f, buffer);

    Elf32Header *elfHeader = buffer;
    Elf32ProgramHeader *elfProgramHeader = buffer + elfHeader->e_phoff;
    Elf32SectionHeader *elfSectionHeader = buffer + elfHeader->e_shoff;
    int (*target)();
    for (int i = 0; i < elfHeader->e_shnum; ++i) {
        if (elfSectionHeader[i].sh_addr != 0) {
            uint8_t *sec;
            if (!vmm_vaddr_is_mapped(elfSectionHeader[i].sh_addr)) {
                sec = (uint8_t*)pmm_alloc_block();
                vmm_map_page(sec, elfSectionHeader[i].sh_addr);
            }
            uint8_t *dat = buffer + elfSectionHeader[i].sh_offset;
            memcpy(elfSectionHeader[i].sh_addr, dat, elfSectionHeader[i].sh_size);
        }
    }
    target = elfHeader->e_entry;
    printf("%d\n", target());
    for (int i = 0; i < elfHeader->e_shnum; ++i) {
        if (elfSectionHeader[i].sh_addr != 0) {
            vmm_free_vaddr_page(elfSectionHeader[i].sh_addr);
        }
    }
    
    struct FAT12RootEntry *msg = fat12_find_file_root_entry(root, "WELCOME ");
    if (!msg) {
        printf("Welcome message not found\n");
    } else {
        uint16_t *dst = (uint16_t*)pmm_alloc_block();
        vmm_map_page(dst, dst);
        fat12_load_file(fat, msg, dst);
        uint8_t *s = (uint8_t*)dst;
        for (unsigned int i = 0; i < msg->filesize; ++i) {
            printf("%c", s[i]);
        }
        vmm_free_vaddr_page(dst);
    }
    puts("\n[#] Kernel end");
    install_tss(0x10, 0);
    // enter_userspace();
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
