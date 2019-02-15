#include <stdio.h>

void putchar(int c)
{
    #ifdef _is_libk
    console_put_char((char)c);
    #else
    
    #endif
}