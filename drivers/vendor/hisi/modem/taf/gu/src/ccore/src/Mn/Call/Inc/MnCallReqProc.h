/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : MnCallReqProc.h
  版 本 号   : 初稿
  作    者   : 丁庆 49431
  生成日期   : 2007年8月22日
  最近修改   : 2007年8月22日
  功能描述   : 应用请求处理模块的接口头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2007年8月22日
    作    者   : 丁庆 49431
    修改内容   : 创建文件
******************************************************************************/
#ifndef  MN_CALL_REQ_PROC_H
#define  MN_CALL_REQ_PROC_H


/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "vos.h"
#include "MnCallApi.h"
#include "MnCallBcProc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)

/*****************************************************************************
  2 常量定义
*****************************************************************************/

/*****************************************************************************
  3类型定义
*****************************************************************************/




/*****************************************************************************
  4 宏定义
*****************************************************************************/


/*****************************************************************************
  5 全局变量声明
*****************************************************************************/


/*****************************************************************************
  6 接口函数声明
*****************************************************************************/
/*****************************************************************************
 函 数 名  : MN_CALL_CallOrigReqProc
 功能描述  : 处理来自应用层的主叫请求
             该函数将检查当前状态是否允许发起一个主叫，并且检查呼叫的号码以确定
             该号码是否被允许以及是否是紧急呼叫号码。检查通过后，向CC发送
             MNCC_SETUP_REQ或MNCC_EMERG_SETUP_REQ原语发起一个呼叫或紧急呼叫。
 输入参数  : clientId    - 发起该请求的Client的ID
             opId        - Operation ID, 标识本次操作
             callId      - 本次主叫的呼叫ID，用来唯一的标识这个呼叫
             pstOrigParm - 主叫请求中携带的参数
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2007年9月20日
    作    者   : 丁庆 49431
    修改内容   : 新生成函数
*****************************************************************************/
VOS_VOID  MN_CALL_CallOrigReqProc(
    MN_CLIENT_ID_T                      clientId,
    MN_OPERATION_ID_T                   opId,
    MN_CALL_ID_T                        callId,
    MN_CALL_ORIG_PARAM_STRU             *pstOrigParm
);


VOS_UINT32 MN_CALL_InternalCallEndReqProc(
    MN_CLIENT_ID_T                      clientId,
    MN_OPERATION_ID_T                   opId,
    MN_CALL_ID_T                        callId,
    const MN_CALL_END_PARAM_STRU       *pstEndParm
);


VOS_VOID  MN_CALL_CallEndReqProc(
    MN_CLIENT_ID_T                      clientId,
    MN_OPERATION_ID_T                   opId,
    MN_CALL_ID_T                        uccallId,
    const MN_CALL_END_PARAM_STRU        *pstEndParm
);

/*****************************************************************************
 函 数 名  : MN_CALL_CallAnswerReqProc
 功能描述  : 处理来自应用层的应答请求
             该函数将检查当前状态是否允许接听一个来电，检查通过后向CC发送
             MNCC_SETUP_RES原语接听来电
 输入参数  : clientId    - 发起该请求的Client的ID
             opId        - Operation ID, 标识本次操作
             callId      - 需要接听的呼叫的ID
             pstAnsParam - 应答请求中携带的参数
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2007年9月20日
    作    者   : 丁庆 49431
    修改内容   : 新生成函数
*****************************************************************************/
VOS_VOID  MN_CALL_CallAnswerReqProc(
    MN_CLIENT_ID_T                      ucClientId,
    MN_OPERATION_ID_T                   opId,
    MN_CALL_ID_T                        callId,
    const MN_CALL_ANS_PARAM_STRU        *pstAnsParam
);


VOS_VOID  MN_CALL_CallSupsCmdReqProc(struct MsgCB *pstCallSups);


VOS_UINT32  MN_CALL_BuildNormalCallReqProc(MN_CALL_ID_T callId);


VOS_UINT32 MN_CALL_GetMoCallBc(
    MN_CALL_ID_T                        CallId,
    NAS_CC_IE_BC_STRU                  *pstBc1,
    NAS_CC_IE_BC_STRU                  *pstBc2,
    MN_CALL_REP_IND_ENUM_U8            *penBcRepeatInd
);


VOS_VOID MN_CALL_ReportEccNumList(VOS_VOID);


VOS_VOID MN_CALL_StkCallOrigReqProc(struct MsgCB * pstMsg);





VOS_UINT32 MN_CALL_MakeNewCall(
    MN_CALL_TYPE_ENUM_U8                enCallType,
    MN_CALL_ID_T                       *pNewCallId
);


VOS_VOID  MN_CALL_RcvAtCssnSetReq(
    MN_CLIENT_ID_T                      clientId,
    MN_OPERATION_ID_T                   opId,
    MN_CALL_SET_CSSN_REQ_STRU          *pstSetCssnReq
);
/*****************************************************************************
 函 数 名  : MN_CALL_GetEccNumList
 功能描述  : 获取紧急呼号码列表
 输入参数  : 无
 输出参数  : pstEccNumInfo
 返 回 值  : 无
*****************************************************************************/
VOS_VOID MN_CALL_GetEccNumList(MN_CALL_ECC_NUM_INFO_STRU          *pstEccNumInfo);

/*****************************************************************************
 函 数 名  : MN_CALL_RcvAtXlemaReq
 功能描述  : 紧急呼号码查询处理
 输入参数  :
 输出参数  : 无
 返 回 值  : 无

*****************************************************************************/
VOS_VOID MN_CALL_RcvAtXlemaReq(MN_CALL_APP_REQ_MSG_STRU            *pstCallMsg);


VOS_VOID  TAF_CALL_RcvStartDtmfReq(struct MsgCB * pstMsgSend);

VOS_VOID  TAF_CALL_RcvStopDtmfReq(struct MsgCB * pstStopDtmf);


#if ((VOS_OS_VER == VOS_WIN32) || (VOS_OS_VER == VOS_NUCLEUS))
#pragma pack()
#else
#pragma pack(0)
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* MN_CALL_REQ_PROC_H */

