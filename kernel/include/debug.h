#ifndef DEBUG_H
#define DEBUG_H

#define BP asm("xchgw %bx, %bx");

void backtrace();

#endif