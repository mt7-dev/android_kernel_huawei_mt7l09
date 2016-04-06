

/*****************************************************************************
   1 头文件包含
******************************************************************************/

#include "VcCodecInterface.h"
#include "OmCodecInterface.h"
#include "PhyCodecInterface.h"
#include "TdmacCodecInterface.h"
#include "CodecInterface.h"
#include "hifidrvinterface.h"
#include "fsm.h"
#include "om_log.h"
#include "codec_typedefine.h"
#include "voice_api.h"
#include "voice_mc.h"
#include "voice_proc.h"
#include "voice_amr_mode.h"
#include "voice_debug.h"

#ifdef  __cplusplus
#if  __cplusplus
extern "C"{
#endif
#endif

/*****************************************************************************
    可维可测信息中包含的C文件编号宏定义
*****************************************************************************/
/*lint -e(767)*/
#define THIS_FILE_ID                    OM_FILE_ID_VOICE_API_C

/*****************************************************************************
   2 全局变量声明
******************************************************************************/

/* 所有运行状态以及网络状态下都需要处理的任务表 */
#define VOICE_ALL_STATE_ALL_MODE_ACT_ITEMS \
/*{*/                                                                                               \
    /* 三个子项定义:    - PID -              - 消息类型 -                    - 对应函数- */         \
    FSM_ACT_TBL_ITEM( 0x0000,       ID_VC_VOICE_SET_VOLUME_REQ,     VOICE_MsgVcSetVolReqIRP),       \
    FSM_ACT_TBL_ITEM( 0x0000,       ID_OM_VOICE_SET_HOOK_REQ,       VOICE_MsgOmSetHookReqIRP),      \
    FSM_ACT_TBL_ITEM( 0x0000,       ID_OM_VOICE_SET_LOOP_REQ,       VOICE_MsgOmSetLoopReqIRP),      \
    FSM_ACT_TBL_ITEM( 0x0000,       ID_OM_VOICE_QUERY_STATUS_REQ,   VOICE_MsgOmQueryStatusReqIRP),  \
    FSM_ACT_TBL_ITEM( 0x0000,       ID_MAC_VOICE_MODE_SET_IND,      VOICE_MsgUmtsMacModeSetIndIRP), \
    FSM_ACT_TBL_ITEM( 0x0000,       ID_TDMAC_VOICE_MODE_SET_IND,    VOICE_MsgTDmacModeSetIndIRP),   \
    FSM_ACT_TBL_ITEM( 0x0000,       ID_VC_VOICE_LOOP_REQ,           VOICE_MsgVoiceLoopReqIRP),      \
    FSM_ACT_TBL_ITEM( 0x0000,       ID_IMSA_VOICE_CFG_REQ,          VOICE_MsgImsaCfgReqR),
/*}*/

/* IDLE 状态下的任务表 */
ACT_STRU g_astVoiceIdleActTbl[] =
{
    /* 三个子项定义:    - PID -              - 消息类型 -                    - 对应函数- */
    FSM_ACT_TBL_ITEM( 0x0000,       ID_VC_VOICE_START_REQ,          VOICE_MsgVcStartReqI),  \
    FSM_ACT_TBL_ITEM( 0x0000,       ID_VC_VOICE_SET_DEVICE_REQ,     VOICE_MsgVcSetDevReqI), \
    FSM_ACT_TBL_ITEM( 0x0000,       ID_AP_HIFI_CCPU_RESET_REQ,      VOICE_MsgApNoteCCPUResetRP),    \
   VOICE_ALL_STATE_ALL_MODE_ACT_ITEMS
};

/* RUNNING态中所有网络状态下都需要处理的任务表 */
#define VOICE_RUNNING_ALL_MODE_ACT_ITEMS \
/*{*/                                                                                               \
    /* 三个子项定义:    - PID -              - 消息类型 -                    - 对应函数- */         \
    FSM_ACT_TBL_ITEM( 0x0000,       ID_VC_VOICE_STOP_REQ,           VOICE_MsgVcStopReqRP),          \
    FSM_ACT_TBL_ITEM( 0x0000,       ID_VC_VOICE_SET_DEVICE_REQ,     VOICE_MsgVcSetDevReqRP),        \
    FSM_ACT_TBL_ITEM( 0x0000,       ID_VC_VOICE_SET_CODEC_REQ,      VOICE_MsgVcSetCodecReqR),       \
    FSM_ACT_TBL_ITEM( 0x0000,       ID_VOICE_VOICE_ENC_REQ,         VOICE_MsgVoiceTxProcReqR),      \
    FSM_ACT_TBL_ITEM( 0x0000,       ID_VOICE_VOICE_DEC_REQ,         VOICE_MsgVoiceRxProcReqR),      \
    FSM_ACT_TBL_ITEM( 0x0000,       ID_OM_VOICE_SET_NV_REQ,         VOICE_MsgOmSetNvReqRP),         \
    FSM_ACT_TBL_ITEM( 0x0000,       ID_OM_VOICE_GET_NV_REQ,         VOICE_MsgOmGetNvReqRP),         \
    FSM_ACT_TBL_ITEM( 0x0000,       ID_VC_VOICE_BACKGROUND_REQ,     VOICE_MsgVcBackGroundReqRP),    \
    FSM_ACT_TBL_ITEM( 0x0000,       ID_VC_VOICE_FOREGROUND_REQ,     VOICE_MsgVcForeGroundReqRP),    \
    FSM_ACT_TBL_ITEM( 0x0000,       ID_VC_VOICE_GROUND_QRY,         VOICE_MsgVcGroundQryReqRP),     \
    FSM_ACT_TBL_ITEM( 0x0000,       ID_AP_HIFI_CCPU_RESET_REQ,      VOICE_MsgApNoteCCPUResetRP),    \
    VOICE_ALL_STATE_ALL_MODE_ACT_ITEMS
/*}*/

/* RUNNING 状态下WCDMA模式的任务表 */
ACT_STRU g_astVoiceRunningWcdmaActTbl[] =
{
    /* 三个子项定义:    - PID -              - 消息类型 -                    - 对应函数- */
    FSM_ACT_TBL_ITEM( 0x0000,       ID_MAC_VOICE_MODE_CHANGE_IND,   VOICE_MsgUmtsMacModeChangeIndR),
    FSM_ACT_TBL_ITEM( 0x0000,       ID_WPHY_VOICE_RX_DATA_IND,      VOICE_MsgWphyRxDataIndR),
    FSM_ACT_TBL_ITEM( 0x0000,       ID_WPHY_VOICE_BACKGROUND_CNF,   VOICE_MsgGUPhyBackGroundCnfRP),
    FSM_ACT_TBL_ITEM( 0x0000,       ID_WPHY_VOICE_FOREGROUND_CNF,   VOICE_MsgGUPhyForeGroundCnfRP),
    FSM_ACT_TBL_ITEM( 0x0000,       ID_WPHY_VOICE_CHANNEL_QUALITY_IND,   VOICE_MsgWphyChannelQualityIndR),
    VOICE_RUNNING_ALL_MODE_ACT_ITEMS
};

/* RUNNING 状态下GSM模式的任务表 */
ACT_STRU g_astVoiceRunningGsmActTbl[] =
{
    /* 三个子项定义:    - PID -              - 消息类型 -                    - 对应函数- */
    FSM_ACT_TBL_ITEM( 0x0000,       ID_GPHY_VOICE_RX_DATA_IND,      VOICE_MsgGphyRxDataIndR),
    FSM_ACT_TBL_ITEM( 0x0000,       ID_GPHY_VOICE_BACKGROUND_CNF,   VOICE_MsgGUPhyBackGroundCnfRP),
    FSM_ACT_TBL_ITEM( 0x0000,       ID_GPHY_VOICE_FOREGROUND_CNF,   VOICE_MsgGUPhyForeGroundCnfRP),
    FSM_ACT_TBL_ITEM( 0x0000,       ID_GPHY_VOICE_CHANNEL_QUALITY_IND,   VOICE_MsgGphyChannelQualityIndR),
    VOICE_RUNNING_ALL_MODE_ACT_ITEMS
};

/* RUNNING 状态下TD-SCDMA模式的任务表 */
ACT_STRU g_astVoiceRunningTdsActTbl[] =
{

    /* 三个子项定义:    - PID -              - 消息类型 -                    - 对应函数- */
    FSM_ACT_TBL_ITEM( 0x0000,       ID_TDPHY_VOICE_RX_DATA_IND,     VOICE_MsgTDphyRxDataIndR),
    FSM_ACT_TBL_ITEM( 0x0000,       ID_TDMAC_VOICE_MODE_CHANGE_IND, VOICE_MsgTDMacModeChangeIndR),
    FSM_ACT_TBL_ITEM( 0x0000,       ID_TDPHY_VOICE_BACKGROUND_CNF,  VOICE_MsgTdPhyBackGroundCnfRP),
    FSM_ACT_TBL_ITEM( 0x0000,       ID_TDPHY_VOICE_FOREGROUND_CNF,  VOICE_MsgTdPhyForeGroundCnfRP),
    VOICE_RUNNING_ALL_MODE_ACT_ITEMS
};
/* RUNNING 状态下LTE模式的任务表 */
ACT_STRU g_astVoiceRunningLteActTbl[] =
{

    /* 三个子项定义:    - PID -              - 消息类型 -                    - 对应函数- */
    VOICE_RUNNING_ALL_MODE_ACT_ITEMS
};

/* PAUSE态中所有网络状态下都需要处理的任务表 */
#define VOICE_PAUSE_ALL_MODE_ACT_ITEMS \
/*{*/                                                                                               \
    /* 三个子项定义:    - PID -              - 消息类型 -                    - 对应函数- */         \
    FSM_ACT_TBL_ITEM( 0x0000,       ID_VC_VOICE_STOP_REQ,           VOICE_MsgVcStopReqRP),          \
    FSM_ACT_TBL_ITEM( 0x0000,       ID_VC_VOICE_SET_DEVICE_REQ,     VOICE_MsgVcSetDevReqRP),        \
    FSM_ACT_TBL_ITEM( 0x0000,       ID_VC_VOICE_SET_CODEC_REQ,      VOICE_MsgVcSetCodecReqP),       \
    FSM_ACT_TBL_ITEM( 0x0000,       ID_OM_VOICE_SET_NV_REQ,         VOICE_MsgOmSetNvReqRP),         \
    FSM_ACT_TBL_ITEM( 0x0000,       ID_OM_VOICE_GET_NV_REQ,         VOICE_MsgOmGetNvReqRP),         \
    FSM_ACT_TBL_ITEM( 0x0000,       ID_VC_VOICE_BACKGROUND_REQ,     VOICE_MsgVcBackGroundReqRP),    \
    FSM_ACT_TBL_ITEM( 0x0000,       ID_VC_VOICE_FOREGROUND_REQ,     VOICE_MsgVcForeGroundReqRP),    \
    FSM_ACT_TBL_ITEM( 0x0000,       ID_VC_VOICE_GROUND_QRY,         VOICE_MsgVcGroundQryReqRP),     \
    FSM_ACT_TBL_ITEM( 0x0000,       ID_AP_HIFI_CCPU_RESET_REQ,      VOICE_MsgApNoteCCPUResetRP),    \
    VOICE_ALL_STATE_ALL_MODE_ACT_ITEMS
/*}*/

/* PAUSE 状态下WCDMA模式的任务表 */
ACT_STRU g_astVoicePauseWcdmaActTbl[] =
{
    FSM_ACT_TBL_ITEM( 0x0000,       ID_WPHY_VOICE_BACKGROUND_CNF,   VOICE_MsgGUPhyBackGroundCnfRP),
    FSM_ACT_TBL_ITEM( 0x0000,       ID_WPHY_VOICE_FOREGROUND_CNF,   VOICE_MsgGUPhyForeGroundCnfRP),
    VOICE_PAUSE_ALL_MODE_ACT_ITEMS
};

/* PAUSE 状态下GSM模式的任务表 */
ACT_STRU g_astVoicePauseGsmActTbl[] =
{
    FSM_ACT_TBL_ITEM( 0x0000,       ID_GPHY_VOICE_BACKGROUND_CNF,   VOICE_MsgGUPhyBackGroundCnfRP),
    FSM_ACT_TBL_ITEM( 0x0000,       ID_GPHY_VOICE_FOREGROUND_CNF,   VOICE_MsgGUPhyForeGroundCnfRP),
    VOICE_PAUSE_ALL_MODE_ACT_ITEMS
};

/* PAUSE 状态下TD模式的任务表 */
ACT_STRU g_astVoicePauseTdActTbl[] =
{
    FSM_ACT_TBL_ITEM( 0x0000,       ID_TDPHY_VOICE_BACKGROUND_CNF,  VOICE_MsgTdPhyBackGroundCnfRP),
    FSM_ACT_TBL_ITEM( 0x0000,       ID_TDPHY_VOICE_FOREGROUND_CNF,  VOICE_MsgTdPhyForeGroundCnfRP),
    VOICE_PAUSE_ALL_MODE_ACT_ITEMS
};

/* PAUSE 状态下LTE模式的任务表 */
ACT_STRU g_astVoicePauseLteActTbl[] =
{
    VOICE_PAUSE_ALL_MODE_ACT_ITEMS
};

/* PAUSE 状态下的任务表 */
ACT_STRU g_astVoicePauseActTbl[] =
{
    /* 三个子项定义:    - PID -              - 消息类型 -                    - 对应函数- */
    VOICE_PAUSE_ALL_MODE_ACT_ITEMS
};

/* VOICE状态表定义 */
STA_STRU g_astVoiceFsmStaTbl[] =
{
    FSM_STA_TBL_ITEM(VOICE_MC_STATE_IDLE,       CODEC_NET_MODE_BUTT,    g_astVoiceIdleActTbl),
    FSM_STA_TBL_ITEM(VOICE_MC_STATE_RUNNING,    CODEC_NET_MODE_W,       g_astVoiceRunningWcdmaActTbl),
    FSM_STA_TBL_ITEM(VOICE_MC_STATE_RUNNING,    CODEC_NET_MODE_G,       g_astVoiceRunningGsmActTbl),
    FSM_STA_TBL_ITEM(VOICE_MC_STATE_RUNNING,    CODEC_NET_MODE_TD,      g_astVoiceRunningTdsActTbl),
    FSM_STA_TBL_ITEM(VOICE_MC_STATE_RUNNING,    CODEC_NET_MODE_L,       g_astVoiceRunningLteActTbl),
    FSM_STA_TBL_ITEM(VOICE_MC_STATE_PAUSE,      CODEC_NET_MODE_BUTT,    g_astVoicePauseActTbl),
    FSM_STA_TBL_ITEM(VOICE_MC_STATE_PAUSE,      CODEC_NET_MODE_W,       g_astVoicePauseWcdmaActTbl),
    FSM_STA_TBL_ITEM(VOICE_MC_STATE_PAUSE,      CODEC_NET_MODE_G,       g_astVoicePauseGsmActTbl),
    FSM_STA_TBL_ITEM(VOICE_MC_STATE_PAUSE,      CODEC_NET_MODE_TD,      g_astVoicePauseTdActTbl),
    FSM_STA_TBL_ITEM(VOICE_MC_STATE_PAUSE,      CODEC_NET_MODE_L,       g_astVoicePauseLteActTbl),
};

/* VOICE流程状态机 */
FSM_DESC_STRU   g_stVoiceFsmDesc;


/* 所有运行状态以及网络状态下都需要处理的任务表 */
#define VOICE_RT_ALL_STATE_ALL_MODE_ACT_ITEMS \
/*{*/                                                                                           \
    /* 三个子项定义:    - PID -              - 消息类型 -                    - 对应函数- */     \
/*}*/

/* RUNNING态中所有网络状态下都需要处理的任务表 */
#define VOICE_RT_RUNNING_ALL_MODE_ACT_ITEMS \
/*{*/                                                                                           \
    /* 三个子项定义:    - PID -              - 消息类型 -                    - 对应函数- */     \
    FSM_ACT_TBL_ITEM( 0x0000, ID_UCOM_VOICE_SUSPEND_CMD,      VOICE_MsgPhySuspendCmdR),         \
    FSM_ACT_TBL_ITEM( 0x0000, ID_VOICE_VOICE_PLAY_IND,        VOICE_MsgVoicePlayIndR),          \
    FSM_ACT_TBL_ITEM( 0x0000, ID_VOICE_VOICE_RECORD_IND,      VOICE_MsgVoiceRecordIndR),        \
    VOICE_RT_ALL_STATE_ALL_MODE_ACT_ITEMS
/*}*/

/* RUNNING 状态下WCDMA模式的任务表 */
ACT_STRU g_astVoiceRtRunningWcdmaActTbl[] =
{
    /* 三个子项定义:    - PID -              - 消息类型 -                    - 对应函数- */
    FSM_ACT_TBL_ITEM( 0x0000, ID_WPHY_VOICE_SYNC_IND,        VOICE_MsgWphySyncIndR),
    VOICE_RT_RUNNING_ALL_MODE_ACT_ITEMS
};

/* RUNNING 状态下GSM模式的任务表 */
ACT_STRU g_astVoiceRtRunningGsmActTbl[] =
{
    /* 三个子项定义:    - PID -              - 消息类型 -                    - 对应函数- */
    FSM_ACT_TBL_ITEM( 0x0000, ID_GPHY_VOICE_UPDATE_PARA_CMD,  VOICE_MsgGphyUpdateParaCmdRP),
    FSM_ACT_TBL_ITEM( 0x0000, ID_GPHY_VOICE_PLAY_IND,         VOICE_MsgGphyPlayIndR),
    FSM_ACT_TBL_ITEM( 0x0000, ID_GPHY_VOICE_RECORD_IND,       VOICE_MsgGphyRecordIndR),
    FSM_ACT_TBL_ITEM( 0x0000, ID_GPHY_VOICE_SYNC_IND,         VOICE_MsgGphySyncIndR),
    VOICE_RT_RUNNING_ALL_MODE_ACT_ITEMS
};

/* RUNNING 状态下TD-SCDMA模式的任务表 */
ACT_STRU g_astVoiceRtRunningTdsActTbl[] =
{
    /* 三个子项定义:    - PID -              - 消息类型 -                    - 对应函数- */
    FSM_ACT_TBL_ITEM( 0x0000, ID_VOICE_VOICE_TD_SYNC_IND,     VOICE_MsgTdSyncIndR),
    VOICE_RT_RUNNING_ALL_MODE_ACT_ITEMS
};

/* RUNNING 状态下Lte模式的任务表 */
ACT_STRU g_astVoiceRtRunningLteActTbl[] =
{
    /* 三个子项定义:    - PID -              - 消息类型 -                    - 对应函数- */
    FSM_ACT_TBL_ITEM( 0x0000,       ID_IMSA_VOICE_RX_DATA_IND,      VOICE_MsgImsaRxDataIndR),
    VOICE_RT_RUNNING_ALL_MODE_ACT_ITEMS
};

/* VOICE状态表定义 */
STA_STRU g_astVoiceRtFsmStaTbl[] =
{
    FSM_STA_TBL_ITEM(VOICE_MC_STATE_RUNNING,    CODEC_NET_MODE_W,       g_astVoiceRtRunningWcdmaActTbl),
    FSM_STA_TBL_ITEM(VOICE_MC_STATE_RUNNING,    CODEC_NET_MODE_G,       g_astVoiceRtRunningGsmActTbl),
    FSM_STA_TBL_ITEM(VOICE_MC_STATE_RUNNING,    CODEC_NET_MODE_TD,      g_astVoiceRtRunningTdsActTbl),
    FSM_STA_TBL_ITEM(VOICE_MC_STATE_RUNNING,    CODEC_NET_MODE_L,       g_astVoiceRtRunningLteActTbl),
};

/* VOICE流程状态机 */
FSM_DESC_STRU   g_stVoiceRtFsmDesc;


/*****************************************************************************
   3 外部函数声明
******************************************************************************/

/*****************************************************************************
   4 函数实现
******************************************************************************/


VOS_UINT32 VOICE_ApiGetCurState(VOS_VOID *pstOsaMsg)
{
    VOS_UINT32 uwMcState;
    VOS_UINT32 uwCurState;
    MsgBlock  *pstMsg    = VOS_NULL;

    pstMsg      = (MsgBlock*)pstOsaMsg;

    /* 获取语音软件运行状态 */
    uwMcState = VOICE_McGetVoiceState(pstMsg->uwSenderPid);

    /* 在高16bit放置主状态(语音软件运行状态) */
    uwCurState = (uwMcState << 16);

    /* 在低16bit放置辅状态(网络模式) */
    switch(uwMcState)
    {
        case VOICE_MC_STATE_IDLE:
        case VOICE_MC_STATE_PAUSE:
        {
            uwCurState |= (VOS_UINT32)CODEC_NET_MODE_BUTT;
        }
        break;

        case VOICE_MC_STATE_RUNNING:
        {
            uwCurState |= (VOS_UINT32)VOICE_McGetNetMode(pstMsg->uwSenderPid);
        }
        break;

        default:
        {
            uwCurState |= (VOS_UINT32)CODEC_NET_MODE_BUTT;
        }
    }

    return uwCurState;
}
VOS_UINT32 VOICE_ApiGetEventType(MsgBlock *pstOsaMsg)
{
    VOS_UINT32 uwSenderPid;
    VOS_UINT32 uwMsgId;
    VOS_UINT32 uwEventType;

    /* 获取发送者的PID*/
    #if 0
    uwSenderPid = pstOsaMsg->uwSenderPid;
    #else
    /* 当前 FSM_ACT_TBL_ITEM 中的 PID 定义都为 0 */
    uwSenderPid = 0;
    #endif

    /* 获取消息ID */
    uwMsgId     = (VOS_UINT32)(*(VOS_UINT16*)(pstOsaMsg->aucValue));

    /* 发送者消息PID放在高16bit,　消息ID放在低16bit */
    uwEventType = (uwSenderPid<<16) | uwMsgId;

    return uwEventType;
}


VOS_UINT32 VOICE_ApiPidInit(enum VOS_INIT_PHASE_DEFINE enInitPhrase)
{
    switch (enInitPhrase)
    {
        case VOS_IP_LOAD_CONFIG:
        {
            /*初始化PID_VOICE状态机*/
            if(VOICE_ApiFsmInit() != VOS_OK)
            {
                return VOS_ERR;
            }

            /* 初始化语音软件 */
            VOICE_McInit();

            break;
        }
        case VOS_IP_FARMALLOC:
        case VOS_IP_INITIAL:
        case VOS_IP_ENROLLMENT:
        case VOS_IP_LOAD_DATA:
        case VOS_IP_FETCH_DATA:
        case VOS_IP_STARTUP:
        case VOS_IP_RIVAL:
        case VOS_IP_KICKOFF:
        case VOS_IP_STANDBY:
        case VOS_IP_BROADCAST_STATE:
        case VOS_IP_RESTART:
        {
            break;
        }
        default:
        {
            break;
        }
    }

    return VOS_OK;
}


VOS_VOID VOICE_ApiPidProc(MsgBlock *pstOsaMsg)
{
    VOS_UINT16 uhwMsgId;

    uhwMsgId = *(VOS_UINT16*)(pstOsaMsg->aucValue);

    FSM_ProcessEvent(&g_stVoiceFsmDesc,
                     VOICE_ApiGetCurState(pstOsaMsg),
                     VOICE_ApiGetEventType(pstOsaMsg),
                     uhwMsgId,
                     pstOsaMsg);
}


VOS_UINT32 VOICE_ApiRtPidInit(enum VOS_INIT_PHASE_DEFINE enInitPhrase)
{
    switch (enInitPhrase)
    {
        case VOS_IP_LOAD_CONFIG:
        {
            /* 初始化PID_VOICE_RT状态机 */
            if(VOICE_ApiRtFsmInit() != VOS_OK)
            {
                return VOS_ERR;

            }

            break;
        }
        case VOS_IP_FARMALLOC:
        case VOS_IP_INITIAL:
        case VOS_IP_ENROLLMENT:
        case VOS_IP_LOAD_DATA:
        case VOS_IP_FETCH_DATA:
        case VOS_IP_STARTUP:
        case VOS_IP_RIVAL:
        case VOS_IP_KICKOFF:
        case VOS_IP_STANDBY:
        case VOS_IP_BROADCAST_STATE:
        case VOS_IP_RESTART:
        {
            break;
        }
        default:
        {
            break;
        }
    }

    return VOS_OK;
}
VOS_VOID VOICE_ApiRtPidProc(MsgBlock *pstOsaMsg)
{
    VOS_UINT16 uhwMsgId;

    uhwMsgId = *(VOS_UINT16*)(pstOsaMsg->aucValue);

    FSM_ProcessEvent(&g_stVoiceRtFsmDesc,
                     VOICE_ApiGetCurState(pstOsaMsg),
                     VOICE_ApiGetEventType(pstOsaMsg),
                     uhwMsgId,
                     pstOsaMsg);
}


VOS_UINT32 VOICE_ApiFsmInit( VOS_VOID )
{
    VOS_UINT32                          uwRslt;

    /* 状态机注册 */
    uwRslt = FSM_RegisterFsm(&(g_stVoiceFsmDesc),
                            (sizeof(g_astVoiceFsmStaTbl)/sizeof(STA_STRU)),
                             g_astVoiceFsmStaTbl,
                             VOICE_ApiFsmErr);

    return uwRslt;
}
VOS_UINT32 VOICE_ApiFsmErr(VOS_UINT32 ulEventType, VOS_VOID *pMsg)
{
    VOS_UINT32 uwRet;

    MsgBlock *pstBlk = (MsgBlock *)pMsg;

    uwRet = VOICE_McHandleFsmErr((VOS_UINT16)(pstBlk->uwSenderPid),
                                 (VOS_UINT16)VOICE_ApiGetMsgId(ulEventType));

    if(UCOM_RET_SUCC == uwRet)
    {

        OM_LogWarning2(VOICE_ApiPidProc_FsmErrHandled, ulEventType, VOICE_ApiGetCurState(pMsg));
    }
    else
    {

        OM_LogError2(VOICE_ApiPidProc_FsmErrUnHandled, ulEventType, VOICE_ApiGetCurState(pMsg));
    }

    return uwRet;
}


VOS_UINT32 VOICE_ApiRtFsmInit( VOS_VOID )
{
    VOS_UINT32                          uwRslt;

    /* 状态机注册 */
    uwRslt = FSM_RegisterFsm(&(g_stVoiceRtFsmDesc),
                            (sizeof(g_astVoiceRtFsmStaTbl)/sizeof(STA_STRU)),
                             g_astVoiceRtFsmStaTbl,
                             VOICE_ApiRtFsmErr);

    return uwRslt;
}
VOS_UINT32 VOICE_ApiRtFsmErr(VOS_UINT32 ulEventType, VOS_VOID *pMsg)
{
    VOS_UINT32 uwRet;

    uwRet = VOICE_McHandleRtFsmErr((VOS_UINT16)VOICE_ApiGetSenderPid(ulEventType),
                                   (VOS_UINT16)VOICE_ApiGetMsgId(ulEventType));

    if(UCOM_RET_SUCC == uwRet)
    {

        OM_LogWarning2(VOICE_ApiRtPidProc_FsmErrHandled, ulEventType, VOICE_ApiGetCurState(pMsg));
    }
    else
    {

        OM_LogError2(VOICE_ApiRtPidProc_FsmErrUnHandled, ulEventType, VOICE_ApiGetCurState(pMsg));
    }

    return uwRet;
}


VOS_UINT32  VOICE_ApiIsIdle(VOS_VOID)
{
    /*表示无业务运行，投票:可下电*/
    if (UCOM_RET_SUCC == VOICE_McApiIsIdle())
    {
        return VOS_OK;
    }
    else
    {
        return VOS_ERR;
    }
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

