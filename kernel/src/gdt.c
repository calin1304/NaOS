#include "gdt.h"

#include <libk/string.h>

enum GDT_ACCESS {
    GDT_ACCESSED        = 0x01,
    GDT_RW              = 0x02,
    GDT_DC              = 0x04,
    GDT_EXECUTABLE      = 0x08,
    GDT_ALWAYS_1        = 0x10,
    GDT_PRIVILEGE_1     = 0x20,
    GDT_PRIVILEGE_2     = 0x40,
    GDT_PRIVILEGE_3     = 0x60,
    GDT_PRESENT         = 0x80
};

enum GDT_FLAGS {
    GDT_SIZE        = 0x4,
    GDT_GRANULARITY = 0x8
};

struct GDTEntry {
    uint16_t    limit;
    uint32_t    base    : 24;
    uint8_t     access;
    uint8_t     limit2  : 4;
    uint8_t     flags   : 4;
    uint8_t     base3;
} __attribute__((packed));

struct GDTPtr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

struct GDTEntry gdt_entries[6];
struct GDTPtr gdt_ptr;

static void gdt_load(struct GDTPtr *ptr)
{
	__asm__ __volatile__("lgdt (%0)" : : "r"(ptr));
}

void gdt_entry_init(struct GDTEntry *entry, 
    uint32_t limit, uint32_t base, uint8_t access, uint8_t flags)
{
    entry->limit    = (uint16_t)(limit & 0xffff);
    entry->limit2   = (limit & 0xf0000) >> 16;
    entry->base     = (uint32_t)(base & 0xffffff);
    entry->base3    = (uint8_t)((base & 0xff000000) >> 24);
    entry->access   = access;
    entry->flags    = flags;
}

void load_tsr(uint16_t sel)
{
    __asm__ __volatile__("ltr %0" : : "r"(sel));
}

struct tss_entry {
	uint32_t prevTss;
	uint32_t esp0;
	uint32_t ss0;
	uint32_t esp1;
	uint32_t ss1;
	uint32_t esp2;
	uint32_t ss2;
	uint32_t cr3;
	uint32_t eip;
	uint32_t eflags;
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;
	uint32_t es;
	uint32_t cs;
	uint32_t ss;
	uint32_t ds;
	uint32_t fs;
	uint32_t gs;
	uint32_t ldt;
	uint16_t trap;
	uint16_t iomap;
} __attribute__((packed));

struct tss_entry TSS;

void gdt_init()
{
    gdt_ptr.limit = sizeof(gdt_entries) - 1;
    gdt_ptr.base = (uint32_t)&gdt_entries;
    
    memset(&gdt_entries[0], 0, sizeof(struct GDTEntry));

    gdt_entry_init(&gdt_entries[1], 0xffffffff, 0x0, GDT_ALWAYS_1 | GDT_PRESENT | GDT_EXECUTABLE | GDT_RW | GDT_DC, GDT_SIZE | GDT_GRANULARITY);
    gdt_entry_init(&gdt_entries[2], 0xffffffff, 0x0, GDT_ALWAYS_1 | GDT_PRESENT | GDT_RW, GDT_SIZE | GDT_GRANULARITY);
    
    gdt_entry_init(&gdt_entries[3], 0xffffffff, 0x0, GDT_ALWAYS_1 | GDT_PRESENT | GDT_EXECUTABLE | GDT_RW | GDT_PRIVILEGE_3 | GDT_DC, GDT_SIZE | GDT_GRANULARITY);
    gdt_entry_init(&gdt_entries[4], 0xffffffff, 0x0, GDT_ALWAYS_1 | GDT_PRESENT | GDT_RW | GDT_PRIVILEGE_3, GDT_SIZE | GDT_GRANULARITY);

    gdt_load(&gdt_ptr);
}

void install_tss(uint16_t kernelSS, uint32_t kernelESP)
{
    uint32_t base = (uint32_t)&TSS;
    gdt_entry_init(&gdt_entries[5], base + sizeof(struct tss_entry), base, 
		GDT_PRESENT | GDT_EXECUTABLE | GDT_PRIVILEGE_3 | GDT_ACCESSED, 0);
    memset((void*)&TSS, 0, sizeof(struct tss_entry));
    
    TSS.ss0 = kernelSS;
    TSS.esp0 = kernelESP;   
    TSS.cs = 0x0b;
    TSS.ss = 0x13;
	TSS.es = 0x13;
	TSS.ds = 0x13;
	TSS.fs = 0x13;
	TSS.gs = 0x13;
    load_tsr(0x2b);
}