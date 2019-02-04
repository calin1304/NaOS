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

struct syscall_frame {
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
} __attribute__((packed));

typedef struct syscall_frame syscall_frame_t;

#endif