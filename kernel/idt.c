#include "idt.h"

#include "console.h"
#include "pic.h"
#include "io.h"
#include "keyboard.h"
#include "clock.h"
#include "kernel/include/syscalls.h"

#include "libk/include/stdio.h"

extern void idt_load(struct IDTPtr *idt_ptr);

extern Console console;

extern void __isr_default();
extern void __isr0();
extern void __isr4();
extern void __isr5();
extern void __isr6();
extern void __isr7();
extern void __isr8();
extern void __isr13();
extern void __isr14();
extern void __int0x80();
extern void __isr_timer();
extern void __isr_keyboard();

void isr0()
{
    printf("[!] Exception 0: Division by zero\n");
    asm volatile ("cli");
    asm volatile ("hlt");
}

void isr4()
{
    printf("[!] Exception 4: Overflow\n");
    asm volatile ("cli");
    asm volatile ("hlt");
}

void isr5()
{
    printf("[!] Exception 5: Bound range exceded\n");
    asm volatile ("cli");
    asm volatile ("hlt");
}

void isr6()
{
    printf("[!] Exception 6: Invalid opcode\n");
    asm volatile ("cli");
    asm volatile ("hlt");
}

void isr7()
{
    printf("[!] Exception 7: Device not available\n");
    asm volatile ("cli");
    asm volatile ("hlt");
}

void isr8()
{
    printf("[!] Exception 8: Double fault\n");
    asm volatile ("cli");
    asm volatile ("hlt");
}

void isr13()
{
    printf("[!] Exception 13: General protection fault\n");
    asm volatile ("cli");
    asm volatile ("hlt");
}

void isr14(uint32_t errorCode)
{
    printf("[!] Exception 14: Page fault, with error code: %x", errorCode);    
    asm volatile ("cli");
    asm volatile ("hlt");
}

void isr_default()
{
}

void int0x80(uint32_t eax)
{
    void (*apicall)() = syscalls[eax];
    apicall();
}

extern Clock clock;

void isr_timer()
{
    // console_printf(&console, "Timer tick\n");
    clock.ticks += 1;
    if (clock.ticks % 100 == 0) {
        clock.seconds += 1;
    }
    // console_display_timer(&console);
    pic_ack(PIC1);
}

void isr_keyboard()
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
        idt_set_gate(i, (uint32_t)__isr_default,  0x8, 0x8e);
    }
    idt_set_gate(0,     (uint32_t)__isr0,           0x8, 0x8e);
    idt_set_gate(4,     (uint32_t)__isr4,           0x8, 0x8e);
    idt_set_gate(5,     (uint32_t)__isr5,           0x8, 0x8e);
    idt_set_gate(6,     (uint32_t)__isr6,           0x8, 0x8e);
    idt_set_gate(7,     (uint32_t)__isr7,           0x8, 0x8e);
    idt_set_gate(8,     (uint32_t)__isr8,           0x8, 0x8e);
    idt_set_gate(13,    (uint32_t)__isr13,          0x8, 0x8e);
    idt_set_gate(14,    (uint32_t)__isr14,          0x8, 0x8e);
    idt_set_gate(0x20,  (uint32_t)__isr_timer,      0x8, 0x8e);
    idt_set_gate(0x21,  (uint32_t)__isr_keyboard,   0x8, 0x8e);

    idt_set_gate(0x80,  (uint32_t)__int0x80,        0x8, 0xee);
    
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