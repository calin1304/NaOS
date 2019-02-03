#ifndef GDT_H
#define GDT_H

#include <stdint.h>

void gdt_init();
void install_tss(uint32_t kernelSS, uint32_t kernelESP);

#endif