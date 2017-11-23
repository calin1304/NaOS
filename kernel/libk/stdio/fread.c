#include "kernel/include/vfs.h"

#include "stdio.h"

size_t fread(void *ptr, size_t size, size_t count, FILE *f)
{
    return vfs_fread(ptr, size, count, f);
}