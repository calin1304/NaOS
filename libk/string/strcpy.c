#include "libk/include/string.h"

char* strcpy(char *dst, const char *src)
{
    while (*dst) {
        ++dst;
    }
    while (*src) {
        *dst = *src;
        ++dst;
        ++src;
    }
    *dst = '\0';
    return dst;
}