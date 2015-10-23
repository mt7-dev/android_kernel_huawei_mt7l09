

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "om.h"
#include "fsm.h"
#include "audio_player.h"
#include "audio_recorder.h"
#include "audio_pcm.h"
#include "audio_comm.h"
#include "ucom_nv.h"
#include "audio_debug.h"
#include "dm3.h"
#include "audio_enhance.h"
#include "xa_dts_pp.h"
#include "audio_recorder.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
    可维可测信息中包含的C文件编号宏定义
*****************************************************************************/
/*lint -e(767)*/
#define THIS_FILE_ID                    OM_FILE_ID_AUDIO_COMM_C

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

/* AUDIO消息状态机(经简化,不进行状态切换) */
STA_STRU g_stAudioStatusDesc;

/* AUDIO实时消息状态机(经简化,不进行状态切换) */
STA_STRU g_stAudioRtStatusDesc;

/* AUDIO非实时消息处理表,此处没有排序 */
ACT_STRU g_astAudioMsgFuncTable[] =
{
    /* 子项定义格式: PID(reserve), 消息类型, 对应处理函数 */
    FSM_ACT_TBL_ITEM(0x0000, ID_AUDIO_UPDATE_PLAY_BUFF_CMD,     AUDIO_PCM_MsgUpdatePlayBuffCmd),
    FSM_ACT_TBL_ITEM(0x0000, ID_AUDIO_UPDATE_CAPTURE_BUFF_CMD,  AUDIO_PCM_MsgUpdateCaptureBuffCmd),
    FSM_ACT_TBL_ITEM(0x0000, ID_OM_AUDIO_SET_HOOK_REQ,          AUDIO_DEBUG_MsgSetHookReq),
    FSM_ACT_TBL_ITEM(0x0000, ID_OM_AUDIO_QUERY_STATUS_REQ,      AUDIO_DEBUG_MsgQueryStatusReq),
    FSM_ACT_TBL_ITEM(0x0000, ID_AUDIO_UPDATE_PLAYER_BUFF_CMD,   AUDIO_PLAYER_MsgUpdatePcmBuffCmd),
    FSM_ACT_TBL_ITEM(0x0000, ID_AP_HIFI_TEST_CMD,               AUDIO_DEBUG_MsgTestCmd),
    FSM_ACT_TBL_ITEM(0x0000, ID_AUDIO_OM_SET_NV_REQ,            AUDIO_ENHANCE_MsgOmSetNvReq),
    FSM_ACT_TBL_ITEM(0x0000, ID_AUDIO_OM_GET_NV_REQ,            AUDIO_ENHANCE_MsgOmGetNvReq),
    FSM_ACT_TBL_ITEM(0x0000, ID_AUDIO_PCM_ENHANCE_START_IND,    AUDIO_ENHANCE_MsgStartInd),
    FSM_ACT_TBL_ITEM(0x0000, ID_AUDIO_PCM_ENHANCE_STOP_IND,     AUDIO_ENHANCE_MsgStopInd),
    FSM_ACT_TBL_ITEM(0x0000, ID_AUDIO_PCM_ENHANCE_SET_DEVICE_IND,   AUDIO_ENHANCE_MsgSetDeviceInd),
    FSM_ACT_TBL_ITEM(0x0000, ID_AP_AUDIO_MLIB_SET_PARA_IND,     AUDIO_ENHANCE_MsgSetParaInd),
    FSM_ACT_TBL_ITEM(0x0000, ID_AUDIO_PLAYER_START_DECODE_IND,  AUDIO_PLAYER_MsgStartDecodeInd),
    FSM_ACT_TBL_ITEM(0x0000, ID_AUDIO_PLAYER_STOP_DECODE_IND,   AUDIO_PLAYER_MsgStopDecodeInd),
};

/* AUDIO实时消息处理表,此处没有排序 */
ACT_STRU g_astAudioRtMsgFuncTable[] =
{
    /* 子项定义格式: PID(reserve), 消息类型, 对应处理函数 */
    FSM_ACT_TBL_ITEM(0x0000, ID_AP_AUDIO_PLAY_START_REQ,        AUDIO_PLAYER_MsgStartReq),
    FSM_ACT_TBL_ITEM(0x0000, ID_AP_AUDIO_PLAY_PAUSE_REQ,        AUDIO_PLAYER_MsgStopReq),
    FSM_ACT_TBL_ITEM(0x0000, ID_AP_AUDIO_PLAY_UPDATE_BUF_CMD,   AUDIO_PLAYER_MsgUpdateApBuffCmd),
    FSM_ACT_TBL_ITEM(0x0000, ID_AP_AUDIO_PLAY_WAKEUPTHREAD_REQ, AUDIO_PLAYER_MsgPlayDoneSignalReq),
    FSM_ACT_TBL_ITEM(0x0000, ID_AP_AUDIO_PLAY_SET_VOL_CMD,      AUDIO_PLAYER_MsgSetVolReq),
    FSM_ACT_TBL_ITEM(0x0000, ID_AP_AUDIO_PLAY_QUERY_TIME_REQ,   AUDIO_PLAYER_MsgQueryTimeReq),
    FSM_ACT_TBL_ITEM(0x0000, ID_AP_AUDIO_PLAY_QUERY_STATUS_REQ, AUDIO_PLAYER_MsgQueryStatusReq),
    FSM_ACT_TBL_ITEM(0x0000, ID_AP_AUDIO_PLAY_SEEK_REQ,         AUDIO_PLAYER_MsgSeekReq),
    FSM_ACT_TBL_ITEM(0x0000, ID_AP_AUDIO_LOOP_BEGIN_REQ,        AUDIO_DEBUG_MsgAudioLoopOpenReq),
    FSM_ACT_TBL_ITEM(0x0000, ID_AP_AUDIO_LOOP_END_REQ,          AUDIO_DEBUG_MsgAudioLoopCloseReq),

    FSM_ACT_TBL_ITEM(0x0000, ID_AP_AUDIO_RECORD_START_CMD,      AUDIO_RECORDER_MsgRecordStartCmd),
    FSM_ACT_TBL_ITEM(0x0000, ID_AP_AUDIO_RECORD_STOP_CMD,       AUDIO_RECORDER_MsgRecordStopCmd),
    FSM_ACT_TBL_ITEM(0x0000, ID_AP_AUDIO_PCM_OPEN_REQ,          AUDIO_PCM_MsgPcmOpenReq),
    FSM_ACT_TBL_ITEM(0x0000, ID_AP_AUDIO_PCM_CLOSE_REQ,         AUDIO_PCM_MsgPcmCloseReq),
    FSM_ACT_TBL_ITEM(0x0000, ID_AP_AUDIO_PCM_HW_PARA_REQ,       AUDIO_PCM_MsgPcmHwParaReq),
    FSM_ACT_TBL_ITEM(0x0000, ID_AP_AUDIO_PCM_HW_FREE_REQ,       AUDIO_PCM_MsgPcmHwFreeReq),
    FSM_ACT_TBL_ITEM(0x0000, ID_AP_AUDIO_PCM_PREPARE_REQ,       AUDIO_PCM_MsgPcmPrepareReq),
    FSM_ACT_TBL_ITEM(0x0000, ID_AP_AUDIO_PCM_TRIGGER_REQ,       AUDIO_PCM_MsgPcmTriggerReq),
    FSM_ACT_TBL_ITEM(0x0000, ID_AP_AUDIO_PCM_POINTER_REQ,       AUDIO_PCM_MsgPcmPointerReq),
    FSM_ACT_TBL_ITEM(0x0000, ID_AP_AUDIO_PCM_SET_BUF_CMD,       AUDIO_PCM_MsgPcmSetBufCmd),
    FSM_ACT_TBL_ITEM(0x0000, ID_AP_HIFI_TEST_CMD,               AUDIO_DEBUG_MsgTestCmd),

    /* DTS音效消息处理函数 */
    FSM_ACT_TBL_ITEM(0x0000, ID_AP_AUDIO_SET_DTS_ENABLE_CMD,    AUDIO_PLAYER_MsgSetDtsEnableCmd),
    FSM_ACT_TBL_ITEM(0x0000, ID_AUDIO_PLAYER_SET_DTS_ENABLE_IND, XA_DTS_MsgSetDtsEnableCmd),
    FSM_ACT_TBL_ITEM(0x0000, ID_AP_AUDIO_SET_DTS_DEV_CMD,       XA_DTS_MsgSetDtsDevCmd),

    FSM_ACT_TBL_ITEM(0x0000, ID_AP_AUDIO_ENHANCE_START_REQ,     AUDIO_ENHANCE_MsgStartReqRt),
    FSM_ACT_TBL_ITEM(0x0000, ID_AP_AUDIO_ENHANCE_STOP_REQ,      AUDIO_ENHANCE_MsgStopReqRt),
    FSM_ACT_TBL_ITEM(0x0000, ID_AP_AUDIO_ENHANCE_SET_DEVICE_REQ,AUDIO_ENHANCE_MsgSetDeviceReqRt),
    FSM_ACT_TBL_ITEM(0x0000, ID_AP_AUDIO_MLIB_SET_PARA_IND,     AUDIO_ENHANCE_MsgSetParaIndRt),
};


/*****************************************************************************
  3 函数实现
*****************************************************************************/



VOS_VOID AUDIO_COMM_Init( VOS_VOID )
{
    VOS_UINT32 uwRet = VOS_OK;

    CODEC_NV_PARA_AUDIO_TRACE_CFG_STRU  stAudioTraceCfg;

    AUDIO_PLAYER_InitBuff();

    AUDIO_PCM_Init();

    /* 读取Audio TRACE相关NV项 */
    uwRet = UCOM_NV_Read(en_NV_AudioTraceCfg,
                         &stAudioTraceCfg,
                         sizeof(stAudioTraceCfg));

    if (VOS_OK != uwRet)
    {
        /* NV 读取失败 */
        OM_LogWarning1(UCOM_NV_Read_ParaError,uwRet);
    }

    /* audio调试功能初始化 */
    AUDIO_DEBUG_Init(&stAudioTraceCfg);

    /* enhance初始化 */
    AUDIO_ENHANCE_Init();

    /* 录音初始化 */
    AUDIO_RECOEDER_Init();

    return;
}
VOS_UINT32 AUDIO_COMM_PidInit(enum VOS_INIT_PHASE_DEFINE enInitPhrase)
{
    switch (enInitPhrase)
    {
        case VOS_IP_LOAD_CONFIG:
        {
            /*初始化PID_AUDIO状态机*/
            AUDIO_COMM_FuncTableInit();

            /* audio模块初始化 */
            AUDIO_COMM_Init();
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


VOS_VOID AUDIO_COMM_PidProc(MsgBlock *pstOsaMsg)
{
    VOS_UINT16     *puhwMsgId           = VOS_NULL;
    ACT_STRU       *pstRetAct           = VOS_NULL;
    STA_STRU       *pstMsgStatus        = AUDIO_COMM_GetMsgStatusPtr();
    ACT_STRU        stKeyAct;

    /* 获取消息ID */
    puhwMsgId = (VOS_UINT16*)pstOsaMsg->aucValue;

    stKeyAct.ulEventType = *puhwMsgId;

    /* 调用二分查找函数bsearch在事件处理表中查找相应的事件处理函数 */
    /* 如果找不到相应的事件处理函数,返回NULL */
    pstRetAct = (ACT_STRU*)FSM_Bsearch((VOS_INT16 *)(&stKeyAct),
                                       (VOS_UINT8 *)(pstMsgStatus->pActTable),
                                       pstMsgStatus->ulSize,
                                       sizeof(ACT_STRU),
                                       FSM_ActCompare);

    /* 若有消息处理函数与之对应,调用之 */
    if (VOS_NULL != pstRetAct)
    {
        (VOS_VOID )(*pstRetAct->pfActionFun)(pstOsaMsg);
    }
    else
    {
        /* 记录异常,DSP_PID_AUDIO收到未知消息,未处理 */
        OM_LogWarning1(AUDIO_COMM_PidProc_UnknownMsg, stKeyAct.ulEventType);
    }

    return;
}
VOS_UINT32 AUDIO_COMM_RtPidInit(enum VOS_INIT_PHASE_DEFINE enInitPhrase)
{
    switch (enInitPhrase)
    {
        case VOS_IP_LOAD_CONFIG:
        {
            /* 初始化PID_MED_RT状态机 */
            AUDIO_COMM_RtFuncTableInit();

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
VOS_VOID AUDIO_COMM_RtPidProc(MsgBlock *pstOsaMsg)
{
    VOS_UINT16     *puhwMsgId           = VOS_NULL;
    ACT_STRU       *pstRetAct           = VOS_NULL;
    STA_STRU       *pstMsgStatus        = AUDIO_COMM_GetRtMsgStatusPtr();
    ACT_STRU        stKeyAct;

    /* 获取消息ID */
    puhwMsgId = (VOS_UINT16*)pstOsaMsg->aucValue;

    stKeyAct.ulEventType = *puhwMsgId;

    /* 调用二分查找函数bsearch在事件处理表中查找相应的事件处理函数 */
    /* 如果找不到相应的事件处理函数,返回NULL */
    pstRetAct = (ACT_STRU*)FSM_Bsearch((VOS_INT16 *)(&stKeyAct),
                                       (VOS_UINT8 *)(pstMsgStatus->pActTable),
                                       pstMsgStatus->ulSize,
                                       sizeof(ACT_STRU),
                                       FSM_ActCompare);

    /* 若有消息处理函数与之对应,调用之 */
    if (VOS_NULL != pstRetAct)
    {
        (VOS_VOID )(*pstRetAct->pfActionFun)(pstOsaMsg);
    }
    else
    {
        /* 记录异常,DSP_PID_AUDIO_RT收到未知消息,未处理 */
        OM_LogWarning1(AUDIO_COMM_RtPidProc_UnknownMsg, stKeyAct.ulEventType);
    }

    return;
}
VOS_VOID AUDIO_COMM_FuncTableInit( VOS_VOID )
{
    STA_STRU       *pstStatusDec        = AUDIO_COMM_GetMsgStatusPtr();

    /* 初始化状态机，建立动作表链接*/
    pstStatusDec->pActTable  = AUDIO_COMM_GetMsgFuncTbl();
    pstStatusDec->ulSize     = sizeof(AUDIO_COMM_GetMsgFuncTbl())/sizeof(ACT_STRU);

    /* 将动作表按消息ID进行排序，从低到高顺序排列 */
    FSM_Sort((VOS_UINT8 *)pstStatusDec->pActTable,
             pstStatusDec->ulSize,
             sizeof(ACT_STRU),
             FSM_ActCompare);

    return;
}


VOS_VOID AUDIO_COMM_RtFuncTableInit( VOS_VOID )
{
    STA_STRU       *pstStatusDec        = AUDIO_COMM_GetRtMsgStatusPtr();

    /* 初始化状态机，建立动作表链接*/
    pstStatusDec->pActTable  = AUDIO_COMM_GetRtMsgFuncTbl();
    pstStatusDec->ulSize     = sizeof(AUDIO_COMM_GetRtMsgFuncTbl())/sizeof(ACT_STRU);

    /* 将动作表按消息ID进行排序，从低到高顺序排列 */
    FSM_Sort((VOS_UINT8 *)pstStatusDec->pActTable,
             pstStatusDec->ulSize,
             sizeof(ACT_STRU),
             FSM_ActCompare);

    return;
}


VOS_UINT32  AUDIO_COMM_IsIdle(VOS_VOID)
{
    /*表示无业务运行，投票:可下电*/
    if ((AUDIO_PCM_STATUS_DEACTIVE == AUDIO_PcmGetState())
        && (AUDIO_PLAYER_STATE_IDLE == AUDIO_PlayerGetState()))
    {
        return VOS_OK;
    }
    else
    {
        return VOS_ERR;
    }
}
VOS_VOID  AUDIO_COMM_CheckContext( VOS_VOID )
{
    VOS_UINT32      uwRet;

    uwRet = AUDIO_PLAYER_CheckBuff();

    if(uwRet != 0)
    {
        OM_LogFatalAndReset(AUDIO_COMM_BuffOverflow);
    }

}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif


