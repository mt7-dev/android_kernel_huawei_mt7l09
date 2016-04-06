
#ifndef __RNIC_ENTITY_H__
#define __RNIC_ENTITY_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#pragma pack(4)

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include "AtRnicInterface.h"
#include "PsCommonDef.h"
#include "ImmInterface.h"
#include "AdsDeviceInterface.h"
#include "RnicLinuxInterface.h"

/*****************************************************************************
  2 宏定义
*****************************************************************************/
/*================================================*/
/* 数值宏定义 */
/*================================================*/

#define RNIC_MAC_ADDR_LEN               (6)                                     /* RNIC MAC地址长度，6字节 */

#define RNIC_MAX_PACKET                 (1536)                                  /* RNIC允许传输的最大包长为1500字节 */
#define RNIC_ETH_HEAD_LEN               (14)                                    /* 以太网头部长度 */

#define RNIC_IPV4_VERSION               (4)                                     /* IP头部中IP V4版本号 */
#define RNIC_IPV6_VERSION               (6)                                     /* IP头部中IP V6版本号 */

#define RNIC_IP_HEAD_VERSION_OFFSET_MASK (0x0F)                                 /* IP头部协议版本偏移量掩码 */

#define RNIC_GET_IP_VERSION(ucFirstData)\
    (((ucFirstData) >> 4) & (RNIC_IP_HEAD_VERSION_OFFSET_MASK))

#define RNIC_IP_HEAD_DEST_ADDR_OFFSET   (4 * 4)                                 /* IP头部目的IP地址偏移量 */

#define RNIC_IPV4_BROADCAST_ADDR        (0xFFFFFFFF)                            /* IPV4广播包地址 */

#define RNIC_DEFAULT_MTU                (1500)                                  /* RNIC默认的MTU值 */

#define RNIC_ETH_HDR_SIZE               (14)
#define RNIC_ETHER_ADDR_LEN             (6)
/*================================================*/
#define RNIC_TYPE_IP                    (0x0800)                                /* IP协议 */
#define RNIC_ETHERTYPE_ARP              (0x0806)                                /* 主机序形式，表示ARP包类型  */
#define RNIC_ETHERTYPE_IPV4             (0x0800)                                /* 主机序形式，表示IPv4包类型  */
#define RNIC_ETHERTYPE_IPV6             (0x86DD)                                /* 主机序形式，表示IPv6包类型  */
#define RNIC_ETH_TYPE                   (0x0001)                                /* 网络序形式，ARP报文的HardwareFormat字段用，表示以太网类型  */
#define RNIC_ARP_REPLY_TYPE             (0x0002)                                /* 网络序形式，ARP报文的OpCode字段用，表示ARP应答类型         */

/* 功能函数宏定义 */
/*================================================*/

/* 获取网卡ID对应的ModemId */
#define RNIC_GET_MODEM_ID_BY_NET_ID(index)              (g_astRnicManageTbl[index].enModemId)

/*******************************************************************************
  3 枚举定义
*******************************************************************************/

enum RNIC_NETCARD_STATUS_TYPE_ENUM
{
    RNIC_NETCARD_STATUS_CLOSED,                                                 /* RNIC为关闭状态 */
    RNIC_NETCARD_STATUS_OPENED,                                                 /* RNIC为打开状态 */
    RNIC_NETCARD_STATUS_BUTT
};
typedef VOS_UINT8 RNIC_NETCARD_STATUS_ENUM_UINT8;


enum RNIC_RESULT_TYPE_ENUM
{
    RNIC_OK             = 0,                                                    /* 正常返回 */
    RNIC_NOTSTARTED     = 1,                                                    /* 未开始 */
    RNIC_INPROGRESS     = 2,                                                    /* 运行中 */
    RNIC_PERM           = 3,
    RNIC_NOENT          = 4,
    RNIC_IO             = 5,
    RNIC_NXIO           = 6,
    RNIC_NOMEM          = 7,                                                    /* 未申请到内存 */                                                    /* 未申请到内存 */
    RNIC_BUSY           = 8,                                                    /* RNIC网卡设备忙 */
    RNIC_NODEV          = 9,                                                    /* 无设备 */
    RNIC_INVAL          = 10,                                                   /* 非法设备 */
    RNIC_NOTSUPP        = 11,                                                   /* 操作不支持 */
    RNIC_TIMEDOUT       = 12,                                                   /* 超时 */
    RNIC_SUSPENDED      = 13,                                                   /* 挂起 */
    RNIC_UNKNOWN        = 14,                                                   /* 未知错误 */
    RNIC_TEST_FAILED    = 15,                                                   /* 测试失败 */
    RNIC_STATE          = 16,                                                   /* 状态错误 */
    RNIC_STALLED        = 17,                                                   /* 失速 */
    RNIC_PARAM          = 18,                                                   /* 参数错误 */
    RNIC_ABORTED        = 19,                                                   /* 请求取消 */
    RNIC_SHORT          = 20,                                                   /* 资源不足 */
    RNIC_EXPIRED        = 21,                                                   /* 溢出 */

    RNIC_ADDR_INVALID   = 22,                                                   /* 无法分配地址 */
    RNIC_OUT_RANGE      = 23,                                                   /* 不在有效范围内 */
    RNIC_PKT_TYPE_INVAL = 24,                                                   /* 无效ip类型 */
    RNIC_ADDMAC_FAIL    = 25,                                                   /* 添加mac头失败 */
    RNIC_RX_PKT_FAIL    = 26,                                                   /* 调用内核接口接收数据失败 */
    RNIC_ERROR          = 0xff,                                                 /* RNIC返回失败 */
    RNIC_BUTT
};
typedef VOS_INT32 RNIC_RESULT_TYPE_ENUM_INT32;

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

typedef struct
{
    VOS_UINT8                           ucEtherDhost[RNIC_ETHER_ADDR_LEN];      /* destination ethernet address */
    VOS_UINT8                           ucEtherShost[RNIC_ETHER_ADDR_LEN];      /* source ethernet address */
    VOS_UINT16                          usEtherType;                            /* packet type ID */
    VOS_UINT8                           aucReserved[2];
}RNIC_ETH_HEADER_STRU;
typedef struct
{
    const VOS_CHAR                     *pucRnicNetCardName;                     /* 网卡名称 */
    RNIC_ETH_HEADER_STRU                stIpv4Ethhead;                          /* IPV4以太网头 */
    RNIC_ETH_HEADER_STRU                stIpv6Ethhead;                          /* IPV6以太网头 */
    MODEM_ID_ENUM_UINT16                enModemId;                              /* Modem Id */
    RNIC_RM_NET_ID_ENUM_UINT8           enRmNetId;                              /* 网卡ID */
    VOS_UINT8                           aucReserved[5];
}RNIC_NETCARD_ELEMENT_TAB_STRU;
typedef struct
{
    struct net_device_stats             stStats;                                /* Linxu内核标准的网卡统计信息结构 */
    struct net_device                  *pstNetDev;                              /* 用于记录Linux内核分配的网卡虚地址 */
    VOS_CHAR                           *pcDevName;                              /* 用于记录Linux内核分配的网卡名称 */
    RNIC_NETCARD_STATUS_ENUM_UINT8      enStatus;                               /* 网卡是否打开标志 */
    RNIC_RM_NET_ID_ENUM_UINT8           enRmNetId;                              /* 设备对应的网卡ID */
    VOS_UINT8                           aucRsv[6];                              /* 保留 */
}RNIC_NETCARD_DEV_INFO_STRU;

/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/
extern const RNIC_NETCARD_ELEMENT_TAB_STRU          g_astRnicManageTbl[];

/*****************************************************************************
  10 函数声明
*****************************************************************************/
VOS_INT RNIC_StopNetCard(
    struct net_device                  *pstNetDev
);
VOS_INT RNIC_OpenNetCard(
    struct net_device                  *pstNetDev
);
VOS_VOID RNIC_SendULIpv4DataInPdpDeactive(
    IMM_ZC_STRU                        *pstSkb,
    RNIC_NETCARD_DEV_INFO_STRU         *pstPriv
);
VOS_VOID RNIC_SendULDataInPdpActive(
    IMM_ZC_STRU                        *pstImmZc,
    RNIC_NETCARD_DEV_INFO_STRU         *pstPriv,
    VOS_UINT8                           ucRabId,
    VOS_UINT8                           ucNetIndex,
    ADS_PKT_TYPE_ENUM_UINT8             enIpType
);
VOS_UINT32 RNIC_TransSkbToImmZC(
    IMM_ZC_STRU                       **pstImmZc,
    struct sk_buff                     *pstSkb,
    RNIC_NETCARD_DEV_INFO_STRU         *pstPriv,
    VOS_UINT8                           ucNetIndex
);
VOS_VOID RNIC_SendULIpv4Data(
    struct sk_buff                     *pstSkb,
    struct net_device                  *pstNetDev,
    RNIC_NETCARD_DEV_INFO_STRU         *pstPriv,
    VOS_UINT8                           ucNetIndex
);
VOS_VOID RNIC_SendULIpv6Data(
    struct sk_buff                     *pstSkb,
    struct net_device                  *pstNetDev,
    RNIC_NETCARD_DEV_INFO_STRU         *pstPriv,
    VOS_UINT8                           ucNetIndex
);
netdev_tx_t RNIC_StartXmit(
    struct sk_buff                     *pstSkb,
    struct net_device                  *pstNetDev
);

VOS_UINT32 RNIC_SendDlData(
    VOS_UINT8                           ucNetIndex,
    IMM_ZC_STRU                        *pstData,
    ADS_PKT_TYPE_ENUM_UINT8             enPdpType
);

#if (FEATURE_ON == FEATURE_CL_INTERWORK)
VOS_UINT32 RNIC_RcvSdioDlData(
    VOS_UINT8                           ucPdnId,
    IMM_ZC_STRU                        *pstData
);
#endif

VOS_UINT32  RNIC_RcvAdsDlData(
    VOS_UINT8                           ucRabid,
    IMM_ZC_STRU                        *pstImmZc,
    ADS_PKT_TYPE_ENUM_UINT8             enPktType
);

VOS_INT RNIC_SetMacAddress(
    struct net_device                  *pstNetDev,
    void                               *pMacAddr
);
VOS_INT RNIC_ChangeMtu(
    struct net_device                  *pstNetDev,
    VOS_INT                             lNewMtu
);
VOS_VOID RNIC_ProcTxTimeout(
    struct net_device                  *pstNetDev
);
VOS_INT RNIC_Ioctrl(
    struct net_device                  *pstNetDev,
    struct ifreq                       *pstIfr,
    VOS_INT                             lCmd
);
VOS_VOID RNIC_DeinitNetCard(
    VOS_VOID                           *pNetDev
);
struct net_device_stats *RNIC_GetNetCardStats(
    struct net_device                  *pstNetDev
);
VOS_INT RNIC_InitNetCard(VOS_VOID);

VOS_UINT32 RNIC_AddMacHead (
    IMM_ZC_STRU                        *pstImmZc,
    const VOS_UINT8                    *pucAddData
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

#endif


