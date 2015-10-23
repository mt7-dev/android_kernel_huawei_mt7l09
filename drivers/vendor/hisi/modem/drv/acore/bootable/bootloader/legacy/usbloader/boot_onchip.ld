

OUTPUT_ARCH(arm)
ENTRY(FirstSector)
SECTIONS
{
    BOOTLOADER_START = CFG_ONCHIP_FASTBOOT_ADDR;
    BOOTLOADER_STK_SIZE = 8192;
    BOOTLOADER_SIZE = 0;

    . = BOOTLOADER_START;
    .boot ALIGN(0): {
        *(.text.FirstSector)
    }

    .text 	ALIGN(4): {
        *(.text.*)
    }
	
    .data   ALIGN(4): {
        *(.data*)
        *(.rodata*)
		*(.got*)
    }
    RECOVERY_ENTRY = .;
    BOOTLOADER_TBSS = RECOVERY_ENTRY + CFG_PRODUCT_CFG_FLASH_RECOVERY_LEN;
	. = BOOTLOADER_TBSS;
    .bss    ALIGN(4): {
    	BOOTLOADER_BSS = .;
        *(.bss*)  *(COMMON)
    }
    BOOTLOADER_BSS_END = .;  
 
    . = ALIGN(4);    
    BOOTLOADER_STK_ADDR = . + BOOTLOADER_STK_SIZE;
    BOOTLOADER_HEAP_ADDR = BOOTLOADER_STK_ADDR + 16;
}


