/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : audio_pcm.c
  版 本 号   : 初稿
  作    者   : C00137131
  生成日期   : 2012年8月2日
  最近修改   :
  功能描述   : Hifi音频输入输出模块
  函数列表   :
              AUDIO_PCM_CaptureDmaIsr
              AUDIO_PCM_GetCurCaptureBuff
              AUDIO_PCM_GetCurPlayBuff
              AUDIO_PCM_MsgPcmCloseReq
              AUDIO_PCM_MsgPcmHwFreeReq
              AUDIO_PCM_MsgPcmHwParaReq
              AUDIO_PCM_MsgPcmOpenReq
              AUDIO_PCM_MsgPcmPointerReq
              AUDIO_PCM_MsgPcmPrepareReq
              AUDIO_PCM_MsgPcmSetBufCmd
              AUDIO_PCM_MsgPcmTriggerReq
              AUDIO_PCM_MsgUpdateCaptureBuffCmd
              AUDIO_PCM_MsgUpdatePlayBuffCmd
              AUDIO_PCM_PlayDmaIsr
              AUDIO_PCM_RegisterPcmCbFunc
              AUDIO_PCM_StartCaptureLoopDMA
              AUDIO_PCM_StartPlayLoopDMA
              AUDIO_PCM_UnRegisterPcmCbFunc
  修改历史   :
  1.日    期   : 2012年8月2日
    作    者   : C00137131
    修改内容   : 创建文件
******************************************************************************/

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "audio_pcm.h"
#include "med_drv_dma.h"
#include "med_drv_sio.h"
#include "om.h"
#include "ucom_comm.h"
#include "codec_op_vec.h"
#include "audio_debug.h"
#include "med_drv_timer_hifi.h"
#include "ucom_nv.h"
#include "audio_enhance.h"
#include "om_cpuview.h"
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
#define THIS_FILE_ID                    OM_FILE_ID_AUDIO_PCM_C

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

/* 保存音频通路相关控制信息 */
AUDIO_PCM_CTRL_OBJ_STRU                 g_stAudioPcmObj;

/* 保存音频播放、录音的Buffer信息 */
UCOM_SEC_TCMBSS
AUDIO_PCM_PINGPONG_BUF_STRU             g_stAudioPcmPbBuff;

AUDIO_PCM_PINGPONG_BUF_STRU             g_stAudioPcmCpBuff;

/* 保存音频播放、录音的临时Buffer信息 */
AUDIO_PCM_TMP_BUF_STRU                  g_stAudioPcmTmpBuff;

UCOM_SEC_TCMBSS
UCOM_ALIGN(32)
DRV_DMA_CXCFG_STRU                      g_astAudioPlayDmaPara[AUDIO_PCM_PINGPONG_BUF_NUM] = {0};/* 当前播放通道循环Buf搬运的DMA配置*/

UCOM_ALIGN(32)
DRV_DMA_CXCFG_STRU                      g_astAudioCaptureDmaPara[AUDIO_PCM_PINGPONG_BUF_NUM] = {0};/* 当前录音通道循环Buf搬运的DMA配置*/

/*****************************************************************************
  3 函数实现
*****************************************************************************/
/*****************************************************************************
 函 数 名  : AUDIO_PCM_Init
 功能描述  : Audio PCM模块初始化
 输入参数  : VOS_VOID
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月5日
    作    者   : C00137131
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID AUDIO_PCM_Init(VOS_VOID)
{
    AUDIO_PCM_CTRL_OBJ_STRU         *pstAudioPcmObj = AUDIO_PcmGetPcmObjPtr();
    VOS_VOID                        *pvTmp          = VOS_NULL;

    /* 初始化g_stAudioPcmObj为全0 */
    UCOM_MemSet(pstAudioPcmObj, 0, sizeof(AUDIO_PCM_CTRL_OBJ_STRU));

    /* 初始化g_stAudioPcmPbBuff全0，需保证Uncache */
    pvTmp   = (VOS_VOID *)AUDIO_PcmGetPcmPbBufPtr();

    UCOM_MemSet(pvTmp, 0, sizeof(AUDIO_PCM_PINGPONG_BUF_STRU));

    /* 初始化g_stAudioPcmCpBuff全0，需保证Uncache */
    pvTmp   = (VOS_VOID *)AUDIO_PcmGetPcmCpBufPtr();

    UCOM_MemSet(pvTmp, 0, sizeof(AUDIO_PCM_PINGPONG_BUF_STRU));

    /* 初始化g_astAudioPlayDmaPara全0，需保证Uncache */
    pvTmp   = (VOS_VOID *)AUDIO_PcmGetPlayDmaCfgPtr();

    UCOM_MemSet(pvTmp, 0, AUDIO_PCM_PINGPONG_BUF_NUM * sizeof(DRV_DMA_CXCFG_STRU));

    /* 初始化g_astAudioCaptureDmaPara全0，需保证Uncache */
    pvTmp   = (VOS_VOID *)UCOM_GetUncachedAddr((VOS_UINT32)AUDIO_PcmGetCaptureDmaCfgPtr());

    UCOM_MemSet(pvTmp, 0, AUDIO_PCM_PINGPONG_BUF_NUM * sizeof(DRV_DMA_CXCFG_STRU));

}

/*****************************************************************************
 函 数 名  : AUDIO_PCM_RegisterPcmCbFunc
 功能描述  : 注册PCM处理时Hifi的回调函数，如录音时的单MIC处理、上下行混音等
 输入参数  : AUDIO_PCM_MODE_ENUM_UINT16         enPcmMode    --输入输出模式
             AUDIO_PCM_OBJ_ENUM_UINT16          enPcmObj     --输入输出对象
             AUDIO_CAPTURE_MODE_ENUM_UINT16     enCaptureMode--录音模式
             AUDIO_PCM_CALLBACK                 pfunCallBack --回调函数
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月24日
    作    者   : C00137131
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 AUDIO_PCM_RegisterPcmCbFunc(
                AUDIO_PCM_MODE_ENUM_UINT16      enPcmMode,
                AUDIO_PCM_OBJ_ENUM_UINT16       enPcmObj,
                AUDIO_CAPTURE_MODE_ENUM_UINT16  enCaptureMode,
                AUDIO_PCM_CALLBACK              pfunCallBack)
{
    AUDIO_PCM_CFG_STRU                 *pstPcmCfg   = VOS_NULL;

    /* 检查入参 */
    if (  (enPcmMode >= AUDIO_PCM_MODE_BUT)
        ||(enPcmObj >= AUDIO_PCM_OBJ_BUT)
        ||(enCaptureMode >= AUDIO_CAPTURE_MODE_BUT))
    {
        return UCOM_RET_FAIL;
    }

    /* 获取该通道的输入或输出描述结构体 */
    pstPcmCfg   = AUDIO_PcmGetPcmCfgPtr(enPcmMode);

    /* 为防止通话录音注册函数被覆盖，通话录音时注册独立函数 */
    if (  (AUDIO_CAPTURE_MODE_CALL == enCaptureMode)
        ||(AUDIO_PCM_OBJ_HIFI == enPcmObj))
    {
        pstPcmCfg->astPcmBuf[enPcmObj].pfunVcRecordCB = pfunCallBack;
    }
    else
    {
        /* 将回调函数保存在通道描述结构体中，区分数据交互对象，即哪个数据需要调用该回调函数 */
        pstPcmCfg->astPcmBuf[enPcmObj].pfunCallBack = pfunCallBack;
    }

    /* 将录音类型保存于当前通道描述结构体中 */
    pstPcmCfg->enCaptureMode    = enCaptureMode;

    OM_LogInfo3(AUDIO_PCM_RegisterCbkOK,
                enPcmMode,
                enPcmObj,
                enCaptureMode);

    return UCOM_RET_SUCC;
}

/*****************************************************************************
 函 数 名  : AUDIO_PCM_UnRegisterPcmCbFunc
 功能描述  : 去注册PCM处理时Hifi的回调函数
 输入参数  : AUDIO_PCM_MODE_ENUM_UINT16 enPcmMode
             AUDIO_PCM_OBJ_ENUM_UINT16  enPcmObj
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月24日
    作    者   : C00137131
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 AUDIO_PCM_UnRegisterPcmCbFunc(
                AUDIO_PCM_MODE_ENUM_UINT16  enPcmMode,
                AUDIO_PCM_OBJ_ENUM_UINT16   enPcmObj)
{
    AUDIO_PCM_CFG_STRU                 *pstPcmCfg   = VOS_NULL;

    /* 检查入参 */
    if (  (enPcmMode >= AUDIO_PCM_MODE_BUT)
        ||(enPcmObj >= AUDIO_PCM_OBJ_BUT))
    {
        return UCOM_RET_FAIL;
    }

    /* 获取该通道的输入或输出描述结构体 */
    pstPcmCfg   = AUDIO_PcmGetPcmCfgPtr(enPcmMode);

    /* 将通道描述结构体中的回调函数清零 */
    pstPcmCfg->astPcmBuf[enPcmObj].pfunCallBack = VOS_NULL;

    return UCOM_RET_SUCC;
}

/*****************************************************************************
 函 数 名  : AUDIO_PCM_MsgPcmOpenReq
 功能描述  : 处理ID_AP_AUDIO_PCM_OPEN_REQ消息，启动Hifi音频输入或输出通道任务，
             区分上下行,该消息作为高优先级任务响应
 输入参数  : VOS_VOID *pvOsaMsg --ID_AP_AUDIO_PCM_OPEN_REQ消息体
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月5日
    作    者   : C00137131
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 AUDIO_PCM_MsgPcmOpenReq(VOS_VOID *pvOsaMsg)
{
    AUDIO_PCM_OPEN_REQ_STRU            *pstPcmOpenMsg   = VOS_NULL;

    /* 按AUDIO_PCM_OPEN_REQ_STRU格式解析消息 */
    pstPcmOpenMsg   = (AUDIO_PCM_OPEN_REQ_STRU*)((MsgBlock *) pvOsaMsg)->aucValue;

    /* 检查入参 */
    if (pstPcmOpenMsg->enPcmMode >= AUDIO_PCM_MODE_BUT)
    {
        return UCOM_RET_ERR_PARA;
    }
    else
    {
        /* 启用整体音频通道,更新PCM模块状态g_stAudioPcmObj.enPcmStatus */
        if (AUDIO_PCM_STATUS_DEACTIVE == AUDIO_PcmGetState())
        {
            AUDIO_PcmSetState(AUDIO_PCM_STATUS_ACTIVE);
        }
    }

    return UCOM_RET_SUCC;

}

/*****************************************************************************
 函 数 名  : AUDIO_PCM_MsgPcmCloseReq
 功能描述  : 处理ID_AP_AUDIO_PCM_CLOSE_REQ消息，停止Hifi输入或输出通道任务
 输入参数  : VOS_VOID *pvOsaMsg     --ID_AP_AUDIO_PCM_CLOSE_REQ消息体
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月5日
    作    者   : C00137131
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 AUDIO_PCM_MsgPcmCloseReq(VOS_VOID *pvOsaMsg)
{
    AUDIO_PCM_CLOSE_REQ_STRU           *pstPcmCloseMsg  = VOS_NULL;
    AUDIO_PCM_CFG_STRU                 *pstPcmCfg       = VOS_NULL;
    AUDIO_PCM_ENHANCE_STOP_IND_STRU     stEnhanceStop;

    /* 按AP_AUDIO_PCM_CLOSE_REQ_STRU格式解析消息 */
    pstPcmCloseMsg  = (AUDIO_PCM_CLOSE_REQ_STRU*)((MsgBlock *) pvOsaMsg)->aucValue;

    /* 检查入参 */
    if (pstPcmCloseMsg->enPcmMode >= AUDIO_PCM_MODE_BUT)
    {
        return UCOM_RET_ERR_PARA;
    }
    else
    {
        /* 获取该通道的输入或输出描述结构体 */
        pstPcmCfg   = AUDIO_PcmGetPcmCfgPtr(pstPcmCloseMsg->enPcmMode);

        if (  (AUDIO_PCM_STATUS_DEACTIVE == pstPcmCfg->astPcmBuf[AUDIO_PCM_OBJ_AP].enBufStatus)
            &&(AUDIO_PCM_STATUS_DEACTIVE == pstPcmCfg->astPcmBuf[AUDIO_PCM_OBJ_HIFI].enBufStatus))
        {
            /* 显式关闭播放或录音通道 */
            AUDIO_PcmSetSpeState(pstPcmCloseMsg->enPcmMode, AUDIO_PCM_STATUS_DEACTIVE);
        }

        /* 若所有通道已关闭，则整个PCM模块也关闭 */
        if (  (AUDIO_PCM_STATUS_DEACTIVE == AUDIO_PcmGetSpeState(AUDIO_PCM_MODE_PLAYBACK))
            &&(AUDIO_PCM_STATUS_DEACTIVE == AUDIO_PcmGetSpeState(AUDIO_PCM_MODE_CAPTURE)))
        {
            AUDIO_PcmSetState(AUDIO_PCM_STATUS_DEACTIVE);

            /* 关闭音频SIO设备或SlimBus通道设备 */
            /* AUDIO_PCM_IO_Close(); */
        }

        /* 关闭音频增强 */

        UCOM_MemSet(&stEnhanceStop, 0, sizeof(AUDIO_PCM_ENHANCE_STOP_IND_STRU));

        stEnhanceStop.uhwMsgId = ID_AUDIO_PCM_ENHANCE_STOP_IND;
        stEnhanceStop.enModule = AUDIO_ENHANCE_MODULE_DEFAULT;

        /* 调用通用VOS发送接口，发送消息 */
        UCOM_SendOsaMsg(DSP_PID_AUDIO_RT,
                        DSP_PID_AUDIO,
                       &stEnhanceStop,
                        sizeof(AUDIO_PCM_ENHANCE_STOP_IND_STRU));

    }

    return UCOM_RET_SUCC;

}


/*****************************************************************************
 函 数 名  : AUDIO_PCM_MsgPcmHwParaReq
 功能描述  : 处理ID_AP_AUDIO_PCM_HW_PARA_REQ消息，进行DMA通路的相关初始化
 输入参数  : VOS_VOID *pvOsaMsg     --ID_AP_AUDIO_PCM_HW_PARA_REQ消息体
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月10日
    作    者   : C00137131
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 AUDIO_PCM_MsgPcmHwParaReq(VOS_VOID *pvOsaMsg)
{
    AUDIO_PCM_HW_PARAMS_REQ_STRU       *pstPcmHwParaMsg = VOS_NULL;
    AUDIO_PCM_CFG_STRU                 *pstPcmCfg       = VOS_NULL;
    AUDIO_PCM_ENHANCE_START_IND_STRU    stEnhanceStartInd;

    /* 按AUDIO_PCM_HW_PARAMS_REQ_STRU格式解析消息 */
    pstPcmHwParaMsg   = (AUDIO_PCM_HW_PARAMS_REQ_STRU*)((MsgBlock *) pvOsaMsg)->aucValue;

    /* 检查入参 */
    if (  (pstPcmHwParaMsg->enPcmMode >= AUDIO_PCM_MODE_BUT)
        ||(pstPcmHwParaMsg->uwSampleRate > AUDIO_PCM_MAX_SAMPLE_RATE)
        ||(pstPcmHwParaMsg->uwChannelNum > AUDIO_PCM_MAX_CHANNEL_NUM))
    {
        return UCOM_RET_ERR_PARA;
    }
    else
    {
        /* 获取当前播放或录音通道的结构体指针 */
        pstPcmCfg   = AUDIO_PcmGetPcmCfgPtr(pstPcmHwParaMsg->enPcmMode);

        /* 保存指定通道的PCM信息，保存于对应的全局变量通道的AUDIO_PCM_CFG_STRU中 */
        pstPcmCfg->uwSampleRate = pstPcmHwParaMsg->uwSampleRate;
        pstPcmCfg->uwChannelNum = pstPcmHwParaMsg->uwChannelNum;

        /* 计算DMA搬运长度，暂仅支持16bit数据传递 */
        pstPcmCfg->uwBufSize    = ((pstPcmCfg->uwSampleRate*pstPcmCfg->uwChannelNum)\
                                    /AUDIO_PCM_FRAME_PER_SEC) * AUDIO_PCM_FORMAT_16_SIZE;

        /* 启动音频增强 */

        UCOM_MemSet(&stEnhanceStartInd, 0, sizeof(AUDIO_PCM_ENHANCE_START_IND_STRU));

        stEnhanceStartInd.uhwMsgId      = ID_AUDIO_PCM_ENHANCE_START_IND;
        stEnhanceStartInd.uwSampleRate  = pstPcmCfg->uwSampleRate;
        stEnhanceStartInd.enModule      = AUDIO_ENHANCE_MODULE_DEFAULT;

        /* 调用通用VOS发送接口，发送消息 */
        UCOM_SendOsaMsg(DSP_PID_AUDIO_RT,
                        DSP_PID_AUDIO,
                       &stEnhanceStartInd,
                        sizeof(AUDIO_PCM_ENHANCE_START_IND_STRU));
    }

    return UCOM_RET_SUCC;

}


/*****************************************************************************
 函 数 名  : AUDIO_PCM_MsgPcmHwFreeReq
 功能描述  : 处理ID_AP_AUDIO_PCM_HW_FREE_REQ消息，释放DMA等相关资源
 输入参数  : VOS_VOID *pvOsaMsg
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月10日
    作    者   : C00137131
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 AUDIO_PCM_MsgPcmHwFreeReq(VOS_VOID *pvOsaMsg)
{
    /* 该处理应该由AP侧PCM驱动直接完成，此处仅作为一个保留接口，后续可删除 */

    return UCOM_RET_SUCC;
}


/*****************************************************************************
 函 数 名  : AUDIO_PCM_MsgPcmPrepareReq
 功能描述  : 处理ID_AP_AUDIO_PCM_PREPARE_REQ消息，初始化相关设备
 输入参数  : VOS_VOID *pvOsaMsg
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月9日
    作    者   : C00137131
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 AUDIO_PCM_MsgPcmPrepareReq(VOS_VOID *pvOsaMsg)
{
    /* 按AUDIO_PCM_PREPARE_REQ_STRU结构解析消息 */

    return UCOM_RET_SUCC;

}

/*****************************************************************************
 函 数 名  : AUDIO_PCM_MsgPcmTriggerReq
 功能描述  : 处理ID_AP_AUDIO_PCM_TRIGGER_REQ消息，更新某数据通道的状态
 输入参数  : VOS_VOID *pvOsaMsg     --ID_AP_AUDIO_PCM_TRIGGER_REQ消息体
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月5日
    作    者   : C00137131
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 AUDIO_PCM_MsgPcmTriggerReq(VOS_VOID *pvOsaMsg)
{
    AUDIO_PCM_TRIGGER_REQ_STRU         *pstPcmTriggerMsg    = VOS_NULL;
    AUDIO_PCM_CFG_STRU                 *pstPcmCfg           = VOS_NULL;
    AUDIO_PCM_OBJ_ENUM_UINT16           enPcmObj            = AUDIO_PCM_OBJ_BUT;
    DRV_SIO_INT_MASK_ENUM_UINT32        enMask          = DRV_SIO_INT_MASK_RX_RIGHT_FIFO_OVER;
    CODEC_NV_PARA_SIO_AUDIO_MASTER_STRU stIsAudioMaster;

    VOS_UINT32                          uwRet               = UCOM_RET_SUCC;

    /* 按AUDIO_PCM_TRIGGER_REQ_STRU格式解析消息 */
    pstPcmTriggerMsg    = (AUDIO_PCM_TRIGGER_REQ_STRU*)((MsgBlock *) pvOsaMsg)->aucValue;

    /* 检查入参 */
    if (  (pstPcmTriggerMsg->enPcmMode >= AUDIO_PCM_MODE_BUT)
        ||(pstPcmTriggerMsg->enCmd >= AUDIO_PCM_TRIGGER_BUT)
        ||(pstPcmTriggerMsg->enPcmObj >= AUDIO_PCM_OBJ_BUT))
    {
        return UCOM_RET_ERR_PARA;
    }

    /* 获取Trigger的数据来源对象 */
    enPcmObj    = pstPcmTriggerMsg->enPcmObj;

    /* 获取该通道的输入或输出描述结构体 */
    pstPcmCfg   = AUDIO_PcmGetPcmCfgPtr(pstPcmTriggerMsg->enPcmMode);

    /*根据trigger指令进行相应操作*/
    switch(pstPcmTriggerMsg->enCmd)
    {
        case AUDIO_PCM_TRIGGER_START:
        case AUDIO_PCM_TRIGGER_RESUME:
        case AUDIO_PCM_TRIGGER_PAUSE_RELEASE:
        {

            /* 根据NV项设定PCM为主或从，在此读入NV项 */
            UCOM_NV_Read(en_NV_Item_SIO_Audio_Master, &(stIsAudioMaster.uhwIsPcmMaster), sizeof(CODEC_NV_PARA_SIO_AUDIO_MASTER_STRU));
            /* 打开音频SIO设备或SlimBus通道设备,若已经打开,要求该接口直接返回OK,不做重新打开的动作 */
            uwRet = AUDIO_PCM_IO_Open(enMask|DRV_SIO_INT_MASK_TX_RIGHT_FIFO_UNDER,
                                      pstPcmCfg->uwSampleRate,
                                      stIsAudioMaster.uhwIsPcmMaster,
                                      VOS_NULL,
                                      0);

            if (UCOM_RET_SUCC != uwRet)
            {
                return UCOM_RET_FAIL;
            }

            /* 保存该通道的输入或输出Buff信息至对应通道结构体中,须保证Uncache访问 */
            pstPcmCfg->astPcmBuf[enPcmObj].uwBufAddr = pstPcmTriggerMsg->uwBufAddr;

            /* 保存该通道的音频子流编号,该编号仅对AP音频通道有效 */
            pstPcmCfg->astPcmBuf[enPcmObj].pSubStream= pstPcmTriggerMsg->pSubStream;

            /* 更新该通道下Buff使用状态为使用状态 */
            pstPcmCfg->astPcmBuf[enPcmObj].enBufStatus = AUDIO_PCM_STATUS_ACTIVE;

            /* 初始化该通道下buff是否ready标志位 */
            pstPcmCfg->astPcmBuf[enPcmObj].enBufReadyFlag = AUDIO_PCM_BUFF_ISREADY_NO;

            /* 当前通道为播放通道且通道未正常工作则启用DMA及SIO */
            if (AUDIO_PCM_STATUS_ACTIVE != pstPcmCfg->enPcmStatus)
            {
                AUDIO_PcmSetState(AUDIO_PCM_STATUS_ACTIVE);

                pstPcmCfg->enPcmStatus  = AUDIO_PCM_STATUS_ACTIVE;

                /* 通道模式为播放 */
                if (AUDIO_PCM_MODE_PLAYBACK == pstPcmTriggerMsg->enPcmMode)
                {
                    /* 启动播放DMA */
                    uwRet = AUDIO_PCM_StartPlayLoopDMA((VOS_UINT16)pstPcmCfg->uwBufSize, AUDIO_PCM_DMAC_CHN_PLAY);
                }
                /* 当前通道为录音通道，且非Hifi通话录音状态 */
                else if (AUDIO_PCM_MODE_CAPTURE == pstPcmTriggerMsg->enPcmMode)
                {
                    /* 启动录音DMA */
                    uwRet = AUDIO_PCM_StartCaptureLoopDMA((VOS_UINT16)pstPcmCfg->uwBufSize, AUDIO_PCM_DMAC_CHN_CAPTURE);
                }
                /* 通道模式为其它，如HDMI等，暂不实现 */
                else
                {
                    /* for pc-lint */
                }

                if (UCOM_RET_SUCC != uwRet)
                {
                    return uwRet;
                }
             }
        }
        break;
        case AUDIO_PCM_TRIGGER_STOP:
        case AUDIO_PCM_TRIGGER_SUSPEND:
        case AUDIO_PCM_TRIGGER_PAUSE_PUSH:
        {
            /* 更新该通道下Buff使用状态为非使用状态 */
            pstPcmCfg->astPcmBuf[enPcmObj].enBufStatus = AUDIO_PCM_STATUS_DEACTIVE;

            /* 初始化该通道下buff是否ready标志位 */
            pstPcmCfg->astPcmBuf[enPcmObj].enBufReadyFlag = AUDIO_PCM_BUFF_ISREADY_NO;

            /* 当前通道AP和HIFI的通道同时为DEACTIVE */
            if ((AUDIO_PCM_STATUS_DEACTIVE == pstPcmCfg->astPcmBuf[AUDIO_PCM_OBJ_AP].enBufStatus)
                &&(AUDIO_PCM_STATUS_DEACTIVE == pstPcmCfg->astPcmBuf[AUDIO_PCM_OBJ_HIFI].enBufStatus))
            {
                /* 更新当前通道的整体状态为停止 */
                AUDIO_PcmSetSpeState(pstPcmTriggerMsg->enPcmMode, AUDIO_PCM_STATUS_DEACTIVE);

                /* 通道模式为播放 */
                if (AUDIO_PCM_MODE_PLAYBACK == pstPcmTriggerMsg->enPcmMode)
                {
                    /* 停止音频输出通道的DMA传输 */
                    DRV_DMA_Stop(AUDIO_PCM_DMAC_CHN_PLAY);
                }
                /* 当前通道为录音通道，且非Hifi通话录音状态 */
                else if ( (AUDIO_PCM_MODE_CAPTURE == pstPcmTriggerMsg->enPcmMode)
                        &&(AUDIO_CAPTURE_MODE_CALL != pstPcmCfg->enCaptureMode))
                {
                    /* 停止音频输入通道的SIO设备 */
                    DRV_DMA_Stop(AUDIO_PCM_DMAC_CHN_CAPTURE);
                }
                /* 通道模式为其它，如HDMI等，暂不实现 */
                else
                {
                    /* for pc-lint */
                }
            }

            if (AUDIO_PCM_OBJ_AP == enPcmObj)
            {
                AUDIO_PCM_TRIGGER_CNF_STRU  stMsgTrigerCnf;
                stMsgTrigerCnf.uhwMsgId     = ID_AUDIO_AP_PCM_TRIGGER_CNF;
                stMsgTrigerCnf.enPcmMode    = pstPcmTriggerMsg->enPcmMode;
                stMsgTrigerCnf.pSubStream   = pstPcmCfg->astPcmBuf[enPcmObj].pSubStream;
                stMsgTrigerCnf.enCmd        = pstPcmTriggerMsg->enCmd;

                /* 调用跨核音频channel接口,通知AP,PERIOD播放完毕,调用snd_pcm_period_elapsed,更新Buffer */
                UCOM_SendAudioMsg(DSP_PID_AUDIO,
                                  ACPU_PID_OM,           /* 此处填写AP侧DRV对应的PID,,待定 */
                                  &stMsgTrigerCnf,
                                  sizeof(AUDIO_PCM_TRIGGER_CNF_STRU));
            }
        }
        break;

        default:
        break;
    }

    return UCOM_RET_SUCC;

}

/*****************************************************************************
 函 数 名  : AUDIO_PCM_MsgPcmPointerReq
 功能描述  : 处理ID_AP_AUDIO_PCM_POINTER_REQ消息，上报当前DMA搬运PERIOD的状态
 输入参数  : VOS_VOID *pvOsaMsg
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月5日
    作    者   : C00137131
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 AUDIO_PCM_MsgPcmPointerReq(VOS_VOID *pvOsaMsg)
{
    /* 该处理应该由AP侧PCM驱动直接完成，此处仅作为一个保留接口，后续可删除 */

    return UCOM_RET_SUCC;
}

/*****************************************************************************
 函 数 名  : AUDIO_PCM_MsgPcmSetBufReq
 功能描述  : 处理ID_AP_AUDIO_PCM_SET_BUF_REQ消息，更新指定通道的输入或输出缓
             冲区地址，输入输出过程中该消息要求每20ms发送一个
 输入参数  : VOS_VOID *pvOsaMsg     --ID_AP_AUDIO_PCM_SET_BUF_REQ消息体
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月20日
    作    者   : C00137131
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 AUDIO_PCM_MsgPcmSetBufCmd(VOS_VOID *pvOsaMsg)
{
    AUDIO_PCM_SET_BUF_CMD_STRU         *pstSetBufMsg    = VOS_NULL;
    AUDIO_PCM_CFG_STRU                 *pstPcmCfg       = VOS_NULL;

    /* 按AUDIO_PCM_SET_BUF_REQ_STRU格式解析消息 */
    pstSetBufMsg  = (AUDIO_PCM_SET_BUF_CMD_STRU*)((MsgBlock *) pvOsaMsg)->aucValue;

    /* 检查入参 */
    if (  (pstSetBufMsg->enPcmMode >= AUDIO_PCM_MODE_BUT)
        ||(pstSetBufMsg->enPcmObj >= AUDIO_PCM_OBJ_BUT))
    {
        return UCOM_RET_ERR_PARA;
    }
    else
    {
        pstPcmCfg = AUDIO_PcmGetPcmCfgPtr(pstSetBufMsg->enPcmMode);

        /* 设置指定通道的输入输出Buff参数,须保证Uncache访问 */
        pstPcmCfg->astPcmBuf[pstSetBufMsg->enPcmObj].uwBufAddr = pstSetBufMsg->uwBufAddr;

        /* 设置该通道下buff是否ready标志位 */
        pstPcmCfg->astPcmBuf[pstSetBufMsg->enPcmObj].enBufReadyFlag = AUDIO_PCM_BUFF_ISREADY_YES;
    }

    return UCOM_RET_SUCC;
}

/*****************************************************************************
 函 数 名  : AUDIO_PCM_StartPlayLoopDMA
 功能描述  : 启动音频播放的DMA通道
 输入参数  : VOS_UINT16 uhwLen          --每次搬运的长度
             VOS_UINT16 usChNum         --搬运的DMA通道号
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月23日
    作    者   : C00137131
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 AUDIO_PCM_StartPlayLoopDMA(
                VOS_UINT16 uhwLen,
                VOS_UINT16 usChNum)
{
    VOS_UINT32              uwRet;
    VOS_UINT32              uwSrcAddrA, uwSrcAddrB;     /* 音频播放的DMA通道的源 地址 */
    VOS_UINT32              uwLliAddrA , uwLliAddrB;    /* 音频播放DMA链接地址，必须为总线地址 */
    VOS_INT16              *pshwCurCBuf = AUDIO_PCM_GetCurPlayBuff();
    VOS_INT16              *shwBufA     = AUDIO_PcmGetPlayBufAPtr();
    VOS_INT16              *shwBufB     = AUDIO_PcmGetPlayBufBPtr();

    DRV_DMA_CXCFG_STRU     *pstDmaCfgA  = VOS_NULL;
    DRV_DMA_CXCFG_STRU     *pstDmaCfgB  = VOS_NULL;

    /* 停止音频播放的DMA */
    DRV_DMA_Stop(usChNum);

    /* 设置SrcAddr的值 */
    if (pshwCurCBuf == shwBufA)
    {
        uwSrcAddrA  = (VOS_UINT32)shwBufB;
        uwSrcAddrB  = (VOS_UINT32)shwBufA;
    }
    else
    {
        uwSrcAddrA  = (VOS_UINT32)shwBufA;
        uwSrcAddrB  = (VOS_UINT32)shwBufB;
    }

    UCOM_MemSet(shwBufA, 0, uhwLen);
    UCOM_MemSet(shwBufB, 0, uhwLen);

    /* 获取DMA配置全局变量的Uncache访问地址 */
    uwLliAddrA      = (VOS_UINT32)AUDIO_PcmGetPlayDmaCfgAPtr();
    uwLliAddrB      = (VOS_UINT32)AUDIO_PcmGetPlayDmaCfgBPtr();

    pstDmaCfgA      = (DRV_DMA_CXCFG_STRU *)(uwLliAddrA);
    pstDmaCfgB      = (DRV_DMA_CXCFG_STRU *)(uwLliAddrB);

    /* 初始化LLI数组的地址 */
    UCOM_MemSet(pstDmaCfgA, 0, AUDIO_PCM_PINGPONG_BUF_NUM * sizeof(DRV_DMA_CXCFG_STRU));

    /* 设置DMA配置参数,配置为链表连接，使用I2S左右声道合并，通道LOOP配置，每个节点上报一个中断 */
    pstDmaCfgA->uwLli            = DRV_DMA_LLI_LINK(uwLliAddrB);
    pstDmaCfgA->uhwACount        = uhwLen;
    pstDmaCfgA->uwSrcAddr        = uwSrcAddrA;
    pstDmaCfgA->uwDstAddr        = DRV_SIO_GetTxRegAddr(DRV_SIO_GetAudioCfgPtr());
    pstDmaCfgA->uwConfig         = DRV_DMA_GetAudioMemSioCfg();

    pstDmaCfgB->uwLli            = DRV_DMA_LLI_LINK(uwLliAddrA);
    pstDmaCfgB->uhwACount        = uhwLen;
    pstDmaCfgB->uwSrcAddr        = uwSrcAddrB;
    pstDmaCfgB->uwDstAddr        = DRV_SIO_GetTxRegAddr(DRV_SIO_GetAudioCfgPtr());
    pstDmaCfgB->uwConfig         = DRV_DMA_GetAudioMemSioCfg();

    /* 配置音频播放DMA通道进行数据搬运，并注册音频播放DMA中断处理函数 */
    uwRet = DRV_DMA_StartWithCfg(usChNum,
                                 pstDmaCfgA,
                                 AUDIO_PCM_PlayDmaIsr,
                                 0);

    return uwRet;
}

/*****************************************************************************
 函 数 名  : AUDIO_PCM_StartCaptureLoopDMA
 功能描述  : 启动音频采集的DMA通道
 输入参数  : VOS_UINT16 uhwLen          --每次搬运的长度
             VOS_UINT16 usChNum         --搬运的DMA通道号
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月23日
    作    者   : C00137131
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 AUDIO_PCM_StartCaptureLoopDMA(
                VOS_UINT16 uhwLen,
                VOS_UINT16 usChNum)
{
    VOS_UINT32              uwRet;
    VOS_UINT32              uwDestAddrA, uwDestAddrB;   /* 音频采集的DMA通道的目的地址 */
    VOS_UINT32              uwLliAddrA, uwLliAddrB;     /* 音频采集DMA链接地址，必须为总线地址 */
    VOS_INT16              *pshwCurCBuf = (VOS_VOID *)UCOM_GetUncachedAddr((VOS_UINT32)AUDIO_PCM_GetCurCaptureBuff());
    VOS_INT16              *shwBufA     = (VOS_VOID *)UCOM_GetUncachedAddr((VOS_UINT32)AUDIO_PcmGetCaptureBufAPtr());
    VOS_INT16              *shwBufB     = (VOS_VOID *)UCOM_GetUncachedAddr((VOS_UINT32)AUDIO_PcmGetCaptureBufBPtr());
    DRV_DMA_CXCFG_STRU     *pstDmaCfgA  = VOS_NULL;
    DRV_DMA_CXCFG_STRU     *pstDmaCfgB  = VOS_NULL;

    /* 停止音频采集的DMA */
    DRV_DMA_Stop(usChNum);

    /* 设置DestAddr的值 */
    if (pshwCurCBuf == shwBufA)
    {
        uwDestAddrA = (VOS_UINT32)shwBufB;
        uwDestAddrB = (VOS_UINT32)shwBufA;
    }
    else
    {
        uwDestAddrA = (VOS_UINT32)shwBufA;
        uwDestAddrB = (VOS_UINT32)shwBufB;
    }

    UCOM_MemSet(shwBufA, 0, uhwLen);
    UCOM_MemSet(shwBufB, 0, uhwLen);

    /* 获取DMA配置全局变量的Uncache访问地址 */
    uwLliAddrA      = UCOM_GetUncachedAddr((VOS_UINT32)AUDIO_PcmGetCaptureDmaCfgAPtr());
    uwLliAddrB      = UCOM_GetUncachedAddr((VOS_UINT32)AUDIO_PcmGetCaptureDmaCfgBPtr());

    pstDmaCfgA      = (DRV_DMA_CXCFG_STRU *)(uwLliAddrA);
    pstDmaCfgB      = (DRV_DMA_CXCFG_STRU *)(uwLliAddrB);

    UCOM_MemSet(pstDmaCfgA, 0, AUDIO_PCM_PINGPONG_BUF_NUM * sizeof(DRV_DMA_CXCFG_STRU));

    /* 设置DMA配置参数,配置为链表连接，使用I2S左右声道合并，通道LOOP配置，每个节点上报一个中断 */
    pstDmaCfgA->uwLli            = DRV_DMA_LLI_LINK(uwLliAddrB);
    pstDmaCfgA->uhwACount        = uhwLen;
    pstDmaCfgA->uwSrcAddr        = DRV_SIO_GetRxRegAddr(DRV_SIO_GetAudioCfgPtr());   /* 待定  */
    pstDmaCfgA->uwDstAddr        = uwDestAddrA;
    pstDmaCfgA->uwConfig         = DRV_DMA_GetAudioSioMemCfg();

    pstDmaCfgB->uwLli            = DRV_DMA_LLI_LINK(uwLliAddrA);
    pstDmaCfgB->uhwACount        = uhwLen;
    pstDmaCfgB->uwSrcAddr        = DRV_SIO_GetRxRegAddr(DRV_SIO_GetAudioCfgPtr());   /* 待定  */
    pstDmaCfgB->uwDstAddr        = uwDestAddrB;
    pstDmaCfgB->uwConfig         = DRV_DMA_GetAudioSioMemCfg();

    /* 配置音频采集DMA通道进行数据搬运，并注册音频采集DMA中断处理函数 */
    uwRet = DRV_DMA_StartWithCfg(usChNum,
                                 pstDmaCfgA,
                                 AUDIO_PCM_CaptureDmaIsr,
                                 0);

    return uwRet;
}

/*****************************************************************************
 函 数 名  : AUDIO_PCM_GetCurPlayBuff
 功能描述  : 根据DMA状态获取播放乒乓缓冲的当前状态，即当前可处理哪个Buff，另
             一个在进行DMA搬运
 输入参数  : VOS_VOID
 输出参数  : 无
 返 回 值  : VOS_INT16*
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月12日
    作    者   : C00137131
    修改内容   : 新生成函数

*****************************************************************************/
VOS_INT16* AUDIO_PCM_GetCurPlayBuff(VOS_VOID)
{
    VOS_UINT32              uwSrcAddr;              /* 音频播放的DMA通道的源地址 */
    VOS_UINT32              uwSrcAddrB;             /* 音频播放乒乓Buff地址 */
    AUDIO_PCM_CFG_STRU     *pstPcmCfg   =   AUDIO_PcmGetPcmCfgPtr(AUDIO_PCM_MODE_PLAYBACK);

    uwSrcAddrB  = (VOS_UINT32)AUDIO_PcmGetPlayBufBPtr();

    /* 读取通道1源地址寄存器 */
    uwSrcAddr   = UCOM_RegRd(DRV_DMA_GetCxSrcAddr(AUDIO_PCM_DMAC_CHN_PLAY));

    /* 当源地址已经为BufferB范围内(LOOP模式)或源地址为BufferA的最后一个位宽(非LOOP模式) */
    /* 此逻辑依赖于BufferA与BufferB地址连续，且BufferA在前 */
    if((uwSrcAddr >= (uwSrcAddrB - sizeof(VOS_UINT32)))
        &&(uwSrcAddr < (uwSrcAddrB + (pstPcmCfg->uwBufSize - sizeof(VOS_UINT32)))))
    {
        return AUDIO_PcmGetPlayBufAPtr();
    }
    else
    {
        return AUDIO_PcmGetPlayBufBPtr();
    }

}

/*****************************************************************************
 函 数 名  : AUDIO_PCM_GetCurCaptureBuff
 功能描述  : 根据DMA状态获取录音乒乓缓冲的当前状态，即当前可处理哪个Buff，另
             一个由DMA占用
 输入参数  : VOS_VOID
 输出参数  : 无
 返 回 值  : VOS_INT16*
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月12日
    作    者   : C00137131
    修改内容   : 新生成函数

*****************************************************************************/
VOS_INT16* AUDIO_PCM_GetCurCaptureBuff(VOS_VOID)
{
    VOS_UINT32              uwDestAddr;     /* 音频录音的DMA通道的目的地址 */
    VOS_UINT32              uwDestAddrB;    /* 音频录音的DMA通道的目的地址 */
    AUDIO_PCM_CFG_STRU     *pstPcmCfg   =   AUDIO_PcmGetPcmCfgPtr(AUDIO_PCM_MODE_CAPTURE);

    uwDestAddrB = UCOM_GetUncachedAddr((VOS_UINT32)AUDIO_PcmGetCaptureBufBPtr());

    /* 读取对应通道的目的地址寄存器 */
    uwDestAddr  = UCOM_RegRd(DRV_DMA_CX_DES_ADDR(AUDIO_PCM_DMAC_CHN_CAPTURE));

    /* 当目的地址已经为BufferB范围内(LOOP模式)或目的地址为BufferA的最后一个位宽(非LOOP模式) */
    /* 此逻辑依赖于BufferA与BufferB地址连续，且BufferA在前 */
    if ( (uwDestAddr >= (uwDestAddrB - sizeof(VOS_UINT32)))
        &&(uwDestAddr < (uwDestAddrB + (pstPcmCfg->uwBufSize - sizeof(VOS_UINT32)))))
    {
        return AUDIO_PcmGetCaptureBufAPtr();
    }
    else
    {
        return AUDIO_PcmGetCaptureBufBPtr();
    }

}


/*****************************************************************************
 函 数 名  : AUDIO_PCM_MsgUpdatePlayBuffCmd
 功能描述  : 处理消息ID_AUDIO_UPDATE_PLAY_BUFF_CMD,更新播放乒乓Buff内容
 输入参数  : VOS_VOID *pvOsaMsg
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月12日
    作    者   : C00137131
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 AUDIO_PCM_MsgUpdatePlayBuffCmd(VOS_VOID *pvOsaMsg)

{
    VOS_INT16                          *pshwPlayBuf = VOS_NULL;
    VOS_INT16                          *pshwTmpBuf  = VOS_NULL;
    AUDIO_PCM_CFG_STRU                 *pstPcmCfg   = VOS_NULL ;
    VOS_UINT32                          uwIndex     = 0;
    AUDIO_PCM_UPDATE_PLAY_BUFF_CMD_STRU stUpdateBuffCmd;

    OM_CPUVIEW_EnterArea((VOS_UCHAR)OM_CPUVIEW_AREA_AUDIO_PCM_UPDATE_BUFF_PLAY);

    /* 调用AUDIO_PCM_GetCurPlayBuff获取当前可处理的音频乒乓Buff地址 */
    pshwPlayBuf = AUDIO_PCM_GetCurPlayBuff();

    /* 获取播放临时缓存 */
    pshwTmpBuf  = AUDIO_PcmGetPlayTmpBufPtr();

    pstPcmCfg   = AUDIO_PcmGetPcmCfgPtr(AUDIO_PCM_MODE_PLAYBACK);

    /* 清空当前播放buf，确保Uncache访问 */
    UCOM_MemSet((VOS_VOID *)pshwPlayBuf,
                0,
                pstPcmCfg->uwBufSize);

    /* 清空当前播放Tmpbuf */
    UCOM_MemSet((VOS_VOID *)pshwTmpBuf,
                0,
                pstPcmCfg->uwBufSize);

    /* Hifi侧播放通道Buff状态为Active，说明Hifi提供的Buff中有数据需要播出 */
    if (AUDIO_PCM_STATUS_ACTIVE == pstPcmCfg->astPcmBuf[AUDIO_PCM_OBJ_HIFI].enBufStatus)
    {
        if (AUDIO_PCM_BUFF_ISREADY_YES == pstPcmCfg->astPcmBuf[AUDIO_PCM_OBJ_HIFI].enBufReadyFlag)
        {
            /* 将HIFI player侧音频数据放入临时buffer */
            UCOM_MemCpy((VOS_VOID *)pshwTmpBuf,
                        pstPcmCfg->astPcmBuf[AUDIO_PCM_OBJ_HIFI].uwBufAddr,
                        pstPcmCfg->uwBufSize);

            /* 将buff标志位恢复 */
            pstPcmCfg->astPcmBuf[AUDIO_PCM_OBJ_HIFI].enBufReadyFlag = AUDIO_PCM_BUFF_ISREADY_NO;
        }
        else
        {
            OM_LogError1(AUDIO_PCM_PlayBackBuffIsNotReady,
                         pstPcmCfg->astPcmBuf[AUDIO_PCM_OBJ_HIFI].enBufReadyFlag);
        }

        /* Hifi播放通道Buff回调函数非空，则调用 */
        if (VOS_NULL !=pstPcmCfg->astPcmBuf[AUDIO_PCM_OBJ_HIFI].pfunCallBack)
        {
            /* 调用回调函数，入参为源Buff，进行采样率变换及通道匹配判断及转换，结果输出到tmpBuf中 */
            pstPcmCfg->astPcmBuf[AUDIO_PCM_OBJ_HIFI].pfunCallBack(pshwTmpBuf,
                                                                  pshwTmpBuf,
                                                                  pstPcmCfg->uwSampleRate,
                                                                  pstPcmCfg->uwChannelNum,
                                                                  pstPcmCfg->uwBufSize);
        }

        /* 发送消息ID_AUDIO_UPDATE_PLAYER_BUFF_CMD进行播放通道的Buff内容更新 */
        stUpdateBuffCmd.uhwMsgId    = ID_AUDIO_UPDATE_PLAYER_BUFF_CMD;

        /* 调用通用VOS发送接口，发送消息 */
        UCOM_SendOsaMsg(DSP_PID_AUDIO_RT,
                        DSP_PID_AUDIO,
                       &stUpdateBuffCmd,
                        sizeof(AUDIO_PCM_UPDATE_PLAY_BUFF_CMD_STRU));
    }

    /*AP侧播放通道Buff状态为Active，说明AP提供的Buff中有数据需要播出*/
    if (AUDIO_PCM_STATUS_ACTIVE == pstPcmCfg->astPcmBuf[AUDIO_PCM_OBJ_AP].enBufStatus)
    {
        AUDIO_PCM_PERIOD_ELAPSED_CMD_STRU       stMsgPeriodElapsed;

        if (AUDIO_PCM_BUFF_ISREADY_YES == pstPcmCfg->astPcmBuf[AUDIO_PCM_OBJ_AP].enBufReadyFlag)
        {
            /* 将buff标志位恢复 */
            pstPcmCfg->astPcmBuf[AUDIO_PCM_OBJ_AP].enBufReadyFlag = AUDIO_PCM_BUFF_ISREADY_NO;
        }
        else
        {
            OM_LogError1(AUDIO_PCM_PlayBackBuffIsNotReady,
                         pstPcmCfg->astPcmBuf[AUDIO_PCM_OBJ_AP].enBufReadyFlag);
        }

        /* pshwTmpBuf与AP侧进行混音，暂时不考虑溢出，确保Uncache读入数据来源 */
        CODEC_OpVvAdd(pshwTmpBuf,
                      (VOS_INT16 *)(pstPcmCfg->astPcmBuf[AUDIO_PCM_OBJ_AP].uwBufAddr),
                      pstPcmCfg->uwBufSize>>1,
                      pshwTmpBuf);

        /* 发送AP过来的数据, add by tonglei */
        AUDIO_DEBUG_SendHookInd(AUDIO_HOOK_APIN_PCM,
                      (VOS_VOID *)pstPcmCfg->astPcmBuf[AUDIO_PCM_OBJ_AP].uwBufAddr,
                      pstPcmCfg->uwBufSize);
        /* add end */
        stMsgPeriodElapsed.uhwMsgId     = ID_AUDIO_AP_PCM_PERIOD_ELAPSED_CMD;
        stMsgPeriodElapsed.enPcmMode    = AUDIO_PCM_MODE_PLAYBACK;
        stMsgPeriodElapsed.pSubStream   = pstPcmCfg->astPcmBuf[AUDIO_PCM_OBJ_AP].pSubStream;

        /* 调用跨核音频channel接口,通知AP,PERIOD播放完毕,调用snd_pcm_period_elapsed,更新Buffer */
        UCOM_SendAudioMsg(DSP_PID_AUDIO,
                          ACPU_PID_OM,           /* 此处填写AP侧DRV对应的PID,,待定 */
                          &stMsgPeriodElapsed,
                          sizeof(AUDIO_PCM_PERIOD_ELAPSED_CMD_STRU));


    }

    /* AP播放通道Hifi回调函数即整体通道处理回调，若非空，则调用进行频谱补偿、叠加音效等 */
    if (VOS_NULL !=pstPcmCfg->astPcmBuf[AUDIO_PCM_OBJ_AP].pfunCallBack)
    {
        /* 调用回调函数，入参为当前播放Buff，进行频谱补偿、叠加音效等 */
        pstPcmCfg->astPcmBuf[AUDIO_PCM_OBJ_AP].pfunCallBack(pshwTmpBuf,
                                                            pshwTmpBuf,
                                                            pstPcmCfg->uwSampleRate,
                                                            pstPcmCfg->uwChannelNum,
                                                            pstPcmCfg->uwBufSize);
    }

    /* 勾取DMA搬运出去的数据 */
    AUDIO_DEBUG_SendHookInd(AUDIO_HOOK_CODECOUT_PCM, pshwTmpBuf, pstPcmCfg->uwBufSize);

    /* 由于sio左右声道取的数据与现有数据相反,因此高16bit与低16bit互换，确保Uncache写入DMA搬运区 */
    for(uwIndex = 0; uwIndex < ((pstPcmCfg->uwBufSize)/2);uwIndex+=2)
    {
        pshwPlayBuf[uwIndex]   = pshwTmpBuf[uwIndex+1];
        pshwPlayBuf[uwIndex+1] = pshwTmpBuf[uwIndex];
    }

    OM_CPUVIEW_ExitArea((VOS_UCHAR)OM_CPUVIEW_AREA_AUDIO_PCM_UPDATE_BUFF_PLAY);

    return UCOM_RET_SUCC;
}

/*****************************************************************************
 函 数 名  : AUDIO_PCM_MsgUpdateCaptureBuffCmd
 功能描述  : 处理消息ID_AUDIO_UPDATE_CAPTURE_BUFF_CMD,更新录音乒乓Buff
             内容
 输入参数  : VOS_VOID *pvOsaMsg     --ID_AUDIO_UPDATE_CAPTURE_BUFF_CMD消息体
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月14日
    作    者   : C00137131
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 AUDIO_PCM_MsgUpdateCaptureBuffCmd(VOS_VOID *pvOsaMsg)
{
    VOS_INT16                          *pshwCaptureBuf  = VOS_NULL;
    VOS_INT16                          *pshwTmpBuf      = VOS_NULL;
    AUDIO_PCM_CFG_STRU                 *pstPcmCfg       = VOS_NULL;
    VOS_UINT32                          uwIndex         = 0;

    OM_CPUVIEW_EnterArea((VOS_UCHAR)OM_CPUVIEW_AREA_AUDIO_PCM_UPDATE_BUFF_CAPTURE);

    /* 调用AUDIO_PCM_GetCurCaptureBuff获取当前已搬运完毕的Buff */
    pshwCaptureBuf  = (VOS_INT16*)UCOM_GetUncachedAddr((VOS_UINT32)AUDIO_PCM_GetCurCaptureBuff());

    /* 获取录音临时缓存 */
    pshwTmpBuf      = AUDIO_PcmGetCaptureTmpBufPtr();

    pstPcmCfg       = AUDIO_PcmGetPcmCfgPtr(AUDIO_PCM_MODE_CAPTURE);

    /* 由于sio左右声道取的数据与现有数据相反,因此高16bit与低16bit互换，确保Uncache写入DMA搬运区 */
    for(uwIndex = 0; uwIndex < ((pstPcmCfg->uwBufSize)/2);uwIndex+=2)
    {
        pshwTmpBuf[uwIndex]   = pshwCaptureBuf[uwIndex+1];
        pshwTmpBuf[uwIndex+1] = pshwCaptureBuf[uwIndex];
    }

    /* 如果是通话录音且注册函数不为空，则将录音数据由pp处理后的上下行数据填充，之前数据废弃 */
    if (VOS_NULL !=pstPcmCfg->astPcmBuf[AUDIO_PCM_OBJ_HIFI].pfunVcRecordCB)
    {
        pstPcmCfg->astPcmBuf[AUDIO_PCM_OBJ_HIFI].pfunVcRecordCB(pshwTmpBuf,
                                                              pshwTmpBuf,
                                                              pstPcmCfg->uwSampleRate,
                                                              pstPcmCfg->uwChannelNum,
                                                              pstPcmCfg->uwBufSize);
    }
    else
    {
        /* AP侧通路的回调函数目的在于进行噪声抑制、频响补偿等，若非空则调用 */
        if (VOS_NULL !=pstPcmCfg->astPcmBuf[AUDIO_PCM_OBJ_AP].pfunCallBack)
        {
            /* 调用回调函数，入参为TmpBuff，进行结果输出到tmpBuf中；不同的录音场
            景，回调函数处理的内容不同；比如，若处于通话录过程中，此回调函数
            需将上下行混音数据填充到tmpBuf中;普通音频录音时需要进行单MIC降噪等*/
            pstPcmCfg->astPcmBuf[AUDIO_PCM_OBJ_AP].pfunCallBack(pshwTmpBuf,
                                                                pshwTmpBuf,
                                                                pstPcmCfg->uwSampleRate,
                                                                pstPcmCfg->uwChannelNum,
                                                                pstPcmCfg->uwBufSize);
        }
    }

    /*AP侧录音通道Buff状态为Active，说明需要给AP提供的Buff中填充录音数据*/
    if (AUDIO_PCM_STATUS_ACTIVE == pstPcmCfg->astPcmBuf[AUDIO_PCM_OBJ_AP].enBufStatus)
    {
        AUDIO_PCM_PERIOD_ELAPSED_CMD_STRU       stMsgPeriodElapsed;

        /* 将录音结果Uncache写入AP提供的Buff中 */
        UCOM_MemCpy((VOS_VOID *)(pstPcmCfg->astPcmBuf[AUDIO_PCM_OBJ_AP].uwBufAddr),
                    pshwTmpBuf,
                    pstPcmCfg->uwBufSize);

        /* 勾取录音数据*/
        AUDIO_DEBUG_SendHookInd(AUDIO_HOOK_MICIN_PCM, pshwTmpBuf, pstPcmCfg->uwBufSize);

        stMsgPeriodElapsed.uhwMsgId     = ID_AUDIO_AP_PCM_PERIOD_ELAPSED_CMD;
        stMsgPeriodElapsed.enPcmMode    = AUDIO_PCM_MODE_CAPTURE;
        stMsgPeriodElapsed.pSubStream   = pstPcmCfg->astPcmBuf[AUDIO_PCM_OBJ_AP].pSubStream;

        /* 调用跨核音频channel接口,通知AP,PERIOD录音完毕,调用snd_pcm_period_elapsed,更新Buffer */
        UCOM_SendAudioMsg(DSP_PID_AUDIO,
                          ACPU_PID_OM,           /* 此处填写AP侧DRV对应的PID,,待定 */
                          &stMsgPeriodElapsed,
                          sizeof(AUDIO_PCM_PERIOD_ELAPSED_CMD_STRU));
    }
    /*Hifi侧录音通道Buff状态为Active，说明需要给Hifi提供的Buff中填充录音数据*/
    else
    {
        if (AUDIO_PCM_STATUS_ACTIVE == pstPcmCfg->astPcmBuf[AUDIO_PCM_OBJ_HIFI].enBufStatus)
        {
            if (VOS_NULL !=pstPcmCfg->astPcmBuf[AUDIO_PCM_OBJ_HIFI].pfunCallBack)
            {
                /* 调用回调函数，入参为TmpBuff，进行结果输出到tmpBuf中，主要进行变采样等*/
                pstPcmCfg->astPcmBuf[AUDIO_PCM_OBJ_HIFI].pfunCallBack(pshwTmpBuf,
                                                                      pshwTmpBuf,
                                                                      pstPcmCfg->uwSampleRate,
                                                                      pstPcmCfg->uwChannelNum,
                                                                      pstPcmCfg->uwBufSize);
            }

            /* 将录音结果写入Hifi提供的Buff中 */
            UCOM_MemCpy((VOS_VOID *)pstPcmCfg->astPcmBuf[AUDIO_PCM_OBJ_HIFI].uwBufAddr,
                        pshwTmpBuf,
                        pstPcmCfg->uwBufSize);

            /* 发送VOS消息，通知Hifi Recorder进行编码及Buff更新 */
        }
    }

    OM_CPUVIEW_ExitArea((VOS_UCHAR)OM_CPUVIEW_AREA_AUDIO_PCM_UPDATE_BUFF_CAPTURE);

    return UCOM_RET_SUCC;

}

/*****************************************************************************
 函 数 名  : AUDIO_PCM_PlayDmaIsr
 功能描述  : 处理音频输出的DMA中断，发送播放Buff更新消息
 输入参数  : DRV_DMA_INT_TYPE_ENUM_UINT16   enIntType   --DMA中断类型
             VOS_UINT32                     uwPara      --中断回调函数参数
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月10日
    作    者   : C00137131
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID AUDIO_PCM_PlayDmaIsr(
                DRV_DMA_INT_TYPE_ENUM_UINT16    enIntType,
                VOS_UINT32                      uwPara)
{
    AUDIO_PCM_UPDATE_PLAY_BUFF_CMD_STRU         stUpdateBuffCmd;

    /* 若中断类型为TC中断,为音频播放DMA正常中断处理流程 */
    if ((  DRV_DMA_INT_TYPE_TC1 == enIntType)
        ||(DRV_DMA_INT_TYPE_TC2 == enIntType))
    {

        /* 发送消息ID_AUDIO_UPDATE_PLAY_BUFF_CMD进行播放通道的Buff内容更新 */
        stUpdateBuffCmd.uhwMsgId    = ID_AUDIO_UPDATE_PLAY_BUFF_CMD;

        /* 调用通用VOS发送接口，发送消息 */
        UCOM_SendOsaMsg(DSP_PID_AUDIO_RT,
                        DSP_PID_AUDIO,
                        &stUpdateBuffCmd,
                        sizeof(AUDIO_PCM_UPDATE_PLAY_BUFF_CMD_STRU));

    }
    /* 若中断类型为ERROR中断,记录异常 */
    else
    {
        /*记录异常，出现DMA Error中断*/
        OM_LogError1(AUDIO_PCM_PlayDmaIsr_ErrInt, enIntType);
    }
}

/*****************************************************************************
 函 数 名  : AUDIO_PCM_CaptureDmaIsr
 功能描述  : 处理音频输入的DMA中断
 输入参数  : DRV_DMA_INT_TYPE_ENUM_UINT16   enIntType   --DMA中断类型
             VOS_UINT32                     uwPara      --中断回调函数参数
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年7月10日
    作    者   : C00137131
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID AUDIO_PCM_CaptureDmaIsr(
                DRV_DMA_INT_TYPE_ENUM_UINT16    enIntType,
                VOS_UINT32                      uwPara)
{
    AUDIO_PCM_UPDATE_CAPTURE_BUFF_CMD_STRU         stUpdateBuffCmd;

    /* 若中断类型为TC中断,为音频录音DMA正常中断处理流程 */
    if ((  DRV_DMA_INT_TYPE_TC1 == enIntType)
        ||(DRV_DMA_INT_TYPE_TC2 == enIntType))
    {

        /* 发送消息ID_AUDIO_UPDATE_CAPTURE_BUFF_CMD进行播放通道的Buff内容更新 */
        stUpdateBuffCmd.uhwMsgId    = ID_AUDIO_UPDATE_CAPTURE_BUFF_CMD;

         /* 调用通用VOS发送接口，发送消息 */
        UCOM_SendOsaMsg(DSP_PID_AUDIO_RT,
                        DSP_PID_AUDIO,
                        &stUpdateBuffCmd,
                        sizeof(AUDIO_PCM_UPDATE_CAPTURE_BUFF_CMD_STRU));

    }
    /* 若中断类型为ERROR中断,记录异常 */
    else
    {
        /*记录异常，出现DMA Error中断*/
        OM_LogError1(AUDIO_PCM_CaptureDmaIsr_ErrInt, enIntType);
    }
}



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif



