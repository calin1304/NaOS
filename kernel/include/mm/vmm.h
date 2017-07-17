#ifndef VMM_H
#define VMM_H

#include <stdint.h>

#include "mm/pmm.h"

typedef uint32_t vaddr;
typedef uint32_t PTEntry;
typedef uint32_t PDEntry;

#define PAGES_PER_TABLE 1024
#define PAGES_PER_DIR   1024
#define PAGE_SIZE       4096

typedef struct PTable_ {
    PTEntry entries[PAGES_PER_TABLE];
} PTable;

typedef struct PDirectory_ {
    PDEntry entries[PAGES_PER_DIR];
} PDirectory;

enum PTE_FLAGS {
    PTE_PRESENT     = 0x1,
    PTE_WRITABLE    = 0x2,
    PTE_USER        = 0x4,
    PTE_ACCESSED    = 0x20,
    PTE_DIRTY       = 0x40,
};

enum PDE_FLAGS {
    PDE_PRESENT         = 0x1,
    PDE_WRITABLE        = 0x2,
    PDE_USER            = 0x4,
    PDE_WRITETHROUGH    = 0x8,
    PDE_CACHEABLE       = 0x10,
    PDE_ACCESSED        = 0x20,
    PDE_4MB_PAGE        = 0x80
};

int         vmm_alloc_page          (PTEntry *e);
void        vmm_free_page           (PTEntry *e);
PTEntry*    vmm_ptable_lookup_entry (PTable *p, vaddr addr);
PDEntry*    vmm_pdir_lookup_entry   (PDirectory *p, vaddr addr);
int         vmm_switch_pdirectory   (PDirectory *dir);
PDirectory* vmm_get_directory       ();
void        vmm_map_page            (void *phys, void *virt);
void        vmm_init                ();
int         vmm_vaddr_is_mapped     (vaddr);
void        vmm_free_vaddr_page     (vaddr);

#endif