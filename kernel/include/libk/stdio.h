#ifndef LIBC_STDIO_H
#define LIBC_STDIO_H

#include <stddef.h>

#ifdef __is_libk
#include <vfs.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* typedef struct { int unused; } FILE; */

void putchar(int c);
void puts(const char *s);
void printf(const char *format, ...);
void sprintf(char *s, const char *format, ...);
void *fopen(const char *filename, const char *mode);
size_t fread(void *ptr, size_t size, size_t count, void *f);

#ifdef __cplusplus
}
#endif

#endif
