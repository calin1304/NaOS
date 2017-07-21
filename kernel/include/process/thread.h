#ifndef THREAD_H
#define THREAD_H

#include <stdint.h>

#define DEFAULT_STACK_SIZE 4096

#define THREAD_PAUSED 0
#define THREAD_RUNNING 1

#define THREAD_PRIORITY_HIGH 0
#define THREAD_PRIORITY_MEDIUM 1
#define THREAD_PRIORITY_LOW 2

typedef int(*entryFn)();

typedef struct TrapFrame_{
    uint32_t esp;
    uint32_t ebp;
    uint32_t eip;
    uint32_t edi;
    uint32_t esi;
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
    uint32_t eflags;
} TrapFrame;

typedef struct Thread_ {
    struct Process_     *parent;
    void                *stack;
    void                *stackLimit;
    void                *kernelStack;
    entryFn             entry;
    int                 priority;
    int                 state;
    TrapFrame           trapFrame;
} Thread;

Thread createThread(struct Process_ *parent, entryFn entry, int priority, int state);

#endif