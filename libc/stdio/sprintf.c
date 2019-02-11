#include <stdio.h>

#include <stdarg.h>

void sprintf(char *s, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vsprintf(s, format, args);
    va_end(args);
}