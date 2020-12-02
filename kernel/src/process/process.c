#include "process/process.h"
#include "vfs.h"
#include "elf.h"
#include "mm/pmm.h"
#include "mm/vmm.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DEFAULT_STACK_SIZE PAGE_SIZE

static int available_id;

Process create_process(entryFn entry)
{
    Process ret = {
        .id = available_id++,
        .state = PROCESS_PAUSED,
        .eip = (void*)entry,
        .next = NULL,
        .stack0 = (uint8_t*)malloc(DEFAULT_STACK_SIZE) + DEFAULT_STACK_SIZE,
        .stack3 = (uint8_t*)malloc(DEFAULT_STACK_SIZE) + DEFAULT_STACK_SIZE,
        .pdir = vmm_copy_pdir(vmm_get_pdir())
    };
    return ret;
}