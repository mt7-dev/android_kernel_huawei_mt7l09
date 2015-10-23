

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "audio.h"
#include "audio_pcm.h"
#include "audio_enhance.h"
#include "audio_debug.h"
#include "om_log.h"
#include "ucom_mem_dyn.h"
#include "mlib_interface.h"
#include "ucom_nv.h"
#include "om_comm.h"
#include "om_cpuview.h"

#include "ucom_pcm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*lint -e(767)*/
#define THIS_FILE_ID                    OM_FILE_ID_AUDIO_ENHANCE_C

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

/* ENHANCE控制结构体 */
AUDIO_ENHANCE_CTRL_STRU                 g_stAudioEnhanceCtrl;

/* ENHANCE缓存结构体 */
AUDIO_ENHANCE_BUFF_STRU                 g_stAudioEnhanceBuff;

/* ENHANCE模块注册表 */
AUDIO_ENHANCE_MODULE_REG_STRU           g_astAudioEnhanceModuleTab[AUDIO_ENHANCE_MODULE_BUTT] = \
{
    /* Default 模块注册 */
    { MLIB_PATH_AUDIO_RECORD,       MLIB_PATH_AUDIO_PLAY },
    { MLIB_PATH_VOIP_CALL_MICIN,    MLIB_PATH_VOIP_CALL_SPKOUT},
    { MLIB_PATH_SIRI_MICIN,         MLIB_PATH_SIRI_SPKOUT},
};

/*****************************************************************************
  3 函数实现
*****************************************************************************/



VOS_UINT32  AUDIO_ENHANCE_MsgStartReqRt( VOS_VOID *pvOsaMsg )
{
    AP_AUDIO_ENHANCE_START_REQ_STRU    *pstStartReq;
    AUDIO_PCM_ENHANCE_START_IND_STRU    stStartInd;
    VOS_VOID                               *pvMsgTail      = VOS_NULL;

    pstStartReq = (AP_AUDIO_ENHANCE_START_REQ_STRU*)((MsgBlock *) pvOsaMsg)->aucValue;

    /* 该消息需要将原消息的CmdId及Sn号回复给Hifi_misc驱动 */
    pvMsgTail       = (VOS_VOID *)((VOS_UINT32)(((MsgBlock *) pvOsaMsg)->aucValue) + sizeof(AP_AUDIO_ENHANCE_START_REQ_STRU));

    UCOM_MemSet(&stStartInd, 0, sizeof(AUDIO_PCM_ENHANCE_START_IND_STRU));

    stStartInd.uhwMsgId     = ID_AUDIO_PCM_ENHANCE_START_IND;
    stStartInd.uwSampleRate = pstStartReq->uwSampleRate;
    stStartInd.enModule     = pstStartReq->enModule;

    /* 调用通用VOS发送接口，发送消息 */
    UCOM_SendOsaMsg(DSP_PID_AUDIO_RT,
                    DSP_PID_AUDIO,
                   &stStartInd,
                    sizeof(AUDIO_PCM_ENHANCE_START_IND_STRU));

    /* 回复AP确认消息 */
    AUDIO_ENHANCE_MsgSyncConfirm(ID_AUDIO_AP_ENHANCE_START_CNF,
                                 UCOM_RET_SUCC,
                                 pvMsgTail,
                                 sizeof(AUDIO_HIFIMISC_MSG_TAIL_STRU));

    return UCOM_RET_SUCC;
}
VOS_UINT32  AUDIO_ENHANCE_MsgStopReqRt( VOS_VOID *pvOsaMsg )
{
    AP_AUDIO_ENHANCE_STOP_REQ_STRU     *pstStopReq;
    AUDIO_PCM_ENHANCE_STOP_IND_STRU     stStopInd;
    VOS_VOID                               *pvMsgTail      = VOS_NULL;

    pstStopReq = (AP_AUDIO_ENHANCE_STOP_REQ_STRU*)((MsgBlock *) pvOsaMsg)->aucValue;

    /* 该消息需要将原消息的CmdId及Sn号回复给Hifi_misc驱动 */
    pvMsgTail       = (VOS_VOID *)((VOS_UINT32)(((MsgBlock *) pvOsaMsg)->aucValue) + sizeof(AP_AUDIO_ENHANCE_STOP_REQ_STRU));

    UCOM_MemSet(&stStopInd, 0, sizeof(AUDIO_PCM_ENHANCE_STOP_IND_STRU));

    stStopInd.uhwMsgId     = ID_AUDIO_PCM_ENHANCE_STOP_IND;
    stStopInd.enModule     = pstStopReq->enModule;

    /* 调用通用VOS发送接口，发送消息 */
    UCOM_SendOsaMsg(DSP_PID_AUDIO_RT,
                    DSP_PID_AUDIO,
                   &stStopInd,
                    sizeof(AUDIO_PCM_ENHANCE_STOP_IND_STRU));

    /* 回复AP确认消息 */
    AUDIO_ENHANCE_MsgSyncConfirm(ID_AUDIO_AP_ENHANCE_STOP_CNF,
                                 UCOM_RET_SUCC,
                                 pvMsgTail,
                                 sizeof(AUDIO_HIFIMISC_MSG_TAIL_STRU));

    return UCOM_RET_SUCC;
}
VOS_UINT32  AUDIO_ENHANCE_MsgSetDeviceReqRt( VOS_VOID *pvOsaMsg )
{
    AP_AUDIO_ENHANCE_SET_DEVICE_REQ_STRU   *pstSetReq;
    AUDIO_PCM_ENHANCE_SET_DEVICE_IND_STRU   stSetInd;
    VOS_VOID                               *pvMsgTail      = VOS_NULL;

    pstSetReq = (AP_AUDIO_ENHANCE_SET_DEVICE_REQ_STRU*)((MsgBlock *) pvOsaMsg)->aucValue;

    /* 该消息需要将原消息的CmdId及Sn号回复给Hifi_misc驱动 */
    pvMsgTail       = (VOS_VOID *)((VOS_UINT32)(((MsgBlock *) pvOsaMsg)->aucValue) + sizeof(AP_AUDIO_ENHANCE_SET_DEVICE_REQ_STRU));

    UCOM_MemSet(&stSetInd, 0, sizeof(AUDIO_PCM_ENHANCE_SET_DEVICE_IND_STRU));

    stSetInd.uhwMsgId   = ID_AUDIO_PCM_ENHANCE_SET_DEVICE_IND;
    stSetInd.uwDevice   = pstSetReq->uwDevice;

    /* 调用通用VOS发送接口，发送消息 */
    UCOM_SendOsaMsg(DSP_PID_AUDIO_RT,
                    DSP_PID_AUDIO,
                   &stSetInd,
                    sizeof(AUDIO_PCM_ENHANCE_SET_DEVICE_IND_STRU));

    /* 回复AP确认消息 */
    AUDIO_ENHANCE_MsgSyncConfirm(ID_AUDIO_AP_ENHANCE_SET_DEVICE_CNF,
                                 UCOM_RET_SUCC,
                                 pvMsgTail,
                                 sizeof(AUDIO_HIFIMISC_MSG_TAIL_STRU));

    return UCOM_RET_SUCC;
}
VOS_UINT32 AUDIO_ENHANCE_MsgSetParaIndRt(VOS_VOID *pstOsaMsg)
{
    /* 进行低优先级消息转发 */
    UCOM_SendOsaMsg(
                DSP_PID_AUDIO_RT,
                DSP_PID_AUDIO,
                pstOsaMsg,
                (((MsgBlock *) pstOsaMsg)->uwLength + VOS_MSG_HEAD_LENGTH));

    return UCOM_RET_SUCC;
}
VOS_UINT32  AUDIO_ENHANCE_MsgStartInd( VOS_VOID *pvOsaMsg )
{
    VOS_UINT32                          uwRet;
    AUDIO_PCM_ENHANCE_START_IND_STRU   *pstStartInd;

    pstStartInd = (AUDIO_PCM_ENHANCE_START_IND_STRU*)pvOsaMsg;

    if(pstStartInd->enModule >= AUDIO_ENHANCE_MODULE_BUTT)
    {
        OM_LogError1(AUDIO_ENHANCE_MsgStartReq_InvalidModule, pstStartInd->enModule);

        return UCOM_RET_FAIL;
    }

    uwRet = AUDIO_ENHANCE_Start(pstStartInd->enModule, pstStartInd->uwSampleRate);

    if(uwRet != UCOM_RET_SUCC)
    {
        OM_LogError2(AUDIO_ENHANCE_MsgStartReq_Failed, pstStartInd->enModule, uwRet);
    }
    else
    {
        OM_LogInfo1(AUDIO_ENHANCE_MsgStartReq_Succ, pstStartInd->enModule);
    }

    return UCOM_RET_SUCC;
}


VOS_UINT32  AUDIO_ENHANCE_MsgStopInd( VOS_VOID *pvOsaMsg )
{
    VOS_UINT32                          uwRet;
    AUDIO_PCM_ENHANCE_STOP_IND_STRU    *pstStopInd;

    pstStopInd = (AUDIO_PCM_ENHANCE_STOP_IND_STRU*)pvOsaMsg;

    if(pstStopInd->enModule >= AUDIO_ENHANCE_MODULE_BUTT)
    {
        OM_LogError1(AUDIO_ENHANCE_MsgStopReq_InvalidModule, pstStopInd->enModule);

        return UCOM_RET_FAIL;
    }

    uwRet = AUDIO_ENHANCE_Stop(pstStopInd->enModule);

    if(uwRet != UCOM_RET_SUCC)
    {
        OM_LogError2(AUDIO_ENHANCE_MsgStopReq_Failed, pstStopInd->enModule, uwRet);
    }
    else
    {
        OM_LogInfo1(AUDIO_ENHANCE_MsgStopReq_Succ, pstStopInd->enModule);
    }

    return UCOM_RET_SUCC;
}


VOS_UINT32  AUDIO_ENHANCE_MsgSetDeviceInd( VOS_VOID *pvOsaMsg )
{
    VOS_UINT32                              uwRet;
    AUDIO_PCM_ENHANCE_SET_DEVICE_IND_STRU  *pstSetDeviceInd;

    pstSetDeviceInd = (AUDIO_PCM_ENHANCE_SET_DEVICE_IND_STRU*)pvOsaMsg;

    if(pstSetDeviceInd->uwDevice >= AUDIO_ENHANCE_DEVICE_BUTT)
    {
        OM_LogError1(AUDIO_ENHANCE_MsgSetDeviceReq_InvalidDevice, pstSetDeviceInd->uwDevice);

        return UCOM_RET_FAIL;
    }

    uwRet = AUDIO_ENHANCE_SetDevice(pstSetDeviceInd->uwDevice);

    if(uwRet != UCOM_RET_SUCC)
    {
        OM_LogError1(AUDIO_ENHANCE_MsgSetDeviceReq_Failed, uwRet);
    }
    else
    {
        OM_LogInfo(AUDIO_ENHANCE_MsgSetDeviceReq_Succ);
    }

    return UCOM_RET_SUCC;
 }


VOS_UINT32 AUDIO_ENHANCE_MsgSetParaInd(VOS_VOID *pstOsaMsg)
{
    VOS_UINT32                          uwRet;
    AP_AUDIO_MLIB_SET_PARA_IND_STRU    *pstApPara;
    MLIB_PARA_STRU                     *pstMlibPara;
    VOS_UINT8                          *pucBuf;
    VOS_UINT32                          uwBufSize;
    AUDIO_ENHANCE_CTRL_STRU            *pstEnchanceCtrl;
    AUDIO_ENHANCE_MODULE_CTRL_STRU     *pstModuleCtrl;

    /* 获取控制结构体 */
    pstEnchanceCtrl = AUDIO_ENHANCE_GetCtrl();

    /* 检查模块合法性 */
    if(pstEnchanceCtrl->enCurrModule >= AUDIO_ENHANCE_MODULE_BUTT)
    {
        OM_LogError1(AUDIO_ENHANCE_MsgOmSetNvReqModuleError, pstEnchanceCtrl->enCurrModule);

        return UCOM_RET_FAIL;
    }

    pstModuleCtrl   = AUDIO_ENHANCE_GetModuleCtrl(pstEnchanceCtrl->enCurrModule);

    pstApPara = (AP_AUDIO_MLIB_SET_PARA_IND_STRU*)((MsgBlock *) pstOsaMsg)->aucValue;

    /* 分配数据报文的空间 */
    uwBufSize = sizeof(MLIB_PARA_STRU) + pstApPara->uwSize;
    pucBuf = (VOS_UINT8*)UCOM_MemAlloc(uwBufSize);
    UCOM_MemSet(pucBuf, 0, uwBufSize);
    pstMlibPara = (MLIB_PARA_STRU*)pucBuf;

    /* 设置数据报文头 */
    pstMlibPara->uwChnNum     = 2;  /* 默认双声道 */
    pstMlibPara->uwSampleRate = pstModuleCtrl->uwSampleRate;
    pstMlibPara->uwFrameLen   = ((pstModuleCtrl->uwSampleRate / 8000) * 160);
    pstMlibPara->uwResolution = 16;
    pstMlibPara->enDevice     = pstEnchanceCtrl->enDevice;
    pstMlibPara->enParaSource = MLIB_PARA_SOURCE_APP;
    pstMlibPara->uwDataSize   = pstApPara->uwSize;

    /* 拷贝数据报文体 */
    UCOM_MemCpy(pstMlibPara->aucData, pstApPara->aucData, pstApPara->uwSize);

    /* 将数据报文发送至对应模块 */
    uwRet = MLIB_PathModuleSet(
                pstApPara->uwPathID,
                pstApPara->uwModuleID,
                pstMlibPara,
                uwBufSize);

    /* 释放报文内存 */
    UCOM_MemFree(pucBuf);

    return uwRet;
}



VOS_UINT32 AUDIO_ENHANCE_MsgOmSetNvReq(VOS_VOID *pstOsaMsg)
{
    VOS_UINT32                          uwRet;
    MSG_CODEC_CNF_STRU                  stRspPrim;
    MSG_OM_MLIB_PARA_SET_STRU          *pstOmPara;
    MLIB_PARA_STRU                     *pstMlibPara;
    VOS_UINT8                          *pucBuf;
    VOS_UINT32                          uwBufSize;
    AUDIO_ENHANCE_CTRL_STRU            *pstEnchanceCtrl;
    AUDIO_ENHANCE_MODULE_CTRL_STRU     *pstModuleCtrl;

    /* 获取控制结构体 */
    pstEnchanceCtrl = AUDIO_ENHANCE_GetCtrl();

    /* 检查模块合法性 */
    if(pstEnchanceCtrl->enCurrModule >= AUDIO_ENHANCE_MODULE_BUTT)
    {
        OM_LogError1(AUDIO_ENHANCE_MsgOmSetNvReqModuleError, pstEnchanceCtrl->enCurrModule);

        return UCOM_RET_FAIL;
    }

    pstModuleCtrl   = AUDIO_ENHANCE_GetModuleCtrl(pstEnchanceCtrl->enCurrModule);

    pstOmPara = (MSG_OM_MLIB_PARA_SET_STRU*)pstOsaMsg;

    /* 分配数据报文的空间 */
    uwBufSize = sizeof(MLIB_PARA_STRU) + pstOmPara->uwSize;
    pucBuf = (VOS_UINT8*)UCOM_MemAlloc(uwBufSize);
    UCOM_MemSet(pucBuf, 0, uwBufSize);
    pstMlibPara = (MLIB_PARA_STRU*)pucBuf;

    /* 设置数据报文头 */
    pstMlibPara->uwChnNum     = 2;  /* 默认双声道 */
    pstMlibPara->uwSampleRate = pstModuleCtrl->uwSampleRate;
    pstMlibPara->uwFrameLen   = ((pstModuleCtrl->uwSampleRate / 8000) * 160);
    pstMlibPara->uwResolution = 16;
    pstMlibPara->enDevice     = pstEnchanceCtrl->enDevice;
    pstMlibPara->enParaSource = MLIB_PARA_SOURCE_TUNER;
    pstMlibPara->uwDataSize   = pstOmPara->uwSize;

    /* 拷贝数据报文体 */
    UCOM_MemCpy(pstMlibPara->aucData, pstOmPara->aucData, pstOmPara->uwSize);

    /* 将数据报文发送至对应模块 */
    uwRet = MLIB_PathModuleSet(
                pstOmPara->uwPathID,
                pstOmPara->uwModuleID,
                pstMlibPara,
                uwBufSize);

    /* 填充回复消息ID */
    stRspPrim.usMsgName = ID_VOICE_OM_SET_NV_CNF;

    /* 填充执行结果 */
    stRspPrim.enExeRslt = (VOS_UINT16)uwRet;

    /* 向SDT回复消息ID_AUDIO_OM_SET_NV_CNF */
    OM_COMM_SendTranMsg(&stRspPrim, sizeof(stRspPrim));

    /* 释放报文内存 */
    UCOM_MemFree(pucBuf);

    return uwRet;
}
VOS_UINT32 AUDIO_ENHANCE_MsgOmGetNvReq(VOS_VOID *pstOsaMsg)
{
    VOS_UINT32                          uwRet;
    MSG_OM_MLIB_PARA_GET_STRU          *pstOmPara;
    MSG_OM_MLIB_PARA_GET_STRU          *pstGetPara;
    VOS_UINT8                          *pucBuf;
    VOS_UINT32                          uwSize;

    pstOmPara = (MSG_OM_MLIB_PARA_GET_STRU*)pstOsaMsg;

    uwSize = sizeof(MSG_OM_MLIB_PARA_GET_STRU) + pstOmPara->uwSize;
    pucBuf = (VOS_UINT8*) UCOM_MemAlloc(uwSize);

    pstGetPara = (MSG_OM_MLIB_PARA_GET_STRU*)pucBuf;

    UCOM_MemCpy(pstGetPara, pstOmPara, sizeof(MSG_OM_MLIB_PARA_GET_STRU));
    UCOM_MemCpy(pstGetPara->aucData, pstOmPara->aucData, pstOmPara->uwSize);

    uwRet = MLIB_PathModuleGet(
                pstOmPara->uwPathID,
                pstOmPara->uwModuleID,
                pstGetPara->aucData,
                pstOmPara->uwSize);

    if(UCOM_RET_SUCC == uwRet)
    {
        pstGetPara->usMsgName = ID_VOICE_OM_GET_NV_CNF;

        OM_COMM_SendTranMsg(pstGetPara, uwSize);
    }

    UCOM_MemFree(pucBuf);

    return UCOM_RET_SUCC;
}


VOS_VOID AUDIO_ENHANCE_MsgSyncConfirm(VOS_UINT16  uhwMsgId,
                                                    VOS_UINT32  uwRet,
                                                    VOS_VOID   *pvTail,
                                                    VOS_UINT32  uwTailLen)
{
    APAUDIO_OP_RSLT_STRU    stCnfMsg;

    /* 清空消息 */
    UCOM_MemSet(&stCnfMsg, 0, sizeof(APAUDIO_OP_RSLT_STRU));

    /* 配置消息和返回值 */
    stCnfMsg.uhwMsgId   = uhwMsgId;
    stCnfMsg.uhwResult  = (VOS_UINT16)uwRet;

    /*  回复消息给AP */
    UCOM_COMM_SendDspMsg(DSP_PID_AUDIO,
                         ACPU_PID_OM,
                        &stCnfMsg,
                         sizeof(APAUDIO_OP_RSLT_STRU),
                         pvTail,
                         uwTailLen);
}
VOS_UINT32  AUDIO_ENHANCE_Init( VOS_VOID )
{
    AUDIO_ENHANCE_CTRL_STRU            *pstCtrl;
    AUDIO_ENHANCE_BUFF_STRU            *pstBuff;

    /* 获取控制结构体 */
    pstCtrl = AUDIO_ENHANCE_GetCtrl();

    UCOM_MemSet(pstCtrl, 0, sizeof(AUDIO_ENHANCE_CTRL_STRU));

    /* 设置保护字 */
    pstCtrl->uwProtectWord1 = UCOM_PROTECT_WORD1;
    pstCtrl->uwProtectWord2 = UCOM_PROTECT_WORD2;
    pstCtrl->uwProtectWord3 = UCOM_PROTECT_WORD3;
    pstCtrl->uwProtectWord4 = UCOM_PROTECT_WORD4;

    /* 获取缓冲结构体 */
    pstBuff = AUDIO_ENHANCE_GetBuff();

    UCOM_MemSet(pstBuff, 0, sizeof(AUDIO_ENHANCE_BUFF_STRU));

    /* 设置保护字 */
    pstBuff->uwProtectWord1 = UCOM_PROTECT_WORD1;
    pstBuff->uwProtectWord2 = UCOM_PROTECT_WORD2;
    pstBuff->uwProtectWord3 = UCOM_PROTECT_WORD3;
    pstBuff->uwProtectWord4 = UCOM_PROTECT_WORD4;

    return UCOM_RET_SUCC;
}


VOS_UINT32 AUDIO_ENHANCE_Start(
                AUDIO_ENHANCE_MODULE_ENUM_UINT32    enModule,
                VOS_UINT32                          uwSampleRate )
{
    VOS_UINT32                          uwRet;
    AUDIO_ENHANCE_CTRL_STRU            *pstEnhanceCtrl;
    AUDIO_ENHANCE_MODULE_CTRL_STRU     *pstModuleCtrl;

    /* 获取控制结构体 */
    pstEnhanceCtrl  = AUDIO_ENHANCE_GetCtrl();
    pstModuleCtrl   = AUDIO_ENHANCE_GetModuleCtrl(enModule);

    /* 设置当前使用的模块 */
    pstEnhanceCtrl->enCurrModule = enModule;

    /* 引用计数递增 */
    pstModuleCtrl->uwRefCnt++;

    if(AUDIO_ENHANCE_STATUS_RUNNING == pstModuleCtrl->enStatus)
    {
        /* 再次注册回调函数 */
        AUDIO_ENHANCE_RegisterPcmCallback();

        return UCOM_RET_SUCC;
    }

    /* 记录采样率 */
    pstModuleCtrl->uwSampleRate = uwSampleRate;

    /* 初始化声学处理 */
    uwRet = AUDIO_ENHANCE_ProcInit(enModule);

    if(uwRet != UCOM_RET_SUCC)
    {
        OM_LogError2(AUDIO_ENHANCE_Init_ProcInitFailed, enModule, uwRet);

        return UCOM_RET_FUNC_DISABLE;
    }

    /* 更新声学处理参数 */
    uwRet = AUDIO_ENHANCE_ProcSetPara(enModule, pstEnhanceCtrl->enDevice);

    if(uwRet != UCOM_RET_SUCC)
    {
        OM_LogError2(AUDIO_ENHANCE_Init_ProcSetParaFailed, enModule, uwRet);

        return UCOM_RET_FUNC_DISABLE;
    }

    pstModuleCtrl->enStatus = AUDIO_ENHANCE_STATUS_RUNNING;

    /* 注册通路处理回调函数 */
    AUDIO_ENHANCE_RegisterPcmCallback();

    /* 当需要变采样时, 初始化变采样 */
    if (uwSampleRate != AUDIO_ENHANCE_DEFAULT_SAMPLE_RATE)
    {
        uwRet = AUDIO_ENHANCE_InitSrc(enModule, uwSampleRate);

        if(uwRet != UCOM_RET_SUCC)
        {
            OM_LogError1(AUDIO_ENHANCE_InitSrcError, uwRet);

            return UCOM_RET_FUNC_DISABLE;
        }
    }

    return UCOM_RET_SUCC;
}
VOS_UINT32 AUDIO_ENHANCE_SetDevice( AUDIO_ENHANCE_DEVICE_ENUM_UINT32 enDevice )
{
    AUDIO_ENHANCE_CTRL_STRU            *pstEnhanceCtrl;
    AUDIO_ENHANCE_MODULE_CTRL_STRU     *pstModuleCtrl;
    VOS_UINT32                          uwRet;

    uwRet = UCOM_RET_SUCC;

    /* 获取控制结构体 */
    pstEnhanceCtrl = AUDIO_ENHANCE_GetCtrl();

    /* 检查模块合法性 */
    if(pstEnhanceCtrl->enCurrModule >= AUDIO_ENHANCE_MODULE_BUTT)
    {
        OM_LogError1(AUDIO_ENHANCE_SetDeviceModuleError, pstEnhanceCtrl->enCurrModule);

        return UCOM_RET_FAIL;
    }

    pstModuleCtrl   = AUDIO_ENHANCE_GetModuleCtrl(pstEnhanceCtrl->enCurrModule);

    /* 记录设备模式 */
    pstEnhanceCtrl->enDevice = enDevice;

    /* 更新处理参数 */
    if(AUDIO_ENHANCE_STATUS_RUNNING == pstModuleCtrl->enStatus)
    {
        uwRet = AUDIO_ENHANCE_ProcSetPara(pstEnhanceCtrl->enCurrModule, enDevice);
    }

    /* 打开/关闭SmartPA */
    if((AUDIO_ENHANCE_MODULE_VOIP == pstEnhanceCtrl->enCurrModule)
     ||(AUDIO_ENHANCE_MODULE_SIRI == pstEnhanceCtrl->enCurrModule))
    {
        AUDIO_ENHANCE_SetExRefPort(enDevice);
    }

    return uwRet;
}
VOS_UINT32 AUDIO_ENHANCE_Stop( AUDIO_ENHANCE_MODULE_ENUM_UINT32 enModule )
{
    AUDIO_ENHANCE_MODULE_CTRL_STRU     *pstModuleCtrl;

    /* 获取控制结构体 */
    pstModuleCtrl   = AUDIO_ENHANCE_GetModuleCtrl(enModule);

    if(0 == pstModuleCtrl->uwRefCnt)
    {
        OM_LogWarning1(AUDIO_ENHANCE_StopRefCntInconsistent, enModule);

        return UCOM_RET_FAIL;
    }

    /* 引用计数递减 */
    pstModuleCtrl->uwRefCnt--;

    if(0 == pstModuleCtrl->uwRefCnt)
    {
        if(AUDIO_ENHANCE_STATUS_RUNNING == pstModuleCtrl->enStatus)
        {
            /* 清空声学处理 */
            AUDIO_ENHANCE_ProcClear(enModule);

            /* 去注册通路处理回调函数 */
            AUDIO_PCM_RegisterPcmCbFunc(
                    AUDIO_PCM_MODE_CAPTURE,
                    AUDIO_PCM_OBJ_AP,
                    VOS_NULL,
                    VOS_NULL);

            AUDIO_PCM_RegisterPcmCbFunc(
                    AUDIO_PCM_MODE_PLAYBACK,
                    AUDIO_PCM_OBJ_AP,
                    VOS_NULL,
                    VOS_NULL);

            /* 关闭变采样 */
            if (pstModuleCtrl->uwSampleRate != AUDIO_ENHANCE_DEFAULT_SAMPLE_RATE)
            {
                AUDIO_ENHANCE_CloseSrc(enModule);
            }
        }

        pstModuleCtrl->enStatus = AUDIO_ENHANCE_STATUS_IDLE;
    }

    return UCOM_RET_SUCC;

}
VOS_VOID AUDIO_ENHANCE_RegisterPcmCallback( VOS_VOID )
{
    /* 注册通路处理回调函数 */
    AUDIO_PCM_RegisterPcmCbFunc(AUDIO_PCM_MODE_CAPTURE,
                                AUDIO_PCM_OBJ_AP,
                                VOS_NULL,
                                (AUDIO_PCM_CALLBACK)AUDIO_ENHANCE_ProcMicIn);

    AUDIO_PCM_RegisterPcmCbFunc(AUDIO_PCM_MODE_PLAYBACK,
                                AUDIO_PCM_OBJ_AP,
                                VOS_NULL,
                                (AUDIO_PCM_CALLBACK)AUDIO_ENHANCE_ProcSpkOut);
}
VOS_UINT32 AUDIO_ENHANCE_ProcInit( AUDIO_ENHANCE_MODULE_ENUM_UINT32 enModule )
{
    VOS_UINT32                          uwRet;
    AUDIO_ENHANCE_MODULE_REG_STRU      *pstModule;

    pstModule = AUDIO_ENHANCE_GetModuleReg(enModule);

    /* 初始化声学处理 */
    uwRet = MLIB_PathInit(pstModule->uwMicInPathID);

    if(uwRet != UCOM_RET_SUCC)
    {
        OM_LogError2(AUDIO_ENHANCE_ProcMicIn_InitErr, pstModule->uwMicInPathID, uwRet);

        return UCOM_RET_FUNC_DISABLE;
    }

    uwRet = MLIB_PathInit(pstModule->uwSpkOutPathID);

    if(uwRet != UCOM_RET_SUCC)
    {
        OM_LogError2(AUDIO_ENHANCE_ProcSpkOut_InitErr, pstModule->uwSpkOutPathID, uwRet);

        return UCOM_RET_FUNC_DISABLE;
    }

    return uwRet;
}
VOS_UINT32 AUDIO_ENHANCE_ProcSetPara(
                AUDIO_ENHANCE_MODULE_ENUM_UINT32            enModule,
                AUDIO_ENHANCE_DEVICE_ENUM_UINT32            enDevice )
{
    VOS_UINT32                          uwRet;
    MLIB_PARA_STRU                      stPara;
    AUDIO_ENHANCE_MODULE_CTRL_STRU     *pstModuleCtrl;
    AUDIO_ENHANCE_MODULE_REG_STRU      *pstModuleReg;

    /* 获取控制结构体 */
    pstModuleCtrl = AUDIO_ENHANCE_GetModuleCtrl(enModule);
    /* 获取注册结构体 */
    pstModuleReg  = AUDIO_ENHANCE_GetModuleReg(enModule);

    UCOM_MemSet(&stPara, 0, sizeof(MLIB_PARA_STRU));

    /* 通路参数设置 */
    stPara.uwChnNum     = 2;
    stPara.uwSampleRate = pstModuleCtrl->uwSampleRate;
    stPara.uwFrameLen   =  ((pstModuleCtrl->uwSampleRate / 8000) * 160);
    stPara.uwResolution = 16;
    stPara.enDevice     = enDevice;
    stPara.enParaSource = MLIB_PARA_SOURCE_NV;
    stPara.uwDataSize   = 0;

    uwRet = MLIB_PathSet(
                pstModuleReg->uwMicInPathID,
                &stPara,
                sizeof(MLIB_PARA_STRU));

    if(uwRet != UCOM_RET_SUCC)
    {
        OM_LogError2(AUDIO_ENHANCE_SetPara_RecordFail, pstModuleReg->uwMicInPathID, uwRet);
    }

    /* 通路参数设置 */
    stPara.uwChnNum     = 2;

    uwRet = MLIB_PathSet(
                pstModuleReg->uwSpkOutPathID,
                &stPara,
                sizeof(MLIB_PARA_STRU));

    if(uwRet != UCOM_RET_SUCC)
    {
        OM_LogError2(AUDIO_ENHANCE_SetPara_RecordFail, pstModuleReg->uwSpkOutPathID, uwRet);
    }

    return uwRet;
}


VOS_UINT32 AUDIO_ENHANCE_ProcMicIn(
                VOS_VOID               *pshwBufOut,
                VOS_VOID               *pshwBufIn,
                VOS_UINT32              uwSampleRate,
                VOS_UINT32              uwChannelNum,
                VOS_UINT32              uwBufSize)
{
    VOS_UINT32                          uwRet;
    AUDIO_ENHANCE_CTRL_STRU            *pstEnhanceCtrl;
    AUDIO_ENHANCE_MODULE_CTRL_STRU     *pstModuleCtrl;
    AUDIO_ENHANCE_MODULE_REG_STRU      *pstModuleReg;
    MLIB_SOURCE_STRU                    stMlibIn, stMlibOut;
    VOS_UINT32                          uwProcSize;
    VOS_UINT32                          uwOutputBuffSize;

    /* 获取控制结构体 */
    pstEnhanceCtrl  = AUDIO_ENHANCE_GetCtrl();
    pstModuleCtrl   = AUDIO_ENHANCE_GetModuleCtrl(pstEnhanceCtrl->enCurrModule);
    pstModuleReg    = AUDIO_ENHANCE_GetModuleReg(pstEnhanceCtrl->enCurrModule);

    /* 打印当前模块 */
    OM_LogInfo1(AUDIO_ENHANCE_Module_MicIn, pstEnhanceCtrl->enCurrModule);

    /* 将输入输出信息清0 */
    UCOM_MemSet(&stMlibIn,  0, sizeof(MLIB_SOURCE_STRU));
    UCOM_MemSet(&stMlibOut, 0, sizeof(MLIB_SOURCE_STRU));

    /* 非Running态数据透传 */
    if(pstModuleCtrl->enStatus != AUDIO_ENHANCE_STATUS_RUNNING)
    {
        UCOM_MemCpy(pshwBufOut, pshwBufIn, uwBufSize);

        OM_LogWarning1(AUDIO_ENHANCE_ProcMicIn_FuncDisable, pstEnhanceCtrl->enCurrModule);

        uwRet = UCOM_RET_SUCC;
    }
    else
    {
        OM_CPUVIEW_EnterArea((VOS_UCHAR)OM_CPUVIEW_AREA_AUDIO_ENHANCE_PROC_MICIN);

        /* 勾取输入数据 */
        AUDIO_DEBUG_SendHookInd(AUDIO_HOOK_ENHANCE_MICIN, pshwBufIn, uwBufSize);

        /* 计算需要处理的数据(单声道) */
        uwProcSize = AUDIO_ENHANCE_GetMonoFrmSize(pstModuleCtrl->uwSampleRate);

        if(pstModuleCtrl->uwSampleRate == uwSampleRate)
        {
            /* 将输入数据由混合数据拆分成2路 */
            UCOM_COMM_GetChnBit16(
                    AUDIO_ENHANCE_GetMicInL(AUDIO_ENHANCE_BUFF_IN),
                    uwProcSize,
                    0,
                    pshwBufIn,
                    2);

            UCOM_COMM_GetChnBit16(
                    AUDIO_ENHANCE_GetMicInR(AUDIO_ENHANCE_BUFF_IN),
                    uwProcSize,
                    1,
                    pshwBufIn,
                    2);
        }
        else
        {
            /* 将输入数据由混合数据拆分成2路 */
            UCOM_COMM_GetChnBit16(
                    AUDIO_ENHANCE_GetMicInL(AUDIO_ENHANCE_BUFF_SWAP),
                    (uwBufSize/2),
                    0,
                    pshwBufIn,
                    2);

            UCOM_COMM_GetChnBit16(
                    AUDIO_ENHANCE_GetMicInR(AUDIO_ENHANCE_BUFF_SWAP),
                    (uwBufSize/2),
                    1,
                    pshwBufIn,
                    2);

            /* 执行变采样 */
            AUDIO_ENHANCE_DoSrc(XA_SRC_VOIP_MICIN_48K_16K_L,
                                AUDIO_ENHANCE_GetMicInL(AUDIO_ENHANCE_BUFF_SWAP),
                                AUDIO_ENHANCE_GetMicInL(AUDIO_ENHANCE_BUFF_IN),
                                &uwOutputBuffSize);

            AUDIO_ENHANCE_DoSrc(XA_SRC_VOIP_MICIN_48K_16K_R,
                                AUDIO_ENHANCE_GetMicInR(AUDIO_ENHANCE_BUFF_SWAP),
                                AUDIO_ENHANCE_GetMicInR(AUDIO_ENHANCE_BUFF_IN),
                                &uwOutputBuffSize);

            /* 勾取左声道降为16K的数据 */
            AUDIO_DEBUG_SendHookInd(
                        AUDIO_HOOK_ENHANCE_MLIB_MICIN_16K_L,
                        AUDIO_ENHANCE_GetMicInL(AUDIO_ENHANCE_BUFF_IN),
                        uwProcSize);

            /* 勾取右声道降为16K的数据 */
            AUDIO_DEBUG_SendHookInd(
                        AUDIO_HOOK_ENHANCE_MLIB_MICIN_16K_R,
                        AUDIO_ENHANCE_GetMicInR(AUDIO_ENHANCE_BUFF_IN),
                        uwProcSize);

        }

        /* 设置输入数据 */
        stMlibIn.uwChnNum = 4;

        /* 2路Mic输入 */
        MLIB_UtilitySetSource(
                    &stMlibIn,
                    0,
                    AUDIO_ENHANCE_GetMicInL(AUDIO_ENHANCE_BUFF_IN),
                    uwProcSize,
                    pstModuleCtrl->uwSampleRate);

        MLIB_UtilitySetSource(
                    &stMlibIn,
                    1,
                    AUDIO_ENHANCE_GetMicInR(AUDIO_ENHANCE_BUFF_IN),
                    uwProcSize,
                    pstModuleCtrl->uwSampleRate);

        /* 处理下行输入数据 */
        AUDIO_ENHANCE_ProcEcRef(pstModuleCtrl->uwSampleRate);

        /* 2路Spk输出也作为输入 */
        MLIB_UtilitySetSource(
                    &stMlibIn,
                    2,
                    AUDIO_ENHANCE_GetSpkOutL(AUDIO_ENHANCE_BUFF_OUT),
                    uwProcSize,
                    pstModuleCtrl->uwSampleRate);

        MLIB_UtilitySetSource(
                    &stMlibIn,
                    3,
                    AUDIO_ENHANCE_GetSpkOutR(AUDIO_ENHANCE_BUFF_OUT),
                    uwProcSize,
                    pstModuleCtrl->uwSampleRate);

        /* 设置输出数据 */
        stMlibOut.uwChnNum = 4;

        MLIB_UtilitySetSource(
                    &stMlibOut,
                    0,
                    AUDIO_ENHANCE_GetMicInL(AUDIO_ENHANCE_BUFF_OUT),
                    uwProcSize,
                    pstModuleCtrl->uwSampleRate);

        MLIB_UtilitySetSource(
                    &stMlibOut,
                    1,
                    AUDIO_ENHANCE_GetMicInR(AUDIO_ENHANCE_BUFF_OUT),
                    uwProcSize,
                    pstModuleCtrl->uwSampleRate);

        /* 2路Spk输出也作为输出 */
        MLIB_UtilitySetSource(
                    &stMlibOut,
                    2,
                    AUDIO_ENHANCE_GetSpkOutL(AUDIO_ENHANCE_BUFF_OUT),
                    uwProcSize,
                    pstModuleCtrl->uwSampleRate);

        MLIB_UtilitySetSource(
                    &stMlibOut,
                    3,
                    AUDIO_ENHANCE_GetSpkOutR(AUDIO_ENHANCE_BUFF_OUT),
                    uwProcSize,
                    pstModuleCtrl->uwSampleRate);

        OM_CPUVIEW_EnterArea((VOS_UCHAR)OM_CPUVIEW_AREA_AUDIO_ENHANCE_PROC_MLIB_MICIN);

        /* 调用声学处理 */
        uwRet = MLIB_PathProc(
            pstModuleReg->uwMicInPathID,
            &stMlibOut,
            &stMlibIn);

        OM_CPUVIEW_ExitArea((VOS_UCHAR)OM_CPUVIEW_AREA_AUDIO_ENHANCE_PROC_MLIB_MICIN);

        if(pstModuleCtrl->uwSampleRate == uwSampleRate)
        {
            /* 将处理后的数据由2路混合 */
            UCOM_COMM_SetChnBit16(
                    pshwBufOut,
                    2,
                    AUDIO_ENHANCE_GetMicInL(AUDIO_ENHANCE_BUFF_OUT),
                    uwProcSize,
                    0);

            UCOM_COMM_SetChnBit16(
                    pshwBufOut,
                    2,
                    AUDIO_ENHANCE_GetMicInR(AUDIO_ENHANCE_BUFF_OUT),
                    uwProcSize,
                    1);
        }
        else
        {

            /* 执行变采样 */
            AUDIO_ENHANCE_DoSrc(XA_SRC_VOIP_MICIN_16K_48K_L,
                                AUDIO_ENHANCE_GetMicInL(AUDIO_ENHANCE_BUFF_OUT),
                                AUDIO_ENHANCE_GetMicInL(AUDIO_ENHANCE_BUFF_SWAP),
                                &uwOutputBuffSize);

            AUDIO_ENHANCE_DoSrc(XA_SRC_VOIP_MICIN_16K_48K_R,
                                AUDIO_ENHANCE_GetMicInR(AUDIO_ENHANCE_BUFF_OUT),
                                AUDIO_ENHANCE_GetMicInR(AUDIO_ENHANCE_BUFF_SWAP),
                                &uwOutputBuffSize);

            /* 勾取左声道升为48K的数据 */
            AUDIO_DEBUG_SendHookInd(
                        AUDIO_HOOK_ENHANCE_MLIB_MICIN_48K_L,
                        AUDIO_ENHANCE_GetMicInL(AUDIO_ENHANCE_BUFF_SWAP),
                        (uwBufSize/2));

            /* 勾取右声道升为48K的数据 */
            AUDIO_DEBUG_SendHookInd(
                        AUDIO_HOOK_ENHANCE_MLIB_MICIN_48K_R,
                        AUDIO_ENHANCE_GetMicInR(AUDIO_ENHANCE_BUFF_SWAP),
                        (uwBufSize/2));

            /* 将处理后的数据由2路混合 */
            UCOM_COMM_SetChnBit16(
                    pshwBufOut,
                    2,
                    AUDIO_ENHANCE_GetMicInL(AUDIO_ENHANCE_BUFF_SWAP),
                    (uwBufSize/2),
                    0);

            UCOM_COMM_SetChnBit16(
                    pshwBufOut,
                    2,
                    AUDIO_ENHANCE_GetMicInR(AUDIO_ENHANCE_BUFF_SWAP),
                    (uwBufSize/2),
                    1);

        }

        OM_CPUVIEW_ExitArea((VOS_UCHAR)OM_CPUVIEW_AREA_AUDIO_ENHANCE_PROC_MICIN);

    }

    return uwRet;
}
VOS_UINT32 AUDIO_ENHANCE_ProcSpkOut(
                VOS_VOID               *pshwBufOut,
                VOS_VOID               *pshwBufIn,
                VOS_UINT32              uwSampleRate,
                VOS_UINT32              uwChannelNum,
                VOS_UINT32              uwBufSize)
{
    VOS_UINT32                          uwRet;
    AUDIO_ENHANCE_CTRL_STRU            *pstEnhanceCtrl;
    AUDIO_ENHANCE_MODULE_CTRL_STRU     *pstModuleCtrl;
    AUDIO_ENHANCE_MODULE_REG_STRU      *pstModuleReg;
    MLIB_SOURCE_STRU                    stMlibIn, stMlibOut;
    VOS_UINT32                          uwProcSize;
    VOS_UINT32                          uwOutputBuffSize;

    /* 获取控制结构体 */
    pstEnhanceCtrl  = AUDIO_ENHANCE_GetCtrl();
    pstModuleCtrl   = AUDIO_ENHANCE_GetModuleCtrl(pstEnhanceCtrl->enCurrModule);
    pstModuleReg    = AUDIO_ENHANCE_GetModuleReg(pstEnhanceCtrl->enCurrModule);

    /* 打印当前模块 */
    OM_LogInfo1(AUDIO_ENHANCE_Module_SpkOut, pstEnhanceCtrl->enCurrModule);

    /* 将输入输出信息清0 */
    UCOM_MemSet(&stMlibIn,  0, sizeof(MLIB_SOURCE_STRU));
    UCOM_MemSet(&stMlibOut, 0, sizeof(MLIB_SOURCE_STRU));

    /* 非Running态数据透传 */
    if(pstModuleCtrl->enStatus != AUDIO_ENHANCE_STATUS_RUNNING)
    {
        UCOM_MemCpy(pshwBufOut, pshwBufIn, uwBufSize);

        OM_LogWarning1(AUDIO_ENHANCE_ProcSpkOut_FuncDisable, pstEnhanceCtrl->enCurrModule);

        uwRet = UCOM_RET_SUCC;
    }
    else
    {
        OM_CPUVIEW_EnterArea((VOS_UCHAR)OM_CPUVIEW_AREA_AUDIO_ENHANCE_PROC_SPKOUT);

        /* 勾取输入数据 */
        AUDIO_DEBUG_SendHookInd(AUDIO_HOOK_ENHANCE_SPKOUT, pshwBufIn, uwBufSize);

        /* 计算需要处理的数据(单声道) */
        uwProcSize = AUDIO_ENHANCE_GetMonoFrmSize(pstModuleCtrl->uwSampleRate);

        if(pstModuleCtrl->uwSampleRate == uwSampleRate)
        {
            /* 将输入数据由混合数据拆分成2路 */
            UCOM_COMM_GetChnBit16(
                    AUDIO_ENHANCE_GetSpkOutL(AUDIO_ENHANCE_BUFF_IN),
                    uwProcSize,
                    0,
                    pshwBufIn,
                    2);

            UCOM_COMM_GetChnBit16(
                    AUDIO_ENHANCE_GetSpkOutR(AUDIO_ENHANCE_BUFF_IN),
                    uwProcSize,
                    1,
                    pshwBufIn,
                    2);
        }
        else
        {
            /* 将输入数据由混合数据拆分成2路 */
            UCOM_COMM_GetChnBit16(
                    AUDIO_ENHANCE_GetSpkOutL(AUDIO_ENHANCE_BUFF_SWAP),
                    (uwBufSize/2),
                    0,
                    pshwBufIn,
                    2);

            UCOM_COMM_GetChnBit16(
                    AUDIO_ENHANCE_GetSpkOutR(AUDIO_ENHANCE_BUFF_SWAP),
                    (uwBufSize/2),
                    1,
                    pshwBufIn,
                    2);

            /* 执行变采样 */
            AUDIO_ENHANCE_DoSrc(XA_SRC_VOIP_SPKOUT_48K_16K_L,
                                AUDIO_ENHANCE_GetSpkOutL(AUDIO_ENHANCE_BUFF_SWAP),
                                AUDIO_ENHANCE_GetSpkOutL(AUDIO_ENHANCE_BUFF_IN),
                                &uwOutputBuffSize);

            AUDIO_ENHANCE_DoSrc(XA_SRC_VOIP_SPKOUT_48K_16K_R,
                                AUDIO_ENHANCE_GetSpkOutR(AUDIO_ENHANCE_BUFF_SWAP),
                                AUDIO_ENHANCE_GetSpkOutR(AUDIO_ENHANCE_BUFF_IN),
                                &uwOutputBuffSize);

            /* 勾取左声道16K的输入数据 */
            AUDIO_DEBUG_SendHookInd(
                        AUDIO_HOOK_ENHANCE_MLIB_SPKOUT_16K_L,
                        AUDIO_ENHANCE_GetSpkOutL(AUDIO_ENHANCE_BUFF_IN),
                        uwProcSize);

            /* 勾取右声道16K的数据 */
            AUDIO_DEBUG_SendHookInd(
                        AUDIO_HOOK_ENHANCE_MLIB_SPKOUT_16K_R,
                        AUDIO_ENHANCE_GetSpkOutR(AUDIO_ENHANCE_BUFF_IN),
                        uwProcSize);


        }

        /* 设置输入数据 */
        stMlibIn.uwChnNum = 2;

        MLIB_UtilitySetSource(
                    &stMlibIn,
                    0,
                    AUDIO_ENHANCE_GetSpkOutL(AUDIO_ENHANCE_BUFF_IN),
                    uwProcSize,
                    pstModuleCtrl->uwSampleRate);

        MLIB_UtilitySetSource(
                    &stMlibIn,
                    1,
                    AUDIO_ENHANCE_GetSpkOutR(AUDIO_ENHANCE_BUFF_IN),
                    uwProcSize,
                    pstModuleCtrl->uwSampleRate);


        /* 设置输出数据 */
        stMlibOut.uwChnNum = 2;

        MLIB_UtilitySetSource(
                    &stMlibOut,
                    0,
                    AUDIO_ENHANCE_GetSpkOutL(AUDIO_ENHANCE_BUFF_OUT),
                    uwProcSize,
                    pstModuleCtrl->uwSampleRate);

        MLIB_UtilitySetSource(
                    &stMlibOut,
                    1,
                    AUDIO_ENHANCE_GetSpkOutR(AUDIO_ENHANCE_BUFF_OUT),
                    uwProcSize,
                    pstModuleCtrl->uwSampleRate);

        OM_CPUVIEW_EnterArea((VOS_UCHAR)OM_CPUVIEW_AREA_AUDIO_ENHANCE_PROC_MLIB_SPKOUT);

        /* 调用声学处理 */
        uwRet = MLIB_PathProc(
            pstModuleReg->uwSpkOutPathID,
            &stMlibOut,
            &stMlibIn);

        OM_CPUVIEW_ExitArea((VOS_UCHAR)OM_CPUVIEW_AREA_AUDIO_ENHANCE_PROC_MLIB_SPKOUT);

        if(pstModuleCtrl->uwSampleRate == uwSampleRate)
        {
            /* 将处理后的数据由2路混合 */
            UCOM_COMM_SetChnBit16(
                    pshwBufOut,
                    2,
                    AUDIO_ENHANCE_GetSpkOutL(AUDIO_ENHANCE_BUFF_OUT),
                    uwProcSize,
                    0);

            UCOM_COMM_SetChnBit16(
                    pshwBufOut,
                    2,
                    AUDIO_ENHANCE_GetSpkOutR(AUDIO_ENHANCE_BUFF_OUT),
                    uwProcSize,
                    1);
        }
        else
        {
            /* 执行变采样 */
            AUDIO_ENHANCE_DoSrc(XA_SRC_VOIP_SPKOUT_16K_48K_L,
                                AUDIO_ENHANCE_GetSpkOutL(AUDIO_ENHANCE_BUFF_OUT),
                                AUDIO_ENHANCE_GetSpkOutL(AUDIO_ENHANCE_BUFF_SWAP),
                                &uwOutputBuffSize);

            AUDIO_ENHANCE_DoSrc(XA_SRC_VOIP_SPKOUT_16K_48K_R,
                                AUDIO_ENHANCE_GetSpkOutR(AUDIO_ENHANCE_BUFF_OUT),
                                AUDIO_ENHANCE_GetSpkOutR(AUDIO_ENHANCE_BUFF_SWAP),
                                &uwOutputBuffSize);

            /* 勾取左声道48K数据 */
            AUDIO_DEBUG_SendHookInd(
                        AUDIO_HOOK_ENHANCE_MLIB_SPKOUT_48K_L,
                        AUDIO_ENHANCE_GetSpkOutL(AUDIO_ENHANCE_BUFF_SWAP),
                        (uwBufSize/2));

            /* 勾取右声道48K数据 */
            AUDIO_DEBUG_SendHookInd(
                        AUDIO_HOOK_ENHANCE_MLIB_SPKOUT_48K_R,
                        AUDIO_ENHANCE_GetSpkOutR(AUDIO_ENHANCE_BUFF_SWAP),
                        (uwBufSize/2));

            /* 将处理后的数据由2路混合 */
            UCOM_COMM_SetChnBit16(
                    pshwBufOut,
                    2,
                    AUDIO_ENHANCE_GetSpkOutL(AUDIO_ENHANCE_BUFF_SWAP),
                    (uwBufSize/2),
                    0);

            UCOM_COMM_SetChnBit16(
                    pshwBufOut,
                    2,
                    AUDIO_ENHANCE_GetSpkOutR(AUDIO_ENHANCE_BUFF_SWAP),
                    (uwBufSize/2),
                    1);
        }

        OM_CPUVIEW_ExitArea((VOS_UCHAR)OM_CPUVIEW_AREA_AUDIO_ENHANCE_PROC_SPKOUT);
    }

    return uwRet;
}
VOS_UINT32 AUDIO_ENHANCE_ProcClear( AUDIO_ENHANCE_MODULE_ENUM_UINT32 enModule )
{
    AUDIO_ENHANCE_MODULE_REG_STRU      *pstModuleReg;

    /* 获取注册结构体 */
    pstModuleReg  = AUDIO_ENHANCE_GetModuleReg(enModule);

    MLIB_PathClear(pstModuleReg->uwMicInPathID);
    MLIB_PathClear(pstModuleReg->uwSpkOutPathID);

    return UCOM_RET_SUCC;
}
VOS_UINT32  AUDIO_ENHANCE_DoSrc(
                XA_SRC_PROC_ID_ENUM_UINT32 enProcId,
                VOS_VOID                  *pInputBuff,
                VOS_VOID                  *pOutputBuff,
                VOS_UINT32                *puwOutputSize)
{
    VOS_UINT32                          uwProcTimes;
    VOS_UINT32                          uwRet;
    AUDIO_ENHANCE_CTRL_STRU            *pstEnhanceCtrl;

    /* 获取控制结构体 */
    pstEnhanceCtrl  = AUDIO_ENHANCE_GetCtrl();

    /* 未初始化则停止工作 */
    if(0 == pstEnhanceCtrl->uwSrcRefFlag)
    {
        OM_LogError(AUDIO_ENHANCE_DoSrcError_UnInit);

        return VOS_ERR;
    }

    /* 根据enProcId采用不同的处理次数
     * 由48K转为16K，输入960个点。一次处理240个点，处理4次 */
    if (  (XA_SRC_VOIP_MICIN_48K_16K_L  == enProcId)
        ||(XA_SRC_VOIP_MICIN_48K_16K_R  == enProcId)
        ||(XA_SRC_VOIP_SPKOUT_48K_16K_L == enProcId)
        ||(XA_SRC_VOIP_SPKOUT_48K_16K_R == enProcId)
        ||(XA_SRC_VOIP_SMARTPA_48K_16K_L == enProcId)
        ||(XA_SRC_VOIP_SMARTPA_48K_16K_R == enProcId))
    {
        uwProcTimes = 4;
    }
    /* 由16K转为48K，输入320个点。一次处理160个点，处理2次 */
    else if (  (XA_SRC_VOIP_MICIN_16K_48K_L  == enProcId)
             ||(XA_SRC_VOIP_MICIN_16K_48K_R  == enProcId)
             ||(XA_SRC_VOIP_SPKOUT_16K_48K_L == enProcId)
             ||(XA_SRC_VOIP_SPKOUT_16K_48K_R == enProcId))
    {
        uwProcTimes = 2;
    }
    /* 待后续扩展 */
    else
    {
        OM_LogError1(AUDIO_ENHANCE_DoSrcError, enProcId);

        return VOS_ERR;
    }

    uwRet = XA_SRC_Proc8x(enProcId,
                          pInputBuff,
                          pOutputBuff,
                          puwOutputSize,
                          uwProcTimes);

    if (VOS_OK != uwRet)
    {
        OM_LogError1(AUDIO_ENHANCE_DoSrcError, uwRet);
    }

    return uwRet;
}


VOS_UINT32  AUDIO_ENHANCE_InitSrc(
                AUDIO_ENHANCE_MODULE_ENUM_UINT32    enModule,
                VOS_UINT32                          uwSampleRate  )
{
    AUDIO_ENHANCE_CTRL_STRU            *pstEnhanceCtrl;
    XA_SRC_USER_CFG_STRU                stMicIn_UP;
    XA_SRC_USER_CFG_STRU                stMicIn_DOWN;
    VOS_INT32                           swRet = VOS_OK;

    /* 获取控制结构体 */
    pstEnhanceCtrl  = AUDIO_ENHANCE_GetCtrl();

    /* 引用标记为0时进行初始化 */
    if(0 == pstEnhanceCtrl->uwSrcRefFlag)
    {
        /* 设置MicIn降采样 */
        stMicIn_DOWN.swChannels         = 1;
        stMicIn_DOWN.swInputChunkSize   = XA_SRC_INPUT_CHUNKSIZE_240;
        stMicIn_DOWN.swInputSmpRat      = 48000;
        stMicIn_DOWN.swOutputSmpRat     = (VOS_INT32)uwSampleRate;

        /* 设置MicIn升采样 */
        stMicIn_UP.swChannels           = 1;
        stMicIn_UP.swInputChunkSize     = XA_SRC_INPUT_CHUNKSIZE_160;
        stMicIn_UP.swInputSmpRat        = (VOS_INT32)uwSampleRate;
        stMicIn_UP.swOutputSmpRat       = 48000;

        /* 初始化MicIn左声道降采样 */
        swRet += XA_SRC_Init(XA_SRC_VOIP_MICIN_48K_16K_L, &stMicIn_DOWN);

        /* 初始化MicIn右声道降采样 */
        swRet += XA_SRC_Init(XA_SRC_VOIP_MICIN_48K_16K_R, &stMicIn_DOWN);

        /* 初始化MicIn左声道升采样 */
        swRet += XA_SRC_Init(XA_SRC_VOIP_MICIN_16K_48K_L, &stMicIn_UP);

        /* 初始化MicIn右声道升采样 */
        swRet += XA_SRC_Init(XA_SRC_VOIP_MICIN_16K_48K_R, &stMicIn_UP);


        /* 初始化SpkOut左声道降采样 */
        swRet += XA_SRC_Init(XA_SRC_VOIP_SPKOUT_48K_16K_L, &stMicIn_DOWN);

        /* 初始化SpkOut右声道降采样 */
        swRet += XA_SRC_Init(XA_SRC_VOIP_SPKOUT_48K_16K_R, &stMicIn_DOWN);

        /* 初始化SpkOut左声道升采样 */
        swRet += XA_SRC_Init(XA_SRC_VOIP_SPKOUT_16K_48K_L, &stMicIn_UP);

        /* 初始化SpkOut右声道升采样 */
        swRet += XA_SRC_Init(XA_SRC_VOIP_SPKOUT_16K_48K_R, &stMicIn_UP);

        /* 初始化SmartPA左声道降采样 */
        swRet += XA_SRC_Init(XA_SRC_VOIP_SMARTPA_48K_16K_L, &stMicIn_DOWN);

        /* 初始化SmartPA右声道降采样 */
        swRet += XA_SRC_Init(XA_SRC_VOIP_SMARTPA_48K_16K_R, &stMicIn_DOWN);

        if (VOS_OK != swRet)
        {
            OM_LogError1(AUDIO_ENHANCE_InitSrcError, swRet);

            return (VOS_UINT32)swRet;
        }
        else
        {
            OM_LogInfo(AUDIO_ENHANCE_InitSrcSucc);
        }
    }

    /* 设置使用标记 */
    pstEnhanceCtrl->uwSrcRefFlag |= (1 << enModule);

    OM_LogInfo1(AUDIO_ENHANCE_InitSrcFlag, pstEnhanceCtrl->uwSrcRefFlag);

    return VOS_OK;

}
VOS_VOID  AUDIO_ENHANCE_CloseSrc( AUDIO_ENHANCE_MODULE_ENUM_UINT32 enModule )
{
    AUDIO_ENHANCE_CTRL_STRU            *pstEnhanceCtrl;

    /* 获取控制结构体 */
    pstEnhanceCtrl  = AUDIO_ENHANCE_GetCtrl();

    /* 未初始化 */
    if(0 == pstEnhanceCtrl->uwSrcRefFlag)
    {
        OM_LogError(AUDIO_ENHANCE_CloseSrcError);

        return;
    }

    /* 将对应模块使用位置0 */
    pstEnhanceCtrl->uwSrcRefFlag &= (~(VOS_UINT32)(1<<enModule));

    if(0 == pstEnhanceCtrl->uwSrcRefFlag)
    {
        XA_SRC_Close(XA_SRC_VOIP_MICIN_48K_16K_L);
        XA_SRC_Close(XA_SRC_VOIP_MICIN_48K_16K_R);
        XA_SRC_Close(XA_SRC_VOIP_MICIN_16K_48K_L);
        XA_SRC_Close(XA_SRC_VOIP_MICIN_16K_48K_R);

        XA_SRC_Close(XA_SRC_VOIP_SPKOUT_48K_16K_L);
        XA_SRC_Close(XA_SRC_VOIP_SPKOUT_48K_16K_R);
        XA_SRC_Close(XA_SRC_VOIP_SPKOUT_16K_48K_L);
        XA_SRC_Close(XA_SRC_VOIP_SPKOUT_16K_48K_R);

        XA_SRC_Close(XA_SRC_VOIP_SMARTPA_48K_16K_L);
        XA_SRC_Close(XA_SRC_VOIP_SMARTPA_48K_16K_R);

        OM_LogInfo(AUDIO_ENHANCE_CloseSrcSucc);
    }

    OM_LogInfo1(AUDIO_ENHANCE_CloseSrcFlag, pstEnhanceCtrl->uwSrcRefFlag);
}
VOS_VOID  AUDIO_ENHANCE_SetExRefPort( AUDIO_ENHANCE_DEVICE_ENUM_UINT32 enDevice)
{
    VOICE_SMART_PA_CFG_NV_STRU          stPaNvCfg;
    AUDIO_PCM_CFG_STRU                 *pstPcmCfg;
    VOS_UINT32                          uwProcSize;
    AUDIO_ENHANCE_CTRL_STRU            *pstEnhanceCtrl;

    /* 获取控制结构体 */
    pstEnhanceCtrl  = AUDIO_ENHANCE_GetCtrl();

    UCOM_MemSet(&stPaNvCfg, 0, sizeof(VOICE_SMART_PA_CFG_NV_STRU));

    pstPcmCfg   = AUDIO_PcmGetPcmCfgPtr(AUDIO_PCM_MODE_PLAYBACK);

    /* 读取NV项 */
    UCOM_NV_Read(en_NV_SmartPACfg,
                 &stPaNvCfg,
                 sizeof(VOICE_SMART_PA_CFG_NV_STRU));

    /* 如果免提且SMART PA可用，这里配置SMART PA SIO */
    if (   (AUDIO_ENHANCE_DEVICE_HANDFREE == enDevice)
        && (CODEC_SWITCH_ON == stPaNvCfg.enEnable))
    {
        /* 计算需要处理的数据(单声道) */
        uwProcSize = AUDIO_ENHANCE_GetMonoFrmSize(pstPcmCfg->uwSampleRate);

        /* 免提模式下，启动SmartPA处下行播放链式DMA搬运 */
        UCOM_PCM_SmartPaStartLoopDma((VOS_UINT16)(uwProcSize * UCOM_PCM_I2S_CHANNEL_NUM),
                                     pstPcmCfg->uwSampleRate,
                                     stPaNvCfg.uhwIsMaster,
                                     UCOM_PCM_GetSmartPaDmacChn(),
                                     VOS_NULL);

        pstEnhanceCtrl->enExRef = AUDIO_ENHANCE_EX_REF_ON;
    }
    else
    {
        /* 关闭接受Smart PA的反馈信号 */
        UCOM_PCM_SmartPaStop();

        pstEnhanceCtrl->enExRef = AUDIO_ENHANCE_EX_REF_OFF;
    }

}


VOS_VOID AUDIO_ENHANCE_ProcEcRef( VOS_UINT32 uwSampleRate )
{
    AUDIO_ENHANCE_CTRL_STRU            *pstEnhanceCtrl;
    AUDIO_PCM_CFG_STRU                 *pstPcmCfg;
    VOS_INT16                          *pshwExRef;
    VOS_UINT32                          uwOutputBuffSize;

    /* 获取控制结构体 */
    pstEnhanceCtrl  = AUDIO_ENHANCE_GetCtrl();

    pstPcmCfg       = AUDIO_PcmGetPcmCfgPtr(AUDIO_PCM_MODE_PLAYBACK);

    if(AUDIO_ENHANCE_EX_REF_ON == pstEnhanceCtrl->enExRef)
    {
        pshwExRef = (VOS_INT16*)UCOM_PCM_SmartPaGetAvailVirtAddr();

        /* 将输入数据由混合数据拆分成2路 */
        UCOM_COMM_GetChnBit16(
                AUDIO_ENHANCE_GetSpkOutL(AUDIO_ENHANCE_BUFF_SWAP),
                AUDIO_ENHANCE_GetMonoFrmSize(pstPcmCfg->uwSampleRate),
                0,
                pshwExRef,
                2);

        UCOM_COMM_GetChnBit16(
                AUDIO_ENHANCE_GetSpkOutR(AUDIO_ENHANCE_BUFF_SWAP),
                AUDIO_ENHANCE_GetMonoFrmSize(pstPcmCfg->uwSampleRate),
                1,
                pshwExRef,
                2);

        /* 执行变采样 */
        AUDIO_ENHANCE_DoSrc(XA_SRC_VOIP_SMARTPA_48K_16K_L,
                            AUDIO_ENHANCE_GetSpkOutL(AUDIO_ENHANCE_BUFF_SWAP),
                            AUDIO_ENHANCE_GetSpkOutL(AUDIO_ENHANCE_BUFF_OUT),
                            &uwOutputBuffSize);

        AUDIO_ENHANCE_DoSrc(XA_SRC_VOIP_SMARTPA_48K_16K_R,
                            AUDIO_ENHANCE_GetSpkOutR(AUDIO_ENHANCE_BUFF_SWAP),
                            AUDIO_ENHANCE_GetSpkOutR(AUDIO_ENHANCE_BUFF_OUT),
                            &uwOutputBuffSize);

        /* 勾取左声道降为16K的数据 */
        AUDIO_DEBUG_SendHookInd(
                    AUDIO_HOOK_ENHANCE_MLIB_SMARTPA_16K_L,
                    AUDIO_ENHANCE_GetSpkOutL(AUDIO_ENHANCE_BUFF_OUT),
                    uwOutputBuffSize);

        /* 勾取右声道降为16K的数据 */
        AUDIO_DEBUG_SendHookInd(
                    AUDIO_HOOK_ENHANCE_MLIB_SMARTPA_16K_R,
                    AUDIO_ENHANCE_GetSpkOutR(AUDIO_ENHANCE_BUFF_OUT),
                    uwOutputBuffSize);
    }
}
VOS_UINT32  AUDIO_ENHANCE_GetCurrentModule( VOS_VOID )
{
    AUDIO_ENHANCE_CTRL_STRU *pstEnhanceCtrl;

    /* 获取控制结构体 */
    pstEnhanceCtrl  = AUDIO_ENHANCE_GetCtrl();

    return pstEnhanceCtrl->enCurrModule;
}


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

