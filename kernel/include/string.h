#ifndef STRING_H
#define STRING_H

#include <stdint.h>

#define MAX_BUFFER_SIZE 256

typedef struct String_ {
    char data[MAX_BUFFER_SIZE];
    unsigned int size;
} String;

String string_new();
void string_append_char(String *this, char c);
void string_append_asciiz(String *this, const char *s);
void string_append_string(String *this, const String *other);
void string_clear(String *this);

void memset(void *ptr, uint8_t val, uint32_t sz);
unsigned int strlen(const char *s);
void strcpy(char *dst, const char *src);
int strcmp(const char *a, const char *b);
int strncmp(const char *a, const char *b, unsigned int n);

#endif