#include "string.h"

int strncmp(const char *str1, const char *str2, size_t num)
{
    while (num--) {
        if (*str1++ != *str2++) {
            return *(unsigned char*)(str1 - 1) - *(unsigned char*)(str2 - 1);
        }
    }
    return 0;
}