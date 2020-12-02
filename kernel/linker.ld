ENTRY(_start)

/* Multiboot stuff */
magic = 0x1BADB002;
flags = 3;
checksum = -(magic + flags);

_symbol_KERNEL_START = 1M;
_symbol_HIGHER_START = 0xC0000000 + 0x100000;

SECTIONS
{
    . = _symbol_HIGHER_START;
	.text : AT(ADDR(.text) - 0xC0000000) ALIGN(4K)
	{
        LONG(magic);    /* MAGIC*/
        LONG(flags);    /* FLAGS */
        LONG(checksum); /* CHECKSUM */
		*(.text);
	}
	.rodata : AT(ADDR(.rodata) - 0xC0000000) ALIGN(4K)
	{
		*(.rodata);
	}
	.data : AT(ADDR(.data) - 0xC0000000) ALIGN(4K)
	{
		*(.data);
	}
	.bss : AT(ADDR(.bss) - 0xC0000000) ALIGN(4K)
	{
		*(COMMON);
		*(.bss);
        . += 4K;
        _symbol_stack_top = . - 0xC0000000;
	}
	_symbol_KERNEL_END = .;

    /DISCARD/ :
    {
        *(.eh_frame);
        *(.comment);
    }
}