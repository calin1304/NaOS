#ifndef CLOCK_H
#define CLOCK_H

#include <stdint.h>

typedef struct Clock_ {
    uint32_t ticks;
} Clock;

Clock clock;

void clock_init();

#endif