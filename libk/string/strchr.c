#include "libk/include/string.h"

char* strchr(char *s, int c)
{
    while (*s != c && *s != '\0') {
        s += 1;
    }
    if (*s == c) {
        return s;
    }
    return NULL;
}