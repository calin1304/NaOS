#include "kernel/include/paging.h"


void idpaging(PageTableEntry *firstEntry, uint32_t from, int size)
{
    from = from & 0xfffff000;
    for(; size > 0; from += 4096, size -= 4096, firstEntry++){
       *firstEntry = from | 1;     // mark page present.
    }
}