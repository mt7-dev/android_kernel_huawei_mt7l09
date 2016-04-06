

#ifndef __ADSUPLINK_H__
#define __ADSUPLINK_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include "AdsCtx.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/
/* 获取IPF UL BD为0的次数 */
#define ADS_UL_GET_EMPTY_BD_COUNT       (10)


/* 构造BD的user field 1,第二个字节为Modem id,第一个字节的高4位为PktTpye,第一个字节的低4位为RabId */
#define ADS_UL_BUILD_BD_USER_FIELD_1(InstanceIndex, RabId) \
        (((((VOS_UINT16)InstanceIndex) << 8) & 0xFF00) | ((ADS_UL_GET_QUEUE_PKT_TYPE(InstanceIndex, RabId) << 4) & 0xF0) | (RabId & 0x0F))

/* 过滤器组号，Modem0用0，MODEM1用1与实例号相同 */
#define ADS_UL_GET_BD_FC_HEAD(InstanceIndex)          (InstanceIndex)

/* 构造属性信息:
           bit0:   int_en  中断使能
           bit2:1  mode    模式控制
           bit3    rsv
           bit6:4  fc_head 过滤器组号 mfc_en控制域为1时，有效
           bit15:7 rsv */
#define ADS_UL_BUILD_BD_ATTRIBUTE(Flag, Mode, FcHead) ((Flag & 0x000F) | (Mode << 1 & 0x0006) | (FcHead << 4 &0x0070))

#define ADS_UL_SET_BD_ATTR_INT_FLAG(usAttr)            ((usAttr) = (usAttr) | 0x1)


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

VOS_VOID ADS_UL_ConfigBD(VOS_UINT32 ulBdNum);
IMM_ZC_STRU* ADS_UL_GetInstanceNextQueueNode(
    VOS_UINT8                           ucInstanceIndex,
    VOS_UINT8                          *pucRabId
);
IMM_ZC_STRU* ADS_UL_GetNextQueueNode(
    VOS_UINT8                          *pucRabId,
    VOS_UINT8                          *pucInstanceIndex
);
VOS_VOID ADS_UL_ProcLinkData(VOS_VOID);
VOS_VOID ADS_UL_ProcMsg(MsgBlock* pMsg);
VOS_UINT32 ADS_UL_RcvTafMsg(MsgBlock *pMsg);
VOS_UINT32 ADS_UL_RcvTafPdpStatusInd(MsgBlock *pMsg);
VOS_UINT32 ADS_UL_ProcPdpStatusInd(
    MODEM_ID_ENUM_UINT16                enModemId,
    VOS_UINT8                           ucRabId,
    ADS_PDP_STATUS_ENUM_UINT8           enPdpStatus,
    ADS_QCI_TYPE_ENUM_UINT8             enQciType,
    ADS_PDP_TYPE_ENUM_UINT8             enPdpType
);
VOS_UINT32 ADS_UL_RcvCdsClearDataInd(MsgBlock *pMsg);
VOS_UINT32 ADS_UL_RcvCdsIpPacketMsg(MsgBlock *pMsg);
VOS_UINT32 ADS_UL_RcvCdsMsg(MsgBlock *pMsg);
VOS_UINT32 ADS_UL_RcvCdsStartSendDataInd(MsgBlock *pMsg);
VOS_UINT32 ADS_UL_RcvCdsStopSendDataInd(MsgBlock *pMsg);
VOS_UINT32 ADS_UL_RcvTimerMsg(MsgBlock *pMsg);
VOS_VOID ADS_UL_SendCdsClearDataRsp(
    MODEM_ID_ENUM_UINT16                enModemId,
    VOS_UINT8                           ucRabId
);
VOS_VOID ADS_UL_SendCdsStartSendDataRsp(
    MODEM_ID_ENUM_UINT16                enModemId,
    VOS_UINT8                           ucRabId
);
VOS_VOID ADS_UL_SendCdsStopSendDataRsp(
    MODEM_ID_ENUM_UINT16                enModemId,
    VOS_UINT8                           ucRabId
);
#if(FEATURE_OFF == FEATURE_SKB_EXP)
VOS_VOID ADS_UL_SaveIpfUlSrcMem(IMM_ZC_STRU *pstImmZcNode);
VOS_VOID ADS_UL_FreeIpfUlConfigSuccSrcMem(VOS_VOID);
VOS_VOID ADS_UL_FreeIpfUlConfigFailSrcMem(VOS_UINT32 ucNeedFreeCnt);
VOS_VOID ADS_UL_ClearIpfUlSrcMem(VOS_VOID);
VOS_VOID ADS_UL_SetMaxQueueLength(
    VOS_UINT32                          ulLength,
    VOS_UINT8                           ucInstanceIndex
);
#endif

VOS_UINT32 ADS_UL_RcvCcpuResetStartInd(
    MsgBlock                           *pstMsg
);

VOS_VOID ADS_UL_RcvTiDsFlowStatsExpired(
    VOS_UINT32                          ulTimerName,
    VOS_UINT32                          ulParam
);

VOS_VOID ADS_UL_StartDsFlowStats(
    VOS_UINT8                           ucInstance,
    VOS_UINT8                           ucRabId
);

VOS_VOID ADS_UL_StopDsFlowStats(
    VOS_UINT8                           ucInstance,
    VOS_UINT8                           ucRabId
);
VOS_VOID ADS_UL_StartRptStatsInfoTimer(
    VOS_UINT8                           ucInstance,
    VOS_UINT8                           ucRabId
);
VOS_VOID ADS_UL_StopRptStatsInfoTimer(VOS_VOID);
VOS_VOID ADS_UL_RcvTiRptStatsInfoExpired(
    VOS_UINT32                          ulTimerName,
    VOS_UINT32                          ulParam
);

VOS_VOID ADS_UL_RcvTiDataStatExpired(
    VOS_UINT32                          ulTimerName,
    VOS_UINT32                          ulParam
);


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

#endif /* end of AdsUlProcData.h */
