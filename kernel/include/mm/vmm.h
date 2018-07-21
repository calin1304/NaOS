#ifndef VMM_H
#define VMM_H

#include <stdint.h>
#include <stddef.h>

#include "mm/pmm.h"

typedef uint32_t vaddr;
typedef uint32_t PTEntry;
typedef uint32_t PDEntry;

#define PAGES_PER_TABLE 1024
#define PAGES_PER_DIR   1024
#define PAGE_SIZE       4096

typedef struct PTable_ {
    PTEntry entries[PAGES_PER_TABLE];
} PTable __attribute__((aligned(PAGE_SIZE)));

typedef struct PDirectory_ {
    PDEntry entries[PAGES_PER_DIR];
} PDirectory __attribute__((aligned(PAGE_SIZE)));

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

void pdir_map_page(PDirectory *dir, void *phys, void *virt);
int pdir_vaddr_is_mapped(PDirectory*, vaddr);
paddr pdir_get_paddr(PDirectory *dir, vaddr);

void pte_add_attrib(PTEntry *e, uint32_t attrib);
void pte_del_attrib(PTEntry *e, uint32_t attrib);
void pte_set_frame(PTEntry *e, paddr phyAddr);
int pte_is_present(PTEntry e);
paddr pte_get_paddr(PTEntry e);
void pde_add_attrib(PDEntry *e, uint32_t attrib);
void pde_set_frame(PDEntry *e, paddr phyAddr);
int pde_is_present(PDEntry e);
paddr pde_get_paddr(PDEntry e);

void* vmm_get_phys_addr(vaddr virt);

int         vmm_alloc_page          (PTEntry *e);
void        vmm_free_page           (PTEntry *e);
int         vmm_switch_pdirectory   (PDirectory *dir);
void        vmm_restore_pdirectory  ();
PDirectory* vmm_get_directory       ();
void        vmm_map_page            (void *phys, void *virt);
void        vmm_mapPages            (PDirectory *pdir, paddr phys, vaddr virt, size_t count);
void        vmm_init                ();
int         vmm_vaddr_is_mapped     (vaddr);
void        vmm_free_vaddr_page     (vaddr);
void        vmm_identity_map        (PDirectory *pdir, paddr start, int count);

#endif