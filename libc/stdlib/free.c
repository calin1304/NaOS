#include <stdlib.h>

#ifdef __is_libk
#include <kmalloc.h>
#endif

void free(void * ptr)
{
    #ifdef __is_libk
    kfree(ptr);
    #else
    #endif
}