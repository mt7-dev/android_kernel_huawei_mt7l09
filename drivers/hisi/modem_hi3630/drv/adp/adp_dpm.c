#include "drv_wakelock.h"
#include "drv_dpm.h"

/*****************************************************************************
 函 数 名  : DRV_PWRCTRL_PWRUP
 功能描述  : W模、G模、L模下PA、RF、BBP、DSP、ABB、HIFI的上电控制。
 输入参数  : enCommMode: PWC_COMM_MODE_WCDMA/PWC_COMM_MODE_GSM/PWC_COMM_MODE_LTE,
             enCommModule:PWC_COMM_MODULE_PA/PWC_COMM_MODULE_RF/PWC_COMM_MODULE_BBP/PWC_COMM_MODULE_DSP/PWC_COMM_MODULE_ABB,
 输出参数  : None
 返 回 值  : PWC_PARA_INVALID/PWC_SUCCESS
*****************************************************************************/
BSP_U32 DRV_PWRCTRL_PWRUP(PWC_COMM_MODE_E enCommMode, PWC_COMM_MODULE_E enCommModule, PWC_COMM_MODEM_E enModemId)
 {
    return PWC_COMM_MODEM_ON;
 }
/*****************************************************************************
 函 数 名  : DRV_PWRCTRL_PWRDOWN
 功能描述  : W模、G模、L模下PA、RF、BBP、DSP、ABB、HIFI的下电控制。
 输入参数  : enCommMode: PWC_COMM_MODE_WCDMA/PWC_COMM_MODE_GSM/PWC_COMM_MODE_LTE,
             enCommModule:PWC_COMM_MODULE_PA/PWC_COMM_MODULE_RF/PWC_COMM_MODULE_BBP/PWC_COMM_MODULE_DSP/PWC_COMM_MODULE_ABB,
 输出参数  : None
 返 回 值  : PWC_PARA_INVALID/PWC_SUCCESS
*****************************************************************************/
 BSP_U32 DRV_PWRCTRL_PWRDOWN (PWC_COMM_MODE_E enCommMode, PWC_COMM_MODULE_E enCommModule, PWC_COMM_MODEM_E enModemId)
 {
    return PWC_COMM_MODEM_OFF;
 }
/*****************************************************************************
 函 数 名  : BSP_PWRCTRL_PwrStatusGet
 功能描述  : W模、G模、L模下PA、RF、BBP、DSP、ABB的上下电状态查询。
 输入参数  : enCommMode: PWC_COMM_MODE_WCDMA/PWC_COMM_MODE_GSM/PWC_COMM_MODE_LTE,
             enCommModule:PWC_COMM_MODULE_PA/PWC_COMM_MODULE_RF/PWC_COMM_MODULE_BBP/PWC_COMM_MODULE_BBE16/PWC_COMM_MODULE_ABB,
 输出参数  : None
 返 回 值  : PWRCTRL_GET_SUCCESS/PWRCTRL_GET_PARAINVALID/PWRCTRL_ONORLOCK/PWRCTRL_OFFORUNLOCK
*****************************************************************************/
  BSP_U32 DRV_PWRCTRL_PWRSTATUSGET(PWC_COMM_MODE_E enCommMode, PWC_COMM_MODULE_E enCommModule, PWC_COMM_MODEM_E enModemId)
 {
	return 0;
 }

/*****************************************************************************
 函 数 名  : DRV_PWRCTRL_PLL_ENABLE
 功能描述  : W模、G模、L模下PA、DSP、ABB的PLL使能。
 输入参数  : enCommMode: PWC_COMM_MODE_WCDMA/PWC_COMM_MODE_GSM/PWC_COMM_MODE_LTE,
             enCommModule:PWC_COMM_MODULE_PA/PWC_COMM_MODULE_DSP/PWC_COMM_MODULE_ABB,
 输出参数  : None
 返 回 值  : PWC_PARA_INVALID/PWC_SUCCESS
*****************************************************************************/
 BSP_U32 DRV_PWRCTRL_PLLENABLE (PWC_COMM_MODE_E enCommMode, PWC_COMM_MODULE_E enCommModule, PWC_COMM_MODEM_E enCommModem)
 {
	return PWC_COMM_MODEM_ON;
 }

/*****************************************************************************
 函 数 名  : DRV_PWRCTRL_PLL_DISABLE
 功能描述  : W模、G模、L模下PA、DSP、ABB的PLL去使能。
 输入参数  : enCommMode: PWC_COMM_MODE_WCDMA/PWC_COMM_MODE_GSM/PWC_COMM_MODE_LTE,
             enCommModule:PWC_COMM_MODULE_PA/PWC_COMM_MODULE_DSP/PWC_COMM_MODULE_ABB,
 输出参数  : None
 返 回 值  : PWC_PARA_INVALID/PWC_SUCCESS
*****************************************************************************/
 BSP_U32 DRV_PWRCTRL_PLL_DISABLE (PWC_COMM_MODE_E enCommMode, PWC_COMM_MODULE_E enCommModule, PWC_COMM_MODEM_E enCommModem)
 {
	return PWC_COMM_MODEM_OFF;
 }
/*****************************************************************************
* 函 数 名  : DRV_PWRCTRL_DSP_RESET
* 功能描述  : L模下DSP复位接口
* 输入参数  :
* 输出参数  :
* 返 回 值  :
*****************************************************************************/
 int  DRV_PWRCTRL_DSP_RESET(void)
 {
	return 0;
 }


/*****************************************************************************
 函 数 名  : BSP_PWRCTRL_PllStatusGet
 功能描述  : W模、G模、L模下PA、DSP、ABB的PLL状态查询。
 输入参数  : enCommMode: PWC_COMM_MODE_WCDMA/PWC_COMM_MODE_GSM/PWC_COMM_MODE_LTE,
             enCommModule:PWC_COMM_MODULE_PA/PWC_COMM_MODULE_DSP/PWC_COMM_MODULE_ABB,
 输出参数  : None
 返 回 值  : PWRCTRL_GET_SUCCESS/PWRCTRL_GET_PARAINVALID/PWRCTRL_ONORLOCK/PWRCTRL_OFFORUNLOCK
*****************************************************************************/
 BSP_U32 DRV_PWRCTRL_PLLSTATUSGET (PWC_COMM_MODE_E enCommMode, PWC_COMM_MODULE_E enCommModule, PWC_COMM_MODEM_E enCommModem)
 {
	return 0;
 }

/*****************************************************************************
 函 数 名  : DRV_PWRCTRL_SLEEPVOTE_LOCK
 功能描述  : 外设禁止睡眠投票接口。
 输入参数  : enClientId:PWC_CLIENT_ID_E
 输出参数  : None
 返 回 值  : PWC_PARA_INVALID/PWC_SUCCESS
*****************************************************************************/
 BSP_U32 DRV_PWRCTRL_SLEEPVOTE_LOCK(PWC_CLIENT_ID_E enClientId)
 {
	return 0;
 }

/*****************************************************************************
 函 数 名  : DRV_PWRCTRL_SLEEPVOTE_UNLOCK
 功能描述  : 外设允许睡眠投票接口。
 输入参数  : enClientId:PWC_CLIENT_ID_E
 输出参数  : None
 返 回 值  : PWC_PARA_INVALID/PWC_SUCCESS
*****************************************************************************/
 BSP_U32 DRV_PWRCTRL_SLEEPVOTE_UNLOCK(PWC_CLIENT_ID_E enClientId)
 {
	return 0;
 }


/*****************************************************************************
 函 数 名  : BSP_PWRCTRL_StandbyStateCcpu/BSP_PWRCTRL_StandbyStateAcpu
 功能描述  : AT^PSTANDBY
 输入参数  :
 输出参数  :
 返回值：
*****************************************************************************/
 unsigned int BSP_PWRCTRL_StandbyStateAcpu(unsigned int ulStandbyTime, unsigned int ulSwitchTime)
 {
	return 0;
 }
 unsigned int BSP_PWRCTRL_StandbyStateCcpu(unsigned int ulStandbyTime, unsigned int ulSwitchTime)
 {
	return 0;
 }

/*****************************************************************************
 函 数 名  : BSP_GUSYS_RFLdoDown
 功能描述  : 本接口实现WCDMA和GSM RF LDO下电。
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
void BSP_GUSYS_RFLdoDown(void)
{

}
/*****************************************************************************
 函 数 名  : BSP_GUSYS_RFLdoOn
 功能描述  : 本接口实现WCDMA和GSM RF LDO上电。
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
void BSP_GUSYS_RFLdoOn(void)
{

}

 /*****************************************************************************
 函 数 名  : BSP_GUSYS_BBPAutoAdjust
 功能描述  : 初始化ABB的自校准使能
 输入参数  : uiSysMode：0：WCDMA模式；1：GSM模式。
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
int  BSP_GUSYS_BBPAutoAdjust(unsigned char ucAdjustMode)
{
    return OK;
}


