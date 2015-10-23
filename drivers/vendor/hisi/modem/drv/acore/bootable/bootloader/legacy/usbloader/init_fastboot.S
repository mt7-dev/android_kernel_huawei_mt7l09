/*************************************************************************
* Copyright(C) 2013 Hisilicon Tech.Co.,Ltd.
*         !!! Notice: This code is not open source.
*             It is confidential
**************************************************************************
* filename: bootsect.S
* History:
*
*************************************************************************/

#include <config.h>
#include <soc_onchiprom.h>
#include <hi_nandc.h>

    .global .__stext
    .section .text.FirstSector
    .globl Start
.__stext:
Start:
    b   start
    b  .
    b  .
    b  .
    b  .
    b  .
    b  .
    b  .
    b  .
    .long CFG_BOOTLOADER_SIZE
    .long 0x0
    .long 0x0 /*留给onchiprom使用，位置不能变*/

start:

    /* set little-endian */
    MRS    r3, spsr
    BIC    r3, r3, #0x200
    MSR    spsr_cxsf, r3

    MRS    r1, cpsr
    BIC	   r1, r1, #0x1f
    ORR	   r1, r1, #0xdf
    MSR	   cpsr, r1

    /* set stack */
    LDR	sp, =BOOTLOADER_STK_ADDR

    bl	__system_control_init

    /* bss clear zero start */
BSS_CLEAR_ZERO:
    MOV	r0, #BOOT_COLD
    LDR r1, =BSS_START
    LDR r2, =BSS_END
    MOV r3, #0x00
    MOV r4, #0x00
    MOV r5, #0x00
LOOP1:
    STMIA   r1!, {r3}
    CMP     r1,   r2
    BLO	    LOOP1

USB_BURN:
    LDR    r1, =OCR_AUTO_ENUM_FLAG_ADDR
    LDR    r2, =0
    LDR    r3, =AUTO_ENUM_FLAG_VALUE
    LDR    r2, [r1]
    CMP    r3,  r2
    BEQ    MAIN

    /* load flash partition table start */
#ifndef BSP_CONFIG_EDA
LOAD_PARTITION_TABLE:
    #ifndef NANDC_OOBLENGTH_8BITECCTYPE
    /* set oob length 32Byte, just effective for ecc type 8bit/512Byte */
    LDR    r1, =(HI_NANDC_REGBASE_ADDR + HI_NFC_OOB_SEL_OFFSET)
    LDR    r2, =0
    STR    r2, [r1]
    #endif
    /* prepare for read fastboot from flash */
    LDR    r1, =(PTABLE_NAND_OFFSET + NANDC_BUF_BASE_ADDR)
    LDR    r2, =SHM_MEM_PTABLE_ADDR
    LDR    r3, =SHM_MEM_PTABLE_SIZE
    ADD    r3,r3,r2
LOOP_PTABLE:
    LDMIA  r1!,{r4-r11}
    STMIA  r2!,{r4-r11}
    CMP    r2,r3
    BLO    LOOP_PTABLE
#endif

MAIN:
    /* call main */
    LDR r4, =_main
    mov pc, r4
    b .

	.balign	4
L_STACK_ADDR:
	.long	BOOTLOADER_STK_ADDR

__system_control_init:
    MRC    p15, 0, r1, c2, c0, 0
    BIC   r1, r1, #0xff
    MCR    p15, 0, r1, c2, c0, 0

    LDR    r1, =MMU_INIT_VALUE        /* Defined in mmuArmLib.h */
    MCR    p15, 0, r1, c1, c0, 0   /* Write to MMU CR */

    MOV    r1, #0                /* data SBZ */
    MCR    p15, 0, r1, c7, c10, 4    /* drain write-buffer */

    /* We only get here with CPU 0 */

    /* 这两句有效 */
    MCR   p15,0,r0,c7,c5,0        /* invalidate Icache to PoU (this core) */
    MCR   p15,0,r0,c7,c5,6        /* invalidate branch predictor (this core) */
    ISB

    MOV    r1, #0x1800
    MCR    p15, 0, r1, c1, c0, 0  /* enable the Icache and branch prediction */
    ISB

    /* disable L1 D-caches */
    MRC    p15, 0, r1, c1, c0, 0     /* Read control register into r1 */
    BIC    r1, r1, #0x4  			/* Clear L1 D-cache enable */
    MCR    p15, 0, r1, c1, c0, 0     /* Write control register */

    /* disable and clear I-cache */
    MRC    p15 , 0, r2, c1, c0, 0    /* Read control register */
    BIC    r2, r2, #0x1000   	    /* Disable I-cache */
    MCR    p15 , 0, r2, c1, c0, 0    /* Write control register */

    ISB

    /* set initial stack pointer so stack grows down from start of code */
    MOV    fp, #0                   /* initialize frame pointer */
    mov	pc, lr
