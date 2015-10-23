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

#ifndef __DRV_DPM_H__
#define __DRV_DPM_H__

#include "drv_comm.h"


/*************************DPM START***********************************/
#ifndef PWC_SUCCESS
#define PWC_SUCCESS        0
#endif

#ifndef PWC_PARA_INVALID
#define PWC_PARA_INVALID				0x1
#endif

/* 返回值定义 */
typedef enum
{
    RET_ERR_PARAM_NULL = -0xF000,
    RET_ERR_PARAM_INVALID,
    RET_ERR_OVERFLOW,
    RET_ERR_INSUFFICIENT,
    RET_ERR_LIMITED,
    RET_ERR_NOT_FOUND,
    RET_ERR_NOT_OPEN,
    RET_ERR_DUPLICATE,
    RET_ERR_CONNECT_FAIL,
    RET_ERR_TIME_OUT,
    RET_ERR_UNKNOWN,
    RET_ERR = -1,
    RET_OK = 0
} EM_PWC_RET;

#define PWRCTRL_GET_SUCCESS         (0x00)
#define PWRCTRL_GET_PARAINVALID     (0x01)
#define PWRCTRL_ONORLOCK            (0x10)
#define PWRCTRL_OFFORUNLOCK         (0x20)


typedef enum tagPWC_COMM_MODE_E
{
    PWC_COMM_MODE_GSM,
    PWC_COMM_MODE_WCDMA,
    PWC_COMM_MODE_LTE,
    PWC_COMM_MODE_TDS,
    PWC_COMM_MODE_LCIPHER,
    PWC_COMM_MODE_BUTT
}PWC_COMM_MODE_E;

typedef enum tagPWC_COMM_MODULE_E
{
    PWC_COMM_MODULE_PA,
    PWC_COMM_MODULE_RF,
    PWC_COMM_MODULE_BBE16,
    PWC_COMM_MODULE_BBP_DRX,
    PWC_COMM_MODULE_BBP_DPA,
    PWC_COMM_MODULE_BBP_MEM,
    PWC_COMM_MODULE_ABB,

    /*GU的OAM还在使用以下枚举*/
    PWC_COMM_MODULE_BBP_PWR,
    PWC_COMM_MODULE_DSP,
    PWC_COMM_MODULE_BBP_SOC,
    PWC_COMM_MODULE_BUTT
}PWC_COMM_MODULE_E;

typedef enum tagPWC_COMM_MODEM_E
{
    PWC_COMM_MODEM_0=0,
    PWC_COMM_MODEM_1=1,
    PWC_COMM_MODEM_BUTT
}PWC_COMM_MODEM_E;

typedef enum tagPWC_COMM_STATUS_E
{
    PWC_COMM_MODEM_ON = 0x10,
    PWC_COMM_MODEM_OFF = 0x20,
/* for v9r1 compact */
    PWRCTRL_COMM_ON = 0x10,
    PWRCTRL_COMM_OFF = 0x20,
    PWC_COMM_STATUS_BUTT
}PWC_COMM_STATUS_E;

/*****************************************************************************
 函 数 名  : DRV_PWRCTRL_PWRUP
 功能描述  : W模、G模、L模下PA、RF、BBP、DSP、ABB、HIFI的上电控制。
 输入参数  : enCommMode: PWC_COMM_MODE_WCDMA/PWC_COMM_MODE_GSM/PWC_COMM_MODE_LTE,
             enCommModule:PWC_COMM_MODULE_PA/PWC_COMM_MODULE_RF/PWC_COMM_MODULE_BBP/PWC_COMM_MODULE_DSP/PWC_COMM_MODULE_ABB,
 输出参数  : None
 返 回 值  : PWC_PARA_INVALID/PWC_SUCCESS
*****************************************************************************/
 BSP_U32 DRV_PWRCTRL_PWRUP (PWC_COMM_MODE_E enCommMode, PWC_COMM_MODULE_E enCommModule, PWC_COMM_MODEM_E enModemId);

/*****************************************************************************
 函 数 名  : DRV_PWRCTRL_PWRDOWN
 功能描述  : W模、G模、L模下PA、RF、BBP、DSP、ABB、HIFI的下电控制。
 输入参数  : enCommMode: PWC_COMM_MODE_WCDMA/PWC_COMM_MODE_GSM/PWC_COMM_MODE_LTE,
             enCommModule:PWC_COMM_MODULE_PA/PWC_COMM_MODULE_RF/PWC_COMM_MODULE_BBP/PWC_COMM_MODULE_DSP/PWC_COMM_MODULE_ABB,
 输出参数  : None
 返 回 值  : PWC_PARA_INVALID/PWC_SUCCESS
*****************************************************************************/
 BSP_U32 DRV_PWRCTRL_PWRDOWN (PWC_COMM_MODE_E enCommMode, PWC_COMM_MODULE_E enCommModule, PWC_COMM_MODEM_E enModemId);

/*****************************************************************************
 函 数 名  : BSP_PWRCTRL_PwrStatusGet
 功能描述  : W模、G模、L模下PA、RF、BBP、DSP、ABB的上下电状态查询。
 输入参数  : enCommMode: PWC_COMM_MODE_WCDMA/PWC_COMM_MODE_GSM/PWC_COMM_MODE_LTE,
             enCommModule:PWC_COMM_MODULE_PA/PWC_COMM_MODULE_RF/PWC_COMM_MODULE_BBP/PWC_COMM_MODULE_DSP/PWC_COMM_MODULE_ABB,
 输出参数  : None
 返 回 值  : PWRCTRL_GET_SUCCESS/PWRCTRL_GET_PARAINVALID/PWRCTRL_ONORLOCK/PWRCTRL_OFFORUNLOCK
*****************************************************************************/
 BSP_U32 DRV_PWRCTRL_PWRSTATUSGET(PWC_COMM_MODE_E enCommMode, PWC_COMM_MODULE_E enCommModule, PWC_COMM_MODEM_E enModemId);

/*****************************************************************************
 函 数 名  : DRV_PWRCTRL_PLL_ENABLE
 功能描述  : W模、G模、L模下PA、DSP、ABB的PLL使能。
 输入参数  : enCommMode: PWC_COMM_MODE_WCDMA/PWC_COMM_MODE_GSM/PWC_COMM_MODE_LTE,
             enCommModule:PWC_COMM_MODULE_PA/PWC_COMM_MODULE_DSP/PWC_COMM_MODULE_ABB,
 输出参数  : None
 返 回 值  : PWC_PARA_INVALID/PWC_SUCCESS
*****************************************************************************/
 BSP_U32 DRV_PWRCTRL_PLLENABLE(PWC_COMM_MODE_E enCommMode, PWC_COMM_MODULE_E enCommModule, PWC_COMM_MODEM_E enCommModem);

/*****************************************************************************
 函 数 名  : DRV_PWRCTRL_PLL_DISABLE
 功能描述  : W模、G模、L模下PA、DSP、ABB的PLL去使能。
 输入参数  : enCommMode: PWC_COMM_MODE_WCDMA/PWC_COMM_MODE_GSM/PWC_COMM_MODE_LTE,
             enCommModule:PWC_COMM_MODULE_PA/PWC_COMM_MODULE_DSP/PWC_COMM_MODULE_ABB,
 输出参数  : None
 返 回 值  : PWC_PARA_INVALID/PWC_SUCCESS
*****************************************************************************/
 BSP_U32 DRV_PWRCTRL_PLLDISABLE (PWC_COMM_MODE_E enCommMode, PWC_COMM_MODULE_E enCommModule, PWC_COMM_MODEM_E enCommModem);


/*****************************************************************************
 函 数 名  : BSP_PWRCTRL_PllStatusGet
 功能描述  : W模、G模、L模下PA、DSP、ABB的PLL状态查询。
 输入参数  : enCommMode: PWC_COMM_MODE_WCDMA/PWC_COMM_MODE_GSM/PWC_COMM_MODE_LTE,
             enCommModule:PWC_COMM_MODULE_PA/PWC_COMM_MODULE_DSP/PWC_COMM_MODULE_ABB,
 输出参数  : None
 返 回 值  : PWRCTRL_GET_SUCCESS/PWRCTRL_GET_PARAINVALID/PWRCTRL_ONORLOCK/PWRCTRL_OFFORUNLOCK
*****************************************************************************/
BSP_U32 DRV_PWRCTRL_PLLSTATUSGET (PWC_COMM_MODE_E enCommMode, PWC_COMM_MODULE_E enCommModule, PWC_COMM_MODEM_E enCommModem);

/***************************************************************************
 函 数 名  : DRV_PWRCTRL_BBPCLKENABLE()
 功能描述  : TCXO 定时器启动
 输入参数  : enModeId      通信模式
    	                     enModuleId    通信模块
    	                     enModemId    卡号
 输出参数  : 无
 返 回 值  : RET_OK         成功
                         RET_ERR        失败
**************************************************************************/
BSP_S32	DRV_PWRCTRL_BBPCLKENABLE(PWC_COMM_MODE_E enModeId, PWC_COMM_MODULE_E enModuleId, PWC_COMM_MODEM_E enModemId);

/***************************************************************************
 函 数 名  : DRV_PWRCTRL_BBPCLKDISABLE()
 功能描述  : TCXO 定时器启动
 输入参数  : enModeId      通信模式
    	                     enModuleId    通信模块
    	                     enModemId    卡号
 输出参数  : 无
 返 回 值  : RET_OK         成功
                         RET_ERR        失败
**************************************************************************/
BSP_S32	DRV_PWRCTRL_BBPCLKDISABLE(PWC_COMM_MODE_E enModeId, PWC_COMM_MODULE_E enModuleId, PWC_COMM_MODEM_E enModemId);


#endif

