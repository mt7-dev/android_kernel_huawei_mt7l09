

#include "vos.h"
#include "hpacomm.h"
#include "sleepflow.h"
#include "hpaoperatertt.h"
#include "bbp_wcdma_on_interface.h"
#include "bbp_wcdma_interface.h"
#include "gbbp_interface.h"
#include "DrvInterface.h"
#include "apminterface.h"
#include "OmApi.h"
#include "OamSpecTaskDef.h"
#include "TtfOamInterface.h"
#include "NVIM_Interface.h"
#include "DrvNvInterface.h"
#include "omprivate.h"
#include "FileSysInterface.h"

#if (RAT_MODE != RAT_GU)
#include "product_config.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
#define    THIS_FILE_ID        PS_FILE_ID_SLEEP_FLOW_C

#if ( FEATURE_MULTI_MODEM == FEATURE_ON )

extern VOS_VOID I1_SLEEP_HookMsgProc(MsgBlock *pstMsg);
#endif

extern VOS_VOID OM_SLEEPInfoEvent(VOS_VOID *pData, VOS_UINT32 ulLength);
#if defined (CHIP_BB_6620CS)
extern VOS_INT32 BSP_ABB_RegGet( VOS_UINT32 regAddr, VOS_UINT32 * getData );
#endif

SLEEP_INFO_STATE_STRU*                  g_pstSleepInfoSlice;

VOS_VOID SHPA_ActivateHw(VOS_VOID)
{
    return;
}

VOS_VOID SHPA_DeactivateHw(VOS_VOID)
{
    return;
}

VOS_VOID SLEEP_EnterLowPower(VOS_VOID)
{
    return;
}

/*****************************************************************************
 函 数 名  : SLEEP_EnableDspPll
 功能描述  : 使能DSP PLL
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

*****************************************************************************/
VOS_VOID SLEEP_EnableDspPll(VOS_VOID)
{
    return;
}

/*****************************************************************************
 函 数 名  : SLEEP_ExitLowPower
 功能描述  : 退出搜网低功耗
 输入参数  : void
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :
*****************************************************************************/
VOS_VOID SLEEP_ExitLowPower(SLEEP_MODE_ENUM_UINT32 enMode)
{
    return;
}



VOS_BOOL SLEEP_IsSleep(VOS_VOID)
{
    return VOS_FALSE;
}

/*****************************************************************************
 函 数 名  : Sleep_ActiveDspPll
 功能描述  : 使能DSP PLL,供HPA调用
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

*****************************************************************************/
VOS_VOID Sleep_ActiveDspPll(VOS_VOID)
{
    return;
}

/*****************************************************************************
 函 数 名  : SLEEP_NotifySndMsg
 功能描述  : 通知SLEEP模块有消息发送到DSP
 输入参数  : pMsgBlock
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :
*****************************************************************************/
VOS_VOID SLEEP_NotifySndMsg(VOS_VOID)
{
    return;
}

/*****************************************************************************
 函 数 名  : GUSLEEP_MasterAwakeSlave
 功能描述  : 主模唤醒GU从模接口
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :
*****************************************************************************/
VOS_VOID GUSLEEP_MasterAwakeSlave(VOS_VOID)
{
    return;
}

/*****************************************************************************
 函 数 名  : SLEEP_32KIsr
 功能描述  : 处理 32K 时钟中断
 输入参数  : void
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :
*****************************************************************************/
VOS_VOID SLEEP_32KIsr(SLEEP_MODE_ENUM_UINT32 enMode)
{
    SLEEP_NOR_STATE_TRACK(enMode, SLEEP_MASTER_BBP_32K);

    SLEEP_IsrProc(BBP_SLEEP_32K_CLOCK_SWITCH_IND, enMode);
}

/*****************************************************************************
 函 数 名  : SLEEP_NormalClockIsr
 功能描述  : 处理 DRX唤醒后时钟切换到61.44M中断
 输入参数  : void
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :
*****************************************************************************/
VOS_VOID SLEEP_NormalClockIsr(SLEEP_MODE_ENUM_UINT32 enMode)
{
    SLEEP_NOR_STATE_TRACK(enMode, SLEEP_MASTER_BBP_NORMAL);

    SLEEP_IsrProc(BBP_SLEEP_NORMAL_CLOCK_SWITCH_IND, enMode);
}

/*****************************************************************************
 函 数 名  : SLEEP_AwakeIsr
 功能描述  : 处理唤醒中断
 输入参数  : void
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :
*****************************************************************************/
VOS_VOID SLEEP_AwakeIsr(SLEEP_MODE_ENUM_UINT32 enMode)
{
    SLEEP_NOR_STATE_TRACK(enMode, SLEEP_MASTER_BBP_WAKE);

    SLEEP_IsrProc(BBP_SLEEP_WAKE_IND, enMode);
}

/*****************************************************************************
  2 全局变量声明
*****************************************************************************/

/* 存放GU模当前系统状态 */
SLEEP_MODE_INFO_STRU                    g_astSleepModeInfo[SLEEP_GU_MODE_TYPE_BUTT];

/* PHY发来消息处理表 */
SLEEP_MSG_FUN_STRU g_astSleepPhyMsgFunTbl[] =
{
    {ID_UPHY_OAM_MASTER_SLEEP_IND,      SLEEP_MasterSleepMsgProc},
    {ID_UPHY_OAM_SLAVE_SLEEP_IND,       SLEEP_SlaveSleepMsgProc},
};

/* 中断消息处理表 */
SLEEP_MSG_FUN_STRU g_astSleepIsrMsgFunTbl[] =
{
    {BBP_SLEEP_32K_CLOCK_SWITCH_IND,    SLEEP_32KClkMsgProc},
    {BBP_SLEEP_WAKE_IND,                SLEEP_WakeMasterMsgProc},
    {BBP_SLEEP_NORMAL_CLOCK_SWITCH_IND, SLEEP_NormalClkMsgProc},
    {SLEEP_TL_WAKE_GU_IND,              SLEEP_TLWakeGUProc},
    {SLEEP_ACTIVATE_ERROR_ID,           SLEEP_ActivateHWMsgProc},
};

/* SLEEP流程记录 */
SLEEP_TRACK_RECORD_STRU                *g_pstSleepTrackRecord   = VOS_NULL_PTR;
SLEEP_NORMAL_STATE_RECORD_STRU         *g_pstSleepNormalState   = VOS_NULL_PTR;

HIFI_STATE_STRU                        *g_pstHifiStateSlice     = VOS_NULL_PTR;
HPA_LOAD_PHY_STATE_STRU                *g_pstLoadPhyState       = VOS_NULL_PTR;

SLEEP_DEVICE_STATE_STRU                *g_pstSleepDeviceState   = VOS_NULL_PTR;

/* 互斥信号量 */
VOS_UINT32                              g_ulSleepSem;

/* TCXO稳定信号量 */
VOS_UINT32                              g_ulSleepTcxoSem;

/* BBP寄存器备份恢复内容 */
VOS_UINT32                              g_ulSleepBBPPosValue    = 0;

VOS_UINT32                              g_ulSleepRusumeTime     = 0;

/* 器件恢复时间 */
WG_DRX_RESUME_TIME_STRU                 g_stSleepDrxResumeTime;

/* 通道参数 */
NV_GUMODE_CHAN_PARA_STRU                g_stSleepChanPara;

VOS_BOOL                                g_bDrxResetFlag = VOS_FALSE;

/* 记录SLEEP模块异常信息文件路径 */
VOS_CHAR                                g_acSleepLogDirPath[SLEEP_INFO_MAX_NAME_LEN];

VOS_CHAR                                g_acSleepLogFilePath[SLEEP_INFO_MAX_NAME_LEN];
#if (FEATURE_ON == FEATURE_MULTI_FS_PARTITION)
#define SLEEP_LOG_DIR                   "/data/modemlog/Log"
#define SLEEP_LOG_PATH                  "/data/modemlog/Log/Sleep_log"
#define SLEEP_UNITARY_LOG_DIR           "/modem_log/Log"
#define SLEEP_UNITARY_LOG_PATH          "/modem_log/Log/Sleep_log"
#else
#define SLEEP_LOG_DIR                   "/yaffs0/Log"
#define SLEEP_LOG_PATH                  "/yaffs0/Log/Sleep_log"
#define SLEEP_UNITARY_LOG_DIR           "/modem_log/Log"
#define SLEEP_UNITARY_LOG_PATH          "/modem_log/Log/Sleep_log"
#endif

/* 记录SLEEP模块异常信息文件序列值，默认0 */
VOS_UINT32                              g_ulSleepLogFileIndex = 0;

/*****************************************************************************
  3 函数申明
*****************************************************************************/


VOS_BOOL SLEEP_IsAwakeProc( SLEEP_MODE_ENUM_UINT32 enMode )
{
    if (SLEEP_TEMP_NORMAL_STATE == g_astSleepModeInfo[enMode].enSystemState)
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}


VOS_UINT32 SLEEP_AwakeInfoGet( SLEEP_MODE_ENUM_UINT32 enMode, VOS_UINT32 *pstAwakeHWInfo, VOS_UINT32 uLen )
{
    MODEM_ID_ENUM_UINT16                enModem;

#if defined (INSTANCE_1)
    enModem = MODEM_ID_1;
#else
    enModem = MODEM_ID_0;
#endif

    if ( (VOS_NULL_PTR == pstAwakeHWInfo)
        || (enMode > VOS_RATMODE_WCDMA)
        || (uLen < UPHY_OAM_SLEEP_INFO_NUM) )
    {
        return VOS_ERR;
    }

    VOS_MemSet(pstAwakeHWInfo, 0x0, sizeof(VOS_UINT32)*uLen);

    pstAwakeHWInfo[0]
        = 0x887755AA;
    pstAwakeHWInfo[1]
        = g_pstSleepNormalState->astRecordItem[enMode][SLEEP_MASTER_BBP_WAKE].ulSlice;
    pstAwakeHWInfo[2]
        = g_pstSleepNormalState->astRecordItem[enMode][SLEEP_MASTER_ACTIVATE_START].ulSlice;
    pstAwakeHWInfo[3]
        = g_pstSleepDeviceState->aulDeviceUpSlice[enModem][enMode][PWC_COMM_MODULE_ABB];
    pstAwakeHWInfo[4]
        = g_pstSleepDeviceState->aulDeviceUpSlice[enModem][enMode][PWC_COMM_MODULE_BBP_DRX];
    pstAwakeHWInfo[5]
        = g_pstSleepDeviceState->aulPLLUpSlice[enModem][enMode][PWC_COMM_MODULE_BBP_DRX];
    pstAwakeHWInfo[6]
        = g_pstSleepDeviceState->aulPLLUpSlice[enModem][enMode][PWC_COMM_MODULE_ABB];
    pstAwakeHWInfo[7]
        = g_pstSleepDeviceState->aulDeviceUpSlice[enModem][enMode][PWC_COMM_MODULE_RF];
    pstAwakeHWInfo[8]
        = g_pstSleepNormalState->astRecordItem[enMode][SLEEP_MASTER_ACTIVATE_END].ulSlice;
    pstAwakeHWInfo[9]
        = g_pstSleepNormalState->astRecordItem[enMode][SLEEP_MASTER_BBP_NORMAL].ulSlice;
    pstAwakeHWInfo[10]
        = g_pstSleepNormalState->astRecordItem[enMode][SLEEP_MASTER_WAKE_PHY_PRE].ulSlice;
    pstAwakeHWInfo[11]
        = g_pstSleepNormalState->astRecordItem[enMode][SLEEP_MASTER_WAKE_PHY].ulSlice;
    pstAwakeHWInfo[12]
        = 0xAA557788;

    if ( SLEEP_INFO_RESET_ENABLE == g_bDrxResetFlag )
    {
        VOS_ProtectionReboot(DRX_ACTIVATEHW_ERROR, THIS_FILE_ID, __LINE__, VOS_NULL_PTR, VOS_NULL);
    }
    else
    {
        SLEEP_IsrProc(SLEEP_ACTIVATE_ERROR_ID, enMode);
    }

    return VOS_OK;
}



VOS_VOID SLEEP_VoteLock(SLEEP_MODE_ENUM_UINT32 enMode)
{
#if defined (INSTANCE_1)
    if (VOS_RATMODE_GSM == enMode)
    {
        DRV_PWRCTRL_SLEEPVOTE_LOCK(PWRCTRL_SLEEP_PS_G1);
    }
#else
    if (VOS_RATMODE_GSM == enMode)
    {
        DRV_PWRCTRL_SLEEPVOTE_LOCK(PWRCTRL_SLEEP_PS_G0);
    }
    else
    {
        DRV_PWRCTRL_SLEEPVOTE_LOCK(PWRCTRL_SLEEP_PS_W0);
    }
#endif

    return;
}


VOS_VOID SLEEP_VoteUnlock(SLEEP_MODE_ENUM_UINT32 enMode)
{
#if defined (INSTANCE_1)
    if (VOS_RATMODE_GSM == enMode)
    {
        DRV_PWRCTRL_SLEEPVOTE_UNLOCK(PWRCTRL_SLEEP_PS_G1);
    }
#else
    if (VOS_RATMODE_GSM == enMode)
    {
        DRV_PWRCTRL_SLEEPVOTE_UNLOCK(PWRCTRL_SLEEP_PS_G0);
    }
    else
    {
        DRV_PWRCTRL_SLEEPVOTE_UNLOCK(PWRCTRL_SLEEP_PS_W0);
    }
#endif

    return;
}


VOS_VOID SLEEP_TimeCal( VOS_VOID )
{
    g_ulSleepRusumeTime = g_stSleepDrxResumeTime.ulAbbPwrRsmTime;

    /* 获取最大时间 */
    if (g_ulSleepRusumeTime < g_stSleepDrxResumeTime.ulBbpPllRsmTime)
    {
        g_ulSleepRusumeTime = g_stSleepDrxResumeTime.ulBbpPllRsmTime;
    }

    if (g_ulSleepRusumeTime < g_stSleepDrxResumeTime.ulRfPwrRsmTime)
    {
        g_ulSleepRusumeTime = g_stSleepDrxResumeTime.ulRfPwrRsmTime;
    }

    if (g_ulSleepRusumeTime < g_stSleepDrxResumeTime.ulPaPwrRsmTime)
    {
        g_ulSleepRusumeTime = g_stSleepDrxResumeTime.ulPaPwrRsmTime;
    }

    /* 换算成Slice单位 */
    g_ulSleepRusumeTime = g_ulSleepRusumeTime/32;

    return;
}


VOS_VOID SLEEP_TCXOIsrProc( VOS_VOID )
{
    VOS_SmV(g_ulSleepTcxoSem);
}


VOS_VOID SLEEP_TCXOEnable(SLEEP_MODE_ENUM_UINT32 enMode)
{
    VOS_UINT32                          ulTcxoResult;
    VOS_UINT32                          ulTcxoTime;
    VOS_UINT32                          ulTcxoSemTime;

    if ((SLEEP_GU_MODE_TYPE_BUTT <= enMode) || (SLEEP_CHANNEL_BOTH <= SLEEP_TCXO_CHAN(enMode)))
    {
        return;
    }

    DRV_TCXO_ENABLE((PWC_COMM_MODEM_E)SLEEP_TCXO_CHAN(enMode), (PWC_COMM_MODE_E)enMode);

    if (PWRCTRL_COMM_OFF == DRV_TCXO_GETSTATUS((PWC_COMM_MODEM_E)SLEEP_TCXO_CHAN(enMode)))
    {
        ulTcxoTime  = g_stSleepDrxResumeTime.ulTcxoRsmTime/1000;

        /* 启动TCXO稳定等待定时器 */
        DRV_TCXO_TIMER_START((PWC_COMM_MODEM_E)SLEEP_TCXO_CHAN(enMode), (PWC_COMM_MODE_E)enMode, (PWC_TCXO_FUNCPTR)SLEEP_TCXOIsrProc, VOS_NULL, ulTcxoTime);

        /* 等待TCXO稳定,Take信号量最小时间长度必须为1 tick */
        ulTcxoSemTime = (20 < ulTcxoTime) ?
            ulTcxoTime : 20;

        ulTcxoResult    = VOS_SmP(g_ulSleepTcxoSem, ulTcxoSemTime);

        if (VOS_OK != ulTcxoResult)
        {
            /* 异常处理流程 */
            g_pstSleepDeviceState->stDeviceTcxoError.ulCount++;
            g_pstSleepDeviceState->stDeviceTcxoError.ulSlice    = OM_GetSlice();
        }
    }

    DRV_TCXO_RFCLKENABLE((PWC_COMM_MODEM_E)SLEEP_TCXO_CHAN(enMode), (PWC_COMM_MODE_E)enMode);

    return;
}
VOS_VOID SLEEP_TCXODisable(SLEEP_MODE_ENUM_UINT32 enMode)
{
    if ((SLEEP_GU_MODE_TYPE_BUTT <= enMode) || (SLEEP_CHANNEL_BOTH <= SLEEP_TCXO_CHAN(enMode)))
    {
        return;
    }

    DRV_TCXO_RFCLKDISABLE((PWC_COMM_MODEM_E)SLEEP_TCXO_CHAN(enMode), (PWC_COMM_MODE_E)enMode);

    /* 关闭TCXO */
    DRV_TCXO_DISABLE((PWC_COMM_MODEM_E)SLEEP_TCXO_CHAN(enMode), (PWC_COMM_MODE_E)enMode);

    return;
}

/*lint -e662 -e661*/


VOS_VOID SLEEP_ABBPowerUp(SLEEP_MODE_ENUM_UINT32 enMode)
{
    if ((SLEEP_GU_MODE_TYPE_BUTT <= enMode) || (SLEEP_CHANNEL_BOTH < SLEEP_ABB_CHAN(enMode)))
    {
        return;
    }

    if (SLEEP_CHANNEL_BOTH == SLEEP_ABB_CHAN(enMode))
    {
        SLEEP_DEVICE_POWUP(SLEEP_CHANNEL_ZERO, enMode, PWC_COMM_MODULE_ABB);
        SLEEP_DEVICE_POWUP(SLEEP_CHANNEL_ONE,  enMode, PWC_COMM_MODULE_ABB);
    }
    else
    {
        SLEEP_DEVICE_POWUP(SLEEP_ABB_CHAN(enMode), enMode, PWC_COMM_MODULE_ABB);
    }

    return;
}


VOS_VOID SLEEP_ABBPowerDown(SLEEP_MODE_ENUM_UINT32 enMode)
{
    if ((SLEEP_GU_MODE_TYPE_BUTT <= enMode) || (SLEEP_CHANNEL_BOTH < SLEEP_ABB_CHAN(enMode)))
    {
        return;
    }

    if (SLEEP_CHANNEL_BOTH == SLEEP_ABB_CHAN(enMode))
    {
        SLEEP_DEVICE_POWDOWN(SLEEP_CHANNEL_ZERO, enMode, PWC_COMM_MODULE_ABB);
        SLEEP_DEVICE_POWDOWN(SLEEP_CHANNEL_ONE,  enMode, PWC_COMM_MODULE_ABB);
    }
    else
    {
        SLEEP_DEVICE_POWDOWN(SLEEP_ABB_CHAN(enMode), enMode, PWC_COMM_MODULE_ABB);
    }

    return;
}


VOS_VOID SLEEP_RFPowerUp(SLEEP_MODE_ENUM_UINT32 enMode)
{
    if ((SLEEP_GU_MODE_TYPE_BUTT <= enMode) || (SLEEP_CHANNEL_BOTH < SLEEP_RF_CHAN(enMode)))
    {
        return;
    }

    if (SLEEP_CHANNEL_BOTH == SLEEP_RF_CHAN(enMode))
    {
        SLEEP_DEVICE_POWUP(SLEEP_CHANNEL_ZERO, enMode, PWC_COMM_MODULE_RF);
        SLEEP_DEVICE_POWUP(SLEEP_CHANNEL_ONE,  enMode, PWC_COMM_MODULE_RF);
    }
    else
    {
        SLEEP_DEVICE_POWUP(SLEEP_RF_CHAN(enMode), enMode, PWC_COMM_MODULE_RF);
    }

    return;
}


VOS_VOID SLEEP_RFPowerDown(SLEEP_MODE_ENUM_UINT32 enMode)
{
    if ((SLEEP_GU_MODE_TYPE_BUTT <= enMode) || (SLEEP_CHANNEL_BOTH < SLEEP_RF_CHAN(enMode)))
    {
        return;
    }

    if (SLEEP_CHANNEL_BOTH == SLEEP_RF_CHAN(enMode))
    {
        SLEEP_DEVICE_POWDOWN(SLEEP_CHANNEL_ZERO, enMode, PWC_COMM_MODULE_RF);
        SLEEP_DEVICE_POWDOWN(SLEEP_CHANNEL_ONE,  enMode, PWC_COMM_MODULE_RF);
    }
    else
    {
        SLEEP_DEVICE_POWDOWN(SLEEP_RF_CHAN(enMode), enMode, PWC_COMM_MODULE_RF);
    }

    return;
}


VOS_VOID SLEEP_ABBPllEnable(SLEEP_MODE_ENUM_UINT32 enMode)
{
    if ((SLEEP_GU_MODE_TYPE_BUTT <= enMode) || (SLEEP_CHANNEL_BOTH < SLEEP_ABB_CHAN(enMode)))
    {
        return;
    }

    if (SLEEP_CHANNEL_BOTH == SLEEP_ABB_CHAN(enMode))
    {
        SLEEP_DEVICEPLL_POWUP((PWC_COMM_MODEM_E)SLEEP_CHANNEL_ZERO, enMode, PWC_COMM_MODULE_ABB);
        SLEEP_DEVICEPLL_POWUP((PWC_COMM_MODEM_E)SLEEP_CHANNEL_ONE, enMode, PWC_COMM_MODULE_ABB);
    }
    else
    {
        SLEEP_DEVICEPLL_POWUP((PWC_COMM_MODEM_E)SLEEP_ABB_CHAN(enMode), enMode, PWC_COMM_MODULE_ABB);
    }

    return;
}


VOS_VOID SLEEP_ABBPllDisable(SLEEP_MODE_ENUM_UINT32 enMode)
{
    if ((SLEEP_GU_MODE_TYPE_BUTT <= enMode) || (SLEEP_CHANNEL_BOTH < SLEEP_ABB_CHAN(enMode)))
    {
        return;
    }

    if (SLEEP_CHANNEL_BOTH == SLEEP_ABB_CHAN(enMode))
    {
        SLEEP_DEVICEPLL_POWDOWN((PWC_COMM_MODEM_E)SLEEP_CHANNEL_ZERO, enMode, PWC_COMM_MODULE_ABB);
        SLEEP_DEVICEPLL_POWDOWN((PWC_COMM_MODEM_E)SLEEP_CHANNEL_ONE, enMode, PWC_COMM_MODULE_ABB);
    }
    else
    {
        SLEEP_DEVICEPLL_POWDOWN((PWC_COMM_MODEM_E)SLEEP_ABB_CHAN(enMode), enMode, PWC_COMM_MODULE_ABB);
    }

    return;
}

/*lint +e662 +e661*/

VOS_VOID SLEEP_PowerUp(SLEEP_MODE_ENUM_UINT32 enMode)
{
    MODEM_ID_ENUM_UINT16                enModem;
    VOS_UINT32                          ulSlice;

#if defined (INSTANCE_1)
    enModem = MODEM_ID_1;
#else
    enModem = MODEM_ID_0;
#endif

    if (DRV_NOT_START_UMTS == DRV_BBP_GPIO_GET())
    {
        return;
    }

    /* 打开TXCO */
    SLEEP_TCXOEnable(enMode);

    /* 上电ABB */
    SLEEP_ABBPowerUp(enMode);

    /* 上电BBP */
    SLEEP_DEVICE_POWUP(enModem, enMode, PWC_COMM_MODULE_BBP_DRX);

    /* 打开BBP外设时钟 */
    DRV_PWRCTRL_BBPCLKENABLE((PWC_COMM_MODE_E)enMode, PWC_COMM_MODULE_BBP_DRX, (PWC_COMM_MODEM_E)enModem);

    /* 打开ABB PLL */
    SLEEP_ABBPllEnable(enMode);

    /* 打开BBP PLL */
    SLEEP_DEVICEPLL_POWUP((PWC_COMM_MODEM_E)enModem, (PWC_COMM_MODE_E)enMode, PWC_COMM_MODULE_BBP_DRX);

    /* 上电RF */
    SLEEP_RFPowerUp(enMode);

    ulSlice = OM_GetSlice();

    /* 换算成Slice等待器件稳定 */
    while((g_ulSleepRusumeTime) > (OM_GetSlice() - ulSlice))
    {
        ;
    }

    if (VOS_RATMODE_WCDMA == enMode)
    {
        HPA_Conncet3GISR();
    }
    else
    {
#if defined (INSTANCE_1)
        HPA_Conncet2GISR1();
#else
        HPA_Conncet2GISR0();
#endif
    }

    return;
}


VOS_UINT32 SLEEP_ReadBaseCntChip( VOS_VOID )
{
    VOS_UINT32                          ulBaseSlot;
    VOS_UINT32                          ulBaseChip;

    /* 读取BBP绝对时刻计数器，第一次读产生获取指令，第二次读为延时，第三次获取最终结果 */
    ulBaseSlot  = HPA_Read32Reg(WBBP_SYS_BASE_CNT_ADDR);
    ulBaseSlot  = HPA_Read32Reg(WBBP_SYS_BASE_CNT_ADDR);
    ulBaseSlot  = HPA_Read32Reg(WBBP_SYS_BASE_CNT_ADDR);

    /* 将绝对时刻换算成CHIP单位 */
    ulBaseChip  = (ulBaseSlot>>16)&0xFFF;
    ulBaseSlot  = (ulBaseSlot>>28)&0xF;
    ulBaseChip  += ulBaseSlot*2560;

    return ulBaseChip;
}


VOS_VOID SLEEP_ResumeReg( VOS_VOID )
{
#if (VOS_OS_VER != VOS_WIN32)
    VOS_UINT32                          ulNewChip;
    VOS_UINT32                          ulNowChip;
    VOS_UINT32                          ulNewPos;
    VOS_UINT32                          ulCfgSlot;
    VOS_UINT32                          ulCfgChip;
    VOS_UINT32                          ulCurrentChip = 0;
    VOS_UINT32                          ulSlice;

    SLEEP_NOR_STATE_TRACK(VOS_RATMODE_WCDMA, SLEEP_MASTER_BBP_RESUME_START);

    /*打开WBBP内部子模块的时钟和解复位*/
    HPA_Write32Reg(WBBP_SYS_RST_CON_W1_ADDR, 0xffff0000);

    /*将WBBP定时切换到CPICH态*/
    HPA_Write32RegMask(WBBP_TIMING_MODE_ADDR, 0x01);
    HPA_Write32RegMask(WBBP_TIMING_CFG_READY_ADDR, 0x01);

    /*需要在获取当前绝对时刻前配置最早径为0，如果此时离绝对帧头不远，
      便可以等待绝对帧头到来，以实现定时获取状态清零*/
    HPA_Write32Reg(WBBP_TRACE_CPI_FRM_POS_ADDR, 0);
    HPA_Write32Reg(WBBP_PATH_CFG_READY_ADDR, 1);

    ulNewChip   = SLEEP_ReadBaseCntChip();
    ulSlice     = OM_GetSlice();

    /*当前时刻快要到绝对帧头时，直接等待绝对帧头到来，以实现定时获取状态清零，
      同时可以避免计数器饱和翻转导致绝对时刻的先后判断困难*/
    if ( ulNewChip > SLEEP_WAKE_BASE_FRM_PROTECT_CHIP )
    {
        for (;;)
        {
            ulNowChip = SLEEP_ReadBaseCntChip();
            /*等待绝对计数器饱和翻转，即等待绝对帧头到来*/
            if ( ulNowChip < SLEEP_WAKE_BASE_FRM_PROTECT_CHIP )
            {
                break;
            }

            /* 超时保护，10ms后保护退出 */
            if ((OM_GetSlice() - ulSlice) > SLEEP_WAKE_BBP_POS_PROTECT_SLICE)
            {
                if (VOS_NULL_PTR != g_pstSleepDeviceState)
                {
                    g_pstSleepDeviceState->ulBBPResumeTimeout++;
                }

                break;
            }
        }
    }
    else
    {
        /*将当前绝对时刻加上150 CHIP作为临时最早径配置给定时模块，并等待150 CHIP，
          当最早径到来时，实现定时获取状态清零*/
        ulCurrentChip   = ulNewChip;
        ulNewChip   += 150;
        ulCfgSlot   = (ulNewChip/2560)%15;
        ulCfgChip   = (ulNewChip%2560);

        ulNewPos = (ulCfgSlot<<16) + (ulCfgChip<<4);

        HPA_Write32RegMask(WBBP_TRACE_CPI_FRM_POS_ADDR,ulNewPos);
        HPA_Write32RegMask(WBBP_PATH_CFG_READY_ADDR,0x1);

        for (;;)
        {
            ulNowChip = SLEEP_ReadBaseCntChip();
            /*等待当前时刻大于临时最早径，同时判断是否有反转场景*/
            if ( (ulNewChip < ulNowChip) || (ulNowChip < ulCurrentChip) )
            {
                break;
            }

            /* 超时保护，10ms后保护退出 */
            if ((OM_GetSlice() - ulSlice) > SLEEP_WAKE_BBP_POS_PROTECT_SLICE)
            {
                if (VOS_NULL_PTR != g_pstSleepDeviceState)
                {
                    g_pstSleepDeviceState->ulBBPResumeTimeout++;
                }

                break;
            }
        }
    }

    /*恢复定时模块的服务小区CPICH最早径信息*/
    HPA_Write32Reg(WBBP_TRACE_CPI_FRM_POS_ADDR, g_ulSleepBBPPosValue);
    HPA_Write32Reg(WBBP_PATH_CFG_READY_ADDR, 1);

    SLEEP_NOR_STATE_TRACK(VOS_RATMODE_WCDMA, SLEEP_MASTER_BBP_RESUME_END);
#endif
}

/*****************************************************************************
 函 数 名  : SLEEP_ForceWakeProtected
 功能描述  : 判断正常唤醒最近是否到来
 输入参数  : void
 输出参数  : 无
 返 回 值  : VOS_OK  - 不会到来，可以正常唤醒BPP
             VOS_ERR - 马上来，不用唤醒BPP
 调用函数  :
 被调函数  :
*****************************************************************************/
VOS_UINT32 SLEEP_ForceWakeProtected(VOS_VOID)
{
    VOS_UINT32  ulWakeSfn;
    VOS_UINT32  ulWakeSlot;
    VOS_UINT32  ulSysSfn;
    VOS_UINT32  ulSysSlot;

    /*当前时刻帧号[11:0]，偏移时隙[31:28]*/
    ulSysSfn  = HPA_Read32Reg(WBBP_SYS_CNT_SLEEP_1CARD_ADDR);
    ulSysSlot  = ulSysSfn>>28;
    ulSysSfn  &= 0xfff;
    ulSysSlot  = (ulSysSfn*SLEEP_SLOT_LEN) + ulSysSlot;

    /*正常唤醒时刻帧号[11:0]*/
    ulWakeSfn = HPA_Read32Reg(WBBP_CK_WAKE_SFN_1CARD_ADDR)&0xfff;
    /*正常唤醒时刻的偏移时隙[19:16]*/
    ulWakeSlot = HPA_Read32Reg(WBBP_CK_WAKE_CNT_1CARD_ADDR);
    ulWakeSlot = (ulWakeSlot>>16)&0xf;
    /*防止在执行该函数期间，正常中断已经来了，故将正常唤醒时刻延后2个时隙*/
    ulWakeSlot = ((ulWakeSfn*SLEEP_SLOT_LEN) + ulWakeSlot) + SLEEP_WAKE_DELAY;

    /*考虑ulWakeCnt反转的情况*/
    if (ulWakeSlot < ulSysSlot)
    {
        ulWakeSlot += SLEEP_SFN_CYCLE_LEN;
    }

    /*判断在4个时隙内是否有正常WAKE中断上报*/
    if ((ulSysSlot + (SLEEP_WAKE_DELAY + SLEEP_WAKE_INTERVAL))
        < ulWakeSlot)
    {
        return VOS_OK;
    }

    return VOS_ERR;
}

VOS_VOID SLEEP_DeactivateHw(SLEEP_MODE_ENUM_UINT32 enMode)
{
    MODEM_ID_ENUM_UINT16                enModem;

#if defined (INSTANCE_1)
    enModem = MODEM_ID_1;
#else
    enModem = MODEM_ID_0;
#endif

    if (DRV_NOT_START_UMTS == DRV_BBP_GPIO_GET())
    {
        return;
    }

    if (VOS_RATMODE_WCDMA == enMode)
    {
        HPA_3GIsrDisable(enModem);
    }
    else
    {
        HPA_2GIsrDisable(enModem);
    }

    /* BBP寄存器备份 */
    if (VOS_RATMODE_WCDMA == enMode)
    {
        g_ulSleepBBPPosValue  = HPA_Read32Reg(WBBP_TRACE_CPI_FRM_POS_ADDR);
    }

    /* 下电RF */
    SLEEP_RFPowerDown(enMode);

    /* 关闭ABB PLL */
    SLEEP_ABBPllDisable(enMode);

    /* 关闭BBP PLL */
    SLEEP_DEVICEPLL_POWDOWN((PWC_COMM_MODEM_E)enModem, (PWC_COMM_MODE_E)enMode, PWC_COMM_MODULE_BBP_DRX);

    /* 关闭BBP外设时钟 */
    DRV_PWRCTRL_BBPCLKDISABLE((PWC_COMM_MODE_E)enMode, PWC_COMM_MODULE_BBP_DRX, (PWC_COMM_MODEM_E)enModem);

    /* 下电BBP */
    SLEEP_DEVICE_POWDOWN(enModem, enMode, PWC_COMM_MODULE_BBP_DRX);

    /* 下电ABB */
    SLEEP_ABBPowerDown(enMode);

    /* 关闭TCXO */
    SLEEP_TCXODisable(enMode);

    return;
}


VOS_VOID SLEEP_ActivateHw(SLEEP_MODE_ENUM_UINT32 enMode)
{
    MODEM_ID_ENUM_UINT16                enModem;
    VOS_UINT32                          ulSlice;

#if defined (INSTANCE_1)
    enModem = MODEM_ID_1;
#else
    enModem = MODEM_ID_0;
#endif

    if (DRV_NOT_START_UMTS == DRV_BBP_GPIO_GET())
    {
        return;
    }

    SLEEP_NOR_STATE_TRACK(enMode, SLEEP_MASTER_ACTIVATE_START);

    /* 打开TCXO */
    SLEEP_TCXOEnable(enMode);

    /* 上电ABB */
    SLEEP_ABBPowerUp(enMode);

    /* 上电BBP */
    SLEEP_DEVICE_POWUP(enModem, enMode, PWC_COMM_MODULE_BBP_DRX);

    /* 打开BBP外设时钟 */
    DRV_PWRCTRL_BBPCLKENABLE((PWC_COMM_MODE_E)enMode, PWC_COMM_MODULE_BBP_DRX, (PWC_COMM_MODEM_E)enModem);

    /* 打开BBP PLL */
    SLEEP_DEVICEPLL_POWUP((PWC_COMM_MODEM_E)enModem, (PWC_COMM_MODE_E)enMode, PWC_COMM_MODULE_BBP_DRX);

    /* 打开ABB PLL */
    SLEEP_ABBPllEnable(enMode);

    /* 上电RF */
    SLEEP_RFPowerUp(enMode);

#if defined (CHIP_BB_6620CS)
    if ( VOS_NULL_PTR != g_pstSleepDeviceState )
    {
        g_pstSleepDeviceState->aulABBState[2]   = HPA_Read32Reg(0xF7410874);
        g_pstSleepDeviceState->aulABBState[3]   = HPA_Read32Reg(0xF741085C);
        g_pstSleepDeviceState->aulABBState[4]   = HPA_Read32Reg(0xF7410864);
        g_pstSleepDeviceState->aulABBState[5]   = HPA_Read32Reg(0xF741086C);
        g_pstSleepDeviceState->aulABBState[6]   = HPA_Read32Reg(0xF7410650);
        g_pstSleepDeviceState->aulABBState[7]   = HPA_Read32Reg(0xF74106F8);
        g_pstSleepDeviceState->aulABBState[8]   = HPA_Read32Reg(0xF711A6A8);
        g_pstSleepDeviceState->aulABBState[9]   = HPA_Read32Reg(0xF711A72C);
        g_pstSleepDeviceState->aulABBState[10]  = HPA_Read32Reg(0xF711A73C);
        BSP_ABB_RegGet(0xd0, &g_pstSleepDeviceState->aulABBState[11]);
        BSP_ABB_RegGet(0xd1, &g_pstSleepDeviceState->aulABBState[12]);
        BSP_ABB_RegGet(0xdb, &g_pstSleepDeviceState->aulABBState[13]);
        BSP_ABB_RegGet(0xe1, &g_pstSleepDeviceState->aulABBState[14]);
        BSP_ABB_RegGet(0xe2, &g_pstSleepDeviceState->aulABBState[15]);
        BSP_ABB_RegGet(0xe3, &g_pstSleepDeviceState->aulABBState[16]);
        BSP_ABB_RegGet(0xe6, &g_pstSleepDeviceState->aulABBState[17]);
    }
#endif

    ulSlice = OM_GetSlice();

    /* 换算成Slice等待器件稳定 */
    while((g_ulSleepRusumeTime) > (OM_GetSlice() - ulSlice))
    {
        ;
    }

#if defined (CHIP_BB_6620CS)
    if ( VOS_NULL_PTR != g_pstSleepDeviceState )
    {
        g_pstSleepDeviceState->aulABBState[18]  = HPA_Read32Reg(0xF7410874);
        g_pstSleepDeviceState->aulABBState[19]  = HPA_Read32Reg(0xF741085C);
        g_pstSleepDeviceState->aulABBState[20]  = HPA_Read32Reg(0xF7410864);
        g_pstSleepDeviceState->aulABBState[21]  = HPA_Read32Reg(0xF741086C);
        g_pstSleepDeviceState->aulABBState[22]  = HPA_Read32Reg(0xF7410650);
        g_pstSleepDeviceState->aulABBState[23]  = HPA_Read32Reg(0xF74106F8);
        g_pstSleepDeviceState->aulABBState[24]  = HPA_Read32Reg(0xF711A6A8);
        g_pstSleepDeviceState->aulABBState[25]  = HPA_Read32Reg(0xF711A72C);
        g_pstSleepDeviceState->aulABBState[26]  = HPA_Read32Reg(0xF711A73C);
        BSP_ABB_RegGet(0xd0, &g_pstSleepDeviceState->aulABBState[27]);
        BSP_ABB_RegGet(0xd1, &g_pstSleepDeviceState->aulABBState[28]);
        BSP_ABB_RegGet(0xdb, &g_pstSleepDeviceState->aulABBState[29]);
        BSP_ABB_RegGet(0xe1, &g_pstSleepDeviceState->aulABBState[30]);
        BSP_ABB_RegGet(0xe2, &g_pstSleepDeviceState->aulABBState[31]);
        BSP_ABB_RegGet(0xe3, &g_pstSleepDeviceState->aulABBState[32]);
        BSP_ABB_RegGet(0xe6, &g_pstSleepDeviceState->aulABBState[33]);
    }
#endif

    /* BBP寄存器恢复 */
    if (VOS_RATMODE_WCDMA == enMode)
    {
        SLEEP_ResumeReg();
    }

    if (VOS_RATMODE_WCDMA == enMode)
    {
        if (BBP_ERROR_VALUE == HPA_Read32Reg(WBBP_ARM_INT01_MASK_ADDR))
        {
            if (VOS_NULL_PTR != g_pstSleepDeviceState)
            {
                g_pstSleepDeviceState->astDevicePowerError[enMode].ulCount++;
                g_pstSleepDeviceState->astDevicePowerError[enMode].ulSlice  = OM_GetSlice();
            }
        }

        HPA_3GIsrEnable(enModem);
    }
    else
    {
        if (BBP_ERROR_VALUE == HPA_Read32Reg(g_aulGBBPRegAddr[INT_MASK][enModem]))
        {
            ulSlice = OM_GetSlice();

            if (VOS_NULL_PTR != g_pstSleepDeviceState)
            {
                g_pstSleepDeviceState->aulABBState[0]       = ulSlice;
            }

            while (SLEEP_BBP_ERROR_TIMEOUT > (OM_GetSlice() - ulSlice))
            {
                if (BBP_ERROR_VALUE != HPA_Read32Reg(g_aulGBBPRegAddr[INT_MASK][enModem]))
                {
                    break;
                }
            }

            if (VOS_NULL_PTR != g_pstSleepDeviceState)
            {
                g_pstSleepDeviceState->aulABBState[1]   = OM_GetSlice();

                g_pstSleepDeviceState->astDevicePowerError[enMode].ulCount++;
                g_pstSleepDeviceState->astDevicePowerError[enMode].ulSlice  = OM_GetSlice();

                DRV_SYSTEM_ERROR(DRX_REPORT_BBP_READ_ERROR, (VOS_INT32)g_pstSleepDeviceState->aulABBState[0], (VOS_INT32)g_pstSleepDeviceState->aulABBState[1], VOS_NULL_PTR, VOS_NULL);
             }
        }

        HPA_2GIsrEnable(enModem);
    }

    SLEEP_NOR_STATE_TRACK(enMode, SLEEP_MASTER_ACTIVATE_END);

    return;
}

#if (RAT_MODE != RAT_GU)
VOS_VOID SLEEP_InfoInd( SLEEP_MODE_ENUM_UINT32 enMode )
{
    if (VOS_NULL_PTR == g_pstSleepInfoSlice)
    {
        return;
    }

    g_pstSleepInfoSlice->ulAwakeIsrSlice     = g_pstSleepNormalState->astRecordItem[enMode][SLEEP_MASTER_BBP_WAKE].ulSlice;
    g_pstSleepInfoSlice->ulAwakeStartSlice   = g_pstSleepNormalState->astRecordItem[enMode][SLEEP_MASTER_ACTIVATE_START].ulSlice;
    g_pstSleepInfoSlice->ulAwakeEndSlice     = g_pstSleepNormalState->astRecordItem[enMode][SLEEP_MASTER_ACTIVATE_END].ulSlice;
    g_pstSleepInfoSlice->ulSwitchIsrSlice    = g_pstSleepNormalState->astRecordItem[enMode][SLEEP_MASTER_BBP_NORMAL].ulSlice;
    g_pstSleepInfoSlice->ulSwitchTaskSlice   = g_pstSleepNormalState->astRecordItem[enMode][SLEEP_MASTER_WAKE_PHY].ulSlice;

    if (g_pstSleepInfoSlice->ulAwakeEndSlice >= g_pstSleepInfoSlice->ulSwitchIsrSlice)
    {
        if ((g_pstSleepInfoSlice->ulAwakeEndSlice-g_pstSleepInfoSlice->ulSwitchIsrSlice) >= SLEEP_RECORD_4MS)
        {
            /* 时钟切换中断早于上电任务完成4ms以上场景 */
            g_pstSleepInfoSlice->ulSwitchDelayType5Count++;
        }
        else if ((g_pstSleepInfoSlice->ulAwakeEndSlice-g_pstSleepInfoSlice->ulSwitchIsrSlice) >= SLEEP_RECORD_3MS)
        {
            /* 时钟切换中断早于上电任务完成3至4ms场景 */
            g_pstSleepInfoSlice->ulSwitchDelayType4Count++;
        }
        else if ((g_pstSleepInfoSlice->ulAwakeEndSlice-g_pstSleepInfoSlice->ulSwitchIsrSlice) >= SLEEP_RECORD_2MS)
        {
            /* 时钟切换中断早于上电任务完成2至3ms场景 */
            g_pstSleepInfoSlice->ulSwitchDelayType3Count++;
        }
        else if ((g_pstSleepInfoSlice->ulAwakeEndSlice-g_pstSleepInfoSlice->ulSwitchIsrSlice) >= SLEEP_RECORD_1MS)
        {
            /* 时钟切换中断早于上电任务完成1至2ms场景 */
            g_pstSleepInfoSlice->ulSwitchDelayType2Count++;
        }
        else
        {
            /* 时钟切换中断早于上电任务完成1ms以下 */
            g_pstSleepInfoSlice->ulSwitchDelayType1Count++;
        }

        g_pstSleepInfoSlice->ulWarnEventFlag    = VOS_TRUE;
        g_pstSleepInfoSlice->ulWarnEventCount   = SLEEP_RECORD_MAX_EVENT_COUNT;
    }

    /* 如果统计发现有异常唤醒，则进行可维可测信息上报，同时清空标志位 */
    if (VOS_TRUE == g_pstSleepInfoSlice->ulWarnEventFlag)
    {
        g_pstSleepInfoSlice->ulWarnEventCount--;

        if (0 == g_pstSleepInfoSlice->ulWarnEventCount)
        {
            g_pstSleepInfoSlice->ulWarnEventFlag    = VOS_FALSE;
        }

        OM_SLEEPInfoEvent(g_pstSleepInfoSlice, sizeof(SLEEP_INFO_STATE_STRU));
    }

    return;
}
#endif


VOS_UINT32 SLEEP_WakePhy(VOS_UINT32 ulMsgId, SLEEP_MODE_ENUM_UINT32 enMode, UPHY_OAM_WAKE_TYPE_ENUM_UINT32 enWakeType)
{
    UPHY_OAM_WAKE_CMD_STRU               *pstPhyMsg;

    /* 申请消息内存 */
    pstPhyMsg = (UPHY_OAM_WAKE_CMD_STRU*)VOS_AllocMsg(WUEPS_PID_SLEEP,
                    sizeof(UPHY_OAM_WAKE_CMD_STRU) - VOS_MSG_HEAD_LENGTH);

    /* 申请失败 */
    if (VOS_NULL_PTR == pstPhyMsg)
    {
        return VOS_ERR;
    }

    pstPhyMsg->ulReceiverPid = DSP_PID_DRX;

    pstPhyMsg->enMsgId       = (VOS_UINT16)ulMsgId;
#if defined (INSTANCE_1)
    pstPhyMsg->enModem       = MODEM_ID_1;
#else
    pstPhyMsg->enModem       = MODEM_ID_0;
#endif
    pstPhyMsg->enRatMode     = enMode;
    pstPhyMsg->enWakeType    = enWakeType;

    /* 发送消息 */
    if (VOS_OK != VOS_SendMsg(WUEPS_PID_SLEEP, pstPhyMsg))
    {
        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_VOID SLEEP_IsrProc(VOS_UINT32 ulMsgId, SLEEP_MODE_ENUM_UINT32 enMode)
{
    VOS_INT                             lLockLevel;
    SLEEP_ISR_MSG_STRU                 *pstIsrMsg;

    lLockLevel = VOS_SplIMP();

    /* 禁止深睡投票 */
    SLEEP_VoteLock(enMode);

    /* 中断序列记录 */
    SLEEP_ISR_RECORD(ulMsgId, enMode);

    /* 分配消息内存 */
    pstIsrMsg = (SLEEP_ISR_MSG_STRU*)VOS_AllocMsg(WUEPS_PID_SLEEP,
                    sizeof(SLEEP_ISR_MSG_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstIsrMsg)
    {
        VOS_Splx(lLockLevel);

        return;
    }

    pstIsrMsg->ulReceiverPid = WUEPS_PID_SLEEP;
    pstIsrMsg->usMsgId       = (VOS_UINT16)ulMsgId;
    pstIsrMsg->enRatMode     = enMode;
    pstIsrMsg->ulSlice       = OM_GetSlice();

    if (VOS_OK != VOS_SendMsg(WUEPS_PID_SLEEP, pstIsrMsg))
    {
        /*; */
    }

    VOS_Splx(lLockLevel);

    return;
}


VOS_VOID SLEEP_32KClkMsgProc(MsgBlock *pstMsg)
{
    UPHY_OAM_SLEEP_IND_STRU             *pstPhyReq;
    SLEEP_MODE_ENUM_UINT32              enMode;
    VOS_UINT32                          ulQueueSize = 0;

    pstPhyReq = (UPHY_OAM_SLEEP_IND_STRU*)pstMsg;

    enMode    = pstPhyReq->enRatMode;

    /* 参数检测 */
    if (SLEEP_GU_MODE_TYPE_BUTT <= enMode)
    {
        /* 异常打印和记录 */
        PS_LOG1(WUEPS_PID_SLEEP, 0, PS_PRINT_ERROR, "SLEEP_32KClkMsgProc: Mode para is wrong.", (VOS_INT32)enMode);
        return;
    }

    (VOS_VOID)VOS_SmP(g_ulSleepSem, 0);

    /* 假如当前系统为NORMAL态则切换为主模睡眠暂态 */
    if (SLEEP_NORMAL_STATE == g_astSleepModeInfo[enMode].enSystemState)
    {
        g_astSleepModeInfo[enMode].enSystemState = SLEEP_MASTER_TEMP_LP_STATE;

        VOS_SmV(g_ulSleepSem);

        return;
    }

    /* 假如当前系统为主模睡眠暂态则切换为主模睡眠态 */
    if (SLEEP_MASTER_TEMP_LP_STATE == g_astSleepModeInfo[enMode].enSystemState)
    {
        /* 下电RTT子系统 */
        SLEEP_DeactivateHw(enMode);

        SLEEP_NOR_STATE_TRACK(enMode, SLEEP_MASTER_LP);

        g_astSleepModeInfo[enMode].enSystemState = SLEEP_MASTER_LP_STATE;

#ifdef FEATURE_DSP2ARM
        /* 获取GUPHY消息队列中消息个数 */
        if (VOS_RATMODE_WCDMA == enMode)
        {
            ulQueueSize = VOS_GetQueueSizeFromFid(DSP_FID_WPHY);
        }
        else
        {
            ulQueueSize = VOS_GetQueueSizeFromFid(DSP_FID_GPHY);
        }
#endif
        /* 如果消息队列中有消息，则进入强制唤醒流程 */
        if (0 < ulQueueSize)
        {
            SLEEP_MasterForceWake(enMode);
        }
        else
        {
            /* 允许深睡投票 */
            SLEEP_VoteUnlock(enMode);
        }

        VOS_SmV(g_ulSleepSem);

        return;
    }

    VOS_SmV(g_ulSleepSem);

    return;
}


VOS_VOID SLEEP_MasterSleepMsgProc(MsgBlock *pstMsg)
{
    UPHY_OAM_SLEEP_IND_STRU                 *pstPhyReq;

    pstPhyReq = (UPHY_OAM_SLEEP_IND_STRU*)pstMsg;

    if (VOS_RATMODE_WCDMA == pstPhyReq->enRatMode)
    {
        /* 睡眠前完成SFN、CFN的更新 */
        HPA_ReadCfnSfn();

        /* 睡眠前通知层2判断邮箱数据是否搬完 */
        WTTF_MAC_R99MailBox_ReadForIpcHalt();
    }

    SLEEP_NOR_STATE_TRACK(pstPhyReq->enRatMode, SLEEP_MASTER_PHY_HALT);

    /* 与32K中断相同的处理流程 */
    SLEEP_32KClkMsgProc(pstMsg);

    return;
}
VOS_VOID SLEEP_WakeMasterMsgProc(MsgBlock *pstMsg)
{
    SLEEP_ISR_MSG_STRU                 *pstIsrReq;
    SLEEP_MODE_ENUM_UINT32              enMode;

    pstIsrReq = (SLEEP_ISR_MSG_STRU*)pstMsg;

    enMode    = pstIsrReq->enRatMode;

    /* 参数检测 */
    if (SLEEP_GU_MODE_TYPE_BUTT <= enMode)
    {
        /* 异常打印和记录 */
        PS_LOG1(WUEPS_PID_SLEEP, 0, PS_PRINT_ERROR, "SLEEP_WakeMasterMsgProc: Mode para is wrong.", (VOS_INT32)enMode);
        return;
    }

    (VOS_VOID)VOS_SmP(g_ulSleepSem, 0);

    /* 只有系统在主模低功耗状态才能进入到NORMAL暂态 */
    if (SLEEP_MASTER_LP_STATE == g_astSleepModeInfo[enMode].enSystemState)
    {
        /* 上电RTT子系统 */
        SLEEP_ActivateHw(enMode);

        g_astSleepModeInfo[enMode].enWakeType    = UPHY_OAM_NORMAL_WAKE;
        g_astSleepModeInfo[enMode].enSystemState = SLEEP_TEMP_NORMAL_STATE;
    }

    VOS_SmV(g_ulSleepSem);

    return;
}


VOS_VOID SLEEP_NormalClkMsgProc(MsgBlock *pstMsg)
{
    SLEEP_ISR_MSG_STRU                 *pstIsrReq;
    SLEEP_MODE_ENUM_UINT32              enMode;

    pstIsrReq = (SLEEP_ISR_MSG_STRU*)pstMsg;

    enMode    = pstIsrReq->enRatMode;

    /* 参数检测 */
    if (SLEEP_GU_MODE_TYPE_BUTT <= enMode)
    {
        /* 异常打印和记录 */
        PS_LOG1(WUEPS_PID_SLEEP, 0, PS_PRINT_ERROR, "SLEEP_NormalClkMsgProc: Mode para is wrong.", (VOS_INT32)enMode);
        return;
    }

    SLEEP_NOR_STATE_TRACK(enMode, SLEEP_MASTER_WAKE_PHY_PRE);

    (VOS_VOID)VOS_SmP(g_ulSleepSem, 0);

    /* 记录当前系统状态和Slice值 */

    ;

    /* 只有系统在NORMAL暂态才能进入到NORMAL态 */
    if (SLEEP_TEMP_NORMAL_STATE == g_astSleepModeInfo[enMode].enSystemState)
    {
        /* 发送消息给PHY_IDLE唤醒PHY */
        SLEEP_WakePhy(ID_OAM_UPHY_MASTER_WAKE_CMD, enMode, g_astSleepModeInfo[enMode].enWakeType);

        SLEEP_NOR_STATE_TRACK(enMode, SLEEP_MASTER_WAKE_PHY);

        SLEEP_MSG_RECORD(ID_OAM_UPHY_MASTER_WAKE_CMD, DSP_PID_DRX, g_astSleepModeInfo[enMode].enWakeType);

        g_astSleepModeInfo[enMode].enSystemState = SLEEP_NORMAL_STATE;

#if (RAT_MODE != RAT_GU)
        SLEEP_InfoInd(enMode);
#endif
    }

    VOS_SmV(g_ulSleepSem);

    return;
}
VOS_VOID SLEEP_SlaveSleepMsgProc(MsgBlock *pstMsg)
{
    UPHY_OAM_SLEEP_IND_STRU            *pstPhyReq;
    SLEEP_MODE_ENUM_UINT32              enMode;
    VOS_INT32                           lMeansFlag;
    VOS_UINT32                          ulQueueSize;

    pstPhyReq = (UPHY_OAM_SLEEP_IND_STRU*)pstMsg;

    enMode    = pstPhyReq->enRatMode;

    /* 参数检测 */
    if (SLEEP_GU_MODE_TYPE_BUTT <= enMode)
    {
        /* 异常打印和记录 */
        PS_LOG1(WUEPS_PID_SLEEP, 0, PS_PRINT_ERROR, "SLEEP_SlaveSleepMsgProc: Mode para is wrong.", (VOS_INT32)enMode);
        return;
    }

    (VOS_VOID)VOS_SmP(g_ulSleepSem, 0);

    SLEEP_NOR_STATE_TRACK(enMode, SLEEP_SLAVE_PHY_HALT);

    /* 只有系统在NORMAL态才能进入到从模低功耗 */
    if (SLEEP_NORMAL_STATE == g_astSleepModeInfo[enMode].enSystemState)
    {
        /* 下电RTT */
        SLEEP_DeactivateHw(enMode);

        SLEEP_NOR_STATE_TRACK(enMode, SLEEP_SLAVE_LP);

        g_astSleepModeInfo[enMode].enSystemState = SLEEP_SLAVE_LP_STATE;

#ifdef FEATURE_DSP2ARM
        /* 获取GUPHY消息队列中消息个数 */
        if (VOS_RATMODE_WCDMA == enMode)
        {
            ulQueueSize = VOS_GetQueueSizeFromFid(DSP_FID_WPHY);
        }
        else
        {
            ulQueueSize = VOS_GetQueueSizeFromFid(DSP_FID_GPHY);
        }
#endif

        /* 获取主模测量是否启动，由于底软代码未提供暂未实现 */
        lMeansFlag = DRV_PWC_GET_MEANSFLAG(PWC_COMM_MODE_WCDMA);

        if ((0 < ulQueueSize) || (VOS_TRUE == lMeansFlag))
        {
            SLEEP_SlaveForceWake(enMode);
        }
        else
        {
            /* 允许深睡投票 */
            SLEEP_VoteUnlock(enMode);
        }
    }

    VOS_SmV(g_ulSleepSem);

    return;
}
VOS_VOID SLEEP_TLWakeGUProc(MsgBlock *pstMsg)
{
    SLEEP_ISR_MSG_STRU                 *pstIsrReq;
    SLEEP_MODE_ENUM_UINT32              enMode;

    pstIsrReq = (SLEEP_ISR_MSG_STRU*)pstMsg;

    enMode    = pstIsrReq->enRatMode;

    (VOS_VOID)VOS_SmP(g_ulSleepSem, 0);

    SLEEP_SlaveForceWake(enMode);

    VOS_SmV(g_ulSleepSem);

    return;
}


VOS_UINT32 SLEEP_InfoFileName( VOS_CHAR * cFileName )
{
    FILE                                *fp;
    VOS_CHAR                            acIndex[3];

    if ( VOS_NULL_PTR == cFileName )
    {
        return VOS_ERR;
    }

    /* 读取最后保存文件序列值 */
    fp = DRV_FILE_OPEN((VOS_CHAR*)g_acSleepLogFilePath, "rb+");

    /* 如果没有读取到文件序列值文件，代表首次保存，文件序列值使用默认值0开始 */
    if (VOS_NULL_PTR == fp)
    {
        g_ulSleepLogFileIndex = 0;

        if (0 != DRV_FILE_MKDIR((VOS_CHAR*)g_acSleepLogDirPath))
        {
            /* 文件夹创建失败或者文件夹已经存在 */
        }

        /* 将当前文件序列值记录至Sleep_log文件中 */
        fp = DRV_FILE_OPEN((VOS_CHAR*)g_acSleepLogFilePath, "wb+");

        if(VOS_NULL_PTR == fp)
        {
            return VOS_ERR;
        }

        DRV_FILE_WRITE(&g_ulSleepLogFileIndex, sizeof(VOS_UINT32), 1, fp);

        DRV_FILE_CLOSE(fp);
    }
    else
    {
        DRV_FILE_READ(&g_ulSleepLogFileIndex, sizeof(VOS_UINT32), 1, fp);

        DRV_FILE_CLOSE(fp);
    }

    VOS_MemSet(cFileName, 0, SLEEP_INFO_MAX_NAME_LEN);

    VOS_StrNCpy(cFileName, g_acSleepLogFilePath, VOS_StrLen(g_acSleepLogFilePath));

    VOS_MemSet(acIndex, 0, sizeof(VOS_CHAR)*3);

    VOS_sprintf(acIndex, ".%d", g_ulSleepLogFileIndex);

    VOS_StrNCpy((VOS_CHAR *)(cFileName+VOS_StrLen(g_acSleepLogFilePath)), acIndex, VOS_StrLen(acIndex));

    return VOS_OK;
}


VOS_UINT32 SLEEP_InfoWriteLog( VOS_UINT32 *pstInfo, VOS_UINT32 uLen )
{
    VOS_UINT32                          ulLen;
    VOS_CHAR                            ucFileName[SLEEP_INFO_MAX_NAME_LEN];
    FILE                                *fp;

    if ( VOS_NULL_PTR == pstInfo || VOS_NULL == uLen )
    {
        return VOS_ERR;
    }

    if (VOS_ERR == SLEEP_InfoFileName(ucFileName))
    {
        return VOS_ERR;
    }

    fp = DRV_FILE_OPEN((VOS_CHAR*)ucFileName, "ab+");   /*按照追加方式打开文件*/

    if(VOS_NULL_PTR == fp)                              /*如果文件打开失败说明建立文件也不成功*/
    {
        return VOS_ERR;
    }

    DRV_FILE_LSEEK(fp, 0, DRV_SEEK_END);

    ulLen = (VOS_UINT32)DRV_FILE_TELL(fp);

    if(ulLen >= SLEEP_INFO_MAX_SIZE)                    /*文件大小超过限制*/
    {
        DRV_FILE_CLOSE(fp);

        g_ulSleepLogFileIndex++;

        if (g_ulSleepLogFileIndex >= SLEEP_INFO_MAX_FILE)
        {
            g_ulSleepLogFileIndex = 0;
        }

        /* 将当前文件序列值记录至Sleep_log文件中 */
        fp = DRV_FILE_OPEN((VOS_CHAR*)g_acSleepLogFilePath, "wb+");

        if(VOS_NULL_PTR == fp)
        {
            return VOS_ERR;
        }

        DRV_FILE_WRITE(&g_ulSleepLogFileIndex, sizeof(VOS_UINT32), 1, fp);

        DRV_FILE_CLOSE(fp);

        /* 获取新记录文件名 */
        if (VOS_ERR == SLEEP_InfoFileName(ucFileName))
        {
            return VOS_ERR;
        }

        fp = DRV_FILE_OPEN((VOS_CHAR*)ucFileName, "wb+");  /*清空文件内容*/

        if(VOS_NULL_PTR == fp)                      /*如果文件打开失败说明建立文件也不成功*/
        {
            return VOS_ERR;
        }
    }

    DRV_FILE_WRITE(pstInfo, sizeof(VOS_UINT32), uLen, fp);

    DRV_FILE_CLOSE(fp);

    return VOS_OK;
}
VOS_UINT32 SLEEP_GetLogPath(VOS_CHAR *pucBuf, VOS_CHAR *pucOldPath, VOS_CHAR *pucUnitaryPath)
{
    NV_PRODUCT_TYPE_FOR_LOG_DIRECTORY   stLogDir;

    if (VOS_OK != NV_Read(en_NV_Item_ProductTypeForLogDirectory, &stLogDir, sizeof(stLogDir)))
    {
        return VOS_ERR;
    }

    /*0:MBB V3R3 stick/E5,etc.; 1:V3R3 M2M & V7R2; 2:V9R1 phone; 3:K3V3&V8R1;*/
    if ((1 == stLogDir.usProductType) || (3 == stLogDir.usProductType))
    {
        VOS_StrCpy(pucBuf, pucUnitaryPath);
    }
    else
    {
        VOS_StrCpy(pucBuf, pucOldPath);
    }

    return VOS_OK;
}
VOS_VOID  SLEEP_ActivateHWMsgProc( MsgBlock *pstMsg )
{
    SLEEP_ISR_MSG_STRU                 *pstIsrReq;
    MODEM_ID_ENUM_UINT16                enModem;
    SLEEP_MODE_ENUM_UINT32              enMode;
    VOS_UINT32                          aulSliceInfo[SLEEP_INFO_MAX_LEN];

#if defined (INSTANCE_1)
    enModem = MODEM_ID_1;
#else
    enModem = MODEM_ID_0;
#endif

    pstIsrReq   = (SLEEP_ISR_MSG_STRU*)pstMsg;

    enMode      = pstIsrReq->enRatMode;

    aulSliceInfo[0] = 0xAA5555AA;
    aulSliceInfo[1] = OM_GetSlice();
    aulSliceInfo[2] = enModem;
    aulSliceInfo[3] = enMode;
    aulSliceInfo[4] = g_pstSleepNormalState->astRecordItem[enMode][SLEEP_MASTER_BBP_WAKE].ulSlice;
    aulSliceInfo[5] = g_pstSleepNormalState->astRecordItem[enMode][SLEEP_MASTER_ACTIVATE_START].ulSlice;
    aulSliceInfo[6] = g_pstSleepDeviceState->aulDeviceUpSlice[enModem][enMode][PWC_COMM_MODULE_ABB];
    aulSliceInfo[7] = g_pstSleepDeviceState->aulDeviceUpSlice[enModem][enMode][PWC_COMM_MODULE_BBP_DRX];
    aulSliceInfo[8] = g_pstSleepDeviceState->aulPLLUpSlice[enModem][enMode][PWC_COMM_MODULE_BBP_DRX];
    aulSliceInfo[9] = g_pstSleepDeviceState->aulPLLUpSlice[enModem][enMode][PWC_COMM_MODULE_ABB];
    aulSliceInfo[10] = g_pstSleepDeviceState->aulDeviceUpSlice[enModem][enMode][PWC_COMM_MODULE_RF];
    aulSliceInfo[11] = g_pstSleepNormalState->astRecordItem[enMode][SLEEP_MASTER_ACTIVATE_END].ulSlice;
    aulSliceInfo[12] = g_pstSleepNormalState->astRecordItem[enMode][SLEEP_MASTER_BBP_NORMAL].ulSlice;
    aulSliceInfo[13] = g_pstSleepNormalState->astRecordItem[enMode][SLEEP_MASTER_WAKE_PHY_PRE].ulSlice;
    aulSliceInfo[14] = g_pstSleepNormalState->astRecordItem[enMode][SLEEP_MASTER_WAKE_PHY].ulSlice;
    aulSliceInfo[15] = 0x55AAAA55;

    SLEEP_InfoWriteLog(aulSliceInfo, SLEEP_INFO_MAX_LEN);

    return;
}


VOS_UINT32 SLEEP_MasterWakeSlave(SLEEP_MODE_ENUM_UINT32 enRatMode)
{
    switch (enRatMode)
    {
        case VOS_RATMODE_WCDMA:
        case VOS_RATMODE_GSM:
            SLEEP_IsrProc(SLEEP_TL_WAKE_GU_IND, VOS_RATMODE_WCDMA);
            break;

        case VOS_RATMODE_LTE:
        case VOS_RATMODE_TDS:
#if (RAT_MODE != RAT_GU)
            TLSLEEP_MasterAwakeSlave();
#endif
            break;

        default:
            return VOS_ERR;
    }

    return VOS_OK;
}


VOS_VOID SLEEP_MasterForceWake(SLEEP_MODE_ENUM_UINT32 enMode)
{
    if (SLEEP_MASTER_LP_STATE == g_astSleepModeInfo[enMode].enSystemState)
    {
        /* 禁止深睡投票 */
        SLEEP_VoteLock(enMode);

        /* 上电RTT子系统 */
        SLEEP_ActivateHw(enMode);

        SLEEP_NOR_STATE_TRACK(enMode, SLEEP_MASTER_FORCE_WAKE);

        /*判断最近是否有正常WAKE中断上报*/
        if (VOS_RATMODE_WCDMA == enMode)
        {
            if (VOS_OK != SLEEP_ForceWakeProtected())
            {
                return;
            }

            /* 强制唤醒W BBP */
            HPA_Write32RegMask(WBBP_SWITCH_UP_1CARD_ADDR, BBP_AWAKE_BIT);
        }
        else
        {
            /* 强制唤醒G BBP */
            HPA_Write32RegMask(OAM_GBBP_DRX_ARM_WAKE_EN_ADDR, BIT_N(2));
        }

        g_astSleepModeInfo[enMode].enWakeType    = UPHY_OAM_FORCE_WAKE;
        g_astSleepModeInfo[enMode].enSystemState = SLEEP_TEMP_NORMAL_STATE;
    }

    return;
}
VOS_VOID SLEEP_SlaveForceWake(SLEEP_MODE_ENUM_UINT32 enMode)
{
    VOS_UINT32                          ulStartSlice;

    /* 判断当前是否在从模低功耗状态 */
    if (SLEEP_SLAVE_LP_STATE == g_astSleepModeInfo[enMode].enSystemState)
    {
        ulStartSlice    = OM_GetSlice();

        /* 禁止深睡投票 */
        SLEEP_VoteLock(enMode);

        /* 上电RTT */
        SLEEP_ActivateHw(enMode);

        SLEEP_MSG_RECORD(ID_OAM_UPHY_SLAVE_WAKE_CMD, DSP_PID_DRX, ulStartSlice);

        /* 发送消息给PHY_IDLE唤醒PHY */
        SLEEP_WakePhy(ID_OAM_UPHY_SLAVE_WAKE_CMD, enMode, UPHY_OAM_FORCE_WAKE);

        SLEEP_NOR_STATE_TRACK(enMode, SLEEP_SLAVE_FORCE_WAKE);

        g_astSleepModeInfo[enMode].enSystemState = SLEEP_NORMAL_STATE;
    }

    return;
}


VOS_VOID SLEEP_HookMsgProc(MsgBlock *pstMsg)
{
    if (VOS_FALSE != VOS_CheckInterrupt())
    {
        return;
    }

    /* 判断是否有消息发送给WPHY */
    if (DSP_PID_WPHY == pstMsg->ulReceiverPid)
    {
        (VOS_VOID)VOS_SmP(g_ulSleepSem, 0);

        SLEEP_MasterForceWake(VOS_RATMODE_WCDMA);
        SLEEP_SlaveForceWake(VOS_RATMODE_WCDMA);

        VOS_SmV(g_ulSleepSem);

        return;
    }

    /* 判断是否有消息发送给GPHY */
    if (DSP_PID_GPHY == pstMsg->ulReceiverPid)
    {
        (VOS_VOID)VOS_SmP(g_ulSleepSem, 0);

        SLEEP_MasterForceWake(VOS_RATMODE_GSM);
        SLEEP_SlaveForceWake(VOS_RATMODE_GSM);

        VOS_SmV(g_ulSleepSem);

        return;
    }

#if ( FEATURE_MULTI_MODEM == FEATURE_ON )

#if defined (INSTANCE_1)
    /* 在MODEM1 中不需要在调用其他HOOK函数 */
#else
    /*lint -e718 -e746 修改人:ganlan;检视人:徐铖 51137 */
    /* 在双实例中需要通知MODEM1 SLEEP模块的钩子函数 */
    I1_SLEEP_HookMsgProc(pstMsg);
    /*lint +e718 +e746 修改人:ganlan;检视人:徐铖 51137 */
#endif  /*defined (INSTANCE_1)*/

#endif  /*( FEATURE_MULTI_MODEM == FEATURE_ON )*/

    return;
}


VOS_VOID SLEEP_MsgProc(MsgBlock* pstMsg)
{
    SLEEP_MSG_FUN_STRU                 *pstMsgFun;
    VOS_UINT32                          ulTlbSize;
    VOS_UINT32                          ulIndex;
    UPHY_OAM_SLEEP_IND_STRU             *pstPhyReq;
    VOS_UINT32                          ulStartSlice = 0;

    pstPhyReq = (UPHY_OAM_SLEEP_IND_STRU*)pstMsg;

    /* 中断消息处理 */
    if (WUEPS_PID_SLEEP == pstPhyReq->ulSenderPid)
    {
        pstMsgFun = g_astSleepIsrMsgFunTbl;
        ulTlbSize = sizeof(g_astSleepIsrMsgFunTbl)/sizeof(SLEEP_MSG_FUN_STRU);
    }
    /* PHY发来消息处理*/
    else if (DSP_PID_DRX == pstPhyReq->ulSenderPid)
    {
        pstMsgFun = g_astSleepPhyMsgFunTbl;
        ulTlbSize = sizeof(g_astSleepPhyMsgFunTbl)/sizeof(SLEEP_MSG_FUN_STRU);
    }
    else
    {
        /* 未知消息直接返回 */
        LogPrint1("SLEEP_MsgProc: Unknown ulSenderPid:%d.\r\n",
                        (VOS_INT)pstMsg->ulSenderPid);

        SLEEP_MSG_RECORD(pstPhyReq->enMsgId, pstPhyReq->ulSenderPid, ulStartSlice);

        return;
    }

    for (ulIndex = 0; ulIndex < ulTlbSize; ulIndex++)
    {
        /* 查找到对应消息ID的处理函数 */
        if (pstPhyReq->enMsgId == (UPHY_OAM_MSG_ID_ENUM_UINT16)pstMsgFun[ulIndex].ulMsgId)
        {
            ulStartSlice = OM_GetSlice();

            pstMsgFun[ulIndex].pMsgFun(pstMsg);

            /* 记录处理的消息 */
            SLEEP_MSG_RECORD(pstPhyReq->enMsgId, pstPhyReq->enRatMode, ulStartSlice);

            return;
        }
    }

    SLEEP_MSG_RECORD(pstPhyReq->enMsgId, pstPhyReq->ulSenderPid, ulStartSlice);

    /* 异常打印和记录 */
    LogPrint2("SLEEP_MsgProc: Unknown Msg, ulSenderPid:%d, enMsgId:%d.\r\n",
                (VOS_INT)pstPhyReq->ulSenderPid, (VOS_INT)pstPhyReq->enMsgId);
    return;
}


VOS_VOID SLEEP_Init(VOS_VOID)
{
    SLEEP_MODE_ENUM_UINT32              enMode;
    VOS_UINT32                          ulRecordAddr;

    /* 初始化定位信息 */
    ulRecordAddr = DRV_EXCH_MEM_MALLOC(VOS_DUMP_MEM_TOTAL_SIZE);

#if defined (INSTANCE_1)
    if (VOS_NULL_PTR != ulRecordAddr)
    {
        ulRecordAddr += sizeof(VOS_UINT32);

        ulRecordAddr += sizeof(SLEEP_TRACK_RECORD_STRU) + sizeof(SLEEP_NORMAL_STATE_RECORD_STRU)
                        + sizeof(SLEEP_DEVICE_STATE_STRU) + sizeof(SLEEP_INFO_STATE_STRU);
    }
#endif

    if (VOS_NULL_PTR != ulRecordAddr)
    {
        *(VOS_UINT32*)ulRecordAddr = 0x5555aaaa;

        ulRecordAddr += sizeof(VOS_UINT32);

        g_pstSleepTrackRecord = (SLEEP_TRACK_RECORD_STRU*)ulRecordAddr;

        VOS_MemSet(g_pstSleepTrackRecord, 0, sizeof(SLEEP_TRACK_RECORD_STRU));

        ulRecordAddr += sizeof(SLEEP_TRACK_RECORD_STRU);

        g_pstSleepNormalState = (SLEEP_NORMAL_STATE_RECORD_STRU*)ulRecordAddr;

        VOS_MemSet(g_pstSleepNormalState, 0, sizeof(SLEEP_NORMAL_STATE_RECORD_STRU));

        ulRecordAddr += sizeof(SLEEP_NORMAL_STATE_RECORD_STRU);

        g_pstSleepDeviceState   = (SLEEP_DEVICE_STATE_STRU *)ulRecordAddr;

        VOS_MemSet(g_pstSleepDeviceState,   0, sizeof(SLEEP_DEVICE_STATE_STRU));

        ulRecordAddr += sizeof(SLEEP_DEVICE_STATE_STRU);

        g_pstSleepDeviceState->ulDeviceFlag = 0x5A5A5A5A;

        g_pstSleepInfoSlice      = (SLEEP_INFO_STATE_STRU *)ulRecordAddr;

        ulRecordAddr += sizeof(SLEEP_INFO_STATE_STRU);

        g_pstSleepInfoSlice->ulEndFlag   = 0x87878787;
    }

    /* 初始化系统状态 */
    for (enMode = 0; enMode < SLEEP_GU_MODE_TYPE_BUTT; enMode++)
    {
        g_astSleepModeInfo[enMode].enSystemState = SLEEP_NORMAL_STATE;
    }

    if(VOS_OK != VOS_SmMCreate("SLEEP", VOS_SEMA4_PRIOR | VOS_SEMA4_INVERSION_SAFE, &g_ulSleepSem))
    {
        LogPrint("SLEEP_Init: Create SLEEP.\r\n");
        return;
    }

    if(VOS_OK != VOS_SmCCreate("TCXO", 0, VOS_SEMA4_FIFO, &g_ulSleepTcxoSem))
    {
        LogPrint("SLEEP_Init: Create TCXO.\r\n");
        return;
    }

    /* 读取NV器件稳定时间 */
    if(NV_OK != NV_Read(en_NV_Item_WG_DRX_RESUME_TIME, &g_stSleepDrxResumeTime, sizeof(WG_DRX_RESUME_TIME_STRU)))
    {
        LogPrint("SLEEP_Init: Read en_NV_Item_WG_DRX_RESUME_TIME Fail.\r\n");
        return;
    }

    /* 读取NV器件稳定时间 */
    if(NV_OK != NV_Read(en_NV_Item_GUMODE_CHAN_PARA, &g_stSleepChanPara, sizeof(NV_GUMODE_CHAN_PARA_STRU)))
    {
        LogPrint("SLEEP_Init: Read en_NV_Item_GUMODE_CHAN_PARA Fail.\r\n");
        return;
    }

    /* 读取DRX上电流程异常控制使能标志 */
    if(NV_OK != NV_Read(en_NV_Item_DRX_RESET_ENABLE_CFG, &g_bDrxResetFlag, sizeof(NV_SLEEP_DRX_RESET_ENABLE_STRU)))
    {
        LogPrint("SLEEP_Init: Read en_NV_Item_DRX_RESET_ENABLE_CFG Fail.\r\n");
        return;
    }

    if (VOS_OK != SLEEP_GetLogPath(g_acSleepLogDirPath, SLEEP_LOG_DIR, SLEEP_UNITARY_LOG_DIR))
    {
        /* 文件系统异常不需要退出功耗初始化流程 */
    }

    if (VOS_OK != SLEEP_GetLogPath(g_acSleepLogFilePath, SLEEP_LOG_PATH, SLEEP_UNITARY_LOG_PATH))
    {
        /* 文件系统异常不需要退出功耗初始化流程 */
    }

    SLEEP_TimeCal();

    /* 注册SLEEP钩子函数给OSA */
#if defined (INSTANCE_1)
#else
    VOS_RegisterAwakeFun(SLEEP_HookMsgProc);
#endif

    return;
}


VOS_UINT32 WuepsSleepPidInit(enum VOS_INIT_PHASE_DEFINE ip)
{
    switch( ip )
    {
        case VOS_IP_LOAD_CONFIG:
            break;

        default:
            break;
    }

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : WuepsSleepFidInit
 功能描述  : SLEEP 模块 FID 的初始化函数
 输入参数  : enum VOS_INIT_PHASE_DEFINE ip
 输出参数  : 无
 返 回 值  : VOS_UINT32 FID 初始化结果
 调用函数  :
 被调函数  :
*****************************************************************************/
VOS_UINT32 WuepsSleepFidInit(enum VOS_INIT_PHASE_DEFINE ip)
{
    VOS_UINT32                          ulRslt;

    switch( ip )
    {
        case VOS_IP_LOAD_CONFIG:
        {
            ulRslt = VOS_RegisterPIDInfo(WUEPS_PID_SLEEP,
                                        (Init_Fun_Type)WuepsSleepPidInit,
                                        (Msg_Fun_Type)SLEEP_MsgProc);

            if( VOS_OK != ulRslt )
            {
                return VOS_ERR;
            }

            ulRslt = VOS_RegisterTaskPrio(WUEPS_FID_SLEEP, COMM_SLEEP_TASK_PRIO);

            if( VOS_OK != ulRslt )
            {
                return VOS_ERR;
            }

            SLEEP_Init();

            break;
        }

        default:
            break;
    }

    return VOS_OK;
}

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

