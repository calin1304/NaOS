#ifndef PROCESS_H
#define PROCESS_H

#include <mm/vmm.h>
#include <process/thread.h>
#include <idt.h>

#define MAX_THREADS_PER_PROCESS 1

typedef int (*entryFn)();

enum {
    PROCESS_CREATED,
    PROCESS_TERMINATED,
    PROCESS_RUNNING,
    PROCESS_PAUSED
};

typedef struct Process_ {
    size_t id;
    int state;
    page_directory_t *pdir;
    Thread threads[MAX_THREADS_PER_PROCESS];    
    struct Process_ *next;
    void *eip;
    void *stack0;
    void *stack3;
    syscall_frame_t regs;
} Process;

Process create_process(entryFn entry);

#endif