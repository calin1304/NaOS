/** This module implements the kernel-space heap. It uses a list nodes to store all the
 * allocated data.
 */

#include <kmalloc.h>

#include "mm/pmm.h"
#include "mm/vmm.h"

/** Address of heap start. */
#define HEAP_START 0x006000000

/** Initialize the heap. */
void heap_initialize()
{
    // Allocate an initial heap block.
    vmm_map((HeapNode*)pmm_alloc_block(), HEAP_START);
    heap.head = (HeapNode*)HEAP_START;
    heap.end = heap.head;
    heap.size = 0;
    heapSegmentEnd = ((uint8_t*)heap.head) + PAGE_SIZE;
    memset(heap.head, 0, PAGE_SIZE);
}

/** Given a heap node addres, return address of the next heap node. */
static void* heap_node_next(const HeapNode *p)
{
    // TODO: Why do we have a `next` field in a `HeapNode` if we have this function?
    return (uint8_t*)(&(p->data)) + p->allocated;
}

/** Allocate a new heap node. */
void* heap_alloc_new_node(unsigned int size)
{    
    heap.end->allocated = size;
    heap.end->next = heap_node_next(heap.end);
    HeapNode *ret = heap.end;
    heap.end = heap.end->next;
    heap.size += 1;
    return ret;
}

/** Return all the memory used by the heap. */
unsigned int heap_get_available_memory()
{
    // TODO: What is this function actually trying to do?
    return (uint8_t*)heapSegmentEnd - (uint8_t*)heap.end - sizeof(HeapNode);
}

/** Check if we can allocate `s` bytes on the heap. */
int heap_has_available_memory(unsigned int s) 
{   
    return heap_get_available_memory() >= s;
}

/** Try to grow the heap by `s` bytes. */
void heap_extend(unsigned int s)
{
    int pageCount = (s / PAGE_SIZE) + (s / PAGE_SIZE == 0);
    while (pageCount--) {
        void *p = pmm_alloc_block();
        vmm_map(p, heapSegmentEnd);
        heapSegmentEnd += PAGE_SIZE;
    }
}

/** Allocate `size` bytes on the heap, returning the starting address. */
void* kmalloc(unsigned int size)
{
    if (!heap.head) {
        heap_initialize();
    }
    if (!heap_has_available_memory(size)) {
        heap_extend(size);
    }
    return &(((HeapNode*)heap_alloc_new_node(size))->data);
}

/** Free memory at address `ptr`. */
void kfree(void * ptr)
{
    // TODO: Check for double free?
    // TODO: Check if memory is actully in heap?
    HeapNode *hn = (HeapNode*)(((int*)ptr - 2));
    hn->allocated = 0;
}