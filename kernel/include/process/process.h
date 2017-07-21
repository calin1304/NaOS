#ifndef PROCESS_H
#define PROCESS_H

#include <mm/vmm.h>
#include <process/thread.h>

#define MAX_THREADS_PER_PROCESS 1

#define PROCESS_PAUSED 0
#define PROCESS_RUNNING 1

typedef struct Process_ {
    int id;
    int state;
    PDirectory *pdir;
    Thread threads[MAX_THREADS_PER_PROCESS];
} Process;

Process* createProcess(const char* filename);

#endif