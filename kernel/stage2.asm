BITS 32

extern kmain

global _start

SECTION .text
_start:
	call kmain
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

global __isr14
__isr14:
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
	iret
	
define_isr_wrapper isr_default
define_isr_wrapper isr_timer
define_isr_wrapper isr_keyboard

global __int0x80
__int0x80:
	push ebx
	push eax
	extern int0x80
	call int0x80
	pop eax
	pop ebx

	iret

global enablePaging
enablePaging:
	mov eax, cr0
	or eax, 0x80000001
	mov cr0, eax
	ret
	
global loadPDBR
loadPDBR:
	mov eax, [esp+4];
	mov cr3, eax
	ret

global vm_flush_tlb_page
vm_flush_tlb_page:
	cli
	invlpg [esp+4]
	sti
	ret