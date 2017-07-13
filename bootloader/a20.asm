; checkA20:
; 	push bx
; 	push si
; 	push es

; 	mov ax, 0xffff
; 	mov es, ax
; 	mov si, 0x7e07
	
; 	mov ax, [es:si]
; 	mov bx, [ds:bootsig]
; 	cmp ax, bx
; 	jnz .A20Enabled
	
; 	shl bx, 8
; 	mov [ds:bootsig], bx
; 	mov ax, [es:si]
; 	cmp ax, bx
; 	jnz .A20Enabled
	
; 	mov ax, 0
; 	jmp .return
; 	.A20Enabled:
; 		mov ax, 1
; 	.return:
; 	pop es
; 	pop si
; 	pop bx
;	ret

enableA20: ; Fast A20 gate
	push ax
	in al, 0x92
	or al, 2
	out 0x92, al
	pop ax
	ret