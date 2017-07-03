#include "gdt.h"

#include "string.h"

extern void gdt_load(struct GDTPtr *);

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

void gdt_init()
{
    gdt_ptr.limit = sizeof(gdt_entries) - 1;
    gdt_ptr.base = (uint32_t)&gdt_entries;
    
    memset(&gdt_entries[0], 0, sizeof(struct GDTEntry));

    gdt_entry_init(&gdt_entries[1], 0xffffffff, 0x0, 0x9a, 0xc);
    gdt_entry_init(&gdt_entries[2], 0xffffffff, 0x0, 0x92, 0xc);

    gdt_load(&gdt_ptr);
}