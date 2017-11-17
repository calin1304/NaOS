#include "process/process.h"
#include "vfs.h"
#include "elf.h"
#include "mm/pmm.h"
#include "mm/vmm.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int currentAvailablePID = 1;

int get_next_availablePID() 
{
    return currentAvailablePID++;
}

Process* createProcess(const char *path)
{
    FILE *f = fopen(path, "r");

    Process *process = malloc(sizeof(Process));
    process->id = get_next_availablePID();
    process->state = PROCESS_PAUSED;
    process->pdir = pmm_alloc_block();
    vmm_map_page(process->pdir, process->pdir);
    memset(process->pdir, 0, sizeof(PDirectory));
    vmm_identity_map(process->pdir, 0x0, 1024);
    
    uint8_t *buffer = malloc(f->size);
    fread(buffer, sizeof(uint8_t), f->size, f);
    Elf32Header *elfHeader = (Elf32Header*)buffer;
    Elf32ProgramHeader *elfProgramHeader = buffer + elfHeader->e_phoff;
    Elf32SectionHeader *elfSectionHeader = (Elf32SectionHeader*)(buffer + elfHeader->e_shoff);
    
    for (int i = 0; i < elfHeader->e_shnum; ++i) {
        if (elfSectionHeader[i].sh_addr != 0) {
            uint8_t *sec;
            if (!pdir_vaddr_is_mapped(process->pdir, elfSectionHeader[i].sh_addr)) {
                 sec = pmm_alloc_block();
                pdir_map_page(process->pdir, sec, (void*)elfSectionHeader[i].sh_addr);
            } else {
                sec = (uint8_t*)pdir_get_paddr(process->pdir, elfSectionHeader[i].sh_addr);
            }
            uint8_t *dat = buffer + elfSectionHeader[i].sh_offset;
            memcpy(sec + (elfSectionHeader[i].sh_addr % PAGE_SIZE), dat, elfSectionHeader[i].sh_size);
        }
    }
    
    int (*entry)() = elfHeader->e_entry;

    Thread *mainThread = process->threads;
    *mainThread = createThread(process, entry, THREAD_PRIORITY_HIGH, THREAD_PAUSED);
    return process;
}