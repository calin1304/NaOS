#ifndef PROCESS_H
#define PROCESS_H

#include <mm/vmm.h>
#include <process/thread.h>

#define MAX_THREADS_PER_PROCESS 1

typedef int (*entryFn)();

enum {
    PROCESS_CREATED,
    PROCESS_TERMINATED,
    PROCESS_RUNNING,
    PROCESS_PAUSED
};

typedef struct Process_ {
    int id;
    int state;
    PDirectory *pdir;
    Thread threads[MAX_THREADS_PER_PROCESS];    
    struct Process_ *next;
    uint32_t eip;
} Process;

Process *current_process;

Process* createProcess(const char* filename);
Process create_process(int id, entryFn entry);

#endif