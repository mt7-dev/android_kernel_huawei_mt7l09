
/******************************************************************************
*    Copyright (c) 2009-2011 by  Hisilicon Tech. Co., Ltd.
*    All rights reserved.
* ***
*
******************************************************************************/
#ifndef _NANDC_MTD_H_
#define _NANDC_MTD_H_

#ifdef __cplusplus
extern "C"
{
#endif
#include "hi_nandc.h"
#include "nandc_inc.h"

#include "nandc_v400.h"
#include "nandc_v600.h"


#if defined (NANDC_USE_V400)

#define NANDC_NAME                  NANDC4_NAME
#define NANDC_REG_SIZE              NANDC4_REG_SIZE
#define NANDC_BUFSIZE_BASIC			NANDC4_BUFSIZE_BASIC
#define NANDC_BUFSIZE_EXTERN        NANDC4_BUFSIZE_EXTERN
#define NANDC_BUFSIZE_TOTAL         NANDC4_BUFSIZE_TOTAL

#elif defined (NANDC_USE_V600)

#define NANDC_NAME                  NANDC6_NAME
#define NANDC_REG_SIZE              NANDC6_REG_SIZE
#define NANDC_BUFSIZE_BASIC			NANDC6_BUFSIZE_BASIC
#define NANDC_BUFSIZE_EXTERN        NANDC6_BUFSIZE_EXTERN
#define NANDC_BUFSIZE_TOTAL         NANDC6_BUFSIZE_TOTAL

#else
#error no bsp board defined!!
#endif

#define NANDC_ADDR_CYCLE_MASK                   (0x4)
#define NANDC_CHIP_DELAY                        (25)

#ifdef CONFIG_PM 
#define END_FLAG    0xFFFFFFFF
#endif
/*****************************************************************************/

/*****************************************************************************/


u32  nandc_host_init_mtd(struct nandc_host * host,
                                    u32 pagesize,
                                    u32 sparesize,
                                    u32 chipsize,
                                    u32 erasesize,
                                    u32 buswidth,
                                    u32 numchips,
                                    struct mtd_partition  * ptable,
                                    u32 nr_parts
                                    );
/*****************************************************************************/
#ifdef CONFIG_PM 
u32 reg_offset[NANDC_SLEEP_BUFFER_SIZE / sizeof(u32)] = 
{
    HI_NFC_CON_OFFSET,
    HI_NFC_PWIDTH_OFFSET,
    HI_NFC_OPIDLE_OFFSET,
    
#ifdef NANDC_USE_V400
    HI_NFC_OP_OFFSET,
#else
    HI_NFC_DMA_CTRL_OFFSET,
#endif    

    END_FLAG,
};

#endif
/*****************************************************************************/


#ifdef __cplusplus
}
#endif

#endif  /*_NANDC_MTD_H_*/



