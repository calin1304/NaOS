#include "kernel/include/syscalls.h"

#include <stdio.h>

void *syscalls[] = {
    syscall_puts,
    syscall_puts
};

void syscall_puts()
{
    char *s;
    __asm__ __volatile__ ("movl %%ebx, %0" : "=r"(s));
    puts(s);
}