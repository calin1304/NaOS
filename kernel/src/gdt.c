#include "gdt.h"

#include <libk/string.h>

enum GDT_ACCESS {
    GDT_ACCESSED        = 0x01,
    GDT_RW              = 0x02,
    GDT_DC              = 0x04,
    GDT_EXECUTABLE      = 0x08,
    GDT_PRIVILEGE_1     = 0x20,
    GDT_PRIVILEGE_2     = 0x40,
    GDT_PRIVILEGE_3     = 0x60,
    GDT_PRESENT         = 0x80
};

enum GDT_FLAGS {
    GDT_SIZE        = 0x4,
    GDT_GRANULARITY = 0x8
};

struct gdt_entry {
    uint16_t limit0;
    uint32_t base0 : 24;
    uint8_t accessed : 1;
    /* is read access for code segments allowed (write is never allowed)
       is write access for data segments allowed (read is always allowed)*/
    uint8_t rw : 1; 
    /* data selector: 0 => segment grows up, 1 => grows down (offset > limit)
       code selector: 1 => code can be executed from equal or lower privilege
            (ring 3 far jump to conforming code in ring 2)
    */
    uint8_t dc : 1;
    uint8_t executable : 1; // 0 => data selector
    uint8_t s : 1; // descriptor type, 1 for code or data, 0 for system
    uint8_t privilege : 2;
    uint8_t present : 1;
    uint8_t limit1 : 4;
    uint8_t zero : 2;
    uint8_t size : 1; // 0 for 16 bit pmode, 1 for 32 bit pmode
    uint8_t granularity : 1; // 0 => limit is 1 B blocks, 1 => limit 4 KiB blocks
    uint8_t base1;
} __attribute__((packed));

struct gdt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

typedef struct gdt_entry gdt_entry_t;
typedef struct gdt_ptr gdt_ptr_t;

gdt_entry_t gdt_entries[6];
gdt_ptr_t gdt_ptr;

static inline void gdt_load(struct gdt_ptr *ptr)
{
	__asm__ __volatile__("lgdt (%0)" : : "r"(ptr));
}

static void gdt_entry_init(struct gdt_entry *entry, 
    uint32_t limit, uint32_t base, uint8_t access, uint8_t flags)
{
    memset(entry, 0, sizeof(gdt_entry_t));
    entry->limit0 = (uint16_t)(limit & 0xffff);
    entry->limit1 = (limit & 0xf0000) >> 16;
    entry->base0 = (uint32_t)(base & 0xffffff);
    entry->base1 = (uint8_t)((base & 0xff000000) >> 24);
    // Set access bits
    entry->rw = (access & GDT_RW) >> 1;
    entry->dc = (access & GDT_DC) >> 2;
    entry->executable = (access & GDT_EXECUTABLE) >> 3;
    entry->privilege = (access & GDT_PRIVILEGE_3) >> 5;
    entry->s = 1;
    entry->present = 1;
    // Set flag bits
    entry->size = (flags & GDT_SIZE) >> 2;
    entry->granularity = (flags & GDT_GRANULARITY) >> 3;
}

static inline void load_tsr(uint16_t sel)
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
    
    memset(&gdt_entries[0], 0, sizeof(struct gdt_entry));

    gdt_entry_init(&gdt_entries[1], 0xffffffff, 0, GDT_EXECUTABLE | GDT_RW | GDT_DC, GDT_SIZE | GDT_GRANULARITY);
    gdt_entry_init(&gdt_entries[2], 0xffffffff, 0, GDT_RW, GDT_SIZE | GDT_GRANULARITY);
    
    gdt_entry_init(&gdt_entries[3], 0xffffffff, 0, GDT_EXECUTABLE | GDT_RW | GDT_PRIVILEGE_3 | GDT_DC, GDT_SIZE | GDT_GRANULARITY);
    gdt_entry_init(&gdt_entries[4], 0xffffffff, 0, GDT_RW | GDT_PRIVILEGE_3, GDT_SIZE | GDT_GRANULARITY);

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