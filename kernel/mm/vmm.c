#include "mm/vmm.h"

#include "libk/include/string.h"

#define PDIR_INDEX(x) (((x) >> 22) & 0x3ff)
#define PTABLE_INDEX(x) (((x) >> 12) & 0x3ff)
#define PAGE_OFFSET(x) ((x) & 0xfff)
#define PAGE_PHYS_ADDR(x) (*(x) & (~0xfff))

PDirectory *currentDirectory = 0;
PDirectory *oldDirectory = 0;

void vmm_flush_tlb_page(vaddr addr)
{
    asm volatile(
        "cli\n"
        "invlpg %0\n"
        "sti"
        :
        : "m"(addr)
    );
}

void pte_add_attrib(PTEntry *e, uint32_t attrib)
{
    *e |= attrib;
}

void pte_del_attrib(PTEntry *e, uint32_t attrib)
{
    *e &= (~attrib);
}

void pte_set_frame(PTEntry *e, paddr phyAddr)
{
    *e = (phyAddr) | (*e & 0xfff);
}

int pte_is_present(PTEntry e)
{
    return (e & PTE_PRESENT) == PTE_PRESENT;
}

// int pte_is_writable(PTEntry e)
// {
//     return (e & PTE_WRITABLE) != 0;
// }

paddr pte_get_paddr(PTEntry e)
{
    return (e & (~0xfff));
}

void pde_add_attrib(PDEntry *e, uint32_t attrib)
{
    *e |= attrib;
}

// void pde_del_attrib(PDEntry *e, uint32_t attrib)
// {
//     *e &= (~attrib);
// }

void pde_set_frame(PDEntry *e, paddr phyAddr)
{
    *e = (phyAddr) | (*e & 0xfff);
}

int pde_is_present(PDEntry e)
{
    return (e & PDE_PRESENT) == PDE_PRESENT;
}

// int pde_is_user(PDEntry e)
// {
//     return (e & PDE_USER) != 0;
// }

// int pde_is_writable(PDEntry e)
// {
//      return (e & PDE_WRITABLE) != 0;
// }

paddr pde_get_paddr(PDEntry e)
{
    return (e & (~0xfff));
}

void vmm_ptable_clear(PTable *t)
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
}

int vmm_switch_pdirectory(PDirectory *dir)
{
    if (!dir) {
        return 0;
    }
    oldDirectory = currentDirectory;
    currentDirectory = dir;
    pmm_load_pdbr(currentDirectory);
    return 1;
}

void vmm_restore_pdirectory()
{    
    vmm_switch_pdirectory(oldDirectory);
}

PDirectory* vmm_get_directory()
{
    return currentDirectory;
}

void vmm_map_page(void *phys, void *virt)
{
    pdir_map_page(currentDirectory, phys, virt);
}

void vmm_init()
{
    PDirectory *dir = (PDirectory*)pmm_alloc_block();
    if (!dir) {
        asm("cli\nhlt");
        return;
    }
    memset(dir, 0, sizeof(PDirectory));
    vmm_identity_map(dir, 0x0, 1024);

    vmm_switch_pdirectory(dir);
    pmm_enable_paging();
}

int vmm_vaddr_is_mapped(vaddr addr)
{
    return pdir_vaddr_is_mapped(currentDirectory, addr);
}

void vmm_free_vaddr_page(vaddr addr)
{
    PDEntry pde = currentDirectory->entries[PDIR_INDEX(addr)];
    PTable *table = (PTable*)pde_get_paddr(pde);
    PTEntry *pte = &(table->entries[PTABLE_INDEX(addr)]);
    if (!pte_is_present(*pte)) {
        return;
    }
    vmm_free_page(pte);
    vmm_flush_tlb_page(addr);
}

void vmm_identity_map(PDirectory *pdir, paddr start, int count)
{
    vaddr currAddr = start;// & (~0xfff);
    for (int i = 0; i < count; ++i) {
        PDEntry *t = &(pdir->entries[PDIR_INDEX(currAddr)]);
        if (*t == 0) {
            PTable *newTable = pmm_alloc_block();
            memset(newTable, 0, sizeof(PTable));
            pde_add_attrib(t, PDE_WRITABLE);
            pde_add_attrib(t, PDE_PRESENT);
            pde_set_frame(t, (paddr)newTable);
        }
        PTable *table = (PTable*)pde_get_paddr(*t);
        PTEntry *e = &(table->entries[PTABLE_INDEX(currAddr)]);
        pte_add_attrib(e, PTE_WRITABLE);
        pte_add_attrib(e, PTE_PRESENT);
        pte_set_frame(e, (paddr)currAddr);
        currAddr += PAGE_SIZE;
    }
}

void pdir_map_page(PDirectory *dir, void *phys, void *virt)
{
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
        pde_set_frame(e, (paddr)table);
    }
    PTable *table = (PTable*)pde_get_paddr(*e);
    PTEntry *page = &(table->entries[PTABLE_INDEX((uint32_t)virt)]);
    pte_set_frame(page, (paddr)phys);
    pte_add_attrib(page, PTE_PRESENT);
    pte_add_attrib(page, PTE_WRITABLE);
}

int pdir_vaddr_is_mapped(PDirectory *dir, vaddr addr)
{
    if (!pde_is_present(dir->entries[PDIR_INDEX(addr)])) {
        return 0;
    }
    PTable *table = (PTable*)pde_get_paddr(dir->entries[PDIR_INDEX(addr)]);
    if (!pte_is_present(table->entries[PTABLE_INDEX(addr)])) {
        return 0;
    }
    return 1;
}

paddr pdir_get_paddr(PDirectory *dir, vaddr virt)
{
    PDEntry *pde = &(dir->entries[PDIR_INDEX(virt)]);
    PTable *t = pde_get_paddr(*pde);
    PTEntry *pte = &(t->entries[PTABLE_INDEX(virt)]);
    return pte_get_paddr(*pte);
}

void* vmm_get_phys_addr(vaddr virt)
{
    PDEntry *pde = &(currentDirectory->entries[PDIR_INDEX(virt)]);
    PTable *pt = (PTable*)pde_get_paddr(*pde);
    PTEntry *pte = &(pt->entries[PTABLE_INDEX(virt)]);
    return pte_get_paddr(*pte) + PAGE_OFFSET(virt);
}