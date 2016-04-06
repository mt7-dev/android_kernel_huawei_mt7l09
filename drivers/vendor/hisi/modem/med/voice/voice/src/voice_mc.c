

/*****************************************************************************
   1 头文件包含
******************************************************************************/
#include "OmCodecInterface.h"
#include "hifidrvinterface.h"
#include "ImsCodecInterface.h"

#include "codec_typedefine.h"
#include "ucom_comm.h"
#include "ucom_nv.h"
#include "ucom_mem_dyn.h"
#include "om.h"
#include "med_drv_sio.h"
#include "drv_mailbox.h"

#include "voice_log.h"
#include "voice_proc.h"
#include "voice_pcm.h"
#include "voice_mc.h"
#include "voice_debug.h"
#include "voice_amr_mode.h"
#include "voice_diagnose.h"

#include "med_drv_timer_hifi.h"
#include "VosPidDef.h"
#include "CodecInterface.h"
#include "mlib_interface.h"
#include "ucom_pcm.h"

#include "voice_jb_interface.h"

#ifdef  __cplusplus
#if  __cplusplus
extern "C"{
#endif
#endif

/*****************************************************************************
    可维可测信息中包含的C文件编号宏定义
*****************************************************************************/
/*lint -e(767)*/
#define THIS_FILE_ID                    OM_FILE_ID_VOICE_MC_C

/* 与VOICE交互的其他组件PID数量 */

#define PID_UNSET                       (0)

/* PID范围定义宏 */
#define MODEM0_BEGIN                    (WUEPS_PID_MAC)
#define MODEM0_END                      (PS_PID_IMSVA)

#define MODEM1_BEGIN                    (I1_WUEPS_PID_USIM)
#define MODEM1_END                      (I1_DSP_PID_APM)

#define CODEC_BEGIN                     (DSP_PID_VOICE_RESERVED)
#define CODEC_END                       (DSP_PID_HIFI_OM)

#define OM_BEGIN                        (WUEPS_PID_OM)
#define OM_END                          (WUEPS_PID_OM)


/*****************************************************************************
   2 全局变量定义
******************************************************************************/
/* PID转换表,卡0支持多模、卡一只支持G */
VOS_UINT32                              g_auwModemMcPidTbl[VOICE_MC_MODEM_NUM_BUTT][VOICE_MC_INDEX_BUTT]
                                     = {{I0_WUEPS_PID_VC,I0_DSP_PID_GPHY,
                                         DSP_PID_WPHY,DSP_PID_TDPHY,
                                         WUEPS_PID_MAC,TPS_PID_MAC,
                                         UCOM_PID_DSP_IMSA},
                                       {I1_WUEPS_PID_VC,I1_DSP_PID_GPHY,
                                        PID_UNSET,PID_UNSET,
                                        PID_UNSET,PID_UNSET,
                                        PID_UNSET}};

/* 语音软件模块运行状态全局变量 */
VOICE_MC_OBJS_STRU                      g_stVoiceMcObjs;

/*****************************************************************************
   3 外部全局变量声明
******************************************************************************/

/*****************************************************************************
   4 函数实现
******************************************************************************/

/*****************************************************************************
 函 数 名  : VOICE_McInit
 功能描述  : 语音软件的初始化，在使用语音软件的开始调用一次
 输入参数  : VOS_VOID
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年6月10日
    作    者   : 谢明辉 58441
    修改内容   : 修改函数注释模板

*****************************************************************************/
VOS_VOID VOICE_McInit(VOS_VOID)
{
    CODEC_NV_PARA_VOICE_TRACE_CFG_STRU  stVoiceTraceCfg;
    EN_OPENDSP_CONFIG_STRU              stOpenDspConfig;
    VOICE_MC_OBJS_STRU                 *pstObj         = VOICE_McGetMcObjsPtr();
    VOICE_MC_MODEM_STRU                *pstMcModemObj0 = VOICE_McGetModemObjPtr(VOICE_MC_MODEM0);
    VOICE_MC_MODEM_STRU                *pstMcModemObj1 = VOICE_McGetModemObjPtr(VOICE_MC_MODEM1);
    VOICE_DIAG_NV_STRU                  stVoiceDiagCfg;

    UCOM_MemSet(pstObj, 0, sizeof(VOICE_MC_OBJS_STRU));
    UCOM_MemSet(&stOpenDspConfig, 0, sizeof(EN_OPENDSP_CONFIG_STRU));

    /* 当前无前台通道号 */
    pstObj->enActiveModemNo = VOICE_MC_MODEM_NUM_BUTT;

    /* 初始化通路对象 */
    VOICE_McModemObjInit(pstMcModemObj0);
    VOICE_McModemObjInit(pstMcModemObj1);

    /* 读取SmartPA配置项 */
    UCOM_NV_Read(en_NV_SmartPACfg,
                 &(pstObj->stSmartPaCfg),
                 sizeof(pstObj->stSmartPaCfg));

    /* 设置当前Profile */
    UCOM_NV_Read(en_NV_OpenDSP_Config,
                &stOpenDspConfig,
                sizeof(EN_OPENDSP_CONFIG_STRU));

    MLIB_SelectProfile(stOpenDspConfig.ucProfile);

    /* AMR速率模式控制初始化 */
    VOICE_AmrModeCtrlInit();

    /* 信号处理初始化 */
    VOICE_ProcInit();

    /* 数据IO初始化 */
    VOICE_PcmInit();

    /* 读取VOICE TRACE相关NV项 */
    UCOM_NV_Read(en_NV_VoiceTraceCfg,
                 &stVoiceTraceCfg,
                 sizeof(stVoiceTraceCfg));

    /* 调试功能初始化 */
    VOICE_DbgInit(&stVoiceTraceCfg);

    /* 读取语音故障相关NV项 */
    UCOM_NV_Read(en_NV_VoiceDiagnoseCfg,
                 &stVoiceDiagCfg,
                 sizeof(stVoiceDiagCfg));

    /* 初始化语音质量检测模块 */
    VOICE_DiagInit(&stVoiceDiagCfg);

    #ifdef _MED_ERRORLOG
    /* 初始化ErrorLog */
    VOICE_ErrLogInit(&stVoiceTraceCfg.stErrlogCfg);
    #endif

    /* TD 5ms 子帧中断 */
    /* 根据V9R1 <<Hi6620V100 SOC逻辑需求规格表_DMA请求和中断分配.xlsx>>,这里为tds_stu_hifi_int的ID请求号 */
    VOS_ConnectInterrupt(DRV_TDSCDMA_CTRL_SYNC_INTR, VOICE_SyncTdSubFrm5msIsr);

    /* 初始化JB */
    VOICE_JB_Init();

    return;
}
VOS_VOID VOICE_McDestory( )
{
    /* 清理Proc */
    VOICE_ProcDestroy(VOICE_ProcGetObjsPtr());
}

#if 0
/*****************************************************************************
 函 数 名  : VOICE_McSndStopCallback
 功能描述  : 配置sound接口的回调函数
 输入参数  : VOS_VOID
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年6月10日
    作    者   : 谢明辉 58441
    修改内容   : 修改函数注释模板

*****************************************************************************/
VOS_VOID VOICE_McSndStopCallback(VOS_VOID)
{
    CODEC_INOUT_STRU                    *pstInout = VOICE_McGetInoutPtr();

    if(CODEC_INOUT_SOUND == pstInout->usTxIn)
    {
        pstInout->usTxIn = CODEC_INOUT_DEFAULT;
    }

    if(CODEC_INOUT_SOUND == pstInout->usRxIn)
    {
        pstInout->usRxIn = CODEC_INOUT_DEFAULT;
    }

    /* 上报VOICE_VOICE_VC_SOUND_END_IND原语,V3R2版本不实现SOUND功能 */

    return;
}

/*****************************************************************************
 函 数 名  : VOICE_McConfigSnd
 功能描述  : 配置sound接口的回调函数
 输入参数  : sTxVolume:      上行sound配置音量大小,单位db
             sRxVolume:      下行sound配置音量大小,单位db
             usSoundId:      sound序号
             usSoundLoopCnt: sound播放循环次数
 输出参数  : 无
 返 回 值  : VOICE_RET_OK          成功
             VOICE_RET_ERROR       失败
             VOICE_RET_ERR_PARAM   参数错误
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年6月10日
    作    者   : 谢明辉 58441
    修改内容   : 修改函数注释模板

*****************************************************************************/
VOS_UINT32 VOICE_McConfigSnd(
                VOS_INT16               sTxVolume,
                VOS_INT16               sRxVolume,
                VOS_UINT16              usSoundId,
                VOS_UINT16              usSoundLoopCnt)
{
    VOICE_PROC_SND_CFG_STRU          stSoundConfig;
    VOS_UINT32                          uwRet        = UCOM_RET_SUCC;
    VOS_UINT16                          usSoundSwich = CODEC_SWITCH_ON;
    CODEC_INOUT_STRU                     *pstInout     = VOICE_McGetInoutPtr();

    /* 根据输入判断是设置上行或下行音量 */
    if((CODEC_INOUT_SOUND == pstInout->usTxIn)
      && (CODEC_INOUT_SOUND == pstInout->usRxIn))
    {
        stSoundConfig.enTxRx = VOICE_TXRX_TXRX;
    }
    else if(CODEC_INOUT_SOUND == pstInout->usTxIn)
    {
        stSoundConfig.enTxRx = VOICE_TXRX_TX;
    }
    else if(CODEC_INOUT_SOUND == pstInout->usRxIn)
    {
        stSoundConfig.enTxRx = VOICE_TXRX_RX;
    }
    else
    {
        usSoundSwich = CODEC_SWITCH_OFF;
    }

    /* 设置循环次数和音量大小 */
    stSoundConfig.usRptCnt   = usSoundLoopCnt;
    stSoundConfig.usSndId    = usSoundId;
    stSoundConfig.sTxVolume  = sTxVolume;
    stSoundConfig.sRxVolume  = sRxVolume;
    stSoundConfig.funcSndInd = VOICE_McSndStopCallback;
    uwRet                    = VOICE_ProcConfigSound(usSoundSwich, &stSoundConfig);

    return uwRet;
}
#endif

/*****************************************************************************

 消息响应函数命名规则

 函 数 名  : VOICE_Msg[发送组件][消息缩写][I(dle)\R(unning)\P(ause)]
 功能描述  : [I\R\P]表示消息响应的语音软件运行主状态,可以组合使用
 输入参数  : VOS_VOID *pstOsaMsg
 返 回 值  : VOS_UINT32

*****************************************************************************/


VOS_UINT32 VOICE_MsgVcSetVolReqIRP(VOS_VOID *pstOsaMsg)
{
    VOS_UINT32                      uwRet;
    VOS_UINT32                      uwSenderPid;
    VOICE_MC_MODEM_NUM_ENUM_UINT16  enModemNo;              /* 通道号枚举 */
    VOICE_MC_MODEM_STRU            *pstModem   = VOS_NULL;
    VCVOICE_SET_VOLUME_REQ_STRU    *pstVolPrim = VOS_NULL;

    /*原语ID合法性判断由于已经有消息状态机保证，此处不做判断 */
    pstVolPrim  = (VCVOICE_SET_VOLUME_REQ_STRU*)pstOsaMsg;

    /* 根据senderPid获取通道号 */
    uwSenderPid = pstVolPrim->uwSenderPid;
    enModemNo   = VOICE_McGetModemNum(uwSenderPid);

    /* 对enModemNo进行保护 */
    if (VOICE_MC_MODEM_NUM_BUTT <= enModemNo)
    {
        OM_LogError(VOICE_GetModemNumError);

        return UCOM_RET_FAIL;
    }

    /* 获取通路对象 */
    pstModem    = VOICE_McGetModemObjPtr(enModemNo);

    /* 检查参数合法性 */
    if (   (pstVolPrim->enVolTarget >= VCVOICE_VOLUME_TARGET_BUTT)
        || (pstVolPrim->sVolValue   >  VOICE_PROC_GAIN_MAX)
        || (pstVolPrim->sVolValue   <  VOICE_PROC_GAIN_MIN))
    {
        uwRet = UCOM_RET_ERR_PARA;
    }
    else
    {
        /* 更新使能参数 */
        /* 上行 */
        if ( VCVOICE_VOLUME_TARGET_UP == pstVolPrim->enVolTarget )
        {
            pstModem->shwVolumeTx = pstVolPrim->sVolValue;
        }
        /* 下行 */
        else
        {
            pstModem->shwVolumeRx = pstVolPrim->sVolValue;
        }

        uwRet = UCOM_RET_SUCC;
    }

    if ( UCOM_RET_SUCC == uwRet)
    {
        OM_LogInfo(VOICE_MsgSetVolReq_Ok);
    }
    else
    {
        OM_LogError(VOICE_MsgSetVolReq_ParamError);
    }

    /*  回复消息ID_VOICE_VC_SET_VOLUME_CNF给NAS */
    VOICE_McSendCnfMsgToVc(ID_VOICE_VC_SET_VOLUME_CNF, uwSenderPid, uwRet);

    return uwRet;
}


VOS_UINT32 VOICE_MsgVcSetDevReqI(VOS_VOID *pstOsaMsg)
{
    VOS_UINT32                          uwRet;
    VOICE_MC_MODEM_NUM_ENUM_UINT16      enModemNo;              /* 通道号枚举 */
    VCVOICE_SET_DEVICE_REQ_STRU        *pstDevice;

    pstDevice = (VCVOICE_SET_DEVICE_REQ_STRU*)pstOsaMsg;

    /* 根据senderPid获取通道号 */
    enModemNo = VOICE_McGetModemNum(pstDevice->uwSenderPid);

    /* 对enModemNo进行保护 */
    if (VOICE_MC_MODEM_NUM_BUTT <= enModemNo)
    {
        OM_LogError(VOICE_GetModemNumError);

        return UCOM_RET_FAIL;
    }

    /* 设置设备模式 */
    uwRet = VOICE_McSetDev(pstDevice->usDeviceMode, VOS_FALSE, enModemNo);

    /* 回复NAS执行结果 */
    VOICE_McSendCnfMsgToVc(ID_VOICE_VC_SET_DEVICE_CNF, pstDevice->uwSenderPid, uwRet);

    return uwRet;
}
VOS_UINT32 VOICE_MsgVcSetDevReqRP(VOS_VOID *pstOsaMsg)
{
    VOS_UINT32                          uwRet;
    VOICE_MC_MODEM_NUM_ENUM_UINT16      enModemNo;              /* 通道号枚举 */
    VCVOICE_SET_DEVICE_REQ_STRU        *pstDevice;

    pstDevice = (VCVOICE_SET_DEVICE_REQ_STRU*)pstOsaMsg;

    /* 根据senderPid获取通道号 */
    enModemNo = VOICE_McGetModemNum(pstDevice->uwSenderPid);

    /* 对enModemNo进行保护 */
    if (VOICE_MC_MODEM_NUM_BUTT <= enModemNo)
    {
        OM_LogError(VOICE_GetModemNumError);

        return UCOM_RET_FAIL;
    }

    /* 设置设备模式 */
    uwRet = VOICE_McSetDev(pstDevice->usDeviceMode, VOS_TRUE, enModemNo);

    /* 回复NAS执行结果 */
    VOICE_McSendCnfMsgToVc(ID_VOICE_VC_SET_DEVICE_CNF, pstDevice->uwSenderPid, uwRet);

    return uwRet;
}
VOS_UINT32 VOICE_MsgVcStartReqI(VOS_VOID *pstOsaMsg)
{
    VOS_UINT32                      uwRet;
    VOS_UINT32                      uwJbStartRet = UCOM_RET_SUCC;
    VCVOICE_START_REQ_STRU         *pstMsg       = VOS_NULL;

    pstMsg      = (VCVOICE_START_REQ_STRU  *)pstOsaMsg;

    /* 开始语音处理 */
    uwRet = VOICE_McStart(pstMsg);

    /* LTE模式下，启动JB */
    if(CODEC_NET_MODE_L == pstMsg->enMode)
    {
        uwJbStartRet = VOICE_JB_Start(VCVOICE_TYPE_AMRWB);
        if(UCOM_RET_SUCC != uwJbStartRet)
        {
            OM_LogError(VOICE_MsgStartReq_JbInitFail);
        }
    }

    /* 回复NAS执行结果 */
    VOICE_McSendCnfMsgToVc(ID_VOICE_VC_START_CNF, pstMsg->uwSenderPid, uwRet);

    OM_LogInfo(VOICE_MsgStartReq_Ok);

    return uwRet;
}
VOS_UINT32 VOICE_MsgVcSetCodecReqR(VOS_VOID *pstOsaMsg)
{
    VOS_UINT32                          uwRet;
    VOICE_MC_MODEM_NUM_ENUM_UINT16      enModemNo;              /* 通道号枚举 */
    VCVOICE_SET_CODEC_REQ_STRU         *pstSetCodec;

    /*原语ID合法性判断由于已经有消息状态机保证，此处不做判断 */
    pstSetCodec = (VCVOICE_SET_CODEC_REQ_STRU*)pstOsaMsg;

    /* 根据senderPid获取通道号 */
    enModemNo   = VOICE_McGetModemNum(pstSetCodec->uwSenderPid);

    /* 对enModemNo进行保护 */
    if (VOICE_MC_MODEM_NUM_BUTT <= enModemNo)
    {
        OM_LogError(VOICE_GetModemNumError);

        return UCOM_RET_FAIL;
    }

    /* 设置声码器 */
    uwRet = VOICE_McSetCodec(pstSetCodec, enModemNo);

    /* 回复消息ID_VOICE_VC_SET_CODEC_CNF给NAS */
    VOICE_McSendCnfMsgToVc(ID_VOICE_VC_SET_CODEC_CNF, pstSetCodec->uwSenderPid, uwRet);

    /* 记录日志信息 */
    OM_LogInfo(VOICE_MsgSetCodecReq_Ok);

    return uwRet;
}
VOS_UINT32 VOICE_MsgVcSetCodecReqP(VOS_VOID *pstOsaMsg)
{
    VOS_UINT32                      uwRet;
    VOS_UINT32                      uwSenderPid;
    VOICE_MC_MODEM_NUM_ENUM_UINT16  enModemNo;              /* 通道号枚举 */
    VCVOICE_SET_CODEC_REQ_STRU     *pstSetCodec;

    /*原语ID合法性判断由于已经有消息状态机保证，此处不做判断 */
    pstSetCodec = (VCVOICE_SET_CODEC_REQ_STRU*)pstOsaMsg;

    /* 根据senderPid获取通道号 */
    uwSenderPid = pstSetCodec->uwSenderPid;
    enModemNo   = VOICE_McGetModemNum(uwSenderPid);

    /* 对enModemNo进行保护 */
    if (VOICE_MC_MODEM_NUM_BUTT <= enModemNo)
    {
        OM_LogError(VOICE_GetModemNumError);

        return UCOM_RET_FAIL;
    }

    /* 设置声码器 */
    uwRet = VOICE_McSetCodec(pstSetCodec, enModemNo);

    if (UCOM_RET_SUCC == uwRet)
    {
        /* 恢复语音为运行状态 */
        VOICE_McResume(pstSetCodec->enCodecType, enModemNo);
    }

    /* 回复消息ID_VOICE_VC_SET_CODEC_CNF给NAS */
    VOICE_McSendCnfMsgToVc(ID_VOICE_VC_SET_CODEC_CNF, uwSenderPid, uwRet);

    /* 记录日志信息 */
    OM_LogInfo(VOICE_MsgSetCodecReq_Ok);

    return uwRet;
}


VOS_UINT32 VOICE_MsgVcStopReqRP(VOS_VOID *pstOsaMsg)
{
    VOICE_MC_MODEM_NUM_ENUM_UINT16    enModemNo;
    VCVOICE_STOP_REQ_STRU            *pstStop;

    VOS_UINT32  uwRet = UCOM_RET_SUCC;

    /* 获取SenderPid */
    pstStop     = (VCVOICE_STOP_REQ_STRU *)pstOsaMsg;

    /* 根据SenderPid获取通道号 */
    enModemNo = VOICE_McGetModemNum(pstStop->uwSenderPid);

    /* 对enModemNo进行保护 */
    if (VOICE_MC_MODEM_NUM_BUTT <= enModemNo)
    {
        OM_LogError(VOICE_GetModemNumError);

        return UCOM_RET_FAIL;
    }

    /* 停止语音软件运行 */
    if (enModemNo == VOICE_McGetForeGroundNum())
    {
        VOICE_McStop(enModemNo);

        /* 设置当前激活Modem号 */
        VOICE_McSetForeGroundNum(VOICE_MC_MODEM_NUM_BUTT);
    }

    /* 用于语音可维可测，将当前modem的suspend时间戳置0 */
    g_auwSuspendBeginTs[enModemNo] = 0;

    /* 释放JB */
    VOICE_JB_Free();

    /* 语音对象初始化 */
    VOICE_McModemObjInit(VOICE_McGetModemObjPtr(enModemNo));

    /* 回复消息ID_VOICE_VC_STOP_CNF给NAS */
    VOICE_McSendCnfMsgToVc(ID_VOICE_VC_STOP_CNF, pstStop->uwSenderPid, uwRet);

    OM_LogInfo(VOICE_MsgStopReq_OK);

    return uwRet;
}
VOS_UINT32 VOICE_MsgOmSetNvReqRP(VOS_VOID *pstOsaMsg)
{
    VOS_UINT32                          uwRet;
    MSG_CODEC_CNF_STRU                  stRspPrim;
    MSG_OM_MLIB_PARA_SET_STRU          *pstOmPara;
    MLIB_PARA_STRU                     *pstMlibPara;
    VOS_UINT8                          *pucBuf;
    VOS_UINT32                          uwBufSize;

    pstOmPara = (MSG_OM_MLIB_PARA_SET_STRU*)pstOsaMsg;

    /* 分配数据报文的空间 */
    uwBufSize = sizeof(MLIB_PARA_STRU) + pstOmPara->uwSize;
    pucBuf = (VOS_UINT8*)UCOM_MemAlloc(uwBufSize);
    UCOM_MemSet(pucBuf, 0, uwBufSize);
    pstMlibPara = (MLIB_PARA_STRU*)pucBuf;

    /* 设置数据报文头 */
    pstMlibPara->uwChnNum     = 2;
    pstMlibPara->uwSampleRate = VOICE_ProcGetSampleRate();
    pstMlibPara->uwFrameLen   = ( (pstMlibPara->uwSampleRate == 16000) ? CODEC_FRAME_LENGTH_WB: CODEC_FRAME_LENGTH_NB);
    pstMlibPara->uwResolution = 16;
    pstMlibPara->enDevice     = VOICE_McGetDeviceMode();
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

    /* 向SDT回复消息ID_VOICE_OM_SET_NV_CNF */
    OM_COMM_SendTranMsg(&stRspPrim, sizeof(stRspPrim));

    /* 释放报文内存 */
    UCOM_MemFree(pucBuf);

    return uwRet;
}


VOS_UINT32 VOICE_MsgOmGetNvReqRP(VOS_VOID *pstOsaMsg)
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


VOS_UINT32 VOICE_MsgOmQueryStatusReqIRP(VOS_VOID *pstOsaMsg)
{
    VOS_UINT32                      uwRet    = UCOM_RET_SUCC;
    MSG_VOICE_OM_QUERY_RSP_STRU     stQueryMsg;
    VOICE_MC_MODEM_NUM_ENUM_UINT16  enActiveModemNo = VOICE_McGetForeGroundNum();
    MsgBlock                       *pstMsg    = VOS_NULL;
    VOICE_PROC_CODEC_STRU          *pstCodec;

    /* 无前台通路，不处理 */
    if (VOICE_MC_MODEM_NUM_BUTT <= enActiveModemNo)
    {
        return uwRet;
    }

    pstCodec    = VOICE_McGetModemCodecPtr(enActiveModemNo);
    pstMsg      = (MsgBlock*)pstOsaMsg;

    UCOM_MemSet(&stQueryMsg, 0, sizeof(stQueryMsg));

    stQueryMsg.usMsgId       = ID_VOICE_OM_QUERY_STATUS_CNF;
    stQueryMsg.usState       = VOICE_McGetVoiceState(pstMsg->uwSenderPid);
    stQueryMsg.enNetMode     = VOICE_McGetNetMode(pstMsg->uwSenderPid);
    stQueryMsg.enCodecType   = pstCodec->enCodecType;
    stQueryMsg.enDtxMode     = (VOS_UINT16)pstCodec->stEncInObj.enDtxMode;
    stQueryMsg.enAmrMode     = (VOS_UINT16)pstCodec->stEncInObj.enAmrMode;
    stQueryMsg.stInout       = *(VOICE_McGetInoutPtr());

    /*调用OM_COMM_SendTranMsg接口进行Trans头填充，并发送给SDT*/
    OM_COMM_SendTranMsg(&stQueryMsg, sizeof(MSG_VOICE_OM_QUERY_RSP_STRU));

    OM_LogInfo(VOICE_MsgQueryStatusReq_Ok);
    return uwRet;
}


VOS_UINT32 VOICE_MsgPhySuspendCmdR(VOS_VOID *pstOsaMsg)
{
    VOICE_MC_MODEM_NUM_ENUM_UINT16    enModemNo;
    MsgBlock                         *pstSuspend = VOS_NULL;

    /* 获取SenderPid */
    pstSuspend     = (MsgBlock *)pstOsaMsg;

    /* 根据SenderPid获取通道号 */
    enModemNo = VOICE_McGetModemNum(pstSuspend->uwSenderPid);

    /* 对enModemNo进行保护 */
    if (VOICE_MC_MODEM_NUM_BUTT <= enModemNo)
    {
        OM_LogError(VOICE_GetModemNumError);

        return UCOM_RET_FAIL;
    }

    /* 如果当前为前台，停止设备 */
    if (enModemNo == VOICE_McGetForeGroundNum())
    {
        /* 将语音相关设备停止 */
        VOICE_McStop(enModemNo);
    }

    /*将语音软件状态置为暂停*/
    VOICE_McSetVoiceState(enModemNo, VOICE_MC_STATE_PAUSE);

    /* 记录日志 */
    OM_LogInfo(VOICE_MsgSuspendCmd_OK);

    /* 重置ErrorLog计数器 */
    VOICE_ErrLogReset();

    /* 记录suspend消息时间戳 */
    g_auwSuspendBeginTs[enModemNo] = DRV_TIMER_ReadSysTimeStamp();

    return UCOM_RET_SUCC;
}
VOS_UINT32 VOICE_MsgWphySyncIndR(VOS_VOID *pstOsaMsg)
{
    VOS_UINT16                  uhwCfn;
    VOS_UINT32                  uwSyncOffset;
    VOS_UINT32                  uwRet      = UCOM_RET_FAIL;
    WPHY_VOICE_SYNC_IND_STRU   *pstSyncMsg = VOS_NULL;
    VOS_UINT32                  uwTimerLen;
    VOS_UINT16                  uhwModemNo;
    VOICE_MC_DMA_FLAG_STRU     *pstDmaFlag = VOICE_McGetDmaFlagPtr();
    VOICE_MC_TIMER_STRU        *pstTimer   = VOICE_McGetTimerPtr();
    VOS_UINT16                  uhwActiveModemNo = VOICE_McGetForeGroundNum();

    /* 按WPHY_VOICE_SYNC_IND_STRU结构进行消息解析*/
    pstSyncMsg  = (WPHY_VOICE_SYNC_IND_STRU*)pstOsaMsg;

    /* 获取通路号 */
    uhwModemNo  = VOICE_McGetModemNum(pstSyncMsg->uwSenderPid);

    /* 当前不在前台，不处理 */
    if (uhwActiveModemNo != uhwModemNo)
    {
        return UCOM_RET_SUCC;
    }

    /* 获取消息发送时刻的Cfn号 */
    uhwCfn      = pstSyncMsg->uhwCfn;

    /* 若该Sync消息为偶数帧发送则为异常 */
    if (0 == (uhwCfn % 2))
    {
        /* 记录异常 */
        uwRet = UCOM_RET_ERR_MSG;
    }
    else
    {
        /* 读取SOC计数器，获取当前时戳并保存*/
        pstDmaFlag->uwSyncTimestamp = DRV_TIMER_ReadSysTimeStamp();

        /* 计算消息发送时刻距离本帧帧头的距离并保存，单位0.1ms */
        uwSyncOffset = (((pstSyncMsg->uhwSlotOffset * VOICE_CHIP_NUMBER_BY_SLOT)
                + pstSyncMsg->uhwChipOffset) * 100) / VOICE_CHIP_NUMBER_BY_FRAME;

        /* 对于上行冲突检测，需要保存同步消息当前时刻 */
        pstDmaFlag->uwSyncOffset = uwSyncOffset;

        /*若上行DMA标志为未启动，则启动3ms定时器，产生UMTS上行DMA启动时刻*/
        if (CODEC_SWITCH_OFF == pstDmaFlag->usMicInRun)
        {
            /*定时器长度uwTimerLen = VOICE_UMTS_TX_BEGIN_TIMESTAMP - uwSyncOffset;*/
            uwTimerLen = VOICE_UMTS_TX_BEGIN_TIMESTAMP - uwSyncOffset;

            /*启动3ms定时器,并将定时器指针保存为g_stVoiceMcObjs.stTimer.pTimer3ms*/
            DRV_TIMER_Start(&(pstTimer->uwTimer3ms),
                            uwTimerLen,
                            DRV_TIMER_MODE_ONESHOT,
                            VOICE_Timer3msIsr,
                            0);
        }

        /*若下行DMA标志为未启动，则启动6ms定时器，产生UMTS下行DMA启动时刻*/
        if (CODEC_SWITCH_OFF == pstDmaFlag->usSpkOutRun)
        {
            /*定时器长度 = VOICE_UMTS_RX_BEGIN_TIMESTAMP - uwSyncOffset;*/
            uwTimerLen = VOICE_UMTS_RX_BEGIN_TIMESTAMP - uwSyncOffset;

            /*启动6ms定时器,并将定时器指针保存为g_stVoiceMcObjs.stTimer.uwTimer6ms*/
            DRV_TIMER_Start(&(pstTimer->uwTimer6ms),
                            uwTimerLen,
                            DRV_TIMER_MODE_ONESHOT,
                            VOICE_Timer6msIsr,
                            0);
        }

        uwRet = UCOM_RET_SUCC;
    }

    /* 记录日志 */
    OM_LogFreqInfo(VOICE_MsgWphySyncInd_Done, VOICE_LOG_PERIOD);

    return uwRet;
}


VOS_UINT32 VOICE_MsgTdSyncIndR(VOS_VOID *pstOsaMsg)
{
    VOS_UINT32                          uwTimerLen;
    VOS_UINT16                          uhwModemNo;
    VOS_UINT32                          uwSyncOffset = 0;                     /*当前收到中断的时间点和偶数帧起始时刻点一致*/
    VOICE_VOICE_TD_SYNC_IND_STRU       *pstSyncMsg;
    VOICE_MC_DMA_FLAG_STRU             *pstDmaFlag   = VOICE_McGetDmaFlagPtr();
    VOICE_MC_TIMER_STRU                *pstTimer     = VOICE_McGetTimerPtr();


    /* 按TDPHY_MED_SYNC_IND_STRU结构进行消息解析*/
    pstSyncMsg  = (VOICE_VOICE_TD_SYNC_IND_STRU*)pstOsaMsg;

    if (NULL == pstSyncMsg)
    {
        return UCOM_RET_ERR_STATE;
    }

    /* 获取通路号 */
    uhwModemNo  = VOICE_McGetModemNum(pstSyncMsg->uwSenderPid);

    /* 当前不在前台，不处理 */
    if (VOICE_MC_CODEC_RANGE != uhwModemNo)
    {
        return UCOM_RET_SUCC;
    }

    /*判断当前语音软件是否处于running态*/
    if (VOICE_MC_STATE_RUNNING != VOICE_McGetVoiceState(pstSyncMsg->uwSenderPid))
    {
        return UCOM_RET_ERR_STATE;
    }

    /* 读取SOC计数器，获取当前时戳并保存*/
    pstDmaFlag->uwSyncTimestamp = DRV_TIMER_ReadSysTimeStamp();
    pstDmaFlag->uwSyncOffset    = 0;

    /*若上行DMA标志为未启动，则启动定时器，产生TDS-CDMA上行DMA启动时刻*/
    if (CODEC_SWITCH_OFF == pstDmaFlag->usMicInRun)
    {
        /*定时器长度uwTimerLen = VOICE_TDS_TX_BEGIN_TIMESTAMP - uwSyncOffset;*/
        uwTimerLen = VOICE_TDS_TX_BEGIN_TIMESTAMP - uwSyncOffset;

        /*启动0.3ms定时器,并将定时器指针保存为g_stMedMcObjs.stTimer.uwTdTxTimer*/
        DRV_TIMER_Start(&(pstTimer->uwTdTxTimer),
                          uwTimerLen,
                          DRV_TIMER_MODE_ONESHOT,
                          VOICE_TdsTxTimerIsr,
                          0);
    }

    /*若下行DMA标志为未启动，则启动定时器，产生TDS-CDMA下行DMA启动时刻*/
    if (CODEC_SWITCH_OFF == pstDmaFlag->usSpkOutRun)
    {
        /*定时器长度 = VOICE_TDS_RX_BEGIN_TIMESTAMP - uwSyncOffset;*/
        uwTimerLen = VOICE_TDS_RX_BEGIN_TIMESTAMP - uwSyncOffset;

        /*启动8ms定时器,并将定时器指针保存为g_stMedMcObjs.stTimer.uwTdRxTimer*/
        DRV_TIMER_Start(&(pstTimer->uwTdRxTimer),
                          uwTimerLen,
                          DRV_TIMER_MODE_ONESHOT,
                          VOICE_TdsRxTimerIsr,
                          0);
    }

    return UCOM_RET_SUCC;
}


VOS_UINT32 VOICE_MsgWphyRxDataIndR(VOS_VOID *pstOsaMsg)
{
    WPHY_VOICE_RX_DATA_IND_STRU    *pstDecMsg        = VOS_NULL;
    VOS_UINT16                      uhwActiveModemNo = VOICE_McGetForeGroundNum();
    VOS_UINT16                      uhwModemNo;

    /* 按ID_WPHY_VOICE_RX_DATA_IND消息格式解析 */
    pstDecMsg = (WPHY_VOICE_RX_DATA_IND_STRU*)pstOsaMsg;

    /* 获取通路号 */
    uhwModemNo  = VOICE_McGetModemNum(pstDecMsg->uwSenderPid);

    /* 当前不在前台，不处理 */
    if (uhwActiveModemNo != uhwModemNo)
    {
        return UCOM_RET_SUCC;
    }

    /* 将新收到的数据压入解码缓存队列中 */
    VOICE_ProcAddDecSerialWrite(pstDecMsg, sizeof(WPHY_VOICE_RX_DATA_IND_STRU));

    #ifdef _MED_ERRORLOG
    /* 坏帧 */
    if (VOICE_ERRLOG_FQI_QUALITY_BAD == pstDecMsg->enQualityIdx)
    {
        /* ErrorLog 记录事件 */
        VOICE_ErrLogEventRec(CODEC_ERRLOG_EVENTS_WCDMA_BBP_DATA_BAD);
    }

    /* ErrorLog 记录事件 */
    VOICE_ErrLogEventRec(VOICE_ERRLOG_EVENTS_WCDMA_DEC_INT_RECEIVE);

    #endif

    return UCOM_RET_SUCC;
}
VOS_UINT32 VOICE_MsgGphyRxDataIndR(VOS_VOID *pstOsaMsg)
{
    GPHY_VOICE_RX_DATA_IND_STRU *pstDecMsg        = VOS_NULL;
    VOICE_PROC_CODEC_STRU       *pstCodec         = VOICE_ProcGetCodecPtr();
    VOS_UINT16                   uhwActiveModemNo = VOICE_McGetForeGroundNum();
    VOS_UINT16                   uhwModemNo;

    pstDecMsg   = (GPHY_VOICE_RX_DATA_IND_STRU*)pstOsaMsg;

    /* 获取通路号 */
    uhwModemNo  = VOICE_McGetModemNum(pstDecMsg->uwSenderPid);

    /* 当前不在前台，不处理 */
    if (uhwActiveModemNo != uhwModemNo)
    {
        OM_LogWarning(VOICE_MsgGphyRxDataInd_ModemMismatch);
        return UCOM_RET_SUCC;
    }

    /* 对enModemNo进行保护 */
    if (VOICE_MC_MODEM_NUM_BUTT <= uhwModemNo)
    {
        OM_LogError(VOICE_GetModemNumError);

        return UCOM_RET_FAIL;
    }

    /* 判断该帧声码器是否和SetCodec中的信息一致，若不一致则丢弃该帧信息 */
    if ( pstDecMsg->enCodecType != pstCodec->enCodecType )
    {
        OM_LogWarning(VOICE_MsgGphyRxDataInd_CodecTypeMismatch);
        VOICE_McSetGsmCodecTypeMismatch(uhwModemNo, VOICE_PROC_IS_YES);

        /* 不处理，直接返回 */
        return UCOM_RET_ERR_STATE;
    }
    else
    {
        VOICE_McSetGsmCodecTypeMismatch(uhwModemNo, VOICE_PROC_IS_NO);
    }

    /* 将新收到的数据压入解码缓存队列中 */
    VOICE_ProcAddDecSerialWrite(pstDecMsg, sizeof(GPHY_VOICE_RX_DATA_IND_STRU));

    return UCOM_RET_SUCC;
}


VOS_UINT32 VOICE_MsgTDphyRxDataIndR(VOS_VOID *pstOsaMsg)
{
    TDPHY_VOICE_RX_DATA_IND_STRU       *pstDecMsg        = MED_NULL;
    VOS_UINT16                          uhwActiveModemNo = VOICE_McGetForeGroundNum();
    VOS_UINT16                          uhwModemNo;

    /* 按ID_TDPHY_MED_RX_DATA_IND消息格式解析 */
    pstDecMsg   = (TDPHY_VOICE_RX_DATA_IND_STRU*)pstOsaMsg;

    /* 获取通道号 */
    uhwModemNo  = VOICE_McGetModemNum(pstDecMsg->uwSenderPid);

    /* 当前不在前台，不做处理 */
    if (uhwActiveModemNo != uhwModemNo)
    {
        return UCOM_RET_SUCC;
    }

    /*判断当前语音软件是否处于running态*/
    if (VOICE_MC_STATE_RUNNING != VOICE_McGetVoiceState(pstDecMsg->uwSenderPid))
    {
        return UCOM_RET_ERR_STATE;
    }

    /* 将新收到的数据压入解码缓存队列中 */
    VOICE_ProcAddDecSerialWrite(pstDecMsg, sizeof(TDPHY_VOICE_RX_DATA_IND_STRU));

    return UCOM_RET_SUCC;
}
VOS_UINT32 VOICE_MsgImsaRxDataIndR( VOS_VOID *pstOsaMsg )
{
    IMSA_VOICE_RX_DATA_IND_STRU        *pstImsaMsg       = MED_NULL;
    VOS_UINT16                          uhwActiveModemNo = VOICE_McGetForeGroundNum();
    VOS_UINT16                          uhwModemNo;
    VOS_UINT32                          uwRet;

    /* 按IMSA_VOICE_RX_DATA_IND消息格式解析 */
    pstImsaMsg = (IMSA_VOICE_RX_DATA_IND_STRU*)pstOsaMsg;

    /* 获取通道号 */
    uhwModemNo  = VOICE_McGetModemNum(pstImsaMsg->uwSenderPid);

    /* 当前不在前台，不做处理 */
    if (uhwActiveModemNo != uhwModemNo)
    {
        return UCOM_RET_SUCC;
    }

    /*判断当前语音软件是否处于running态*/
    if (VOICE_MC_STATE_RUNNING != VOICE_McGetVoiceState(pstImsaMsg->uwSenderPid))
    {
        return UCOM_RET_ERR_STATE;
    }

    /* 上下行编码参数配置 */
    uwRet = VOICE_ProcCfgImsCodec(pstImsaMsg->usCodecType,
                                  pstImsaMsg->usDtxEnable,
                                  pstImsaMsg->usRateMode);

    /* 声码器不匹配，作为坏帧处理 */
    if (UCOM_RET_SUCC != uwRet)
    {
        OM_LogWarning(VOICE_MsgImsaRxDataInd_CodecTypeMismatch);

        /* 置Qualification为bad */
        pstImsaMsg->usQualityIdx = CODEC_AMR_FQI_QUALITY_BAD;
    }

    /* 将包送给静态JB或AJB */
    VOICE_JB_AddPkt(pstOsaMsg);

    return uwRet;
}


VOS_UINT32 VOICE_MsgImsaCfgReqR( VOS_VOID *pstOsaMsg )
{
    IMSA_VOICE_CFG_REQ_STRU            *pstImsaCfgMsg    = MED_NULL;
    VOICE_IMSA_CFG_CNF_STRU             stImsaCfnMsg;
    VOS_UINT32                          uwRet;

    /* 按ID_IMSA_VOICE_CFG_REQ消息格式解析 */
    pstImsaCfgMsg = (IMSA_VOICE_CFG_REQ_STRU*)pstOsaMsg;

    if ((VOICE_JB_PKT_TIME_20MS == pstImsaCfgMsg->ulTransTime)
      ||(VOICE_JB_PKT_TIME_40MS == pstImsaCfgMsg->ulTransTime))
    {
        /* 设置打包时长 */
        uwRet = VOICE_JB_SetSjbPktTimeTx(pstImsaCfgMsg->ulTransTime );
        VOICE_JB_SetLtePktTime(pstImsaCfgMsg->ulTransTime);
    }
    else
    {
        uwRet = UCOM_RET_ERR_MSG;
    }

    /* 回复消息ID_VOICE_IMSA_CFG_CNF */
    stImsaCfnMsg.usMsgId    = ID_VOICE_IMSA_CFG_CNF;
    stImsaCfnMsg.ulResult   = uwRet;

    UCOM_SendOsaMsg(DSP_PID_VOICE_RT,
                    pstImsaCfgMsg->uwSenderPid,
                    &stImsaCfnMsg,
                    sizeof(VOICE_IMSA_CFG_CNF_STRU));

    return uwRet;
}
VOS_UINT32 VOICE_MsgGphyUpdateParaCmdRP(VOS_VOID *pstOsaMsg)
{
    VOS_UINT32                       uwRet;
    GPHY_VOICE_UPDATE_PARA_CMD_STRU *pstUpdateMsg;
    VOICE_PROC_CODEC_STRU           *pstCodec;
    VOS_UINT16                       uhwModemNo;

    /* 按GPHY_VOICE_UPDATE_PARA_CMD_STRU结构进行消息解析 */
    pstUpdateMsg = (GPHY_VOICE_UPDATE_PARA_CMD_STRU*)pstOsaMsg;

    /* 获取通路号 */
    uhwModemNo   = VOICE_McGetModemNum(pstUpdateMsg->uwSenderPid);

    /* 对enModemNo进行保护 */
    if (VOICE_MC_MODEM_NUM_BUTT <= uhwModemNo)
    {
        OM_LogError(VOICE_GetModemNumError);

        return UCOM_RET_FAIL;
    }

    /* 获取编解码对象 */
    pstCodec     = VOICE_McGetModemCodecPtr(uhwModemNo);

    /* 调用VOICE_ProcCfgGsmUlCodec根据内容进行编码参数更新 */
    uwRet = VOICE_ProcCfgGsmUlCodec(pstUpdateMsg,uhwModemNo);

    /* 若声码器类型不匹配，上报相应的警告信息 */
    if (pstUpdateMsg->enCodecType != pstCodec->enCodecType)
    {
        VOICE_McSetGsmCodecTypeMismatch(uhwModemNo,VOICE_PROC_IS_YES);
        OM_LogWarning(VOICE_MsgUpdateGsmParaCmd_CodecTypeMismatch);
    }
    else
    {
        VOICE_McSetGsmCodecTypeMismatch(uhwModemNo,VOICE_PROC_IS_NO);
        OM_LogInfo1(VOICE_MsgUpdateGsmParaCmd_Ok, uwRet);
    }

    return uwRet;

}


VOS_UINT32 VOICE_MsgGphyPlayIndR(VOS_VOID *pstOsaMsg)
{
    VOS_UINT32                          uwRet;
    VOICE_GPHY_PLAY_CNF_STRU            stCnfMsg;
    VOS_UINT16                          uhwNetMode;         /* 网络模式 */
    VOS_UINT16                          uhwModemNo;
    VOS_UINT16                          uhwActiveModemNo = VOICE_McGetForeGroundNum();
    MsgBlock                           *pstMsg           = VOS_NULL;

    /* 获取的SenderPid */
    pstMsg        = (MsgBlock *)pstOsaMsg;

    /* 获取通道号 */
    uhwModemNo    = VOICE_McGetModemNum(pstMsg->uwSenderPid);

    if (uhwActiveModemNo != uhwModemNo)
    {
        return UCOM_RET_SUCC;
    }

    /* 对enModemNo进行保护 */
    if (VOICE_MC_MODEM_NUM_BUTT <= uhwModemNo)
    {
        OM_LogError(VOICE_GetModemNumError);

        return UCOM_RET_FAIL;
    }

    UCOM_MemSet(&stCnfMsg, 0, sizeof(stCnfMsg));

    uhwNetMode = VOICE_McGetForeGroundNetMode(uhwActiveModemNo);

    if(   (CODEC_NET_MODE_G  == uhwNetMode)
       && (VOICE_PROC_IS_YES == VOICE_McGetGsmCodecTypeMismatch(uhwModemNo)))
    {
        OM_LogWarning(VOICE_MsgPlayInd_CodecTypeMismatch);
        uwRet = UCOM_RET_FAIL;
    }
    else
    {
        /* 启动语音播放 */
        uwRet = VOICE_McPlay();
    }

    /* 回复消息ID_VOICE_GPHY_PLAY_CNF */
    stCnfMsg.uhwMsgId = ID_VOICE_GPHY_PLAY_CNF;
    stCnfMsg.uwRslt   = uwRet;

    /* 回复消息ID_VOICE_GPHY_PLAY_CNF */
    UCOM_SendOsaMsg(DSP_PID_VOICE_RT,
                    pstMsg->uwSenderPid,
                    &stCnfMsg,
                    sizeof(VOICE_GPHY_PLAY_CNF_STRU));

    /* 记录日志 */
    OM_LogInfo(VOICE_MsgPlayInd_Done);

    return uwRet;
}


VOS_UINT32 VOICE_MsgGphyRecordIndR(VOS_VOID *pstOsaMsg)
{
    VOS_UINT32                  uwRet;
    VOICE_GPHY_RECORD_CNF_STRU  stCnfMsg;
    VOS_UINT16                  uhwNetMode;         /* 网络模式 */
    VOS_UINT16                  uhwModemNo;
    VOS_UINT16                  uhwActiveModemNo = VOICE_McGetForeGroundNum();
    MsgBlock                   *pstMsg      = VOS_NULL;

    /* 获取的SenderPid */
    pstMsg        = (MsgBlock *)pstOsaMsg;

    /* 获取通道号 */
    uhwModemNo    = VOICE_McGetModemNum(pstMsg->uwSenderPid);

    if (uhwActiveModemNo != uhwModemNo)
    {
        return UCOM_RET_SUCC;
    }

    /* 对enModemNo进行保护 */
    if (VOICE_MC_MODEM_NUM_BUTT <= uhwModemNo)
    {
        OM_LogError(VOICE_GetModemNumError);

        return UCOM_RET_FAIL;
    }

    UCOM_MemSet(&stCnfMsg, 0, sizeof(stCnfMsg));

    uhwNetMode = VOICE_McGetForeGroundNetMode(uhwActiveModemNo);
    if(   (CODEC_NET_MODE_G  == uhwNetMode)
       && (VOICE_PROC_IS_YES == VOICE_McGetGsmCodecTypeMismatch(uhwModemNo)))
    {
        OM_LogWarning(VOICE_MsgRecordInd_CodecTypeMismatch);
        uwRet = UCOM_RET_FAIL;
    }
    else
    {
        /* 启动语音采集 */
        uwRet = VOICE_McRecord();
    }

    /* 回复消息ID_VOICE_GPHY_RECORD_CNF */
    stCnfMsg.uhwMsgId = ID_VOICE_GPHY_RECORD_CNF;
    stCnfMsg.uwRslt   = uwRet;

    /* 回复消息ID_VOICE_GPHY_RECORD_CNF */
    UCOM_SendOsaMsg(DSP_PID_VOICE_RT,
                    pstMsg->uwSenderPid,
                    &stCnfMsg,
                    sizeof(VOICE_GPHY_RECORD_CNF_STRU));

    /* 记录日志 */
    OM_LogInfo(VOICE_MsgRecordInd_Done);

    return uwRet;
}



VOS_UINT32 VOICE_MsgGphySyncIndR(VOS_VOID *pstOsaMsg)
{
    VOS_UINT32                  uwSyncOffset;
    VOS_UINT32                  uwRet      = UCOM_RET_SUCC;
    GPHY_VOICE_SYNC_IND_STRU   *pstSyncMsg = VOS_NULL;
    VOS_UINT32                  uwTimerLen;
    VOICE_MC_DMA_FLAG_STRU     *pstDmaFlag = VOICE_McGetDmaFlagPtr();
    VOICE_MC_TIMER_STRU        *pstTimer   = VOICE_McGetTimerPtr();
    VOS_UINT32                  uwTxDataOffset;
    VOS_UINT16                  uhwModemNo;
    VOS_UINT16                  uhwActiveModemNo = VOICE_McGetForeGroundNum();

    /* 按GPHY_VOICE_SYNC_IND_STRU结构进行消息解析*/
    pstSyncMsg  = (GPHY_VOICE_SYNC_IND_STRU*)pstOsaMsg;

    /* 获取通路号 */
    uhwModemNo  = VOICE_McGetModemNum(pstSyncMsg->uwSenderPid);

    /* 当前不在前台，不处理 */
    if (uhwActiveModemNo != uhwModemNo)
    {
        return UCOM_RET_SUCC;
    }

    /* 读取SOC计数器，获取当前时戳并保存*/
    pstDmaFlag->uwSyncTimestamp = DRV_TIMER_ReadSysTimeStamp();

    /* 计算消息收到时刻距离消息中第12帧第0时隙的距离，单位0.1ms */
    uwSyncOffset = ((VOICE_GSM_QB_PARA * pstSyncMsg->uwQb) / (5000 * 100));

    /* 对于上行冲突检测，需要保存同步消息当前时刻和GPHY信道编码时刻 */
    pstDmaFlag->uwSyncOffset   = uwSyncOffset;
    pstDmaFlag->uwChanCodeTime = (VOS_UINT32)pstSyncMsg->uhwChanCodeTime;

    /* 计算GPHY要求上报上行数据的时刻偏差 */
    if(pstSyncMsg->uhwChanCodeTime <= (VOICE_GSM_RX_DMA_TIMESTAMP + uwSyncOffset))
    {
        uwTxDataOffset = pstSyncMsg->uhwChanCodeTime + VOICE_GSM_ONE_FRAME_OFFSET;
    }
    else
    {
        uwTxDataOffset = pstSyncMsg->uhwChanCodeTime;
    }

    /*若上行DMA标志为未启动，则启动上行定时器，产生GSM上行DMA启动时刻*/
    if (CODEC_SWITCH_OFF == pstDmaFlag->usMicInRun)
    {
        /*定时器长度*/
        uwTimerLen = (uwTxDataOffset - VOICE_GSM_TX_DMA_TIMESTAMP) - uwSyncOffset;

        /*启动3ms定时器,并将定时器指针保存为g_stVoiceMcObjs.stTimer.pTimer3ms*/
        DRV_TIMER_Start(&(pstTimer->uwTimer3ms),
                        uwTimerLen,
                        DRV_TIMER_MODE_ONESHOT,
                        VOICE_Timer3msIsr,
                        0);

    }

    /*若下行DMA标志为未启动，则启动20ms定时器，产生GSM下行DMA启动时刻*/
    if (CODEC_SWITCH_OFF == pstDmaFlag->usSpkOutRun)
    {
        /*定时器长度*/
        uwTimerLen = (uwTxDataOffset - VOICE_GSM_RX_DMA_TIMESTAMP) - uwSyncOffset;

        /*启动20ms定时器,并将定时器指针保存为g_stVoiceMcObjs.stTimer.uwTimer6ms*/
        DRV_TIMER_Start(&(pstTimer->uwTimer6ms),
                        uwTimerLen,
                        DRV_TIMER_MODE_ONESHOT,
                        VOICE_Timer6msIsr,
                        0);
    }

    /* 记录日志 */
    OM_LogInfo(VOICE_MsgGphySyncInd_Done);

    return uwRet;
}


VOS_UINT32 VOICE_MsgGphyChannelQualityIndR( VOS_VOID *pstOsaMsg )
{
    GPHY_VOICE_CHANNEL_QUALITY_IND_STRU *pstChannelQualityMsg = VOS_NULL;
    VOS_UINT16                           uhwActiveModemNo     = VOICE_McGetForeGroundNum();
    VOS_UINT16                           uhwModemNo;
    VOICE_DIAG_NV_STRU                  *pstDiagCfg           = VOICE_DiagGetCfgPtr();
    VOICE_DIAG_CHANNEL_QUALITY_STRU     *pstDiagChanQa        = VOICE_DiagGetChannelQualityPtr();
    VOS_UINT32                           uwDiagChanQaSize;
    VOS_UINT32                           uwOsaMsgTextSize;
    VOS_INT16                            shwGSMChanQaLev;

    /* 清空语音故障检测模块中保存的信道质量信息 */
    UCOM_MemSet(pstDiagChanQa, 0, sizeof(VOICE_DIAG_CHANNEL_QUALITY_STRU));

    pstChannelQualityMsg = (GPHY_VOICE_CHANNEL_QUALITY_IND_STRU*)pstOsaMsg;
    uwOsaMsgTextSize     = sizeof(GPHY_VOICE_CHANNEL_QUALITY_IND_STRU) - VOS_MSG_HEAD_LENGTH;

    /* 获取通路号 */
    uhwModemNo  = VOICE_McGetModemNum(pstChannelQualityMsg->uwSenderPid);

    /* 当前不在前台，不处理 */
    if (uhwActiveModemNo != uhwModemNo)
    {
        return UCOM_RET_SUCC;
    }

    /* 对enModemNo进行保护 */
    if (VOICE_MC_MODEM_NUM_BUTT <= uhwModemNo)
    {
        OM_LogError(VOICE_GetModemNumError);

        return UCOM_RET_FAIL;
    }

    /* 检测当前信号质量 */
    shwGSMChanQaLev = (VOS_INT16)pstChannelQualityMsg->uhwRxLevVal - VOICE_DIAG_GSM_RX_LEVEL_OFFSET;

    if(shwGSMChanQaLev <= pstDiagCfg->stChanQaPara.shwGsmChanQaThd)
    {
        pstDiagChanQa->uhwIsBadCell = VOICE_PROC_IS_YES;

        OM_LogWarning(VOICE_MsgGphyChannelQualityIndR_BadCell);
    }

    /* 物理层上报消息大小不能超过预留大小 */
    uwDiagChanQaSize = VOICE_DIAG_CHANNEL_QUALITY_LEN * sizeof(VOS_UINT32);

    if(uwOsaMsgTextSize > uwDiagChanQaSize)
    {
        OM_LogError(VOICE_MsgGphyChannelQualityIndR_Overflow);

        return UCOM_RET_FAIL;
    }

    /* 保存GPHY信道质量信息，用于语音故障检测 */
    UCOM_MemCpy(pstDiagChanQa->auwChannelQuality,
                &pstChannelQualityMsg->uhwMsgId,
                uwOsaMsgTextSize);

    return UCOM_RET_SUCC;

}
VOS_UINT32 VOICE_MsgWphyChannelQualityIndR( VOS_VOID *pstOsaMsg  )
{
    WPHY_VOICE_CHANNEL_QUALITY_IND_STRU *pstChannelQualityMsg = VOS_NULL;
    VOS_UINT16                           uhwActiveModemNo     = VOICE_McGetForeGroundNum();
    VOS_UINT16                           uhwModemNo;
    VOICE_DIAG_NV_STRU                  *pstDiagCfg           = VOICE_DiagGetCfgPtr();
    VOICE_DIAG_CHANNEL_QUALITY_STRU     *pstDiagChanQa        = VOICE_DiagGetChannelQualityPtr();
    VOS_UINT32                           uwDiagChanQaSize;
    VOS_UINT32                           uwOsaMsgTextSize;
    VOS_INT16                            shwUMTSChanQaLev;
    VOS_UINT16                           uhwServCellId;

    /* 清空语音故障检测模块中保存的信道质量信息 */
    UCOM_MemSet(pstDiagChanQa, 0, sizeof(VOICE_DIAG_CHANNEL_QUALITY_STRU));

    pstChannelQualityMsg = (WPHY_VOICE_CHANNEL_QUALITY_IND_STRU*)pstOsaMsg;
    uwOsaMsgTextSize     = sizeof(WPHY_VOICE_CHANNEL_QUALITY_IND_STRU) - VOS_MSG_HEAD_LENGTH;

    /* 获取通路号 */
    uhwModemNo  = VOICE_McGetModemNum(pstChannelQualityMsg->uwSenderPid);

    /* 当前不在前台，不处理 */
    if (uhwActiveModemNo != uhwModemNo)
    {
        return UCOM_RET_SUCC;
    }

    /* 对enModemNo进行保护 */
    if (VOICE_MC_MODEM_NUM_BUTT <= uhwModemNo)
    {
        OM_LogError(VOICE_GetModemNumError);

        return UCOM_RET_FAIL;
    }

    /* 检测当前信道质量 */
    uhwServCellId = pstChannelQualityMsg->uhwServCellId;
    if(uhwServCellId >= WPHY_VOICE_WCDMA_MAX_CELL_NUM)
    {
        OM_LogError(VOICE_MsgWphyChannelQualityIndR_ServCellId_Err);

        return UCOM_RET_FAIL;
    }

    shwUMTSChanQaLev = pstChannelQualityMsg->astCellInfo[uhwServCellId].shwCpichEcN0;

    if((shwUMTSChanQaLev >> 3) <= pstDiagCfg->stChanQaPara.shwUmtsChanQaThd)
    {
        pstDiagChanQa->uhwIsBadCell = VOICE_PROC_IS_YES;

        OM_LogWarning(VOICE_MsgWphyChannelQualityIndR_BadCell);
    }

    uwDiagChanQaSize = VOICE_DIAG_CHANNEL_QUALITY_LEN * sizeof(VOS_UINT32);

    /* 物理层上报消息大小不能超过预留大小 */
    if(uwOsaMsgTextSize > uwDiagChanQaSize)
    {
        OM_LogError(VOICE_MsgWphyChannelQualityIndR_Overflow);

        return UCOM_RET_FAIL;
    }

    /* 保存WPHY信道质量信息，用于语音故障检测 */
    UCOM_MemCpy(pstDiagChanQa->auwChannelQuality,
                &pstChannelQualityMsg->uhwMsgId,
                uwOsaMsgTextSize);

    return UCOM_RET_SUCC;
}
VOS_UINT32 VOICE_MsgVoicePlayIndR(VOS_VOID *pstOsaMsg)
{
    VOS_UINT32  uwRet;

    /* 启动语音播放 */
    uwRet = VOICE_McPlay();

    /* 记录日志 */
    OM_LogInfo(VOICE_MsgPlayInd_Done);

    return uwRet;
}


VOS_UINT32 VOICE_MsgVoiceRecordIndR(VOS_VOID *pstOsaMsg)
{
    VOS_UINT32  uwRet;

    /* 启动语音采集 */
    uwRet = VOICE_McRecord();

    /* 记录日志 */
    OM_LogInfo(VOICE_MsgRecordInd_Done);

    return uwRet;
}


VOS_UINT32 VOICE_MsgVoiceTxProcReqR(VOS_VOID *pstOsaMsg)
{
    VOS_UINT32           uwRet;

    /* 记录已经收到了编码消息,用于监控语音流程 */
    g_stDiagDmaIsr.uhwTxDmaIsrFlag = VOICE_DIAG_TRUE;

    /* 语音上行处理前的准备工作, 例如AMR速率调整 */
    uwRet = VOICE_McPreTxProc();
    if(uwRet != UCOM_RET_SUCC)
    {
        return uwRet;

    }

    /* 将I2S左右声道合并数据分解回左右声道 */
    uwRet = VOICE_PcmHybrid2Stereo(g_psVoicePcmMicIn,
                                   VOICE_PcmGetMcInBufPtr(),
                                   VOICE_PcmGetRcInBufPtr(),
                                   CODEC_PCM_FRAME_LENGTH);
    if (UCOM_RET_SUCC != uwRet)
    {
        return  UCOM_RET_FAIL;
    }

    /* 语音上行编码 */
    uwRet = VOICE_McTxEnc();
    if (UCOM_RET_SUCC != uwRet)
    {
        return  UCOM_RET_FAIL;
    }

    /* 发送上行编码结果 */
    VOICE_McPostTxProc();


    /* 语音上行PP处理 */
    uwRet = VOICE_McTxProc();
    if(uwRet != UCOM_RET_SUCC)
    {
        return uwRet;

    }

    return uwRet;
}


VOS_UINT32 VOICE_MsgVoiceRxProcReqR(VOS_VOID *pstOsaMsg)
{
    VOS_UINT32 uwRet;

    /* 记录已经收到了解码消息,用于监控语音流程 */
    g_stDiagDmaIsr.uhwRxDmaIsrFlag = VOICE_DIAG_TRUE;

    /* 本帧下行处理 */
    uwRet = VOICE_McRxProc();

    return uwRet;
}


VOS_UINT32 VOICE_MsgVcForeGroundReqRP( VOS_VOID *pstOsaMsg  )
{
    VOS_UINT32                       uwRet;
    VOS_UINT32                       uwSenderPid;                /* nas的PID */
    VOICE_MC_MODEM_NUM_ENUM_UINT16   enModemNo;
    CODEC_NET_MODE_ENUM_UINT16       enNetMode;
    VOICE_PROC_CODEC_STRU           *pstCodec;
    VCVOICE_FOREGROUND_REQ_STRU     *pstForeGroundReq = VOS_NULL;
    VOICE_PROC_OBJS_STRU            *pstProc          = VOICE_ProcGetObjsPtr();
    VOICE_MC_DMA_FLAG_STRU          *pstDmaFlag = VOICE_McGetDmaFlagPtr();
    VOS_UINT32                       uwUlTimer;
    VOS_UINT32                       uwDlTimer;

    /* 获取senderPid */
    pstForeGroundReq = (VCVOICE_FOREGROUND_REQ_STRU*)pstOsaMsg;
    uwSenderPid      = pstForeGroundReq->uwSenderPid;

    /* 根据senderPid获取通道号 */
    enModemNo = VOICE_McGetModemNum(uwSenderPid);

    /* 对enModemNo进行保护 */
    if (VOICE_MC_MODEM_NUM_BUTT <= enModemNo)
    {
        OM_LogError(VOICE_GetModemNumError);

        return UCOM_RET_FAIL;
    }

    /* 根据通道号,获取对应modem对象 */
    enNetMode     = VOICE_McGetModemNetMode(enModemNo);

    /* 已经在foreground下,回复nas成功 */
    if(enModemNo == VOICE_McGetForeGroundNum())
    {
        VOICE_McSendCnfMsgToVc(ID_VOICE_VC_FOREGROUND_CNF,
                               uwSenderPid,
                               VCVOICE_EXECUTE_RSLT_SUCC);

        return UCOM_RET_SUCC;
    }

    /* 另一个还处于前台状态，将另一路设置为后台 */
    if(VOICE_MC_MODEM_NUM_BUTT > VOICE_McGetForeGroundNum())
    {
        VOICE_McSetBackGround(VOICE_McGetForeGroundNum());
    }

    /* PROC全局变量中的codec指针置为此通路的codec */
    pstProc->pstCodec = VOICE_McGetModemCodecPtr(enModemNo);

    /* 更新PMC和IO通道，更新PP处理参数 */
    uwRet = VOICE_McUpdate(enModemNo);

    /* 回给nas失败 */
    if(UCOM_RET_SUCC != uwRet)
    {
        pstProc->pstCodec = VOS_NULL;

        VOICE_McSendCnfMsgToVc(ID_VOICE_VC_FOREGROUND_CNF,
                               uwSenderPid,
                               VCVOICE_EXECUTE_RSLT_FAIL);

        return UCOM_RET_FAIL;
    }

    /* TD起5ms帧同步中断 */
    if(CODEC_NET_MODE_TD == enNetMode)
    {
        /* 根据V9R1 <<Hi6620V100 SOC逻辑需求规格表_DMA请求和中断分配.xlsx>>,这里为tds_stu_hifi_int的ID请求号 */
        VOS_EnableInterrupt(DRV_TDSCDMA_CTRL_SYNC_INTR);

        /* 使能屏蔽寄存器，使其状态为不屏蔽状态 */
        UCOM_RegBitWr(DRV_TDSCDMA_CTRL_EN_ADDR, UCOM_BIT0, UCOM_BIT0, 1);
    }

    /* 起上下行DMA */
    if(CODEC_NET_MODE_L == enNetMode)
    {
        /*若下行DMA标志为未启动，则启动5ms定时器，产生IMS下行DMA启动时刻*/
        if (CODEC_SWITCH_OFF == pstDmaFlag->usSpkOutRun)
        {
            /* 启动下行DMA */
            DRV_TIMER_Start(&uwDlTimer,
                            VOICE_IMS_RX_BEGIN_TIMESTAMP,
                            DRV_TIMER_MODE_ONESHOT,
                            VOICE_Timer6msIsr,
                            0);
        }

        /*若上行DMA标志为未启动，则启动4ms定时器，产生IMS上行DMA启动时刻*/
        if (CODEC_SWITCH_OFF == pstDmaFlag->usMicInRun)
        {
            /* 启动上行DMA */
            DRV_TIMER_Start(&uwUlTimer,
                            VOICE_IMS_TX_BEGIN_TIMESTAMP,
                            DRV_TIMER_MODE_ONESHOT,
                            VOICE_Timer3msIsr,
                            0);
        }

        /* IMS下语音编码参数更新,默认速率wb为23.05，nb为12.2 */
        VOICE_ProcCfgImsCodec((pstProc->pstCodec)->enCodecType,CODEC_DTX_DISABLE,CODEC_AMR_RATE_MODE_122K);
    }

    /* 设置通路状态为前台 */
    VOICE_McSetModemState(enModemNo, VOICE_MC_MODEM_STATE_FOREGROUND);

    /* 设置当前激活Modem号 */
    VOICE_McSetForeGroundNum(enModemNo);

    /* FA偷帧检测计数清零 */
    VOICE_ProcFacchFrameCntReset();

    /* 将声码器上下行初始化标志设置为未初始化 */
    pstCodec = VOICE_ProcGetCodecPtr();
    pstCodec->usIsDecInited = VOICE_PROC_IS_NO;
    pstCodec->usIsEncInited = VOICE_PROC_IS_NO;

    /* 通知物理层退出静默 */
    VOICE_McModemInformToPhy(enNetMode,
                            enModemNo,
                            VOICE_MC_FOREGROUND_OPT);

    /* 给对应nas发CNF消息 */
    VOICE_McSendCnfMsgToVc(ID_VOICE_VC_FOREGROUND_CNF,
                           uwSenderPid,
                           VCVOICE_EXECUTE_RSLT_SUCC);

    return UCOM_RET_SUCC;
}
VOS_UINT32 VOICE_MsgVcBackGroundReqRP( VOS_VOID *pstOsaMsg  )
{
    VOICE_MC_MODEM_NUM_ENUM_UINT16   enModemNo;
    VOICE_MC_MODEM_STATE_ENUM_UINT16 enModemState;
    VCVOICE_BACKGROUND_REQ_STRU     *pstBackGroundReq = VOS_NULL;


    /* 获取senderPid */
    pstBackGroundReq = (VCVOICE_BACKGROUND_REQ_STRU*)pstOsaMsg;

    /* 根据senderPid获取通道号 */
    enModemNo     = VOICE_McGetModemNum(pstBackGroundReq->uwSenderPid);

    /* 对enModemNo进行保护 */
    if (VOICE_MC_MODEM_NUM_BUTT <= enModemNo)
    {
        OM_LogError(VOICE_GetModemNumError);

        return UCOM_RET_FAIL;
    }

    /* 根据通道号,获取对应modem对象 */
    enModemState  = VOICE_McGetModemState(enModemNo);

    /* 已经在background下,回复nas成功 */
    if( VOICE_MC_MODEM_STATE_BACKGROUND == enModemState )
    {
        VOICE_McSendCnfMsgToVc(ID_VOICE_VC_BACKGROUND_CNF, pstBackGroundReq->uwSenderPid, VCVOICE_EXECUTE_RSLT_SUCC);

        return UCOM_RET_SUCC;
    }

    /* 设置为后台模式 */
    VOICE_McSetBackGround(enModemNo);

    /* 给对应nas发CNF消息 */
    VOICE_McSendCnfMsgToVc(ID_VOICE_VC_BACKGROUND_CNF, pstBackGroundReq->uwSenderPid, VCVOICE_EXECUTE_RSLT_SUCC);

    return UCOM_RET_SUCC;
}
VOS_UINT32 VOICE_MsgVcGroundQryReqRP( VOS_VOID *pstOsaMsg )
{
    VOICE_MC_MODEM_NUM_ENUM_UINT16   enModemNo;
    VOICE_MC_MODEM_STATE_ENUM_UINT16 enModemState;
    VCVOICE_GROUND_QRY_STRU         *pstGroundQry = VOS_NULL;

    /* 获取senderPid */
    pstGroundQry     = (VCVOICE_GROUND_QRY_STRU*)pstOsaMsg;

    /* 根据senderPid获取通道号 */
    enModemNo        = VOICE_McGetModemNum(pstGroundQry->uwSenderPid);

    /* 对enModemNo进行保护 */
    if (VOICE_MC_MODEM_NUM_BUTT <= enModemNo)
    {
        OM_LogError(VOICE_GetModemNumError);

        return UCOM_RET_FAIL;
    }

    /* 根据通道号,获取对应modem对象 */
    enModemState     = VOICE_McGetModemState(enModemNo);

    /* 通路状态通知NAS */
    VOICE_McSendQryCnfMsgToVc(ID_VOICE_VC_GROUND_RSP, pstGroundQry->uwSenderPid, enModemState);

    return UCOM_RET_SUCC;
}
VOS_UINT32 VOICE_MsgGUPhyBackGroundCnfRP( VOS_VOID *pstOsaMsg )
{
    DSP_VOICE_RESULT_ENUM_UINT16     enResult;
    GUPHY_VOICE_BACKGROUND_CNF_STRU *pstCnf = VOS_NULL;

    pstCnf   = (GUPHY_VOICE_BACKGROUND_CNF_STRU *)pstOsaMsg;

    enResult = pstCnf->enResult;

    if(DSP_VOICE_RESULT_SUCC == enResult)
    {
        OM_LogInfo(VOICE_MsgPhyBackGroundCnf_SUCC);
    }
    else
    {
        OM_LogError1(VOICE_MsgPhyBackGroundCnf_FAIL, pstCnf->uwSenderPid);
    }

    return UCOM_RET_SUCC;
}


VOS_UINT32 VOICE_MsgGUPhyForeGroundCnfRP( VOS_VOID *pstOsaMsg )
{
    DSP_VOICE_RESULT_ENUM_UINT16     enResult;
    GUPHY_VOICE_FOREGROUND_CNF_STRU *pstCnf = VOS_NULL;

    pstCnf   = (GUPHY_VOICE_FOREGROUND_CNF_STRU *)pstOsaMsg;

    enResult = pstCnf->enResult;

    if(DSP_VOICE_RESULT_SUCC == enResult)
    {
        OM_LogInfo(VOICE_MsgPhyForeGroundCnf_SUCC);
    }
    else
    {
        OM_LogError1(VOICE_MsgPhyForeGroundCnf_FAIL, pstCnf->uwSenderPid);
    }

    return UCOM_RET_SUCC;
}


VOS_UINT32 VOICE_MsgTdPhyBackGroundCnfRP( VOS_VOID *pstOsaMsg )
{
    TDPHY_VOICE_RESULT_ENUM_UINT16   enResult;
    TDPHY_VOICE_BACKGROUND_CNF_STRU *pstCnf = VOS_NULL;

    pstCnf   = (TDPHY_VOICE_BACKGROUND_CNF_STRU *)pstOsaMsg;

    enResult = pstCnf->enResult;

    if(TDPHY_VOICE_RESULT_SUCC == enResult)
    {
        OM_LogInfo(VOICE_MsgPhyBackGroundCnf_SUCC);
    }
    else
    {
        OM_LogError1(VOICE_MsgPhyBackGroundCnf_FAIL, pstCnf->uwSenderPid);
    }

    return UCOM_RET_SUCC;
}


VOS_UINT32 VOICE_MsgTdPhyForeGroundCnfRP( VOS_VOID *pstOsaMsg )
{
    TDPHY_VOICE_RESULT_ENUM_UINT16   enResult;
    TDPHY_VOICE_FOREGROUND_CNF_STRU *pstCnf = VOS_NULL;

    pstCnf   = (TDPHY_VOICE_FOREGROUND_CNF_STRU *)pstOsaMsg;

    enResult = pstCnf->enResult;

    if(TDPHY_VOICE_RESULT_SUCC == enResult)
    {
        OM_LogInfo(VOICE_MsgPhyForeGroundCnf_SUCC);
    }
    else
    {
        OM_LogError1(VOICE_MsgPhyForeGroundCnf_FAIL, pstCnf->uwSenderPid);
    }

    return UCOM_RET_SUCC;
}

/*****************************************************************************
 函 数 名  : VOICE_McSendQryCnfMsgToVc
 功能描述  : 回复Nas通路状态
 输入参数  : 无
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月24日
    作    者   : 路由切换
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID VOICE_McSendQryCnfMsgToVc(VOS_UINT16 uhwMsgId,VOS_UINT32 uwReceiverPid, VOS_UINT32 uwRet)
{
    VCVOICE_GROUND_RSP_STRU    stCnfMsg;

    /* 清空消息 */
    UCOM_MemSet(&stCnfMsg, 0, sizeof(VCVOICE_GROUND_RSP_STRU));

    /* 配置消息和返回值 */
    stCnfMsg.usMsgName        = uhwMsgId;
    stCnfMsg.enState          = (VOS_UINT16)uwRet;

    /* 根据uwReceiverPid,回复消息给相应NAS */
    UCOM_SendOsaMsg(DSP_PID_VOICE,
                    uwReceiverPid,
                    &stCnfMsg,
                    sizeof(VCVOICE_GROUND_RSP_STRU));
}

/*****************************************************************************
 函 数 名  : VOICE_McLinkInformToPhy
 功能描述  : 通知物理层通路状态
 输入参数  : 无
 输出参数  : 无
 返 回 值  : VOS_UINT16
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月23日
    作    者   : 路由切换
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID VOICE_McModemInformToPhy(
                CODEC_NET_MODE_ENUM_UINT16 enNetMode,
                VOICE_MC_MODEM_NUM_ENUM_UINT16 enModemNo,
                VOICE_MC_MODEM_OPT_ENUM_UINT16 enModemOpt
                )
{
    VOS_UINT32                      uwReceiverPid;              /* 需要通知的组件的PID */
    VOS_UINT16                      uhwMsgId;

    switch( enNetMode )
    {
        case CODEC_NET_MODE_G:
        {
            if( VOICE_MC_FOREGROUND_OPT == enModemOpt )
            {
                uhwMsgId = ID_VOICE_GPHY_FOREGROUND_REQ;
            }
            else
            {
                uhwMsgId = ID_VOICE_GPHY_BACKGROUND_REQ;
            }
            uwReceiverPid = g_auwModemMcPidTbl[enModemNo][VOICE_MC_GPHY_INDEX];
            VOICE_McSendModemMsgToGUPhy(enModemOpt,uwReceiverPid,uhwMsgId);
        }
        break;
        case CODEC_NET_MODE_W:
        {
            if( VOICE_MC_FOREGROUND_OPT == enModemOpt )
            {
                uhwMsgId = ID_VOICE_WPHY_FOREGROUND_REQ;
            }
            else
            {
                uhwMsgId = ID_VOICE_WPHY_BACKGROUND_REQ;
            }
            uwReceiverPid = g_auwModemMcPidTbl[enModemNo][VOICE_MC_WPHY_INDEX];
            VOICE_McSendModemMsgToGUPhy(enModemOpt,uwReceiverPid,uhwMsgId);
        }
        break;
        case CODEC_NET_MODE_TD:
        {
            if( VOICE_MC_FOREGROUND_OPT == enModemOpt )
            {
                uhwMsgId = ID_VOICE_TDPHY_FOREGROUND_REQ;
            }
            else
            {
                uhwMsgId = ID_VOICE_TDPHY_BACKGROUND_REQ;
            }
            uwReceiverPid = g_auwModemMcPidTbl[enModemNo][VOICE_MC_TDPHY_INDEX];
            VOICE_McSendModemMsgToTDPhy(enModemOpt,uwReceiverPid,uhwMsgId);
        }
        break;
        default:
        {

        }
    }


}
VOS_VOID VOICE_McSendModemMsgToTDPhy(
                VOICE_MC_MODEM_OPT_ENUM_UINT16 enLinkOpt,
                VOS_UINT32                     uwReceiverPid,
                VOS_UINT16                     uhwMsgId
                )
{
    switch(enLinkOpt)
    {
        case VOICE_MC_BACKGROUND_OPT:
        {
            VOICE_TDPHY_BACKGROUND_REQ_STRU stBackGroundReqMsg;
            /* 清空消息 */
            UCOM_MemSet(&stBackGroundReqMsg, 0, sizeof(VOICE_TDPHY_BACKGROUND_REQ_STRU));

            stBackGroundReqMsg.usMsgId = uhwMsgId;

            /* 根据uwReceiverPid,回复消息给相应TD物理层 */
            UCOM_SendOsaMsg(DSP_PID_VOICE,
                        uwReceiverPid,
                        &stBackGroundReqMsg,
                        sizeof(VOICE_TDPHY_BACKGROUND_REQ_STRU));
        }
        break;
        case VOICE_MC_FOREGROUND_OPT:
        {
            VOICE_TDPHY_FOREGROUND_REQ_STRU stForeGroundReqMsg;
            /* 清空消息 */
            UCOM_MemSet(&stForeGroundReqMsg, 0, sizeof(VOICE_TDPHY_FOREGROUND_REQ_STRU));

            stForeGroundReqMsg.usMsgId = uhwMsgId;

            /* 根据uwReceiverPid,回复消息给相应TD物理层 */
            UCOM_SendOsaMsg(DSP_PID_VOICE,
                        uwReceiverPid,
                        &stForeGroundReqMsg,
                        sizeof(VOICE_TDPHY_FOREGROUND_REQ_STRU));
        }
        break;
        default:
        {
        }

    }
}


VOS_VOID VOICE_McSendModemMsgToGUPhy(
                VOICE_MC_MODEM_OPT_ENUM_UINT16 enLinkOpt,
                VOS_UINT32 uwReceiverPid,
                VOS_UINT16 uhwMsgId)
{
    switch(enLinkOpt)
    {
        case VOICE_MC_BACKGROUND_OPT:
        {
            VOICE_GUPHY_BACKGROUND_REQ_STRU stBackGroundReqMsg;
             /* 清空消息 */
            UCOM_MemSet(&stBackGroundReqMsg, 0, sizeof(VOICE_GUPHY_BACKGROUND_REQ_STRU));

            stBackGroundReqMsg.usMsgId = uhwMsgId;

            /* 根据uwReceiverPid,回复消息给相应GU物理层 */
            UCOM_SendOsaMsg(DSP_PID_VOICE,
                            uwReceiverPid,
                            &stBackGroundReqMsg,
                            sizeof(VOICE_GUPHY_BACKGROUND_REQ_STRU));
        }
        break;
        case VOICE_MC_FOREGROUND_OPT:
        {
            VOICE_GUPHY_FOREGROUND_REQ_STRU stForeGroundReqMsg;
             /* 清空消息 */
            UCOM_MemSet(&stForeGroundReqMsg, 0, sizeof(VOICE_GUPHY_FOREGROUND_REQ_STRU));

            stForeGroundReqMsg.usMsgId = uhwMsgId;

            /* 根据uwReceiverPid,回复消息给相应GU物理层 */
            UCOM_SendOsaMsg(DSP_PID_VOICE,
                            uwReceiverPid,
                            &stForeGroundReqMsg,
                            sizeof(VOICE_GUPHY_FOREGROUND_REQ_STRU));
        }
        break;
        default:
        {

        }
    }
}


VOS_UINT16 VOICE_McGetModemNum( VOS_UINT32 uwSenderPid )
{
    VOS_UINT16    enRet;

    /* 根据PID判断通道号 */
    if (( (MODEM0_BEGIN <= uwSenderPid) && (MODEM0_END >= uwSenderPid))
        || (DSP_PID_TDPHY == uwSenderPid))
    {
        enRet = VOICE_MC_MODEM0_RANGE;
    }
    else if ((MODEM1_BEGIN <= uwSenderPid) && (MODEM1_END >= uwSenderPid))
    {
        enRet = VOICE_MC_MODEM1_RANGE;
    }
    else if ((CODEC_BEGIN <= uwSenderPid) && (CODEC_END >= uwSenderPid))
    {
        enRet = VOICE_MC_CODEC_RANGE;
    }
    else if ((OM_BEGIN <= uwSenderPid) && (OM_END >= uwSenderPid))
    {
        enRet = VOICE_MC_OM_RANGE;
    }
    else
    {
        enRet = VOICE_MC_PID_RANGE_NUM_BUTT;
    }

    return enRet;
}
VOS_UINT32 VOICE_McHandleFsmErr(VOS_UINT16 uhwSenderPid, VOS_UINT16 uhwMsgId)
{
    VOS_UINT32  uwRet = UCOM_RET_SUCC;

    /* 当前 FSM_ACT_TBL_ITEM 中的 PID 定义都为 0, 暂未考虑SenderPid */
    switch(uhwMsgId)
    {
        case ID_VC_VOICE_START_REQ:
        {
            VOICE_McSendCnfMsgToVc(ID_VOICE_VC_START_CNF, uhwSenderPid, UCOM_RET_ERR_STATE);
        }
        break;
        case ID_VC_VOICE_STOP_REQ:
        {
            VOICE_McSendCnfMsgToVc(ID_VOICE_VC_STOP_CNF, uhwSenderPid, UCOM_RET_ERR_STATE);
        }
        break;
        case ID_VC_VOICE_SET_CODEC_REQ:
        {
            VOICE_McSendCnfMsgToVc(ID_VOICE_VC_SET_CODEC_CNF, uhwSenderPid, UCOM_RET_ERR_STATE);
        }
        break;
        default:
        {

            /* 错误未被处理 */
            uwRet = UCOM_RET_ERR_MSG;
        }
    }

    return uwRet;
}


VOS_UINT32 VOICE_McHandleRtFsmErr(VOS_UINT16 uhwSenderPid, VOS_UINT16 uhwMsgId)
{
    /* 暂不处理任何异常 */

    return UCOM_RET_ERR_MSG;
}
VOS_UINT32 VOICE_McSetDev(VOS_UINT16 uhwDevMode, VOS_UINT32 uwUpdate, VOS_UINT16 uhwModemNo)
{
    VOS_UINT32                    uwRet;
    CODEC_NET_MODE_ENUM_UINT16    enNetMode;
    VOICE_MC_OBJS_STRU           *pstObj         = VOICE_McGetMcObjsPtr();
    VOICE_MC_DMA_FLAG_STRU       *pstDmaFlag     = VOICE_McGetDmaFlagPtr();
    VOS_UINT32                    uwUlTimer;
    VOS_UINT32                    uwDlTimer;

    /* 获取网络制式 */
    enNetMode   = VOICE_McGetModemNetMode(uhwModemNo);

    /* 校验和更新设备模式 */
    if(uhwDevMode >= VCVOICE_DEV_MODE_BUTT )
    {
        OM_LogError(VOICE_MsgSetDevReq_ParamError);

        return UCOM_RET_ERR_PARA;
    }

    /* 设置设备模式 */
    VOICE_McSetDeviveMode(uhwDevMode);

    /*通道设置*/
    VOICE_McSetDevChannel(uhwDevMode);

    /* 更新处理参数 */
    uwRet = UCOM_RET_SUCC;
    if(VOS_TRUE == uwUpdate)
    {
        /* 前台状态下需要更新 */
        if (uhwModemNo == VOICE_McGetForeGroundNum())
        {
            uwRet = VOICE_ProcUpdate(uhwDevMode, enNetMode);
        }

        /* 读取NV项 */
        UCOM_NV_Read(en_NV_SmartPACfg,
                     &(pstObj->stSmartPaCfg),
                     sizeof(pstObj->stSmartPaCfg));

        /* 如果免提且SMART PA可用，这里配置SMART PA SIO */
        if (   (VCVOICE_DEV_MODE_HANDFREE == VOICE_McGetDeviceMode())
            && (VOICE_SMART_PA_EN_ENABLE  == VOICE_McGetSmartPaEn()))
        {
            /* 切换至免提, 需重置DMA，重新启动MicIn、SpeakOut和EcRef对应的DMA */
            pstDmaFlag->usMicInRun   = CODEC_SWITCH_OFF;

            pstDmaFlag->usSpkOutRun  = CODEC_SWITCH_OFF;

            /* VOLTE下，切换为免提时，重新启动上下行DMA，以正常勾取SmartPa处数据 */
            if(CODEC_NET_MODE_L == enNetMode)
            {
                /* 启动下行DMA */
                DRV_TIMER_Start(&uwDlTimer,
                                VOICE_IMS_RX_BEGIN_TIMESTAMP,
                                DRV_TIMER_MODE_ONESHOT,
                                VOICE_Timer6msIsr,
                                0);

                /* 启动上行DMA */
                DRV_TIMER_Start(&uwUlTimer,
                                VOICE_IMS_TX_BEGIN_TIMESTAMP,
                                DRV_TIMER_MODE_ONESHOT,
                                VOICE_Timer3msIsr,
                                0);
            }
        }
        else
        {
            /* 关闭接受Smart PA的反馈信号 */
            UCOM_PCM_SmartPaStop();
        }
    }

    OM_LogInfo(VOICE_MsgSetDevReq_Ok);

    return uwRet;
}
VOS_UINT32 VOICE_McSetCodec(VCVOICE_SET_CODEC_REQ_STRU *pstMsg, VOS_UINT16 uhwModemNo)
{
    VOS_UINT32                  uwRet;
    VOS_UINT16                  uhwNvUpdateFlag;
    VOS_UINT16                  uhwCodecType;
    VOS_UINT16                  uhwNetMode    = VOICE_McGetModemNetMode(uhwModemNo);
    VOS_UINT16                  uhwModemState = VOICE_McGetModemState(uhwModemNo); /*通路状态 */
    VOICE_MC_DMA_FLAG_STRU     *pstDmaFlag    = VOICE_McGetDmaFlagPtr();
    VCVOICE_DEV_MODE_ENUM_UINT16        enDevMode;                /* 语音设备类型 */
    VOS_UINT32                          swSampleRate;

    /* 获取声码器类型 */
    uhwCodecType = pstMsg->enCodecType;

    /* 预处理: 合法性检查 + AMR2->AMR */
    uwRet = VOICE_McPreCfgCodecType(pstMsg->enMode, &uhwCodecType);
    if (UCOM_RET_SUCC != uwRet)
    {
        return uwRet;
    }

    /* 设置声码器以及网络制式 */
    VOICE_McSetCodecType(pstMsg->enMode, uhwCodecType, uhwModemNo);

    /* 判断是否为宽窄带切换 */
    uhwNvUpdateFlag = VOICE_ProcCheckUpdate(uhwCodecType);

    /*网络模式为L时，初始化JitterBuffer*/
    if ( CODEC_NET_MODE_L == pstMsg->enMode )
    {
        uwRet = VOICE_JB_Start(pstMsg->enCodecType);

        /* 初始化失败 */
        if (uwRet != UCOM_RET_SUCC)
        {
            OM_LogError(VOICE_McSetCodecRegAjbCodec);
            return uwRet;
        }
    }

    /* 前台需要更新声码器类型 */
    if(VOICE_McGetForeGroundNum() == uhwModemNo)
    {
        /* 只在采样率变化时更新PP模块参数 */
        if(VOICE_PROC_IS_YES == uhwNvUpdateFlag)
        {
            uwRet = VOICE_McUpdate(uhwModemNo);

            if (UCOM_RET_SUCC != uwRet)
            {
                return uwRet;
            }
        }
        else
        {
            swSampleRate = VOICE_ProcGetSampleRate();
            enDevMode    = VOICE_McGetDeviceMode();

            /* 更新数据IO模块(数据通道8K/16K更新) */
            uwRet = VOICE_PcmUpdate(swSampleRate, enDevMode);

            if(uwRet != UCOM_RET_SUCC)
            {
                return uwRet;
            }

            /* 重置DMA */
            pstDmaFlag->usMicInRun  = CODEC_SWITCH_OFF;
            pstDmaFlag->usSpkOutRun = CODEC_SWITCH_OFF;
        }

    }

    /* 后处理: 通知相关组件 */
    VOICE_McPostCfgCodecType(pstMsg->enCodecType, uhwModemNo);

    /* RAT变化通知物理层前后台状态 */
    if (uhwNetMode != pstMsg->enMode)
    {
       VOICE_McModemInformToPhy(pstMsg->enMode, uhwModemNo, uhwModemState);

       /* LTE下SRVCC不发suspend消息，不能统计切换时长 */
       if(uhwNetMode != CODEC_NET_MODE_L)
       {
           /* 检测网络切换时间是否过长 */
           VOICE_DiagSuspend(uhwModemNo, pstMsg->enMode, uhwNetMode);
       }
    }

    return UCOM_RET_SUCC;
}



VOS_UINT32 VOICE_McStart(VCVOICE_START_REQ_STRU *pstMsg)
{
    VOS_UINT32                       uwRet;
    VOS_UINT16                       uhwCodecType;
    VOICE_MC_MODEM_NUM_ENUM_UINT16   enModemNo;                                    /* 通道号枚举 */
    VOICE_MC_MODEM_NUM_ENUM_UINT16   enActiveModemNo = VOICE_McGetForeGroundNum(); /* 前台通道号 */
    VOICE_PROC_OBJS_STRU            *pstProc         = VOICE_ProcGetObjsPtr();

    /* 网络制式检测 */
    if (CODEC_NET_MODE_BUTT <= pstMsg->enMode)
    {
        return UCOM_RET_ERR_STATE;
    }

    /* 获取声码器类型 */
    uhwCodecType = pstMsg->enCodecType;

    /* 当前有前台,返回失败 */
    if (VOICE_MC_MODEM_NUM_BUTT > enActiveModemNo)
    {
        return UCOM_RET_FAIL;

    }

    /* 预处理: 合法性检查 + AMR2->AMR */
    uwRet = VOICE_McPreCfgCodecType(pstMsg->enMode, &uhwCodecType);
    if (UCOM_RET_SUCC != uwRet)
    {
       return uwRet;
    }

    /* 获取通道号 */
    enModemNo   = VOICE_McGetModemNum(pstMsg->uwSenderPid);

    /* 对enModemNo进行保护 */
    if (VOICE_MC_MODEM_NUM_BUTT <= enModemNo)
    {
        OM_LogError(VOICE_GetModemNumError);

        return UCOM_RET_FAIL;
    }

    /* PROC全局变量中的codec指针置为当前通路的codec */
    pstProc->pstCodec = VOICE_McGetModemCodecPtr(enModemNo);

    /* 设置声码器以及网络制式 */
    VOICE_McSetCodecType(pstMsg->enMode, uhwCodecType, enModemNo);

    /* 更新处理参数 */
    uwRet = VOICE_McUpdate(enModemNo);

    if (UCOM_RET_SUCC != uwRet)
    {
        pstProc->pstCodec = VOS_NULL;
        return uwRet;
    }

    /* 后处理: 通知相关组件 */
    VOICE_McPostCfgCodecType(pstMsg->enCodecType, enModemNo);

    /* 切换语音软件运行状态 */
    VOICE_McSetVoiceState(enModemNo, VOICE_MC_STATE_RUNNING);

    /* 设置uwSenderPid所在通路对象状态为前台 */
    VOICE_McSetModemState(enModemNo, VOICE_MC_MODEM_STATE_FOREGROUND);

    /* 设置当前激活Modem号 */
    VOICE_McSetForeGroundNum(enModemNo);

    /* 将当前激活Modem号的语音故障上报标志置为0 */
    UCOM_MemSet(VOICE_DiagGetReportFlag(enModemNo),
                0,
                (VOICE_DIAG_CHECK_POINT_NUM * sizeof(VOS_UINT16)));

    return uwRet;

}


VOICE_MC_STATE_ENUM_UINT16 VOICE_McGetVoiceState(VOS_UINT32 uwSenderPid)
{
    VOICE_MC_STATE_ENUM_UINT16      enMcVoiceState;
    VOICE_MC_PID_RANGE_ENUM_UINT16  enPidRange;

    enPidRange  = VOICE_McGetModemNum(uwSenderPid);

    switch(enPidRange)
    {
        case VOICE_MC_MODEM0_RANGE:
        case VOICE_MC_MODEM1_RANGE:
        {
            enMcVoiceState = VOICE_McGetModemVoiceState(enPidRange);
        }
        break;
        case VOICE_MC_CODEC_RANGE:
        case VOICE_MC_OM_RANGE:
        {
            enMcVoiceState = VOICE_McGetForeGroundVoiceState();
        }
        break;
        default:
        {
            enMcVoiceState = VOICE_MC_STATE_IDLE;
        }
    }

    return enMcVoiceState;
}


VOICE_MC_STATE_ENUM_UINT16 VOICE_McGetForeGroundVoiceState(VOS_VOID)
{
    VOICE_MC_STATE_ENUM_UINT16  enVoiceState = VOICE_MC_STATE_IDLE;
    VOICE_MC_MODEM_STRU        *pstMcModem   = VOS_NULL;

    pstMcModem = VOICE_McGetForeGroundObj();

    if(VOS_NULL != pstMcModem)
    {
        enVoiceState = pstMcModem->enState;
    }

    return enVoiceState;
}

/*****************************************************************************
 函 数 名  : VOICE_McGetForeGroundNetMode
 功能描述  : 获取前台网络模式
 输入参数  : VOS_VOID
 输出参数  : 无
 返 回 值  : VOS_UINT16
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年2月19日
    作    者   : g002017040
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT16 VOICE_McGetForeGroundNetMode( VOS_UINT16 uhwActiveModemNo )
{
    CODEC_NET_MODE_ENUM_UINT16  enNetMode    = CODEC_NET_MODE_BUTT;

    /* 取前台网络制式 */
    if (VOICE_MC_MODEM_NUM_BUTT > uhwActiveModemNo)
    {
        enNetMode = VOICE_McGetModemNetMode(uhwActiveModemNo);
    }

    return enNetMode;
}
VOS_UINT32 VOICE_McApiIsIdle(VOS_VOID)
{
    /* 两条通路皆为IDLE,可以下电 */
    if ((VOICE_MC_STATE_IDLE == VOICE_McGetModemVoiceState(VOICE_MC_MODEM0)) &&
         (VOICE_MC_STATE_IDLE == VOICE_McGetModemVoiceState(VOICE_MC_MODEM1)))
    {
        return UCOM_RET_SUCC;
    }

    return UCOM_RET_FAIL;
}
VOICE_MC_MODEM_STRU* VOICE_McGetForeGroundObj(VOS_VOID)
{
    VOICE_MC_MODEM_STRU   *pstModem            = VOS_NULL;
    VOS_UINT16             uhwActiveModemNo    = VOICE_McGetForeGroundNum();

    if (VOICE_MC_MODEM_NUM_BUTT > uhwActiveModemNo)
    {
        pstModem = VOICE_McGetModemObjPtr(uhwActiveModemNo);
    }

    return pstModem;
}


VOS_VOID VOICE_McSetBackGround(VOICE_MC_MODEM_NUM_ENUM_UINT16      enModemNo)
{
    VOICE_PROC_OBJS_STRU            *pstProc          = VOICE_ProcGetObjsPtr();
    CODEC_NET_MODE_ENUM_UINT16       enNetMode;

    enNetMode     = VOICE_McGetModemNetMode(enModemNo);

    /* 设置通路状态为background */
    VOICE_McSetModemState(enModemNo, VOICE_MC_MODEM_STATE_BACKGROUND);

    /* 设置当前激活Modem号 */
    VOICE_McSetForeGroundNum(VOICE_MC_MODEM_NUM_BUTT);

    /* PROC全局变量中的codec指针置NULL */
    pstProc->pstCodec = VOS_NULL;

    /* 停止dma和sio */
    VOICE_McStop(enModemNo);

    /* 通知物理层静默 */
    VOICE_McModemInformToPhy(enNetMode,enModemNo,VOICE_MC_BACKGROUND_OPT);
}
CODEC_NET_MODE_ENUM_UINT16 VOICE_McGetNetMode(VOS_UINT32 uwSenderPid)
{
    CODEC_NET_MODE_ENUM_UINT16      enMcNetMode     = CODEC_NET_MODE_BUTT;
    VOICE_MC_PID_RANGE_ENUM_UINT16  enPidRange;
    VOICE_MC_MODEM_NUM_ENUM_UINT16   enActiveModemNo = VOICE_McGetForeGroundNum();
    enPidRange  = VOICE_McGetModemNum(uwSenderPid);

    switch(enPidRange)
    {
        case VOICE_MC_MODEM0_RANGE:
        case VOICE_MC_MODEM1_RANGE:
        {
            enMcNetMode = VOICE_McGetModemNetMode(enPidRange);
        }
        break;
        case VOICE_MC_CODEC_RANGE:
        case VOICE_MC_OM_RANGE:
        {
            if (VOICE_MC_MODEM_NUM_BUTT > enActiveModemNo)
            {
                enMcNetMode = VOICE_McGetForeGroundNetMode(enActiveModemNo);
            }
        }
        break;
        default:
        {
        }
    }

    return enMcNetMode;
}

/*****************************************************************************
 函 数 名  : VOICE_McResume
 功能描述  : 网络进行异系统切换完成后语音处理恢复
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2009年8月11日
    作    者   : 苏庄銮 59026
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID VOICE_McResume(VCVOICE_TYPE_ENUM_UINT16 enCodecType, VOS_UINT16 uhwModemNo)
{
    VOICE_McSetVoiceState(uhwModemNo, VOICE_MC_STATE_RUNNING);

    OM_LogInfo(VOICE_McDoResume_OK);
}


VOS_VOID VOICE_McStop(VOS_UINT16 uhwModemNo)
{
    VCVOICE_DEV_MODE_ENUM_UINT16        enDevMode;     /* 设备模式 */
    CODEC_NET_MODE_ENUM_UINT16          enNetMode;     /* 网络制式 */
    VOICE_PCM_BUFFER_STRU              *pstPcmBuffTmp  = VOS_NULL;
    VOS_INT16                          *pshwLineOut    = VOICE_ProcGetLineOutBufPtr();

    /* 获取Timer */
    VOICE_MC_TIMER_STRU *pstTimer   = VOICE_McGetTimerPtr();

    /* 清空解码缓存中的数据 */
    VOICE_ProcInitDecSerialWrite();

    /* 清空编码前PCM码流buff */
    UCOM_MemSet(pshwLineOut,
                0,
                (CODEC_PCM_MAX_FRAME_LENGTH * sizeof(VOS_UINT16)));

    /* 清空缓存中的PCM码流 */
    pstPcmBuffTmp   = (&g_stVoicePcmBuff);
    UCOM_MemSet(pstPcmBuffTmp,  0, sizeof(g_stVoicePcmBuff));

    /* 清空上帧解码前缓存的DecSerial数据 */
    UCOM_MemSet(VOICE_ProcGetPrevDecDataPtr(), 0, GPHY_VOICE_DATA_LEN * sizeof(VOS_INT16));

    /*停止MIC处DMA搬运*/
    DRV_DMA_Stop(VOICE_MC_DMAC_CHN_MIC);

    /*停止SPK处DMA搬运*/
    DRV_DMA_Stop(VOICE_MC_DMAC_CHN_SPK);

    /*停止Smart PA的DMA*/
    UCOM_PCM_SmartPaStop();

    /*初始化语音软件DMA相关标志*/
    UCOM_MemSet(VOICE_McGetDmaFlagPtr(), 0, sizeof(VOICE_MC_DMA_FLAG_STRU));

    /* 获取设备模式 */
    enDevMode = VOICE_McGetDeviceMode();

    /* PC VOICE模式下停止20ms定时器 */
    if (VCVOICE_DEV_MODE_PCVOICE == enDevMode)
    {
        DRV_TIMER_StopPrecise(&(pstTimer->uwTimer20ms));
        DRV_TIMER_StopPrecise(&(pstTimer->uwTimer20msRx));

    }
    else
    {
        /*停止SIO设备*/
        DRV_SIO_Voice_Close();
    }

    /* 获取网络制式 */
    enNetMode = VOICE_McGetModemNetMode(uhwModemNo);

    /* TD取消5ms帧同步中断 */
    if((CODEC_NET_MODE_TD == enNetMode) && (VOICE_MC_STATE_RUNNING == VOICE_McGetModemVoiceState(uhwModemNo)))
    {
        /* 根据V9R1 <<Hi6620V100 SOC逻辑需求规格表_DMA请求和中断分配.xlsx>>,这里为tds_stu_hifi_int的ID请求号 */
        VOS_DisableInterrupt(DRV_TDSCDMA_CTRL_SYNC_INTR);

        /* 去使能屏蔽寄存器，使其状态为屏蔽状态 */
        UCOM_RegBitWr(DRV_TDSCDMA_CTRL_EN_ADDR, UCOM_BIT0, UCOM_BIT0, 0);

    }

    /* 清空语音诊断模块统计的帧信息 */
    VOICE_DiagReset();

    /* 停止语音诊断模块30ms周期定时器 */
    DRV_TIMER_StopPrecise(&(pstTimer->uwTimer30msDiag));

}


VOS_VOID VOICE_McModemObjInit( VOICE_MC_MODEM_STRU *pstMcModemObj )
{
    VOICE_PROC_CODEC_STRU  *pstCodec;
    VOICE_DEC_SERIAL_STRU  *pstDecBuf = VOICE_ProcGetDecBufPtr();

    if (VOS_NULL == pstMcModemObj)
    {
        return ;
    }

    UCOM_MemSet(pstMcModemObj,0,sizeof(VOICE_MC_MODEM_STRU));

    /* 通路状态置为UNSET */
    pstMcModemObj->enModemState = VOICE_MC_MODEM_STATE_UNSET;

    /* 运行状态置为IDLE */
    pstMcModemObj->enState      = VOICE_MC_STATE_IDLE;

    /* 网络制式置为CODEC_NET_MODE_BUTT */
    pstMcModemObj->enNetMode    = CODEC_NET_MODE_BUTT;

    /* 编解码初始化 */
    pstCodec = &pstMcModemObj->stCodec;

    /* 置声码器输入输出缓存 */
    pstCodec->stEncOutObj.pshwEncSerial = VOICE_ProcGetEncBufPtr();
    pstCodec->stDecInObj.pshwDecSerial  = pstDecBuf->asDecSerial;
}
VOS_UINT32 VOICE_McPreTxProc( VOS_VOID )
{
    VOS_UINT32  uwRet            = UCOM_RET_SUCC;
    VOS_UINT16  uhwActiveModemNo = VOICE_McGetForeGroundNum();
    VOS_UINT16  uhwNetMode       = VOICE_McGetForeGroundNetMode(uhwActiveModemNo);

    /* 若是UMTS模式下,检查更新AMR速率控制参数 */
    if ((CODEC_NET_MODE_W == uhwNetMode)
         ||(CODEC_NET_MODE_TD == uhwNetMode))
    {
        /* 调用AMR速率控制检查更新函数 */
        uwRet = VOICE_ProcCfgUmtsUlCodec();
        if (uwRet == UCOM_RET_FAIL)
        {
            return  UCOM_RET_FAIL;
        }
    }

    return uwRet;
}


VOS_UINT32 VOICE_McTxEnc( VOS_VOID )
{
    VOS_UINT32                          uwRet       = UCOM_RET_SUCC;
    VOS_INT16                          *pshwLineOut = VOICE_ProcGetLineOutBufPtr();
    VOICE_PROC_CODEC_STRU              *pstCodec    = VOICE_ProcGetCodecPtr();
    VOS_INT16                          *pshwEncBuf  = VOICE_ProcGetEncBufPtr();

    /* 1 将声码器输入端的PCM数据钩取上报给OM */
    VOICE_DbgSendHook(VOICE_HOOK_CODECIN_PCM, pshwLineOut, (VOS_UINT16)CODEC_PCM_FRAME_LENGTH);

    /* 2 开始一帧语音数据的编码 */
    uwRet = VOICE_ProcEncode(pshwLineOut, pshwEncBuf);

    if (UCOM_RET_SUCC != uwRet)
    {
        OM_LogError(VOICE_McDoEnc_ProcEncodeError);
    }

    /* G711直接是PCM码流，不需要上报 */
    if (CODEC_G711 != pstCodec->enCodecType)
    {
        /* 3 记录编解码结构体, 供PC仿真使用 */
        VOICE_DbgSaveCodecObj(pshwEncBuf, VOICE_DBG_CODEC_MODE_ENCODE, pstCodec);

        /* 4 将编码后的码流数据钩取上报给OM */
        VOICE_DbgSendHook(VOICE_HOOK_TX_STRM, pshwEncBuf, VOICE_CODED_FRAME_WITH_OBJ_LEN);
    }

    if (UCOM_RET_SUCC == uwRet)
    {
        OM_LogFreqInfo(VOICE_McDoEnc_Done, VOICE_LOG_PERIOD);
    }

    return uwRet;

}
VOS_UINT32 VOICE_McTxProc(VOS_VOID)
{
    VOS_UINT32                          uwRet       = UCOM_RET_SUCC;
    VOS_INT16                          *pshwMicIn   = g_psVoicePcmMicIn;
    VOS_INT16                          *pshwMcIn    = VOICE_PcmGetMcInBufPtr();
    VOS_INT16                          *pshwRcIn    = VOICE_PcmGetRcInBufPtr();
    VOS_INT16                          *pshwLineOut = VOICE_ProcGetLineOutBufPtr();
    VOICE_MC_DMA_FLAG_STRU             *pstDmaFlag  = VOICE_McGetDmaFlagPtr();
    VOICE_HOOK_STRU                    *pstHook     = VOICE_DbgGetHookPtr();
    VOS_UINT16                          uhwActModem = VOICE_McGetForeGroundNum();
    VOS_INT16                          *pshwSpkOutTemp = VOICE_PcmGetSpkOutBufTempPtr();
    VOS_INT16                           shwVolumeTx;
    VOICE_MC_MODEM_STRU                *pstModem;
    VOICE_MC_OBJS_STRU                 *pstVoiceMcObj;
    VOS_UINT32                          uwSmpRate;

    /* 获取mc控制结构体 */
    pstVoiceMcObj               = VOICE_McGetMcObjsPtr();

    /* 前后处理开始, 不允许DMA切换 */
    pstDmaFlag->usMicInSwEnable = CODEC_SWITCH_OFF;

    /* 更新本次通话帧号 */
    pstHook->ulFrameTick        = pstHook->ulFrameTick + 1;

    /* 根据Active Modem Number获取上行音量信息 */
    pstModem    = VOICE_McGetModemObjPtr(uhwActModem);
    shwVolumeTx = pstModem->shwVolumeTx;

    /* 1 将从Mic输入的PCM数据钩取上报给OM */
    VOICE_DbgSendHook(VOICE_HOOK_MICIN_PCM,
                      pshwMicIn,
                      (VOS_UINT16)((VOS_UINT16)CODEC_PCM_FRAME_LENGTH*DRV_SIO_GetSioChannelNum(DRV_SIO_GetVoiceCfgPtr())));

    /* 2 上行语音前后处理 */
    uwRet = VOICE_ProcTxPp(pshwMcIn, pshwRcIn, pshwLineOut);
    if (UCOM_RET_SUCC != uwRet)
    {
        OM_LogError(VOICE_McDoPp_ProcPreError);
    }
    else
    {
        OM_LogFreqInfo(VOICE_McDoPp_Done, VOICE_LOG_PERIOD);
    }

    /* 设置上行音量，在TxPP完成之后 */
    uwRet = VOICE_ProcVolume(shwVolumeTx, pshwLineOut, pshwLineOut);
    if (UCOM_RET_SUCC != uwRet)
    {
        OM_LogError(VOICE_ProcVolume_InvalidVolume);
    }

    /* 上行处理完毕后设置可切换标志 */
    pstDmaFlag->usMicInSwEnable  = CODEC_SWITCH_ON;

    if (VOS_NULL != pstVoiceMcObj->pfunCallBack)
    {
        /* 获取当前采样率 */
        uwSmpRate = VOICE_ProcGetSampleRate();

        /* 将上下行数据发送给audio_recorder */
        (VOS_VOID)pstVoiceMcObj->pfunCallBack(pshwLineOut, pshwSpkOutTemp, uwSmpRate);

        OM_LogInfo1(VOICE_ProcVolume_InvalidVolume,uwSmpRate);

    }

    return uwRet;
}


VOS_UINT32 VOICE_McPostTxProc( VOS_VOID )
{
    VOS_UINT16  uhwActiveModemNo = VOICE_McGetForeGroundNum();
    VOS_UINT16  uhwNetMode       = VOICE_McGetForeGroundNetMode(uhwActiveModemNo);

    /* 发送上行编码结果 */
    switch(uhwNetMode)
    {
        case CODEC_NET_MODE_W:
        {

            /* 当前网络制式为UMTS时,发送上行编码结果给WPHY */
            VOICE_McTxDataToWphy();
        }
        break;
        case CODEC_NET_MODE_G:
        {

            /* 当前网络制式为GSM时,发送上行编码结果给GPHY */
            VOICE_McTxDataToGphy();

            /* 上行时序冲突检测 */
            VOICE_McGsmChkMicConflick();
        }
        break;
        case CODEC_NET_MODE_TD:
        {

            /* 当前网络制式为TD时,发送上行编码结果给TDPHY */
            VOICE_McTxDataToTDphy();

            /* 上行时序冲突检测 */
            VOICE_McChkMicConflick();
        }
        break;
        case CODEC_NET_MODE_L:
        {
            /* 当前网络制式为LTE时,发送上行编码结果给IMSA */
            VOICE_McTxDataToImsa();
        }
        break;
        default:
        {
            OM_LogWarning(VOICE_McPostTxProc_NetModeMisMatch);
        }
    }

    return UCOM_RET_SUCC;
}
VOS_UINT32 VOICE_McRxPreProc(VOS_UINT16 *puhwRxFrmLost, VOS_INT16 *pshwVolumeRx, VOS_INT16 **ppshwDecData)
{
    VOS_UINT32                   uwRet           = UCOM_RET_SUCC;
    VOS_UINT16                   uhwRxFrmLost    = VOICE_PROC_IS_NO;
    VOICE_DEC_SERIAL_STRU       *pstDecBuf       = VOICE_ProcGetDecBufPtr();
    VOICE_PROC_CODEC_STRU       *pstCodec        = VOICE_ProcGetCodecPtr();
    VOS_UINT16                   uhwActModem     = VOICE_McGetForeGroundNum();
    VOS_INT16                    shwVolumeRx;
    VOICE_MC_MODEM_STRU         *pstModem;
    VOICE_DEC_SERIAL_STRU        *pstDecWrite    = VOICE_ProcGetDecSerialWritePtr();
    VOS_UINT16                   uhwNetMode      = VOICE_McGetForeGroundNetMode(uhwActModem);
    VOS_UINT32                   uwTimeStamp     = 0;
    VOS_UINT32                   uwDuration      = 0;
    VOS_INT16                   *pshwDecData     = VOS_NULL;

    /* LTE且不使能AJB，走静态JB流程 */
    if ((0 == VOICE_JB_IsAjbEnable()) && (CODEC_NET_MODE_L == uhwNetMode))
    {
        uwRet = VOICE_JB_GetSjbPkt();
        if(UCOM_RET_SUCC != uwRet)
        {
            return uwRet;
        }
    }

    /* 使用AJB时,不用做这些操作 */
    if ((CODEC_NET_MODE_L != uhwNetMode) || (0 == VOICE_JB_IsAjbEnable()))
    {
        /* 读取SOC计数器，获取当前时戳 */
        uwTimeStamp = DRV_TIMER_ReadSysTimeStamp();

        /* 根据中断进入时间计算停留时间 */
        uwDuration = UCOM_COMM_CycSub(uwTimeStamp, pstDecBuf->uwTimeStamp, 0xffffffff);

        /*若解码缓存BUFF中存满两帧数据，且当前与缓存的前一帧的时间差超过30ms，则取后一帧缓存数据进行解码*/
        if ((((uwDuration * VOICE_TIMESTAMP_MS_RATIO) / DRV_TIMER_GetOmFreq()) > VOICE_LAST_FRAME_OFFSET)
            && (pstDecWrite == (pstDecBuf + VOICE_CODED_DECODE_BUF_FRAME_NUM)))
        {
            UCOM_MemCpy(pstDecBuf, pstDecBuf + 1, sizeof(VOICE_DEC_SERIAL_STRU));

            VOICE_ProcSubDecSerialWritePtr();
        }

        /* 下行丢帧检测,LTE网络下不检测 */
        uhwRxFrmLost = VOICE_McDetectRxFrmLost();
    }

    /* 获取当前激活Modem的下行音量 */
    pstModem    = VOICE_McGetModemObjPtr(uhwActModem);
    shwVolumeRx = pstModem->shwVolumeRx;

    /* 配置下行解码参数 */
    uwRet = VOICE_ProcCfgDlCodec(pstModem->enNetMode,
                                 pstDecBuf->asDecSerial,
                                 uhwRxFrmLost,
                                 &pshwDecData);

    if (UCOM_RET_SUCC != uwRet)
    {
        /* 计算下一次存储解码数据的地址 */
        VOICE_ProcSubDecSerialWrite(uhwRxFrmLost);

        OM_LogError(VOICE_ProcCfgDlCodecError);
        return uwRet;
    }

    /* ErrorLog 20ms 周期触发 */
    VOICE_ErrLogEventCycle(VOICE_ERRLOG_CHECK_PERIOD);

    /* 1 编码后环回@解码之前: 使用上一帧的编码结果覆盖本次的待解码数据 */
    VOICE_DbgCheckAndLoop(CODEC_LOOP_CODEC);

    /* G711直接是PCM码流，不需要上报 */
    if (CODEC_G711 != pstCodec->enCodecType)
    {
    	/* 2.1 记录编解码结构体, 供PC仿真使用 */
    	VOICE_DbgSaveCodecObj(pshwDecData, VOICE_DBG_CODEC_MODE_DECODE, pstCodec);

    	/* 2.2 将下行码流数据钩取上报给OM */
	VOICE_DbgSendHook(VOICE_HOOK_RX_STRM, pshwDecData, VOICE_CODED_FRAME_WITH_OBJ_LEN );
    }

    /* 检查解码状态, 若出现异常则启动ErrorLog打印 */
    VOICE_ErrLogCheckDecStatus(&pstCodec->stDecInObj);

    /* RxProc中还需要用到这三个参数，所以前处理结束后，再传出这三个值 */
    *puhwRxFrmLost = uhwRxFrmLost;
    *pshwVolumeRx  = shwVolumeRx;
    *ppshwDecData  = pshwDecData;
    return uwRet;
}


VOS_UINT32 VOICE_McRxProc(VOS_VOID)
{
    VOS_UINT32                   uwRet           = UCOM_RET_SUCC;
    VOS_UINT16                   uhwRxFrmLost    = VOICE_PROC_IS_NO;
    VOS_INT16                   *pshwLineIn      = VOICE_ProcGetLineInBufPtr();
    VOS_INT16                   *pshwSpkOut      = VOICE_PcmGetSpkOutBufPtr();
    VOS_INT16                   *pshwSpkOutTemp  = VOICE_PcmGetSpkOutBufTempPtr();
    CODEC_INOUT_STRU            *pstInOut        = VOICE_McGetInoutPtr();
    VOICE_MC_DMA_FLAG_STRU      *pstDmaFlag      = VOICE_McGetDmaFlagPtr();
    VOS_UINT16                   uhwActModem     = VOICE_McGetForeGroundNum();
    VOS_INT16                    shwVolumeRx;
    VOS_INT16                   *pshwDecData     = VOS_NULL;
    VOS_UINT16                   uhwNetMode      = VOICE_McGetForeGroundNetMode(uhwActModem);
    VOS_UINT32                   uwAjbOutDataLen;

    /* 若在G下类型不匹配则不进行解码操作 */
    if (   (CODEC_NET_MODE_G  == uhwNetMode)
           && (VOICE_PROC_IS_YES == VOICE_McGetGsmCodecTypeMismatch(uhwActModem)))
    {
        /* 清空下行SpeakOut缓冲buff */
        UCOM_MemSet(pshwSpkOut, 0, CODEC_PCM_MAX_FRAME_LENGTH * 2 * sizeof(VOS_INT16));
        UCOM_MemSet(pshwSpkOutTemp, 0, CODEC_PCM_MAX_FRAME_LENGTH * sizeof(VOS_INT16));

        return uwRet;
    }

    /* 解码前处理 */
    uwRet = VOICE_McRxPreProc(&uhwRxFrmLost, &shwVolumeRx, &pshwDecData);
    if(UCOM_RET_SUCC != uwRet)
    {
        return uwRet;
    }

    if ((CODEC_INOUT_DEFAULT == pstInOut->usRxIn)
    || (CODEC_INOUT_DATA == pstInOut->usRxIn))
    {
        /* 解码尚未完成,不允许DMA搬运 */
        pstDmaFlag->usSpkOutSwEnable = CODEC_SWITCH_OFF;

        /* 3 开始一帧语音数据的解码，如果是LTE下且应用AJB，则从AJB里取 */
        if ((VOICE_JB_IsAjbEnable()) && (CODEC_NET_MODE_L == uhwNetMode))
        {
            uwRet = (VOS_UINT32)VOICE_JB_GetAjbPcmData(VOICE_JB_PKT_TIME_20MS, pshwLineIn, &uwAjbOutDataLen);
        }
        else
        {
            uwRet = VOICE_ProcDecode(pshwDecData, pshwLineIn);

            /* 计算下一次存储解码数据的地址 */
            VOICE_ProcSubDecSerialWrite(uhwRxFrmLost);

            /* 4 将从声码器解码输出的PCM数据钩取上报给OM,如果使用AJB，就由AJB负责勾 */
            VOICE_DbgSendHook(VOICE_HOOK_CODECOUT_PCM, pshwLineIn, (VOS_UINT16)CODEC_PCM_FRAME_LENGTH);
        }

        if (UCOM_RET_SUCC != uwRet)
        {
            OM_LogError(VOICE_MsgDoDec_ProcDecodeError);
        }

        /* 5 若启动编码前环回则将上一次的上行编码前数据环回至下行解码处 */
        VOICE_DbgCheckAndLoop(CODEC_LOOP_BEFORE_CODEC);

        /* 6 设置下行音量，在RxPP之前 */
        uwRet = VOICE_ProcVolume(shwVolumeRx, pshwLineIn, pshwLineIn);
        if (UCOM_RET_SUCC != uwRet)
        {
            OM_LogError(VOICE_ProcVolume_InvalidVolume);
        }

        /* 7 开始下行数据预处理,包括增益叠加、滤波、侧音叠加等*/
        uwRet = VOICE_ProcRxPp(pshwLineIn, pshwSpkOutTemp);

        /* 统一处理错误 */
        if (UCOM_RET_SUCC != uwRet)
        {
            OM_LogError(VOICE_McDoPp_ProcPostError);
        }
        else
        {
            OM_LogFreqInfo(VOICE_McDoPp_Done, VOICE_LOG_PERIOD);
        }

        /* 将预处理以后的数据填充为I2S双声道数据 */
        VOICE_PcmStereo2Hybrid(pshwSpkOutTemp,pshwSpkOut, CODEC_PCM_FRAME_LENGTH);

        /* 8 若启动DMA环回则将上一次的上行Mic采集的数据环回至下行 */
        VOICE_DbgCheckAndLoop(CODEC_LOOP_DMA);

        pstDmaFlag->usSpkOutSwEnable = CODEC_SWITCH_ON;

    }

    /* 10 将送入Spk的PCM数据钩取上报给OM */
    VOICE_DbgSendHook(VOICE_HOOK_SPKOUT_PCM,
                      pshwSpkOut,
                      (VOS_UINT16)((VOS_UINT16)CODEC_PCM_FRAME_LENGTH*DRV_SIO_GetSioChannelNum(DRV_SIO_GetVoiceCfgPtr())));

    OM_LogFreqInfo(VOICE_McDoDec_Done, VOICE_LOG_PERIOD);

    return uwRet;

}


VOS_UINT32 VOICE_McPlay(VOS_VOID)
{
    VOS_UINT32              uwRet;
    VOS_UINT32              uwSampleRate;
    CODEC_INOUT_STRU       *pstInout    = VOICE_McGetInoutPtr();
    VOICE_MC_DMA_FLAG_STRU *pstDmaFlag  = VOICE_McGetDmaFlagPtr();
    VOICE_MC_TIMER_STRU    *pstTimer    = VOICE_McGetTimerPtr();
    VOICE_MC_OBJS_STRU     *pstObj      = VOICE_McGetMcObjsPtr();

    /* 如果是手机模式,则进行链式DMA搬运,否则不进行动作 */
    if (CODEC_INOUT_DEFAULT == pstInout->usRxOut)
    {
        pstDmaFlag->usSpkOutSwEnable = CODEC_SWITCH_ON;

        /* 启动SPK处下行播放链式DMA搬运 */
        uwRet = VOICE_PcmSpkOutStartLoopDMA(VOICE_MC_DMAC_CHN_SPK, VOICE_SpkDmaIsr);

        if (   (VCVOICE_DEV_MODE_HANDFREE == VOICE_McGetDeviceMode())
            && (VOICE_SMART_PA_EN_ENABLE  == VOICE_McGetSmartPaEn()))
        {
            uwSampleRate = VOICE_ProcGetSampleRate();

            /* 免提模式下，启动SmartPA处下行播放链式DMA搬运 */
            UCOM_PCM_SmartPaStartLoopDma((VOS_UINT16)(CODEC_PCM_FRAME_LENGTH_BYTES * UCOM_PCM_I2S_CHANNEL_NUM),
                                         uwSampleRate,
                                         pstObj->stSmartPaCfg.uhwIsMaster,
                                         UCOM_PCM_GetSmartPaDmacChn(),
                                         VOICE_EcRefDmaIsr);
        }

        /* 标志当前SPK处DMA状态为已启动 */
        pstDmaFlag->usSpkOutRun      = CODEC_SWITCH_ON;

        /* 清空上帧解码前缓存的DecSerial数据 */
        UCOM_MemSet(VOICE_ProcGetPrevDecDataPtr(), 0, GPHY_VOICE_DATA_LEN * sizeof(VOS_INT16));

        /* 记录日志 */
        OM_LogInfo(VOICE_McStartPcmTrans_SpkOut);
    }
    else if (CODEC_INOUT_DATA == pstInout->usRxOut)
    {
        /* 先停止20ms定时器 */
        DRV_TIMER_StopPrecise(&(pstTimer->uwTimer20msRx));

        /* 给自己发解码消息 */
        uwRet = DRV_TIMER_Start(&(pstTimer->uwTimer01msRx),
                                1,
                                DRV_TIMER_MODE_ONESHOT,
                                VOICE_Timer20msRxIsr,
                                0);

        /* 重新启动20ms定时器,单位0.1ms */
        uwRet = DRV_TIMER_StartPrecise(&(pstTimer->uwTimer20msRx),
                                200,
                                DRV_TIMER_MODE_PERIODIC,
                                VOICE_Timer20msRxIsr,
                                0);

        /* PC Voice模式下行DMA搬运在解码后直接进行 */
        uwRet = UCOM_RET_SUCC;
    }
    else
    {
        uwRet = UCOM_RET_ERR_PARA;
    }

    /* 启动30ms周期性定时器，用于监控上下行DMA中断是否按时收到 */
    DRV_TIMER_StartPrecise(&(pstTimer->uwTimer30msDiag),
                        300,
                        DRV_TIMER_MODE_PERIODIC,
                        VOICE_Timer30msDiagIsr,
                        0);

    return uwRet;
}
VOS_UINT32 VOICE_McRecord(VOS_VOID)
{
    VOS_UINT32              uwRet;
    CODEC_INOUT_STRU       *pstInout    = VOICE_McGetInoutPtr();
    VOICE_MC_DMA_FLAG_STRU *pstDmaFlag  = VOICE_McGetDmaFlagPtr();
    VOICE_MC_TIMER_STRU    *pstTimer    = VOICE_McGetTimerPtr();

    /* 若为手机模式,则启动上行链式DMA搬运 */
    if (CODEC_INOUT_DEFAULT == pstInout->usTxIn)
    {
        /* 启动MIC处上行采集链式DMA搬运 */
        uwRet = VOICE_PcmMicInStartLoopDMA(VOICE_MC_DMAC_CHN_MIC, VOICE_MicDmaIsr);

        /* 标志当前MIC处DMA状态为已启动 */
        pstDmaFlag->usMicInRun = CODEC_SWITCH_ON;

        /* 记录日志 */
        OM_LogInfo(VOICE_McStartPcmTrans_MicIn);
    }
    /* 若为PC VOICE模式，则启动20ms周期性定时器，在定时器中再进行上行DMA搬运配置*/
    else if (CODEC_INOUT_DATA == pstInout->usTxIn)
    {
        /* 先停止20ms定时器 */
        DRV_TIMER_StopPrecise(&(pstTimer->uwTimer20ms));

        /* 给自己发编码消息 */
        uwRet = DRV_TIMER_Start(&(pstTimer->uwTimer01msTx),
                                1,
                                DRV_TIMER_MODE_ONESHOT,
                                VOICE_Timer20msIsr,
                                0);

        /* 重新启动20ms定时器 */
        uwRet = DRV_TIMER_StartPrecise(&(pstTimer->uwTimer20ms),
                        200,
                        DRV_TIMER_MODE_PERIODIC,
                        VOICE_Timer20msIsr,
                        0);
    }
    else
    {
        uwRet = UCOM_RET_ERR_PARA;
    }

    return uwRet;
}


VOS_UINT32 VOICE_McUpdate(VOS_UINT16 uhwModemNo)
{
    VOS_UINT32                          uwRet;
    VCVOICE_DEV_MODE_ENUM_UINT16        enDevMode;                /* 语音设备类型 */
    CODEC_NET_MODE_ENUM_UINT16          enNetMode;                /* 网络模式 */
    VOICE_MC_DMA_FLAG_STRU             *pstDmaFlag     = VOICE_McGetDmaFlagPtr();
    VOS_UINT32                          swSampleRate   = VOICE_ProcGetSampleRate();

    /* 注意: 由于Proc中的PP模块初始化时依赖于Pcm模块中的Frame Length设置,
       因此PcmUpdate先于ProcUpdate */
    /* 获取设备模式 */
    enDevMode = VOICE_McGetDeviceMode();

    /* 获取网络模式 */
    enNetMode = VOICE_McGetModemNetMode(uhwModemNo);

    /* 更新数据IO模块(数据通道8K/16K更新) */
    uwRet = VOICE_PcmUpdate(swSampleRate, enDevMode);

    if(uwRet != UCOM_RET_SUCC)
    {
        return uwRet;
    }

    /* 重置DMA */
    pstDmaFlag->usMicInRun  = CODEC_SWITCH_OFF;
    pstDmaFlag->usSpkOutRun = CODEC_SWITCH_OFF;

    /* 更新信号处理模块(PP参数更新) */
    uwRet = VOICE_ProcUpdate(enDevMode,enNetMode);

    if(uwRet != UCOM_RET_SUCC)
    {
        return uwRet;
    }

    return uwRet;
}
VOS_UINT32 VOICE_McPreCfgCodecType(VOS_UINT16 uhwNetMode, VOS_UINT16 *puhwCodecType)
{
    VOS_UINT16 uhwCodecType = *puhwCodecType;

    /* 参数有效性判断 */
    if (uhwNetMode >= CODEC_NET_MODE_BUTT)
    {
        OM_LogError(VOICE_McCheckStartMsg_InvalidNetMode);
        return UCOM_RET_ERR_PARA;
    }

    if (uhwCodecType >= CODEC_BUTT)
    {
        OM_LogError(VOICE_McCheckStartMsg_InvalidCodecType);
        return UCOM_RET_ERR_PARA;
    }

    if (CODEC_NET_MODE_W == uhwNetMode)
    {
        /* WCDMA 声码器 */
        if (   (uhwCodecType != CODEC_AMR)
            && (uhwCodecType != CODEC_AMR2)
            && (uhwCodecType != CODEC_AMRWB) )
        {
            OM_LogError(VOICE_McCheckStartMsg_UnmatchableWcdmaCodec);
            return UCOM_RET_ERR_PARA;
        }

        /* 内部AMR统一处理 */
        if (CODEC_AMR2 == uhwCodecType)
        {
            *puhwCodecType = CODEC_AMR;
        }
    }
    else if (CODEC_NET_MODE_TD == uhwNetMode)
    {
        /* TDS-CDMA 只支持AMR声码器 */
        if ( (uhwCodecType != CODEC_AMR)
            && (uhwCodecType != CODEC_AMR2))
        {
            OM_LogError(VOICE_McCheckStartMsg_UnmatchableTDCodec);
            return UCOM_RET_ERR_PARA;
        }

        /* 设置AMR类型 */
        VOICE_TDAmrModeCtrlSetType(uhwCodecType);

        /* 内部AMR统一处理 */
        if (CODEC_AMR2 == uhwCodecType)
        {
            *puhwCodecType = CODEC_AMR;
        }
    }
    else if(CODEC_NET_MODE_G == uhwNetMode)
    {
        /* GSM 声码器 */
        if (   (CODEC_AMR2 == uhwCodecType)
            || (CODEC_G711 == uhwCodecType))
        {
            OM_LogError(VOICE_McCheckStartMsg_UnmatchableGsmCodec);
            return UCOM_RET_ERR_PARA;
        }
    }
    else
    {
        /* IMS 声码器 */
        if (   (uhwCodecType != CODEC_AMR)
            && (uhwCodecType != CODEC_AMR2)
            && (uhwCodecType != CODEC_AMRWB)
            && (uhwCodecType != CODEC_G711))
        {
            OM_LogError(VOICE_McCheckStartMsg_UnmatchableImsCodec);
            return UCOM_RET_ERR_PARA;
        }

        /* 内部AMR统一处理 */
        if (CODEC_AMR2 == uhwCodecType)
        {
            *puhwCodecType = CODEC_AMR;
        }
    }

    return UCOM_RET_SUCC;
}
VOS_VOID VOICE_McSetCodecType(VOS_UINT16 uhwNetMode, VOS_UINT16 uhwCodecType, VOS_UINT16 uhwModemNo)
{
    VOICE_PROC_CODEC_STRU          *pstCodec = VOS_NULL;

    /* 网络模式更新 */
    VOICE_McSetModemNetMode(uhwModemNo, uhwNetMode);

    /* 声码器类型更新 */
    pstCodec = VOICE_McGetModemCodecPtr(uhwModemNo);
    pstCodec->enCodecType = uhwCodecType;
    pstCodec->usIsEncInited  = VOICE_PROC_IS_NO;
    pstCodec->usIsDecInited  = VOICE_PROC_IS_NO;
}


VOS_VOID VOICE_McPostCfgCodecType(VOS_UINT16 uhwCodecType,VOS_UINT16 uhwModemNo)
{
    CODEC_NET_MODE_ENUM_UINT16      enNetMode;              /* 网络制式 */
    VOS_UINT32                      uwReceiverPid;
    VOICE_MC_MODEM_STRU            *pstModem   = VOS_NULL;
    VOICE_MC_DMA_FLAG_STRU         *pstDmaFlag = VOICE_McGetDmaFlagPtr();
    VOS_UINT32                      uwUlTimer;
    VOS_UINT32                      uwDlTimer;

    /* 获取通路对象 */
    pstModem  = VOICE_McGetModemObjPtr(uhwModemNo);

    /* 获取网络制式 */
    enNetMode = pstModem->enNetMode;

    /* 根据网络制式进行处理 */
    switch(enNetMode)
    {
       case CODEC_NET_MODE_G:
       {
            /* 通知GPHY声码器配置事件 */
            uwReceiverPid = g_auwModemMcPidTbl[uhwModemNo][VOICE_MC_GPHY_INDEX];
            VOICE_McInformGphySetCodec(uwReceiverPid);
       }
       break;
       case CODEC_NET_MODE_W:
       {
            /* 设置AMR类型(AMR / AMR2 / AMRWB) */
            uwReceiverPid = g_auwModemMcPidTbl[uhwModemNo][VOICE_MC_WTTF_INDEX];
            VOICE_UmtsAmrModeCtrlSetType(uhwCodecType,uwReceiverPid);
       }
       break;
       case CODEC_NET_MODE_TD:
       {
            /* 根据V9R1 <<Hi6620V100 SOC逻辑需求规格表_DMA请求和中断分配.xlsx>>,这里为tds_stu_hifi_int的ID请求号 */
            /* TD使能5ms帧同步中断 */
            VOS_EnableInterrupt(DRV_TDSCDMA_CTRL_SYNC_INTR);

            /* 使能屏蔽寄存器，使其状态为不屏蔽状态 */
            UCOM_RegBitWr(DRV_TDSCDMA_CTRL_EN_ADDR, UCOM_BIT0, UCOM_BIT0, 1);
       }
       break;
       case CODEC_NET_MODE_L:
       {
            /* 起上下行DMA */
            /*若下行DMA标志为未启动，则启动5ms定时器，产生IMS下行DMA启动时刻*/
            if (CODEC_SWITCH_OFF == pstDmaFlag->usSpkOutRun)
            {
                /* 启动下行DMA */
                DRV_TIMER_Start(&uwDlTimer,
                                VOICE_IMS_RX_BEGIN_TIMESTAMP,
                                DRV_TIMER_MODE_ONESHOT,
                                VOICE_Timer6msIsr,
                                0);
            }

            /*若上行DMA标志为未启动，则启动4ms定时器，产生IMS上行DMA启动时刻*/
            if (CODEC_SWITCH_OFF == pstDmaFlag->usMicInRun)
            {
                /* 启动上行DMA */
                DRV_TIMER_Start(&uwUlTimer,
                                VOICE_IMS_TX_BEGIN_TIMESTAMP,
                                DRV_TIMER_MODE_ONESHOT,
                                VOICE_Timer3msIsr,
                                0);
            }

            /* IMS下语音编码参数更新,默认速率wb为23.05，nb为12.2 */
            VOICE_ProcCfgImsCodec(uhwCodecType,CODEC_DTX_DISABLE,CODEC_AMR_RATE_MODE_122K);
       }
       break;
       default:
       {
       }
    }
}
VOS_UINT16 VOICE_McDetectRxFrmLost(VOS_VOID)
{
    VOS_UINT16              uhwRxFrmLost     = VOICE_PROC_IS_NO;
    VOICE_DEC_SERIAL_STRU  *pshwDecBuf       = VOICE_ProcGetDecBufPtr();
    VOICE_DEC_SERIAL_STRU  *pDecSerialWrite  = VOICE_ProcGetDecSerialWritePtr();

    /* 物理层下行丢帧 */
    if(pDecSerialWrite <= pshwDecBuf)
    {
        /* 下行丢帧 */
        uhwRxFrmLost = VOICE_PROC_IS_YES;

        /* 记录异常 */
        OM_LogWarning(VOICE_McRxFrmLost);
    }

    return(uhwRxFrmLost);
}
VOS_VOID VOICE_McTxDataToWphy(VOS_VOID)
{
    VOICE_WPHY_TX_DATA_IND_STRU     stEncRslt;
    VOS_UINT32                      uwReceiverPid;
    VOICE_MC_MODEM_NUM_ENUM_UINT16  enModemNo  = VOICE_MC_MODEM_NUM_BUTT;

    /* 获取前台modem的通道号 */
    enModemNo = VOICE_McGetForeGroundNum();

    if(VOICE_MC_MODEM_NUM_BUTT == enModemNo)
    {
        return ;
    }

    UCOM_MemSet(&stEncRslt, 0, sizeof(stEncRslt));

    stEncRslt.uhwMsgId   = (VOS_UINT16)ID_VOICE_WPHY_TX_DATA_IND;

    /* 填充ABC子流长度及AMR码流数据 */
    UCOM_MemCpy((VOICE_WPHY_AMR_SERIAL_STRU*)&stEncRslt.enQualityIdx,
                VOICE_ProcGetEncBufPtr(),
                sizeof(VOICE_WPHY_AMR_SERIAL_STRU));

    /* 获取PID */
    uwReceiverPid = g_auwModemMcPidTbl[enModemNo][VOICE_MC_WPHY_INDEX];

    /* ID_VOICE_WPHY_TX_DATA_IND消息发送给WPHY */
    UCOM_SendOsaMsg(DSP_PID_VOICE,
                    uwReceiverPid,
                    &stEncRslt,
                    sizeof(VOICE_WPHY_TX_DATA_IND_STRU));

    return;
}


VOS_VOID VOICE_McTxDataToGphy(VOS_VOID)
{
    VOICE_GPHY_TX_DATA_IND_STRU     stEncRslt;
    VOS_UINT32                      uwReceiverPid;
    VOICE_MC_MODEM_NUM_ENUM_UINT16  enModemNo   = VOICE_MC_MODEM_NUM_BUTT;
    VOICE_PROC_CODEC_STRU          *pstCodec    = VOICE_ProcGetCodecPtr();

    /* 获取前台modem的通道号 */
    enModemNo = VOICE_McGetForeGroundNum();

    if(VOICE_MC_MODEM_NUM_BUTT == enModemNo)
    {
        return ;
    }

    if (VOICE_PROC_IS_YES == VOICE_McGetGsmCodecTypeMismatch(enModemNo))
    {
        return ;
    }

    UCOM_MemSet(&stEncRslt, 0, sizeof(stEncRslt));

    /* 填充ID_VOICE_GPHY_TX_DATA_IND消息各项内容 */
    stEncRslt.uhwMsgId        = (VOS_UINT16)ID_VOICE_GPHY_TX_DATA_IND;
    stEncRslt.enSpFlag        = pstCodec->stEncOutObj.enSpFlag;
    stEncRslt.enVadFlag       = pstCodec->stEncOutObj.enVadFlag;
    stEncRslt.enAmrFrameType  = pstCodec->stEncOutObj.enAmrFrameType;
    stEncRslt.enAmrMode       = pstCodec->stEncOutObj.enAmrMode;
    stEncRslt.enCodecType     = pstCodec->enCodecType;

    /* 填充编码后语音数据 */
    UCOM_MemCpy(stEncRslt.auhwData,
                VOICE_ProcGetEncBufPtr(),
                (GPHY_VOICE_DATA_LEN * sizeof(VOS_UINT16)));

    /* 获取PID */
    uwReceiverPid = g_auwModemMcPidTbl[enModemNo][VOICE_MC_GPHY_INDEX];

    /* 给GPHY发送ID_VOICE_GPHY_TX_DATA_IND消息 */
    UCOM_SendOsaMsg(DSP_PID_VOICE,
                    uwReceiverPid,
                    &stEncRslt,
                    sizeof(VOICE_GPHY_TX_DATA_IND_STRU));

    return;
}
VOS_VOID VOICE_McTxDataToTDphy(VOS_VOID)
{
    VOICE_TDPHY_TX_DATA_IND_STRU      stEncRslt;
    VOS_UINT32                        uwReceiverPid;
    VOICE_MC_MODEM_NUM_ENUM_UINT16    enModemNo  = VOICE_MC_MODEM_NUM_BUTT;
    VOICE_PROC_CODEC_STRU            *pstCodec   = VOICE_ProcGetCodecPtr();

    /* 获取前台modem的通道号 */
    enModemNo = VOICE_McGetForeGroundNum();

    if(VOICE_MC_MODEM_NUM_BUTT == enModemNo)
    {
        return ;
    }

    UCOM_MemSet(&stEncRslt, 0, sizeof(stEncRslt));

    stEncRslt.uhwMsgId                 = (VOS_UINT16)ID_VOICE_TDPHY_TX_DATA_IND;
    stEncRslt.uhwFrameType             = (VOS_UINT16)((pstCodec->stEncOutObj).enAmrFrameType);

    /* 填充ABC子流长度及AMR码流数据 */
    UCOM_MemCpy((VOICE_WPHY_AMR_SERIAL_STRU*)&stEncRslt.enQualityIdx,
                VOICE_ProcGetEncBufPtr(),
                sizeof(VOICE_WPHY_AMR_SERIAL_STRU));

    /* 获取PID */
    uwReceiverPid = g_auwModemMcPidTbl[enModemNo][VOICE_MC_TDPHY_INDEX];

    /* ID_MED_TDPHY_TX_DATA_IND消息发送给TDPHY */
    UCOM_SendOsaMsg(DSP_PID_VOICE,
                    uwReceiverPid,
                    &stEncRslt,
                    sizeof(VOICE_TDPHY_TX_DATA_IND_STRU));

    return;
}


VOS_VOID VOICE_McTxDataToImsa(VOS_VOID)
{
    VOICE_IMSA_TX_DATA_IND_STRU     stEncRslt;
    VOS_UINT32                      uwReceiverPid;
    VOS_INT16                      *pEncBuf         = VOICE_ProcGetEncBufPtr();
    VOICE_MC_MODEM_NUM_ENUM_UINT16  enModemNo       = VOICE_MC_MODEM_NUM_BUTT;

    /* 获取前台modem的通道号 */
    enModemNo = VOICE_McGetForeGroundNum();

    /* modem号合法性检查 */
    if(VOICE_MC_MODEM_NUM_BUTT <= enModemNo)
    {
        return ;
    }

    /* 获取PID */
    uwReceiverPid = g_auwModemMcPidTbl[enModemNo][VOICE_MC_IMSA_INDEX];

    /* PID合法性检查 */
    if (PID_UNSET == uwReceiverPid)
    {
        OM_LogWarning1(VOICE_McTxReceivePid_UNSET,enModemNo);
        return ;
    }

    UCOM_MemSet(&stEncRslt, 0, sizeof(VOICE_IMSA_TX_DATA_IND_STRU));

    /* 拷贝上行编码数据 */
    UCOM_MemCpy(&(stEncRslt.usStatus),
                &pEncBuf[0],
                sizeof(IMS_UL_DATA_OBJ_STRU));

    stEncRslt.usMsgId = ID_VOICE_IMSA_TX_DATA_IND;

    VOICE_JB_SendImsaPkt(&stEncRslt);

    OM_LogFreqInfo(VOICE_McTxDataToImsa_Done, VOICE_LOG_PERIOD);

    return;
}
VOS_VOID VOICE_McChkMicConflick(VOS_VOID)
{
    VOS_UINT32                  uwTimeStamp      = 0;
    VOICE_MC_DMA_FLAG_STRU     *pstDmaFlag       = VOICE_McGetDmaFlagPtr();
    VOS_UINT16                  uhwActiveModemNo = VOICE_McGetForeGroundNum();
    VOS_UINT16                  uhwNetMode       = VOICE_McGetForeGroundNetMode(uhwActiveModemNo);
    VOS_UINT32                  uwMaxEncOffset;

    /* 如果当前网络模式UMTS模则进行上行冲突检测 */
    if (CODEC_NET_MODE_W == uhwNetMode)
    {
        uwMaxEncOffset = VOICE_UMTS_MAX_ENC_OFFSET;
    }

    /* TD网络模式下进行上行时序冲突检测 */
    else if(CODEC_NET_MODE_TD== uhwNetMode)
    {
        uwMaxEncOffset = VOICE_TDS_MAX_ENC_OFFSET;
    }
    else
    {
        return;
    }

    /* 读取SOC计数器，获取当前时戳 */
    uwTimeStamp = DRV_TIMER_ReadSysTimeStamp();

    /* 检查该时刻距离本帧头位置是否超过9.5ms */
    if (((UCOM_COMM_CycSub(uwTimeStamp, pstDmaFlag->uwSyncTimestamp, 0xffffffff)
        * VOICE_TIMESTAMP_MS_RATIO) / DRV_TIMER_GetOmFreq())
        >= (uwMaxEncOffset - pstDmaFlag->uwSyncOffset))
    {
        /* DMA切换冲突, 需重置DMA */
        pstDmaFlag->usMicInRun  = CODEC_SWITCH_OFF;

        pstDmaFlag->usSpkOutRun  = CODEC_SWITCH_OFF;

        /* 记录异常 */
        OM_LogWarning(VOICE_McChkMicConflick_MicInDmaRest);

        VOICE_ErrLogEventRec(CODEC_ERRLOG_EVENTS_DMA_MIC_RESET);
    }
}


VOS_VOID VOICE_McGsmChkMicConflick(VOS_VOID)
{
    VOS_UINT32                  uwTimeStamp      = 0;
    VOICE_MC_DMA_FLAG_STRU     *pstDmaFlag       = VOICE_McGetDmaFlagPtr();
    VOS_UINT32                  uwTimeDiff;
    VOS_UINT32                  uwTimeOffset;


    /* 读取SOC计数器，获取当前时戳 */
    uwTimeStamp = DRV_TIMER_ReadSysTimeStamp();

    /* 计算当前时刻距离同步时刻的时间差,模20ms，单位0.1ms */
    uwTimeDiff = ((UCOM_COMM_CycSub(uwTimeStamp, pstDmaFlag->uwSyncTimestamp, 0xffffffff)
                 * VOICE_TIMESTAMP_MS_RATIO) / DRV_TIMER_GetOmFreq()) + pstDmaFlag->uwSyncOffset;

    /* 计算当前时刻相对物理层要求上报TX_DATA的时刻偏移，单位0.1ms */
    uwTimeOffset = ((uwTimeDiff + VOICE_GSM_ONE_FRAME_OFFSET) - pstDmaFlag->uwChanCodeTime) % VOICE_GSM_ONE_FRAME_OFFSET;

    /* 检查该时刻是否超过GPhy要求上报TX_DATA的最晚时刻 */
    if (uwTimeOffset < VOICE_GSM_MIN_OFFSET)
    {
        /* DMA切换冲突, 需重置DMA */
        pstDmaFlag->usMicInRun  = CODEC_SWITCH_OFF;

        pstDmaFlag->usSpkOutRun  = CODEC_SWITCH_OFF;

        /* 记录异常 */
        OM_LogWarning(VOICE_McChkMicRightConflick_MicInDmaRest);

        VOICE_ErrLogEventRec(CODEC_ERRLOG_EVENTS_DMA_MIC_RESET);
    }

    /* 检查该时刻是否早于GPhy要求上报TX_DATA的最早时刻 */
    if (uwTimeOffset > VOICE_GSM_MAX_OFFSET)
    {
        /* DMA切换冲突, 需重置DMA */
        pstDmaFlag->usMicInRun   = CODEC_SWITCH_OFF;

        pstDmaFlag->usSpkOutRun  = CODEC_SWITCH_OFF;

        /* 记录异常 */
        OM_LogWarning(VOICE_McChkMicLeftConflick_MicInDmaRest);

        VOICE_ErrLogEventRec(CODEC_ERRLOG_EVENTS_DMA_MIC_RESET);
    }
}
VOS_VOID VOICE_McChkSpkConflick(VOS_VOID)
{
    VOICE_MC_DMA_FLAG_STRU          *pstDmaFlag       = VOICE_McGetDmaFlagPtr();
    CODEC_INOUT_STRU                *pstInout         = VOICE_McGetInoutPtr();
    VOS_UINT16                       uhwActiveModemNo = VOICE_McGetForeGroundNum();
    VOICE_GPHY_SYNC_REQ_STRU         stSyncReqMsg;
    UCOM_MemSet(&stSyncReqMsg, 0, sizeof(stSyncReqMsg));

    /* 若当前模式为非PC VOICE模式(如手机模式)，则可能存在下行时序冲突 */
    /* PC VOICE下解码后启DMA的机制保证了不会出现下行播放时序冲突 */
    if (CODEC_INOUT_DATA != pstInout->usRxOut)
    {
        /* 若SpkOut处DMA中断时刻检测到还未接收到新的下行数据，则认为出现时序冲突 */
        if (CODEC_SWITCH_OFF == pstDmaFlag->usSpkOutSwEnable)
        {
            /* 出现下行时序冲突, 重置语音运行时序，*/
            pstDmaFlag->usMicInRun  = CODEC_SWITCH_OFF;

            pstDmaFlag->usSpkOutRun  = CODEC_SWITCH_OFF;

            /* 记录异常 */
            OM_LogWarning(VOICE_McChkSpkConflick_SpkOutDmaRest);

            /* 无前台返回 */
            if(VOICE_MC_MODEM_NUM_BUTT <= uhwActiveModemNo)
            {
                /* ErrorLog 记录事件 */
                VOICE_ErrLogEventRec(CODEC_ERRLOG_EVENTS_DMA_SPK_RESET);
                return ;
            }

            /* ErrorLog 记录事件 */
            VOICE_ErrLogEventRec(CODEC_ERRLOG_EVENTS_DMA_SPK_RESET);
        }
        /* 若未出现冲突,则标志该帧下行数据已使用 */
        else
        {
            pstDmaFlag->usSpkOutSwEnable = CODEC_SWITCH_OFF;
        }
    }
}
VOS_VOID VOICE_McTdChkMicConflick(VOS_VOID)
{
    VOS_UINT32                          uwTimeStamp = 0;
    VOICE_MC_DMA_FLAG_STRU             *pstDmaFlag;

    pstDmaFlag  = VOICE_McGetDmaFlagPtr();

    /* 读取SOC计数器，获取当前时戳 */
    uwTimeStamp = DRV_TIMER_ReadSysTimeStamp();

    /* 检查该时刻距离本帧头位置是否超过16.7ms */
    if (((UCOM_COMM_CycSub(uwTimeStamp, pstDmaFlag->uwSyncTimestamp, 0xffffffff)
        * VOICE_TIMESTAMP_MS_RATIO) / DRV_TIMER_GetOmFreq())
        >= (VOICE_TDS_MAX_ENC_OFFSET - pstDmaFlag->uwSyncOffset))
    {
        /* DMA切换冲突, 需重置DMA */
        pstDmaFlag->usMicInRun  = CODEC_SWITCH_OFF;
        pstDmaFlag->usSpkOutRun = CODEC_SWITCH_OFF;

        /* 记录异常 */
        OM_LogWarning(VOICE_McChkMicConflick_MicInDmaRest);

        VOICE_ErrLogEventRec(CODEC_ERRLOG_EVENTS_DMA_MIC_RESET);
    }
}


VOS_VOID VOICE_McSetDevChannel(VOS_UINT16 uhwDevMode)
{
    CODEC_INOUT_STRU    *pstInout = VOICE_McGetInoutPtr();

    /* 根据场景模式配置上行输入通道和下行输出通道 */
    if (VCVOICE_DEV_MODE_PCVOICE != uhwDevMode)
    {
        pstInout->usTxIn        = CODEC_INOUT_DEFAULT;
        pstInout->usRxOut       = CODEC_INOUT_DEFAULT;
    }
    else
    {
        pstInout->usTxIn        = CODEC_INOUT_DATA;
        pstInout->usRxOut       = CODEC_INOUT_DATA;
    }

}

/*****************************************************************************
 函 数 名  : VOICE_McInformGphySetCodec
 功能描述  : 通知GPHY编解码类型被设置
 输入参数  : VOS_VOID
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2010年4月10日
    作    者   : 苏庄銮 59026
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID VOICE_McInformGphySetCodec(VOS_UINT32 uwReceiverPid)
{
    VOICE_GPHY_SET_CODEC_IND_STRU   stSetCodecInd;

    UCOM_MemSet(&stSetCodecInd, 0, sizeof(stSetCodecInd));

    /* 填充消息ID */
    stSetCodecInd.uhwMsgId    = ID_VOICE_GPHY_SET_CODEC_IND;

    /* 发送消息，消息目的地址为PID_GPHY，消息ID为ID_CODEC_GPHY_SET_CODEC_IND */
    UCOM_SendOsaMsg(DSP_PID_VOICE,
                    uwReceiverPid,/* UCOM_PID_DSP_GPHY */
                    &stSetCodecInd,
                    sizeof(VOICE_GPHY_SET_CODEC_IND_STRU));

    return;
}
VOS_VOID VOICE_McSendCnfMsgToVc(VOS_UINT16 uhwMsgId,VOS_UINT32 uwReceiverPid, VOS_UINT32 uwRet)
{
    VCVOICE_OP_RSLT_STRU    stCnfMsg;

    /* 清空消息 */
    UCOM_MemSet(&stCnfMsg, 0, sizeof(VCVOICE_OP_RSLT_STRU));

    /* 配置消息和返回值 */
    stCnfMsg.usMsgName        = uhwMsgId;
    stCnfMsg.enExeRslt        = (VOS_UINT16)uwRet;

    /* 根据uwReceiverPid,回复消息给相应NAS */
    UCOM_SendOsaMsg(DSP_PID_VOICE,
                    uwReceiverPid,
                    &stCnfMsg,
                    sizeof(VCVOICE_OP_RSLT_STRU));
}
VOS_UINT32 VOICE_McRegisterCaptureCallBack(VOICE_MC_CALLBACK pfunCallBack)
{
    if(VOS_NULL == pfunCallBack)
    {
        return UCOM_RET_FAIL;
    }
    else
    {
        VOICE_McSetCaptureCallBack(pfunCallBack);
    }

    return UCOM_RET_SUCC;
}


VOS_VOID VOICE_McUnRegisterCaptureCallBack(VOS_VOID)
{
    VOICE_McSetCaptureCallBack(VOS_NULL);
}

/*****************************************************************************
 函 数 名  : VOICE_Timer3msIsr
 功能描述  : W下3ms定时器中断处理函数,注册后在DRV层被调用
 输入参数  : uwTimer   -- 定时器句柄
             uwPara    -- 中断函数入参,注册时传入
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年6月3日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID VOICE_Timer3msIsr(VOS_UINT32 uwTimer, VOS_UINT32 uwPara)
{
    GPHY_VOICE_RECORD_IND_STRU  stRecordIndMsg;

    UCOM_MemSet(&stRecordIndMsg, 0, sizeof(stRecordIndMsg));

    /* 处理UMTS下启动的3ms定时,手机模式时启动上行DMA链式搬运或PC VOICE下的20ms定时 */
    /* 填充消息ID */
    stRecordIndMsg.uhwMsgId = ID_VOICE_VOICE_RECORD_IND;

    /* 发送语音采集消息ID_CODEC_CODEC_RECORD_IND给PID_VOICE_RT,开始上行语音采集 */
    UCOM_SendOsaMsg(DSP_PID_VOICE,
                    DSP_PID_VOICE_RT,
                    &stRecordIndMsg,
                    sizeof(GPHY_VOICE_RECORD_IND_STRU));

    return;
}

/*****************************************************************************
 函 数 名  : VOICE_Timer6msIsr
 功能描述  : W下6ms定时器中断处理函数,注册后在DRV层被调用
 输入参数  : uwTimer   -- 定时器句柄
             uwPara    -- 中断函数入参,注册时传入
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年6月3日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID VOICE_Timer6msIsr(VOS_UINT32 uwTimer, VOS_UINT32 uwPara)
{
    GPHY_VOICE_PLAY_IND_STRU    stPlayIndMsg;

    UCOM_MemSet(&stPlayIndMsg, 0, sizeof(stPlayIndMsg));

    /* 处理UMTS下启动的6ms定时,手机模式时启动下行DMA链式搬运,PC VOICE模式时不做动作 */
    /* 填充消息ID */
    stPlayIndMsg.uhwMsgId = ID_VOICE_VOICE_PLAY_IND;

    /* 发送语音播放消息ID_CODEC_CODEC_PLAY_IND给PID_VOICE_RT,开始下行语音播放 */
    UCOM_SendOsaMsg(DSP_PID_VOICE,
                    DSP_PID_VOICE_RT,
                    &stPlayIndMsg,
                    sizeof(GPHY_VOICE_PLAY_IND_STRU));

    return;
}

/*****************************************************************************
 函 数 名  : VOICE_Timer20msIsr
 功能描述  : 20ms周期定时器中断处理函数,注册后在DRV层被调用
 输入参数  : uwTimer   -- 定时器句柄
             uwPara    -- 中断函数入参,注册时传入
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年6月3日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID VOICE_Timer20msIsr(VOS_UINT32 uwTimer, VOS_UINT32 uwPara)
{
    /* 启动上行Mic处DMA搬运 */
    VOICE_PcmStartDMAFromDataIn(VOICE_MC_DMAC_CHN_MIC);

    return;
}
VOS_VOID VOICE_Timer20msRxIsr(VOS_UINT32 uwTimer, VOS_UINT32 uwPara)
{
    /* 单声道数据输出 */
    VOICE_PcmStartDMAToDataOut(VOICE_MC_DMAC_CHN_SPK);

    return;
}

/*****************************************************************************
 函 数 名  : VOICE_SyncTdSubFrm5msIsr
 功能描述  : TD 5ms 子帧中断
 输入参数  :
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年6月3日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID VOICE_SyncTdSubFrm5msIsr(VOS_VOID)
{
    VOICE_VOICE_TD_SYNC_IND_STRU    stSyncInd;
    VOS_UINT32                      uwFrmCnt = 0;


    /* 因为5ms中断是电平中断，需要先进行清中断操作 */
    UCOM_RegBitWr(DRV_TDSCDMA_CTRL_INT_CLEAR_ADDR, UCOM_BIT0, UCOM_BIT0, 1);

    /* 读取帧中断寄存器内容，仅低16位有效 */
    uwFrmCnt = UCOM_RegRd(DRV_TDSCDMA_FRM_CNT_REG_ADDR);
    uwFrmCnt = uwFrmCnt & 0xffff;

    /* 按照设计，第4i(i∈整数)个5ms中断产生的时间和偶数帧帧头时间一致，此时MED给MED_RT发送同步消息。这里使用宏反而更不容易理解 */
    if (0 == (uwFrmCnt & 0x3))
    {
        stSyncInd.uhwMsgId = ID_VOICE_VOICE_TD_SYNC_IND;
        /* 发送编码消息ID_MED_MED_TD_SYNC_IND至PID_MED_RT,开始语音处理 */
        UCOM_SendOsaMsg(DSP_PID_VOICE,
                        DSP_PID_VOICE_RT,
                        &stSyncInd,
                        sizeof(VOICE_VOICE_TD_SYNC_IND_STRU));
    }
    return;
}


VOS_VOID VOICE_TdsTxTimerIsr(VOS_UINT32 uwTimer, VOS_UINT32 uwPara)
{
    GPHY_VOICE_RECORD_IND_STRU            stRecordIndMsg;

    UCOM_MemSet(&stRecordIndMsg, 0, sizeof(stRecordIndMsg));

    if (VOICE_MC_STATE_RUNNING == VOICE_McGetForeGroundVoiceState())
    {
        /* 处理TDS下启动的0.3ms定时 */
        /* 填充消息ID */
        stRecordIndMsg.uhwMsgId = ID_VOICE_VOICE_RECORD_IND;

        /* 发送语音采集消息ID_VOICE_VOICE_RECORD_IND给DSP_PID_VOICE_RT,开始上行语音采集 */
        UCOM_SendOsaMsg(DSP_PID_VOICE,
                        DSP_PID_VOICE_RT,
                        &stRecordIndMsg,
                        sizeof(GPHY_VOICE_RECORD_IND_STRU));
    }
    else
    {
        OM_LogWarning(VOICE_TimerTxIsr_WarningState);
    }

    return;
}

/*****************************************************************************
 函 数 名  : VOICE_TdsRxTimerIsr
 功能描述  : W下6ms定时器中断处理函数,注册后在DRV层被调用
 输入参数  : uwTimer   -- 定时器句柄
             uwPara    -- 中断函数入参,注册时传入
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年6月3日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID VOICE_TdsRxTimerIsr(VOS_UINT32 uwTimer, VOS_UINT32 uwPara)
{
    GPHY_VOICE_PLAY_IND_STRU              stPlayIndMsg;

    UCOM_MemSet(&stPlayIndMsg, 0, sizeof(stPlayIndMsg));

    if (VOICE_MC_STATE_RUNNING == VOICE_McGetForeGroundVoiceState())
    {
        /* 处理TDS-CDMA下启动的6ms定时 */
        /* 填充消息ID */
        stPlayIndMsg.uhwMsgId = ID_VOICE_VOICE_PLAY_IND;

        /* 发送语音播放消息ID_VOICE_VOICE_PLAY_IND给PID_VOICE_RT,开始下行语音播放 */
        UCOM_SendOsaMsg(DSP_PID_VOICE,
                        DSP_PID_VOICE_RT,
                        &stPlayIndMsg,
                        sizeof(GPHY_VOICE_PLAY_IND_STRU));
    }
    else
    {
        OM_LogWarning(VOICE_TimerRxIsr_WarningState);
    }

    return;
}
VOS_VOID VOICE_Timer30msDiagIsr(VOS_UINT32 uwTimer, VOS_UINT32 uwPara)
{
    HIFI_ERR_EVT_VOICE_CONTROL_FAULT_STRU stVoiceCtrlErr;
    VOS_UINT16                            uhwActiveModemNo = VOICE_McGetForeGroundNum();
    VOICE_PROC_CODEC_STRU                *pstCodec         = VOICE_McGetModemCodecPtr(uhwActiveModemNo);

    UCOM_MemSet(&stVoiceCtrlErr, 0, sizeof(stVoiceCtrlErr));

   /* 没有收到上行DMA中断或下行DMA中断 */
    if((VOICE_DIAG_FALSE == g_stDiagDmaIsr.uhwTxDmaIsrFlag)
     ||(VOICE_DIAG_FALSE == g_stDiagDmaIsr.uhwRxDmaIsrFlag))
    {
        stVoiceCtrlErr.uhwErrorReason = VOICE_CONTROL_FAULT;
        stVoiceCtrlErr.uhwDevMode     = VOICE_McGetDeviceMode();
        stVoiceCtrlErr.uhwActiveModem = uhwActiveModemNo;
        stVoiceCtrlErr.uhwNetMode     = VOICE_McGetModemNetMode(uhwActiveModemNo);
        stVoiceCtrlErr.uhwCodecType   = pstCodec->enCodecType;
        stVoiceCtrlErr.uhwTxDmaErr    = g_stDiagDmaIsr.uhwTxDmaIsrFlag;
        stVoiceCtrlErr.uhwRxDmaErr    = g_stDiagDmaIsr.uhwRxDmaIsrFlag;

        /* 上报语音流程错误告警 */
        VOICE_DiagAlarmReport(uhwActiveModemNo,
                              VOICE_CONTROL_FAULT,
                              &stVoiceCtrlErr,
                              sizeof(stVoiceCtrlErr),
                              HIFI_ERR_LOG_VOICE_CONTROL_FAULT);
    }
    else
    {
        /* 清空本次记录 */
        g_stDiagDmaIsr.uhwTxDmaIsrFlag = VOICE_DIAG_FALSE;
        g_stDiagDmaIsr.uhwRxDmaIsrFlag = VOICE_DIAG_FALSE;
    }

    return;
}
VOS_UINT32 VOICE_MsgApNoteCCPUResetRP(VOS_VOID *pstOsaMsg)
{

    HIFI_AP_CCPU_RESET_CNF_STRU    stCnfMsg;

    /* 如果1通路处于通话状态，则停止通话状态 */
    if (VOICE_MC_STATE_RUNNING == VOICE_McGetModemVoiceState(VOICE_MC_MODEM0))
    {
        /* 停止语音软件运行 */
        VOICE_McStop(VOICE_MC_MODEM0);

        /* 设置当前激活Modem号 */
        VOICE_McSetForeGroundNum(VOICE_MC_MODEM_NUM_BUTT);

        /* 语音对象初始化 */
        VOICE_McModemObjInit(VOICE_McGetModemObjPtr(VOICE_MC_MODEM0));
    }

    /* 如果2通路处于通话状态，则停止通话状态 */
    if (VOICE_MC_STATE_RUNNING == VOICE_McGetModemVoiceState(VOICE_MC_MODEM1))
    {
        /* 停止语音软件运行 */
        VOICE_McStop(VOICE_MC_MODEM1);

        /* 设置当前激活Modem号 */
        VOICE_McSetForeGroundNum(VOICE_MC_MODEM_NUM_BUTT);

        /* 语音对象初始化 */
        VOICE_McModemObjInit(VOICE_McGetModemObjPtr(VOICE_MC_MODEM1));
    }

    UCOM_MemSet(&stCnfMsg, 0, sizeof(stCnfMsg));

    /* 回复消息ID_AP_HIFI_CCPU_RESET_REQ */
    stCnfMsg.uhwMsgId   = ID_HIFI_AP_CCPU_RESET_CNF;
    stCnfMsg.uhwResult  = VOS_OK;

    UCOM_SendDrvMsg(DSP_PID_VOICE,
                ACPU_PID_OM,
                &stCnfMsg,
                sizeof(HIFI_AP_CCPU_RESET_CNF_STRU));

    /* 记录日志 */
    OM_LogInfo(VOICE_MsgApNoteCCPUReset_SUCC);

    return VOS_OK;

}


VOS_UINT32 VOICE_MsgVoiceLoopReqIRP(VOS_VOID *pstOsaMsg)
{
    VOS_UINT32              uwRet;
    VCVOICE_LOOP_REQ_STRU  *pstVcLoopSet;

    pstVcLoopSet = (VCVOICE_LOOP_REQ_STRU *)pstOsaMsg;

    if (VCVOICE_LOOP_START == pstVcLoopSet->enMode)
    {
        CODEC_PcmSetPcmFrameLength(CODEC_FRAME_LENGTH_NB);
        VOICE_McSetVoiceState(VOICE_MC_MODEM0, VOICE_MC_STATE_RUNNING);

        /* 网络模式更新，因为只有网络状态为RUNNING态且为非BUTT时，才能响应该消息 */
        VOICE_McSetModemNetMode(VOICE_MC_MODEM0, CODEC_NET_MODE_W);

        uwRet = VOICE_McSetLoopEnable();
    }
    else
    {
        uwRet = VOICE_McSetLoopDisable();

        /* 网络模式更新为MODE_BUTT */
        VOICE_McSetModemNetMode(VOICE_MC_MODEM0, CODEC_NET_MODE_BUTT);

        VOICE_McSetVoiceState(VOICE_MC_MODEM0, VOICE_MC_STATE_IDLE);
    }

    /* 回复NAS执行结果 */
    VOICE_McSendCnfMsgToVc(ID_VOICE_VC_LOOP_CNF, pstVcLoopSet->uwSenderPid, uwRet);

    return uwRet;

}
VOS_UINT32 VOICE_McSetLoopEnable(VOS_VOID)
{
    VOS_UINT16                      uhwIsModeMaster;
    DRV_SIO_INT_MASK_ENUM_UINT32    enMask;
    VOS_UINT32                      uwRet;

    enMask  = DRV_SIO_INT_MASK_RX_RIGHT_FIFO_OVER;
    enMask |= DRV_SIO_INT_MASK_RX_LEFT_FIFO_OVER;
    enMask |= DRV_SIO_INT_MASK_TX_RIGHT_FIFO_UNDER;
    enMask |= DRV_SIO_INT_MASK_TX_LEFT_FIFO_UNDER;

    /* 读取SIO主从模式设置 */
    UCOM_NV_Read( en_NV_Item_SIO_Voice_Master,
                 &uhwIsModeMaster,
                  sizeof(uhwIsModeMaster));

    uwRet  = DRV_SIO_Voice_Open(enMask, DRV_SIO_SAMPLING_8K, uhwIsModeMaster, VOICE_SioIsr, 0);

    uwRet += VOICE_PcmMicInStartLoopDMA(VOICE_MC_DMAC_CHN_MIC, VOICE_PcmVcLoopMicIsr);

    uwRet += VOICE_PcmSpkOutStartLoopDMA(VOICE_MC_DMAC_CHN_SPK, VOICE_PcmVcLoopSpkIsr);

    if (UCOM_RET_SUCC == uwRet)
    {
        return UCOM_RET_SUCC;
    }
    else
    {
        return UCOM_RET_FAIL;
    }
}
VOS_UINT32 VOICE_McSetLoopDisable(VOS_VOID)
{
    DRV_SIO_Voice_Close();

    DRV_DMA_Stop(VOICE_MC_DMAC_CHN_MIC);
    DRV_DMA_Stop(VOICE_MC_DMAC_CHN_SPK);

    return UCOM_RET_SUCC;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

