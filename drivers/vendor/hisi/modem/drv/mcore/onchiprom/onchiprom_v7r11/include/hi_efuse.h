/******************************************************************************/
/*  Copyright (C), 2007-2013, Hisilicon Technologies Co., Ltd. */
/******************************************************************************/
/* File name     : hi_efuse.h */
/* Version       : 2.0 */
/* Created       : 2013-11-27*/
/* Last Modified : */
/* Description   :  The C union definition file for the module efuse*/
/* Function List : */
/* History       : */
/* 1 Date        : */
/* Modification  : Create file */
/******************************************************************************/

#ifndef __HI_EFUSE_H__
#define __HI_EFUSE_H__

/*
 * Project: hi
 * Module : efuse
 */

#include "hi_base.h"
#define HI_EFUSE_BASE_ADDR	0x90021000
//#define HI_EFUSE_BASE_ADDR	0xA0000000

/********************************************************************************/
/*    efuse 寄存器偏移定义(项目名_模块名_寄存器名_OFFSET)        */
/********************************************************************************/
#define    HI_HW_CFG_OFFSET                                  (0x1C) /* 存放efuse上电解复位后从group==15读取的数据寄存器，用于启动判断等。初始值由用户通过efuse烧写确定 */

/********************************************************************************/
/*    efuse 寄存器定义(项目名_模块名_寄存器名_T)        */
/********************************************************************************/
typedef union
{
    struct
    {
        unsigned int    pgm_disable                : 1; /* [0..0] 0：64页1：128页 */
        unsigned int    pad_disable                : 1; /* [1..1] 0：64页1：128页 */
        unsigned int    jtag_en                    : 1; /* [2..2] 0：64页1：128页 */
        unsigned int    boot_sel                   : 1; /* [3..3] 0：64页1：128页 */
        unsigned int    secboot_en                 : 1; /* [4..4] 0：64页1：128页 */
        unsigned int    nf_ctrl_ena0               : 1; /* [5..5] 控制第一组nand信息从管脚还是efuse中读取0：nandc从管脚读取nand flash配置参数1：nandc从efuse中读取nand falsh配置参数 */
        unsigned int    nf_block_size1             : 1; /* [6..6] 0：64页1：128页 */
        unsigned int    nf_ecc_type1               : 2; /* [8..7] 00：无ECC01：1bitECC/4bitECC11：8bitECC10：24bitECC */
        unsigned int    nf_page_size1              : 2; /* [10..9] 00：page size 512字节01：page size 2K字节11：page size 4K字节10：page size 8K字节 */
        unsigned int    nf_addr_num1               : 1; /* [11..11] 0：4周期地址1：5周期地址 */
        unsigned int    nf_ctrl_ena1               : 1; /* [12..12] 控制第二组nand信息从管脚还是efuse中读取0：nandc从管脚读取nand flash配置参数1：nandc从efuse中读取nand falsh配置参数 */
        unsigned int    reserved_2                 : 3; /* [15..13]  */
        unsigned int    nf_block_size              : 1; /* [16..16] 0：64页1：128页 */
        unsigned int    nf_ecc_type                : 2; /* [18..17] 00：无ECC01：1bitECC/4bitECC11：8bitECC10：24bitECC */
        unsigned int    reserved_1                 : 1; /* [19..19]  */
        unsigned int    nf_page_size               : 2; /* [21..20] 00：page size 512字节01：page size 2K字节11：page size 4K字节10：page size 8K字节 */
        unsigned int    nf_addr_num                : 1; /* [22..22] 0：4周期地址1：5周期地址 */
        unsigned int    reserved_0                 : 2; /* [24..23] 0：nandc从管脚读取nand flash配置参数1：nandc从efuse中读取nand falsh配置参数 */
        unsigned int    nf_ctrl_ena                : 1; /* [25..25] V721中不使用0：nandc从管脚读取nand flash配置参数1：nandc从efuse中读取nand falsh配置参数 */
        unsigned int    undefined                  : 6; /* [31..26]  */
    } bits;
    unsigned int    u32;
}HI_HW_CFG_T;    /* 存放efuse上电解复位后从group==15读取的数据寄存器，用于启动判断等。初始值由用户通过efuse烧写确定 */


/********************************************************************************/
/*    efuse 函数(项目名_模块名_寄存器名_成员名_set)        */
/********************************************************************************/
HI_SET_GET(hi_hw_cfg_pgm_disable,pgm_disable,HI_HW_CFG_T,HI_EFUSE_BASE_ADDR, HI_HW_CFG_OFFSET)
HI_SET_GET(hi_hw_cfg_pad_disable,pad_disable,HI_HW_CFG_T,HI_EFUSE_BASE_ADDR, HI_HW_CFG_OFFSET)
HI_SET_GET(hi_hw_cfg_jtag_en,jtag_en,HI_HW_CFG_T,HI_EFUSE_BASE_ADDR, HI_HW_CFG_OFFSET)
HI_SET_GET(hi_hw_cfg_boot_sel,boot_sel,HI_HW_CFG_T,HI_EFUSE_BASE_ADDR, HI_HW_CFG_OFFSET)
HI_SET_GET(hi_hw_cfg_secboot_en,secboot_en,HI_HW_CFG_T,HI_EFUSE_BASE_ADDR, HI_HW_CFG_OFFSET)
HI_SET_GET(hi_hw_cfg_nf_ctrl_ena0,nf_ctrl_ena0,HI_HW_CFG_T,HI_EFUSE_BASE_ADDR, HI_HW_CFG_OFFSET)
HI_SET_GET(hi_hw_cfg_nf_block_size1,nf_block_size1,HI_HW_CFG_T,HI_EFUSE_BASE_ADDR, HI_HW_CFG_OFFSET)
HI_SET_GET(hi_hw_cfg_nf_ecc_type1,nf_ecc_type1,HI_HW_CFG_T,HI_EFUSE_BASE_ADDR, HI_HW_CFG_OFFSET)
HI_SET_GET(hi_hw_cfg_nf_page_size1,nf_page_size1,HI_HW_CFG_T,HI_EFUSE_BASE_ADDR, HI_HW_CFG_OFFSET)
HI_SET_GET(hi_hw_cfg_nf_addr_num1,nf_addr_num1,HI_HW_CFG_T,HI_EFUSE_BASE_ADDR, HI_HW_CFG_OFFSET)
HI_SET_GET(hi_hw_cfg_nf_ctrl_ena1,nf_ctrl_ena1,HI_HW_CFG_T,HI_EFUSE_BASE_ADDR, HI_HW_CFG_OFFSET)
HI_SET_GET(hi_hw_cfg_reserved_2,reserved_2,HI_HW_CFG_T,HI_EFUSE_BASE_ADDR, HI_HW_CFG_OFFSET)
HI_SET_GET(hi_hw_cfg_nf_block_size,nf_block_size,HI_HW_CFG_T,HI_EFUSE_BASE_ADDR, HI_HW_CFG_OFFSET)
HI_SET_GET(hi_hw_cfg_nf_ecc_type,nf_ecc_type,HI_HW_CFG_T,HI_EFUSE_BASE_ADDR, HI_HW_CFG_OFFSET)
HI_SET_GET(hi_hw_cfg_reserved_1,reserved_1,HI_HW_CFG_T,HI_EFUSE_BASE_ADDR, HI_HW_CFG_OFFSET)
HI_SET_GET(hi_hw_cfg_nf_page_size,nf_page_size,HI_HW_CFG_T,HI_EFUSE_BASE_ADDR, HI_HW_CFG_OFFSET)
HI_SET_GET(hi_hw_cfg_nf_addr_num,nf_addr_num,HI_HW_CFG_T,HI_EFUSE_BASE_ADDR, HI_HW_CFG_OFFSET)
HI_SET_GET(hi_hw_cfg_reserved_0,reserved_0,HI_HW_CFG_T,HI_EFUSE_BASE_ADDR, HI_HW_CFG_OFFSET)
HI_SET_GET(hi_hw_cfg_nf_ctrl_ena,nf_ctrl_ena,HI_HW_CFG_T,HI_EFUSE_BASE_ADDR, HI_HW_CFG_OFFSET)
HI_SET_GET(hi_hw_cfg_undefined,undefined,HI_HW_CFG_T,HI_EFUSE_BASE_ADDR, HI_HW_CFG_OFFSET)
#endif // __HI_EFUSE_H__

