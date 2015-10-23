

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/


#ifndef __DRV_MB_HIFI_H__
#define __DRV_MB_HIFI_H__

#include "vos.h"
#include "drv_mailbox.h"
#include "drv_mailbox_cfg.h"
#include "HifiOmInterface.h"
#include "ucom_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define DRV_SOCP_SRC_CHAN_ID_HIFI           (9)
#define DRV_SOCP_DES_CHAN_ID_HIFI           (3)                                 /*此为MSP动态申请，后续作为参数传入*/
#define DRV_SOCP_CHAN_LEN_HIFI              (0x2000)
#define DRV_SOCP_GET_ENCSRC_WPTR(m)         (DRV_SOCP_BASE_ADDR+(0x0100+((m)*0x40)))/* SOCP编码源缓存写指针寄存器 */
#define DRV_SOCP_GET_ENCSRC_RPTR(m)         (DRV_SOCP_BASE_ADDR+(0x0104+((m)*0x40)))/* SOCP编码源缓存读指针寄存器 */
#define DRV_SOCP_GET_ENCSRC_BUFADDR(m)      (DRV_SOCP_BASE_ADDR+(0x0108+((m)*0x40)))/* SOCP编码源缓存首地址寄存器 */
#define DRV_SOCP_GET_ENCSRC_BUFCFG0(m)      (DRV_SOCP_BASE_ADDR+(0x010C+((m)*0x40)))/* SOCP编码源缓存长度寄存器 */
#define DRV_SOCP_GET_ENCSRC_BUFCFG1(m)      (DRV_SOCP_BASE_ADDR+(0x0110+((m)*0x40)))

#define DRV_SOCP_GET_ENCSRC_RDQWPTR(m)      (DRV_SOCP_BASE_ADDR+(0x0114+((m)*0x40)))
#define DRV_SOCP_GET_ENCSRC_RDQRPTR(m)      (DRV_SOCP_BASE_ADDR+(0x0118+((m)*0x40)))
#define DRV_SOCP_GET_ENCSRC_RDQADDR(m)      (DRV_SOCP_BASE_ADDR+(0x011C+((m)*0x40)))
#define DRV_SOCP_GET_ENCSRC_RDQCFG(m)       (DRV_SOCP_BASE_ADDR+(0x0120+((m)*0x40)))

#define DRV_SOCP_GET_ENCDEST_WPTR(n)        (DRV_SOCP_BASE_ADDR+(0x0900+((n)*0x20)))
#define DRV_SOCP_GET_ENCDEST_RPTR(n)        (DRV_SOCP_BASE_ADDR+(0x0904+((n)*0x20)))
#define DRV_SOCP_GET_ENCDEST_BUFADDR(n)     (DRV_SOCP_BASE_ADDR+(0x0908+((n)*0x20)))
#define DRV_SOCP_GET_ENCDEST_BUFCFG(n)      (DRV_SOCP_BASE_ADDR+(0x090C+((n)*0x20)))

#define DRV_SOCP_ENC_SRST_REG               (DRV_SOCP_BASE_ADDR + (0x0004))     /* SOCP编码通道软复位寄存器 */
#define DRV_SOCP_REG_ENC_RAWINT1            (DRV_SOCP_BASE_ADDR + (0x0068))
#define DRV_SOCP_REG_APP_MASK1              (DRV_SOCP_BASE_ADDR + (0x0060))
#define DRV_SOCP_RESET_TIMECNT              (1000)                              /* SOCP通道复位计数 */
#define DRV_SOCP_ENCSRC_BYPASS_DISABLE      (0U)                                /* 通道旁路没有使能 */
#define DRV_SOCP_ENCSRC_CHNMODE_CTSPACKET   (0U)                                /* 表示连续数据包结构 */
#define DRV_SOCP_CHAN_PRIORITY_0            (0U)                                /* 通道优先级最低，为0 */
#define DRV_SOCP_DATA_TYPE_1                (1U)                                /* SOCP数据类型为GU可维可测数据,与drvinterface.h中的SOCP_DATA_TYPE_E枚举含义相同 */

#define DRV_SOCP_MAGIC_NUMBER               (0x48495349)                        /* ASCII "HISI"  */
#define DRV_SOCP_USBHEAD_NUMBER             (0xaaaa5555)                        /* GU的USB数据包头 */
#define DRV_SOCP_USBTAIL_NUMBER             (0x5555aaaa)                        /* GU的USB数据包尾 */
#define DRV_SOCP_HIFI_SERVICE_ID            (0x7)                               /* 服务类型,此处填0x7 */
#define DRV_SOCP_HIFI_SESSION_ID            (0x6)                               /* CPU范围，此处填0x6 */
#define DRV_SOCP_HEADER_LEN                 (sizeof(DRV_SOCP_EMPTY_MSG_STRU)-8) /* 块状buffer传输时自usMsgLen及保留字段后（不包括）消息包头的长度，单位byte */
#define DRV_SOCP_SEG_NUM_ONE                (0x1)                               /* 表示不分段，固定填1 */
#define DRV_SOCP_SEG_SN_ONE                 (0x1)                               /* 1表示当前段序号为1 */

#define DRV_MAILBOX_PROTECT1                (MAILBOX_PROTECT1)                        /* 邮箱保护字1，用于邮箱头及邮箱 */
#define DRV_MAILBOX_PROTECT2                (MAILBOX_PROTECT2)                        /* 邮箱保护字2，用于邮箱头及邮箱 */
#define DRV_MAILBOX_PROTECT_LEN             (MAILBOX_PROTECT_LEN)                                 /* 邮箱保护字长度，单位byte，邮箱头及邮箱头、尾均有两个这样的保护字 */
#define DRV_MAILBOX_MSGHEAD_NUMBER          (MAILBOX_MSGHEAD_NUMBER)                        /* 跨核消息消息分隔字 */
#define DRV_MAILBOX_INFO_NO                 (3)                                 /* 邮箱信息描述表中单个邮箱的信息条数 */
#define DRV_MAILBOX_IPC_INFO_NUM            (2)                                 /* 互发中断的信息域数目 */

#define DRV_MAILBOX_MSG_DELAY_THD           ((VOS_UINT32)(DRV_TIMER_OM_FREQ/25))/* 跨核邮箱消息最长滞留时间(40ms)所对应的Slice值 */

#define DRV_MAILBOX_GetMailBoxHeadAddr(uhwId)(g_uwDrvMailboxAddrTbl[(uhwId)][0])/* 获取邮箱uhwId的邮箱头首地址 */
#define DRV_MAILBOX_GetMailBoxAddr(uhwId)   (g_uwDrvMailboxAddrTbl[(uhwId)][1]) /* 获取邮箱uhwId的首地址 */
#define DRV_MAILBOX_GetMailBoxLen(uhwId)    (g_uwDrvMailboxAddrTbl[(uhwId)][2]) /* 获取邮箱uhwId的长度 */

#define DRV_MAILBOX_InitMBHeadAddr(uhwId, uwAddr)      \
                                            (g_uwDrvMailboxAddrTbl[(uhwId)][0] = (uwAddr))/* 初始化邮箱uhwId的邮箱头首地址 */
#define DRV_MAILBOX_InitMBAddr(uhwId, uwAddr)          \
                                            (g_uwDrvMailboxAddrTbl[(uhwId)][1] = (uwAddr)) /* 初始化邮箱uhwId的首地址 */
#define DRV_MAILBOX_InitMBLen(uhwId, uwLen)          \
                                            (g_uwDrvMailboxAddrTbl[(uhwId)][2] = (uwLen)) /* 初始化邮箱uhwId的长度 */

#define DRV_MAILBOX_GetMailCode(uwCpuId, uwChannel) \
                                            (g_uwDrvMailCodeTbl[uwCpuId][uwChannel])

#define DRV_MAILBOX_GetMailBoxTx(uhwCpuId)  (g_uhwDrvMailboxIdTbl[uhwCpuId][0]) /* 获取HIFI对应uhwCpuId的上行邮箱编号 */
#define DRV_MAILBOX_GetMailBoxRx(uhwCpuId)  (g_uhwDrvMailboxIdTbl[uhwCpuId][1]) /* 获取HIFI对应uhwCpuId的下行邮箱编号 */

#define DRV_MAILBOX_GetTargetCpu(uhwCpuId)  (g_uhwDrvMailboxIpcTbl[uhwCpuId][0])/* 通过全系统CPU ID获取其对应的IPC Target Cpu编号 */
#define DRV_MAILBOX_GetIntrSrc(uhwCpuId)    (g_uhwDrvMailboxIpcTbl[uhwCpuId][1])/* 获取指定CPU的HiFi跨核消息通知中断bit编号 */

#define DRV_MAILBOX_GetMailBoxSeqAddr()     (&g_auwDrvMailboxSeqNum[0])         /* 获取跨核邮箱消息序列记录全局变量首地址 */
#define DRV_MAILBOX_GetMailBoxSeqNum(uhwId) (g_auwDrvMailboxSeqNum[(uhwId)])    /* 获取该跨核邮箱当前记录的消息序列号(SN号) */
#define DRV_MAILBOX_SetMailBoxSeqNum(uhwId, uwNum)          \
                                            (g_auwDrvMailboxSeqNum[(uhwId)] = (uwNum))/* 设置该跨核邮箱当前记录的消息序列号(SN号) */
/*****************************************************************************
  3 枚举定义
*****************************************************************************/

/*****************************************************************************
 实体名称  : DRV_MAILBOX_ID_ENUM
 功能描述  : 邮箱编号枚举定义
*****************************************************************************/
enum DRV_MAILBOX_ID_ENUM
{
    DRV_MAILBOX_ID_HIFI2CCPU = 0,       /* HIFI往CCPU的邮箱 */
    DRV_MAILBOX_ID_CCPU2HIFI,           /* CCPU往HIFI的邮箱 */
    DRV_MAILBOX_ID_HIFI2ACPU,           /* HIFI往ACPU的邮箱 */
    DRV_MAILBOX_ID_ACPU2HIFI,           /* ACPU往HIFI的邮箱 */
    DRV_MAILBOX_ID_HIFI2BBE16,          /* HIFI往BBE16的邮箱 */
    DRV_MAILBOX_ID_BBE162HIFI,          /* BBE16往HIFI的邮箱 */
    DRV_MAILBOX_ID_BUTT
};
typedef VOS_UINT16 DRV_MAILBOX_ID_ENUM_UINT16;


/*****************************************************************************
 实体名称  : DRV_MAILBOX_TXRX_ENUM
 功能描述  : 发送、接收标志
*****************************************************************************/
enum DRV_MAILBOX_TXRX_ENUM
{
    DRV_MAILBOX_TXRX_TX = 0,            /* 发送 */
    DRV_MAILBOX_TXRX_RX,                /* 接收 */
    DRV_MAILBOX_TXRX_BUTT
};
typedef VOS_UINT16 DRV_MAILBOX_TXRX_ENUM_UINT16;

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
 实体名称  : DRV_MAILBOX_QUEUE_STRU
 功能描述  : MAILBOX/SOCP队列缓存控制结构
*****************************************************************************/
typedef struct
{
    VOS_UINT32      uwBase;            /* 队列基地址       */
    VOS_UINT32      uwLength;          /* 队列长度，单位byte */
    VOS_UINT32      uwFront;           /* 队列写指针, 绝对地址，in   */
    VOS_UINT32      uwRear;            /* 队列读指针, 绝对地址，out  */
}DRV_MAILBOX_QUEUE_STRU;


/************************************************************************
 结构名    : IPC_MSG_BLK_STRU
 结构说明  : 核间消息块
 ************************************************************************/
typedef struct
{
    MAILBOX_MSG_HEADER stMsgHead;      /* 消息头 */
    unsigned long      ulSenderCpuId;      /* 消息发送CPU */
    unsigned long      ulSenderPid;        /* 消息发送PID */
    unsigned long      ulReceiverCpuId;    /* 消息接收CPU */
    unsigned long      ulReceiverPid;      /* 消息接收PID */
    unsigned long      ulLength;           /* 消息正文长度,bytes */
    unsigned char      aucValue[4];        /* 消息正文 */
} IPC_MSG_BLK_STRU;

/*****************************************************************************
 实体名称  : DRV_SOCP_EMPTY_MSG_STRU
 功能描述  : SOCP通道空消息结构
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          uwMagicNum;     /* 0x48495349 */
    VOS_UINT16                          uhwMsgLen;      /* 消息长度   */
    VOS_UINT16                          uhwReserve;     /* 保留       */
    VOS_UINT32                          uwReserve1;     /* 根据OM处各类SOCP搬移通道统一处理，增加32bit作为保留字 */
    OM_SOCP_FRAME_HEAD_STRU             stFrmHead;      /* OM帧头     */
    VOS_UINT32                          uwUsbHead;      /* 0xaaaa5555 */
    OM_SOCP_FRAME_SEG_STRU              stFrmSegHead;   /* OM帧分段头 */
    VOS_UINT32                          uwUsbTail;      /* 0x5555aaaa */
}DRV_SOCP_EMPTY_MSG_STRU;

/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern VOS_VOID DRV_MAILBOX_CheckRecMsg(
                       VOS_UINT32              uwMailboxId,
                       VOS_UINT32              uwTimeStamp,
                       MAILBOX_MSG_HEADER    *pstMsgHead);
extern VOS_VOID DRV_MAILBOX_Init( VOS_VOID );
extern VOS_UINT32 DRV_MAILBOX_Read(
                       IPC_MSG_BLK_STRU       *pstMsgBlk,
                       DRV_MAILBOX_QUEUE_STRU *pstQueue);
extern VOS_VOID DRV_MAILBOX_ReadQueue(
                       DRV_MAILBOX_QUEUE_STRU *pstQueue,
                       VOS_UCHAR              *pucData,
                       VOS_UINT32              uwSize);
extern VOS_VOID DRV_MAILBOX_ReceiveMsg(VOS_UINT32 uwMailboxId);
extern VOS_UINT32 DRV_MAILBOX_SendMsg(
                       VOS_UINT32                  uwCpuId,
                       VOS_UINT32                  uwChannel,
                       VOS_UINT32                  uwPriority,
                       VOS_UCHAR                  *pucMsg,
                       VOS_UINT32                  uwMsgSize);
extern VOS_UINT32 DRV_MAILBOX_Write(
                       DRV_MAILBOX_ID_ENUM_UINT16  enMailboxId,
                       VOS_UINT32                  uwCpuId,
                       VOS_UINT32                  uwPriority,
                       VOS_UCHAR                  *pucwMsg,
                       VOS_UINT32                  uwSize,
                       VOS_UINT32                  uwChannel);
extern VOS_VOID DRV_MAILBOX_WriteQueue(
                       DRV_MAILBOX_QUEUE_STRU *pstQueue,
                       VOS_UCHAR              *pucData,
                       VOS_UINT32              uwSize);
extern VOS_VOID DRV_SOCP_Init(VOS_VOID);
extern VOS_VOID DRV_SOCP_Stop(VOS_VOID);
extern VOS_UINT32 DRV_SOCP_Write(UCOM_DATA_BLK_STRU *pstBlk, VOS_UINT32 uwBlkNum);



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of med_drv_mb_hifi.h */

