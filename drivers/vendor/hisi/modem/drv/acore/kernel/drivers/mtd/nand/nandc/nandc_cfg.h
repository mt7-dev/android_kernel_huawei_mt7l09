
/******************************************************************************
*    Copyright (c) 2009-2011 by  Hisilicon Tech. Co., Ltd.
*    All rights reserved.
* ***
*
******************************************************************************/
#ifndef _NANDC_CFG_H_
#define _NANDC_CFG_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <bsp_memmap.h>
#include <hi_base.h>
#include <hi_syssc.h>
#include "nandc_v400.h"
#include "nandc_v600.h"
#include <bsp_nandc.h>

/* keep them for compatibility */
#define NANDC_STATUS_TIMEOUT	                (10)
#define NANDC_MAX_PAGE_SWAP                     (8)

#define NANDC_MAX_CHIPS                         (1)

#define NANDC_BAD_FLAG_FIRSTPAGE_OFFSET         (0)
#define NANDC_BAD_FLAG_SECONDPAGE_OFFSET        (1)
#define NANDC_BAD_FLAG_BYTE_SIZE                (4)

#define  NANDC_NULL_ASSERT_FORMAT               "0x%x,file:%s; line:%d \n"
#define  NANDC_NULL_PARAM_FORMAT                "NULL parameter\n "

#define NANDC_RD_RESULT_TIMEOUT                 (100000)
#define NANDC_PRG_RESULT_TIMEOUT                (1000000)
#define NANDC_ERASE_RESULT_TIMEOUT              (10000000)
#define NANDC_RESULT_TIME_OUT                   (10000000)

#define NANDC_RD_RETRY_CNT                      (3)

#define NANDC_MARK_SIZE                         (64)             /*in byte*/
#define NANDC_BADBLOCK_FLAG_SIZE                NANDC_MARK_SIZE  /*in byte*/
#define NANDC_ERASED_FLAG_SIZE                  NANDC_MARK_SIZE  /*in byte*/

#define NANDC_MAX_BLOCK_MASK                    (0xFFFFF)  /*(1MB)*/
#define NANDC_MAX_PAGE_MASK                     (0x1FFF)  /*(8kB)*/

#ifndef CONFIG_YAFFS_NO_YAFFS1
#define YAFFS_BYTES_PER_SPARE                   16
#endif

#define NANDC_REG_BASE_ADDR                    (HI_NANDC_REGBASE_ADDR)
#define NANDC_BUFFER_BASE_ADDR                 HI_NAND_MEM_BUFFER_ADDR
#define RVDS_TRACE_WRITE_BUFF                  (0x33f00000)
#define NANDC_SKIP_DEBUG
#define NANDC_RANDOM_ACCESS_THRESHOLD          (512)
#define NANDC_ECC_ACCESS_THRESHOLD             (NANDC_BUFSIZE_BASIC)
#define NANDC_READ_RESULT_DELAY

#if defined(NANDC_USE_V400)
#define NANDC_NAME                  NANDC4_NAME
#define NANDC_REG_SIZE              NANDC4_REG_SIZE
#define NANDC_BUFSIZE_BASIC			NANDC4_BUFSIZE_BASIC
#define NANDC_BUFSIZE_EXTERN        NANDC4_BUFSIZE_EXTERN
#define NANDC_BUFSIZE_TOTAL         NANDC4_BUFSIZE_TOTAL
#define NANDC_SLEEP_BUFFER_SIZE     (20)
#define NANDC_INT_CLEAR_ALL          nandc4_int_clear_all

#elif defined(NANDC_USE_V600)
#define NANDC_NAME                  NANDC6_NAME
#define NANDC_REG_SIZE              NANDC6_REG_SIZE
#define NANDC_BUFSIZE_BASIC			NANDC6_BUFSIZE_BASIC
#define NANDC_BUFSIZE_EXTERN        NANDC6_BUFSIZE_EXTERN
#define NANDC_BUFSIZE_TOTAL         NANDC6_BUFSIZE_TOTAL
#define NANDC_SLEEP_BUFFER_SIZE     (20)
#define NANDC_IRQ                   NANDC6_IRQ
#define NANDC_INT_CLEAR_ALL         nandc6_int_clear_all

/* #define FEATURE_NANDC_DMA_USE_INTERRUPT */  /* A core will use interrupt if this macro opens */
#else
#error no nand controller defined!!
#endif

#ifdef __cplusplus
}
#endif

#endif  /*_NANDC_DEF_H_*/
