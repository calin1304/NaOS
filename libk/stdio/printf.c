#include <stdarg.h>

#include "libk/include/stdio.h"

#include "kernel/include/console.h"

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