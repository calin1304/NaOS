#ifndef IDT_H
#define IDT_H

#include <stdint.h>

struct IDTEntry {
    uint16_t    baseLo;
    uint16_t    selector;
    uint8_t     reserved; // = 0
    uint8_t     flags;
    uint16_t    baseHi;
} __attribute__((packed));

struct IDTPtr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

struct IDTEntry    idt[256];
struct IDTPtr      idtp;

void idt_init();
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);
void* idt_get_gate(uint8_t num);
void idt_install();


#endif