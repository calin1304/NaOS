%macro DUMP_REG 1
	push ax
	mov ax, %1
	call debug_reg
	pop ax
%endmacro

%macro DUMP_ALL_REGS 0
	DUMP_REG ax
	DUMP_REG bx
	DUMP_REG cx
	DUMP_REG dx
%endmacro