#include <libk/string.h>

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

void* memset(void *ptr, int value, size_t num)
{
    uint8_t *v = (uint8_t*)ptr;
    for (unsigned int  i = 0; i < num; ++i, ++v) {
        *v = value;
    }
    return ptr;
}

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

int strcmp(const char *str1, const char *str2)
{
    while (*str1 == *str2 && *str1) {
        ++str1;
        ++str2;
    }
    if (*str1 || *str2) {
        return *str1 < *str2 ? -1 : 1;
    }
    return 0;
}

char* strcpy(char *dst, const char *src)
{
    while (*src) {
        *dst = *src;
        ++dst;
        ++src;
    }
    *dst = '\0';
    return dst;
}

size_t strlen(const char *s)
{
    const char *p = s;
    while (*p) { ++p; }
    return (unsigned int)(p - s);
}

int strncmp(const char *str1, const char *str2, size_t num)
{
    while (num--) {
        if (*str1++ != *str2++) {
            return *(unsigned char*)(str1 - 1) - *(unsigned char*)(str2 - 1);
        }
    }
    return 0;
}

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

char* strtok(char *s, const char *delims)
{
    static char* currPos;
    static char* stringEnd;
    if (s != NULL) {
        currPos = s;
        stringEnd = s;
        while (*stringEnd != '\0') {
            if (strchr(delims, *stringEnd)) {
                *stringEnd = '\0';
            }
            stringEnd += 1;
        }
    }
    while (*currPos != '\0') {
        currPos += 1;
    }
    while (*currPos == '\0' && currPos < stringEnd) {
        currPos += 1;
    }
    if (currPos == stringEnd){
        return NULL;
    }
    return currPos;
}