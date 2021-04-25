#include "libk/stdlib.h"

#include <kmalloc.h>

void free(void * ptr)
{
    kfree(ptr);
}

void* malloc(unsigned int size)
{
    return kmalloc(size);
}
