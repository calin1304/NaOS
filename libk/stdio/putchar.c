#include "libk/include/stdio.h"

#include "kernel/include/console.h"

void putchar(int c)
{
    console_put_char(&console, (char)c);
}