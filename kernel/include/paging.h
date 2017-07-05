#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>

typedef uint32_t PageDirectoryEntry;
typedef uint32_t PageTableEntry;

typedef struct PageTable_ {
    PageTableEntry entries[1024];
} PageTable;

void idpaging(PageTableEntry *firstEntry, uint32_t from, int size);

#endif