#include <stdint.h>

void* kmalloc(unsigned int size);
void kfree(void * ptr);


typedef struct HeapNode_ {
    uint32_t allocated;
    // TODO: Do we need `next`?
    struct HeapNode_ *next;
    uint8_t *data;
} HeapNode;

typedef struct Heap_ {
    HeapNode *head;
    // TODO: Do we need both `end and `size`?
    HeapNode *end;
    int size;
} Heap;

Heap heap;
void* heapSegmentEnd;