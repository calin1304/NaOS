#include <stdio.h>

size_t fread(void *ptr, size_t size, size_t count, FILE *f)
{
    #ifdef __is_libk
    return vfs_fread(ptr, size, count, f);
    #else
    
    #endif
}