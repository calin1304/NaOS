#include "string.h"

String string_new()
{
    String ret;
    ret.data[0] = '\0';
    ret.size = 0;
    return ret;
}

void string_append_char(String *this, char c)
{
    this->data[this->size++] = c;
    this->data[this->size] = '\0';
}

void string_append_asciiz(String *this, const char *s)
{
    strcpy(this->data, s);
    this->size += strlen(s);
}

void string_append_string(String *this, const String *other)
{
    string_append_asciiz(this, other->data);
}

void string_clear(String *this)
{
    this->data[0] = '\0';
    this->size = 0;
}

void strcpy(char *dst, const char *src)
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
}

void memset(void *ptr, uint8_t val, uint32_t sz)
{
    uint8_t *v = (uint8_t*)ptr;
    for (uint32_t i = 0; i < sz; ++i) {
        v[i] = val;
    }
}

unsigned int strlen(const char *s)
{
    const char *p = s;
    while (*p) { ++p; }
    return (unsigned int)(p - s);
}

int strcmp(const char *a, const char *b)
{
    while (*a == *b && *a) {
        ++a;
        ++b;
    }
    if (*a || *b) {
        return *a < *b ? -1 : 1;
    }
    return 0;
}

int strncmp(const char *a, const char *b, unsigned int n)
{
    while (*a == *b && *a && n > 0) {
        ++a;
        ++b;
        --n;
    }
    if (n > 0) {
        return *a < *b ? -1 : 1;
    }
    return 0;
}