/*************************************************************************
*   版权所有(C) 1987-2009, 深圳华为技术有限公司.
*
*   文 件 名 :  bsp_bbp.h
*
*   作    者 :
*
*   描    述 :  本文件命名为"bsp_bbp.h"
*
*   修改记录 :
*************************************************************************/
#ifndef _BSP_BBP_H_
#define _BSP_BBP_H_

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__CMSIS_RTOS)
#include "osl_common.h"
#else
#include "osl_types.h"
#endif
#include "soc_interrupts.h"
#include "drv_bbp.h"
#include "drv_dpm.h"
#include "drv_comm.h"
#include "drv_timer.h"
#include "bsp_om.h"

#define  bbp_print_error(fmt, ...)   (bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_BBP, "[bbp]: <%s> "fmt, __FUNCTION__, ##__VA_ARGS__))
#define  bbp_print_info(fmt, ...)    (bsp_trace(BSP_LOG_LEVEL_INFO, BSP_MODU_BBP, "[bbp]: <%s> "fmt, __FUNCTION__, ##__VA_ARGS__))

/*共享内存使用*/
#define IRM_POWRON_OFFSET 0

s32  bbp_int_init(void);
void bsp_bbp_timerintregcb(BSPBBPIntTimerFunc pfunc);
void bsp_bbp_dltbintregcb(BSPBBPIntDlTbFunc pfunc);
void bsp_bbp_timerintclear(void);
int bsp_bbp_timerintenable(void);
void bsp_bbp_timerintdisable(void);
u32 bsp_bbp_getcurtime(unsigned long long *pcurtime);
u16 bsp_bbp_getsysframe(void);
u16 bsp_bbp_getsyssubframe(void);

void bsp_bbp_tdstf_regcb(BSPBBPIntTdsFunc pFunc);
int bsp_bbp_tdstf_enable(void);
void bsp_bbp_tdstf_disable(void);

int bbp_pwrctrl_irm_poweron(void);
int bbp_pwrctrl_irm_poweroff(void);
PWC_COMM_STATUS_E bbp_pwrctrl_irm_status(void);

void bbp_wakeup_int_enable(PWC_COMM_MODE_E mode);
void bbp_wakeup_int_disable(PWC_COMM_MODE_E mode);
void bbp_wakeup_int_clear(PWC_COMM_MODE_E mode);
/*****************************************************************************
* 函数  : bsp_bbp_get_tds_subframe
* 功能  : get tds system sub frame num
* 输入  : void
* 输出  : void
* 返回  : u32
*****************************************************************************/
u16 bsp_bbp_get_tds_subframe(void);

/*****************************************************************************
* 函数  : bsp_bbp_set_tds_subframoffset
* 功能  : set tds system sub frame offset
* 输入  : void
* 输出  : void
* 返回  : u32
*****************************************************************************/
u16 bsp_bbp_set_tds_subframoffset(u16 sf_offset);

/*****************************************************************************
* 函数  : bsp_bbp_get_tds_sleep_time
* 功能  : Get tds sleep time
* 输入  : void
* 输出  : void
* 返回  : u32
*****************************************************************************/
u32 bsp_bbp_get_tds_sleep_time(void);

/*****************************************************************************
* 函数  : bsp_bbp_get_tdsclk_switch
* 功能  : get_tdsclk_switch
* 输入  : void
* 输出  : 0表示切换到系统时钟，1表示切换到32k时钟
* 返回  : void
*****************************************************************************/
int bsp_bbp_get_tdsclk_switch(void);

/*****************************************************************************
Function:   BSP_PWC_GetMeansFlag
Description:
Input:
Output:     the means flag value;
Return:
Others:
*****************************************************************************/
 BSP_S32 BSP_PWC_GetMeansFlag(PWC_COMM_MODE_E enCommMode);
/*****************************************************************************
* 函 数 : bbp_get_wakeup_time
* 功 能 : 被低功耗调用，用来查询睡眠超时时刻
* 输 入 : 无
* 输 出 : void
* 返 回 : bbp睡眠剩余时间，单位为32.768KHz时钟计数
*****************************************************************************/
u32 bsp_bbp_get_wakeup_time(void);
/*****************************************************************************
* 函 数 : bsp_bbp_g1_poweron
* 功 能 : 被低功耗调用，用来给G1 BBP上电
* 输 入 : void
* 输 出 : void
* 返 回 : 0为成功，-1失败
* 说 明 :
*****************************************************************************/
int bsp_bbp_pwrctrl_g1bbp_poweron(void);
/*****************************************************************************
* 函 数 : bsp_bbp_g1_poweroff
* 功 能 : 被低功耗调用，用来给G1 BBP下电
* 输 入 : 模式
* 输 出 : void
* 返 回 : 下电是否成功
* 说 明 :
*****************************************************************************/
int bsp_bbp_pwrctrl_g1bbp_poweroff(void);

/*****************************************************************************
* 函 数 : bsp_bbp_pwrctrl_g1_status
* 功 能 : 被低功耗调用，用来给G1 BBP上电
* 输 入 : void
* 输 出 : void
* 返 回 : 0x10为open, 0x20为close
* 说 明 :
*****************************************************************************/
PWC_COMM_STATUS_E bsp_bbp_pwrctrl_g1bbp_status(void);

/*****************************************************************************
* 函 数 : bsp_bbp_g2_poweron
* 功 能 : 被低功耗调用，用来给G2 BBP上电
* 输 入 : void
* 输 出 : void
* 返 回 : 0为成功，-1失败
* 说 明 :
*****************************************************************************/
int bsp_bbp_pwrctrl_g2bbp_poweron(void);

/*****************************************************************************
* 函 数 : bsp_bbp_g2_poweroff
* 功 能 : 被低功耗调用，用来给G1 BBP下电
* 输 入 : 模式
* 输 出 : void
* 返 回 : 下电是否成功
* 说 明 :
*****************************************************************************/
int bsp_bbp_pwrctrl_g2bbp_poweroff(void);

/*****************************************************************************
* 函 数 : bsp_bbp_pwrctrl_g2_status
* 功 能 : 被低功耗调用，用来给G1 BBP上电
* 输 入 : void
* 输 出 : void
* 返 回 : 0x10为open, 0x20为close
* 说 明 :
*****************************************************************************/
PWC_COMM_STATUS_E bsp_bbp_pwrctrl_g2bbp_status(void);

/*****************************************************************************
* 函 数 : bsp_bbp_wbbp_poweron
* 功 能 : 被低功耗调用，用来给W BBP上电
* 输 入 : void
* 输 出 : void
* 返 回 : 0为成功，-1失败
* 说 明 :
*****************************************************************************/
int bsp_bbp_pwrctrl_wbbp_poweron(void);

/*****************************************************************************
* 函 数 : bsp_bbp_wbbp_poweroff
* 功 能 : 被低功耗调用，用来给W BBP下电
* 输 入 : 模式
* 输 出 : void
* 返 回 : 下电是否成功
* 说 明 :
*****************************************************************************/
int bsp_bbp_pwrctrl_wbbp_poweroff(void);

/*****************************************************************************
* 函 数 : bsp_bbp_pwrctrl_wbbp_status
* 功 能 : 被低功耗调用，用来给W BBP上电
* 输 入 : void
* 输 出 : void
* 返 回 : 0x10为open, 0x20为close
* 说 明 :
*****************************************************************************/
PWC_COMM_STATUS_E bsp_bbp_pwrctrl_wbbp_status(void);

/*****************************************************************************
* 函 数 : bsp_bbp_twbbp_poweron
* 功 能 : 被低功耗调用，用来给TW BBP上电
* 输 入 : void
* 输 出 : void
* 返 回 : 0为成功，-1失败
* 说 明 :
*****************************************************************************/
int bsp_bbp_pwrctrl_twbbp_poweron(void);

/*****************************************************************************
* 函 数 : bsp_bbp_wbbp_poweroff
* 功 能 : 被低功耗调用，用来给W BBP下电
* 输 入 : 模式
* 输 出 : void
* 返 回 : 下电是否成功
* 说 明 :
*****************************************************************************/
int bsp_bbp_pwrctrl_twbbp_poweroff(void);

/*****************************************************************************
* 函 数 : bsp_bbp_pwrctrl_wbbp_status
* 功 能 : 被低功耗调用，用来给W BBP上电
* 输 入 : void
* 输 出 : void
* 返 回 : 0x10为open, 0x20为close
* 说 明 :
*****************************************************************************/
PWC_COMM_STATUS_E bsp_bbp_pwrctrl_twbbp_status(void);

/*****************************************************************************
* 函 数 : bsp_bbp_pwrctrl_tdsbbp_clk_enable
* 功 能 : 被低功耗调用，用来给TDS开钟
* 输 入 : void
* 输 出 : void
* 返 回 : 0为成功，-1失败
* 说 明 : 
*****************************************************************************/
int bsp_bbp_pwrctrl_tdsbbp_clk_enable(void);
/*****************************************************************************
* 函 数 : bsp_bbp_pwrctrl_tdsbbp_clk_disable
* 功 能 : 被低功耗调用，用来给TDS关钟
* 输 入 : void
* 输 出 : void
* 返 回 : 0为成功，-1失败
* 说 明 : 
*****************************************************************************/
int bsp_bbp_pwrctrl_tdsbbp_clk_disable(void);

/*****************************************************************************
* 函 数 : bsp_bbp_pwrctrl_wbbp_clk_disable
* 功 能 : 被低功耗调用，用来给w模关钟
* 输 入 : void
* 输 出 : void
* 返 回 : 0为成功，-1失败
* 说 明 : 
*****************************************************************************/
int bsp_bbp_pwrctrl_wbbp_clk_disable(void);

/*****************************************************************************
* 函 数 : bsp_bbp_pwrctrl_tdsbbp_clk_enable
* 功 能 : 被低功耗调用，用来给W开钟
* 输 入 : void
* 输 出 : void
* 返 回 : 0为成功，-1失败
* 说 明 : 
*****************************************************************************/
int bsp_bbp_pwrctrl_wbbp_clk_enable(void);
/*****************************************************************************
* 函 数 : bsp_bbp_lps_get_ta
* 功 能 : 被lps调用,查看上行子帧头相对系统子帧头的提前量
* 输 入 : void
* 输 出 : void
* 返 回 : 
* 说 明 : 
*****************************************************************************/
u16 bsp_bbp_lps_get_ta(void);
u32 bbp_get_wakeup_time(PWC_COMM_MODE_E mode);

int bsp_bbp_int_debug(void);

u32 bsp_bbp_get_gubbp_wakeup_status(void);

#ifdef __cplusplus
}
#endif

#endif /*end #ifndef _BSP_DRV_SCI_H_*/


