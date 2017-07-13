#ifndef GDT_H
#define GDT_H

#include <stdint.h>

enum GDT_ACCESS {
    GDT_ACCESSED    = 0x01,
    GDT_RW          = 0x02,
    GDT_DC          = 0x04,
    GDT_EXECUTABLE  = 0x08,
    GDT_PRIVILETE   = 0x60,
    GDT_PRESENT     = 0x80
};

enum GDT_FLAGS {
    GDT_SIZE        = 0x40,
    GDT_GRANULARITY = 0x80
};

struct GDTEntry {
    uint16_t    limit;
    // uint16_t    base;
    // uint8_t     base2;
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

struct GDTEntry gdt_entries[3];
struct GDTPtr   gdt_ptr;

void gdt_init();

#endif