/*************************************************************************
*   版权所有(C) 1987-2020, 深圳华为技术有限公司.
*
*   文 件 名 :  config.h
*
*   作    者 :  wuzechun
*
*   描    述 :  TCM空间划分
*
*   修改记录 :  2011年6月9日  v1.00  wuzechun  创建
*
*************************************************************************/

#ifndef __CONFIG_H__
#define __CONFIG_H__

/***********************************************************************
 | OnChipRom|
***********************************************************************/

#define ONCHIPROM_VER       "OnChipRom Ver 1.0"

#define M3_TCM_ADDR                    (0x10000000)	/*TCM起始地址*/
#define M3_TCM_SIZE                    (64*1024)    /* 64K */

#define SRAM_ADDR                      (0x4FE00000)     /* AXI Memory起始地址 */
#define SRAM_SIZE                      (128*1024)       /* AXI Memory大小 */

#define BOOTLOAD_SIZE_MAX       (60*1024)      /* BootLoader最大长度 */

/* BootLoader或RAM初始化程序在TCM上保存位置 */
#define M3_TCM_BL_ADDR     M3_TCM_ADDR
#define BOOT_RST_ADDR_OFFEST 0x04       /*存储boot启动地址的地址偏移*/
#define BOOT_RST_ADDR_PP     (M3_TCM_ADDR + BOOT_RST_ADDR_OFFEST)  /*thumb指令，寻址要求地址必须为基数*/


/* 共享数据区放在SRAM顶端,BootLoader启动后需要拷贝走 */
#define M3_SRAM_SHARE_DATA_SIZE sizeof(tOcrShareData)
#define M3_SRAM_SHARE_DATA_ADDR (SRAM_ADDR + SRAM_SIZE - M3_SRAM_SHARE_DATA_SIZE)

/* OnChipRom运行过标志和自举标志都在tOcrShareData结构体内 */
#define OCR_INITED_FLAG_VALUE	0x23456789

#define AUTO_ENUM_FLAG_VALUE   0x82822828

#endif /* config.h */

