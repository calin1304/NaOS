#include <stdint.h>

void _start() __attribute__((noreturn));

extern void kmain();

extern unsigned int _symbol_stack_top;

static uint32_t pdir[1024] __attribute__((aligned(4096))) = {0};
static uint32_t ptable[1024] __attribute__((aligned(4096))) = {0};

void _start()
{
    __asm__ ("lea %0, %%esp" : : "m"(_symbol_stack_top));
    uint32_t magic;
    void *mb;
    __asm__ __volatile__ ("mov %%eax, %0" : : "m"(magic)); /* Multiboot magic*/
    __asm__ __volatile__ ("mov %%ebx, %0" : : "m"(mb));    /* Multiboot structure ptr*/

    /* Map 3G virtual to 1M physical */
    *((char*)pdir - 0xc0000000) = (((uintptr_t)(&ptable - 0xc0000000) & (~0xfff)) << 12) | 0x3;
    for (int i = 0; i < 1024; ++i) {
        *((char*)ptable - 0xc0000000) = (((i * 4096) & (~0xfff)) << 12) | 0x3;
    }
    /* enable paging */
    __asm__ __volatile__("mov %0, %%cr3" : : "r"((uintptr_t)&pdir - 0xc0000000));
    __asm__ __volatile__("mov %cr0, %eax\n\t"
                         "or $0x80000001, %eax\n\t"
                         "mov %eax, %cr0");
    kmain(mb, magic);
    __builtin_unreachable();
}