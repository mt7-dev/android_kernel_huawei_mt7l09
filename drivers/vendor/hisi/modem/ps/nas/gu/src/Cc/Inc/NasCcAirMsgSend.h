
#ifndef  NAS_CC_AIRMSG_SEND_H
#define  NAS_CC_AIRMSG_SEND_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#pragma pack(4)

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "vos.h"
#include "NasCcCommon.h"
#include "NasCcIe.h"


/*****************************************************************************
  2 函数声明
*****************************************************************************/
/*****************************************************************************
 函 数 名  : NAS_CC_SendConnectAck
 功能描述  : 发送Connect acknowladge消息
 输入参数  : ucTi - Transaction ID
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年2月14日
    作    者   : 丁庆 49431
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID  NAS_CC_SendConnectAck(
    VOS_UINT8                           ucTi
);


/*****************************************************************************
 函 数 名  : NAS_CC_SendDisconnect
 功能描述  : 发送Disconnect消息
 输入参数  : ucTi       - Transaction ID
             enCauseVal - 挂断的原因值
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年2月14日
    作    者   : 丁庆 49431
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID  NAS_CC_SendDisconnect(
    VOS_UINT8                           ucTi,
    NAS_CC_CAUSE_VALUE_ENUM_U32         enCauseVal
);


/*****************************************************************************
 函 数 名  : NAS_CC_SendRelease
 功能描述  : 发送Release消息
 输入参数  : ucTi          - Transaction ID
             bCausePresent - 是否在release消息中携带原因
             enCauseVal    - 挂断的原因值
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年2月14日
    作    者   : 丁庆 49431
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID  NAS_CC_SendRelease(
    VOS_UINT8                           ucTi,
    VOS_BOOL                            bCausePresent,
    NAS_CC_CAUSE_VALUE_ENUM_U32         enCauseVal,
    VOS_BOOL                            bCause2Present,
    NAS_CC_CAUSE_VALUE_ENUM_U32         enCause2Val
);


/*****************************************************************************
 函 数 名  : NAS_CC_SendReleaseComplete
 功能描述  : 发送release complete消息
 输入参数  : ucTi          - Transaction ID
             bCausePresent - 是否在release complete消息中携带原因
             enCauseVal    - 挂断的原因值
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年2月14日
    作    者   : 丁庆 49431
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID  NAS_CC_SendReleaseComplete(
    VOS_UINT8                           ucTi,
    VOS_BOOL                            bCausePresent,
    NAS_CC_CAUSE_VALUE_ENUM_U32         enCauseVal
);


/*****************************************************************************
 函 数 名  : NAS_CC_SendStartDtmf
 功能描述  : 发送Start Dtmf消息
 输入参数  : ucTi  - Transaction ID
              ucKey - 要发送的DTMF字符
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年2月15日
    作    者   : 丁庆 49431
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID  NAS_CC_SendStartDtmf(
    VOS_UINT8                           ucTi,
    VOS_UINT8                           ucKey
);


/*****************************************************************************
 函 数 名  : NAS_CC_SendStopDtmf
 功能描述  : 发送Stop Dtmf消息
 输入参数  : ucTi  - Transaction ID
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年2月15日
    作    者   : 丁庆 49431
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID  NAS_CC_SendStopDtmf(
    VOS_UINT8                           ucTi
);


/*****************************************************************************
 函 数 名  : NAS_CC_SendStatus
 功能描述  : 发送status消息
 输入参数  : ucTi          - Transaction ID
             enCauseVal    - 发送status的原因
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年2月14日
    作    者   : 丁庆 49431
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID  NAS_CC_SendStatus(
    VOS_UINT8                           ucTi,
    NAS_CC_CAUSE_VALUE_ENUM_U32         enCauseVal
);

/*****************************************************************************
 函 数 名  : NAS_CC_FillCauseIe
 功能描述  :
 输入参数  : enCauseVal - 原因值
 输出参数  : pstCauseIe - 填好的Cause IE
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年2月14日
    作    者   : 丁庆 49431
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID  NAS_CC_FillCauseIe(
    NAS_CC_CAUSE_VALUE_ENUM_U32          enCauseVal,
    NAS_CC_IE_CAUSE_STRU                *pstCauseIe
);

/*****************************************************************************
 函 数 名  : NAS_CC_SendMmccStartCcReq
 功能描述  : 发送start cc消息
 输入参数  : ucTi - Transaction ID
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   :
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID NAS_CC_SendMmccStartCcReq(
      VOS_UINT8                           ucTi
);


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

#endif /* NAS_CC_AIRMSG_SEND_H */

