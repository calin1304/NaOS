#ifndef PIC_H
#define PIC_H

#include <stdint.h>

#define PIC1 0x20
#define PIC2 0xA0

#define PIC1_DATA PIC1 + 1
#define PIC2_DATA PIC2 + 1

#define ICW1 0x11
#define ICW4 0x01

#define PIC_ACK 0x20


void init_pics(int pic1, int pic2);
void pic_ack(uint8_t interrupt);
uint8_t pic_get_isr(uint8_t pic);

#endif