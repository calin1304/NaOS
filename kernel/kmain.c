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

#include "libk/include/stdio.h"
#include "libk/include/stdlib.h"

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

// extern void __isr_timer();

// ProcessListNode *currRunning;

// void scheduler_interrupt()
// {
//     if (currRunning) {
//         Process *proc = currRunning->process;
//         proc->ticks += 1;
//         if (proc->ticks == 5) {

//         }
//     }
//     asm volatile("leave");
//     asm volatile(
//         "jmp %0"
//         :
//         : "r"(__isr_timer)
//     );
// }

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
    // FILE *f = (FILE*)fopen("a:/welcome", "");
    // if (!f) {
    //     printf("Could not open file\n");
    // }
    // printf("%s %d bytes\n", f->name, f->size);
    // uint8_t *buffer = malloc(f->size);
    // int n = fread(buffer, sizeof(uint8_t), f->size, f);
    // printf("%d bytes read\n", n);
    // for (int i = 0; i < f->size; ++i) {
    //     printf("%c", buffer[i]);
    // }
    Process *init = createProcess("a:/init");
    Process *app = createProcess("a:/app");
    ProcessList pl = process_list_new();
    process_list_add(&pl, init);
    process_list_add(&pl, app);

    // void *timer_isr = idt_get_gate(0x20);
    // printf("%p", timer_isr);
    // asm volatile("cli");
    // idt_set_gate(0x20, (uint32_t)scheduler_interrupt, 0x8, 0x8e);
    // asm volatile("sti");

    while (!process_list_is_empty(&pl)) {
        Process *proc = pl.head->process;
        int (*entry)() = proc->threads[0].entry;        
        vmm_switch_pdirectory(proc->pdir);
        asm volatile(
            "lea .after_iret, %%eax\n"
            "pushl %%eax\n"
            "pushl %0\n"
            "pushl %1\n"
            "retf\n"
            ".after_iret:"
            :
            : "i"(0x8), "m"(entry)
        );
        // entry();
        vmm_restore_pdirectory();
        process_list_pop_front(&pl);
    }
    puts("\n[#] Kernel end");
    // uint32_t esp;
    // asm volatile("movl %%esp, %0" : "=r"(esp));
    // install_tss(0x10, esp);
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
