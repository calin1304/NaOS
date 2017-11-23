#include "malloc.h"

void free(void * ptr)
{
    HeapNode *hn = (HeapNode*)(((int*)ptr - 2));
    hn->allocated = 0;
}