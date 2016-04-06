/**************************************************************************/
/* Configure memory regions */
MALLOC_SIZE = 1024;
MEMORY
{
  /* FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 64k */
  RAM  (rwx) : ORIGIN = 0x10000000, LENGTH = 63k
  RAM_HEAP (rwx) : ORIGIN = 0x1000FC00, LENGTH = 1024
}


/* Linker script to place sections and symbol values. Should be used together
 * with other linker script that defines memory regions FLASH and RAM.
 * It references following symbols, which must be defined in code:
 *   Reset_Handler : Entry of reset handler
 *
 * It defines following symbols, which code can use without definition:
 *   __exidx_start
 *   __exidx_end
 *   __etext
 *   __data_start__
 *   __preinit_array_start
 *   __preinit_array_end
 *   __init_array_start
 *   __init_array_end
 *   __fini_array_start
 *   __fini_array_end
 *   __data_end__
 *   __bss_start__
 *   __bss_end__
 *   __end__
 *   end
 *   __HeapLimit
 *   __StackLimit
 *   __StackTop
 *   __stack
 */
ENTRY(Reset_Handler)

SECTIONS
{
	.text :
	{
		__text_section_start = .;
		KEEP(*(.isr_vector))
		*(.text*)

		*(.rodata)
	} > RAM

	__etext = .;

	.data :
	{
		. = ALIGN(4);
		__data_start__ = .;
		*(vtable)
		*(.data*)
		__data_end__ = .;

	} > RAM

	.ram_heap :
	{
		__alloc_start__ = .;
	} > RAM_HEAP
	__alloc_end__ = __alloc_start__ + MALLOC_SIZE;

	.bss :
	{
		. = ALIGN(4);
		__bss_start__ = .;
		*(.bss*)
		*(COMMON)
		. = ALIGN(4);
		__bss_end__ = .;
	} > RAM
	__file_size = __data_end__ - __text_section_start;
	
	.heap (COPY):
	{
		__end__ = .;
		end = __end__;
		*(.heap*)
		__HeapLimit = .;
	} > RAM

	/* .stack_dummy section doesn't contains any symbols. It is only
	 * used for linker to calculate size of stack sections, and assign
	 * values to stack symbols later */
	.stack_dummy (COPY):
	{
		*(.stack*)
	} > RAM

	/* Set stack top to end of RAM, and stack limit move down by
	 * size of stack_dummy section */
	__StackTop = ORIGIN(RAM) + LENGTH(RAM);
	__StackLimit = __StackTop - SIZEOF(.stack_dummy);
	PROVIDE(__stack = __StackTop);

	/* Check if data + heap + stack exceeds RAM limit */
	ASSERT(__StackLimit >= __HeapLimit, "region RAM overflowed with stack")
}
/**************************************************************************/
