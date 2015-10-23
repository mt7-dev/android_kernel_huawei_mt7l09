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
#ifndef __DRV_WAKECLOCK_H__
#define __DRV_WAKECLOCK_H__


/******************************************************
*睡眠投票ID ,从0开始，最多32个
*涉及投票的组件需要在此添加LOCK ID
*请同步修改wakelock_balong.c中的debug_wakelock
*******************************************************/
typedef enum tagPWC_CLIENT_ID_E
{
    PWRCTRL_SLEEP_TLPS = 0,   /*MSP--fuxin*/
    PWRCTRL_SLEEP_PS_G0,        /*GU--ganlan*/
    PWRCTRL_SLEEP_PS_W0,       /*GU--ganlan*/
    PWRCTRL_SLEEP_PS_G1,        /*GU--ganlan*/
    PWRCTRL_SLEEP_PS_W1,       /*GU--ganlan*/
    PWRCTRL_SLEEP_FTM,           /*GU--zhangyizhan*/
    PWRCTRL_SLEEP_FTM_1,       /*GU--zhangyizhan*/
    PWRCTRL_SLEEP_NAS,           /*GU--zhangyizhan*/
    PWRCTRL_SLEEP_NAS_1,       /*GU--zhangyizhan*/
    PWRCTRL_SLEEP_OAM,		   /* LTE --yangzhi */
    PWRCTRL_SLEEP_SCI0,		   /* LTE --yangzhi */
    PWRCTRL_SLEEP_SCI1,		   /* LTE --yangzhi */
    PWRCTRL_SLEEP_DSFLOW,	   /* NAS --zhangyizhan */
    PWRCTRL_SLEEP_TEST,		   /* PM  ---shangmianyou */
    PWRCTRL_SLEEP_UART0,        /*UART0 -zhangliangdong */
    PWRCTRL_SLEEP_TDS,         /*TRRC&TL2----leixiantiao*/
    /*以下部分的ID已经不使用了，后期会删除*/
    PWRCTRL_SLEEP_RNIC,
    LOCK_ID_BOTTOM =32
}PWC_CLIENT_ID_E;

/*****************************************************************************
 函 数 名  : DRV_PWRCTRL_SLEEPVOTE_UNLOCK
 功能描述  : 外设允许睡眠投票接口。
 输入参数  : lock_id id
 输出参数  : None
 返 回 值  : 无
*****************************************************************************/

unsigned int BSP_PWRCTRL_SleepVoteLock(PWC_CLIENT_ID_E id);
unsigned int BSP_PWRCTRL_SleepVoteUnLock(PWC_CLIENT_ID_E id);

#define DRV_PWRCTRL_SLEEPVOTE_LOCK(_clk_id)     BSP_PWRCTRL_SleepVoteLock(_clk_id)
#define DRV_PWRCTRL_SLEEPVOTE_UNLOCK(_clk_id)     BSP_PWRCTRL_SleepVoteUnLock(_clk_id)
/*****************************************************************************
 函 数 名  : BSP_PWRCTRL_StandbyStateCcpu/BSP_PWRCTRL_StandbyStateAcpu
 功能描述  : AT^PSTANDBY
 输入参数  :
 输出参数  :
 返回值：
*****************************************************************************/
 unsigned int BSP_PWRCTRL_StandbyStateAcpu(unsigned int ulStandbyTime, unsigned int ulSwitchTime);
 unsigned int BSP_PWRCTRL_StandbyStateCcpu(unsigned int ulStandbyTime, unsigned int ulSwitchTime);
#define DRV_PWRCTRL_STANDBYSTATEACPU(ulStandbyTime, ulSwitchTime)	BSP_PWRCTRL_StandbyStateAcpu(ulStandbyTime, ulSwitchTime)
#define DRV_PWRCTRL_STANDBYSTATECCPU(ulStandbyTime, ulSwitchTime)	BSP_PWRCTRL_StandbyStateCcpu(ulStandbyTime, ulSwitchTime)

#endif

