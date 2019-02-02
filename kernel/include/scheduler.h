#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include <process/process.h>

void init_scheduler();
void switch_process(uint32_t *isr_frame);
void scheduler_add(Process *p);
void scheduler_start();

#endif