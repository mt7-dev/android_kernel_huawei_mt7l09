
OUTPUT_ARCH(arm)
ENTRY(FirstSector)
SECTIONS
{
    BOOTLOADER_START = CFG_FASTBOOT_DDR_ENTRY;
    BOOTLOADER_STK_SIZE = 8192;

    . = BOOTLOADER_START;
    .boot ALIGN(0): {
        *(.text.FirstSector)
	. = ALIGN(4);		
    }

    .text ALIGN(4): {
        *(.text.*)
	. = ALIGN(4);
    }

    .data ALIGN(4): {
        *(.data*)
	. = ALIGN(4);
        *(.rodata*)
	. = ALIGN(4);
    }
    RECOVERY_ENTRY = .;
	
    .bss ALIGN(4):  {
    	BOOTLOADER_BSS = .;
        *(.bss*)
	. = ALIGN(4);
	*(COMMON)
	. = ALIGN(4);
    }
    BOOTLOADER_BSS_END = .;
     . = ALIGN(4);
     
    BOOTLOADER_SIZE = . - BOOTLOADER_START;
    BOOTLOADER_STK_ADDR = . + BOOTLOADER_STK_SIZE;  
    BOOTLOADER_HEAP_ADDR = BOOTLOADER_STK_ADDR;
}

