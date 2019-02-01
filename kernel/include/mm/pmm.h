#ifndef PMM_H
#define PMM_H

#include <stdint.h>

#include "multiboot.h"

typedef uint32_t paddr;

void pmm_init(multiboot_info_t *mbt);
void* pmm_alloc_block();
void pmm_free_block(void*);

void pmm_load_pdbr(void *pdAddr);
void pmm_enable_paging();
void pmm_disable_paging();

#endif