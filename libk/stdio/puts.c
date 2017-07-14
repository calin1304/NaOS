#include "libk/include/stdio.h"

#include "kernel/include/console.h"

void puts(const char *s)
{
    printf("%s\n", s);;
}