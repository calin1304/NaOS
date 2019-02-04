#ifndef VMM_H
#define VMM_H

#include <stdint.h>
#include <stddef.h>

#include "mm/pmm.h"

#define PAGE_SIZE       4096

typedef uint32_t vaddr;
typedef struct page_directory page_directory_t;

void vmm_init();
void vmm_map(paddr p, vaddr v);
page_directory_t* vmm_copy_pdir(page_directory_t*);
page_directory_t* vmm_get_pdir();
void vmm_set_pdir(page_directory_t*);

#endif