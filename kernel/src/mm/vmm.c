#include "mm/vmm.h"

#include <libk/string.h>

struct pd_entry {
    uint8_t present : 1;
    uint8_t writable : 1;
    uint8_t user : 1;
    uint8_t write_through : 1;
    uint8_t cache_disabled : 1;
    uint8_t accessed : 1;
    uint8_t zero : 1;
    uint8_t size : 1;
    uint8_t ignored : 4;
    uint32_t pt_paddr : 20;
} __attribute__((packed));

struct pt_entry {
    uint8_t present : 1;
    uint8_t writable : 1;
    uint8_t user : 1;
    uint8_t write_through : 1;
    uint8_t cache_disabled : 1;
    uint8_t accessed : 1;
    uint8_t dirty : 1;
    uint8_t zero : 1;
    uint8_t global : 1;
    uint8_t ignored : 3;
    uint32_t page_paddr : 20;
} __attribute__((packed));

typedef struct pd_entry pd_entry_t;
typedef struct pt_entry pt_entry_t;

#define PAGES_PER_TABLE 1024
#define PAGES_PER_DIR   1024

struct page_directory {
    pd_entry_t entries[PAGES_PER_DIR];
} __attribute__((aligned(PAGE_SIZE)));

struct page_table {
    pt_entry_t entries[PAGES_PER_TABLE];
} __attribute__((aligned(PAGE_SIZE)));

typedef struct page_directory page_directory_t;
typedef struct page_table page_table_t;

#define VADDR_PD_OFFSET(x) (((x) >> 22))
#define VADDR_PT_OFFSET(x) (((x) >> 12) & 0x3ff)
#define VADDR_PAGE_OFFSET(x) ((x) & 0xfff)

#define PD_ENTRY(pd, vaddr) &((pd).entries[VADDR_PD_OFFSET(vaddr)])
#define PT_ENTRY(pt, vaddr) &((pt).entries[VADDR_PT_OFFSET(vaddr)])

#define PDE_PADDR(pde) ((uintptr_t)(pde).pt_paddr << 12)
#define PTE_PADDR(pte) ((uintptr_t)(pte).page_paddr << 12)

#define PADDR_TO_FRAME(paddr) ((uintptr_t)(paddr) >> 12);

#define IS_VALID_PDE(x) (*(uint32_t*)(x) != 0)

page_directory_t pdir;
page_directory_t *current_dir = NULL;

static void init_pd_entry(pd_entry_t *pde)
{
    page_table_t *pt = pmm_alloc_block();
    memset(pt, 0, sizeof(page_table_t));
    pde->present = 1;
    pde->writable = 1;
    pde->user = 1;
    pde->pt_paddr = PADDR_TO_FRAME(pt);
}

static void identity_map(page_directory_t *dir, paddr start, size_t pages)
{
    vaddr currAddr = start;
    for (; pages; --pages, currAddr += PAGE_SIZE) {
        pd_entry_t *pde = PD_ENTRY(*dir, currAddr);
        if (!IS_VALID_PDE(pde)) {
            init_pd_entry(pde);
        }
        page_table_t *pt = PDE_PADDR(*pde);
        pt_entry_t *pte = PT_ENTRY(*pt, currAddr);
        pte->writable = 1;
        pte->present = 1;
        pte->user = 1;
        pte->page_paddr = PADDR_TO_FRAME(currAddr);
    }
}

void vmm_init()
{
    identity_map(&pdir, 0x0, 1024);
    vmm_set_pdir(&pdir);
    pmm_enable_paging();
}

void vmm_map(paddr p, vaddr v)
{
    pd_entry_t *pde = PD_ENTRY(*current_dir, v);
    if (!pde->present) {
        init_pd_entry(pde);
    }
    page_table_t *pt = PDE_PADDR(*pde);
    pt_entry_t *pte = PT_ENTRY(*pt, v);
    pte->present = 1;
    pte->writable = 1;
    pte->user = 1;
    pte->page_paddr = PADDR_TO_FRAME(p)
}

page_directory_t* vmm_copy_pdir(page_directory_t *pdir)
{
    //TODO: Should allocate to page boundary with malloc to map in vspace
    page_directory_t *ret = pmm_alloc_block();
    memcpy(ret, pdir, sizeof(page_directory_t));
    for (int i = 0; i < PAGES_PER_DIR; ++i) {
        if (IS_VALID_PDE(&pdir->entries[i])) {
            page_table_t *pt = PDE_PADDR(pdir->entries[i]);
            page_table_t *t = pmm_alloc_block();
            memcpy(t, pt, sizeof(page_table_t));
            ret->entries[i].pt_paddr = PADDR_TO_FRAME(t);
        }
    }
    return ret;
}

page_directory_t* vmm_get_pdir()
{
    return current_dir;
}

void vmm_set_pdir(page_directory_t *new_pdir)
{
    current_dir = new_pdir;
    pmm_load_pdbr(current_dir);
}