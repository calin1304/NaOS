#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>

void init_scheduler();
void switch_process(uint32_t *isr_frame);

#endif