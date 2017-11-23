#ifndef VECTOR_H
#define VECTOR_H

typedef struct Vector_ {
    void *data;
    int dataSize;
    int size;
    int capacity;
} Vector;

void vectorInit(Vector *r, int dataSize);
void vectorPushBack(Vector *r, void *value);
void vectorResize(Vector *r, int value);
void* vectorGet(Vector *r, int idx);

#endif