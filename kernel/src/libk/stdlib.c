#include "libk/stdlib.h"

#include <kmalloc.h>
#include <utils.h>

void free(void * ptr)
{
    kfree(ptr);
}

void* malloc(unsigned int size)
{
    return kmalloc(size);
}

void __assert(const char *msg, const char *file, unsigned int line)
{
    DEBUG("Assert failed: %s in %s:%d\n", msg, file, line);
    HALT;
}
