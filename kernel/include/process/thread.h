#ifndef THREAD_H
#define THREAD_H

#include <process/process.h>

#define DEFAULT_STACK_SIZE 4096

#define THREAD_PAUSED 0

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
    int(*entry)();
    uint32_t            priority;
    int                 state;
    TrapFrame           trapFrame;
} Thread;

#endif