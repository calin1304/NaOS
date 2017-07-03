#include "libc/include/string.h"

int strncmp(const char *str1, const char *str2, size_t num)
{
    while (*str1 == *str2 && *str1 && num > 0) {
        ++str1;
        ++str2;
        --num;
    }
    if (num > 0) {
        return *str1 < *str2 ? -1 : 1;
    }
    return 0;
}