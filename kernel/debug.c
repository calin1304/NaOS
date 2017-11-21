#include "debug.h"

#include <stdio.h>

void backtrace()
{
    int *ebp = 0;
    asm("movl %%ebp, %0" : : "m"(ebp));
    ebp = (int*)ebp[0]; // skip 2 isr frames
    ebp = (int*)ebp[0];
    int caller = ebp[1];
    while (caller != 0) {
        printf("-> %x\n", caller);
        ebp = (int*)ebp[0];
        caller = ebp[1];
    }
}