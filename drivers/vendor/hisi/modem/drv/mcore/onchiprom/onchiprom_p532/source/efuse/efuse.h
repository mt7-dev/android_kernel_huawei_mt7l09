/*************************************************************************
*   版权所有(C) 1987-2020, 深圳华为技术有限公司.
*
*   文 件 名 :  efuse.h
*
*   作    者 :  wuzechun
*
*   描    述 :  efuse.c 的头文件
*
*   修改记录 :  2011年6月9日  v1.00  wuzechun  创建
*
*************************************************************************/

#ifndef __EFUSE_H__
#define __EFUSE_H__

#include "hi_efuse.h"

#ifdef __cplusplus
extern "C" {
#endif


/***************************** efuse register *****************************/

#define EFUSE_BASE_REG  HI_EFUSE_BASE_ADDR

#define HI_APB_CLK      (25000000)
#define EFUSE_COUNT_CFG (5)
#define PGM_COUNT_CFG   (HI_APB_CLK / 1000000 * 12 - EFUSE_COUNT_CFG)

#define EFUSEC_CFG       (EFUSE_BASE_REG+0x0)
#define EFUSEC_PG_EN     (1<<0)
#define EFUSEC_PRE_PG    (1<<1)
#define EFUSEC_RD_EN     (1<<2)
#define EFUSEC_APB     (1<<3)
#define EFUSEC_SIG_SEL_INNER     (1<<3)

#define EFUSEC_STATUS   (EFUSE_BASE_REG+0x04)
#define EFUSEC_PG_STATUS (1<<0)
#define EFUSEC_RD_STATUS (1<<1)
#define EFUSEC_PRE_PG_FIN (1<<2)

#define EFUSE_GROUP    (EFUSE_BASE_REG+0x08)
#define EFUSE_GP_MASK   ((1<<7)-1)

#define EFUSE_PG_VALUE    (EFUSE_BASE_REG+0x0C)

#define EFUSEC_COUNT    (EFUSE_BASE_REG+0x10)
#define EFUSE_COUNT_MASK   ((1<<8)-1)

#define EFUSEC_PGM_COUNT    (EFUSE_BASE_REG+0x14)
#define EFUSE_PGM_COUNT_MASK   ((1<<16)-1)

#define EFUSEC_DATA     (EFUSE_BASE_REG+0x18)

/* EFUSE未烧写时默认值为0 */
#define EFUSEC_HW_CFG       (EFUSE_BASE_REG+0x1C)
/* 烧写控制 */
#define EFUSEC_PG_CTRL_POS      0
#define EFUSEC_PG_CTRL_WIDTH    1
#define EFUSEC_PG_CTRL_MASK     (((1<<EFUSEC_PG_CTRL_WIDTH)-1)<<EFUSEC_PG_CTRL_POS)
#define EFUSEC_PG_CTRL_DIS      (1<<EFUSEC_PG_CTRL_POS)
/* 安全校验使能标志 */
#define EFUSEC_SEC_EN_POS       4
#define EFUSEC_SEC_EN_WIDTH     1
#define EFUSEC_SEC_EN_MASK      (((1<<EFUSEC_SEC_EN_WIDTH)-1)<<EFUSEC_SEC_EN_POS)
#define EFUSEC_SEC_EN           (1<<EFUSEC_SEC_EN_POS)
/* JTAG访问控制 */
#define EFUSEC_JTAG_CTRL_POS     2
#define EFUSEC_JTAG_CTRL_WIDTH   1
#define EFUSEC_JTAG_CTRL_MASK    (((1<<EFUSEC_JTAG_CTRL_WIDTH)-1)<<EFUSEC_JTAG_CTRL_POS)
#define EFUSEC_JTAG_CTRL_EN      (0<<EFUSEC_JTAG_CTRL_POS)


#define EFUSE_MAX_SIZE  64     /* Efuse大小 */
#define EFUSE_HAD_WRITED 1      /*Efuse 已经烧写*/
#define EFUSE_NOT_WRITED 0      /*Efuse 没有烧写*/

int efuseRead(UINT32 group, UINT32* pBuf, UINT32 num);
int efuseWrite(UINT32 group, UINT32* pBuf, UINT32 num);
int efuseWriteCheck(UINT32* value, UINT32 num);

#ifdef __cplusplus
}
#endif

#endif /* efuse.h */

