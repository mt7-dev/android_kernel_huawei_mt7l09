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

#ifndef __DRV_TIMER_H__
#define __DRV_TIMER_H__

#include "drv_comm.h"
#include "soc_timer_enum.h"
/*************************TIMER BEGIN*****************************/

typedef enum
{
	TIMER_CCPU_DSP_DRX_PROT_ID              =CCPU_DSP_DRX_TIMER,              /*v9r1 新添加，用途待确认，先保证编译通过    liujing*/
	ACORE_SOFTTIMER_ID                      =ACPU_SOFTTIMER_WAKE,             /*A core 唤醒源软timer id                lixiaojie*/
	CCORE_SOFTTIMER_ID                      =CCPU_SOFTTIMER_WAKE,             /*C core 唤醒源软timer id                lixiaojie*/
	ACORE_WDT_TIMER_ID                      =ACPU_WDT_TIMER,                  /*A CORE看门狗                           yangqiang*/
	CCORE_WDT_TIMER_ID                      =CCPU_WDT_TIMER,                  /*C CORE看门狗                           yangqiang*/
	TIME_STAMP_ID                           =STAMP_TIMER,                     /*P531上使用，A core C CORE M3时间戳,在fastboot阶段使能 ，V7R2使用BBP时间戳lixiaojie*/              
	USB_TIMER_ID                            =USB_TIMER,                       /*A CORE    V7R2 USB使用                lvhui*/
	CCORE_SOFTTIMER_NOWAKE_ID               =CCPU_SOFTTIMER_NOWAKE,           /*C core 非唤醒源软timer                 lixiaojie*/
	ACORE_SOFTTIMER_NOWAKE_ID               =ACPU_SOFTTIMER_NOWAKE,           /*A core 非唤醒源软timer                 lixiaojie*/
	TIMER_DSP_TIMER2_ID                     =DSP_TIMER2,                      /*DSP使用                                 liujing*/
	TIMER_MCPU_ID                           =MCU_TIMER,                       /*M3使用                                lixiaojie*/
	TIMER_HIFI_TIMER2_ID                    =HIFI_TIMER2,                     /*hifi使用                              fuying*/
	TIMER_UDELAY_TIMER_ID                   =UDELAY_TIMER,                    /*c核提供udelay函数使用，在fastboot阶段使能              lixiaojie*/
	CCORE_SYSTEM_TIMER_ID                   =CCPU_SYSTIMER,                   /*C Core系统时钟                        luting*/
	ACORE_SYSTEM_TIMER_ID                   =ACPU_SYSTIMER,                   /*A CORE系统时钟                        luting*/
	ACORE_SOURCE_TIMER_ID                   =ACPU_SOURCE_TIMER,               /*A CORE 事件源                        luting */
	TIMER_ACPU_CPUVIEW_ID                   =ACPU_VIEW_TIMER,                 /*A CORE CPU VIEW                      duxiaopeng*/
	TIMER_CCPU_CPUVIEW_ID                   =CCPU_VIEW_TIMER,                 /*C CORE CPU VIEW                     duxiaopeng*/
	TIMER_HIFI_TIMER1_ID                    =HIFI_TIMER1,                     /*hifi使用                             fuying*/
	TIMER_ACPU_OSA_ID                       =ACPU_OSA_TIMER,                  /*A CORE软件定时                        cuijunqiang*/
	TIMER_CCPU_OSA_ID                       =CCPU_OSA_TIMER,                  /*C CORE软件定时                        cuijunqiang*/
	TIMER_CCPU_DRX1_STABLE_ID               =CCPU_DRX1,                       /*C CORE tcxo稳定时钟xujingcui            */
	TIMER_DSP_TIMER1_ID                     =DSP_TIMER1,                      /*DSP使用                             liujing*/
	TIMER_CCPU_DRX2_STABLE_ID               =CCPU_DRX2  ,                     /*C CORE tcxo稳定时钟xujingcui                    */
	TIMER_CCPU_DRX_TIMER_ID                 =CCPU_DRX_TIMER,                  /*处理非实时性唤醒,cuijunqiang          */
	TIMER_ACPU_OM_TCXO_ID                   =ACPU_OM_TIMER,                   /*解决vos timer频繁唤醒, cuijunqiang    */
	TIMER_DSP_SWITCH_DELAY_ID               =DSP_SWITCH_DELAY_ID,             /*解决低功耗dsp启动延时, cuijunqiang    */
	TIMER_CCPU_G1PHY_DRX_ID                 =1000 ,                           /*只有V9R1使用，V7R2和K3V3等产品形态打桩:luoqingquan*/
	TIMER_ID_MAX                            =24
} DRV_TIMER_ID;

typedef enum
{
    TIMER_ONCE_COUNT = 0,        /* 单次定时器模式 */
    TIMER_PERIOD_COUNT,          /* 周期定时器模式 */
    TIMER_FREERUN_COUNT,         /* 自由定时器模式 */
    TIMER_COUNT_BUTT
}DRV_TIMER_MODE;

typedef enum
{
    TIMER_UNIT_MS = 0,           /* 0表示单位ms模式 */
    TIMER_UNIT_US,               /* 1表示单位us模式 */
    TIMER_UNIT_NONE,                /* 2表示单位1，即直接操作load寄存器模式  */
    TIMER_UNIT_BUTT
}DRV_TIMER_UNIT;

BSP_VOID DRV_TIMER_DEBUG_REGISTER(unsigned int timer_id,FUNCPTR_1 routinue, int arg); 


extern int DRV_TIMER_START
(
    unsigned int     usrClkId,
    FUNCPTR_1        routine,
    int              arg,
    unsigned int     timerValue,
    unsigned int     mode,
    unsigned int     unitType
);


extern int DRV_TIMER_STOP(unsigned int usrClkId);


extern int DRV_TIMER_GET_REST_TIME(unsigned int usrClkId, unsigned int unitType, unsigned int * pRestTime);

/************************************************************************
 * FUNCTION
 *       BSP_GetHardTimerCurTime
 * DESCRIPTION
 *       Get hard timer Current Value
 * INPUTS
 *       NONE
 * OUTPUTS
 *       NONE
     RTC OM
 *************************************************************************/
extern BSP_VOID BSP_ClearTimerINT(BSP_VOID);
#define DRV_CLEAR_TIMER_INT()   BSP_ClearTimerINT()
/************************************************************************
 * FUNCTION
 *       BSP_StartHardTimer
 * DESCRIPTION
 *       start hard timer
 * INPUTS
 *       value -- timer's value.uint is 32K cycle
 * OUTPUTS
 *       NONE
    RTC OM
 *************************************************************************/
extern BSP_VOID BSP_StartHardTimer(BSP_U32 value);
#define DRV_STATR_HARD_TIMER(value)   BSP_StartHardTimer(value)
/************************************************************************
 * FUNCTION
 *       BSP_StopHardTimer
 * DESCRIPTION
 *       Stop hard timer
 * INPUTS
 *       NONE
 * OUTPUTS
 *       NONE
      RTC OM
 *************************************************************************/
extern BSP_VOID BSP_StopHardTimer(BSP_VOID);
#define DRV_STOP_HARD_TIMER()   BSP_StopHardTimer()
/************************************************************************
 * FUNCTION
 *       BSP_GetHardTimerCurTime
 * DESCRIPTION
 *       GetHardTimerCurTime
 * INPUTS
 *       NONE
 * OUTPUTS
 *       NONE
    RTC OM
 *************************************************************************/
extern BSP_U32 BSP_GetHardTimerCurTime(BSP_VOID);
#define DRV_GET_TIMER_CUR_TIME()   BSP_GetHardTimerCurTime()
/************************************************************************
 * FUNCTION
 *       BSP_GetSliceValue
 * DESCRIPTION
 *       GetSliceValue
 * INPUTS
 *       NONE
 * OUTPUTS
 *       NONE
       获取时间戳，时间戳返回值为递增值
 *************************************************************************/
extern unsigned int BSP_GetSliceValue(void);
#define DRV_GET_SLICE()   BSP_GetSliceValue()
#define PWRCTRL_GetSleepSlice() BSP_GetSliceValue()

/************************************************************************
 * FUNCTION
 *       bsp_get_32k_ms
 * DESCRIPTION
 *       GetSliceValue
 * INPUTS
 *       NONE
 * OUTPUTS
 *       NONE
       获取时间戳，时间戳返回值为递增值
 *************************************************************************/
extern unsigned int bsp_get_32k_ms(void);
#define DRV_GET_32K_MS()   bsp_get_32k_ms()

UINT32 omTimerGet(void);
UINT32 omTimerTickGet(void);
/*****************************************************************************
 函 数 名  : BSP_32K_GetTick
 功能描述  : 32K时钟对应Tick查询接口
 输入参数  : None
 输出参数  : None
 返 回 值  : 32K时钟对应Tick值
*****************************************************************************/
 BSP_U32 BSP_32K_GetTick( BSP_VOID );

/*****************************************************************************
 Function   : BSP_PWC_SetTimer4WakeSrc
 Description: 设置timer4作为唤醒源
 Input      :
 Return     : void
 Other      :
*****************************************************************************/
 VOID BSP_PWC_SetTimer4WakeSrc(VOID);

/*****************************************************************************
 Function   : BSP_PWC_DelTimer4WakeSrc
 Description: 设置timer4不作为唤醒源
 Input      :
            :
 Return     : void
 Other      :
*****************************************************************************/
 VOID BSP_PWC_DelTimer4WakeSrc(VOID);
   
#endif

