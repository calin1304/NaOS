BITS 16

ROOT_OFFSET EQU 0x0200
FAT_OFFSET EQU 0x1e00

begin: jmp _start

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

# Write integer to screen
# dx -> integer to write
putInt:
	pusha
	mov bx, hexTable
	mov cx, 4
	.LOOP:
		mov al, dh
		shr al, 4
		xlat
		mov ah, 0x0e
		int 0x10
		shl dx, 4
		loop .LOOP
	mov al, 0xa
	call putChar
	mov al, 0xd
	call putChar
	popa
	ret
	
putChar:
	mov ah, 0x0e
	int 0x10
	ret
	
putString:
	mov ah, 0x0E
	xor bx, bx
	xor cx, cx
	.LOOP:
		lodsb
		or al, al
		jz .EXIT
		int 0x10
		jmp .LOOP
	.EXIT:
		ret

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
	
chslba:
	mov al, [cylinder]
	mul byte [bpbHeadsPerCylinder]
	add al, [head]
	mul byte [bpbSectorsPerTrack]
	add al, [sector]
	sub al, 1
	ret

resetFloppy:
	xor ax, ax
	mov  dl, [driveNumber]
	int 13h
	jc error
	ret

printCHS:
	movzx dx, [sector]
	call putInt
	movzx dx, [head]
	call putInt
	movzx dx, [cylinder]
	call putInt
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
		jmp error
	.done:
		pop cx
		ret

; bx has index of next entry in FAT
computeNextCluster:
	mov si, 0x1200
	xor cx, cx
	mov ax, 3
	mul bx
	shr ax, 1
	add si, ax
	mov dx, bx
	and dx, 1
	jz .even
	.odd:
		mov cl, [si]
		add si, 1
		mov ch, byte[si]
		shr cx, 4
		mov [currentCluster], cx
		jmp .done
	.even:
		movzx cx, byte[si]
		shl cx, 4
		add si, 1
		movzx bx, byte[si]
		or bx, 0x0f
		add cx, bx
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
	xor ax, ax
	mov ss, ax
	mov ebp, 0x7c00
	mov esp, 0x7c00
	sti

	cld

loadFAT:
	mov ax, 1
	call lbachs
	mov al, 1
	mov bx, 0x1200
	call readSector

loadRootDirectory:
	mov ax, 19
	call lbachs
	mov al, 1
	mov bx, 0x1000
	call readSector
	
	mov si, 0x1000
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

		.next: ; Should stop at end of root directory
			dec cx
			add si, cx
			add si, 0x15
			jmp findStage2		

loadKernel:
	or ax, ax
	jz exit
	mov di, 0x200

	.loop:
		mov ax, [currentCluster]
		add ax, 31
		call lbachs
		mov al, 1
		mov bx, di
		call readSector
	.loop_increment:
		mov bx, [currentCluster]
		call computeNextCluster
		mov ax, [currentCluster]
		cmp ax, 0x0FF8
		jg stage2Loaded
		add di, 0x200
		jmp .loop

stage2Loaded:
	jmp 0x0:0x7e00

exit:
	cli
	hlt

error:
	mov si, errorMsg
	call putString
	jmp exit

stage2Filename: db "STAGE2  BIN"
driveNumber: 	db 0

currentCluster:	dw 0
lba:			dw 0
sector: 		db 7
cylinder: 		db 0
head:			db 1
errorMsg:		db "Error", 0xa, 0xd, 0
hexTable:		db "0123456789ABCDEF"
	
times 510 - ($-$$) db 0
db 0x55
db 0xaa
