#include "debug.h"

#include <stdio.h>

void backtrace()
{
    int *ebp = 0;
    __asm__ __volatile__("movl %%ebp, %0" : : "m"(ebp));
    ebp = (int*)ebp[0]; // skip 2 isr frames
    ebp = (int*)ebp[0];
    int caller = ebp[1];
    while (caller != 0) {
        printf("-> %x\n", caller);
        ebp = (int*)ebp[0];
        caller = ebp[1];
    }
}

void dump_regs()
{
    DUMP_REG_32("eax");
    DUMP_REG_32("ebx");
    DUMP_REG_32("ecx");
    DUMP_REG_32("edx");
    DUMP_REG_32("esi");
    DUMP_REG_32("edi");
    DUMP_REG_32("esp");
    DUMP_REG_32("ebp");
}