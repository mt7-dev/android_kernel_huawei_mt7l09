/******************************************************************************/
/*  Copyright (C), 2007-2013, Hisilicon Technologies Co., Ltd. */
/******************************************************************************/
/* File name     : hi_syssc_interface.h */
/* Version       : 2.0 */
/* Created       : 2013-04-18*/
/* Last Modified : */
/* Description   : */
/* Function List : */
/* History       : */
/* 1 Date        : */
/* Modification  : Create file */
/******************************************************************************/
#ifndef __HI_SYSCRG_INTER_H__
#define __HI_SYSCRG_INTER_H__

#include "hi_base.h"
#include "hi_syssc.h"

/*此宏用于CLK模块*/
#ifndef CONFIG_K3V3_CLK_CRG
#define CONFIG_K3V3_CLK_CRG
#endif

#ifndef __KERNEL__
static __inline__ void set_hi_crg_clkdis5_irm_pd_clk_dis(unsigned int v)
{
	return set_hi_crg_clkdis5_irmbbp_pd_clk_dis(v);
}


/* get clk/mtcmos status  ---- start */
static __inline__ unsigned int get_hi_crg_clkstat1(void)
{    return 0;}
static __inline__ unsigned int get_hi_crg_clkstat2(void)
{    return 0;}
static __inline__ unsigned int get_hi_crg_clkstat3(void)
{
#if defined(__KERNEL__)
	return readl((const volatile void *)(HI_SYSCRG_BASE_ADDR+HI_CRG_CLKSTAT3_OFFSET));
#else
	return readl((HI_SYSCRG_BASE_ADDR+HI_CRG_CLKSTAT3_OFFSET));
#endif
}
static __inline__ unsigned int get_hi_crg_clkstat4(void)
{
#if defined(__KERNEL__)
	return readl((const volatile void *)(HI_SYSCRG_BASE_ADDR+HI_CRG_CLKSTAT4_OFFSET));
#else
	return readl((HI_SYSCRG_BASE_ADDR+HI_CRG_CLKSTAT4_OFFSET));
#endif
}
static __inline__ unsigned int get_hi_crg_clkstat5(void)
{
#if defined(__KERNEL__)
	return readl((const volatile void *)(HI_SYSCRG_BASE_ADDR+HI_CRG_CLKSTAT5_OFFSET));
#else
	return readl((HI_SYSCRG_BASE_ADDR+HI_CRG_CLKSTAT5_OFFSET));
#endif
}
/* get clk/mtcmos status  ---- end */
#endif

#endif

