/** This module implements the physical memory manager. It uses a bitmap where each bit
 * describes if that corresponding block is used or not.
 */

#include "mm/pmm.h"

#include <string.h>
#include <error.h>
#include <utils.h>

/** The manager only allocates memory in blocks of size BLOCK_SIZE. */
#define BLOCK_SIZE 4096

/** Get bitmap index from memory address. */
#define BITMAP_INDEX_ADDR(base, i) ((base) + (i) / sizeof(uint32_t))

/** Given a memory address, get the bitmap index for that block. */
#define ADDR_TO_BLOCK(x) ((unsigned int)(x) / BLOCK_SIZE)
/** Given an index into the bitmap, get the starting memory address of that block. */
#define BLOCK_TO_ADDR(x) ((void*)((x) * BLOCK_SIZE));

// property: BLOCK_TO_ADDR(ADDR_TO_BLOCK(addr)) == addr
// property: ADDR_TO_BLOCK(BLOCK_TO_ADDR(i)) == i

/** Symbols defined in the linker script. */
extern void _symbol_KERNEL_START;
extern void _symbol_KERNEL_END;

/** Start address of bitmap */
uint32_t *pmm_bitmap = 0;
/** How many blocks are described by the bitmap; basically memory size. */
uint32_t pmm_bitmap_block_count = 0;

/** Set memory block at index `i` as used. */
static void set_block(int i)
{
    uint32_t *p = BITMAP_INDEX_ADDR(pmm_bitmap, i);
    *p = SET_BIT(*p, i % sizeof(uint32_t));
}

/** Set memory block at index `i` as unused. */
static void unset_block(int i)
{
    uint32_t *p = BITMAP_INDEX_ADDR(pmm_bitmap, i);
    *p = UNSET_BIT(*p, i % sizeof(uint32_t));
}

/** Set multiple memory blocks, starting from index `i`, as used. */
static void set_blocks(unsigned int i, unsigned int count)
{
    while (count--) set_block(i++);
}

/** Set multiple memory blocks, starting from index `i`, as unused. */
static void unset_blocks(unsigned int i, unsigned int count)
{
    while (count--) unset_block(i++);
}

/** Check if the memory block at index `i` as used or not. */
static unsigned int test_block(unsigned int i)
{
    uint32_t *p = BITMAP_INDEX_ADDR(pmm_bitmap, i);
    return TEST_BIT(*p, i % (sizeof(uint32_t)));
}

/** Initialize the memory manager. */
void pmm_init(void *bitmap, multiboot_info_t *mbt)
{
    // Reserver blocks occupied by bitmap
    // TODO: Compute size of bitmap and reserved blocks based on that
    pmm_bitmap = bitmap;
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
    // Reserve first block so that we don't trash real-mode interrupts.
    set_block(0);
    // Reserve blocks occupied by kernel
    int kernel_blocks = ((uintptr_t)&_symbol_KERNEL_END - (uintptr_t)&_symbol_KERNEL_START + 1) / BLOCK_SIZE;
    set_blocks(ADDR_TO_BLOCK(&_symbol_KERNEL_START), kernel_blocks);
}

static unsigned int find_free_block()
{
    for (unsigned int i = 0; i < pmm_bitmap_block_count; ++i) {
        if (test_block(i) == 0) {
            return i;
        }
    }
    last_error = ENOMEM;
    return -ENOMEM;
}

/** This function allocates a memory block, returning its address and setting it as used.
 */
void* pmm_alloc_block()
{
    unsigned int i = find_free_block();
    if (i == -ENOMEM) {
        return NULL;
    }
    set_block(i);
    return BLOCK_TO_ADDR(i);
}

/** Free a memory block, setting it as unused */
void pmm_free_block(void* p)
{
    unset_block(ADDR_TO_BLOCK(p));
}

/** This functions loads `pdAddr` in the cr3 register. This is used when enabling pagind,
 * `pdAddr` being the address of the page directory
 */
void pmm_load_pdbr(void *pdAddr)
{
	__asm__ __volatile__("movl %0, %%cr3" : : "r"(pdAddr));
}

/** Enable paging by setting the paging (PG) and protection (PE) bits in CR0 */
void pmm_enable_paging()
{
    // TODO: There is more than one paging mode available. Make sure we are enabling the right one.
    // TODO: Use a variable backed by EAX and macro to set bits.
    __asm__ __volatile__ (
        "movl %%cr0, %%eax\n"
        "orl $0x80000001, %%eax\n"
        "movl %%eax, %%cr0\n"
        :
        :
        : "%eax"
    );
}

/** Disable paging by unsetting the paging (PG) and protection (PE) bits in CR0 */
void pmm_disable_paging()
{
    uint32_t cr0;
    // Read CR0 register.
    __asm__ __volatile__("movl %%cr0, %0" : "=r"(cr0));
    // Unset bits.
    // TODO: Use a macro instead of bitwise operations.
    __asm__ __volatile__("movl %0, %%cr0" : : "r"(cr0 & (~0x80000000)));
}