#include "kernel/include/process/thread.h"

Thread createThread(struct Process_ *parent, entryFn entry, int priority, int state)
{
    Thread ret;
    ret.parent = parent;
    ret.entry = entry;
    ret.priority = priority;
    ret.state = state;

    ret.trapFrame.eax = 0;
    ret.trapFrame.ebx = 0;
    ret.trapFrame.ecx = 0;
    ret.trapFrame.edx = 0;
    ret.trapFrame.esi = 0;
    ret.trapFrame.edi = 0;
    ret.trapFrame.eip = 0;
    ret.trapFrame.ebp = 0;
    // ret.trapFrame.eflags = flags;
    return ret;
}