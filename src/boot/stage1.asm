BITS 16

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

%include "defines.asm"
%include "macros.asm"
%include "drive.asm"
%include "a20.asm"

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

DAP:
	db 0x10
	db 0
dap_sector_count	dw 1
dap_offset			dw 0x1000
dap_segment			dw 0x07c0
dap_lba				dq 0
	
_start:
	mov dl, 0x80
	mov [driveNumber], dl
	; Setting up data segment registers
	cli
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
	mov ebp, 0x7b00
	mov esp, 0x7b00
	sti
	cld

check_int13_extension_installed:
	mov ah, 0x41
	mov bx, 0x55aa
	mov dl, 0x80
	int 0x13
	jc exit
	
loadFAT:
	mov ax, 1
	mov bx, FAT_BASE
	mov cx, FAT_SECTOR_COUNT
	call readSectorExtended

loadRootDirectory:
	mov ax, 19
	mov bx, ROOT_BASE
	mov cx, ROOT_SECTOR_COUNT
	call readSectorExtended
	
	mov dx, ROOT_BASE + 13*512
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
		.next:
			dec cx
			add si, cx
			add si, 0x15
			cmp si, dx
			jg exit
			jmp findStage2		

loadKernel:
	or ax, ax
	jz exit
	mov di, KERNEL_OFFSET

	.loop:
		mov ax, [currentCluster]
		add ax, 31
		mov bx, di
		mov cx, 1
		call readSectorExtended
	.loop_increment:
		add di, 0x200
		mov bx, [currentCluster]
		call computeNextCluster
		mov ax, [currentCluster]
		cmp ax, 0x0FFF
		jge stage2Loaded
		jmp .loop

stage2Loaded:
	; .set_vga_video_mode:
	; 	xor ax, ax
	; 	mov al, 0x13
	; 	int 0x10
	call checkA20
	cmp ax, 1
	jz .jumpToKernel
	call enableA20
	.jumpToKernel:
		jmp 0x0:0x7e00

exit:
	cli
	hlt

debug_reg:
	pusha
	mov cx, 4
	mov dx, ax
	mov ah, 0x0e
	.loop:
		mov al, dh
		shl dx, 4
		shr al, 4
		mov bx, hextable
		xlat
		xor bx, bx
		int 0x10
		loop .loop
	mov al, ' '
	int 0x10
	popa
	ret
	

stage2Filename: db "STAGE2  BIN"
driveNumber: 	db 0

currentCluster:	dw 0
sector: 		db 7
cylinder: 		db 0
head:			db 1

hextable: db "0123456789ABCDEF"
	
times 510 - ($-$$) db 0
bootsig: dw 0xaa55
