

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "voice_diagnose.h"
#include "codec_op_lib.h"
#include "med_drv_timer_hifi.h"
#include "ucom_mem_dyn.h"
#include "voice_debug.h"
#include "ucom_nv.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
    可维可测信息中包含的C文件编号宏定义
*****************************************************************************/
/*lint -e(767)*/
#define THIS_FILE_ID                    OM_FILE_ID_VOICE_DIAGNOSE_C

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
VOICE_DIAG_FRAMENUM_STRU        g_stDiagFrameNum;
VOS_UINT32                      g_uwDiagDLSilenceNum;                            /* 下行连续检测出静音的次数 */
VOICE_DIAG_NV_STRU              g_stVoiceDiagCfg;
VOS_UINT16                      g_uhwVoiceDiagReportFalg[VOICE_DIAG_MODEM_NUM_BUTT][VOICE_ERR_CAUSE_BUTT];
VOICE_DIAG_CHANNEL_QUALITY_STRU g_stChannelQuality;                              /* 物理层上报的信道质量信息，用于语音故障检测 */

VOICE_DIAG_DMA_ISR_STRU         g_stDiagDmaIsr;
VOS_UINT32                      g_auwSuspendBeginTs[VOICE_DIAG_MODEM_NUM_BUTT]; /* 网络切换开始语音软件被暂停的时刻 */

NV_ID_ERR_LOG_CTRL_INFO_STRU    g_stSysErrLogCfg;
/*****************************************************************************
  3 函数实现
*****************************************************************************/


VOS_VOID VOICE_DiagInit(VOICE_DIAG_NV_STRU *pstVoiceDiagCfg)
{
    VOICE_DIAG_NV_STRU       *pstDiagCfg = VOICE_DiagGetCfgPtr();

    /* 读取系统ErrLog NV控制参数 */
    UCOM_NV_Read(en_NV_Item_ErrLogCtrlInfo,
                 &g_stSysErrLogCfg,
                 sizeof(g_stSysErrLogCfg));

    /* 读取语音监测模块的Nv参数 */
    UCOM_MemSet(pstDiagCfg, 0, sizeof(VOICE_DIAG_NV_STRU));
    UCOM_MemCpy(pstDiagCfg, pstVoiceDiagCfg, sizeof(VOICE_DIAG_NV_STRU));

    /* 清空帧统计信息 */
    VOICE_DiagReset();

    /* 将所有监测点是否上报消息的标志都置为0 */
    UCOM_MemSet(g_uhwVoiceDiagReportFalg,
                0,
                ((VOS_UINT32)VOICE_DIAG_MODEM_NUM_BUTT * VOICE_ERR_CAUSE_BUTT * sizeof(VOS_UINT16)));

    /* 清空DMA统计信息 */
    UCOM_MemSet(&g_stDiagDmaIsr, 0, sizeof(VOICE_DIAG_DMA_ISR_STRU));

    /* 将时间戳清空 */
    UCOM_MemSet(g_auwSuspendBeginTs, 0, VOICE_DIAG_MODEM_NUM_BUTT * sizeof(VOS_UINT32));

}


VOS_VOID VOICE_DiagGsmGoodFrmNum( VOICE_DIAG_RX_DATA_STRU *pstRxData )
{
    VOICE_DIAG_FRAMENUM_STRU *pstFrameNum = VOICE_DiagGetFrameNumPtr();
    CODEC_ENUM_UINT16         enCodecType;

    enCodecType = pstRxData->enCodecType;

    if((CODEC_FR   == enCodecType)
      ||(CODEC_EFR == enCodecType)
      ||(CODEC_HR  == enCodecType))
    {
        /* SID帧 */
        if(pstRxData->enSid != CODEC_SID_SPEECH)
        {
            if(CODEC_HR  == enCodecType)
            {
                pstFrameNum->uhwGoodFrameNum += VOICE_DIAG_HR_SID_NUM;
            }
            else
            {
                pstFrameNum->uhwGoodFrameNum += VOICE_DIAG_FR_EFR_SID_NUM;
            }
        }
        /* GOOD帧 */
        else if(CODEC_BFI_NO == pstRxData->enBfi)
        {
            pstFrameNum->uhwGoodFrameNum++;
        }
        else
        {
        }
    }
    else
    {
        if((CODEC_AMR_TYPE_RX_SID_FIRST == pstRxData->enAmrFrameType)
         ||(CODEC_AMR_TYPE_RX_SID_UPDATE == pstRxData->enAmrFrameType)
         ||(CODEC_AMR_TYPE_RX_SID_BAD == pstRxData->enAmrFrameType))
        {
            pstFrameNum->uhwGoodFrameNum += VOICE_DIAG_AMR_SID_NUM;
        }
        else if(CODEC_AMR_TYPE_RX_SPEECH_GOOD == pstRxData->enAmrFrameType)
        {
            pstFrameNum->uhwGoodFrameNum++;
        }
        else
        {
        }
    }
}


VOS_VOID VOICE_DiagCalcFrameNum(VOICE_DIAG_RX_DATA_STRU *pstRxData)
{
    VOICE_DIAG_FRAMENUM_STRU *pstFrameNum = VOICE_DiagGetFrameNumPtr();

    switch(pstRxData->enNetMode)
    {
        case CODEC_NET_MODE_G:
        {
            VOICE_DiagGsmGoodFrmNum(pstRxData);

            break;
        }
        case CODEC_NET_MODE_W:
        case CODEC_NET_MODE_TD:
        case CODEC_NET_MODE_L:
        {
            if(CODEC_AMR_FQI_QUALITY_GOOD == pstRxData->enQualityIdx)
            {
                pstFrameNum->uhwGoodFrameNum++;
            }

            break;
        }
        default:
        {
            OM_LogError(VOICE_DiagnoseGoodFrameNum_UnkownMode);
        }

    }

    pstFrameNum->uhwTotalFrameNum += 1;

}


VOS_VOID VOICE_DiagReset( VOS_VOID )
{
    VOICE_DIAG_FRAMENUM_STRU *pstFrameNum = VOICE_DiagGetFrameNumPtr();

    pstFrameNum->uhwGoodFrameNum = 0;
    pstFrameNum->uhwTotalFrameNum = 0;

    /* 将下行连续检测出静音的次数置为0 */
    VOICE_DiagSetDLSilenceNum(0);

    /* 将保存的信息质量信息清空 */
    UCOM_MemSet(VOICE_DiagGetChannelQualityPtr(),
                0,
                sizeof(VOICE_DIAG_CHANNEL_QUALITY_STRU));

}


VOS_VOID VOICE_DiagLineIn(
                VOICE_DIAG_MODEM_NUM_ENUM_UINT16 enActiveModemNo,
                VOICE_DIAG_RX_DATA_STRU         *pstRxData)
{
    VOICE_DIAG_FRAMENUM_STRU               *pstFrameNum   = VOICE_DiagGetFrameNumPtr();
    VOICE_DIAG_NV_STRU                     *pstDiagCfg    = VOICE_DiagGetCfgPtr();
    VOICE_DIAG_CHANNEL_QUALITY_STRU        *pstDiagChanQa = VOICE_DiagGetChannelQualityPtr();
    VOS_INT32                               shwDiagEnable;
    VOS_INT16                               shwDiagTime;                        /* 统计时间，单位(s) */
    VOS_INT16                               shwGoodFrmPercentThd;               /* Q15 (0.8*32767) */
    HIFI_ERROR_EVENT_ONE_WAY_NO_SOUND_STRU  stOneWayNoSound;

    UCOM_MemSet(&stOneWayNoSound, 0, sizeof(stOneWayNoSound));

    /* 获取NV参数 */
    shwDiagEnable        = pstDiagCfg->shwEnable;
    shwDiagTime          = pstDiagCfg->stLineInPara.shwDiagTime;
    shwGoodFrmPercentThd = pstDiagCfg->stLineInPara.shwGoodFrmPercentThd;

    /* LineIn处语音故障检测不使能，直接返回 */
    if(!shwDiagEnable)
    {
        return;
    }

    /* 考虑当前网络信号质量的影响 */
    if(0 == pstDiagCfg->stChanQaPara.shwIsIgnoreChanQa)
    {
        /* 当前信道质量很差，不检测，直接返回 */
        if(pstDiagChanQa->uhwIsBadCell)
        {
            return;
        }
    }

    /* 统计Good帧 */
    VOICE_DiagCalcFrameNum(pstRxData);

    /* 若统计时间不够检测条件，直接返回 */
    if(pstFrameNum->uhwTotalFrameNum < (shwDiagTime * VOICE_DIAG_FRAME_NUM_PER_SECOND))
    {
        return;
    }

    /* GOOD帧的比重低于门限 */
    if(pstFrameNum->uhwGoodFrameNum < CODEC_OpMult((VOS_INT16)pstFrameNum->uhwTotalFrameNum,
                                                    shwGoodFrmPercentThd))
    {
        VOICE_DiagAddDLSilenceNum();
    }
    else
    {
        VOICE_DiagSetDLSilenceNum(0);
    }

    /* 将所有帧计数值都置为0 */
    UCOM_MemSet(VOICE_DiagGetFrameNumPtr(), 0, sizeof(VOICE_DIAG_FRAMENUM_STRU));

    /* 若连续3次都为silence，则上报检测结果 */
    if(VOICE_DiagGetDLSilenceNum() >= VOICE_DIAG_MAX_SILENCE_NUM)
    {
        /* 记录语音单通时的HIFI结构体 */
        stOneWayNoSound.uhwDevMode       = pstRxData->enDevMode;
        stOneWayNoSound.uhwActiveModem   = enActiveModemNo;
        stOneWayNoSound.uhwNetMode       = pstRxData->enNetMode;
        stOneWayNoSound.uhwIsBadCell     = pstDiagChanQa->uhwIsBadCell;
        stOneWayNoSound.uhwCodecType     = pstRxData->enCodecType;
        stOneWayNoSound.uhwEncoderInited = pstRxData->usIsEncInited;
        stOneWayNoSound.uhwDecoderInited = pstRxData->usIsDecInited;
        stOneWayNoSound.uhwCheckPoint    = VOICE_DIAG_REPORT_LINEIN;
        UCOM_MemCpy(stOneWayNoSound.uwData,
                    pstDiagChanQa->auwChannelQuality,
                    VOICE_DIAG_CHANNEL_QUALITY_LEN * sizeof(VOS_UINT32));

        /* 上报检测结果为单通 */
        VOICE_DiagMsgReport(VOICE_DIAG_REPORT_LINEIN,
                            enActiveModemNo,
                            &stOneWayNoSound);
    }

}
VOS_VOID VOICE_DiagMsgReport(
                VOICE_DIAG_ERR_CAUSE_ENUM_UINT16         uhwErrCause,
                VOS_UINT16                               uhwModemNo,
                HIFI_ERROR_EVENT_ONE_WAY_NO_SOUND_STRU  *pstOneWayNoSound)

{
    VOS_UINT16                    *puhwDiagReportFlag = VOICE_DiagGetReportFlag(uhwModemNo);
    OM_ERR_LOG_REPORT_CNF_STRU    *pstOmErrLogCnf;
    OM_ERR_LOG_DIAG_STRU          *pstDiagErrLog;
    VOS_UINT32                     uwDiagErrLogLen;
    VOS_UINT32                     uwOmErrLogCnfSize;

    /* 系统的ErrLog功能关闭 */
    if(!g_stSysErrLogCfg.ucAlmStatus)
    {
        return;
    }

    /* 语音主动告警不使能 */
    if(!(g_stSysErrLogCfg.aucReportBitMap[0] & ERR_LOG_ALARM_REPORT_ENABLE)
       || (!(g_stSysErrLogCfg.aucReportBitMap[1] & ERR_LOG_VOICE_ALARM_ENABLE)))
    {
        return;
    }

    /* 该监测点没有上报过单通消息 */
    if(!puhwDiagReportFlag[uhwErrCause])
    {
        /* 为故障上报数据结构申请内存 */
        uwDiagErrLogLen = sizeof(OM_ERR_LOG_DIAG_STRU) + sizeof(HIFI_ERROR_EVENT_ONE_WAY_NO_SOUND_STRU);
        pstDiagErrLog = (OM_ERR_LOG_DIAG_STRU*)UCOM_MemAlloc(uwDiagErrLogLen);

        /* 为故障上报消息申请内存 */
        uwOmErrLogCnfSize = uwDiagErrLogLen + sizeof(OM_ERR_LOG_REPORT_CNF_STRU);
        pstOmErrLogCnf = (OM_ERR_LOG_REPORT_CNF_STRU*)UCOM_MemAlloc(uwOmErrLogCnfSize);


        /* 填充故障上报数据结构 */
        pstDiagErrLog->stOmErrLogHeader.ulMsgModuleId = OM_ERR_LOG_MOUDLE_ID_HIFI;
        pstDiagErrLog->stOmErrLogHeader.usModemId = uhwModemNo;
        pstDiagErrLog->stOmErrLogHeader.usAlmId = HIFI_ERR_LOG_ONE_WAY_NO_SOUND;
        pstDiagErrLog->stOmErrLogHeader.usAlmLevel = HIFI_ERR_LOG_ALARM_CRITICAL;
        pstDiagErrLog->stOmErrLogHeader.usAlmType = HIFI_ERR_LOG_ALARM_COMMUNICATION;
        pstDiagErrLog->stOmErrLogHeader.usAlmLowSlice = DRV_TIMER_ReadSysTimeStamp();
        pstDiagErrLog->stOmErrLogHeader.ulAlmLength = sizeof(HIFI_ERROR_EVENT_ONE_WAY_NO_SOUND_STRU);
        UCOM_MemCpy(pstDiagErrLog->auhwContent,
                    pstOneWayNoSound,
                    sizeof(HIFI_ERROR_EVENT_ONE_WAY_NO_SOUND_STRU));

        /* 故障上报消息 */
        pstOmErrLogCnf->ulMsgName   = ID_OM_FAULT_ERR_LOG_IND;
        pstOmErrLogCnf->ulMsgType   = OM_ERR_LOG_MSG_FAULT_REPORT;
        pstOmErrLogCnf->ulRptlen    = uwDiagErrLogLen;
        UCOM_MemCpy(pstOmErrLogCnf->aucContent, pstDiagErrLog, uwDiagErrLogLen);

    #if(FEATURE_ON == FEATURE_PTM)
        /* 给OM发送ID_OM_AUDIO_ERR_LOG_IND消息 */
        UCOM_SendOsaMsg(DSP_PID_VOICE,
                        ACPU_PID_OM,
                        pstOmErrLogCnf,
                        uwOmErrLogCnfSize);
    #endif

        /* 上报单通信息 */
        OM_LogError2(VOICE_DiagnoseMsgReport_OneWayNoSound,
                     uhwErrCause,
                     uhwModemNo);

        puhwDiagReportFlag[uhwErrCause] = VOICE_DIAG_TRUE;

        /* 释放内存 */
        UCOM_MemFree(pstDiagErrLog);
        UCOM_MemFree(pstOmErrLogCnf);
    }

}



VOS_VOID VOICE_DiagAlarmReport(
                VOS_UINT16                               uhwModemNo,
                VOICE_DIAG_ERR_CAUSE_ENUM_UINT16         uhwAlarmCause,
                VOS_VOID                                *pstReportData,
                VOS_UINT32                               uwDataSize,
                HIFI_ERR_LOG_ALARM_ID_ENUM_UINT16        uhwAlarmId)

{
    OM_ERR_LOG_REPORT_CNF_STRU    *pstOmErrLogCnf;
    OM_ERR_LOG_DIAG_STRU          *pstDiagErrLog;
    VOS_UINT16                    *puhwDiagReportFlag = VOICE_DiagGetReportFlag(uhwModemNo);
    VOS_UINT32                     uwDiagErrLogSize;
    VOS_UINT32                     uwOmErrLogCnfSize;
    VOICE_DIAG_NV_STRU            *pstDiagCfg = VOICE_DiagGetCfgPtr();

    /* 系统的ErrLog功能关闭 */
    if(!g_stSysErrLogCfg.ucAlmStatus)
    {
        OM_LogInfo1(VOICE_DiagnoseMsgReport_VoiceAlarm, 1);
        return;
    }

    /* 语音主动告警不使能 */
    if(!(g_stSysErrLogCfg.aucReportBitMap[0] & ERR_LOG_ALARM_REPORT_ENABLE)
       || (!(g_stSysErrLogCfg.aucReportBitMap[1] & ERR_LOG_VOICE_ALARM_ENABLE)))
    {
        OM_LogInfo1(VOICE_DiagnoseMsgReport_VoiceAlarm, 2);
        return;
    }

    /* 本组件告警上报功能不使能 */
    if(!pstDiagCfg->shwEnable)
    {
        OM_LogInfo1(VOICE_DiagnoseMsgReport_VoiceAlarm, 3);
        return;
    }

    /* 本通电话内已经上报过同类告警 */
    if(puhwDiagReportFlag[uhwAlarmCause])
    {
        return;
    }

    /* 根据告警原因填充对应的上报内容 */
    uwDiagErrLogSize = sizeof(OM_ERR_LOG_DIAG_STRU) + uwDataSize;

    /* 为故障上报数据结构申请内存 */
    pstDiagErrLog = (OM_ERR_LOG_DIAG_STRU*)UCOM_MemAlloc(uwDiagErrLogSize);

    /* 填充告警上报数据头内容 */
    pstDiagErrLog->stOmErrLogHeader.ulMsgModuleId = OM_ERR_LOG_MOUDLE_ID_HIFI;
    pstDiagErrLog->stOmErrLogHeader.usModemId = uhwModemNo;
    pstDiagErrLog->stOmErrLogHeader.usAlmId = uhwAlarmId;
    pstDiagErrLog->stOmErrLogHeader.usAlmLevel = HIFI_ERR_LOG_ALARM_MAJOR;
    pstDiagErrLog->stOmErrLogHeader.usAlmType = HIFI_ERR_LOG_ALARM_COMMUNICATION;
    pstDiagErrLog->stOmErrLogHeader.usAlmLowSlice = DRV_TIMER_ReadSysTimeStamp();
    pstDiagErrLog->stOmErrLogHeader.ulAlmLength = uwDataSize;
    UCOM_MemCpy(pstDiagErrLog->auhwContent, pstReportData, uwDataSize);

    /* 为故障上报消息申请内存 */
    uwOmErrLogCnfSize = uwDiagErrLogSize + sizeof(OM_ERR_LOG_REPORT_CNF_STRU);
    pstOmErrLogCnf = (OM_ERR_LOG_REPORT_CNF_STRU*)UCOM_MemAlloc(uwOmErrLogCnfSize);

    /* 故障上报消息 */
    pstOmErrLogCnf->ulMsgName   = ID_OM_ALARM_ERR_LOG_IND;
    pstOmErrLogCnf->ulMsgType   = OM_ERR_LOG_MSG_ALARM_REPORT;
    pstOmErrLogCnf->ulRptlen    = uwDiagErrLogSize;
    UCOM_MemCpy(pstOmErrLogCnf->aucContent, pstDiagErrLog, uwDiagErrLogSize);

#if(FEATURE_ON == FEATURE_PTM)
    /* 给OM发送ID_OM_AUDIO_ERR_LOG_IND消息 */
    UCOM_SendOsaMsg(DSP_PID_VOICE,
                    ACPU_PID_OM,
                    pstOmErrLogCnf,
                    uwOmErrLogCnfSize);
#endif

    /* 上报单通信息 */
    OM_LogError1(VOICE_DiagnoseMsgReport_VoiceAlarm, uhwModemNo);

    puhwDiagReportFlag[uhwAlarmCause] = VOICE_DIAG_TRUE;

    /* 释放内存 */
    UCOM_MemFree(pstDiagErrLog);
    UCOM_MemFree(pstOmErrLogCnf);

}


VOS_VOID VOICE_DiagSuspend(
                VOS_UINT16              uhwModemNo,
                VOS_UINT16              uhwCurNetMode,
                VOS_UINT16              uhwPreNetMode)
{
    VOS_UINT32                           uwTimeStamp;
    VOS_UINT32                           uwSusPendTime;         /* 单位:0.1ms */
    HIFI_ERR_EVT_VOICE_SUSPEND_SLOW_STRU stSuspendSlowErr;
    VOICE_DIAG_NV_STRU                  *pstDiagCfg = VOICE_DiagGetCfgPtr();

    /* 读取SOC计数器，获取当前时戳 */
    uwTimeStamp = DRV_TIMER_ReadSysTimeStamp();

    /* 计算网络切换耗时:SetCodec时间戳 - Suspend时间戳 */
    uwSusPendTime = (UCOM_COMM_CycSub(uwTimeStamp, g_auwSuspendBeginTs[uhwModemNo], 0xffffffff)
        * VOICE_DIAG_TIMESTAMP_MS_RATIO) / DRV_TIMER_GetOmFreq();

    /* 网络切换耗时太长 */
    if ( uwSusPendTime >= (VOS_UINT16)pstDiagCfg->stLineInPara.ahwReserve[0])
    {
        stSuspendSlowErr.uhwErrorReason = VOICE_SUSPEND_SLOW;
        stSuspendSlowErr.uhwActiveModem = uhwModemNo;
        stSuspendSlowErr.uhwCurNetMode  = uhwCurNetMode;
        stSuspendSlowErr.uhwPreNetMode  = uhwPreNetMode;
        stSuspendSlowErr.uwSuspendTime  = uwSusPendTime;

        /* 上报告警 */
        VOICE_DiagAlarmReport(uhwModemNo,
                              VOICE_SUSPEND_SLOW,
                              &stSuspendSlowErr,
                              sizeof(stSuspendSlowErr),
                              HIFI_ERR_LOG_VOICE_SUSPEND_SLOW);
    }

    return;
}






#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

