#ifndef THREAD_H
#define THREAD_H

#include <stdint.h>

#define DEFAULT_STACK_SIZE 4096

enum ThreadState {
    THREAD_STATE_PAUSED,
    THREAD_STATE_RUNNING
};

enum ThreadPriority {
    THREAD_PRIORITY_LOW,
    THREAD_PRIORITY_NORMAL,
    THREAD_PRIORITY_HIGH
};

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
    enum ThreadPriority priority;
    enum ThreadState    state;
    TrapFrame           trapFrame;
} Thread;

Thread createThread(struct Process_ *parent, entryFn entry, int priority, int state);

#endif
