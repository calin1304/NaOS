#include "pic.h"

#include "io.h"

/* init_pics()
 * init the PICs and remap them
 */
void init_pics(int pic1, int pic2)
{
	/* send ICW1 */
	outb(PIC1, ICW1);
	outb(PIC2, ICW1);

	/* send ICW2 */
	outb(PIC1 + 1, pic1);	/* remap */
	outb(PIC2 + 1, pic2);	/*  pics */

	/* send ICW3 */
	outb(PIC1 + 1, 4);	/* IRQ2 -> connection to slave */
	outb(PIC2 + 1, 2);

	/* send ICW4 */
	outb(PIC1 + 1, ICW4);
	outb(PIC2 + 1, ICW4);

	/* disable all IRQs */
	outb(PIC1 + 1, 0xFF);
}

void pic_ack(uint8_t interrupt)
{
  if (interrupt < 0x20 || interrupt > 0x28 + 7) {
     return;
  }
  if (interrupt < 0x28) {
    outb(PIC1, PIC_ACK);
  } else {
    outb(PIC2, PIC_ACK);
  }
}

uint8_t pic_get_isr(uint8_t pic)
{
  outb(pic, 0x3);
  return inb(pic);
}