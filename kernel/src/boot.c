#include <stdint.h>

extern void kmain();

extern unsigned int _symbol_stack_top;

__attribute__((noreturn))
void _start()
{
    // Put stack top in ESP register
    __asm__ __volatile__ ("movl %0, %%esp" : : "c"(&_symbol_stack_top));

    // Get multiboot magic number from EAX
    uint32_t multiboot_magic;
    __asm__ ("movl %%eax, %0" : "=a"(multiboot_magic));

    // Get mulitboot info structure pointer from EBX
    void *multiboot_info;
    __asm__ ("movl %%ebx, %0" : "=b"(multiboot_info));

    // Call kernel
    kmain(multiboot_info, multiboot_magic);
    __builtin_unreachable();
}
