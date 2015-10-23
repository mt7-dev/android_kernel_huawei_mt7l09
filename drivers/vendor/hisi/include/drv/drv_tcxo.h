/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  drv_clk.h
*
*   作    者 :  xujingcui
*
*   描    述 :  本文件命名为"drv_clk", 给出V7R2底软和协议栈之间的CLK_API接口
*
*   修改记录 :  2013年1月18日  v1.00 xujingcui创建
*************************************************************************/
#ifndef __DRV_TCXO_H__
#define __DRV_TCXO_H__

#include <drv_dpm.h>
typedef BSP_S32 (*PWC_TCXO_FUNCPTR)(int);


 int BSP_TCXO_RfclkStatus(PWC_COMM_MODEM_E enModemId);
#define DRV_TCXO_RFCLKSTATUS(enModemId) BSP_TCXO_RfclkStatus(enModemId)

 int BSP_TCXO_RfclkEnable(PWC_COMM_MODEM_E  enModemId, PWC_COMM_MODE_E enModeType);
#define DRV_TCXO_RFCLKENABLE(enModemId, enModeType) BSP_TCXO_RfclkEnable(enModemId, enModeType)

 int BSP_TCXO_RfclkDisable(PWC_COMM_MODEM_E  enModemId, PWC_COMM_MODE_E enModeType);
#define DRV_TCXO_RFCLKDISABLE(enModemId, enModeType) BSP_TCXO_RfclkDisable(enModemId, enModeType)

 int bsp_tcxo_getstatus(PWC_COMM_MODEM_E enModemId);
#define DRV_TCXO_GETSTATUS(enModemId)   bsp_tcxo_getstatus(enModemId)

 int bsp_tcxo_enable(PWC_COMM_MODEM_E  enModemId, PWC_COMM_MODE_E enModeType);
#define DRV_TCXO_ENABLE(enModemId, enModeType)  bsp_tcxo_enable(enModemId, enModeType)

 int bsp_tcxo_disable(PWC_COMM_MODEM_E  enModemId, PWC_COMM_MODE_E enModeType);
#define DRV_TCXO_DISABLE(enModemId, enModeType) bsp_tcxo_disable(enModemId, enModeType)

 int bsp_tcxo_timer_start(PWC_COMM_MODEM_E  enModemId, PWC_COMM_MODE_E enModeType, PWC_TCXO_FUNCPTR routine, int arg, unsigned int timerValue);
#define DRV_TCXO_TIMER_START(enModemId, enModeType, routine, arg, timerValue)   bsp_tcxo_timer_start(enModemId, enModeType, routine, arg, timerValue)





#endif
