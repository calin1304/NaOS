#include "io.h"

uint8_t inb(uint16_t port)
{
    uint8_t ret;
    asm volatile("inb %%dx, %%al" : "=a" (ret): "d" (port));
    return ret;
}

uint16_t inw(uint16_t port)
{
    uint16_t ret;
    asm volatile("inw %%dx, %%ax" : "=a" (ret): "d" (port));
    return ret;
}

void outb(uint16_t port, uint8_t value)
{
   asm volatile ("outb %%al,%%dx": :"d" (port), "a" (value));
}