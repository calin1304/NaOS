#include <stdarg.h>

#include "stdio.h"

#include "kernel/include/console.h"

const char *hexTable = "0123456789ABCDEF";

void vsprintf(char *s, const char *format, va_list args)
{
    const char *p;
    for (p = format; *p != '\0'; ++p) {
        if (*p == '%') {
            char t = *(++p);
            if (t == 'x' || t == 'p') {
                *(s++) = '0';
                *(s++) = 'x';
                uint32_t val = va_arg(args, uint32_t);
                for (int i = 0; i < 8; ++i, val <<= 4) {
                    uint8_t c = hexTable[(val & 0xf0000000) >> 28];
                    *(s++) = c;
                }
            } else if (t == 's') {
                const char *q = va_arg(args, const char *);
                while (*q) {
                    *(s++) = *(q++);
                }
            } else if (t == 'c') {
                *(s++) = va_arg(args, char);
            } else if ('t' == '%') {
                *(s++) = '%';
            }
        } 
        else {
            *(s++) = *(p);
        }
    }
    *s = '\0';
}

void sprintf(char *s, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vsprintf(s, format, args);
    va_end(args);
}

void vprintf(const char *format, va_list args)
{
    console_vprintf(&console, format, args);
}

void printf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}