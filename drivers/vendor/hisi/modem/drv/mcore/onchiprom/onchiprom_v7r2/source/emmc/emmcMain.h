/*************************************************************************
*   版权所有(C) 1987-2004, 深圳华为技术有限公司.
*
*   文 件 名 :  emmcMain.h
*
*   作    者 :  wangzhongshun
*
*   描    述 :  emmc自举的总流程控制头文件
*
*   修改记录 :  2011年6月15日  v1.00  wangzhongshun  创建
*
*************************************************************************/
#ifndef __EMMC_MAIN_H__
#define __EMMC_MAIN_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include  "emmcConfig.h" 

#ifdef EMMC_DEBUG
void emmc_print_current_tick( void );
void emmc_reg_dump(void);
VOID emmc_packet_dump(UINT8 *pcAddr);
#endif

VOID emmc_ulong2byte(UINT8 *pucByte, UINT32 ulLong);
UINT32 emmc_byte2ulong(UINT8 *pucByte);
void emmc_delay_ms( UINT32 ulMsecond );


/*****************************************************************************
* 函 数 名  : emmc_slave_dereset
*
* 功能描述  : 给emmc设置解复位/解复位
*
* 输入参数  : bReset:
*             TRUE:复位
*             FALSE:解复位
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 其它说明  : 使用管脚gpio3.3
*
*****************************************************************************/
VOID emmc_slave_reset(BOOL bReset);

/*****************************************************************************
* 函 数 名  : emmc_read_bootloader
*
* 功能描述  : 从eMMC/MMC/SD中读取bootloader
*
* 输入参数  : UINT32 dst  
* 输出参数  : 无
*
* 返 回 值  : 0 :读取成功
*             -1:读取失败
*
* 其它说明  : 无
*
*****************************************************************************/
INT32 emmc_read_bootloader(UINT32 dst);

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif /* end of emmc.h */
