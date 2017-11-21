#include <stdint.h>
#include <stddef.h>

// TODO: Remove duplicate definitions of elf32 data

#define EI_NIDENT 16

typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Word;
typedef uint32_t Elf32_Addr;
typedef uint32_t Elf32_Off;

typedef struct {
        unsigned char   e_ident[EI_NIDENT];
        Elf32_Half      e_type;
        Elf32_Half      e_machine;
        Elf32_Word      e_version;
        Elf32_Addr      e_entry;
        Elf32_Off       e_phoff;
        Elf32_Off       e_shoff;
        Elf32_Word      e_flags;
        Elf32_Half      e_ehsize;
        Elf32_Half      e_phentsize;
        Elf32_Half      e_phnum;
        Elf32_Half      e_shentsize;
        Elf32_Half      e_shnum;
        Elf32_Half      e_shstrndx;
} Elf32Header;

typedef struct {
	Elf32_Word	p_type;
	Elf32_Off	p_offset;
	Elf32_Addr	p_vaddr;
	Elf32_Addr	p_paddr;
	Elf32_Word	p_filesz;
	Elf32_Word	p_memsz;
	Elf32_Word	p_flags;
	Elf32_Word	p_align;
} Elf32ProgramHeader;

typedef struct {
    Elf32_Word sh_name;
    Elf32_Word sh_type;
    Elf32_Word sh_flags;
    Elf32_Word sh_addr;
    Elf32_Word sh_offset;
    Elf32_Word sh_size;
    Elf32_Word sh_link;
    Elf32_Word sh_info;
    Elf32_Word sh_addralign;
    Elf32_Word sh_entsize;
} Elf32SectionHeader;

const char * KERNEL_NAME = "kernel";

#define PT_LOAD 0x00000001

char* moveKernel(char *src)
{
    Elf32Header *eh = (Elf32Header*)src;
    Elf32ProgramHeader *ph = (Elf32ProgramHeader*)(src + eh->e_phoff);
    for (int i = 0; i < eh->e_phnum; ++i) {
        if (ph->p_type == PT_LOAD) {
            char *p = src + ph->p_offset;
            char *dst = (char*)ph->p_paddr;
            size_t count = ph->p_filesz;
            while (count--) {
                *(dst++) = *(p++);
            }
        }
        ++ph;
    }
    return eh->e_entry;
}

void main(int imageSize, char * imageStart, void * mmap, int mmapEntries)
{
    int fileTableImageOffset = *(((int*)(imageStart + imageSize))-1);
    char *fileTable = imageStart + fileTableImageOffset;
    int kernelOffset = -1;
    int kernelSize = -1;
    char *p = fileTable-1;
    char *imageEnd = imageStart + imageSize;
    while (p != imageEnd && kernelOffset == -1) {
        ++p;
        if (*p != '=') {
            continue;
        }
        char *q = p;
        while (*q != '\0') {
            --q;
        }
        ++q;
        int i;
        for (i = 0; q[i] != '='; ++i) {
            if (q[i] != KERNEL_NAME[i]) {
                break;
            }
        }
        if (q[i] == '=') {
            kernelOffset = *((int*)(q + i + 1));
            kernelSize = *((int*)(q + i + 1) + 1);
        }
        while (*q != '=') {
            ++q;
        }
        ++q;
        p = (char*)(((int*)q) + 2);
    }
    void *kmain = moveKernel(imageStart + kernelOffset);
    ((void (*)(void*, uint16_t))kmain)(mmap, mmapEntries);
    for(;;);
}