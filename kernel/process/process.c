#include "process/process.h"
#include "vfs.h"
#include "elf.h"
#include "mm/pmm.h"
#include "mm/vmm.h"
#include <stdio.h>
#include <string.h>

int createProcess(const char *path)
{
    FILE *f = fopen(path, "r");

    // Create new virtual space for process
    PDirectory vspace;
    memset(&vspace, 0, sizeof(PDirectory));
    vmm_identity_map(&vspace, 0x00000000, 1);
    // vmm_switch_vspace(&vspace);

    uint8_t *buffer = pmm_alloc_block();
    vmm_map_page(buffer, buffer);
    fread(buffer, sizeof(uint8_t), f->size, f);
    Elf32Header *elfHeader = (Elf32Header*)buffer;
    // Elf32ProgramHeader *elfProgramHeader = buffer + elfHeader->e_phoff;
    Elf32SectionHeader *elfSectionHeader = (Elf32SectionHeader*)(buffer + elfHeader->e_shoff);
    
    for (int i = 0; i < elfHeader->e_shnum; ++i) {
        if (elfSectionHeader[i].sh_addr != 0) {
            if (!vmm_vaddr_is_mapped(elfSectionHeader[i].sh_addr)) {
                void *sec = pmm_alloc_block();
                vmm_map_page(sec, (void*)elfSectionHeader[i].sh_addr);
            }
            uint8_t *dat = buffer + elfSectionHeader[i].sh_offset;
            memcpy(elfSectionHeader[i].sh_addr, dat, elfSectionHeader[i].sh_size);
        }
    }
    // vmm_free_vaddr_page(buffer);
    int (*entry)() = elfHeader->e_entry;
    entry();
    return 0;

    // void *processStack = pmm_alloc_block();
    // vmm_map_page(processStack, 0xffffff00);
    // asm volatile(
    //     "movl %0, %%ebp\n"
    //     "movl %1, %%esp\n"
    //     :
    //     : "r"(processStack), "r"(((uint32_t*)processStack)+1)
    // );

    // Process *process = pmm_alloc_block();
    // vmm_map_page(process, process);
    // process->id = get_next_availablePID();
    // process->state = PROCESS_PAUSED;
    // process->pdir = vspace;

    // process.threads[0] = createThread(process, thread;
    // void *threadStack = vmm_alloc_page(); // Create new stack
    
    // threads[0] = createThread(process, threadStack, (uint8_t*)threadStack + DEFAULT_STACK_SIZE, /*kernelStack*/, THREAD_PAUSED);
}