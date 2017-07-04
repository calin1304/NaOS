#include "malloc.h"

Heap heap;

#define HEAP_START 0x2000

void heap_initialize()
{
    heap.head   = (HeapNode*)HEAP_START;
    heap.end    = heap.head;
    heap.size   = 0;
}

void* heap_alloc_new_node(unsigned int size)
{    
    heap.end->allocated = size;
    heap.end->next = (struct HeapNode_*)&(heap.end->data) + size;
    HeapNode *ret = heap.end;
    heap.end = heap.end->next;
    heap.size += 1;
    return ret;
}

void* malloc(unsigned int size)
{
    return &((HeapNode*)heap_alloc_new_node(size))->data;
}