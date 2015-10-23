
/******************************************************************************
*    Copyright (c) 2009-2011 by  Hisilicon Tech. Co., Ltd.
*    All rights reserved.
* ***
*
******************************************************************************/
#ifndef _NANDC_PORT_H_
#define _NANDC_PORT_H_
    
#ifdef __cplusplus
extern "C"
{
#endif
#include "product_config.h"

#ifdef __FASTBOOT__

/* keep them for compatibility */
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;
typedef unsigned long long u64;

#ifndef FLASHLESS_MACRO
#define FLASHLESS_MACRO
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
#endif


#define NANDC_DEBUG                                 (0x00000001)
#define NANDC_USE_ASSERT                            (0x00000008)
#define NANDC_USE_MEMPOOL                           (0x00000010)
#define NANDC_READ_ONLY                             (0x00000020)
#define NANDC_LOW_POWER                             (0x00000040)
#define NANDC_USE_MTD                               (0x00000080)

extern void put_str(char *pt,...);

#define NANDC_REG_READ32(addr)                              (*(volatile u32*)(addr) )
#define NANDC_REG_WRITE32(addr, val)                        (*(volatile u32 *)(addr) = (val))
/*下面的两个写寄存器的功能是有区别的*/
/*这里只有相关的bit位，不相关的bit位不变*/
#define NANDC_REG_SETBIT32(addr, pos, bits, val)       \
                (*(volatile u32 *)(addr) = (*(volatile u32 *)(addr) & \
                (~((((u32)1 << (bits)) - 1) << (pos)))) \
                 | ((u32)((val) & (((u32)1 << (bits)) - 1)) << (pos)))
/*写相关的bit位的值,其他的bit位的值为0*/
#define NANDC_REG_WRITEBIT32(addr, pos, bits, val)       \
                (*(volatile u32 *)(addr) =  ((u32)((val) & (((u32)1 << (bits)) - 1)) << (pos)))



#define NANDC_REG_GETBIT32(addr, pos, bits)     \
                 ((*(volatile u32 *)(addr)  >> (pos)) & \
                 (((u32)1 << (bits)) - 1))







#include "config.h"
#include "string.h"
#include <boot/flash.h>

#define NANDC_COMPILE_FLAG                          (NANDC_USE_ASSERT | NANDC_DEBUG | NANDC_LOW_POWER)
#define NANDC_POOL_SIZE_MAX                         (4)

#define STU_SET(element)                            element = 
#define __iomem
#define himalloc(p)                                 alloc(p)
#define hifree(p)                                   free(p)
#define hiassert(p)                                 (*(volatile int*)(0xffffffff) = 0)
#define hiout(p)                                    cprintf p


#endif


#ifdef __cplusplus
}
#endif

#endif  /*_NANDC_PORT_H_*/

