#include "mm/vmm.h"

#include "libk/include/string.h"

#define PDIR_INDEX(x) (((x) >> 22) & 0x3ff)
#define PTABLE_INDEX(x) (((x) >> 12) & 0x3ff)
#define PAGE_OFFSET(x) ((x) & 0xfff)
#define PAGE_PHYS_ADDR(x) (*(x) & (~0xfff))

extern void vm_flush_tlb_page(vaddr addr);

static void pte_add_attrib(PTEntry *e, uint32_t attrib)
{
    *e |= attrib;
}

static void pte_del_attrib(PTEntry *e, uint32_t attrib)
{
    *e &= (~attrib);
}

static void pte_set_frame(PTEntry *e, paddr phyAddr)
{
    *e = (phyAddr) | (*e & 0xfff);
}

static int pte_is_present(PTEntry e)
{
    return (e & PTE_PRESENT) == PTE_PRESENT;
}

static int pte_is_writable(PTEntry e)
{
    return (e & PTE_WRITABLE) != 0;
}

static paddr pte_get_paddr(PTEntry e)
{
    return (e & (~0xfff));
}

static void pde_add_attrib(PDEntry *e, uint32_t attrib)
{
    *e |= attrib;
}

static void pde_del_attrib(PDEntry *e, uint32_t attrib)
{
    *e &= (~attrib);
}

static void pde_set_frame(PDEntry *e, paddr phyAddr)
{
    *e = (phyAddr) | (*e & 0xfff);
}

static int pde_is_present(PDEntry e)
{
    return (e & PDE_PRESENT) == PDE_PRESENT;
}

static int pde_is_user(PDEntry e)
{
    return (e & PDE_USER) != 0;
}

static int pde_is_writable(PDEntry e)
{
     return (e & PDE_WRITABLE) != 0;
}

static paddr pde_get_paddr(PTEntry e)
{
    return (e & (~0xfff));
}

static void vmm_ptable_clear(PTable *t)
{
    memset(t, 0, sizeof(PTable));
}

int vmm_alloc_page(PTEntry *e)
{
    void *p = pmm_alloc_block();
    if (!p) {
        return 0;
    }
    pte_set_frame(e, (paddr)p);
    pte_add_attrib(e, PTE_PRESENT);
    return 1;
}

void vmm_free_page(PTEntry *e)
{
    void *p = (void*)pte_get_paddr(*e);
    if (p) {
        pmm_free_block(p);
    }
    pte_del_attrib(e, PTE_PRESENT);
    // *e = 0;
}

// PTEntry* vmm_ptable_lookup_entry(PTable *p, vaddr addr);
// PDEntry* vmm_pdir_lookup_entry(PDirectory *p, vaddr addr);

PDirectory *currentDirectory = 0;
paddr currentPDBR = 0;

int vmm_switch_pdirectory(PDirectory *dir)
{
    if (!dir) {
        return 0;
    }
    currentDirectory = dir;
    pmm_load_pdbr(currentDirectory);
    return 1;
}

// PDirectory* vmm_get_directory();
void vmm_map_page(void *phys, void *virt)
{
    PDirectory *dir = currentDirectory;
    PDEntry *e = &(dir->entries[PDIR_INDEX((uint32_t)virt)]);
    if (pde_is_present(*e) == 0) {
        PTable *table = (PTable*)pmm_alloc_block();
        if (!table) {
            asm("cli\nhlt");
            return;
        }
        memset(table, 0, sizeof(PTable));
        pde_add_attrib(e, PDE_PRESENT);
        pde_add_attrib(e, PDE_WRITABLE);
        pde_add_attrib(e, PDE_USER);
        pde_set_frame(e, (paddr)table);
    }
    PTable *table = (PTable*)PAGE_PHYS_ADDR(e);
    PTEntry *page = &(table->entries[PTABLE_INDEX((uint32_t)virt)]);
    pte_set_frame(page, (paddr)phys);
    pte_add_attrib(page, PTE_PRESENT);
    pte_add_attrib(page, PTE_WRITABLE);
    pte_add_attrib(page, PTE_USER);
}

void vmm_init()
{
    PTable *table = (PTable*)pmm_alloc_block();
    if (!table) {
        asm("cli\nhlt");
        return;
    }
    vmm_ptable_clear(table);
    unsigned int frame, virt;

    for (int i = 0, frame = 0x0, virt = 0x00000000; i < 1024; ++i) {
        PTEntry *page = &(table->entries[i]);
        pte_add_attrib(page, PTE_PRESENT);
        pte_add_attrib(page, PTE_WRITABLE);
        pte_add_attrib(page, PTE_USER);
        pte_set_frame(page, frame);
        frame += 4096;
        virt += 4096;
    }

    PDirectory *dir = (PDirectory*)pmm_alloc_block();
    if (!dir) {
        asm("cli\nhlt");
        return;
    }
    memset(dir, 0, sizeof(PDirectory));

    PDEntry *entry = &(dir->entries[PDIR_INDEX(0x00000000)]);
    pde_add_attrib(entry, PDE_PRESENT);
    pde_add_attrib(entry, PDE_WRITABLE);
    pde_add_attrib(entry, PDE_USER);
    pde_set_frame(entry, (paddr)table);

    vmm_switch_pdirectory(dir);
    pmm_enable_paging();
}

int vmm_vaddr_is_mapped(vaddr addr)
{
    PDirectory *dir = currentDirectory;
    if (!pde_is_present(dir->entries[PDIR_INDEX(addr)])) {
        return 0;
    }
    PTable *table = pde_get_paddr(dir->entries[PDIR_INDEX(addr)]);
    if (!pte_is_present(table->entries[PTABLE_INDEX(addr)])) {
        return 0;
    }
    return 1;
}

void vmm_free_vaddr_page(vaddr addr)
{
    PDEntry pde = currentDirectory->entries[PDIR_INDEX(addr)];
    PTable *table = pde_get_paddr(pde);
    PTEntry *pte = &(table->entries[PTABLE_INDEX(addr)]);
    if (!pte_is_present(*pte)) {
        return;
    }
    vmm_free_page(pte);
    vm_flush_tlb_page(addr);
}