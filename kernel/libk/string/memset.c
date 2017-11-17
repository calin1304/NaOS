#include "string.h"

void* memset(void *ptr, int value, size_t num)
{
    uint8_t *v = (uint8_t*)ptr;
    for (unsigned int  i = 0; i < num; ++i, ++v) {
        *v = value;
    }
    return ptr;
}