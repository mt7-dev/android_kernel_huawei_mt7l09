/******************************************************************************

                  版权所有 (C), 2012-2013, 华为技术有限公司

 ******************************************************************************
  文 件 名   : audio_debug.c
  版 本 号   : 初稿
  作    者   : 王贵林 W164657
  生成日期   : 2012年7月26日
  最近修改   :
  功能描述   : 调试功能
  函数列表   :
              AUDIO_DEBUG_Init
              AUDIO_DEBUG_Info_Query
              AUDIO_DEBUG_SendHookInd
              AUDIO_DEBUG_MsgSetHookReq

  修改历史   :
  1.日    期   : 2012年7月26日
    作    者   : 王贵林 W164657
    修改内容   : 创建文件

******************************************************************************/

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "ucom_config.h"
#include "ucom_nv.h"
#include "audio_debug.h"
#include "om_comm.h"
#include "med_drv_timer_hifi.h"
#include "audio_enhance.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
    可维可测信息中包含的C文件编号宏定义
*****************************************************************************/
/*lint -e(767)*/
#define THIS_FILE_ID                    OM_FILE_ID_AUDIO_DEBUG_C

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

/* HOOK使能、钩取目标、帧号 */
AUDIO_HOOK_STRU   g_stAudioDbgHookObj = {CODEC_SWITCH_ON, 0x07};

/*****************************************************************************
  3 函数实现
*****************************************************************************/

/*****************************************************************************
 函 数 名  : AUDIO_DEBUG_Init
 功能描述  : 音频调试模块初始化
 输入参数  : pstAudioTraceCfg       --输入的NV参数，按固定NV格式
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月26日
    作    者   : 王贵林 W164657
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID AUDIO_DEBUG_Init( CODEC_NV_PARA_AUDIO_TRACE_CFG_STRU *pstAudioTraceCfg )
{
    AUDIO_HOOK_STRU               *pstHookCtrl = AUDIO_UtilGetHookPtr();

    /* 按输入的nv参数进行初始化赋值 */
    pstHookCtrl->enHookEnable   = (pstAudioTraceCfg->stAudioHookCfg).uhwEnable;
    pstHookCtrl->usHookTarget   = (pstAudioTraceCfg->stAudioHookCfg).uhwTarget;

}
VOS_UINT32 AUDIO_DEBUG_MsgQueryStatusReq(VOS_VOID *pstOsaMsg)
{
    MSG_AUDIO_OM_QUERY_RSP_STRU         stQueryMsg;
    AUDIO_PCM_CFG_STRU                 *pstPcmCfg   = VOS_NULL;

    UCOM_MemSet(&stQueryMsg, 0, sizeof(stQueryMsg));

    /* 获取播放通道的输入或输出描述结构体 */
    pstPcmCfg   = AUDIO_PcmGetPcmCfgPtr(AUDIO_PCM_MODE_PLAYBACK);

    stQueryMsg.usMsgId              = ID_AUDIO_OM_QUERY_STATUS_CNF;
    stQueryMsg.enPlayBackStatus     = pstPcmCfg->enPcmStatus;
    stQueryMsg.uwPlayBackSampleRate = pstPcmCfg->uwSampleRate;
    stQueryMsg.uwPlayBackChannelNum = pstPcmCfg->uwChannelNum;

    /* 获取录音通道的输入或输出描述结构体 */
    pstPcmCfg   = AUDIO_PcmGetPcmCfgPtr(AUDIO_PCM_MODE_CAPTURE);

    stQueryMsg.enCaptureStatus      = pstPcmCfg->enPcmStatus;
    stQueryMsg.enCaptureMode        = pstPcmCfg->enCaptureMode;
    stQueryMsg.uwCaptureSampleRate  = pstPcmCfg->uwSampleRate;
    stQueryMsg.uwCaptureChannelNum  = pstPcmCfg->uwChannelNum;

    /* 获取音频增强通路当前模块 */
    stQueryMsg.uwEnhanceModule      = AUDIO_ENHANCE_GetCurrentModule();

    /*调用OM_COMM_SendTranMsg接口进行Trans头填充，并发送给SDT*/
    OM_COMM_SendTranMsg(&stQueryMsg, sizeof(MSG_AUDIO_OM_QUERY_RSP_STRU));

    OM_LogInfo(AUDIO_MsgQueryStatusReq_Ok);

    return UCOM_RET_SUCC;
}

/*****************************************************************************
 函 数 名  : AUDIO_DEBUG_MsgSetHookReq
 功能描述  : 音频钩取数据设置原语处理函数
 输入参数  : pstOsaMsg          --音频数据钩取设置消息ID_AUDIO_OM_SET_HOOK_REQ
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月26日
    作    者   : 王贵林 W164657
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 AUDIO_DEBUG_MsgSetHookReq(VOS_VOID *pstOsaMsg)
{
    VOS_UINT32                            uwRet      = UCOM_RET_SUCC;
    MSG_OM_AUDIO_HOOK_REQ_STRU           *pstPrim    = VOS_NULL;
    AUDIO_HOOK_STRU                      *pstHook    = AUDIO_UtilGetHookPtr();
    MSG_CODEC_CNF_STRU                    stRspPrim;

    if (VOS_NULL == pstOsaMsg)
    {
        return UCOM_RET_NUL_PTR;
    }

    UCOM_MemSet(&stRspPrim, 0, sizeof(stRspPrim));

    /* 原语ID合法性判断由于已经有消息状态机保证，此处不做判断 */
    pstPrim = (MSG_OM_AUDIO_HOOK_REQ_STRU *)pstOsaMsg;

    /* 参数合法性判断 */
    if ((CODEC_SWITCH_BUTT <= pstPrim->enHookEnable)
       || (AUDIO_HOOK_BUTT <= pstPrim->usHookTarget))
    {
        uwRet = UCOM_RET_ERR_PARA;

        /* 记录日志信息 */
        OM_LogError(AUDIO_MsgSetHookReq_Fail);
    }
    else
    {
        /* 从原语中获取数据钩取相关变量值更新全局变量 */
        pstHook->enHookEnable   = pstPrim->enHookEnable;
        pstHook->usHookTarget   = pstPrim->usHookTarget;

        uwRet = UCOM_RET_SUCC;

        /* 记录日志信息 */
        OM_LogInfo(AUDIO_MsgSetHookReq_Ok);
    }

    /* 填充消息ID和执行结果 */
    stRspPrim.usMsgName = ID_AUDIO_OM_SET_HOOK_CNF;
    stRspPrim.enExeRslt = (VOS_UINT16)uwRet;

    /* 向SDT回复消息ID_AUDIO_OM_SET_HOOK_CNF */
    OM_COMM_SendTranMsg(&stRspPrim, sizeof(stRspPrim));

    return uwRet;
}

/*****************************************************************************
 函 数 名  : AUDIO_DEBUG_SendHookInd
 功能描述  : 钩取指定位置数据进行上报
 输入参数  : uhwPos:  钩取数据位置
             pvData:  钩取数据buffer地址
             uhwLen:  钩取数据实际内容长度(32bit)
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月26日
    作    者   : 王贵林 W164657
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID AUDIO_DEBUG_SendHookInd(
                VOS_UINT16              uhwPos,
                VOS_VOID               *pvData,
                VOS_UINT32              uwLen)
{
    MSG_AUDIO_OM_HOOK_STRU       stHookHead;
    UCOM_DATA_BLK_STRU           astBlk[2];
    AUDIO_HOOK_STRU             *pstHookObj  = AUDIO_UtilGetHookPtr();
    VOS_UCHAR                   *pvSubData   = (VOS_UCHAR *)pvData;
    VOS_UINT16                   uhwHookLen  = 0;
    VOS_UINT16                   uhwBlkCnt   = 0;
    VOS_UINT16                   uhwLoop     = 0;

    if (CODEC_SWITCH_OFF == pstHookObj->enHookEnable )
    {
        return;
    }

    if(uhwPos == (uhwPos & (pstHookObj->usHookTarget)))
    {

        uhwBlkCnt = ((VOS_UINT16)uwLen / AUDIO_MAX_HOOK_LEN) + 1;

        /* 填充原语内容 */
        stHookHead.usMsgId      = (VOS_UINT16)ID_AUDIO_OM_HOOK_IND;
        stHookHead.usHookTarget = uhwPos;

        /* 循环发送blk数据，每次最多3840 byte pvData */
        for(uhwLoop = 0; uhwLoop < uhwBlkCnt; uhwLoop++)
        {
            if(uhwLoop != (uhwBlkCnt - 1))
            {
                uhwHookLen = AUDIO_MAX_HOOK_LEN;
            }
            else
            {
                uhwHookLen = (VOS_UINT16)(uwLen - (uhwLoop * AUDIO_MAX_HOOK_LEN));
            }

            stHookHead.ulTimeStamp  = DRV_TIMER_ReadSysTimeStamp();
            stHookHead.usHookLength = uhwHookLen;

            /* 填充地址和长度 */
            astBlk[0].pucData       = (VOS_UCHAR *)&stHookHead;
            astBlk[0].uwSize        = sizeof(stHookHead);

            /* 填充地址和长度 */
            astBlk[1].pucData       = (VOS_UCHAR *)pvSubData;
            astBlk[1].uwSize        = uhwHookLen;

            /* 上报HOOK信息 */
            OM_COMM_SendTrans(astBlk, 2);

            pvSubData               = pvSubData + uhwHookLen;
        }
    }

    pvSubData = VOS_NULL;
}
VOS_UINT32 AUDIO_DEBUG_MsgAudioLoopOpenReq(VOS_VOID *pvOsaMsg)
{
    /* 待后续实现 */
    AUDIO_LOOP_CNF_STRU         stAudioLoopCnfMsg;

    /* 判定环回设置是否成功 */
    if ( UCOM_RET_SUCC == DRV_SIO_Audio_Loop(DRV_SIO_SAMPLING_48K))
    {
        stAudioLoopCnfMsg.enAudioLoopCnf    = AUDIO_PLAY_RESULT_OK;
    }
    else
    {
        stAudioLoopCnfMsg.enAudioLoopCnf    = AUDIO_PLAY_RESULT_FAIL;
        DRV_SIO_Audio_Close();
    }

    /* 按AUDIO_PLAYER_DONE_IND_STRU格式填充消息 */
    stAudioLoopCnfMsg.uhwMsgId          = ID_AUDIO_AP_LOOP_BEGIN_CNF;


    /* 调用跨核音频channel接口,通知AP音频环回结果 */
    UCOM_SendAudioMsg(DSP_PID_AUDIO,
                      ACPU_PID_OM,           /* 此处填写AP侧DRV对应的PID，待定 */
                     &stAudioLoopCnfMsg,
                      sizeof(AUDIO_LOOP_CNF_STRU));

    return UCOM_RET_SUCC;
}
VOS_UINT32 AUDIO_DEBUG_MsgAudioLoopCloseReq(VOS_VOID *pvOsaMsg)
{
    AUDIO_LOOP_CNF_STRU         stAudioLoopCnfMsg;

    /* 关闭SIO */
    DRV_SIO_Audio_Close();

    /* 按AUDIO_PLAYER_DONE_IND_STRU格式填充消息 */
    stAudioLoopCnfMsg.uhwMsgId          = ID_AUDIO_AP_LOOP_END_CNF;
    stAudioLoopCnfMsg.enAudioLoopCnf    = AUDIO_PLAY_RESULT_OK;

    /* 调用跨核音频channel接口,通知AP关闭音频环回 */
    UCOM_SendAudioMsg(DSP_PID_AUDIO,
                      ACPU_PID_OM,           /* 此处填写AP侧DRV对应的PID,,待定 */
                     &stAudioLoopCnfMsg,
                      sizeof(AUDIO_LOOP_CNF_STRU));

    return UCOM_RET_SUCC;
}


VOS_UINT32  AUDIO_DEBUG_MsgTestCmd(VOS_VOID *pvOsaMsg)
{
    VOS_UINT32          uwRetValue  = 0x55aa55aa;
    TEST_HIFI_MSG_STRU *pstTestMsg  = (TEST_HIFI_MSG_STRU*)((MsgBlock *) pvOsaMsg)->aucValue;

    VOS_UINT32          uwLen       = sizeof(TEST_HIFI_MSG_STRU);

    UCOM_COMM_SendMsg(DSP_PID_AUDIO,DSP_PID_AUDIO_RT,DRV_MAILBOX_CHANNEL_VOS_CH,
        ((VOS_CHAR*)pstTestMsg+uwLen),pstTestMsg->ulMsgLen);

    UCOM_SendTestMsg(DSP_PID_AUDIO,ACPU_PID_OM,&uwRetValue,sizeof(VOS_UINT32));

    return UCOM_RET_SUCC;
}

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

