#include "libk/include/string.h"

char* strstr(const char *s1, const char *s2)
{
    while (*s1 != '\0') {
        char *p = s1;
        char *q = s2;
        while (*p == *q && *p != '\0' && *q != '\0') {
            p += 1;
            q += 1;
        }
        if (*p == *q && *p == '\0') {
            return s1;
        }
        s1 += 1;
    }
    return NULL;
}