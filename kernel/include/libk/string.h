#ifndef STRING_H
#define STRING_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void* memset(void *ptr, int value, size_t num);
void* memcpy(void *dst, void *src, size_t num);
size_t strlen(const char *s);
char* strcpy(char *dst, const char *src);
int strcmp(const char *str1, const char *str2);
int strncmp(const char *str1, const char *str2, size_t num);
char* strtok(char *s, const char *delims);
char* strchr(char *s, int c);
char* strstr(const char *s1, const char *s2);

#ifdef __cplusplus
}
#endif

#endif
