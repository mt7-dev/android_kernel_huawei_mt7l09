/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : MnMsgUsimProc.h
  版 本 号   : 初稿
  作    者   : 周君 40661
  生成日期   : 2008年2月15日
  最近修改   :
  功能描述   :
  函数列表   :
  修改历史   :
  1.日    期   : 2008年2月15日
    作    者   : 周君 40661
    修改内容   : 创建文件

******************************************************************************/
#ifndef __MNMSGUSIMPROC_H__
#define __MNMSGUSIMPROC_H__


/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include  "vos.h"
#include  "MnMsgSmCommProc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 全局变量声明
*****************************************************************************/

/*****************************************************************************
  3 接口函数声明
*****************************************************************************/


VOS_UINT32  MN_MSG_SndUsimGetFileReq(
    VOS_UINT16                          usEfId,
    VOS_UINT8                           ucRecNum
);


VOS_UINT32 MN_MSG_SndUsimSetFileReq(
    VOS_UINT16                          usEfId,
    VOS_UINT8                           *pucEf,
    VOS_UINT8                           ucEfLen,
    VOS_UINT8                           ucRecNum
);


VOS_UINT32  MN_MSG_SndUsimGetMaxRecReq(
    VOS_UINT16                          usEfId
);


/*****************************************************************************
 函 数 名  : MN_MSG_InitParm
 功能描述  : 开机时需要初始化当前设置USIM的记录数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年2月15日
    作    者   : 周君 40661
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID MN_MSG_InitParm(VOS_VOID);


/*****************************************************************************
 函 数 名  : MN_MSG_GetSmCurrRecFromUsim
 功能描述  : 获取当前向USIM取短信的记录数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年2月15日
    作    者   : 周君 40661
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT8  MN_MSG_GetSmCurrRecFromUsim(VOS_VOID);


/*****************************************************************************
 函 数 名  : MN_MSG_SetSmCurrRecFromUsim
 功能描述  : 设置当前向USIM取短信的记录数
 输入参数  : ucCurrRec  - 需要设置的记录数
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年2月15日
    作    者   : 周君 40661
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID  MN_MSG_SetSmCurrRecFromUsim(
    VOS_UINT8                           ucCurrRec
);


VOS_UINT32 MN_MSG_CheckFdn(
    MN_MSG_MO_ENTITY_STRU              *pstMoEntity,
    VOS_BOOL                           *pbCheckFdn
);


VOS_UINT32 MN_MSG_SendMsgAvailable(
    VOS_BOOL                                bBufferEntity,
    VOS_UINT32                              ulIndex,
    MN_MSG_MO_ENTITY_STRU                  *pstMoEntity
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

#endif /* __MNMSGUSIMPROC_H__ */

