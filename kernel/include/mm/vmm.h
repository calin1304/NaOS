#ifndef VMM_H
#define VMM_H

#include <stdint.h>
#include <stddef.h>

#include "mm/pmm.h"

#define PAGE_SIZE       4096

typedef uint32_t vaddr;

void vmm_init();
void vmm_map(paddr p, vaddr v);

#endif