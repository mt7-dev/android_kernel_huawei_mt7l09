/*************************************************************************
*   版权所有(C) 1987-2004, 深圳华为技术有限公司.
*
*   文 件 名 :  sdio_slave.h
*
*   作    者 :  xumushui
*
*   描    述 :  slave模块寄存器定义
*
*   修改记录 :  2011年8月27日  v1.00  xumushui      创建
*
*************************************************************************/
#ifndef _SDIO_SLAVE_H
#define _SDIO_SLAVE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <linux/kernel.h>
#include <linux/string.h>
#include "drv_comm.h"

//*********Definition of Data Types used*************
typedef unsigned char BYTE;   //8-bit data type
typedef unsigned int  WORD;   //16-bit data type
typedef unsigned long DWORD;  //32-bit data type	

#define PRODUCT_CFG_ASIC_TYPE_V7R1_PILOT
/*define fun0 registers*/
#define CLK_WAKE_UP                 0x0000
#define ESW_CCCR_REG                0x0004
#define ADMA_SYS_ADDR               0x0008
#define ESW_CARD_RDY                0x000C
#define ESW_FUN_RDY                 0x0010
#define AHB_FUN0_INT_ENABLE         0x0014   
#define AHB_FUN0_INT_STATUS         0x0018
#define SOFT_RESET_VALID            0x001C
#define AHB_MEM_INT_ENABLE          0x0020  
#define AHB_MEM_INT_STATUS          0x0024
#define GLOBAL_INT_STATUS_ENABLE    0x0028  
#define GLOBAL_INT_STATUS           0x002C
#define CSA_POINTER                 0x0030
#define IO_ACCESS_MODE              0x0034
#define UHS_SUPPORT                 0x0038
#define CLK_DELAY_TIMER             0x003c
#define POWER_CONTROL               0x0040
#define POWER_STATE                 0x0044

/*define fun1 registers*/
#define FUN1_DATA_PORT              0x0100
#define ESW_OCR_REG                 0x0104
//#define SD_INT_IDENT_REG          //0x0108 accessed by sd host only
//#define SD_INT_ENABLE             //accessed by sd host only
#define ARASAN_SDIO_F1_INT_ENABLE      0x9

#define AHB_TRANS_CNT               0x010C   
#define SDIO_TRANS_CNT              0x0110   
#define AHB_FUN1_INT_STATUS         0x0114  
#define AHB_FUN1_INT_ENABLE         0x0118
#define ESW_FBR_REG                 0x011C
#define ESW_IOR_REG                 0x0120
#define HOST_GENERAL_PURPOSE        0x0124
#define ARM_GENERAL_PURPOSE         0x0128
#define FUN1_RD_DATA_RDY            0x012C
#define BLOCK_SIZE_REG              0x0244
#define ARGUMENT_REG                0x0248

/* ARGUMENT Register field */
#define ARGUMENT_BITS_COUNT         (0x000001FF)
#define ARGUMENT_BITS_REGOFS        (0x03FFFE00)
#define ARGUMENT_BITS_OP_MODE       (0x04000000)
#define ARGUMENT_BITS_BLK_MODE      (0x08000000)
#define ARGUMENT_BITS_FUN_NO        (0x70000000)
#define ARGUMENT_BITS_RW_FLAG       (0x80000000)

#define PROG_REG_FUNCTION_RDY		0x0001
#define PROG_REG_FUN1_DATA_RDY		0x0002
/*define CCCR configuration*/
#define PROG_REG_CCCR_VER           0x00000F
#define PROG_REG_SDIO_VER           0x0000F0
#define PROG_REG_SD_VER             0x000F00
#define PROG_REG_SCSI				0x001000    
#define PROG_REG_SDC				0x002000
#define PROG_REG_SMB				0x004000
#define PROG_REG_SRW				0x008000
#define PROG_REG_SBS				0x010000
#define PROG_REG_S4MI				0x020000
#define PROG_REG_LSC				0x040000
#define PROG_REG_4BLS				0x080000
#define PROG_REG_MEM_PRE            0xC00000
#define PROG_REG_CMD_INX            0x3F0000

/* Program Register field */
#define PROG_REG_CARD_RDY			0x00000001
#define PROG_REG_ESW_OCR            0x00ff8000
#define PROG_REG_ESW_FBR            0x00000017  
#define PROG_FUN1_IOR_RDY           0x00000001  
#define PROG_FUN_RDY_BIT            0x00000001	
#define PROG_REG_ESW_CCCR_30        0x00B3F343
#define PROG_REG_ESW_CCCR_20        0x00B3F232
#define PROG_FBR_CSA_SUP            0x00000010
#define PROG_FBR_SPS_SUP            0x00002000
#define PROG_ESW_CCCR_SHS           0x00200000      /*high speed support*/

/* Definitions for Interrupt Enable/Status Register fields　*/
#define INT_EN_REG_WR_TRAN_OVER		0x00000001
#define INT_EN_REG_RD_TRAN_OVER		0x00000002
#define INT_EN_REG_RD_TRAN_START   	0x00000040
#define INT_EN_REG_WR_TRAN_START    0x00000080
#define INT_EN_REG_RD_ERR			0x00000004
#define INT_EN_REG_RST              0x00000008
#define INT_EN_REG_ADMA_END         0x00000100
#define INT_EN_REG_ADMA             0x00000800 
#define INT_STATUS_CLEAR_ALL		0xFFFFFFF8

/* argument register [25:9]: 17 bits REG ADDR of CMD53 */
#define REG_ADDR_MASK               0x3FFFE00 
#define CIS0_ADDR_VALUE             0x0001000  
#define CIS1_ADDR_VALUE             0x0002000
#define CSA_ADDR_VALUE              0x000010f
#define FUN1_DATA_PORT_VALUE        0x0000000

/* global int enable/status register fields */
#define GLOBAL_INT_FROM_FUN0        (0x00000001)
#define GLOBAL_INT_FROM_MEM         (0x00000002)
#define GLOBAL_INT_FROM_FN1         (0x00000004)
#define GLOBAL_INT_BITS_ALL         (0x00000007)
#define GLOBAL_INT_MASK             (0x00000007)

/* fun0 int enable/status register fields */
#define FUN0_INT_AHBSOFT_RST        (0x00000001)
#define FUN0_INT_VOL_SWITCH_CMD     (0x00000002)
#define FUN0_INT_CMD19_RD_START     (0x00000004)
#define FUN0_INT_CMD19_RD_TRANSOVER (0x00000008)
#define FUN0_INT_WR_START           (0x00000010)
#define FUN0_INT_WR_TRANSOVER       (0x00000020)
#define FUN0_INT_RD_START           (0x00000040)
#define FUN0_INT_RD_TRANSOVER       (0x00000080)
#define FUN0_INT_RD_TRANSERR        (0x00000100)
#define FUN0_INT_ADMA_END           (0x00000200)
#define FUN0_INT_ADMA               (0x00000400)
#define FUN0_INT_ADMA_ERR           (0x00000800)
#define FUN0_INT_BITS_ALL           (0x00000FFF)
#define FUN0_INT_MASK               (0x00000FFF)

/* fun1 int enable/status register fields */
#define FUN1_INT_WR_OVER            (0x00000001)
#define FUN1_INT_RD_OVER            (0x00000002)
#define FUN1_INT_RD_ERR             (0x00000004)
#define FUN1_INT_RST                (0x00000008)
#define FUN1_INT_HOSTMSG_RDY        (0x00000010)
#define FUN1_INT_ACKTOARM           (0x00000020)
#define FUN1_INT_SDIO_RD_START      (0x00000040)
#define FUN1_INT_SDIO_WR_START      (0x00000080)
#define FUN1_INT_ADMA_END           (0x00000100)
#define FUN1_INT_SUSPEND            (0x00000200)
#define FUN1_INT_RESUME             (0x00000400)
#define FUN1_INT_ADMA               (0x00000800)
#define FUN1_INT_ADMA_ERR           (0x00001000)
#define FUN1_INT_FUN1_EN            (0x00002000)
#define FUN1_INT_BITS_ALL           (0x00003FFF)
#define FUN1_INT_MASK               (0x00003FFF)

/* mem int enable/status register fields */
#define MEM_INT_MASK                (0x0000FFFF)

/*Timeout for initialization process is 5 mili sec*/
#define TIME_OUT_VALUE              0x30d40  
#ifdef PRODUCT_CFG_ASIC_TYPE_V7R1_PILOT
#define IO_SPEED_MODE               0x00047
#elif defined (PRODUCT_CFG_ASIC_TYPE_V7R1)
#define IO_SPEED_MODE               0x0004f
#else
#error 
#endif
#define OCR_REG_VOLTAGE_RANGE		0x00FF8000	
#define CARD_RDY_TIMEOUT            100

#ifdef __cplusplus
}
#endif

#endif

