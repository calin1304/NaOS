#ifndef LIBC_STDIO_H
#define LIBC_STDIO_H

#include <stddef.h>

#include "vfs.h"

void putchar(int c);
void puts(const char *s);
void printf(const char *format, ...);
void sprintf(char *s, const char *format, ...);
FILE *fopen(const char *filename, const char *mode);
size_t fread(void *ptr, size_t size, size_t count, FILE *f);

#endif