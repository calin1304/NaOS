BITS 16
ORG 0x7c00

begin: jmp 0x0:_start

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
	xor eax, eax
	xor ebx, ebx
	xor ecx, ecx
	xor edx, edx
	xor esi, esi
	xor edi, edi
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	mov ebp, 0x7c00
	mov esp, 0x7c00
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
	mov dx, ds
	call readSectorExtended

loadRootDirectory:
	mov ax, 19
	mov bx, ROOT_BASE
	mov cx, ROOT_SECTOR_COUNT
	mov dx, ds
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
			add si, 2
			mov eax, DWORD [si]
			mov DWORD [stage2Size], eax
			mov ax, [currentCluster]
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
		mov dx, KERNEL_SEGMENT
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
	; call checkA20
	; cmp ax, 1
	; jz jumpToKernel
	call enableA20

	getMemoryMap:
		xor esi, esi
		xor ebx, ebx
		mov di, MEMORY_MAP_BUFFER
		.L1:
		mov eax, 0xe820
		mov edx, 0x534D4150
		mov ecx, 24
		int 0x15
		jc exit
		add di, 24
		inc si
		cmp ebx, 0
		jnz .L1

	jumpToKernel:
		cli
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
		mov esp, 0x7c00
		mov ebp, 0x7c00
		push esi
		push DWORD MEMORY_MAP_BUFFER
		push DWORD KERNEL_LONG_ADDR
		push DWORD [stage2Size]
		jmp 0x8:KERNEL_LONG_ADDR

exit:
	cli
	hlt	

stage2Filename: db "KER     BIN"
stage2Size:		dd 0
driveNumber: 	db 0

currentCluster:	dw 0

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

times 510 - ($-$$) db 0
bootsig: dw 0xaa55
