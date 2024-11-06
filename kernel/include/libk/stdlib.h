#ifndef STDLIB_H
#define STDLIB_H

void* malloc(unsigned int size);
void free(void * ptr);

#define assert(EX) (void)((EX) || (__assert(#EX, __FILE__, __LINE__)))

#endif
