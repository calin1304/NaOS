resetFloppy:
	xor ax, ax
	mov  dl, [driveNumber]
	int 13h
	jc exit
	ret

readSectorExtended:
	mov [dap_lba], ax 			; LBA address
	mov [dap_offset], bx		; Output data buffer offset
	mov [dap_sector_count], cx	; How many sectors to read
	mov [dap_segment], ds		; Output data buffer segment
	
	mov ah, 0x42
	mov dl, 0x80
	mov si, DAP
	int 0x13
	jc exit
	ret