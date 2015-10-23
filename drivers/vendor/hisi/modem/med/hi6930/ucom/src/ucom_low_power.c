

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "ucom_low_power.h"
#include "med_drv_ipc.h"
#include "med_drv_timer_hifi.h"
#include "med_drv_watchdog.h"
#include "HifiOmInterface.h"
#include "med_drv_mb_hifi.h"
#include "CodecNvInterface.h"
#include "ucom_nv.h"
#include "om_cpuview.h"
#include "soc_baseaddr_interface.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
UCOM_POWER_CTRL_STRU                    g_uwUcomPowerCtrl;

/*****************************************************************************
  3 函数实现
*****************************************************************************/
extern VOS_UINT32                       VOICE_ApiIsIdle(VOS_VOID);
//extern VOS_UINT32                       AUDIO_COMM_IsIdle(VOS_VOID);


VOS_VOID UCOM_LOW_PowerTask(VOS_UINT32 ulVosCtxSw)
{
    VOS_UINT32                              uwTimeStamps  = DRV_TIMER_ReadSysTimeStamp();
    VOS_UINT32                              uwIdleTime    = 0;
    VOS_UINT32                              uwVoiceIsIdle = VOS_OK;
    VOS_UINT32                              uwAudioIsIdle = VOS_OK;
    VOS_CPU_SR                              srSave;

    /* 锁中断 */
    srSave = VOS_SplIMP();

    /* 若第一次上电后进入Idle任务，则触发IPC中断通知C-ARM，HIFI启动完毕 */
    if (UCOM_IS_FIRST_IDLE())
    {
        #if 0
        /* 触发中断,标明此次中断为HIFI启动完毕中断，此功能MCU暂未实现，后续实现后打开 */
        DRV_IPC_TrigInt(DRV_IPC_MCU_CORE,IPC_MCU_INT_SRC_HIFI_PU);
        #endif

        /*使能IPC中断*/
        VOS_EnableInterrupt(DRV_IPC_INT_NO_HIFI);

        /* 标明已第一次进入IDLE任务 */
        UCOM_SET_FIRST_IDLE();

        /* 更新本次进入IDLE任务的时间及任务切换计数*/
        UCOM_SET_LAST_IDLE_TIME(uwTimeStamps);

        UCOM_SET_LAST_SW_CNT(ulVosCtxSw);

        /* 释放中断 */
        VOS_Splx(srSave);

        return;
    }

    /* 若上下文未切换，则标明系统一直处于IDLE当中，则进行低功耗动作 */
    if (ulVosCtxSw == UCOM_GET_LAST_SW_CNT())
    {
        /* 计算本次空闲时间 */
        uwIdleTime = UCOM_COMM_CycSub(uwTimeStamps,
                                      UCOM_GET_LAST_IDLE_TIME(),
                                      0xffffffff);

        /*若空闲时间大于阈值，则认为满足进入低功耗的条件*/
        if (uwIdleTime > UCOM_GET_WI_TIME_THD())
        {
            uwVoiceIsIdle = VOICE_ApiIsIdle();
            uwAudioIsIdle = VOS_OK;//AUDIO_COMM_IsIdle();

            /*Hifi上各业务模块进行投票，若均无业务则准备下电，否则进入WI低功耗模式*/
            if ((VOS_OK != uwVoiceIsIdle) || (VOS_OK != uwAudioIsIdle))
            {
                /* 更新本次进入IDLE任务的时间及任务切换计数 */
                UCOM_SET_LAST_IDLE_TIME(uwTimeStamps);

                UCOM_SET_LAST_SW_CNT(ulVosCtxSw);

                if(UCOM_GET_WATCHDOG_ENABLE())
                {
                    /*进WFI之前关闭看门狗*/
                    DRV_WATCHDOG_Stop();
                }

                OM_CPUVIEW_EnterArea((VOS_UCHAR)OM_CPUVIEW_AREA_UCOM_WFI);

                /*若当前HIFI还有业务运行，则进入Wait For Interrupt模式*/
                UCOM_SET_WFI(0);

                OM_CPUVIEW_ExitArea((VOS_UCHAR)OM_CPUVIEW_AREA_UCOM_WFI);

                /*出WFI启动看门狗*/
                if(UCOM_GET_WATCHDOG_ENABLE())
                {
                    DRV_WATCHDOG_Start(UCOM_GET_WD_TIMEOUT_LEN());
                }
            }
            /*若空闲时间大于阈值，则认为满足下电条件，否则继续轮空*/
            else if (uwIdleTime >= UCOM_GET_PD_TIME_THD())
            {
                /* 更新本次进入IDLE任务的时间及任务切换计数*/
                UCOM_SET_LAST_IDLE_TIME(uwTimeStamps);

                if(UCOM_GET_WATCHDOG_ENABLE())
                {
                    /* 停止Hifi的WatchDog */
                    DRV_WATCHDOG_Stop();
                }

                /* 停止Hifi的Socp通道 */
                DRV_SOCP_Stop();

                OM_CPUVIEW_EnterArea((VOS_UCHAR)OM_CPUVIEW_AREA_UCOM_PD);

                if(UCOM_GET_POWEROFF_ENABLE())
                {
                    /* 触发中断，通知MCU可以进行HIFI下电 */
                    DRV_IPC_TrigInt(DRV_IPC_MCU_CORE,IPC_MCU_INT_SRC_HIFI_PD);
                }

                /* 进入WFI 等待下电 */
                UCOM_SET_WFI(0);

                /* 重新初始化Hifi的socp通道 */
                DRV_SOCP_Init();

                OM_CPUVIEW_ExitArea((VOS_UCHAR)OM_CPUVIEW_AREA_UCOM_PD);

            }
            else
            {
                /* for pc lint */
            }
        }
    }
    else
    {
        /* 更新本次进入IDLE任务的时间及任务切换计数*/
        UCOM_SET_LAST_IDLE_TIME(uwTimeStamps);

        UCOM_SET_LAST_SW_CNT(ulVosCtxSw);
    }

    /* 释放中断 */
    VOS_Splx(srSave);

}
VOS_VOID UCOM_LOW_PowerInit(VOS_VOID)
{
    VOS_UINT32                              uwRet;
    CODEC_NV_PARA_HIFI_POWEROFF_CFG_STRU    stHifiPowerOffCfg;
    CODEC_NV_PARA_HIFI_WATCHDOG_CFG_STRU    stHifiWatchDogCfg;

    UCOM_MemSet(&stHifiPowerOffCfg, 0, sizeof(CODEC_NV_PARA_HIFI_POWEROFF_CFG_STRU));
    UCOM_MemSet(&stHifiWatchDogCfg, 0, sizeof(CODEC_NV_PARA_HIFI_WATCHDOG_CFG_STRU));

    /* 读取HIFI下电使能设置NV值 */
    uwRet = UCOM_NV_Read(en_NV_HifiPowerOffCfg, &stHifiPowerOffCfg, sizeof(CODEC_NV_PARA_HIFI_POWEROFF_CFG_STRU));

    /* 如果NV读取失败，获取默认值 */
    if( NV_OK != uwRet )
    {
        UCOM_SET_POWEROFF_ENABLE(VOS_TRUE);
        UCOM_SET_WI_TIME_THD(UCOM_HIFI_WI_TIME_THD);
        UCOM_SET_PD_TIME_THD(UCOM_HIFI_PD_TIME_THD);
    }
    else
    {
        /* 如果NV读取成功，更新全局变量 */
        UCOM_SET_POWEROFF_ENABLE(stHifiPowerOffCfg.uhwPowerOffEnable);
        UCOM_SET_WI_TIME_THD(stHifiPowerOffCfg.uwWITimeThd);
        UCOM_SET_PD_TIME_THD(stHifiPowerOffCfg.uwPDTimeThd);
    }

    /* 读取HIFI watchdog 设置NV值 */
    uwRet = UCOM_NV_Read(en_NV_HifiWatchDogCfg, &stHifiWatchDogCfg, sizeof(CODEC_NV_PARA_HIFI_WATCHDOG_CFG_STRU));
    /* 如果NV读取失败，获取默认值 */
    if( NV_OK != uwRet )
    {
        UCOM_SET_WATCHDOG_ENABLE(VOS_TRUE);
        UCOM_SET_WD_TIMEOUT_LEN(DRV_WATCHDOG_TIMEOUT_LEN);
    }
    else
    {
        /* 如果NV读取成功，更新全局变量 */
        UCOM_SET_WATCHDOG_ENABLE(stHifiWatchDogCfg.uhwWDEnable);
        UCOM_SET_WD_TIMEOUT_LEN(stHifiWatchDogCfg.uwWDTimeoutLen);
    }

    return;
}


VOS_VOID UCOM_LOW_PowerDRF(VOS_VOID)
{
    VOS_UINT32          uwBitEnable;

    /* 查询当前hifi状态，hifi处于语音通话状态时，直接返回  */
    if (VOS_ERR == VOICE_ApiIsIdle())
    {
        return;
    }

    OM_CPUVIEW_EnterArea((VOS_UCHAR)OM_CPUVIEW_AREA_UCOM_DRF);

    uwBitEnable = VOS_GetInterruptMask();

    /* 屏蔽所有中断 */
    VOS_SetInterruptMask(0);

    /* hifi处于音频播放状态时，向MCU发送IPC中断，要求DDR进入自刷新模式 */
    DRV_IPC_TrigInt(DRV_IPC_MCU_CORE,IPC_MCU_INT_SRC_HIFI_DDR_VOTE);

    /*进WFI之前关闭看门狗*/
    DRV_WATCHDOG_Stop();

    /* 控制hifi进入WFI状态，由于之前屏蔽了所有中断，即只有NMI中断可打断该状态 */
    UCOM_SET_WFI_NMI(0);

    /*出WFI启动看门狗*/
    DRV_WATCHDOG_Start(DRV_WATCHDOG_TIMEOUT_LEN);

    VOS_SetInterruptMask(uwBitEnable);

    OM_CPUVIEW_ExitArea((VOS_UCHAR)OM_CPUVIEW_AREA_UCOM_DRF);

    return;

}

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

