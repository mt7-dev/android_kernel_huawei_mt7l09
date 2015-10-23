#include "soc_baseaddr_interface.h"

#ifndef __SOC_INTERFACE_H
#define __SOC_INTERFACE_H


#include "soc_baseaddr_interface.h"
#include "bsp_memmap.h"

/****************************************************************************
    1)基本类型定义,采用原始基本类型定义
    2)不能包含其他任何头文件
    3) SOC提供基地址为32位的，依靠宏转为16位的给物理层使用
    BASE_ADDR:         32位地址
    BASE_ADDR_HIGH/LOW:16位地址
 ****************************************************************************/

#ifndef BIT_X
#define BIT_X(num)                          (((unsigned long)0x01) << (num))
#endif


/****************************************************************************
 *                   各个地址空间基地址偏移定义                             *
 *                                                                          *
 ****************************************************************************/
/* ZSP 代码空间地址偏移 */
#define ZSP_ITCM_BASE_ADDR                  ((unsigned long)0x00000000)
#define ZSP_ITCM_BASE_ADDR_HIGH             ((unsigned long)ZSP_ITCM_BASE_ADDR>>16)

/* ZSP 数据空间地址偏移 */
#define ZSP_DTCM_BASE_ADDR                  ((unsigned long)0x00000000)
#define ZSP_DTCM_BASE_ADDR_HIGH             ((unsigned long)ZSP_DTCM_BASE_ADDR>>16)

/* (W)BBP 地址偏移 */
#define WBBP_BASE_ADDR                      (SOC_BBP_WCDMA_BASE_ADDR)
#define WBBP_BASE_ADDR_HIGH                 ((unsigned long)WBBP_BASE_ADDR>>16)

#ifdef INSTANCE_1
/* GBBP 地址偏移 */
#define GBBP_BASE_ADDR                      ((unsigned long)SOC_BBP_GSM1_BASE_ADDR)
#define GBBP_BASE_ADDR_HIGH                 ((unsigned long)SOC_BBP_GSM1_BASE_ADDR>>16)

/* GBBP非掉电区基地址 */
#define GDRX_BASE_ADDR                      ((unsigned long)SOC_BBP_GSM1_ON_BASE_ADDR)
#define GDRX_BASE_ADDR_HIGH                 ((unsigned long)SOC_BBP_GSM1_ON_BASE_ADDR>>16)
#else
/* GBBP 地址偏移 */
#define GBBP_BASE_ADDR                      ((unsigned long)SOC_BBP_GSM_BASE_ADDR)
#define GBBP_BASE_ADDR_HIGH                 ((unsigned long)SOC_BBP_GSM_BASE_ADDR>>16)

/* GBBP非掉电区基地址 */
#define GDRX_BASE_ADDR                      ((unsigned long)SOC_BBP_GSM_ON_BASE_ADDR)
#define GDRX_BASE_ADDR_HIGH                 ((unsigned long)SOC_BBP_GSM_ON_BASE_ADDR>>16)
#endif

/* AHB邮箱地址偏移 */
#define AHB_BASE_ADDR                       (g_stUphyExtMemAddr.uwDspUpMemAddr)
#define AHB_BASE_ADDR_HIGH                  ((unsigned long)AHB_BASE_ADDR>>16)

/* BBPCOMM的基地址 */
#define BBPCOMM_BASE_ADDR                   (SOC_BBP_COMM_BASE_ADDR)

/*UPACC 基地址*/
#define HSUPA_UPACC_BASE_ADDR               (SOC_UPACC_BASE_ADDR)
#define HSUPA_UPACC_BASE_ADDR_HIGH          ((unsigned long)HSUPA_UPACC_BASE_ADDR>>16)

/* DMA 地址偏移 */
#define DMAC_BASE_ADDR                      ((unsigned long)0x20380000)
#define DMAC_BASE_ADDR_HIGH                 ((unsigned long)DMAC_BASE_ADDR>>16)

#define SOC_BASE_ADDR                       HI_SYSCTRL_BASE_ADDR
#define SOC_BASE_ADDR_HIGH                  ((unsigned long)SOC_BASE_ADDR>>16)

/* 为了和北研芯片统一，有部分系统控制字地址搬移到AHB中 */
#define SYS_CTRL_ADDRESS                    ( SOC_BASE_ADDR_HIGH )

#if 0
/*********************SSI0 寄存器地址****************************************/
#define HAL_SSI0_BASE_ADDR                  ((unsigned long)0x5F061000)
#define HAL_SSI0_BASE_ADDR_HIGH             ((unsigned long)HAL_SSI0_BASE_ADDR>>16)
#endif
#define IPCRIS_BASE_ADDR                    ((unsigned long)0x27054000 )
#define IPCRIS_BASE_ADDR_HIGH               ((unsigned long)IPCRIS_BASE_ADDR>>16)
#define IPCRIS_BASE_ADDR_LOW                (0x4000)

/*APM邮箱基地址*/
#define APM_BASE_ADDR                       (0x0)    /* _H2ASN_Skip */

#define SOC_TIMER_BASE_ADDR                 ((unsigned long)0x27067000 )

/****************************************************************************
 *                   各个地址空间基地址转定义                               *
 *                                                                          *
 ****************************************************************************/
#define SOC_WBBP_CLK_SEL_ADDR               ( SOC_BASE_ADDR + 0x148 )

/*============================DMA级联数组存放地址===============================*/
#define DMA_LLI_PARA_HIGH_ADDR                  (AHB_BASE_ADDR_HIGH)



/****************************************************************************
 *                   DSP使用的定时器定义                                    *
 *                                                                          *
 ****************************************************************************/

/******************************************************************************
 关于Timer的使用说明:
 1.早期使用的Dual-Timer不再使用，为了和北研保持一致，SOC增加了新Timer
 2.SOC 目前还继续保留 Dual-Timer，但没有分配使用
 3.默认给ZSP分配一个Timer,可同时启动8个定时器，前4个为32K时钟，后4个为19.2M时钟
 4.考虑到系统稳定性，不在NMI中断中响应Timer中断，SOC默认屏蔽字为关闭
 5.为了方便后续代码维护，下面描述的Timer0和Timer1的编号是虚拟的，在不同版本中
   会和芯片不同的TimerX对应，只要Timer的机制不改变，映射的过程仅仅是配置寄存器
   地址的变化。

 ******************************************************************************/

#define ARM_SOC_CPUVIEW_TIMER_VALUE         (0x90003000)    /* CPU View Timer调整，这里仅保证编译通过 */
#define ARM_SOC_CPUVIEW_TIMER_LOAD          (0x90003000)    /* CPU View Timer调整，这里仅保证编译通过 */
#define ARM_SOC_CPUVIEW_TIMER_CTRL          (0x90003000)    /* CPU View Timer调整，这里仅保证编译通过 */
#endif


