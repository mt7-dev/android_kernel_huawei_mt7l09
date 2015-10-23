SECTIONS
{
	ENTRY(main)
	. = 0x00;
	.text : {
		*(header)
		*(.text)
		*(.rodata)
		}
	.data : {
		*(.data)
		}
	.bss : {
		*(.bss)
		}
}