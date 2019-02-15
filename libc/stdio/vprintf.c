#include <stdio.h>

#include <stdarg.h>

#ifdef __is_libk
#include <console.h>
#endif

void vprintf(const char *format, va_list args)
{
    #ifdef __is_libk
    console_vprintf(format, args);
    #else

    #endif
}