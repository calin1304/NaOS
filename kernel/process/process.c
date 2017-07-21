#include "process/process.h"
#include "vfs.h"
#include "elf.h"
#include "mm/pmm.h"
#include "mm/vmm.h"
#include "libk/include/stdio.h"
#include "libk/include/string.h"
#include "libk/include/stdlib.h"

static int currentAvailablePID = 1;

#define PROCESS_PAUSED 0

int get_next_availablePID() 
{
    return currentAvailablePID++;
}

Process* createProcess(const char *path)
{
    FILE *f = fopen(path, "r");

    // Create new virtual space for process
    // PDirectory vspace;
    // memset(&vspace, 0, sizeof(PDirectory));
    // vmm_identity_map(&vspace, 0x00000000, 3);
    // printf("%x\n", &vspace);
    // for (int i = 0; i < 10; ++i) {
    //     printf("%x ", vspace.entries[i]);
    // }
    // printf("\n");
    // PTable *t = vspace.entries[0] & (~0xfff);
    // printf("%x\n", t);
    // for (int i = 0;i  < 10; ++i) {
    //     printf("%x ", t->entries[i]);
    // }
    
    // asm("movl %0, %%eax" : : "r"(&vspace));
    // asm ("cli\nhlt");
    // vmm_switch_pdirectory(&vspace);
    

    uint8_t *buffer = malloc(f->size);
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
    
    int (*entry)() = elfHeader->e_entry;

    Process *proc = pmm_alloc_block();
    vmm_map_page(proc, proc);

    Process *process = malloc(sizeof(Process));
    process->id = get_next_availablePID();
    process->state = PROCESS_PAUSED;
    // process->pdir = vspace;

    Thread *mainThread = process->threads;
    mainThread->parent = process;
    mainThread->state = THREAD_PAUSED;
    mainThread->entry = (void*)entry;

    for (int i = 0; i < elfHeader->e_shnum; ++i) {
        if (elfSectionHeader[i].sh_addr != 0) {
            if (vmm_vaddr_is_mapped(elfSectionHeader[i].sh_addr)) {
                vmm_free_vaddr_page((vaddr)elfSectionHeader[i].sh_addr);
            }
        }
    }
    // vmm_restore_pdirectory();

    return process;

    // void *processStack = pmm_alloc_block();
    // vmm_map_page(processStack, 0xffffff00);
    // asm volatile(
    //     "movl %0, %%ebp\n"
    //     "movl %1, %%esp\n"
    //     :
    //     : "r"(processStack), "r"(((uint32_t*)processStack)+1)
    // );

    // process.threads[0] = createThread(process, thread;
    // void *threadStack = vmm_alloc_page(); // Create new stack
    
    // threads[0] = createThread(process, threadStack, (uint8_t*)threadStack + DEFAULT_STACK_SIZE, /*kernelStack*/, THREAD_PAUSED);
}