#include "clock.h"

void clock_init(Clock *this)
{
    this->ticks = 0;
    this->seconds = 0;
}

void sleep(uint32_t milis)
{
    uint32_t currentTime = clock.ticks;
    while (clock.ticks - currentTime < (milis/10));
}