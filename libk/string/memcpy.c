#include "libk/include/string.h"

void* memcpy(void *dst, void *src, size_t num)
{
    uint8_t *dst_ = (uint8_t*)dst;
    uint8_t *src_ = (uint8_t*)src;
    while (num > 0) {
        *dst_ = *src_;
        ++dst_;
        ++src_;
        num -= 1;
    }
    return dst;
}