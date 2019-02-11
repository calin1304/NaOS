#include <string.h>

int strncmp(const char *str1, const char *str2, size_t num)
{
    for (; num; --num, ++str1, ++str2) {
        if (*str1 != *str2) {
            return *(unsigned char*)str1 - *(unsigned char*)str2;
        }
    }
    return 0;
}