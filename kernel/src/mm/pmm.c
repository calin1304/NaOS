#include "mm/pmm.h"

#include <string.h>

#define PMM_BLOCK_SIZE 4096

uint32_t *pmmBitmap;
uint32_t pmmBitmapBlockCount;

#define PMM_BLOCK_FROM_PADDR(x) ((unsigned int)(x) / PMM_BLOCK_SIZE)
#define PMM_BLOCK_ADDR(x) ((void*)((x) * PMM_BLOCK_SIZE));

extern void _symbol_KERNEL_START;
extern void _symbol_KERNEL_END;

void pmm_set_block(int i)
{
    uint32_t *p = pmmBitmap + i / sizeof(uint32_t);
    *p = (*p) | (1 << (i % sizeof(uint32_t)));
}

void pmm_unset_block(int i)
{
    uint32_t *p = pmmBitmap + i / sizeof(uint32_t);
    *p = (*p) & (~(1 << (i % sizeof(uint32_t))));
}

void pmm_set_blocks(unsigned int i, unsigned int count)
{
    for (; count > 0; ++i, --count) {
        pmm_set_block(i);
    }
}

void pmm_unset_blocks(unsigned int i, unsigned int count)
{
    for (; count > 0; ++i, --count) {
        pmm_unset_block(i);
    }
}

unsigned int pmm_test_block(unsigned int i)
{
    return pmmBitmap[i/sizeof(uint32_t)] & (1 << i%(sizeof(uint32_t)));
}

void pmm_init(multiboot_info_t *mbt)
{    
    pmmBitmap = &_symbol_KERNEL_END;
    pmm_set_block(PMM_BLOCK_FROM_PADDR(pmmBitmap));
    pmmBitmapBlockCount = 0;
    multiboot_memory_map_t *mmap = (multiboot_memory_map_t*)mbt->mmap_addr;    
    while (mmap < mbt->mmap_addr + mbt->mmap_length) {
        pmmBitmapBlockCount += mmap->len_low;
        mmap = (multiboot_memory_map_t*) ((unsigned int)mmap + mmap->size + sizeof(mmap->size));
    }
    pmmBitmapBlockCount /= PMM_BLOCK_SIZE;
    memset(pmmBitmap, 0xffffffff, pmmBitmapBlockCount/sizeof(uint32_t));
    mmap = mbt->mmap_addr;
    while (mmap < mbt->mmap_addr + mbt->mmap_length) {
        if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE) {
            pmm_unset_blocks(PMM_BLOCK_FROM_PADDR(mmap->addr_low), mmap->len_low / PMM_BLOCK_SIZE);
        }
        mmap = (multiboot_memory_map_t*) ((unsigned int)mmap + mmap->size + sizeof(mmap->size));
    }
    pmm_set_block(0);
    int kernel_blocks = ((uintptr_t)&_symbol_KERNEL_END - (uintptr_t)&_symbol_KERNEL_START + 1) / PMM_BLOCK_SIZE;
    pmm_set_blocks(PMM_BLOCK_FROM_PADDR(&_symbol_KERNEL_START), kernel_blocks);
}

unsigned int pmm_get_first_free_block()
{
    for (unsigned int i = 1; i < pmmBitmapBlockCount; ++i) {
        if (pmm_test_block(i) == 0) {
            return i;
        }
    }
    return 0;
}

void* pmm_alloc_block()
{
    unsigned int i = pmm_get_first_free_block();
    pmm_set_block(i);
    void *ret = PMM_BLOCK_ADDR(i);
    if (ret == 0) {
        asm("cli\nhlt");
    }
    return ret;
}

void pmm_free_block(void* p)
{
    pmm_unset_block(PMM_BLOCK_FROM_PADDR(p));
}

void pmm_load_pdbr(void *pdAddr)
{
	__asm__ __volatile__("movl %0, %%cr3" : : "r"(pdAddr));
}

void pmm_enable_paging()
{
    __asm__ __volatile__ (
        "movl %%cr0, %%eax\n"
        "orl $0x80000001, %%eax\n"
        "movl %%eax, %%cr0\n"
        :
        :
        : "%eax"
    );
}

void pmm_disable_paging()
{
    uint32_t cr0;
    asm volatile("movl %%cr0, %0" : "=r"(cr0));
    asm volatile("movl %0, %%cr0" : : "r"(cr0 & (~0x80000000)));
}