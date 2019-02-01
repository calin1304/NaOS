#include <scheduler.h>
#include <libk/stdio.h>
#include <process/process.h>

extern Process *current_process;

void init_scheduler()
{
}

void switch_process(uint32_t *isr_frame)
{
    if (!current_process) {
        return;
    }
    current_process->eip = isr_frame[1];
    current_process = current_process->next;
    isr_frame[1] = current_process->eip;
}