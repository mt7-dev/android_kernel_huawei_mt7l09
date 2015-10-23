

#ifndef __M3BOOT_H__
#define __M3BOOT_H__

#include <product_config.h>
#include <ptable_com.h>
#include <soc_memmap.h>
#include <hi_uart.h>
#include <hi_nandc.h>

/******************** AXI MEM BASE & CPU BOOT ADDR *******************/
#define  RUN_BASE_ADDR_APPA9         FASTBOOT_DDR_ENTRY
#define  APPA9_FASTBOOT_BASE         (HI_NAND_MEM_BUFFER_ADDR+PTABLE_FASTBOOT_START)
#define  FASTBOOT_SIZE_IN_NAND       (APPA9_FASTBOOT_BASE + 0x24)

/******************************* m3 *********************************/
#define  M3_KERNEL_ENTRY             (HI_M3TCM0_MEM_ADDR + 4)
#define  M3_BOOT_LOOP_NEW_ADDR       (HI_M3TCM0_MEM_ADDR + 0xf000) /* 60KB */

#define  ALIGN( __size, __bits )      ( ( ( ( __size - 1 ) >> __bits ) + 1 ) << __bits )

/******************* uart print macro definition ********************/

    #define printUart(value)       \
            ldr    r1, =HI_UART0_REGBASE_ADDR ;\
            ldr    r2, =value      ;\
            str    r2, [r1,#UART_REGOFF_THR]

#endif /* __M3BOOT_H__ */
