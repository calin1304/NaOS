ENTRY(_start)

magic = 0x1BADB002;
flags = 3;
checksum = -(magic + flags);

SECTIONS
{	. = 1M;
	_symbol_KERNEL_START = .;
    .multiboot ALIGN(4) :
    {
        LONG(magic)     /* MAGIC*/
        LONG(flags)     /* FLAGS */
        LONG(checksum)  /* CHECKSUM */
    }
    .bss ALIGN(16) :
    {
        _symbol_stack_bottom = .;
        src/boot.o(.bss); /* stack */
        . += 4K;
        _symbol_stack_top = .;
    }
    .text ALIGN(4K) :
    {
        src/boot.o(.text);
    }
	.text ALIGN(4K) :
	{
		*(.text);
	}
	.rodata ALIGN(4K) :
	{
		*(.rodata);
	}
	.data ALIGN(4K) :
	{
		*(.data);
	}
	.bss ALIGN(4K) :
	{
		*(COMMON);
		*(.bss);
	}
	_symbol_KERNEL_END = .;
}