#include <stdlib.h>

#ifdef __is_libk
#include <kmalloc.h>
#endif

void* malloc(unsigned int size)
{
    #ifdef __is_libk
    return kmalloc(size);
    #else

    #endif
}