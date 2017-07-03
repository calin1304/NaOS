#ifndef LIBC_STRING_H
#define LIBC_STRING_H

#include <stddef.h>
#include <stdint.h>

void* memset(void *ptr, int value, size_t num);
unsigned int strlen(const char *s);
char* strcpy(char *dst, const char *src);
int strcmp(const char *str1, const char *str2);
int strncmp(const char *str1, const char *str2, size_t num);

#endif