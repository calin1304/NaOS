#include "libk/malloc.h"

#include "mm/pmm.h"
#include "mm/vmm.h"

#define HEAP_START 0x006000000

void heap_initialize()
{
    vmm_map((HeapNode*)pmm_alloc_block(), HEAP_START);
    heap.head = (HeapNode*)HEAP_START;
    heap.end = heap.head;
    heap.size = 0;
    heapSegmentEnd = ((uint8_t*)heap.head) + PAGE_SIZE;
    memset(heap.head, 0, PAGE_SIZE);
}

static void* heap_node_next(const HeapNode *p)
{
    return (uint8_t*)(&(p->data)) + p->allocated;
}

void* heap_alloc_new_node(unsigned int size)
{    
    heap.end->allocated = size;
    heap.end->next = heap_node_next(heap.end);
    HeapNode *ret = heap.end;
    heap.end = heap.end->next;
    heap.size += 1;
    return ret;
}

unsigned int heap_get_available_memory()
{
    return (uint8_t*)heapSegmentEnd - (uint8_t*)heap.end - sizeof(HeapNode);
}

int heap_has_available_memory(unsigned int s) 
{   
    return heap_get_available_memory() >= s;
}

void heap_extend(unsigned int s)
{
    int pageCount = (s / PAGE_SIZE) + (s % PAGE_SIZE);
    while (pageCount) {
        void *p = pmm_alloc_block();
        vmm_map(p, heapSegmentEnd);
        heapSegmentEnd += PAGE_SIZE;
        --pageCount;
    }
}

void* malloc(unsigned int size)
{
    if (!heap.head) {
        heap_initialize();
    }
    if (!heap_has_available_memory(size)) {
        heap_extend(size);
    }
    return &(((HeapNode*)heap_alloc_new_node(size))->data);
}