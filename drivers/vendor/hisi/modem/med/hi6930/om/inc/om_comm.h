/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : om_comm.h
  版 本 号   : 初稿
  作    者   : 苏庄銮 59026
  生成日期   : 2011年6月1日
  最近修改   :
  功能描述   : om_comm.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2011年6月1日
    作    者   : 苏庄銮 59026
    修改内容   : 创建文件

******************************************************************************/

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include "ucom_comm.h"

/* _H2ASN_Include OmCodecInterface_new.h */

#ifndef __OM_COMM_H__
#define __OM_COMM_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  2 宏定义
*****************************************************************************/

#define OM_COMM_TRANS_BLK_MAX_NUM       (4)                 /* 单次发送透明数据块最大数目 */
#define OM_COMM_TRACE_BLK_MAX_NUM       (2)                 /* 单次发送TRACE数据块最大数目 */

/* 全局变量访问封装 */
#define OM_COMM_GetMsgStatusPtr()       (&g_stOmStatusDesc)
#define OM_COMM_GetMsgFuncTbl()         (g_astOmMsgFuncTable)

#define OM_COMM_SEC_DTCM                UCOM_SECTION(".om.debug.bss")
#define OM_COMM_SetMsgSeqNum(uwVar)     (g_uwOmMsgSeqNum = (uwVar))             /*设置HIFI当前记录的消息SN号*/

#define OM_COMM_GetAndIncMsgSeqNum()    (g_uwOmMsgSeqNum++)                     /*获取HIFI当前记录的消息SN号，并自增1*/
#define OM_COMM_GetOMConnStatus()       (g_OMConnectionStatus)
#define OM_COMM_SetOMConnStatus(uhwsta) (g_OMConnectionStatus = (uhwsta))

/*****************************************************************************
 函 数 名  : OM_COMM_SendTranMsg
 功能描述  : 以透明消息格式发送指定消息
 输入参数  : pstMsg - 待发送的消息
             uwLen  - 待发送消息长度
 输出参数  : 无
*****************************************************************************/
#define OM_COMM_SendTranMsg(pstMsg, uwLen) /*lint -e{717} */        \
    do{                                                             \
        UCOM_DATA_BLK_STRU              stSendTransMsgDataBlk;      \
        stSendTransMsgDataBlk.pucData   = (VOS_UCHAR *)(pstMsg);    \
        stSendTransMsgDataBlk.uwSize    = (uwLen);                  \
        OM_COMM_SendTrans(&stSendTransMsgDataBlk, 1);               \
    }while(0)


/*****************************************************************************
 函 数 名  : OM_COMM_CycAdd
 功能描述  : 考虑给定范围内循环, 计算给定两个量的和
 输入参数  : Deta   - 加数
             Pre    - 被加数
             CycLen - 环形队列线性空间的长度
 输出参数  : 无
 返 回 值  : (Cur - Pre) 或者(CycLen - Pre + Cur)
*****************************************************************************/
#define OM_COMM_CycAdd(Pre, Deta, CycLen)                    \
    (((Pre) + (Deta)) % (CycLen))

/*****************************************************************************
  3 枚举定义
*****************************************************************************/


/*****************************************************************************
  4 消息头定义
*****************************************************************************/


/*****************************************************************************
  5 消息定义
*****************************************************************************/


/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/


/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/
#if 0                                                       /* _H2ASN_Skip */
/*****************************************************************************
  H2ASN生成的ASN文件需要添加
*****************************************************************************/

/*****************************************************************************
  H2ASN顶级消息结构定义
*****************************************************************************/
/* 需要更改相应的ASN编码,去掉L方式,仅保留V方式 */
typedef struct
{
    CODEC_OM_MSG_ID_ENUM_UINT16           enMsgID;            /* _H2ASN_MsgChoice_Export CODEC_OM_MSG_ID_ENUM_UINT16 */

    VOS_UINT8                           aucMsgBlock[2];
    /***************************************************************************
        _H2ASN_MsgChoice_When_Comment          CODEC_OM_MSG_ID_ENUM_UINT16
    ****************************************************************************/
}CODEC_OM_TRACE_MSG_DATA;

/* 协议消息结构*/
typedef struct
{
    VOS_UINT32 uwSenderCpuId;
    VOS_UINT32 uwSenderPid;
    VOS_UINT32 uwReceiverCpuId;
    VOS_UINT32 uwReceiverPid;
    VOS_UINT32 uwLength;
    CODEC_OM_TRACE_MSG_DATA               stMsgData;
}CODECOmTrace_MSG;

#endif                                                     /* _H2ASN_Skip */

/*****************************************************************************
  9 全局变量声明
*****************************************************************************/

extern VOS_UINT16   g_OMConnectionStatus;

/*****************************************************************************
  10 函数声明
*****************************************************************************/

extern VOS_VOID OM_COMM_Init(VOS_VOID);
extern VOS_VOID OM_COMM_InitFuncTable(VOS_VOID);
extern VOS_VOID OM_COMM_IsrNmi(VOS_VOID);
extern VOS_VOID OM_COMM_IsrSysException (VOS_UINT32 uwExceptionNo);
extern VOS_UINT32 OM_COMM_MsgExtDefReq(VOS_VOID *pvOsaMsg);
extern VOS_UINT32 OM_COMM_PidInit(enum VOS_INIT_PHASE_DEFINE enInitPhrase);
extern VOS_VOID OM_COMM_PidProc(MsgBlock *pstOsaMsg);
extern VOS_VOID OM_COMM_SendTrace(VOS_UCHAR *pucTrace, VOS_UINT32 uwLength);
extern VOS_VOID OM_COMM_SendTrans(UCOM_DATA_BLK_STRU *pstDataBlk, VOS_UINT32 uwBlkCnt);
extern VOS_UINT32 OM_COMM_MsgOmConnectCmd(VOS_VOID *pvOsaMsg);
extern VOS_UINT32 OM_COMM_MsgOmDisConnectCmd(VOS_VOID *pvOsaMsg);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of om_comm.h */
