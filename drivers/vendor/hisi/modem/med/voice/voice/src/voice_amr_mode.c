/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : med_amr_mode_ctrl.c
  版 本 号   : 初稿
  作    者   : 谢明辉 58441
  生成日期   : 2011年6月29日
  最近修改   :
  功能描述   :
  函数列表   :
              VOICE_AmrModeCtrlGetDtx
              VOICE_AmrModeCtrlChangeMode
              VOICE_AmrModeCtrlInit
              VOICE_UmtsAmrModeCtrlSetType
              VOICE_MsgUmtsMacModeChangeIndR
              VOICE_MsgUmtsMacModeSetIndIRP
  修改历史   :
  1.日    期   : 2011年6月29日
    作    者   : 谢明辉 58441
    修改内容   : 创建文件

******************************************************************************/

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "codec_typedefine.h"
#include "TdmacCodecInterface.h"
#include "voice_amr_mode.h"
#include "voice_proc.h"
#include "ucom_comm.h"
#include "om.h"

#ifdef  __cplusplus
#if  __cplusplus
extern "C"{
#endif
#endif

/*****************************************************************************
    可维可测信息中包含的C文件编号宏定义
*****************************************************************************/
/*lint -e(767)*/
#define THIS_FILE_ID                    OM_FILE_ID_VOICE_AMR_MODE_SC

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
VOICE_AMR_MODE_CTRL_OBJ                 g_stVoiceAmrModeCtrlObj;                  /* AMR模式控制数据实体, 详细参见VOICE_AMR_MODE_CTRL_OBJ定义 */

/*****************************************************************************
  3 函数实现
*****************************************************************************/

/*****************************************************************************
 函 数 名  : VOICE_AmrModeCtrlInit
 功能描述  : AMR模式控制初始化
 输入参数  : VOS_VOID
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年7月3日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID VOICE_AmrModeCtrlInit(VOS_VOID)
{
    VOICE_AMR_MODE_CTRL_OBJ              *pstObj = VOICE_AMR_MODE_CTRL_GetObjPtr();

    /* 缺省设置: AMR2|12.2kbps */
    UCOM_MemSet(pstObj, 0, sizeof(VOICE_AMR_MODE_CTRL_OBJ));

    pstObj->enAmrType       = WTTFVOICE_AMR_TYPE_AMR2;
    pstObj->enCurrentMode   = WTTFVOICE_AMR_CODECMODE_TYPE_122K;
    pstObj->enTargetMode    = WTTFVOICE_AMR_CODECMODE_TYPE_122K;
    pstObj->enDtxEnable     = VOICE_AMR_DTX_DISABLE;
    pstObj->uhwNumInAcs     = 1;
    pstObj->aenAcs[0]       = WTTFVOICE_AMR_CODECMODE_TYPE_122K;
    pstObj->enAcsSet        = VOICE_AMR_ACS_SET_NO;

    OM_LogInfo(VOICE_AmrModeCtrlInit_Ok);
}

/*****************************************************************************
 函 数 名  : VOICE_UmtsAmrModeCtrlSetType
 功能描述  : 通知MAC新的UMTS AMR type
 输入参数  : enCodecType - AMR|AMR2
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年7月3日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID VOICE_UmtsAmrModeCtrlSetType(CODEC_ENUM_UINT16 enCodecType,VOS_UINT32 uwReceiverPid)
{
    VOICE_AMR_MODE_CTRL_OBJ            *pstObj    = VOICE_AMR_MODE_CTRL_GetObjPtr();
    WTTFVOICE_MAC_AMR_TYPE_REQ_STRU     stRspMsg;

    UCOM_MemSet(&stRspMsg, 0, sizeof(stRspMsg));


    /* 获取NB或WB类型，并处理AMRNB和AMRNB2*/
    if (CODEC_AMRWB == enCodecType)
    {
        pstObj->enAmrType    = WTTFVOICE_AMR_TYPE_AMRWB;                         /*WB下不区分AMR和AMR2，总是填充为AMR */
    }
    else
    {
        /* 获取AMR TYPE */
        pstObj->enAmrType
            = ((CODEC_AMR == enCodecType) ? WTTFVOICE_AMR_TYPE_AMR : WTTFVOICE_AMR_TYPE_AMR2);

    }

    /* 回复消息准备 */
    stRspMsg.enMsgName  = ID_VOICE_MAC_AMR_TYPE_REQ;
    stRspMsg.enAmrType  = pstObj->enAmrType;

    UCOM_SendOsaMsg(DSP_PID_VOICE,
                    uwReceiverPid,
                    &stRspMsg,
                    sizeof(WTTFVOICE_MAC_AMR_TYPE_REQ_STRU));

    /* 记录日志 */
    OM_LogInfo(VOICE_AmrModeCtrlSetType_Ok);
}
VOS_VOID VOICE_TDAmrModeCtrlSetType(CODEC_ENUM_UINT16 enCodecType)
{
    VOICE_AMR_MODE_CTRL_OBJ                *pstObj      = VOICE_AMR_MODE_CTRL_GetObjPtr();
    TDMACVOICE_VOICE_MAC_AMR_TYPE_REQ_STRU  stRspMsg;

    UCOM_MemSet(&stRspMsg, 0, sizeof(stRspMsg));


    /* 获取AMR TYPE */
    pstObj->enAmrType
        = ((CODEC_AMR == enCodecType) ? TDMACVOICE_AMR_TYPE_AMR : TDMACVOICE_AMR_TYPE_AMR2);

    /* 回复消息准备 */
    stRspMsg.enMsgName  = ID_VOICE_TDMAC_AMR_TYPE_REQ;
    stRspMsg.enAmrType  = pstObj->enAmrType;

    UCOM_SendOsaMsg(DSP_PID_VOICE,
                    UCOM_PID_PS_TDTTF,
                    &stRspMsg,
                    sizeof(TDMACVOICE_VOICE_MAC_AMR_TYPE_REQ_STRU));

    /* 记录日志 */
    OM_LogInfo(VOICE_AmrModeCtrlSetType_Ok);
}

/*****************************************************************************
 函 数 名  : VOICE_AmrModeCtrlGetDtx
 功能描述  : 获取UMTS AMR DTX是能标志
 输入参数  : VOS_VOID
 输出参数  : 无
 返 回 值  : VOICE_AMR_DTX_ENUM_UINT16
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年7月3日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
VOICE_AMR_DTX_ENUM_UINT16 VOICE_AmrModeCtrlGetDtx(VOS_VOID)
{
    return g_stVoiceAmrModeCtrlObj.enDtxEnable;
}

/*****************************************************************************
 函 数 名  : VOICE_AmrModeCtrlChangeMode
 功能描述  : 获取当前使用的AMR速率模式, 限每20ms调用一次
 输入参数  : VOS_VOID
 输出参数  : 无
 返 回 值  : CODEC_AMR_RATE_MODE_ENUM_U16
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年7月3日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT16 VOICE_AmrModeCtrlChangeMode(VOS_VOID)
{
    VOICE_AMR_MODE_CTRL_OBJ            *pstObj = VOICE_AMR_MODE_CTRL_GetObjPtr();
    VOS_INT16                           shwCnt = 0;
    VOS_INT16                           shwFlag;

    if (VOICE_AMR_ACS_SET_NO == VOICE_AMR_MODE_CTRL_GetAcsSet())
    {
        OM_LogWarning(VOICE_AmrModeCtrlChangeMode_AcsNotSet);
    }

    /* 帧计数, 取值0,1 */
    pstObj->uhwFrmCnt += 1;
    if (pstObj->uhwFrmCnt >= 2)
    {
        pstObj->uhwFrmCnt = 0;
    }

    shwFlag = ((WTTFVOICE_AMR_TYPE_AMR2    == pstObj->enAmrType)
              || (WTTFVOICE_AMR_TYPE_AMRWB == pstObj->enAmrType))
              && (0 == pstObj->uhwFrmCnt);

    /* 若为AMR2或AMRWB则隔帧(40ms)调整一次速率 */
    if ( VOS_TRUE == shwFlag )
    {
        return pstObj->enCurrentMode;
    }

    /* 当前已经是目标速率则直接返回 */
    if (pstObj->enTargetMode == pstObj->enCurrentMode)
    {
        return pstObj->enCurrentMode;
    }

    /* 查找当前速率在ACS中的位置 */
    for (shwCnt = 0; shwCnt < (VOS_INT16)(pstObj->uhwNumInAcs); shwCnt++)
    {
        if (pstObj->aenAcs[shwCnt] == pstObj->enCurrentMode)
        {
            break;
        }
    }

    /* 若当前速率不在ACS中则强制为最小值 */
    if (shwCnt >= (VOS_INT16)(pstObj->uhwNumInAcs))
    {
        shwCnt = 0;
        pstObj->enCurrentMode = pstObj->aenAcs[shwCnt];
        OM_LogWarning(VOICE_AmrModeCtrlChangeMode_InvalidTarget);
    }

    /* 速率需要增大 */
    if (pstObj->enTargetMode > pstObj->enCurrentMode)
    {
        shwCnt += 1;
        if (shwCnt >= (VOS_INT16)(pstObj->uhwNumInAcs))
        {
            /* 当前已经是最大允许速率模式则不变 */
            OM_LogWarning(VOICE_AmrModeCtrlChangeMode_TargetOverflowAcsTop);
        }
        else
        {
            pstObj->enCurrentMode = pstObj->aenAcs[shwCnt];
        }
    }
    /* 速率需要减小 */
    else
    {
        shwCnt -= 1;
        if (shwCnt < 0)
        {
            /* 当前已经是最小允许速率模式则不变 */
            OM_LogWarning(VOICE_AmrModeCtrlChangeMode_TargetOverflowAcsBottom);
        }
        else
        {
            pstObj->enCurrentMode = pstObj->aenAcs[shwCnt];
        }
    }

    return pstObj->enCurrentMode;

}

/*****************************************************************************
 函 数 名  : VOICE_MsgUmtsMacModeSetIndIRP
 功能描述  : UMTS AMR速率模式设置消息处理
 输入参数  : pstOsaMsg
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年7月3日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 VOICE_MsgUmtsMacModeSetIndIRP(VOS_VOID *pstOsaMsg)
{
    VOICE_AMR_MODE_CTRL_OBJ                  *pstObj    = VOICE_AMR_MODE_CTRL_GetObjPtr();
    WTTFVOICE_MAC_VOICE_MODE_SET_IND_STRU    *pstSetMsg = VOS_NULL;
    WTTFVOICE_MAC_MODE_SET_RSP_STRU           stRspMsg;
    VOS_UINT16                                enAmrMode;
    VOS_UINT16                                uhwModeNum;
    VOS_UINT16                                uhwCnt;
    VOS_UINT16                                uhwCodecModeTypeButt;

    UCOM_MemSet(&stRspMsg, 0, sizeof(stRspMsg));

    /* 消息获取 */
    pstSetMsg = (WTTFVOICE_MAC_VOICE_MODE_SET_IND_STRU*)pstOsaMsg;

    /* 回复消息准备 */
    UCOM_MemSet(&stRspMsg, 0, sizeof(WTTFVOICE_MAC_MODE_SET_RSP_STRU));
    stRspMsg.enMsgName  = ID_VOICE_MAC_MODE_SET_RSP;
    stRspMsg.usOpId     = pstSetMsg->usOpId;

    stRspMsg.enResult   = WTTFVOICE_RSLT_FAIL;

    /* 参数检查 */
    /* enCodecType取值范围检查；ACS中AMR MODE个数检查: ACS中至少有一个合法AMR MODE*/
    uhwModeNum  = pstSetMsg->usCodecModeCnt;
    if (   (0 == uhwModeNum)
        || (pstSetMsg->enCodecType >= WTTFVOICE_AMR_BANDWIDTH_TYPE_BUTT))
    {
        /* 发送消息ID_VOICE_MAC_MODE_SET_RSP给WTTF */
        UCOM_SendOsaMsg(DSP_PID_VOICE,
                        UCOM_PID_PS_WTTF,
                        &stRspMsg,
                        sizeof(WTTFVOICE_MAC_MODE_SET_RSP_STRU));

        OM_LogError(VOICE_MsgMacModeSetInd_InvalidAcs);

        return UCOM_RET_ERR_PARA;
    }

    /* ACS合法性检查 */
    if(WTTFVOICE_AMR_BANDWIDTH_TYPE_NB == pstSetMsg->enCodecType)
    {
        uhwCodecModeTypeButt = WTTFVOICE_AMR_CODECMODE_TYPE_BUTT;
    }
    else
    {
        uhwCodecModeTypeButt = WTTFVOICE_AMRWB_CODECMODE_TYPE_BUTT;
    }

    for (uhwCnt = 0; uhwCnt < uhwModeNum; uhwCnt++)
    {
        if (pstSetMsg->aenCodecModes[uhwCnt] >= uhwCodecModeTypeButt)
        {
            /* 发送消息ID_VOICE_MAC_MODE_SET_RSP给WTTF */
            UCOM_SendOsaMsg(DSP_PID_VOICE,
                            UCOM_PID_PS_WTTF,
                            &stRspMsg,
                            sizeof(WTTFVOICE_MAC_MODE_SET_RSP_STRU));

            OM_LogError(VOICE_MsgMacModeSetInd_InvalidMode);

            return UCOM_RET_ERR_PARA;
        }
    }

    /* SID合法性检查 */
    if (pstSetMsg->usSidEnable >= VOICE_AMR_DTX_BUTT)
    {
        /* 发送消息ID_VOICE_MAC_MODE_SET_RSP给WTTF */
        UCOM_SendOsaMsg(DSP_PID_VOICE,
                        UCOM_PID_PS_WTTF,
                        &stRspMsg,
                        sizeof(WTTFVOICE_MAC_MODE_SET_RSP_STRU));

        OM_LogError(VOICE_MsgMacModeSetInd_InvalidSid);

        return UCOM_RET_ERR_PARA;
    }

    /* 读取设置参数 */
    enAmrMode               = pstSetMsg->aenCodecModes[uhwModeNum-1];           /* 默认取最大值作为当前AMR MODE */
    pstObj->enCurrentMode   = enAmrMode;
    pstObj->enTargetMode    = enAmrMode;
    pstObj->uhwNumInAcs     = uhwModeNum;
    pstObj->enDtxEnable     = pstSetMsg->usSidEnable;
    UCOM_MemCpy(pstObj->aenAcs,
                pstSetMsg->aenCodecModes,
                (VOS_UINT32)(uhwModeNum * sizeof(VOS_UINT16)));

    /* 回复RSP消息 */
    stRspMsg.enCurrMode = enAmrMode;
    stRspMsg.enAmrType  = pstObj->enAmrType;
    stRspMsg.enResult   = WTTFVOICE_RSLT_SUCC;

    /* 发送消息ID_VOICE_MAC_MODE_SET_RSP给WTTF */
    UCOM_SendOsaMsg(DSP_PID_VOICE,
                    UCOM_PID_PS_WTTF,
                    &stRspMsg,
                    sizeof(WTTFVOICE_MAC_MODE_SET_RSP_STRU));

    /* 记录已经收到过MAC下发的AMR速率模式设置消息 */
    VOICE_AMR_MODE_CTRL_SetAcsSet(VOICE_AMR_ACS_SET_YES);

    OM_LogInfo(VOICE_MsgMacModeSetInd_Ok);

    return UCOM_RET_SUCC;
}

/*****************************************************************************
 函 数 名  : VOICE_MsgUmtsMacModeChangeIndR
 功能描述  : 改变AMR 速率模式
 输入参数  : pstOsaMsg
 输出参数  : 无
 返 回 值  : VOS_UINT16
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年7月3日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 VOICE_MsgUmtsMacModeChangeIndR(VOS_VOID *pstOsaMsg)
{
    VOICE_AMR_MODE_CTRL_OBJ                    *pstObj    = VOICE_AMR_MODE_CTRL_GetObjPtr();
    WTTFVOICE_MAC_VOICE_MODE_CHANGE_IND_STRU   *pstIndMsg = VOS_NULL;
    WTTFVOICE_MAC_MODE_CHANGE_RSP_STRU          stRspMsg;
    VOS_UINT16                                  enAmrMode;
    VOS_UINT16                                  uhwCnt;
    VOS_UINT16                                  uhwCodecModeTypeButt;

    /* 消息获取 */
    pstIndMsg = (WTTFVOICE_MAC_VOICE_MODE_CHANGE_IND_STRU*)pstOsaMsg;

    /* 回复消息准备 */
    UCOM_MemSet(&stRspMsg, 0, sizeof(WTTFVOICE_MAC_MODE_CHANGE_RSP_STRU));
    stRspMsg.enMsgName  = ID_VOICE_MAC_MODE_CHANGE_RSP;
    stRspMsg.usOpId     = pstIndMsg->usOpId;

    /* 参数检查 */

    /* 检查目标AMR MODE是否在当前ACS中 */
    if (WTTFVOICE_AMR_BANDWIDTH_TYPE_NB == pstIndMsg->enCodecType)
    {
        uhwCodecModeTypeButt = WTTFVOICE_AMR_CODECMODE_TYPE_BUTT;
    }
    else if (WTTFVOICE_AMR_BANDWIDTH_TYPE_WB == pstIndMsg->enCodecType)
    {
        uhwCodecModeTypeButt = WTTFVOICE_AMRWB_CODECMODE_TYPE_BUTT;
    }
    else
    {
        OM_LogError(VOICE_MsgDoChangeAmrModeInd_InvalidCodecType);
        return UCOM_RET_ERR_PARA;
    }

    enAmrMode = uhwCodecModeTypeButt;

    for (uhwCnt = 0; uhwCnt < pstObj->uhwNumInAcs; uhwCnt++)
    {
        if ( pstIndMsg->enTargetMode == pstObj->aenAcs[uhwCnt] )
        {
            enAmrMode = pstIndMsg->enTargetMode;
            break;
        }
    }

    if (uhwCodecModeTypeButt  == enAmrMode)
    {
        stRspMsg.enResult   = WTTFVOICE_RSLT_FAIL;

        /* 发送消息ID_VOICE_MAC_MODE_CHANGE_RSP给WTTF */
        UCOM_SendOsaMsg(DSP_PID_VOICE,
                        UCOM_PID_PS_WTTF,
                        &stRspMsg,
                        sizeof(WTTFVOICE_MAC_MODE_CHANGE_RSP_STRU));

        OM_LogError(VOICE_MsgDoChangeAmrModeInd_InvalidMode);

        return UCOM_RET_ERR_PARA;
    }

    /* 接受参数 */
    pstObj->enTargetMode    = pstIndMsg->enTargetMode;

    /* 回复RSP消息 */
    stRspMsg.enCurrMode     = pstObj->enCurrentMode;
    stRspMsg.enTargetMode   = pstObj->enTargetMode;
    stRspMsg.enResult       = WTTFVOICE_RSLT_SUCC;

    /* 发送消息ID_VOICE_MAC_MODE_CHANGE_RSP给WTTF */
    UCOM_SendOsaMsg(DSP_PID_VOICE,
                    UCOM_PID_PS_WTTF,
                    &stRspMsg,
                    sizeof(WTTFVOICE_MAC_MODE_CHANGE_RSP_STRU));

    OM_LogInfo(VOICE_MsgDoChangeAmrModeInd_Ok);

    return UCOM_RET_SUCC;
}


VOS_UINT32 VOICE_MsgTDmacModeSetIndIRP(VOS_VOID *pstOsaMsg)
{
    VOICE_AMR_MODE_CTRL_OBJ                *pstObj    = VOICE_AMR_MODE_CTRL_GetObjPtr();
    TDMACVOICE_MAC_VOICE_MODE_SET_IND_STRU *pstSetMsg = VOS_NULL;
    TDMACVOICE_VOICE_MAC_MODE_SET_RSP_STRU  stRspMsg;
    VOS_UINT16                              enAmrMode;
    VOS_UINT16                              uhwModeNum;
    VOS_UINT16                              uhwCnt;
    VOS_UINT16                              uhwCodecModeTypeButt;

    UCOM_MemSet(&stRspMsg, 0, sizeof(stRspMsg));

    /* 消息获取 */
    pstSetMsg = (TDMACVOICE_MAC_VOICE_MODE_SET_IND_STRU*)pstOsaMsg;

    /* 回复消息准备 */
    UCOM_MemSet(&stRspMsg, 0, sizeof(TDMACVOICE_VOICE_MAC_MODE_SET_RSP_STRU));
    stRspMsg.enMsgName  = ID_VOICE_TDMAC_MODE_SET_RSP;
    stRspMsg.usOpId     = pstSetMsg->usOpId;

    stRspMsg.enResult   = TDMACVOICE_RSLT_FAIL;

    /* 参数检查 */
    /* enCodecType取值范围检查；ACS中AMR MODE个数检查: ACS中至少有一个合法AMR MODE*/
    uhwModeNum  = pstSetMsg->usCodecModeCnt;
    if (   (0 == uhwModeNum)
        || (pstSetMsg->enCodecType >= TDMACVOICE_AMR_BANDWIDTH_TYPE_BUTT))
    {
        /* 发送消息ID_CODEC_MAC_MODE_SET_RSP给TD TTF */
        UCOM_SendOsaMsg(DSP_PID_VOICE,
                        UCOM_PID_PS_TDTTF,
                        &stRspMsg,
                        sizeof(TDMACVOICE_VOICE_MAC_MODE_SET_RSP_STRU));

        OM_LogError(VOICE_MsgMacModeSetInd_InvalidAcs);

        return UCOM_RET_ERR_PARA;
    }

    uhwCodecModeTypeButt = TDMACVOICE_AMR_CODECMODE_TYPE_BUTT;

    for (uhwCnt = 0; uhwCnt < uhwModeNum; uhwCnt++)
    {
        if (pstSetMsg->aenCodecModes[uhwCnt] >= uhwCodecModeTypeButt)
        {
            /* 发送消息ID_VOICE_MAC_MODE_SET_RSP给TD TTF */
            UCOM_SendOsaMsg(DSP_PID_VOICE,
                            UCOM_PID_PS_TDTTF,
                            &stRspMsg,
                            sizeof(TDMACVOICE_VOICE_MAC_MODE_SET_RSP_STRU));

            OM_LogError(VOICE_MsgMacModeSetInd_InvalidMode);

            return UCOM_RET_ERR_PARA;
        }
    }

    /* SID合法性检查 */
    if (pstSetMsg->usSidEnable >= VOICE_AMR_DTX_BUTT)
    {
        /* 发送消息ID_VOICE_MAC_MODE_SET_RSP给TD TTF */
        UCOM_SendOsaMsg(DSP_PID_VOICE,
                        UCOM_PID_PS_TDTTF,
                        &stRspMsg,
                        sizeof(TDMACVOICE_VOICE_MAC_MODE_SET_RSP_STRU));

        OM_LogError(VOICE_MsgMacModeSetInd_InvalidSid);

        return UCOM_RET_ERR_PARA;
    }

    /* 读取设置参数 */
    enAmrMode               = pstSetMsg->aenCodecModes[uhwModeNum-1];           /* 默认取最大值作为当前AMR MODE */
    pstObj->enCurrentMode   = enAmrMode;
    pstObj->enTargetMode    = enAmrMode;
    pstObj->uhwNumInAcs     = uhwModeNum;
    pstObj->enDtxEnable     = pstSetMsg->usSidEnable;
    UCOM_MemCpy(pstObj->aenAcs,
                pstSetMsg->aenCodecModes,
                (VOS_UINT32)(uhwModeNum * sizeof(VOS_UINT16)));

    /* 回复RSP消息 */
    stRspMsg.enCurrMode = enAmrMode;
    stRspMsg.enAmrType  = pstObj->enAmrType;
    stRspMsg.enResult   = TDMACVOICE_RSLT_SUCC;

    /* 发送消息ID_CODEC_MAC_MODE_SET_RSP给TD TTF */
    UCOM_SendOsaMsg(DSP_PID_VOICE,
                    UCOM_PID_PS_TDTTF,
                    &stRspMsg,
                    sizeof(TDMACVOICE_VOICE_MAC_MODE_SET_RSP_STRU));

    /* 记录已经收到过MAC下发的AMR速率模式设置消息 */
    VOICE_AMR_MODE_CTRL_SetAcsSet(VOICE_AMR_ACS_SET_YES);

    OM_LogInfo(VOICE_MsgMacModeSetInd_Ok);

    return UCOM_RET_SUCC;
}
VOS_UINT32 VOICE_MsgTDMacModeChangeIndR(VOS_VOID *pstOsaMsg)
{
    VOICE_AMR_MODE_CTRL_OBJ                    *pstObj    = VOICE_AMR_MODE_CTRL_GetObjPtr();
    TDMACVOICE_MAC_VOICE_MODE_CHANGE_IND_STRU  *pstIndMsg = VOS_NULL;
    TDMACVOICE_VOICE_MAC_MODE_CHANGE_RSP_STRU   stRspMsg;
    VOS_UINT16                                  enAmrMode;
    VOS_UINT16                                  uhwCnt;
    VOS_UINT16                                  uhwCodecModeTypeButt;

    /* 消息获取 */
    pstIndMsg = (TDMACVOICE_MAC_VOICE_MODE_CHANGE_IND_STRU*)pstOsaMsg;

    /* 回复消息准备 */
    UCOM_MemSet(&stRspMsg, 0, sizeof(TDMACVOICE_MAC_VOICE_MODE_CHANGE_IND_STRU));
    stRspMsg.enMsgName  = ID_VOICE_TDMAC_MODE_CHANGE_RSP;
    stRspMsg.usOpId     = pstIndMsg->usOpId;

    /* 参数检查 */

    /* 检查目标AMR MODE是否在当前ACS中 */
    if (TDMACVOICE_AMR_BANDWIDTH_TYPE_NB == pstIndMsg->enCodecType)
    {
        uhwCodecModeTypeButt = TDMACVOICE_AMR_CODECMODE_TYPE_BUTT;
    }
    else
    {
        OM_LogError(VOICE_MsgDoChangeAmrModeInd_InvalidCodecType);
        return UCOM_RET_ERR_PARA;
    }

    enAmrMode = uhwCodecModeTypeButt;

    for (uhwCnt = 0; uhwCnt < pstObj->uhwNumInAcs; uhwCnt++)
    {
        if ( pstIndMsg->enTargetMode == pstObj->aenAcs[uhwCnt] )
        {
            enAmrMode = pstIndMsg->enTargetMode;
            break;
        }
    }

    if (uhwCodecModeTypeButt  == enAmrMode)
    {
        stRspMsg.enResult   = TDMACVOICE_RSLT_FAIL;

        /* 发送消息ID_CODEC_MAC_MODE_CHANGE_RSP给TD TTF */
        UCOM_SendOsaMsg(DSP_PID_VOICE,
                        UCOM_PID_PS_TDTTF,
                        &stRspMsg,
                        sizeof(TDMACVOICE_VOICE_MAC_MODE_CHANGE_RSP_STRU));

        OM_LogError(VOICE_MsgDoChangeAmrModeInd_InvalidMode);

        return UCOM_RET_ERR_PARA;
    }

    /* 接受参数 */
    pstObj->enTargetMode    = pstIndMsg->enTargetMode;

    /* 回复RSP消息 */
    stRspMsg.enCurrMode     = pstObj->enCurrentMode;
    stRspMsg.enTargetMode   = pstObj->enTargetMode;
    stRspMsg.enResult       = TDMACVOICE_RSLT_SUCC;

    /* 发送消息ID_CODEC_MAC_MODE_CHANGE_RSP给TD TTF */
        UCOM_SendOsaMsg(DSP_PID_VOICE,
                        UCOM_PID_PS_TDTTF,
                        &stRspMsg,
                        sizeof(TDMACVOICE_VOICE_MAC_MODE_CHANGE_RSP_STRU));

    OM_LogInfo(VOICE_MsgDoChangeAmrModeInd_Ok);

    return UCOM_RET_SUCC;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

