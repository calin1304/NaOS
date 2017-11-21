#include <stdint.h>

#include "defines.h"
#include "idt.h"
#include "gdt.h"
#include "console.h"
#include "io.h"
#include "pic.h"
#include "clock.h"
#include "fs/fat12/fat12.h"
#include "drivers/ata/ata.h"
#include "mm/memory.h"
#include "mm/pmm.h"
#include "mm/vmm.h"
#include "kernel/include/elf.h"

#include "process/process.h"

#include <stdio.h>
#include <stdlib.h>

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
    uint16_t currentRing;
    asm volatile("movw %%ss, %0" : "=r"(currentRing));
    printf("Welcome to userspace, current ring: %d\n", currentRing & 0x3);
    // asm("hlt");
}

typedef struct ProcessListNode_ {
    Process *process;
    struct ProcessListNode_ *next;
} ProcessListNode;

typedef struct ProcessList_ {
    ProcessListNode *head;
    int size;
} ProcessList;

void process_list_add(ProcessList *l, Process *proc)
{
    ProcessListNode *newNode = malloc(sizeof(ProcessListNode));
    newNode->process = proc;
    newNode->next = 0;
    if (!l->head) {
        l->head = newNode;
    } else {
        ProcessListNode *p = l->head;
        while (p->next) {
            p = p->next;
        }
        p->next = newNode;
    }
    l->size += 1;
}

int process_list_is_empty(ProcessList *l)
{
    return l->size == 0;
}

void process_list_pop_front(ProcessList *l)
{
    if (l->head) {
        // Call destructors, free memory
        l->head = l->head->next;
    }
    l->size -= 1;
}

ProcessList process_list_new()
{
    ProcessList ret;
    ret.head = 0;
    ret.size = 0;
    return ret;
}

void kmain(struct MemoryMapInfo* mminfo, uint16_t mmentries)
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

    ATADrivePorts ports;
    ports.dataPort = 0x1f0;
    ports.errorPort = 0x1f1;
    ports.sectorCountPort = 0x1f2;
    ports.lowLBAPort = 0x1f3;
    ports.midLBAPort = 0x1f4;
    ports.highLBAPort = 0x1f5;
    ports.headPort = 0x1f6;
    ports.commandPort = 0x1f7;
    ports.primaryControlRegister = 0x3F6;
    
    ATADrive drive;
    drive.ports = ports;

    fat12_init(0, &drive);

    FAT12FileSystem *fat12FS = getFAT12Driver();
    vfsInit();
    vfsMount("/", fat12FS);
    puts("\n[#] Kernel end");
    for(;;);
}
