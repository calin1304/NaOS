#include "mm/pmm.h"

#include <libk/string.h>

#define BLOCK_SIZE 4096

#define BITMAP_INDEX_ADDR(base, i) ((base) + (i) / sizeof(uint32_t))
#define SET_BIT(x, i) ((x) | (1 << (i)))
#define UNSET_BIT(x, i) ((x) & (~(1 << (i))))
#define TEST_BIT(x, i) ((x) & (1 << (i)))

#define ADDR_TO_BLOCK(x) ((unsigned int)(x) / BLOCK_SIZE)
#define BLOCK_TO_ADDR(x) ((void*)((x) * BLOCK_SIZE));

extern void _symbol_KERNEL_START;
extern void _symbol_KERNEL_END;

uint32_t *pmm_bitmap = &_symbol_KERNEL_END;
uint32_t pmm_bitmap_block_count = 0;

static void set_block(int i)
{
    uint32_t *p = BITMAP_INDEX_ADDR(pmm_bitmap, i);
    *p = SET_BIT(*p, i % sizeof(uint32_t));
}

static void unset_block(int i)
{
    uint32_t *p = BITMAP_INDEX_ADDR(pmm_bitmap, i);
    *p = UNSET_BIT(*p, i % sizeof(uint32_t));
}

static void set_blocks(unsigned int i, unsigned int count)
{
    while (count--) set_block(i++);
}

static void unset_blocks(unsigned int i, unsigned int count)
{
    while (count--) unset_block(i++);
}

static unsigned int test_block(unsigned int i)
{
    uint32_t *p = BITMAP_INDEX_ADDR(pmm_bitmap, i);
    return TEST_BIT(*p, i % (sizeof(uint32_t)));
}

void pmm_init(multiboot_info_t *mbt)
{
    // Reserver blocks occupied by bitmap
    // TODO: Compute size of bitmap and reserver blocks based on that
    set_block(ADDR_TO_BLOCK(pmm_bitmap));
    // Compute number of blocks in memory map
    multiboot_memory_map_t *mmap = (multiboot_memory_map_t*)mbt->mmap_addr;
    while (mmap < mbt->mmap_addr + mbt->mmap_length) {
        pmm_bitmap_block_count += mmap->len_low;
        mmap = (multiboot_memory_map_t*) ((uintptr_t)mmap + mmap->size + sizeof(mmap->size));
    }
    // Set all memory blocks as occupied in bitmap
    pmm_bitmap_block_count /= BLOCK_SIZE;
    memset(pmm_bitmap, 0xffffffff, pmm_bitmap_block_count/sizeof(uint32_t));
    // Set free memory as free blocks in bitmap
    mmap = mbt->mmap_addr;
    while (mmap < mbt->mmap_addr + mbt->mmap_length) {
        if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE) {
            unset_blocks(ADDR_TO_BLOCK(mmap->addr_low), mmap->len_low / BLOCK_SIZE);
        }
        mmap = (multiboot_memory_map_t*) ((uintptr_t)mmap + mmap->size + sizeof(mmap->size));
    }
    // Reserver first block
    set_block(0);
    // Reserve blocks occupied by kernel
    int kernel_blocks = ((uintptr_t)&_symbol_KERNEL_END - (uintptr_t)&_symbol_KERNEL_START + 1) / BLOCK_SIZE;
    set_blocks(ADDR_TO_BLOCK(&_symbol_KERNEL_START), kernel_blocks);
}

static unsigned int find_free_block()
{
    for (unsigned int i = 1; i < pmm_bitmap_block_count; ++i) {
        if (test_block(i) == 0) {
            return i;
        }
    }
    return 0; // FIXME: Return error
}

void* pmm_alloc_block()
{
    unsigned int i = find_free_block();
    set_block(i);
    void *ret = BLOCK_TO_ADDR(i);
    // FIXME: Return error instead of halting
    if (ret == 0) {
        __asm__ __volatile__("cli\nhlt");
    }
    return ret;
}

void pmm_free_block(void* p)
{
    unset_block(ADDR_TO_BLOCK(p));
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
    __asm__ __volatile__("movl %%cr0, %0" : "=r"(cr0));
    __asm__ __volatile__("movl %0, %%cr0" : : "r"(cr0 & (~0x80000000)));
}