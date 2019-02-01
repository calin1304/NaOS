#ifndef DEBUG_H
#define DEBUG_H

#define BP __asm__ __volatile__("xchgw %bx, %bx");

#define ASM_DUMP_REG_32(reg, var) \
    __asm__ __volatile__("movl %%"reg", %0" : "=r"((var)));

#define DUMP_REG_32(reg){\
    uint32_t val;\
    ASM_DUMP_REG_32(reg, val);\
    printf(reg" = %x\n", val);\
}

void backtrace();
void dump_regs();

#endif