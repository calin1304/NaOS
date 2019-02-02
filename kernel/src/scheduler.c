#include <scheduler.h>
#include <libk/stdio.h>

static Process *current_process = NULL;

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

void scheduler_add(Process *p)
{
    if (!current_process) {
        current_process = p;
        current_process->next = p;
    } else {
        p->next = current_process->next;
        current_process->next = p;
    }
}

void scheduler_start()
{
    ((entryFn)current_process->eip)();
}