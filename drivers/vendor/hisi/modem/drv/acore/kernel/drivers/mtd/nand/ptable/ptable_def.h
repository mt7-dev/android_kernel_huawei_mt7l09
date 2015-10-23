
/******************************************************************************
*    Copyright (c) 2009-2011 by  Hisilicon Tech. Co., Ltd.
*    All rights reserved.
* ***
*
******************************************************************************/


#ifndef _PTABLE_DEF_H_
#define _PTABLE_DEF_H_
    
#ifdef __cplusplus
extern "C"
{
#endif

#ifndef _ASMLANGUAGE

#ifdef __FASTBOOT__
#include "config.h"
#endif
#include "nandc_balong.h"

#define BSPLOGSTR(x) {hiout(("%s",(char *)x));}

#if defined(__FASTBOOT__)
#define BSPLOGU32(x) {hiout(("%x",x));}
#define BSPLOGU16(x) {hiout(("%x",x));}
#define BSPLOGU8(x)  {hiout(("%x",x));}
#else
#define BSPLOGU32(z) {hiout(("%08x",z));}
#define BSPLOGU16(z) {hiout(("%04x",z));}
#define BSPLOGU8(z)  {hiout(("%02x",z));}
#endif

/**********************************************************
*  the bit map of struct ST_PART_TBL.property
*----------------------------------------------------------
* bit:8 ~ bit:32           using property
*----------------------------------------------------------
*          | bit 12 ~ 32    --  not used yet 
*          | bit 11         --  protected
*          | bit 10         --  mount in bootrom 
*          | bit 9          --  mount in vxworks
*          | bit 8          --  auto load 
*----------------------------------------------------------
*  bit 4~7                 -- not used yet 
*----------------------------------------------------------
* bit:3                    -- image  data validity
*----------------------------------------------------------
*         | value           1 -- image is invalid 
*                           0 -- image is valid 
*----------------------------------------------------------
* bit:0 ~ bit:2            -- image  data type
*----------------------------------------------------------
*          value            1 -- indicate　yaffs image data
*         |                 0 -- indicate　normal data 
*-----------------------------------------------------------
************************************************************/

#define PTABLE_PROPERTY_OFFSET          (0x8)

typedef enum 
{
    PTABLE_AUTOLOAD_OFFSET              = PTABLE_PROPERTY_OFFSET,
    PTABLE_MOUNTVXWORKS_OFFSET,
    PTABLE_MOUNTBOOTROM_OFFSET,
    PTABLE_PROTECTED_OFFSET,
    PTABLE_PACKSCAN_OFFSET,
    PTABLE_FACTORY_OFFSET,  
    PTABLE_MTD_OFFSET
}PTABLE_OFFSET;

#define PTABLE_FLAG_AUTOLOAD        (1<<PTABLE_AUTOLOAD_OFFSET)    /*设此标志位 启动需要加载到对应的ddr地址*/
#define MOUNT_VXWORKS               (1<<PTABLE_MOUNTVXWORKS_OFFSET)/*设此标志位 VxWorks启动需要挂接载到对应的文件系统*/
#define MOUNT_BOOTROM               (1<<PTABLE_MOUNTBOOTROM_OFFSET)/*设此标志位 Bootrom启动需要挂接载到对应的文件系统*/
#define PTABLE_PROTECTED            (1<<PTABLE_PROTECTED_OFFSET)   /*设此标志位 数据在分区结构变化时受到保护*/
#define MOUNT_FACTORY               (1<<PTABLE_FACTORY_OFFSET)     /*设此标志位 在产品小镜像mount此分区*/
#define MOUNT_MTD                    (1<<PTABLE_MTD_OFFSET)         /*设此标志位 在A核MTD初始化的时候挂载此分区为MTD设备*/

#define PTABLE_MOUNT_MTD            MOUNT_MTD


#define TRACE_NORMAL(p)             NANDC_TRACE(NFCDBGLVL(NORMAL),p)
#define TRACE_NORMAL_P0(p)  
#define TRACE_NORMAL_P1(p)    
#define TRACE_NORMAL_P2(p)  

#define TRACE_ERROR(p)              NANDC_TRACE(NFCDBGLVL(ERRO),p)
#define TRACE_ERROR_P0(p)      
#define TRACE_ERROR_P1(p)     
#define TRACE_ERROR_P2(p)     

#define TRACE_WARNING(p)        NANDC_TRACE(NFCDBGLVL(WARNING),p)
#define TRACE_WARNING_P0(p)    
#define TRACE_WARNING_P1(p)    
#define TRACE_WARNING_P2(p)    

/*定义分区表总体数据有效性标志位*/
typedef enum
{
    PTABLE_HEAD_VALIDITY_OFFSET = 0,
    PTABLE_HEAD_DEFAULT_OFFSET = 30
}PTABLE_HEAD_OFFSET;

#define PTABLE_FLAG_VALIDITY        (1<<PTABLE_HEAD_VALIDITY_OFFSET) /*设此标志位，启动（不管冷热）的时候进入强制加载模式*/
#define PTABLE_FLAG_DEFAULT         (1<<PTABLE_HEAD_DEFAULT_OFFSET)  /*设此标志位，表示系统使用的是默认分区表 */

/********************************************************************************/

#endif /*_ASMLANGUAGE*/

#ifdef __cplusplus
}
#endif

#endif  /*_NANDC_DEF_H_*/



