
#ifndef  NAS_CC_MMCC_PROC_H
#define  NAS_CC_MMCC_PROC_H

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
#include  "NasOmInterface.h"
#include "NasOmTrans.h"

/*****************************************************************************
  3 类型定义
*****************************************************************************/
typedef struct
{
    NAS_CC_MSG_TYPE_ENUM_U8       enCcMsgId;
    NAS_OTA_MSG_ID_ENUM_UINT16    enOtaMsgId;
} NAS_CC_OTA_MSG_ID_STRU;

/*****************************************************************************
  4 函数声明
*****************************************************************************/
/*****************************************************************************
 函 数 名  : NAS_CC_ProcMmccPrimitive
 功能描述  : 接收和处理来自MM的MMCC原语
 输入参数  : pMsg - MM发给CC的消息
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年1月16日
    作    者   : 丁庆 49431
    修改内容   : 新生成函数
*****************************************************************************/
VOS_VOID  NAS_CC_ProcMmccPrimitive(
    const VOS_VOID                      *pMsg
);

/*****************************************************************************
 函 数 名  : NAS_CC_LocalAbortDtmf
 功能描述  : 通知MN DTMF操作结果
 输入参数  : pMsg - MM发给CC的消息
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年07月02日
    作    者   : 丁庆 49431
    修改内容   : 新生成函数
*****************************************************************************/
VOS_VOID NAS_CC_LocalAbortDtmf(
    NAS_CC_ENTITY_ID_T                  entityId
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

#endif /* NAS_CC_MMCC_PROC_H */

