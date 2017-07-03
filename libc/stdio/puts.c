#include "libc/include/stdio.h"

#include "kernel/include/console.h"

void puts(const char *s)
{
    console_put_string(&console, s);
}