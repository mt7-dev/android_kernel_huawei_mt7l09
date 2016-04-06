

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "ucom_comm.h"
#include "ucom_nv.h"
#include "om_log.h"
#include "voice_log.h"
#include "voice_mc.h"
#include "voice_proc.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef _MED_ERRORLOG
/*****************************************************************************
    可维可测信息中包含的C文件编号宏定义
*****************************************************************************/
/*lint -e(767)*/
#define THIS_FILE_ID                    OM_FILE_ID_VOICE_LOG_C

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

MSG_CODEC_ERRLOG_STRU                     g_stVoiceErrLogMsg;                       /* 上报 OM 的 ErrorLog 消息结构 */
VOICE_ERRLOG_STATUS_STRU                  g_stVoiceErrLogStatus;                    /* ErrorLog 状态 */

/* NV项参数 */
CODEC_ERRLOG_CONFIG_STRU                  g_stVoiceErrLogPara;

/* 事件约束条件 */
VOS_UINT16 g_auhwVoiceErrLogEvntRules[][sizeof(VOICE_ERRLOG_EVENT_RULE_STRU)/sizeof(VOS_UINT16)] =
{
    /* 必须严格按照事件定义的枚举顺序定义本数组的元素 */

    /*          统计事件类型         |         事件所处模式          |           事件默认门限          */

    /* CODEC_ERRLOG_EVENTS_DMA_MIC_RESET */
    { CODEC_ERRLOG_EVENT_TYPE_TOTAL,  CODEC_ERRLOG_EVENT_MODE_ALL,    VOICE_ERRLOG_EVENT_DEFAULT_THD },

    /* CODEC_ERRLOG_EVENTS_DMA_SPK_RESET */
    { CODEC_ERRLOG_EVENT_TYPE_TOTAL,  CODEC_ERRLOG_EVENT_MODE_ALL,    VOICE_ERRLOG_EVENT_DEFAULT_THD },

    /* CODEC_ERRLOG_EVENTS_WCDMA_DEC_INT_LOST */
    { CODEC_ERRLOG_EVENT_TYPE_LOST,   CODEC_ERRLOG_EVENT_MODE_WCDMA,  VOICE_ERRLOG_EVENT_DEFAULT_THD },

    /* CODEC_ERRLOG_EVENTS_WCDMA_BBP_DATA_NONE */
    { CODEC_ERRLOG_EVENT_TYPE_REPEAT, CODEC_ERRLOG_EVENT_MODE_WCDMA,  VOICE_ERRLOG_EVENT_DEFAULT_THD },

    /* CODEC_ERRLOG_EVENTS_WCDMA_BBP_DATA_BAD */
    { CODEC_ERRLOG_EVENT_TYPE_TOTAL,  CODEC_ERRLOG_EVENT_MODE_WCDMA,  VOICE_ERRLOG_EVENT_DEFAULT_THD },

    /* CODEC_ERRLOG_EVENTS_WCDMA_DECIPHER_LOST */
    { CODEC_ERRLOG_EVENT_TYPE_TOTAL,  CODEC_ERRLOG_EVENT_MODE_WCDMA,  VOICE_ERRLOG_EVENT_DEFAULT_THD },

    /* CODEC_ERRLOG_EVENTS_GSM_BFI_FRAME */
    { CODEC_ERRLOG_EVENT_TYPE_REPEAT, CODEC_ERRLOG_EVENT_MODE_GSM,  ( 2 * VOICE_ERRLOG_EVENT_DEFAULT_THD ) },

    /* CODEC_ERRLOG_EVENTS_CODEC_SID_FRAME */
    { CODEC_ERRLOG_EVENT_TYPE_REPEAT, CODEC_ERRLOG_EVENT_MODE_ALL,    VOICE_ERRLOG_EVENT_DEFAULT_THD },

    /* CODEC_ERRLOG_EVENTS_PCVOICE_INTERPOLATE */
    { CODEC_ERRLOG_EVENT_TYPE_TOTAL,  CODEC_ERRLOG_EVENT_MODE_ALL,    VOICE_ERRLOG_EVENT_DEFAULT_THD },

    /* CODEC_ERRLOG_EVENTS_PCVOICE_RINGBUF_FULL */
    { CODEC_ERRLOG_EVENT_TYPE_TOTAL,  CODEC_ERRLOG_EVENT_MODE_ALL,    VOICE_ERRLOG_EVENT_DEFAULT_THD },
};



/*****************************************************************************
  3 函数实现
*****************************************************************************/


VOS_VOID VOICE_ErrLogResetEventCnts( VOICE_ERRLOG_EVENT_INFO_STRU *pstEvent )
{
    pstEvent->uhwCnt     =  0;
    pstEvent->shwLastCyc =  VOICE_ERRLOG_LAST_CYCLE_INI_VAL;
}


VOS_UINT16 VOICE_ErrLogGetEventThd( CODEC_ERRLOG_EVENTS_ENUM_UINT16 enEvent )
{
    VOS_UINT16                              uhwThd;
    VOS_UINT16                             *puhwNvTab;
    VOICE_ERRLOG_EVENT_RULE_STRU             *pstEvntsRule;

    /* 防止数组越界 */
    if( enEvent >= ( (sizeof( CODEC_ERRLOG_CONFIG_STRU ) / sizeof( VOS_UINT16 ) ) - VOICE_ERRLOG_CONFIG_HEAD_LEN ) )
    {
        /* 进行消息打点 */
        OM_LogError(VOICE_ErrLogEventNumBeyondNvItems);

        return VOICE_ERRLOG_EVENT_DEFAULT_THD;
    }

    puhwNvTab       = (VOS_UINT16*)&g_stVoiceErrLogPara;
    pstEvntsRule    = ( VOICE_ERRLOG_EVENT_RULE_STRU* )g_auhwVoiceErrLogEvntRules[enEvent];

    /* 获取对应位置的Nv项 */
    uhwThd = puhwNvTab[enEvent + VOICE_ERRLOG_CONFIG_HEAD_LEN];

    /* 检查Nv项数值合法性 */
    if( uhwThd > VOICE_ERRLOG_EVENT_MAXIMUM_THD )
    {
        /* 进行消息打点 */
        OM_LogError(VOICE_ErrLogInvalidNvItemValue);

        uhwThd = pstEvntsRule->uhwThd;
    }

    return uhwThd;
}
VOS_VOID VOICE_ErrLogInit( CODEC_ERRLOG_CONFIG_STRU *pstErrlogCfg )
{
    VOS_UINT16                          uhwEvent;
    VOICE_ERRLOG_EVENT_INFO_STRU         *pstEvntsInfo;
    VOICE_ERRLOG_EVENT_RULE_STRU         *pstEvntsRule;

    UCOM_MemCpy(&g_stVoiceErrLogPara,
                pstErrlogCfg,
                sizeof(CODEC_ERRLOG_CONFIG_STRU));

    /* ErrorLog运行状态结构体初始化 */
    g_stVoiceErrLogStatus.uhwEnable = g_stVoiceErrLogPara.uhwEnable;
    pstEvntsInfo = g_stVoiceErrLogStatus.astEventsInfo;

    /* 判断Nv项是否损坏 */
    if (g_stVoiceErrLogStatus.uhwEnable >= CODEC_SWITCH_BUTT)
    {
        /* 进行消息打点 */
        OM_LogError(VOICE_ErrLogNvItemsBroken);

        g_stVoiceErrLogStatus.uhwEnable = CODEC_SWITCH_OFF;
    }

    /* 若ErrorLog未使能,则退出 */
    if (CODEC_SWITCH_OFF == g_stVoiceErrLogStatus.uhwEnable)
    {
        /* 进行消息打点 */
        OM_LogError(VOICE_ErrLogNvDisableErrLog);

        return;
    }

    /* 初始化事件计数 */
    for (uhwEvent = 0; uhwEvent < CODEC_ERRLOG_EVENTS_ENUM_BUTT; uhwEvent++)
    {
        /* 初始化错误值计数 */
        VOICE_ErrLogResetEventCnts( &pstEvntsInfo[uhwEvent] );

        /* 初始化事件约束 */
        pstEvntsRule = ( VOICE_ERRLOG_EVENT_RULE_STRU* )g_auhwVoiceErrLogEvntRules[uhwEvent];

        pstEvntsInfo[uhwEvent].stRules.enMode = pstEvntsRule->enMode;
        pstEvntsInfo[uhwEvent].stRules.enType = pstEvntsRule->enType;

        /* 初始化门限数值 */
        pstEvntsInfo[uhwEvent].stRules.uhwThd = VOICE_ErrLogGetEventThd( uhwEvent );

        /* 初始化上报计数 */
        pstEvntsInfo[uhwEvent].uhwRprtd = 0;
    }

    /* 重置计数器 */
    g_stVoiceErrLogStatus.shwCycleCnt = 0;

    return;
}
VOS_VOID VOICE_ErrLogRecord( CODEC_ERRLOG_EVENTS_ENUM_UINT16 enEvent )
{
    /* 事件错误记录结构数组 */
    VOICE_ERRLOG_EVENT_INFO_STRU       *pstEvntsInfo;
    VOICE_MC_STATE_ENUM_UINT16          enState;

    /* 获取前台语音状态 */
    enState = VOICE_McGetForeGroundVoiceState();

    pstEvntsInfo = g_stVoiceErrLogStatus.astEventsInfo;

    /* 事件标号不合理,ErrorLog功能未启用,语音软件非RUNNING态 */
    if ((VOICE_MC_STATE_RUNNING != enState)
        || (CODEC_SWITCH_OFF == g_stVoiceErrLogStatus.uhwEnable)
        || (enEvent >= CODEC_ERRLOG_EVENTS_ENUM_BUTT))
    {
        return;
    }

    /* 根据事件类型进行错误记录 */
    switch(pstEvntsInfo[enEvent].stRules.enType)
    {
        /* 对于总数统计以及丢失事件统计的事件 */
        case CODEC_ERRLOG_EVENT_TYPE_TOTAL:
        case CODEC_ERRLOG_EVENT_TYPE_LOST:
        {
            /* 记录总数 */
            pstEvntsInfo[enEvent].uhwCnt++;

            /* 记录错误发生时刻 */
            pstEvntsInfo[enEvent].shwLastCyc = g_stVoiceErrLogStatus.shwCycleCnt;
        }
        break;

        /* 对于连续事件的统计 */
        case CODEC_ERRLOG_EVENT_TYPE_REPEAT:
        {
            /* 记录重复次数 */
            if (1 == (g_stVoiceErrLogStatus.shwCycleCnt - pstEvntsInfo[enEvent].shwLastCyc))
            {
                pstEvntsInfo[enEvent].uhwCnt++;

                /* 记录错误发生时刻 */
                pstEvntsInfo[enEvent].shwLastCyc = g_stVoiceErrLogStatus.shwCycleCnt;
            }
            else
            {
                /* 当连续事件计数小于门限时,非连续事件进行清零 */
                if (pstEvntsInfo[enEvent].uhwCnt < pstEvntsInfo[enEvent].stRules.uhwThd)
                {
                    pstEvntsInfo[enEvent].uhwCnt = 0;

                    /* 记录错误发生时刻 */
                    pstEvntsInfo[enEvent].shwLastCyc = g_stVoiceErrLogStatus.shwCycleCnt;
                }
            }
        }
        break;

        default:
        {
            /* 默认记录总数 */
            pstEvntsInfo[enEvent].uhwCnt++;

            /* 记录错误发生时刻 */
            pstEvntsInfo[enEvent].shwLastCyc = g_stVoiceErrLogStatus.shwCycleCnt;
        }
        break;
    }

}


VOS_VOID  VOICE_ErrLogUpdateEnv( VOS_VOID )
{
    CODEC_ERRLOG_ENV_STRU              *pstErrEnv;          /* VOICE 组件运行环境 */
    VOS_UINT16                          uhwCodecType;       /* 声码器类型 */
    VOS_UINT16                          uhwAmrMode;         /* AMR速率模式 */
    VOS_UINT16                          uhwNetMode;         /* 网络模式 */
    VOS_UINT16                          uhwActiveModemNo = VOICE_McGetForeGroundNum();
    VOICE_PROC_CODEC_STRU              *pstCodec         = VOICE_ProcGetCodecPtr();

    pstErrEnv       = &g_stVoiceErrLogMsg.stEventRpt.stErrEnv;
    uhwCodecType    = pstCodec->enCodecType;
    uhwAmrMode      = pstCodec->stEncInObj.enAmrMode;

    /* 当前若无前台，无需更新环境状态 */
    if (VOICE_MC_MODEM_NUM_BUTT <= uhwActiveModemNo)
    {
        return ;
    }
    else
    {
        uhwNetMode = VOICE_McGetForeGroundNetMode(uhwActiveModemNo);
    }

    /* 更新声码器状态  */
    switch(uhwCodecType)
    {
        /* AMR */
        case CODEC_AMR:
        {
            if (   (CODEC_NET_MODE_W == uhwNetMode)
                || (CODEC_NET_MODE_TD == uhwNetMode) )
            {
                pstErrEnv->enCodecEnv = CODEC_ERRLOG_ENV_CODEC_WCDMA_AMR_475
                    + (CODEC_ERRLOG_ENV_CODEC_ENUM_UINT16)uhwAmrMode;
            }
            else if (CODEC_NET_MODE_G == uhwNetMode)
            {
                pstErrEnv->enCodecEnv = CODEC_ERRLOG_ENV_CODEC_GSM_AMR_475
                    + (CODEC_ERRLOG_ENV_CODEC_ENUM_UINT16)uhwAmrMode;
            }
            else
            {
                pstErrEnv->enCodecEnv = CODEC_ERRLOG_ENV_CODEC_ENUM_BUTT;
            }
        }
        break;

        /* AMR2 */
        case CODEC_AMR2:
        {
            pstErrEnv->enCodecEnv = CODEC_ERRLOG_ENV_CODEC_WCDMA_AMR2_475
                    + (CODEC_ERRLOG_ENV_CODEC_ENUM_UINT16)uhwAmrMode;
        }
        break;

        /* AMRWB */
        case CODEC_AMRWB:
        {
            if (CODEC_NET_MODE_W == uhwNetMode)
            {
                pstErrEnv->enCodecEnv = CODEC_ERRLOG_ENV_CODEC_WCDMA_AMRWB_660
                    + (CODEC_ERRLOG_ENV_CODEC_ENUM_UINT16)uhwAmrMode;
            }
            else if (CODEC_NET_MODE_G == uhwNetMode)
            {
                pstErrEnv->enCodecEnv = CODEC_ERRLOG_ENV_CODEC_GSM_AMRWB_660
                    + (CODEC_ERRLOG_ENV_CODEC_ENUM_UINT16)uhwAmrMode;
            }
            else
            {
                pstErrEnv->enCodecEnv = CODEC_ERRLOG_ENV_CODEC_ENUM_BUTT;
            }
        }
        break;

        /* EFR, FR, HR */
        case CODEC_EFR:
        case CODEC_FR:
        case CODEC_HR:
        {
            pstErrEnv->enCodecEnv = CODEC_ERRLOG_ENV_CODEC_GSM_EFR
                                    + ( uhwCodecType - CODEC_EFR );
        }
        break;

        default:
        {
            pstErrEnv->enCodecEnv = CODEC_ERRLOG_ENV_CODEC_ENUM_BUTT;
        }
        break;

    }

    /* 更新输入输出配置 */

    pstErrEnv->stInout.uhwTxIn  = g_stVoiceMcObjs.stInout.usTxIn;
    pstErrEnv->stInout.uhwRxOut = g_stVoiceMcObjs.stInout.usRxOut;
}
VOS_VOID  VOICE_ErrLogPackLogHead(
                MNTN_HEAD_INFO_STRU *pstMsgMntnHead,
                CODEC_ERRLOG_EVENTS_ENUM_UINT16 enEvent )
{
    /* 记录错误号 */
    pstMsgMntnHead->ulErrNo = enEvent + VOICE_ERRLOG_ERRNO_BASE;

    /* 写入消息长度, 结果*2以转换成byte单位(ZSP特性) */
    pstMsgMntnHead->ulInfoLen = sizeof( MSG_CODEC_ERRLOG_STRU )
                                 - sizeof( MSG_CODEC_OM_ERRLOG_HEAD_STRU);

    /* 由ARM侧进行填充 */
    pstMsgMntnHead->ulSliceTime = 0;
    pstMsgMntnHead->ulBoardtime = 0;
}
VOS_VOID  VOICE_ErrLogReport( CODEC_ERRLOG_EVENTS_ENUM_UINT16 enEvent )
{
    VOICE_ERRLOG_EVENT_INFO_STRU         *pstEvntsInfo;
    MNTN_HEAD_INFO_STRU                *pstHeadInfo;
    MSG_CODEC_ERRLOG_STRU                *pstErrorLog;

    pstHeadInfo  = (MNTN_HEAD_INFO_STRU *)VOICE_ERRLOG_GetRecordHeadPtr();
    pstEvntsInfo = (VOICE_ERRLOG_EVENT_INFO_STRU *)VOICE_ERRLOG_GetEventsInfoPtr();
    pstErrorLog  = (MSG_CODEC_ERRLOG_STRU *)VOICE_ERRLOG_GetErrLogMsgPtr();

    UCOM_MemSet(pstErrorLog, 0, sizeof(MSG_CODEC_ERRLOG_STRU));

    /* 更新环境状态  */
    VOICE_ErrLogUpdateEnv();

    /* 记录错误事件发生次数 */
    switch (pstEvntsInfo[enEvent].stRules.enType)
    {
        /* 对于总数统计以及连续事件统计的事件 */
        case CODEC_ERRLOG_EVENT_TYPE_TOTAL:
        case CODEC_ERRLOG_EVENT_TYPE_REPEAT:
        {
            /* 记录总数 */
            pstErrorLog->stEventRpt.uhwErrCnt = pstEvntsInfo[enEvent].uhwCnt;
        }
        break;

        /* 对于丢失数目进行统计的事件 */
        case CODEC_ERRLOG_EVENT_TYPE_LOST:
        {
            /* 记录丢失总数 */
            pstErrorLog->stEventRpt.uhwErrCnt =  VOICE_ERRLOG_CHECK_PERIOD
                                                     - pstEvntsInfo[enEvent].uhwCnt;
        }
        break;

        default:
        {
            /* 默认记录总数 */
            pstErrorLog->stEventRpt.uhwErrCnt = pstEvntsInfo[enEvent].uhwCnt;
        }
        break;
    }

    /* 记录错误事件类型 */
    pstErrorLog->stEventRpt.enErrType = pstEvntsInfo[enEvent].stRules.enType;

    /* 写 OM 消息头 */
    pstErrorLog->stOmHead.usTransPrimId = MSG_VOICE_OM_ERRLOG_PID;

    /*  写ErrorLog 头 */
    VOICE_ErrLogPackLogHead(pstHeadInfo, enEvent);

    /* 上报 ErrorLog 信息 */
    UCOM_SendOsaMsg(DSP_PID_VOICE,
                         UCOM_PID_PS_OM,
                         (VOS_VOID *)pstErrorLog,
                         sizeof(MSG_CODEC_ERRLOG_STRU));
}
VOS_UINT32 VOICE_ErrLogCheckEvent( CODEC_ERRLOG_EVENTS_ENUM_UINT16 enEvent )
{
    VOICE_ERRLOG_EVENT_INFO_STRU         *pstEvntsInfo;
    VOS_UINT32                            uwResult;
    VOS_UINT16                            uhwNetMode;
    VOS_UINT16                            uhwActiveModemNo = VOICE_McGetForeGroundNum();

    pstEvntsInfo    = g_stVoiceErrLogStatus.astEventsInfo;
    uwResult        = CODEC_SWITCH_OFF;

    /* 当前若无前台,无需上报 */
    if (VOICE_MC_MODEM_NUM_BUTT <= uhwActiveModemNo)
    {
        return uwResult;
    }
    else
    {
        uhwNetMode = VOICE_McGetForeGroundNetMode(uhwActiveModemNo);
    }

    /* 运行环境检查 */
    if (CODEC_ERRLOG_EVENT_MODE_ALL != pstEvntsInfo[enEvent].stRules.enMode)
    {
        if (uhwNetMode != pstEvntsInfo[enEvent].stRules.enMode)
        {
            return uwResult;
        }
    }

    /* 根据事件类型检查 */
    switch (pstEvntsInfo[enEvent].stRules.enType)
    {
        /* 对于总数统计以及连续事件统计的事件 */
        case CODEC_ERRLOG_EVENT_TYPE_TOTAL:
        case CODEC_ERRLOG_EVENT_TYPE_REPEAT:
        {
            /* 检查事件总数 */
            if (pstEvntsInfo[enEvent].uhwCnt >= pstEvntsInfo[enEvent].stRules.uhwThd)
            {
                uwResult = CODEC_SWITCH_ON;
            }
        }
        break;

        /* 对于丢失数目进行统计的事件 */
        case CODEC_ERRLOG_EVENT_TYPE_LOST:
        {
             /* 检查丢失数值 */
            if (pstEvntsInfo[enEvent].uhwCnt
                < (VOICE_ERRLOG_CHECK_PERIOD - pstEvntsInfo[enEvent].stRules.uhwThd))
            {
                uwResult = CODEC_SWITCH_ON;
            }
        }
        break;

        default:
        {
            /* 默认不上报 */
        }
        break;
    }

    return uwResult;
}


VOS_VOID VOICE_ErrLogCycle( VOS_INT16 shwPeriod )
{
    VOS_UINT16                          uhwEvent;
    VOICE_ERRLOG_EVENT_INFO_STRU       *pstEvntsInfo;
    VOICE_MC_STATE_ENUM_UINT16          enState;

    /* 获取前台语音状态 */
    enState = VOICE_McGetForeGroundVoiceState();

    pstEvntsInfo    = g_stVoiceErrLogStatus.astEventsInfo;

    /* 若语音软件非RUNNING态或者ErrorLog未使能 */
    if ((VOICE_MC_STATE_RUNNING != enState)
        || (CODEC_SWITCH_OFF == g_stVoiceErrLogStatus.uhwEnable))
    {
        return;
    }

    /* 计数递增 */
    g_stVoiceErrLogStatus.shwCycleCnt++;

    /* 检查是否运行满一周期 */
    if (0 == (g_stVoiceErrLogStatus.shwCycleCnt % shwPeriod))
    {
        for (uhwEvent = 0; uhwEvent < CODEC_ERRLOG_EVENTS_ENUM_BUTT; uhwEvent++)
        {
            /* 检查是否符合上报条件 */
            if (VOICE_ErrLogCheckEvent(uhwEvent) != CODEC_SWITCH_OFF)
            {
                /* 检查上报次数 */
                if (pstEvntsInfo[uhwEvent].uhwRprtd < VOICE_ERRLOG_EVENT_REPORT_MAX)
                {
                    /* 上报消息 */
                    VOICE_ErrLogReport(uhwEvent);

                    /* 进行消息打点 */
                    OM_LogInfo(VOICE_ErrLogReportEvent);

                    /* 上报次数递增 */
                    pstEvntsInfo[uhwEvent].uhwRprtd++;
                }
            }

            /* 将事件计数器清零 */
            VOICE_ErrLogResetEventCnts(&pstEvntsInfo[uhwEvent]);
        }

        /* 将计数器清零,防止溢出 */
        g_stVoiceErrLogStatus.shwCycleCnt = 0;

    }
}
VOS_VOID  VOICE_ErrLogResetStatus( VOS_VOID )
{
    VOS_UINT16                       uhwEvent;
    VOICE_ERRLOG_EVENT_INFO_STRU      *pstEvntsInfo;

    pstEvntsInfo    = g_stVoiceErrLogStatus.astEventsInfo;

    /* 未使能ErrorLog */
    if (CODEC_SWITCH_OFF == g_stVoiceErrLogStatus.uhwEnable)
    {
        return;
    }

    for (uhwEvent = 0; uhwEvent < CODEC_ERRLOG_EVENTS_ENUM_BUTT; uhwEvent++)
    {
        /* 将事件计数器清零 */
        VOICE_ErrLogResetEventCnts(&pstEvntsInfo[uhwEvent]);
    }

    /* 将计数器清零 */
    g_stVoiceErrLogStatus.shwCycleCnt = 0;

}

#endif /* _MED_ERRORLOG */



VOS_VOID VOICE_ErrLogCheckDecStatus( VOS_VOID *pstObj )
{
    CODEC_DEC_IN_PARA_STRU  *pstDecObj;
    VOS_UINT16               uhwNetMode;         /* 网络模式 */
    VOS_UINT16               uhwActiveModemNo = VOICE_McGetForeGroundNum();

    /* 当前若无前台，无需检查解码状态 */
    if (VOICE_MC_MODEM_NUM_BUTT <= uhwActiveModemNo)
    {
        return ;
    }
    else
    {
        uhwNetMode = VOICE_McGetForeGroundNetMode(uhwActiveModemNo);
    }

    pstDecObj = (CODEC_DEC_IN_PARA_STRU *)pstObj;

    /* BFI 帧 */
    if (CODEC_BFI_YES == pstDecObj->enBfiFlag)
    {
        /* ErrorLog 记录事件 */
        VOICE_ErrLogEventRec(CODEC_ERRLOG_EVENTS_GSM_BFI_FRAME);
    }

    /* GSM 中的 SID帧 */
    if ((CODEC_NET_MODE_G == uhwNetMode)
       && (pstDecObj->enSidFlag != CODEC_SID_SPEECH))
    {
        /* ErrorLog 记录事件 */
        VOICE_ErrLogEventRec(CODEC_ERRLOG_EVENTS_CODEC_SID_FRAME);
    }

}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

