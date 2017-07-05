BITS 32

extern main

global _start

SECTION .text
_start:
	call main
	jmp $

global idt_load
idt_load:
	mov eax, [esp+4]
	lidt [eax]
	sti
	ret

global gdt_load
gdt_load:
	mov eax, [esp+4]
	lgdt [eax]
	ret

%macro define_isr_wrapper 1
global __%1
__%1:
	pusha
	push gs
	push fs
	push ds
	push es

	extern %1
	call %1

	pop es
	pop ds
	pop fs
	pop gs
	popa
	iret
%endmacro

define_isr_wrapper isr0
define_isr_wrapper isr4
define_isr_wrapper isr5
define_isr_wrapper isr6
define_isr_wrapper isr7
define_isr_wrapper isr8
define_isr_wrapper isr13
define_isr_wrapper isr_default
define_isr_wrapper isr_timer
define_isr_wrapper isr_keyboard
