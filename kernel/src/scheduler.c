/*
 * This module implements the process scheduler. It should build a list of processes to
 * run and go through the list and run each process for a small amount of time.
 */
#include <scheduler.h>
#include "libk/stdio.h"

static Process *current_process = NULL;
static int is_started = 0;

void init_scheduler()
{
}

void switch_process(uint32_t *task_eip, syscall_frame_t *frame)
{
    if (!is_started) {
        return;
    }
    if (current_process->state != PROCESS_STATE_PAUSED) {
        current_process->state = PROCESS_STATE_PAUSED;
        // current_process->stack3 = isr_frame[4]; // Save userspace ESP
        current_process->eip = *task_eip; // Save EIP
        current_process->regs = *frame; // Save register values

        current_process = current_process->next; // Load next process
    }
    current_process->state = PROCESS_STATE_RUNNING;
    *task_eip = current_process->eip; // Set interrupt return EIP to next process
    *frame = current_process->regs; // Restore register values for task
    // isr_frame[4] = current_process->stack3; // Set stack for next process
    // Switch virtual space to new process
    // vmm_set_pdir(current_process->pdir);

    // Set kernel stack in TSS
    // install_tss(0x10, current_process->stack0);

    /* Must save and later restore all registers (eax, ebx...). Don't know if
    they are already saved in tss by context switch */
}

void scheduler_add(Process *p)
{
    if (!current_process) {
        // No process in queue, just add this one and link it to itself.
        current_process = p;
        current_process->next = p;
    } else {
        // Add process before current process.
        // TODO: This might be wrong
        p->next = current_process->next;
        current_process->next = p;
    }
    p->state = PROCESS_STATE_PAUSED;
}

void scheduler_start()
{
    is_started = 1;
}

