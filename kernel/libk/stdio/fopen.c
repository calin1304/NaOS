#include "stdio.h"

#include "kernel/include/vfs.h"

FILE *fopen(const char *filename, const char *mode)
{
    return vfsFOpen(filename, mode);
}