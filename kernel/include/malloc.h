#ifndef MALLOC_H
#define MALLOC_H

#include <stdint.h>

typedef struct HeapNode_ {
    uint32_t allocated;
    struct HeapNode_ *next;
    uint8_t *data; 
} HeapNode;

typedef struct Heap_ {
    HeapNode *head;
    HeapNode *end;
    int size;
} Heap;

void heap_initialize();
void* malloc(unsigned int size);

#endif