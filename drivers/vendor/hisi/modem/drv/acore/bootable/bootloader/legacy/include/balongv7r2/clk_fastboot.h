/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 : clk_fastboot.h
*
*   作    者 :  xujingcui
*
*   描    述 : 本文件用于关闭系统时钟策略中的非用即关IP时钟
*
*   修改记录 :  2013年8月10日  v1.00 xujingcui创建
*************************************************************************/
#ifndef __CLK_FASTBOOT_H__
#define __CLK_FASTBOOT_H__
#include <hi_syscrg_interface.h>
#include <hi_pwrctrl_interface.h>

#ifdef CONFIG_V7R2_CLK_CRG
#define  HI_CRG_DISABLE_OFFSET_01    HI_CRG_CLKDIS1_OFFSET      /*0X4*/
#define  HI_CRG_DISABLE_OFFSET_02    HI_CRG_CLKDIS2_OFFSET      /*0X10*/
#define  HI_CRG_DISABLE_OFFSET_03    HI_CRG_CLKDIS3_OFFSET      /*0X1C*/
#define  HI_CRG_DISABLE_OFFSET_04    HI_CRG_CLKDIS4_OFFSET      /*0X28*/
#define  HI_CRG_DISABLE_OFFSET_05    HI_CRG_CLKDIS5_OFFSET      /*0X34*/

#define  HI_CRG_RESETEN_OFFSET_02   HI_CRG_SRSTEN2_OFFSET      /*0x6C*/

#define HI_PWRCTR_ISO_CELL_OFFSET_01   HI_PWR_CTRL4_OFFSET   /*0XC10*/
#define HI_PWRCTR_MTCOMS_OFFSET            HI_PWR_CTRL7_OFFSET   /*0XC1C*/

#endif

/****************************************************************************
*此函数用于关闭时钟非用即关策略中的IP时钟
**/
void bsp_disable_non_on_clk(void);

#endif
