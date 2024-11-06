#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include <process/process.h>
#include <idt.h>

void init_scheduler();
void scheduler_add(Process *p);
void scheduler_start();

#endif
