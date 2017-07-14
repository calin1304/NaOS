#ifndef GDT_H
#define GDT_H

#include <stdint.h>

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
struct GDTPtr   gdt_ptr;

void gdt_init();
void install_tss(uint16_t kernelSS, uint32_t kernelESP);

#endif