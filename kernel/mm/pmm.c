#include "mm/pmm.h"

#include "libk/include/string.h"

#define PMM_BLOCK_SIZE 4096

uint32_t *pmmBitmap;
uint32_t pmmBitmapBlockCount;

#define PMM_BLOCK_FROM_PADDR(x) ((unsigned int)(x) / PMM_BLOCK_SIZE)
#define PMM_BLOCK_ADDR(x) ((void*)((x) * PMM_BLOCK_SIZE));

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

void pmm_init(struct MemoryMapInfo *mm, uint16_t entries, paddr bitmap)
{    
    pmmBitmap = (uint32_t*)bitmap;
    pmm_set_block(PMM_BLOCK_FROM_PADDR(bitmap));
    pmmBitmapBlockCount = 0;
    for (unsigned int i = 0; i < entries; ++i) {
        pmmBitmapBlockCount += mm[i].length;
    }
    pmmBitmapBlockCount /= PMM_BLOCK_SIZE;
    memset(pmmBitmap, 0xffffffff, pmmBitmapBlockCount/sizeof(uint32_t));
    for (unsigned int i = 1; i < entries; ++i) {
        if (mm[i].type == 0x1) {
            pmm_unset_blocks(PMM_BLOCK_FROM_PADDR(mm[i].base), mm[i].length / PMM_BLOCK_SIZE);
        }
    }
    pmm_set_block(0);
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