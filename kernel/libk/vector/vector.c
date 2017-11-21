#include "vector.h"
#include "stdlib.h"
#include "string.h"

#include <stdint.h>

void vectorInit(Vector *r, int dataSize)
{
    r->data = NULL;
    r->dataSize = dataSize;
    r->size = 0;
    r->capacity = 0;
}

void vectorResize(Vector *r, int value)
{
    void *newData = malloc(value * r->dataSize);
    memcpy(newData, r->data, r->size * r->dataSize);
    //free(newData)
    r->data = newData;
    r->capacity = value;
}

void vectorPushBack(Vector *r, void *value)
{
    if (r->size == r->capacity) {
        vectorResize(r, r->capacity * 2);
    }
    memcpy(((uint8_t*)r->data) + r->size * r->dataSize, value, r->dataSize);
    r->size += 1;
}

void* vectorGet(Vector *r, int idx)
{
    if (idx >= r->size) { 
        return NULL;
    } 
    return ((uint8_t*)r->data) + idx * r->dataSize;
}