#define _ASMLANGUAGE
#include <balongv7r2/config.h>

.section .text.FirstSector

#define CFG_BOOT_SECOND_START     SECEND_TEXT
#define CFG_BOOT_FIRST_SIZE       .__firstsize
#define CFG_BOOT_SECOND_SIZE      .__Secondsize

.global loadfastboot
.global loadimge_end
/**************************************************************
*** load fastboot to ddr
****************************************************************/

loadfastboot:
    LDR  R1, =CFG_FASTBOOT_NAND_BASE
    LDR  R2, =CFG_BOOT_FIRST_SIZE
    ADD  R1, R1, R2
    
    LDR  R2, =CFG_BOOT_SECOND_START

    LDR  R3, =CFG_BOOT_SECOND_SIZE
    
    ADD  R3, R3, R1  

loadfastbootloop:
    LDMIA R1!, {R4-R11}
    STMIA R2!, {R4-R11}
    CMP   R1, R3
    BLO   loadfastbootloop	
	
    mov pc, lr
