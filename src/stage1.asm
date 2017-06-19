BITS 16

ROOT_BASE EQU 0x2500
ROOT_SECTOR_COUNT EQU 13
FAT_BASE EQU 0x3f00
FAT_SECTOR_COUNT EQU 9
KERNEL_OFFSET EQU 0x200

begin: jmp 0x07c0:_start

times 0xb - ($ - begin) db 0

bpbBytesPerSector		dw 512
bpbSectorsPerCluster	db 1
bpbReservedSectors		dw 1
bpbNumberOfFATs			db 2
bpbRootEntries			dw 224
bpbTotalSectors			dw 2880
bpbMediaDescriptor		db 0xF8
bpbSectorsPerFAT		dw 9
bpbSectorsPerTrack		dw 18
bpbHeadsPerCylinder		dw 2
bpbHiddenSectors		dw 0

; Convert from LBA to CHS
; ax -> LBA value
lbachs:
	xor dx, dx
	div word [bpbSectorsPerTrack]
	inc dl
	mov [sector], dl
	
	xor dx, dx
	div word [bpbHeadsPerCylinder]
	mov [head], dl
	mov [cylinder], al
	ret
	
; chslba:
; 	mov al, [cylinder]
; 	mul byte [bpbHeadsPerCylinder]
; 	add al, [head]
; 	mul byte [bpbSectorsPerTrack]
; 	add al, [sector]
; 	sub al, 1
; 	ret

resetFloppy:
	xor ax, ax
	mov  dl, [driveNumber]
	int 13h
	jc exit
	ret

; Read sectors from diskette
; al -> number of sectors to read
; bx -> buffer offset from es
readSector:
	mov cx, 5
	push cx
	.retry:
		mov ah, 02h
		mov ch, [cylinder]
		mov cl, [sector]
		mov dh, [head]
		mov dl, [driveNumber]
		int 13h
		jnc .done
		call resetFloppy
		pop cx
		dec cx
		push cx
		loop .retry
		jmp exit
	.done:
		pop cx
		ret

; bx has index of next entry in FAT
computeNextCluster:
	mov si, FAT_BASE
	xor cx, cx
	mov ax, 3
	mul bl
	shr ax, 1
	add si, ax
	and bx, 1
	jz .even
	.odd:
		mov cx, word[si]
		shr cx, 4
		mov [currentCluster], cx
		jmp .done
	.even:
		mov cx, word[si]
		and ch, 0x0f
		mov [currentCluster], cx
	.done:
		ret
	
_start:
	; Setting up data segment registers
	cli
	mov [driveNumber], dl
	mov ax, 0x07c0
	mov ds, ax
	mov es, ax
	
	; Setting up the stack
	xor eax, eax
	xor ebx, ebx
	xor ecx, ecx
	xor edx, edx
	xor esi, esi
	xor edi, edi
	mov ss, ax
	mov ebp, 0x7c00
	mov esp, 0x7c00
	sti
	cld

loadFAT:
	mov ax, 1
	call lbachs
	mov al, FAT_SECTOR_COUNT
	mov bx, FAT_BASE
	call readSector

loadRootDirectory:
	mov ax, 19
	call lbachs
	mov al, ROOT_SECTOR_COUNT
	mov bx, ROOT_BASE
	call readSector
	
	mov si, ROOT_BASE
	findStage2:
		mov di, stage2Filename
		mov cx, 11
		.loop:
			cmpsb
			jnz .next
			loop .loop
		.computeStage2StartCluster: 
			add si, 0xf
			mov ax, [si]
			mov [currentCluster], ax
			jmp loadKernel

		.next: ; FIXME: Should stop at end of root directory
			dec cx
			add si, cx
			add si, 0x15
			jmp findStage2		

loadKernel:
	or ax, ax
	jz exit
	mov di, KERNEL_OFFSET

	.loop:
		mov ax, [currentCluster]
		add ax, 31
		call lbachs
		mov al, 1
		mov bx, di
		call readSector
	.loop_increment:
		add di, 0x200
		mov bx, [currentCluster]
		call computeNextCluster
		mov ax, [currentCluster]
		cmp ax, 0x0FFF
		jge stage2Loaded
		jmp .loop

stage2Loaded:
	.set_vga_video_mode:
		xor ax, ax
		mov al, 0x13
		int 0x10
	jmp 0x0:0x7e00

exit:
	cli
	hlt

; debug_reg:
; 	push ax
; 	push bx
; 	push dx
; 	push cx

; 	mov cx, 4
; 	mov dx, ax
; 	mov ah, 0x0e
; 	.loop:
; 		mov al, dh
; 		shl dx, 4
; 		shr al, 4
		
; 		mov bx, hextable
; 		xlat
	
; 		xor bx, bx
; 		int 0x10
; 		loop .loop

; 	mov al, ' '
; 	int 0x10

; 	pop cx
; 	pop dx
; 	pop bx
; 	pop ax
; 	ret
	

stage2Filename: db "STAGE2  BIN"
driveNumber: 	db 0

currentCluster:	dw 0
sector: 		db 7
cylinder: 		db 0
head:			db 1

; hextable: db "0123456789ABCDEF"
	
times 510 - ($-$$) db 0
db 0x55
db 0xaa
