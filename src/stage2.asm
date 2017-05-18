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

; idt_descriptor:
; 	.m_baseLow 		dw interruptHandlers
; 	.m_selectior 	dw 0x8
; 	.m_reserved		db 0
; 	.m_flags		db 10001110b
; 	.m_baseHi		dw 0

; idt_ptr:
; 	.limit	dw $ - idt_descriptor - 1
; 	.base	dd idt_descriptor


; extern interrupt_1

; interruptHandlers:
; 	call interrupt_1

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

	; lidt [idt_ptr]
	
	call main
	jmp $
