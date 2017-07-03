#include "libc/include/string.h"

size_t strlen(const char *s)
{
    const char *p = s;
    while (*p) { ++p; }
    return (unsigned int)(p - s);
}