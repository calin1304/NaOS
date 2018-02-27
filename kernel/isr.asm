BITS 32

extern kmain

global _start

SECTION .text

%macro define_isr_wrapper 1
global __%1
__%1:
	cli
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
	sti
	iret
%endmacro

define_isr_wrapper isr0
define_isr_wrapper isr4
define_isr_wrapper isr5
define_isr_wrapper isr6
define_isr_wrapper isr7
define_isr_wrapper isr8
define_isr_wrapper isr13

global __isr14
__isr14:
	cli
	pop eax
	pusha
	push gs
	push fs
	push ds
	push es
	
	push eax
	extern isr14
	call isr14
	pop eax

	pop es
	pop ds
	pop fs
	pop gs
	popa
	sti
	iret
	
define_isr_wrapper isr_default
define_isr_wrapper isr_timer
define_isr_wrapper isr_keyboard

global __int0x80
__int0x80:
	cli
	push eax
	extern int0x80
	call int0x80
	add esp, 4
	sti
	iret