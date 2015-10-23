/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  drv_ncm.h
*
*   作    者 :  wangzhongshun
*
*   描    述 :  usb ncm模块对外头文件
*
*   修改记录 :  2013年2月2日  v1.00  wangzhongshun创建
*************************************************************************/
#ifndef __DRV_NCM_H__
#define __DRV_NCM_H__

#include "drv_comm.h"

typedef struct tagNCM_PKT_S
{
    BSP_U8 *pBuffer;       /* buffer指针*/
    BSP_U32  u32BufLen;      /* buffer长度 */
}NCM_PKT_S;

/* NCM设备类型枚举*/
typedef enum tagNCM_DEV_TYPE_E
{
    NCM_DEV_DATA_TYPE,      /* 数据通道类型，PS使用*/
    NCM_DEV_CTRL_TYPE       /* 控制通道，MSP传输AT命令使用*/
}NCM_DEV_TYPE_E;

/* 上行线路收包函数指针 */
typedef BSP_VOID (*USBUpLinkRxFunc)(UDI_HANDLE handle, BSP_VOID * pPktNode);

/* 包封装释放函数指针 */
typedef BSP_VOID (*USBFreePktEncap)(BSP_VOID *PktEncap);

/* USB IOCTL枚举 */
typedef enum tagNCM_IOCTL_CMD_TYPE_E
{
    NCM_IOCTL_NETWORK_CONNECTION_NOTIF,      /* 0x0,NCM网络是否连接上*/
    NCM_IOCTL_CONNECTION_SPEED_CHANGE_NOTIF, /* 0x1,NCM设备协商的网卡速度*/
    NCM_IOCTL_SET_PKT_ENCAP_INFO,            /* 0x2,设置包封装格式*/
    NCM_IOCTL_REG_UPLINK_RX_FUNC,            /* 0x3,注册上行收包回调函数*/
    NCM_IOCTL_REG_FREE_PKT_FUNC,             /* 0x4,注册释放包封装回调函数*/
    NCM_IOCTL_FREE_BUFF,                     /* 0x5,释放底软buffer*/
    NCM_IOCTL_GET_USED_MAX_BUFF_NUM,         /* 0x6,获取上层可以最多占用的ncm buffer个数*/
    NCM_IOCTL_GET_DEFAULT_TX_MIN_NUM,        /* 0x7,获取默认发包个数阈值，超过该阈值会启动一次NCM传输*/
    NCM_IOCTL_GET_DEFAULT_TX_TIMEOUT,        /* 0x8,获取默认发包超时时间，超过该时间会启动一次NCM传输*/
    NCM_IOCTL_GET_DEFAULT_TX_MAX_SIZE,       /* 0x9,获取默认发包字节阈值，超过该阈值会启动一次NCM传输*/
    NCM_IOCTL_SET_TX_MIN_NUM,                /* 0xa,设置发包个数阈值，超过该阈值会启动一次NCM传输*/
    NCM_IOCTL_SET_TX_TIMEOUT,                /* 0xb,设置发包超时时间，超过该时间会启动一次NCM传输*/
    NCM_IOCTL_SET_TX_MAX_SIZE,               /* 0xc,该命令字不再使用。设置发包字节阈值，超过该阈值会启动一次NCM传输*/
    NCM_IOCTL_GET_RX_BUF_SIZE,               /* 0xd,获取收包buffer大小*/
    NCM_IOCTL_FLOW_CTRL_NOTIF,               /* 0xe,流控控制开关*/
    NCM_IOCTL_REG_AT_PROCESS_FUNC,           /* 0xf,注册AT命令处理回调函数*/
    NCM_IOCTL_AT_RESPONSE,                   /* 0x10,AT命令回应*/
    NCM_IOCTL_REG_CONNECT_STUS_CHG_FUNC,     /* 0x11,注册网卡状态改变通知回调函数*/
    NCM_IOCTL_SET_PKT_STATICS,               /* 0x12,配置统计信息*/
    NCM_IOCTL_GET_FLOWCTRL_STATUS,           /* 0x13 查询NCM流控状态*/
    /* END:   Modified by liumengcun, 2011-4-21 */

    NCM_IOCTL_GET_CUR_TX_MIN_NUM,              /* 0x14 获取当前发包个数阈值*/
    NCM_IOCTL_GET_CUR_TX_TIMEOUT,               /* 0x15 获取当前发包超时时间*/
    NCM_IOCTL_IPV6_DNS_NOTIF,              /*0x16 IPV6 DNS主动上报*/
    /* END:   Modified by liumengcun, 2011-6-23 */
    /* BEGIN: Modified by liumengcun, 2011-7-20 支持IPV6 DNS配置*/
    NCM_IOCTL_SET_IPV6_DNS,                     /* 0x16 配置IPV6 DNS*/
    /* END:   Modified by liumengcun, 2011-7-20 */
    /* BEGIN: Modified by liumengcun, 2011-8-10 MSP新需求*/
    NCM_IOCTL_CLEAR_IPV6_DNS,                     /* 0x17 清除IPV6 DNS在板端的缓存,param在此命令字没有意义，不填空指针即可*/
    NCM_IOCTL_GET_NCM_STATUS,                     /* 0x18 获取NCM网卡状态 enable:TRUE(1);disable:FALSE(0) */
    /* END:   Modified by liumengcun, 2011-8-10 */

    NCM_IOCTL_SET_ACCUMULATION_TIME,

    /* BEGIN: Modified by baoxianchun, 2012-5-17 GU PS 新需求*/
	NCM_IOCTL_SET_RX_MIN_NUM,		/*配置收包个数阈值*/
	NCM_IOCTL_SET_RX_TIMEOUT,			/*配置收包超时时间*/
    /* END: Modified by baoxianchun, 2012-5-17 GU PS 新需求*/

    NCM_IOCTL_REG_NDIS_RESP_STATUS_FUNC   /* NDIS通道AT命令状态处理回调函数 */
}NCM_IOCTL_CMD_TYPE_E;

/* NCM连接状态枚举,NCM_IOCTL_NETWORK_CONNECTION_NOTIF命令字对应参数枚举*/
typedef enum tagNCM_IOCTL_CONNECTION_STATUS_E
{
    NCM_IOCTL_CONNECTION_LINKDOWN,      /* NCM网络断开连接*/
    NCM_IOCTL_CONNECTION_LINKUP         /* NCM网络连接*/
}NCM_IOCTL_CONNECTION_STATUS_E;

/* NCM连接速度结构,NCM_IOCTL_CONNECTION_SPEED_CHANGE_NOTIF命令字对应参数结构体*/
typedef struct tagNCM_IOCTL_CONNECTION_SPEED_S
{
    BSP_U32 u32DownBitRate;
    BSP_U32 u32UpBitRate;
}NCM_IOCTL_CONNECTION_SPEED_S;

/* 包封装结构体,NCM_IOCTL_SET_PKT_ENCAP_INFO命令字对应参数结构体*/
typedef struct tagNCM_PKT_ENCAP_INFO_S
{
    BSP_S32 s32BufOft;      /* buf偏移量 */
    BSP_S32 s32LenOft;      /* len偏移量 */
    BSP_S32 s32NextOft;     /* next偏移量 */
}NCM_PKT_ENCAP_INFO_S;

/* AT命令回复数据指针及长度 NCM_IOCTL_AT_RESPONSE*/
typedef struct tagNCM_AT_RSP_S
{
    BSP_U8* pu8AtAnswer;
    BSP_U32 u32Length;
} NCM_AT_RSP_S;
typedef struct tagNCM_IPV6_DNS_NTF_S
{
    BSP_U8* pu8Ipv6DnsNtf;
    BSP_U32 u32Length;
} NCM_AT_IPV6_DNS_NTF_S;

/* AT命令接收函数指针，该函数为同步接口，对应NCM_IOCTL_REG_AT_PROCESS_FUNC命令字*/
typedef BSP_VOID (*USBNdisAtRecvFunc)(BSP_U8 * pu8Buf, BSP_U32 u32Len);

/* NCM流控开关枚举,NCM_IOCTL_NETWORK_CONNECTION_NOTIF命令字对应参数枚举*/
typedef enum tagNCM_IOCTL_FLOW_CTRL_E
{
    NCM_IOCTL_FLOW_CTRL_ENABLE,      /* 打开流控*/
    NCM_IOCTL_FLOW_CTRL_DISABLE      /* 关闭流控*/
}NCM_IOCTL_FLOW_CTRL_E;

/* NCM网卡状态改变通知枚举,NCM_IOCTL_REG_CONNECT_STUS_CHG_FUNC命令字对应参数枚举*/
typedef enum tagNCM_IOCTL_CONNECT_STUS_E
{
    NCM_IOCTL_STUS_CONNECT,      /* 建链*/
    NCM_IOCTL_STUS_BREAK         /* 网卡断开,断链*/
}NCM_IOCTL_CONNECT_STUS_E;

/* 网卡状态切换通知函数，对应NCM_IOCTL_REG_CONNECT_STUS_CHG_FUNC命令字*/
typedef BSP_VOID (*USBNdisStusChgFunc)(NCM_IOCTL_CONNECT_STUS_E enStatus, BSP_VOID * pBuffer);

/* NDIS通道AT命令状态处理回调函数 */
typedef BSP_VOID (*USBNdisRespStatusCB)(BSP_VOID *pBuffer, BSP_U32 status);

/*  下传上传、下载包的各种统计信息，
    不发送给PC，被动等待PC的读取,NCM_IOCTL_SET_PKT_STATICS命令字对应参数结构*/
typedef struct tagNCM_IOCTL_PKT_STATISTICS_S
{
    BSP_U32     u32TxOKCount;         /*发送包数*/
    BSP_U32     u32RxOKCount;         /*接收包数*/
    BSP_U32     u32TxErrCount;        /*发送错误*/
    BSP_U32     u32RxErrCount;        /*接收错误*/
    BSP_U32     u32TxOverFlowCount;   /*发送溢出丢包*/
    BSP_U32     u32RxOverFlowCount;   /*接收溢出丢包*/
    BSP_U32     u32CurrentTx;         /*发送速率*/
    BSP_U32     u32CurrentRx;         /*接收速率*/
} NCM_IOCTL_PKT_STATISTICS_S;

/* BEGIN: Modified by liumengcun, 2011-7-20 IPV6 DNS配置结构,NCM_IOCTL_SET_IPV6_DNS对应参数结构*/
#define BSP_NCM_IPV6_DNS_LEN     32
 typedef struct tagNCM_IPV6DNS_S  /* 0x16 配置IPV6 DNS*/
 {
     BSP_U8 * pu8Ipv6DnsInfo;/* 32字节，低16字节表示primaryDNS；高16字节表示SecondaryDNS。*/
     BSP_U32 u32Length;
 } NCM_IPV6DNS_S;
/* END:   Modified by liumengcun, 2011-7-20 */

typedef struct tagNCM_PKT_INFO_S
{
    BSP_U32 u32PsRcvPktNum;              /* 收包送到PS的包个数*/
    BSP_U32 u32RcvUnusedNum;             /* 收包不符合PS要求丢弃包个数*/
    BSP_U32 u32NcmSendPktNum;            /* 发包个数*/
}NCM_PKT_INFO_S;

//just for fk silly stub v9r1
static INLINE BSP_VOID  BSP_NCM_Write_Ready(BSP_VOID)
{}

#endif

