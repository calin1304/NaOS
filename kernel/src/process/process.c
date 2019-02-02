#include "process/process.h"
#include "vfs.h"
#include "elf.h"
#include "mm/pmm.h"
#include "mm/vmm.h"
#include <libk/stdio.h>
#include <libk/string.h>
#include <libk/stdlib.h>

Process create_process(int id, entryFn entry)
{
    Process ret = {
        .id = id,
        .state = PROCESS_PAUSED,
        .eip = (void*)entry,
        .next = NULL
    };
    return ret;
}