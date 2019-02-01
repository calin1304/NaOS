#include "idt.h"

#include "console.h"
#include "pic.h"
#include "io.h"
#include "keyboard.h"
#include "clock.h"
#include "syscalls.h"

#include <libk/stdio.h>

#ifdef __x86_64__
typedef unsigned long long int uword_t;
#else
typedef unsigned int uword_t;
#endif

struct interrupt_frame;

static void idt_load(struct IDTPtr *idt_ptr)
{
    __asm__ __volatile__(
        "lidt (%0)\n"
        "sti" : : "r"(idt_ptr)
    );
}

extern Console console;

extern void __int0x80();
extern void __isr_timer();
extern void __isr_keyboard();

void __attribute__((interrupt)) isr0(struct interrupt_frame *frame)
{
    printf("[!] Exception 0: Division by zero\n");
    __asm__ __volatile__ ("cli");
    __asm__ __volatile__ ("hlt");
}

void __attribute__((interrupt)) isr4(struct interrupt_frame *frame)
{
    printf("[!] Exception 4: Overflow\n");
    __asm__ __volatile__ ("cli");
    __asm__ __volatile__ ("hlt");
}

void __attribute__((interrupt)) isr5(struct interrupt_frame *frame)
{
    printf("[!] Exception 5: Bound range exceded\n");
    __asm__ __volatile__ ("cli");
    __asm__ __volatile__ ("hlt");
}

void __attribute__((interrupt)) isr6(struct interrupt_frame *frame)
{
    printf("[!] Exception 6: Invalid opcode\n");
    __asm__ __volatile__ ("cli");
    __asm__ __volatile__ ("hlt");
}

void __attribute__((interrupt)) isr7(struct interrupt_frame *frame)
{
    printf("[!] Exception 7: Device not available\n");
    __asm__ __volatile__ ("cli");
    __asm__ __volatile__ ("hlt");
}

void __attribute__((interrupt)) isr8(struct interrupt_frame *frame)
{
    printf("[!] Exception 8: Double fault\n");
    __asm__ __volatile__ ("cli");
    __asm__ __volatile__ ("hlt");
}

void __attribute__((interrupt)) isr13(struct interrupt_frame *frame)
{
    printf("[!] Exception 13: General protection fault\n");
    __asm__ __volatile__ ("cli");
    __asm__ __volatile__ ("hlt");
}

void __attribute__((interrupt)) isr14(struct interrupt_frame *frame, uword_t errorCode)
{
    printf("[!] Exception 14 - Page fault - ");
    
    if (errorCode & 0x1) {
        printf("Page-protection violation ");
    } else {
        printf("Non-present page ");
    }
    if (errorCode & 0x2) {
        printf("on page write ");
    } else {
        printf("on page read ");
    }
    if (errorCode & 0x4) {
        printf("while CPL = 3 ");
    }
    if (errorCode & 0x8) {
        printf("caused by reading 1 in reserved field ");
    }
    if (errorCode & 0x10) {
        printf("Caused by instructon fetch");
    }
    uint32_t faultAddr;
    // __asm__ __volatile__("movl %cr2, %eax");
    __asm__ __volatile__("movl %%cr2, %0" : "=r"(faultAddr));
    printf("\nAddress: %x\n", faultAddr);
    __asm__ __volatile__ ("cli");
    __asm__ __volatile__ ("hlt");
}

void __attribute__((interrupt)) isr_default(struct interrupt_frame *frame)
{
}

void int0x80(uint32_t eax)
{
    void (*apicall)() = syscalls[eax];
    apicall();
}

extern clock_t clock;

void __attribute__((interrupt)) isr_timer(struct interrupt_frame *frame)
{
    // console_printf(&console, "Timer tick\n");
    clock.ticks += 1;
    if (clock.ticks % 100 == 0) {
        clock.seconds += 1;
    }
    // console_display_timer(&console);
    pic_ack(PIC1);
}

void __attribute__((interrupt)) isr_keyboard(struct interrupt_frame *frame)
{    
    uint8_t scancode = inb(0x60);
    
    if (scancode & 0x80) {
        /* You can use this one to see if the user released the
        *  shift, alt, or control keys... */
    } else {
        char c = keyboardUS[scancode];
        console_put_char(&console, c);
    }
    pic_ack(PIC1);
}

void idt_init() 
{
    for (int i = 0; i < 256; ++i) {
        idt_set_gate(i, (uint32_t)isr_default,  0x8, 0x8e);
    }
    idt_set_gate(0,     (uint32_t)isr0,           0x8, 0x8e);
    idt_set_gate(4,     (uint32_t)isr4,           0x8, 0x8e);
    idt_set_gate(5,     (uint32_t)isr5,           0x8, 0x8e);
    idt_set_gate(6,     (uint32_t)isr6,           0x8, 0x8e);
    idt_set_gate(7,     (uint32_t)isr7,           0x8, 0x8e);
    idt_set_gate(8,     (uint32_t)isr8,           0x8, 0x8e);
    idt_set_gate(13,    (uint32_t)isr13,          0x8, 0x8e);
    idt_set_gate(14,    (uint32_t)isr14,          0x8, 0x8e);
    idt_set_gate(0x20,  (uint32_t)isr_timer,      0x8, 0x8e);
    idt_set_gate(0x21,  (uint32_t)isr_keyboard,   0x8, 0x8e);
    
    idt_install();
}

void idt_install()
{
    idtp.limit = sizeof(struct IDTEntry) * 256 - 1;
    idtp.base = (uint32_t)&idt;
    idt_load(&idtp);
}

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags)
{
    idt[num].flags = flags;
    idt[num].selector = sel;
    idt[num].baseLo = (base & 0x0000ffff);
    idt[num].baseHi = (base & 0xffff0000) >> 16;
}

void* idt_get_gate(uint8_t num)
{
    uint32_t ret;
    ret = idt[num].baseLo | (idt[num].baseHi << 16);
    return (void*)ret;
}