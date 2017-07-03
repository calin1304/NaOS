#ifndef CLOCK_H
#define CLOCK_H

#include <stdint.h>

typedef struct Clock_ {
    uint32_t ticks;
    uint32_t seconds;
} Clock;

Clock clock;

void clock_init();
void sleep(uint32_t milis);

#endif