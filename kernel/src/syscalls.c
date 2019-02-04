#include "syscalls.h"

#include <libk/stdio.h>
#include <idt.h>

void *syscalls[] = {
    syscall_puts,
    syscall_puts
};

void syscall_puts(syscall_frame_t *frame)
{
    puts(frame->ebx);
}