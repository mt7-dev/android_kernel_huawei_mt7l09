/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : VOICE_debug.c
  版 本 号   : 初稿
  作    者   : 谢明辉 58441
  生成日期   : 2011年7月4日
  最近修改   :
  功能描述   : 调试功能
  函数列表   :
              VOICE_DbgCheckAndLoop
              VOICE_DbgInfoHookCfg
              VOICE_DbgSendHook
              VOICE_MsgOmSetHookReqIRP
              VOICE_MsgOmSetLoopReqIRP
  修改历史   :
  1.日    期   : 2011年7月4日
    作    者   : 谢明辉 58441
    修改内容   : 创建文件

******************************************************************************/

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "om.h"
#include "ucom_config.h"
#include "ucom_nv.h"
#include "voice_debug.h"
#include "voice_proc.h"
#include "voice_pcm.h"
#include "med_drv_timer_hifi.h"
#include "ucom_pcm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
    可维可测信息中包含的C文件编号宏定义
*****************************************************************************/
/*lint -e(767)*/
#define THIS_FILE_ID                    OM_FILE_ID_VOICE_DEBUG_C

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
/* 环回相关设置 */
CODEC_LOOP_STRU   g_stVoiceDbgLoop = {CODEC_SWITCH_OFF, CODEC_LOOP_BUTT};

/* HOOK使能、钩取目标、帧号 */
VOICE_HOOK_STRU   g_stVoiceDbgHookObj = {CODEC_SWITCH_OFF, 0, 0};

/*****************************************************************************
  3 函数实现
*****************************************************************************/

/*****************************************************************************
 函 数 名  : VOICE_DbgInit
 功能描述  : 调试模块初始化
 输入参数  : VOS_VOID
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年11月17日
    作    者   : 苏庄銮 59026
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID VOICE_DbgInit( CODEC_NV_PARA_VOICE_TRACE_CFG_STRU *pstVoiceTraceCfg )
{
    CODEC_LOOP_STRU                      *pstLoopObj  = VOICE_DbgGetLoopFLagPtr();
    VOICE_HOOK_STRU                      *pstHookCtrl = VOICE_DbgGetHookPtr();

    pstLoopObj->enLoopEnable    = (pstVoiceTraceCfg->stLoopCfg).uhwEnable;
    pstLoopObj->enLoopType      = (pstVoiceTraceCfg->stLoopCfg).uhwType;

    pstHookCtrl->enHookEnable   = (pstVoiceTraceCfg->stVoiceHookCfg).uhwEnable;
    pstHookCtrl->usHookTarget   = (pstVoiceTraceCfg->stVoiceHookCfg).uhwTarget;
    pstHookCtrl->ulFrameTick    = 0;

}

/*****************************************************************************
 函 数 名  : VOICE_MsgOmSetLoopReqIRP
 功能描述  : 设置环回相关变量
 输入参数  : pstOsaMsg：指向消息数据的指针
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年6月10日
    作    者   : 谢明辉 58441
    修改内容   : 修改函数头注释模板

*****************************************************************************/
VOS_UINT32 VOICE_MsgOmSetLoopReqIRP(VOS_VOID *pstOsaMsg)
{
    VOS_UINT32                            uwRet       = UCOM_RET_SUCC;
    MSG_OM_VOICE_LOOP_REQ_STRU           *pstPrim     = VOS_NULL;
    CODEC_LOOP_STRU                      *pstLoopObj  = VOICE_DbgGetLoopFLagPtr();
    MSG_CODEC_CNF_STRU                    stRspPrim;

    UCOM_MemSet(&stRspPrim, 0, sizeof(stRspPrim));

    /*原语ID合法性判断由于已经有消息状态机保证，此处不做判断 */
    pstPrim = (MSG_OM_VOICE_LOOP_REQ_STRU *)pstOsaMsg;

    /*参数合法性判断*/
    if((pstPrim->stLoop.enLoopEnable > CODEC_SWITCH_ON)
        || (pstPrim->stLoop.enLoopType > CODEC_LOOP_CODEC))
    {
        uwRet = UCOM_RET_ERR_PARA;
    }
    else
    {
        /*设置环回类型*/
        pstLoopObj->enLoopType          = pstPrim->stLoop.enLoopType;

        /*设置环回打开或关闭*/
        pstLoopObj->enLoopEnable        = pstPrim->stLoop.enLoopEnable;
    }

    /* 填充消息ID和执行结果 */
    stRspPrim.usMsgName                 = ID_VOICE_OM_SET_LOOP_CNF;
    stRspPrim.enExeRslt                 = (VOS_UINT16)uwRet;

    /* 向SDT回复消息ID_VOICE_OM_SET_LOOP_CNF */
    OM_COMM_SendTranMsg(&stRspPrim, sizeof(stRspPrim));

    /* 记录日志信息 */
    OM_LogInfo(VOICE_MsgSetLoopReq_Ok);

    return uwRet;
}
VOS_UINT32 VOICE_MsgOmSetHookReqIRP(VOS_VOID *pstOsaMsg)
{
    VOS_UINT32                            uwRet      = UCOM_RET_SUCC;
    MSG_OM_VOICE_HOOK_REQ_STRU           *pstPrim    = VOS_NULL;
    VOICE_HOOK_STRU                      *pstHook    = VOICE_DbgGetHookPtr();
    MSG_CODEC_CNF_STRU                    stRspPrim;

    UCOM_MemSet(&stRspPrim, 0, sizeof(stRspPrim));

    /*原语ID合法性判断由于已经有消息状态机保证，此处不做判断 */
    pstPrim = (MSG_OM_VOICE_HOOK_REQ_STRU *)pstOsaMsg;

    /*参数合法性判断*/
    if ((CODEC_SWITCH_BUTT <= pstPrim->enHookEnable)
        || (VOICE_HOOK_BUTT <= pstPrim->usHookTarget))
    {
        uwRet = UCOM_RET_ERR_PARA;

        /* 记录日志信息 */
        OM_LogInfo(VOICE_MsgSetHookReq_Fail);
    }
    else
    {
        /* 从原语中获取数据钩取相关变量值更新全局变量 */
        pstHook->enHookEnable   = pstPrim->enHookEnable;
        pstHook->usHookTarget   = pstPrim->usHookTarget;
        pstHook->ulFrameTick    = 0;

        /* 通知OM模块同步语音数据钩取设置信息 */
        /* PC Voice功能在V9R1上不实现，暂时屏蔽 */
        /* VOICE_DbgInfoHookCfg(); */

        uwRet = UCOM_RET_SUCC;

        /* 记录日志信息 */
        OM_LogInfo(VOICE_MsgSetHookReq_Ok);
    }

    /* 填充消息ID和执行结果 */
    stRspPrim.usMsgName = ID_VOICE_OM_SET_HOOK_CNF;
    stRspPrim.enExeRslt = (VOS_UINT16)uwRet;

    /* 向SDT回复消息ID_VOICE_OM_SET_HOOK_CNF */
    OM_COMM_SendTranMsg(&stRspPrim, sizeof(stRspPrim));

    return uwRet;
}

/*****************************************************************************
 函 数 名  : VOICE_DbgCheckAndLoop
 功能描述  : 检查是否环回
 输入参数  : uhwPos:     环回位置
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年6月10日
    作    者   : 谢明辉 58441
    修改内容   : 修改函数注释模板

*****************************************************************************/
VOS_VOID VOICE_DbgCheckAndLoop(CODEC_LOOP_ENUM_UINT16 uhwPos)
{
    VOICE_PROC_CODEC_STRU           *pstCodec   = VOICE_ProcGetCodecPtr();
    CODEC_LOOP_STRU                 *pstLoopObj = VOICE_DbgGetLoopFLagPtr();

    if ( (CODEC_SWITCH_OFF == pstLoopObj->enLoopEnable)
        || (uhwPos != pstLoopObj->enLoopType) )
    {
        return;
    }

    if (CODEC_LOOP_CODEC == uhwPos)
    {
        /* 编解码参数转换环回 */

        pstCodec->stDecInObj.enTafFlag     = CODEC_TAF_NONE;    /* EFR/HR/FR始终无边界 */
        pstCodec->stDecInObj.enBfiFlag     = CODEC_BFI_NO;      /* EFR/HR/FR始终为好帧 */
        pstCodec->stDecInObj.enHrUfiFlag   = CODEC_BFI_NO;      /* HR始终为好帧 */

        /* EFR/HR/FR帧类型匹配 */
        if (CODEC_SP_SPEECH == pstCodec->stEncOutObj.enSpFlag)
        {
            pstCodec->stDecInObj.enSidFlag = CODEC_SID_SPEECH;  /* 接收到SPEECH帧 */
        }
        else
        {
            pstCodec->stDecInObj.enSidFlag = CODEC_SID_VALID;   /* 接收到SID帧 */
        }

        /* AMR帧类型匹配 */
        switch (pstCodec->stEncOutObj.enAmrFrameType)
        {
            case CODEC_AMR_TYPE_TX_SPEECH_GOOD:
            {
                pstCodec->stDecInObj.enAmrFrameType = CODEC_AMR_TYPE_RX_SPEECH_GOOD;
                break;
            }
            case CODEC_AMR_TYPE_TX_SID_FIRST:
            {
                pstCodec->stDecInObj.enAmrFrameType = CODEC_AMR_TYPE_RX_SID_FIRST;
                break;
            }
            case CODEC_AMR_TYPE_TX_SID_UPDATE:
            {
                pstCodec->stDecInObj.enAmrFrameType = CODEC_AMR_TYPE_RX_SID_UPDATE;
                break;
            }
            default:
            {
                pstCodec->stDecInObj.enAmrFrameType = CODEC_AMR_TYPE_RX_NO_DATA;
            }
        }

        /* 码流环回 */
        UCOM_MemCpy((void*)&g_stVoiceCodecData.astDecSerial[0].asDecSerial[0],
                    (void*)&g_stVoiceCodecData.asEncSerial[0],
                    (VOICE_CODED_FRAME_WITH_OBJ_LEN * sizeof(VOS_UINT16)));

    }
    else if (CODEC_LOOP_DMA == uhwPos)
    {
        /* DMA环回 */
        UCOM_MemCpy((void*)g_psVoicePcmSpkOut,
                    (void*)g_psVoicePcmMicIn,
                    (VOS_UINT16)CODEC_PCM_FRAME_LENGTH_BYTES*UCOM_PCM_I2S_CHANNEL_NUM);
    }
    else
    {
        /* 编码前环回 if (CODEC_LOOP_BEFORE_CODEC == uhwPos) */
        UCOM_MemCpy((void*)&g_stVoiceCodecData.asLineInBuff[0],
                    (void*)&g_stVoiceCodecData.asLineOutBuff[0],
                    (VOS_UINT16)CODEC_PCM_FRAME_LENGTH_BYTES);
    }

    return;
}

/*****************************************************************************
 函 数 名  : VOICE_DbgInfoHookCfg
 功能描述  : 通知DataTransfer模块数据钩取配置参数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年11月4日
    作    者   : 苏庄銮 59026
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID VOICE_DbgInfoHookCfg(VOS_VOID)
{
    VOICE_HOOK_STRU                      *pstHook    = VOICE_DbgGetHookPtr();
    MSG_CODEC_OM_HOOK_IND_STRU           stInfoMsg;

    /*从原语中获取数据钩取相关变量值更新全局变量*/
    stInfoMsg.usMsgId      = ID_VOICE_OM_SET_HOOK_IND;
    stInfoMsg.usReserve    = 0;
    stInfoMsg.enHookEnable = pstHook->enHookEnable;
    stInfoMsg.usHookTarget = pstHook->usHookTarget;

    UCOM_SendOsaMsg(DSP_PID_VOICE, ACPU_PID_PCVOICE, &stInfoMsg, sizeof(stInfoMsg));

}

/*****************************************************************************
 函 数 名  : VOICE_DbgSendHook
 功能描述  : 钩取指定位置数据进行上报
 输入参数  : uhwPos:  钩取数据位置
             pvData:  钩取数据buffer地址
             uhwLen:  钩取数据实际内容长度(16bit)
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年6月16日
    作    者   : 谢明辉 58441
    修改内容   : 修改函数注释模板

*****************************************************************************/
VOS_VOID VOICE_DbgSendHook(
                VOS_UINT16              uhwPos,
                VOS_VOID               *pvData,
                VOS_UINT16              uhwLen)
{
    VOS_UINT16                     uhwHookLen  = 0;
    MSG_VOICE_OM_HOOK_STRU         stHookHead;
    UCOM_DATA_BLK_STRU             astBlk[2];
    VOICE_HOOK_STRU               *pstHookObj = VOICE_DbgGetHookPtr();

    if (CODEC_SWITCH_OFF == pstHookObj->enHookEnable )
    {
        return;
    }

    if(uhwPos == (uhwPos & (pstHookObj->usHookTarget)))
    {
        uhwHookLen = (uhwLen > VOICE_MAX_HOOK_LEN) ? VOICE_MAX_HOOK_LEN : uhwLen;

        /* 填充原语内容 */
        stHookHead.usMsgId      = (VOS_UINT16)ID_VOICE_OM_HOOK_IND;
        stHookHead.usHookTarget = uhwPos;
        stHookHead.ulFrameTick  = pstHookObj->ulFrameTick;
        stHookHead.ulTimeStamp  = DRV_TIMER_ReadSysTimeStamp();
        stHookHead.usHookLength = (VOS_UINT16)(uhwHookLen * 2);

        /* 填充地址和长度 */
        astBlk[0].pucData       = (VOS_UCHAR *)&stHookHead;
        astBlk[0].uwSize        = sizeof(stHookHead);

        /* 填充地址和长度 */
        astBlk[1].pucData       = (VOS_UCHAR *)pvData;
        astBlk[1].uwSize        = uhwHookLen * 2;


        /* 上报HOOK信息 */
        OM_COMM_SendTrans(astBlk, 2);
    }
}


VOS_VOID VOICE_DbgSaveCodecObj(
                VOS_INT16                          *pshwCodedBuf,
                VOICE_DBG_CODEC_MODE_ENUM_UINT16 uhwCodecMode,
                VOS_VOID                           *pstCodec )
{
    VOS_UINT32 *puwDbgBuf;
    VOS_UINT32  uwFreeSize;

    /* 获取编解码码流结尾位置 */
    puwDbgBuf = (VOS_UINT32 *)&pshwCodedBuf[VOICE_CODED_FRAME_LENGTH];

    /* 在编解码码流末尾添加 Magic Number */
    *puwDbgBuf = VOICE_DBG_CODEC_TITLE_MAGIC_NUM;
    puwDbgBuf++;

    /* 根据编解码模式写入对应的 Magic Number */
    switch(uhwCodecMode)
    {
        case VOICE_DBG_CODEC_MODE_ENCODE:
        {
            *puwDbgBuf = VOICE_DBG_CODEC_ENCODE_MAGIC_NUM;
            break;
        }
        case VOICE_DBG_CODEC_MODE_DECODE:
        {
            *puwDbgBuf = VOICE_DBG_CODEC_DECODE_MAGIC_NUM;
            break;
        }
        default:
        {
            /* 编解码模式错误,则重复写入TITLE */
            *puwDbgBuf = VOICE_DBG_CODEC_TITLE_MAGIC_NUM;
            break;
        }
    }
    puwDbgBuf++;

    /* 计算剩余可写入空间 */
    uwFreeSize = (((VOICE_CODED_FRAME_WITH_OBJ_LEN - VOICE_CODED_FRAME_LENGTH) * sizeof(VOS_INT16)) - (2*sizeof(VOS_UINT32)))
                  - sizeof(GPHY_VOICE_RX_DATA_IND_STRU);

    if((sizeof(VOICE_PROC_CODEC_STRU)) < uwFreeSize)
    {
        /* 写入编解码结构体 */
        UCOM_MemCpy(puwDbgBuf, pstCodec, sizeof(VOICE_PROC_CODEC_STRU));
    }
    else
    {
        /* 空间不足 */
        OM_LogError(VOICE_DbgSaveCodecObj_CannotWriteVOICE_PROC_CODEC_STRUtoDbgBuf);
    }

}

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

