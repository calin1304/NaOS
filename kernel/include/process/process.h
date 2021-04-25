#ifndef PROCESS_H
#define PROCESS_H

#include "mm/vmm.h"
#include "process/thread.h"
#include "idt.h"

#define MAX_THREADS_PER_PROCESS 1

typedef int (*entryFn)();

enum ProcessState {
    PROCESS_STATE_CREATED,
    PROCESS_STATE_TERMINATED,
    PROCESS_STATE_RUNNING,
    PROCESS_STATE_PAUSED
};

typedef struct Process_ {
    uint32_t id;
    enum ProcessState state;
    // PDirectory *pdir;
    Thread threads[MAX_THREADS_PER_PROCESS];
    struct Process_ *next;
    void *eip;
    syscall_frame_t regs;
} Process;

Process create_process(int id, entryFn entry);

#endif
