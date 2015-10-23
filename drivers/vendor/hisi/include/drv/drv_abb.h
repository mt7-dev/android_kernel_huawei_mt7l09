/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  DrvInterface.h
*
*   作    者 :  yangzhi
*
*   描    述 :  本文件命名为"DrvInterface.h", 给出V7R1底软和协议栈之间的API接口统计
*
*   修改记录 :  2011年1月18日  v1.00  yangzhi创建
*************************************************************************/

#ifndef __DRV_ABB_H__
#define __DRV_ABB_H__
#ifdef __cplusplus
extern "C" {
#endif

#include "drv_comm.h"

/*************************ABB START***********************************/

extern int bsp_abb_write(unsigned int regAddr, unsigned int setData);
#define DRV_ABB_REG_SET(regAddr, setData) bsp_abb_write(regAddr, setData)


extern int bsp_abb_read(unsigned int regAddr, unsigned int * getData);
#define DRV_ABB_REG_GET(regAddr, getData) bsp_abb_read(regAddr, getData)


static INLINE int DRV_AUX_ABB_REG_SET(unsigned int regAddr, unsigned int setData)
{
    return 0;     /* 打桩,mohaoju确认 */
}
static INLINE int DRV_AUX_ABB_REG_GET(unsigned int regAddr, unsigned int * getData)
{
    return 0;    /* 打桩,mohaoju确认 */
}

/*****************************************************************************
 函 数 名  : BSP_MNTN_ABBSelfCheck
 功能描述  : 返回abb自检结果
 输入参数  : None
 输出参数  : None
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
static INLINE int BSP_MNTN_ABBSelfCheck(void)
{
	return 0;
}
#define DRV_ABB_SELFCHECK()    BSP_MNTN_ABBSelfCheck()

/*****************************************************************************
 函 数 名  : BSP_GUSYS_ABBGSMRxCtrl
 功能描述  : 本接口实现ABB GSM接收控制。
 输入参数  : ucStatus：0：关闭；1：打开。
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
static INLINE unsigned int  DRV_ABB_GRX_CTRL(unsigned char ucStatus)
{
    return OK;     /* 打桩 */
}


/*****************************************************************************
 函 数 名  : BSP_GUSYS_GsmPllStatusGet
 功能描述  : 读取GSM BBP PLL稳定状态。
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 0：稳定
             1：未稳定
*****************************************************************************/
static INLINE unsigned long DRV_GET_GSM_PLL_STATUS(void)
{
#if 0
    unsigned char ucData;

    /* 调用ABB的寄存器读取接口BSP_ABB_RegGet，读取GPLL状态；*/
    if(OK == BSP_ABB_RegGet(GSM_PLL_REG, &ucData))
    {
        if(ucData & (1 << GSM_PLL_STABLE_BIT))
        {
            return OK;
        }
        else
        {
            return 1;
        }
    }
    else
#endif
    {
        return 1;
    }
}


/*****************************************************************************
 函 数 名  : BSP_GUSYS_WcdmaPllStatusGet
 功能描述  : 读取WCDMA BBP PLL稳定状态。
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 0：稳定
             1：未稳定
*****************************************************************************/
static INLINE unsigned long DRV_GET_WCDMA_PLL_STATUS(void)
{
#if 0   /* v7r1的实现 */
    unsigned char ucData;

    /* 调用ABB的寄存器读取接口BSP_ABB_RegGet，读取WPLL状态；*/
    if(OK == BSP_ABB_RegGet(WCDMA_PLL_REG, &ucData))
    {
        if(ucData & (1 << WCDMA_PLL_STABLE_BIT))
        {
            return OK;
        }
        else
        {
            return 1;
        }
    }
    else
#endif
    {
        return 1;
    }
}

/*****************************************************************************
 函 数 名  : BSP_GUSYS_ABBWCDMARxCtrl
 功能描述  : 本接口实现ABB WCDMA接收控制。
 输入参数  : ucStatus：0：关闭；1：打开。
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
                        -1：操作失败。
*****************************************************************************/
static INLINE unsigned int  DRV_ABB_WRX_CTRL(unsigned char ucStatus)
{
    return OK;     /* 打桩 */
}

/*****************************************************************************
 函 数 名  : BSP_GUSYS_DspPllStatusGet
 功能描述  : 读取DSP PLL稳定状态。
 输入参数  : 无。
 输出参数  :无。
 返 回 值  :
                     0：稳定
                     1：未稳定
*****************************************************************************/
static INLINE unsigned long DRV_GET_DSP_PLL_STATUS(void)
{
    return OK;     /* 打桩 */
}

/* 适配V9R1打桩，在adp_dpm.c中实现 */
extern void DRV_DSP_AHB_RESET(void);
extern void DRV_DSP_AHB_RESET_CANCEL(void);

/*************************ABB END*************************************/

#ifdef __cplusplus
}
#endif

#endif

