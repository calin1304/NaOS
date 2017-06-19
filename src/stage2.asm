BITS 16

extern main

global _start

jmp _start

gdt_start: 
   dq 0           
         
   dw 0xffff          
   dw 0          
   db 0             
   db 10011010b       
   db 11001111b          
   db 0             

   dw 0xffff          
   dw 0             
   db 0             
   db 10010010b       
   db 11001111b       
   db 0            
toc: 
   dw toc - gdt_start - 1
   dd gdt_start

_start:
	cli
	xor ax, ax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	mov esp, 0x7c00
	mov ebp, 0x7c00
	mov esi, gdt_start
	mov edi, gdt_start

	lgdt [toc]
	mov eax, cr0
	or eax, 1
	mov cr0, eax
	jmp 0x8:(protectedMode)

BITS 32
protectedMode:
	mov ax, 0x10
	mov ss, ax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
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
