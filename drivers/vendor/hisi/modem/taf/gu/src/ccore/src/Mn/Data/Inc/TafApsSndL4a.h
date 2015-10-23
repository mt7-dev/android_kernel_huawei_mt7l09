

#ifndef __TAFAPSSNDL4A_H__
#define __TAFAPSSNDL4A_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include "MnApsMultiMode.h"
#include "TafApsDecode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)

#if (FEATURE_ON == FEATURE_LTE)

/*****************************************************************************
  2 宏定义
*****************************************************************************/


/*****************************************************************************
  3 枚举定义
*****************************************************************************/


/*****************************************************************************
  4 全局变量声明
*****************************************************************************/


/*****************************************************************************
  5 消息头定义
*****************************************************************************/


/*****************************************************************************
  6 消息定义
*****************************************************************************/


/*****************************************************************************
  7 STRUCT定义
*****************************************************************************/


/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/

/*****************************************************************************
 函 数 名  : TAF_APS_SndL4aSetCgactReq
 功能描述  : 发送ID_APS_L4A_SET_CGACT_REQ消息, 发起拨号
 输入参数  : ucPdpId                    - APS实体索引
             pstSetPdpCtxStateReq       - +CGACT命令信息
 输出参数  : 无
 返 回 值  : VOS_OK                     - 发送消息成功
             VOS_ERR                    - 发送消息失败
 调用函数  :
 被调函数  :
*****************************************************************************/
VOS_UINT32 TAF_APS_SndL4aSetCgactReq(
    VOS_UINT8                           ucPdpId,
    TAF_PS_SET_PDP_STATE_REQ_STRU      *pstSetPdpCtxStateReq
);

/*****************************************************************************
 函 数 名  : TAF_APS_SndL4aCallOrigReq
 功能描述  : 发送ID_APS_L4A_SET_NDISCONN_REQ消息, 发起拨号
 输入参数  : pstCallOrigReq             - 拨号参数
 输出参数  : 无
 返 回 值  : VOS_OK                     - 发送消息成功
             VOS_ERR                    - 发送消息失败
 调用函数  :
 被调函数  :
*****************************************************************************/
VOS_UINT32 TAF_APS_SndL4aCallOrigReq(
    TAF_PS_CALL_ORIG_REQ_STRU          *pstCallOrigReq
);

/*****************************************************************************
 函 数 名  : TAF_APS_SndL4aCallEndReq
 功能描述  : 发送ID_APS_L4A_SET_NDISCONN_REQ消息, 断开拨号
 输入参数  : pstCalEndReq               - 断开拨号参数
 输出参数  : 无
 返 回 值  : VOS_OK                     - 发送消息成功
             VOS_ERR                    - 发送消息失败
 调用函数  :
 被调函数  :
*****************************************************************************/
VOS_UINT32 TAF_APS_SndL4aCallEndReq(
    TAF_PS_CALL_END_REQ_STRU           *pstCalEndReq
);

/*****************************************************************************
 函 数 名  : TAF_APS_SndL4aCallModifyReq
 功能描述  : 发送ID_APS_L4A_SET_CGCMOD_REQ消息, 断开拨号
 输入参数  : pstCallModifyReq           - 修改EPS承载参数
 输出参数  : 无
 返 回 值  : VOS_OK                     - 发送消息成功
             VOS_ERR                    - 发送消息失败
 调用函数  :
 被调函数  :
*****************************************************************************/
VOS_UINT32 TAF_APS_SndL4aCallModifyReq(
    TAF_PS_CALL_MODIFY_REQ_STRU        *pstCallModifyReq
);


VOS_UINT32 TAF_APS_SndL4aCgansAccReq(
    TAF_PS_CALL_ANSWER_REQ_STRU        *pstCallAnswerReq
);


VOS_UINT32 TAF_APS_SndL4aCgansRejReq(
    TAF_PS_CALL_HANGUP_REQ_STRU        *pstCallHangUpReq
);

#if (FEATURE_ON == FEATURE_IPV6)
/*****************************************************************************
 函 数 名  : TAF_APS_SndL4aIpv6InfoNotifyInd
 功能描述  : 发送ID_APS_L4A_IPV6_INFO_NOTIFY_IND消息
 输入参数  : ucRabId                    - RAB ID
             pstIpv6RaInfo              - IPv6 RA参数
 输出参数  : 无
 返 回 值  : VOS_OK                     - 发送消息成功
             VOS_ERR                    - 发送消息失败
 调用函数  :
 被调函数  :
*****************************************************************************/
VOS_UINT32 TAF_APS_SndL4aIpv6InfoNotifyInd(
    VOS_UINT8                           ucRabId,
    TAF_PDP_IPV6_RA_INFO_STRU          *pstIpv6RaInfo
);
#endif

VOS_VOID TAF_APS_SndL4aSetCgdcontReq(
    VOS_VOID                           *pMsgData
);

VOS_VOID TAF_APS_SndL4aSetCgdscontReq(
    VOS_VOID                           *pMsgData
);

VOS_VOID TAF_APS_SndL4aSetCgtftReq(
    VOS_VOID                           *pMsgData
);

VOS_VOID TAF_APS_SndL4aSetCgautoReq(
    VOS_VOID                           *pMsgData
);

VOS_VOID TAF_APS_SndL4aSetCgcontrdpReq(
    VOS_VOID                           *pMsgData
);

VOS_VOID TAF_APS_SndL4aSetCgscontrdpReq(
    VOS_VOID                           *pMsgData
);

VOS_VOID TAF_APS_SndL4aSetCgtftrdpReq(
    VOS_VOID                           *pMsgData
);

VOS_VOID TAF_APS_SndL4aSetCgeqosReq(
    VOS_VOID                           *pMsgData
);

VOS_VOID TAF_APS_SndL4aSetCgeqosrdpReq(
    VOS_VOID                           *pMsgData
);

VOS_VOID TAF_APS_SndL4aSetAuthdataReq(
    VOS_VOID                           *pMsgData
);

VOS_VOID TAF_APS_SndL4aAbortReq(
    VOS_UINT8                           ucCid
);
VOS_VOID TAF_APS_SndL4aGetLteCsReq(
    VOS_VOID                           *pMsgData
);

VOS_VOID TAF_APS_SndL4aGetCemodeReq(
    VOS_VOID                           *pMsgData
);

VOS_VOID TAF_APS_SndL4aSetPdprofReq(
    VOS_VOID                           *pMsgData
);

VOS_UINT32 TAF_APS_SndL4aPppDialReq(
    TAF_PS_PPP_DIAL_ORIG_REQ_STRU      *pstPppDialOrigReq,
    TAF_APS_AUTH_INFO_STRU             *pstPppAuthInfo,
    TAF_APS_IPCP_INFO_STRU             *pstIpcpInfo
);


#endif

#if (VOS_OS_VER == VOS_WIN32)
#pragma pack()
#else
#pragma pack(0)
#endif




#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of TafApsSndL4a.h */
