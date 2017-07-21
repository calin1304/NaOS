#ifndef PMM_H
#define PMM_H

#include <stdint.h>

#include "kernel/include/mm/memory.h"

typedef uint32_t paddr;

void pmm_init(struct MemoryMapInfo*, uint16_t entries, paddr bitmap);
void* pmm_alloc_block();
void pmm_free_block(void*);

void pmm_load_pdbr(void *pdAddr);
void pmm_enable_paging();
void pmm_disable_paging();

#endif