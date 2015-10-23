
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

#ifndef __KERNEL__
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

/* register set and get */
#define NANDC_REG_SETBIT32(addr, pos, bits, val)       \
                (*(volatile u32 *)(addr) = (*(volatile u32 *)(addr) & \
                (~((((u32)1 << (bits)) - 1) << (pos)))) \
                 | ((u32)((val) & (((u32)1 << (bits)) - 1)) << (pos)))

#define NANDC_REG_WRITEBIT32(addr, pos, bits, val)       \
                (*(volatile u32 *)(addr) =  ((u32)((val) & (((u32)1 << (bits)) - 1)) << (pos)))



#define NANDC_REG_GETBIT32(addr, pos, bits)     \
                 ((*(volatile u32 *)(addr)  >> (pos)) & \
                 (((u32)1 << (bits)) - 1))

#ifdef __KERNEL__ 
#include <linux/version.h>
#include <asm/io.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <bsp_om.h>

#define STU_SET(element)                            element = 

#define NANDC_COMPILE_FLAG                          (NANDC_DEBUG | NANDC_USE_ASSERT | NANDC_LOW_POWER)
#define NANDC_POOL_SIZE_MAX                         (10400 + 1024)

#define __iomem
#define himalloc(p)                                 kmalloc(p, GFP_KERNEL)
#define hifree(p)                                   kfree(p)
#define hiassert(p)                                 BUG_ON(1)
#define hiout(p)                                    printk p 

#elif defined(__FASTBOOT__)
#include "string.h"
#include "config.h"
#include <boot/flash.h>

#define NANDC_COMPILE_FLAG                          (NANDC_USE_ASSERT | NANDC_DEBUG | NANDC_LOW_POWER)
#define NANDC_POOL_SIZE_MAX                         (4)

#define STU_SET(element)                            element = 
#define __iomem
#define hiout(p)                                    cprintf p
#define himalloc(p)                                 alloc(p)
#define hiassert(p)                                 (*(volatile int*)(0xffffffff) = 0)
#define hifree(p)                                   free(p)

#else
#error no plant macor defined
#endif


#ifdef __cplusplus
}
#endif

#endif  /*_NANDC_PORT_H_*/

