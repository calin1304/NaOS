#include <stdio.h>

void *fopen(const char *filename, const char *mode)
{
    #ifdef __is_libk
    return vfsFOpen(filename, mode);
    #else

    #endif
}
