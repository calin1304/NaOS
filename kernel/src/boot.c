#include <stdint.h>

void _start() __attribute__((noreturn));

extern void kmain();

extern unsigned int _symbol_stack_top;

void _start()
{
    register unsigned int stack __asm__ ("esp") = &_symbol_stack_top;
    uint32_t magic;
    void *mb;
    __asm__ __volatile__ ("mov %%eax, %0" : : "m"(magic)); /* Multiboot magic*/
    __asm__ __volatile__ ("mov %%ebx, %0" : : "m"(mb)); /* Multiboot structure ptr*/
    kmain(mb, magic);
    __builtin_unreachable();
}