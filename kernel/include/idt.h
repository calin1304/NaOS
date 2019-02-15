#ifndef IDT_H
#define IDT_H

#include <stdint.h>

void idt_init();

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