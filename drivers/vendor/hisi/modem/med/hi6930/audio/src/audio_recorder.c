

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "audio_recorder.h"
#include "codec_com_codec.h"
#include "voice_proc.h"
#include "om_log.h"
#include "voice_mc.h"
#include "audio_debug.h"
#include "audio_enhance.h"
#include "xa_src_pp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
    可维可测信息中包含的C文件编号宏定义
*****************************************************************************/
/*lint -e(767)*/
#define THIS_FILE_ID                    OM_FILE_ID_AUDIO_RECORDER_C

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
AUDIO_RECORDER_OBJ_STRU                 g_stAudioRecorderObj;

/*****************************************************************************
  3 函数实现
*****************************************************************************/


VOS_UINT32  AUDIO_RECOEDER_Init(VOS_VOID)
{
    AUDIO_RECORDER_OBJ_STRU         *pstAudioRecorderObj = AUDIO_RecorderGetObjPtr();
    AUDIO_RECORDER_VOICE_STRU       *pstVoiceRecord      = &pstAudioRecorderObj->stVoiceRecord;

    /* 初始化 */
    UCOM_MemSet(pstAudioRecorderObj, 0, sizeof(AUDIO_RECORDER_OBJ_STRU));

    /* 显式标记录音类型为null */
    AUDIO_RecorderSetCaptureType(AUDIO_RECORD_TYPE_DISABLE);

    /* 显式初始化CS通话录音引用计数 */
    pstVoiceRecord->uwRefCnt = 0;

    /* 设置CS通话录音buff保护字 */
    pstAudioRecorderObj->uwProtectWord1 = UCOM_PROTECT_WORD1;
    pstAudioRecorderObj->uwProtectWord2 = UCOM_PROTECT_WORD2;
    pstAudioRecorderObj->uwProtectWord3 = UCOM_PROTECT_WORD3;
    pstAudioRecorderObj->uwProtectWord4 = UCOM_PROTECT_WORD4;

    pstVoiceRecord->stRecordBuff.uwProtectWord1 = UCOM_PROTECT_WORD1;
    pstVoiceRecord->stRecordBuff.uwProtectWord2 = UCOM_PROTECT_WORD2;
    pstVoiceRecord->stRecordBuff.uwProtectWord3 = UCOM_PROTECT_WORD3;
    pstVoiceRecord->stRecordBuff.uwProtectWord4 = UCOM_PROTECT_WORD4;
    pstVoiceRecord->stRecordBuff.uwProtectWord5 = UCOM_PROTECT_WORD4;

    return VOS_OK;
}


VOS_UINT32 AUDIO_RECORDER_MsgRecordStartCmd(VOS_VOID *pvOsaMsg)
{
    AUDIO_RECORDER_OBJ_STRU         *pstAudioRecorderObj    = AUDIO_RecorderGetObjPtr();
    AUDIO_RECORD_START_STRU         *pstRecStartMsg         = VOS_NULL ;
    VOS_UINT32                       uwRet                  = VOS_OK;

    /* 异常入参检查 */
    if (VOS_NULL == pvOsaMsg)
    {
        OM_LogError(AUDIO_RecorderBadPara);

        return VOS_ERR;
    }

    /* 按AUDIO_RECORD_START_STRU格式解析消息 */
    pstRecStartMsg = (AUDIO_RECORD_START_STRU *)((MsgBlock *) pvOsaMsg)->aucValue;

    switch(pstRecStartMsg->enRecordType)
    {
        case AUDIO_RECORD_TYPE_CALL:
        {
            uwRet += AUDIO_RECORDER_VoiceRcdStart(pstRecStartMsg);
        }
        break;
        case AUDIO_RECORD_TYPE_3RD_MODEM:
        {
            AUDIO_RecorderSetCaptureType(AUDIO_RECORD_TYPE_3RD_MODEM);

            /* 下行数据取自第三方Modem */
            uwRet += AUDIO_PCM_RegisterPcmCbFunc(AUDIO_PCM_MODE_CAPTURE,
                        AUDIO_PCM_OBJ_AP,
                        AUDIO_CAPTURE_MODE_EXT_CALL,
                        AUDIO_RECORDER_SecondVoiceCallBack);

            /* 引用计数加一 */
            pstAudioRecorderObj->st3RDVoiceRecord.uwRefCnt++;
        }
        break;
        case AUDIO_RECORD_TYPE_AUDIO:
        {
            AUDIO_RecorderSetCaptureType(AUDIO_RECORD_TYPE_AUDIO);

            /* 注册音频录音处理的回调函数 */
            uwRet += AUDIO_PCM_RegisterPcmCbFunc(AUDIO_PCM_MODE_CAPTURE,
                            AUDIO_PCM_OBJ_AP,
                            AUDIO_CAPTURE_MODE_AUDIO,
                            AUDIO_RECORDER_AudioCallBack);

            /* 引用计数加一 */
            pstAudioRecorderObj->stAudioRecord.uwRefCnt++;
        }
        break;
        case AUDIO_RECORD_TYPE_FM:
        {
            AUDIO_RecorderSetCaptureType(AUDIO_RECORD_TYPE_FM);

            /* 注册FM录音处理的回调函数 */
            uwRet += AUDIO_PCM_RegisterPcmCbFunc(AUDIO_PCM_MODE_CAPTURE,
                            AUDIO_PCM_OBJ_AP,
                            AUDIO_CAPTURE_MODE_FM,
                            AUDIO_RECORDER_FMCallBack);

            /* 引用计数加一 */
            pstAudioRecorderObj->stFMRecord.uwRefCnt++;
        }
        break;
        default:
        {
            AUDIO_RecorderSetCaptureType(AUDIO_RECORD_TYPE_DISABLE);

            OM_LogError1(AUDIO_RECORDER_MsgStartErr, pstRecStartMsg->enRecordType);

            return VOS_ERR;
        }
    }

    if(VOS_OK != uwRet)
    {
        AUDIO_RecorderSetCaptureType(AUDIO_RECORD_TYPE_DISABLE);

        OM_LogError1(AUDIO_RECORDER_MsgStartErr, uwRet);
    }

    return uwRet;

}


VOS_UINT32 AUDIO_RECORDER_MsgRecordStopCmd(VOS_VOID *pvOsaMsg)
{
    AUDIO_RECORDER_OBJ_STRU         *pstAudioRecorderObj = AUDIO_RecorderGetObjPtr();
    AUDIO_RECORD_STOP_STRU          *pstStopMsg;
    VOS_UINT32                       uwRet = VOS_OK;

    /* 异常入参检查 */
    if (VOS_NULL == pvOsaMsg)
    {
        OM_LogError(AUDIO_RecorderBadPara);

        return VOS_ERR;
    }

    /* 按AUDIO_RECORD_STOP_STRU格式解析消息 */
    pstStopMsg = (AUDIO_RECORD_STOP_STRU *)((MsgBlock *) pvOsaMsg)->aucValue;

    switch( pstStopMsg->enRecordType)
    {
        case AUDIO_RECORD_TYPE_CALL:
        {
            AUDIO_RECORDER_VoiceRcdStop();
        }
        break;
        case AUDIO_RECORD_TYPE_3RD_MODEM:
        {
            /* 预留接口，待扩充 */

            /* 引用计数加一 */
            pstAudioRecorderObj->st3RDVoiceRecord.uwRefCnt--;
        }
        break;
        case AUDIO_RECORD_TYPE_AUDIO:
        {
            /* 预留接口，待扩充 */

            /* 引用计数减一 */
            pstAudioRecorderObj->stAudioRecord.uwRefCnt--;
        }
        break;
        case AUDIO_RECORD_TYPE_FM:
        {
            /* 预留接口，待扩充 */

            /* 引用计数加一 */
            pstAudioRecorderObj->stFMRecord.uwRefCnt--;
        }
        break;
        default:
        {
            OM_LogError1(AUDIO_RECORDER_MsgStartErr, pstStopMsg->enRecordType);

            return VOS_ERR;
        }
    }

    return uwRet;
}


VOS_UINT32  AUDIO_RECORDER_VoiceRcdStart(AUDIO_RECORD_START_STRU *pstRecStartMsg)
{
    AUDIO_RECORDER_OBJ_STRU         *pstAudioRecorderObj    = AUDIO_RecorderGetObjPtr();
    VOS_UINT32                       uwRet = VOS_OK;

    /* 如果voice没有运行，则上报错误 */
    if ((VOS_OK != VOICE_McApiIsIdle()))
    {
        /* 标志通话录音开始，此后需将ap侧提供的录音buff填充为hifi语音处理之后的数据 */
        AUDIO_RecorderSetCaptureType(AUDIO_RECORD_TYPE_CALL);

        /* 设置目标采样率 */
        pstAudioRecorderObj->stVoiceRecord.uwTargetSmpRate = pstRecStartMsg->uhwTarSmpRate;

        /* 注册voice_mc通话录音处理函数 */
        VOICE_McRegisterCaptureCallBack(AUDIO_RECORDER_VoiceVcMcCB);

        /* 注册audio_recorder通话录音处理函数 */
        uwRet += AUDIO_PCM_RegisterPcmCbFunc(AUDIO_PCM_MODE_CAPTURE,
                                            AUDIO_PCM_OBJ_HIFI,
                                            AUDIO_CAPTURE_MODE_CALL,
                                            AUDIO_RECORDER_VoiceAudioPcmCB);

        /* 引用计数加一 */
        pstAudioRecorderObj->stVoiceRecord.uwRefCnt++;

        /* om消息 */
        OM_LogInfo3(AUDIO_RECORDER_RegisterVcCbOK,
                    pstAudioRecorderObj->stVoiceRecord.uwRefCnt,
                    AUDIO_RecorderGetCaptureType(),
                    uwRet);
    }
    else
    {
        /* om消息 */
        OM_LogError(AUDIO_RECORDER_StartVcCallWhileIdle);

        return VOS_ERR;
    }

    return uwRet;

}


VOS_UINT32  AUDIO_RECORDER_VoiceRcdStop(VOS_VOID)
{
    AUDIO_RECORDER_OBJ_STRU         *pstAudioRecorderObj    = AUDIO_RecorderGetObjPtr();
    AUDIO_RECORDER_VOICE_STRU       *pstVoiceRecord         = &pstAudioRecorderObj->stVoiceRecord;
    VOS_UINT32                       uwRet = VOS_OK;

    /* 去注册通话录音处理函数 */
    VOICE_McUnRegisterCaptureCallBack();

    uwRet += AUDIO_PCM_RegisterPcmCbFunc(AUDIO_PCM_MODE_CAPTURE,
                                         AUDIO_PCM_OBJ_HIFI,
                                         AUDIO_CAPTURE_MODE_CALL,
                                         VOS_NULL);

    /* 标记当前录音任务类型为disable */
    AUDIO_RecorderSetCaptureType(AUDIO_RECORD_TYPE_DISABLE);

    /* 引用计数减一 */
    pstVoiceRecord->uwRefCnt--;
    pstVoiceRecord->uwOrignalSmpRate = 0;
    pstVoiceRecord->uwTargetSmpRate  = 0;

    /* 将通话录音准备输出buff清空，防止引入噪声 */
    UCOM_MemSet(pstVoiceRecord->stRecordBuff.ashwTxDataOut,
                0,
                AUDIO_RECORDER_BUFF_SIZE_1920 * sizeof(VOS_INT16));

    UCOM_MemSet(pstVoiceRecord->stRecordBuff.ashwRxDataOut,
                0,
                AUDIO_RECORDER_BUFF_SIZE_1920 * sizeof(VOS_INT16));

    /* om消息 */
    OM_LogInfo1(AUDIO_RECORDER_UnregisterVcCbOK,
                pstAudioRecorderObj->stVoiceRecord.uwRefCnt);

    return uwRet;

}
VOS_UINT32  AUDIO_RECORDER_VoiceVcMcCB(
                VOS_INT16 *pshwTx,
                VOS_INT16 *pshwRx,
                VOS_UINT32 uwOrignalSmpRate)
{
    AUDIO_RECORDER_OBJ_STRU             *pstAudioRcdObj;
    AUDIO_RECORDER_VOICE_STRU           *pstVoiceRcdObj;
    XA_SRC_USER_CFG_STRU                 stVoiceSmpRate;
    VOS_UINT32                           uwBuffSize;
    VOS_UINT32                           uwRet = VOS_OK;

    /* 对外接口，异常入参检查 */
    if (  (VOS_NULL == pshwTx)
        ||(VOS_NULL == pshwRx)
        ||(AUDIO_RECORDER_SAMPLERATE_16K < uwOrignalSmpRate))
    {
        OM_LogError3(AUDIO_RecorderBadPara,
                     pshwTx,
                     pshwRx,
                     uwOrignalSmpRate);

        return VOS_ERR;
    }

    /* 获取录音对象 */
    pstAudioRcdObj = AUDIO_RecorderGetObjPtr();
    pstVoiceRcdObj = &pstAudioRcdObj->stVoiceRecord;

    /* 由采样率获取数据长度 */
    if (AUDIO_RECORDER_SAMPLERATE_8K == uwOrignalSmpRate)
    {
        uwBuffSize = AUDIO_RECORDER_BUFF_SIZE_320;
    }
    else if (AUDIO_RECORDER_SAMPLERATE_16K == uwOrignalSmpRate)
    {
        uwBuffSize = AUDIO_RECORDER_BUFF_SIZE_640;
    }
    else
    {
        OM_LogError1(AUDIO_RecorderBadPara, uwOrignalSmpRate);

        return VOS_ERR;
    }

    /* 获取对应buff数据 */
    UCOM_MemCpy(pstVoiceRcdObj->stRecordBuff.ashwTxDataIn,
                pshwTx,
                uwBuffSize);

    UCOM_MemCpy(pstVoiceRcdObj->stRecordBuff.ashwRxDataIn,
                pshwRx,
                uwBuffSize);

    /* 如果首次初始化或者语音宽窄带切换，则重新初始化变采样模块 */
    if (pstVoiceRcdObj->uwOrignalSmpRate != uwOrignalSmpRate)
    {
        OM_LogInfo2(AUDIO_RecorderSmpRateChange,
                    pstVoiceRcdObj->uwOrignalSmpRate,
                    uwOrignalSmpRate);

        stVoiceSmpRate.swChannels           = 1;
        stVoiceSmpRate.swInputChunkSize     = XA_SRC_INPUT_CHUNKSIZE_160;
        stVoiceSmpRate.swInputSmpRat        = (VOS_INT32)uwOrignalSmpRate;
        stVoiceSmpRate.swOutputSmpRat       = (VOS_INT32)pstVoiceRcdObj->uwTargetSmpRate;

        /* 初始化变采样模块 */
        uwRet += (VOS_UINT32)XA_SRC_Init(XA_SRC_VOICE_RECORD_TX, &stVoiceSmpRate);
        uwRet += (VOS_UINT32)XA_SRC_Init(XA_SRC_VOICE_RECORD_RX, &stVoiceSmpRate);

        /* 更新记录原始语音数据采样率 */
        pstVoiceRcdObj->uwOrignalSmpRate = uwOrignalSmpRate;

    }

    /* 如果初始化失败，上报om信息 */
    if (VOS_OK != uwRet)
    {
        OM_LogError1(AUDIO_RecorderInitSrcFailed, uwRet);
    }

    return uwRet;
}



VOS_VOID AUDIO_RECORDER_VoiceAudioPcmCB(
                VOS_VOID  *pshwBufOut,
                VOS_VOID  *pshwBufIn,
                VOS_UINT32 uwTarSmpRate,
                VOS_UINT32 uwChannelNum,
                VOS_UINT32 uwTarBufSize)
{
    AUDIO_RECORDER_OBJ_STRU             *pstAudioRcdObj;
    AUDIO_RECORDER_VOICE_STRU           *pstVoiceRcdObj;
    VOS_UINT32                           uwOutputSize;
    VOS_UINT32                           uwProcTimes;

    /* 异常入参检查 */
    if (  (VOS_NULL == pshwBufOut)
        ||(VOS_NULL == pshwBufIn) )
    {
        OM_LogError(AUDIO_RecorderBadPara);
        return;
    }

    /* 获取录音对象 */
    pstAudioRcdObj = AUDIO_RecorderGetObjPtr();
    pstVoiceRcdObj = &pstAudioRcdObj->stVoiceRecord;

    /* 将经过audio SIO口搬运过来的数据丢弃，使用经过降噪处理过的数据 */
    /* 将经过降噪处理的语音数据统一变采样到48K */
    if (AUDIO_RECORDER_SAMPLERATE_8K == pstVoiceRcdObj->uwOrignalSmpRate)
    {
        /* 将数据变采样到48K,每次处理160个点，处理一次 */
        uwProcTimes = 1;
    }
    else if (AUDIO_RECORDER_SAMPLERATE_16K == pstVoiceRcdObj->uwOrignalSmpRate)
    {
        /* 将数据变采样到48K,每次处理160个点，处理一次 */
        uwProcTimes = 2;
    }
    else
    {
        OM_LogError1(AUDIO_RecorderSmpRateErr, pstVoiceRcdObj->uwOrignalSmpRate);
        return;
    }

    /* 将上行数据变采样到48K */
    XA_SRC_Proc8x(XA_SRC_VOICE_RECORD_TX,
                  pstVoiceRcdObj->stRecordBuff.ashwTxDataIn,
                  pstVoiceRcdObj->stRecordBuff.ashwTxDataOut,
                  &uwOutputSize,
                  uwProcTimes);

    /* 将下行数据变采样到48K */
    XA_SRC_Proc8x(XA_SRC_VOICE_RECORD_RX,
                  pstVoiceRcdObj->stRecordBuff.ashwRxDataIn,
                  pstVoiceRcdObj->stRecordBuff.ashwRxDataOut,
                  &uwOutputSize,
                  uwProcTimes);

    /* 将左右声道合并，TX语音数据在左声道存储、RX语音数据在右声道存储 */
    UCOM_COMM_SetChnBit16(
                pshwBufOut,
                2,
                pstVoiceRcdObj->stRecordBuff.ashwTxDataOut,
                AUDIO_RECORDER_BUFF_SIZE_1920,
                0);

    UCOM_COMM_SetChnBit16(
                pshwBufOut,
                2,
                pstVoiceRcdObj->stRecordBuff.ashwRxDataOut,
                AUDIO_RECORDER_BUFF_SIZE_1920,
                1);

}
VOS_VOID AUDIO_RECORDER_AudioCallBack(
                VOS_VOID  *pshwBufOut,
                VOS_VOID  *pshwBufIn,
                VOS_UINT32 uwSampleRate,
                VOS_UINT32 uwChannelNum,
                VOS_UINT32 uwBufSize)
{

    /*透传，之后会调用单Mic降噪*/
    if ((NULL == pshwBufIn) || (NULL == pshwBufOut))
    {
        OM_LogWarning(AUDIO_RECORDER_BuffNullErr);
        return;
    }

    /* 进行上行补偿滤波 */
    AUDIO_ENHANCE_ProcMicIn(
                pshwBufOut,
                pshwBufIn,
                uwSampleRate,
                uwChannelNum,
                uwBufSize);
}


VOS_VOID AUDIO_RECORDER_FMCallBack(
                VOS_VOID  *pshwBufOut,
                VOS_VOID  *pshwBufIn,
                VOS_UINT32 uwSampleRate,
                VOS_UINT32 uwChannelNum,
                VOS_UINT32 uwBufSize)
{
    /*透传*/
    if ((NULL == pshwBufIn) || (NULL == pshwBufOut))
    {
        OM_LogWarning(AUDIO_RECORDER_BuffNullErr);
        return;
    }

    UCOM_MemCpy(pshwBufOut, pshwBufIn, uwBufSize);
}
VOS_VOID AUDIO_RECORDER_SecondVoiceCallBack(
                VOS_VOID  *pshwBufOut,
                VOS_VOID  *pshwBufIn,
                VOS_UINT32 uwSampleRate,
                VOS_UINT32 uwChannelNum,
                VOS_UINT32 uwBufSize)
{
    /*透传*/
    if ((NULL == pshwBufIn) || (NULL == pshwBufOut))
    {
        OM_LogWarning(AUDIO_RECORDER_BuffNullErr);
        return;
    }

    UCOM_MemCpy(pshwBufOut, pshwBufIn, uwBufSize);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif





