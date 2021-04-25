#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include <process/process.h>
#include <idt.h>

void init_scheduler();
void switch_process(uint32_t *task_eip, syscall_frame_t *frame);
void scheduler_add(Process *p);
void scheduler_start();

#endif
