#ifndef CLOCK_H
#define CLOCK_H

#include <stdint.h>

typedef struct clock_t {
    uint32_t ticks;
    uint32_t seconds;
} clock_t;

clock_t clock;

void clock_init(clock_t *);
void sleep(uint32_t milis);

#endif