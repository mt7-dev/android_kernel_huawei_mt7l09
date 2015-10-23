
/******************************************************************************
*    Copyright (c) 2009-2011 by  Hisilicon Tech. Co., Ltd.
*    All rights reserved.
* ***
*
******************************************************************************/


#ifdef __cplusplus
extern "C"
{
#endif

#include "ptable_def.h"
#include "ptable_com.h"

/*----------------------- |  0 byte
 |"pTableHead"            |
 *----------------------- | 16 byte (partition head flag string)
 | the property of table  |
 *----------------------- | 20 byte (partition head flag string)
 |"V7R2_FPGA" (example.)  |
 *----------------------- | 48 byte (partition table version name)
 | <partition info>       |
 |  (size 32byte)         |
 *----------------------- | 96 byte
 | < partition info >     |
 |  (size 32byte)         |
 |------------------------| 144 byte
 :    .............       :
 :    .............       :
 |------------------------| 48 x N byte
 | < partition info >     |
 |  (size 32byte)         |
 |------------------------| 48 x (N+1) byte
 | "T"  (table end flag)  |
 |                        |
 |------------------------| */

//#pragma GCC push_options
//#pragma GCC optimize("O0")

char ptable_tail[PTABLE_TAIL_SIZE] = {PTABLE_TAIL_STR};

/*table partition begin*/
/*
 * Size of ptable.bin is set 2KB, so max number of partitions is 42, if you have to use more than 42 partitions, you must modify somewhere,
 * e.g. fastboot(load ptable, burn ptable.bin to nand), t32 script(nandwrite.cmm).
 */
struct ST_PART_TBL ptable_product[PTABLE_PARTITION_MAX] =
{
    /*name*/        /*offset*/      /*loadsize*/    /*capacity*/   /*loadaddr*/   /*entry*/     /*type*/            /*property*/
#if (PTABLE_M3_BOOT_LEN != 0)
    {PTABLE_M3BOOT_NM       ,PTABLE_M3_BOOT_START   , 0x00000000,PTABLE_M3_BOOT_LEN,  0x00000000,  0x00000000   ,IMAGE_M3BOOT ,DATA_NORMAL | PTABLE_MOUNT_MTD ,   0},
#endif
#if (PTABLE_FASTBOOT_LEN != 0)
    {PTABLE_FASTBOOT_NM     ,PTABLE_FASTBOOT_START    , 0x00000000,PTABLE_FASTBOOT_LEN,  PTABLE_FASTBOOT_ENTRY,  PTABLE_FASTBOOT_ENTRY   ,IMAGE_FASTBOOT  ,DATA_NORMAL | PTABLE_MOUNT_MTD   ,   0},
#endif
#if (PTABLE_NV_LTE_LEN != 0)
    {PTABLE_NVBACK_NM    ,PTABLE_NV_LTE_START   , 0x00000000,PTABLE_NV_LTE_LEN,  0x00000000,  0x00000000   ,IMAGE_NVBACKLTE ,DATA_NORMAL | PTABLE_PROTECTED | PTABLE_MOUNT_MTD   ,   0},
#endif
#if (PTABLE_NVIMG_LEN != 0)
    {PTABLE_NVIMG_NM        ,PTABLE_NVIMG_START   , 0x00000000  ,PTABLE_NVIMG_LEN,  0x00000000,  0x00000000 ,IMAGE_NVIMG   ,DATA_NORMAL   | PTABLE_MOUNT_MTD  ,   0},
#endif
#if (PTABLE_NVDLD_LEN != 0)
    {PTABLE_NVDLOAD_NM      ,PTABLE_NVDLD_START   , 0x00000000   ,PTABLE_NVDLD_LEN,  0x00000000,  0x00000000   ,IMAGE_NVDLD   ,DATA_NORMAL | PTABLE_MOUNT_MTD     ,   0},
#endif
#if (PTABLE_NVFACTORY_LEN != 0)
    {PTABLE_NVDEFAULT_NM    ,PTABLE_NVFACTORY_START   , 0x00000000   ,PTABLE_NVFACTORY_LEN,   0x00000000,  0x00000000   ,IMAGE_NVFACTORY   ,DATA_NORMAL | PTABLE_MOUNT_MTD    ,   0},
#endif
#if (PTABLE_OEMINFO_LEN != 0)
    {PTABLE_OEMINFO_NM      ,PTABLE_OEMINFO_START   ,0x00000000 ,PTABLE_OEMINFO_LEN,   0x00000000,  0x00000000   ,IMAGE_OEMINFO    ,DATA_NORMAL | PTABLE_MOUNT_MTD,   0},
#endif
#if (PTABLE_YAFFS_USERDATA_LEN != 0)
    {PTABLE_USERDATA_NM     ,PTABLE_YAFFS_USERDATA_START   ,0x00000000   ,PTABLE_YAFFS_USERDATA_LEN,   0x00000000,  0x00000000   ,IMAGE_USERDATA    ,DATA_YAFFS | PTABLE_MOUNT_MTD       ,   0},
#endif
#if (PTABLE_YAFFS_ONLINE_LEN != 0)
    {PTABLE_ONLINE_NM       ,PTABLE_YAFFS_ONLINE_START   ,0x00000000   ,PTABLE_YAFFS_ONLINE_LEN,   0x00000000,  0x00000000   ,IMAGE_ONLINE, DATA_YAFFS | PTABLE_MOUNT_MTD      ,   0},
#endif
#if (PTABLE_BOOTIMAGE_LEN != 0)
    {PTABLE_KERNEL_NM       ,PTABLE_BOOTIMAGE_START ,0x00000000   ,PTABLE_BOOTIMAGE_LEN, (PTABLE_FASTBOOT_ENTRY + PRODUCT_CFG_FLASH_FASTBOOT_LEN), (PTABLE_FASTBOOT_ENTRY + PRODUCT_CFG_FLASH_FASTBOOT_LEN), IMAGE_KERNEL    ,DATA_NORMAL | PTABLE_MOUNT_MTD,   0},
#endif
#if (PTABLE_BOOTIMAGEBAK_LEN != 0)    
    {PTABLE_KERNELBK_NM       ,PTABLE_BOOTIMAGEBAK_START ,0x00000000   ,PTABLE_BOOTIMAGEBAK_LEN, (PTABLE_FASTBOOT_ENTRY + PRODUCT_CFG_FLASH_FASTBOOT_LEN), (PTABLE_FASTBOOT_ENTRY + PRODUCT_CFG_FLASH_FASTBOOT_LEN), IMAGE_KERNELBK    ,DATA_NORMAL | PTABLE_MOUNT_MTD,   0},
#endif
#if (PTABLE_LOGO_LEN != 0)
    {PTABLE_LOGO_NM       ,PTABLE_LOGO_START ,0x00000000   ,PTABLE_LOGO_LEN, 0x00000000, 0x00000000, IMAGE_LOGO, DATA_NORMAL | PTABLE_MOUNT_MTD,   0},
#endif
#if (PTABLE_M3IMAGE_LEN != 0)
    {PTABLE_M3IMG_NM      ,PTABLE_M3IMAGE_START   ,0x00000000   ,PTABLE_M3IMAGE_LEN,   0x00000000,  0x00000000   ,IMAGE_M3IMAGE    ,DATA_NORMAL | PTABLE_MOUNT_MTD       ,   0},
#endif
#if (PTABLE_DSP_LEN != 0)
    {PTABLE_DSP_NM          ,PTABLE_DSP_START ,0x00000000   ,PTABLE_DSP_LEN,   0x00000000,  0x00000000   ,IMAGE_DSP    ,DATA_NORMAL | PTABLE_MOUNT_MTD,   0},
#endif
#if (PTABLE_MISC_LEN != 0)
    {PTABLE_MISC_NM         ,PTABLE_MISC_START   ,0x00000000   ,PTABLE_MISC_LEN,   0x00000000,  0x00000000   ,IMAGE_MISC    ,DATA_NORMAL | PTABLE_MOUNT_MTD       ,   0},
#endif
#if (PTABLE_CCORE_LEN != 0)
    {PTABLE_VXWORKS_NM      ,PTABLE_CCORE_START   ,0x00000000   ,PTABLE_CCORE_LEN,   PTABLE_VXWORKS_CCORE_ENTRY,  PTABLE_VXWORKS_CCORE_ENTRY   ,IMAGE_CCORE     ,DATA_NORMAL | PTABLE_MOUNT_MTD                       ,   0}, 
#endif
#if (PTABLE_RESERVE1_LEN != 0)
    {PTABLE_RESERVE1_NM      ,PTABLE_RESERVE1_START   ,0x00000000   ,PTABLE_RESERVE1_LEN,   0x00000000,  0x00000000   ,IMAGE_RESERVE1     ,DATA_NORMAL | PTABLE_MOUNT_MTD                       ,   0}, 
#endif
#if (PTABLE_RESERVE2_LEN != 0)
    {PTABLE_RESERVE2_NM      ,PTABLE_RESERVE2_START   ,0x00000000   ,PTABLE_RESERVE2_LEN,   0x00000000,  0x00000000   ,IMAGE_RESERVE2     ,DATA_NORMAL | PTABLE_MOUNT_MTD                       ,   0}, 
#endif
#if (PTABLE_RESERVE3_LEN != 0)    
    {PTABLE_RESERVE3_NM      ,PTABLE_RESERVE3_START   ,0x00000000   ,PTABLE_RESERVE3_LEN,  0x00000000,  0x00000000   ,IMAGE_RESERVE3     ,DATA_YAFFS | PTABLE_MOUNT_MTD                       ,   0}, 
#endif
#if (PTABLE_OM_LEN != 0)    
    {PTABLE_OM_NM           ,PTABLE_OM_START   ,0x00000000   ,PTABLE_OM_LEN,   0x00000000,  0x00000000   ,IMAGE_OM    ,DATA_YAFFS | PTABLE_MOUNT_MTD       ,   0},
#endif
#if (PTABLE_YAFFS_APP_LEN != 0)    
    {PTABLE_APP_NM          ,PTABLE_YAFFS_APP_START  ,0x00000000   ,PTABLE_YAFFS_APP_LEN,   0x00000000,  0x00000000   ,IMAGE_APP    ,DATA_YAFFS |  PTABLE_MOUNT_MTD       ,   0},
#endif
#if (PTABLE_YAFFS_WEBUI_LEN != 0)
    {PTABLE_WEBUI_NM        ,PTABLE_YAFFS_WEBUI_START   ,0x00000000   ,PTABLE_YAFFS_WEBUI_LEN,   0x00000000,  0x00000000   ,IMAGE_WEBUI    ,DATA_YAFFS | PTABLE_MOUNT_MTD      ,   0},
#endif
#if (PTABLE_YAFFS_SYSIMG_LEN != 0)
    {PTABLE_SYSTEM_NM       ,PTABLE_YAFFS_SYSIMG_START   ,0x00000000   ,PTABLE_YAFFS_SYSIMG_LEN,   0x00000000,  0x00000000   ,IMAGE_SYSTEM    ,DATA_YAFFS | PTABLE_MOUNT_MTD       ,   0},
#endif
#if (PTABLE_CACHE_LEN != 0)
    {PTABLE_CACHE_NM        ,PTABLE_CACHE_START   ,0x00000000   ,PTABLE_CACHE_LEN,   0x00000000,  0x00000000   ,IMAGE_CACHE    ,DATA_NORMAL | PTABLE_MOUNT_MTD       ,   0},
#endif
#if (PTABLE_RECOVERY_A_LEN != 0)    
    {PTABLE_RECOVERYA_NM   ,PTABLE_RECOVERY_A_START   ,0x00000000   ,PTABLE_RECOVERY_A_LEN,   0x00000000,  0x00000000   ,IMAGE_RECOVERY_A     ,DATA_NORMAL | PTABLE_MOUNT_MTD    ,   0},
#endif
#if (PTABLE_RECOVERY_B_LEN != 0)
    {PTABLE_RECOVERYB_NM   ,PTABLE_RECOVERY_B_START   ,0x00000000   ,PTABLE_RECOVERY_B_LEN,   0x00000000,  0x00000000   ,IMAGE_RECOVERY_B     ,DATA_NORMAL | PTABLE_MOUNT_MTD     ,   0},
#endif
#if (PTABLE_ISO_LEN != 0)
    {PTABLE_CDROMISO_NM     ,PTABLE_ISO_START   ,0x00000000   ,PTABLE_ISO_LEN,   0x00000000,  0x00000000   ,IMAGE_CDROMISO    ,DATA_YAFFS | PTABLE_MOUNT_MTD       ,   0},
#endif
    {PTABLE_END_STR ,0x00000000   ,0x00000000   ,0x00000000,   0x00000000,  0x00000000   ,IMAGE_PART_TOP  ,DATA_YAFFS                        ,   0}
};

struct ST_PTABLE_HEAD ptable_head =
{
	PTABLE_HEAD_STR,
	0x80000000,
	PTABLE_VER_ID_STR,
	PTABLE_VER_STR
};
//#pragma GCC pop_options

#ifdef __cplusplus
}
#endif

