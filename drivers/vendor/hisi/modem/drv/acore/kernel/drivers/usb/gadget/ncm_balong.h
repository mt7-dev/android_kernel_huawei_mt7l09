/*************************************************************************
*   版权所有(C) 1987-2012, 深圳华为技术有限公司.
*
*   文 件 名 :  ncm_balong.h
*
*   作    者 :  wangzhongshun
*
*   描    述 :  NCM应用层头文件
*
*   修改记录 :  2013年2月16日  v1.00  wangzhongshun  创建

*************************************************************************/
#ifndef __NCM_BALONG_H__
#define __NCM_BALONG_H__

#include <linux/types.h>
#include <linux/skbuff.h>

#include "bsp_ncm.h"

#ifdef CONFIG_ETH_BYPASS_MODE

/* 1:RNDIS配置管理 */
#define RNDIS_MAX_TX_TRANSFER_SIZE (45*1514)
#define RNDIS_MAX_RX_TRANSFER_SIZE (45*1514)

#define RNDIS_MULTI_BFR_SIZE       (1)
/* 2:NCM配置管理 */
#define NCM_MAX_TX_TRANSFER_SIZE        (256*1024)
#define NCM_MAX_RX_TRANSFER_SIZE        (32*1024)
#define NCM_CACHE_LINE_SIZE             (32)
#define NCM_RX_BUF_RESERVED_LEN         (NCM_CACHE_LINE_SIZE)
#define LINUX_NCM_MAX_TX_TRANSFER_SIZE  (4*1024)

/* 接收缓冲区大小，根据宏来区别,RNDIS形态不需要NCM,所以设置的小 */
#if 0
#if ((FEATURE_HILINK == FEATURE_ON) || (FEATURE_E5_HILINK == FEATURE_ON))
#define NCM_MALLOC_RX_BUF_SIZE  2016
#define NCM_RCV_BUFFER_NUM 1  /* NCM 收包buffer个数，每个buffer大小是MAX_RX_TRANSFER_SIZE*/
#endif
#endif
#define NCM_MALLOC_RX_BUF_SIZE  (NCM_MAX_RX_TRANSFER_SIZE + (NCM_RX_BUF_RESERVED_LEN)*2)
#define NCM_RCV_BUFFER_NUM 8  /* NCM 收包buffer个数，每个buffer大小是MAX_RX_TRANSFER_SIZE*/

/* 3:ECM配置管理 */
#define ECM_MAX_TX_TRANSFER_SIZE (42 * 1514)
#define ECM_MAX_RX_TRANSFER_SIZE (42 * 1514)

/* 4:ECM配置管理 */
/* TXQ_RING 大小，根据STICK宏来区别,STICK形态不需要net,所以设置的小 */
#if 0
#if (FEATURE_STICK == FEATURE_ON)
#define NET_MAX_TXQ_RING_NUM        (2)
#endif
#endif
#define NET_MAX_TXQ_RING_NUM        (2048)

#define NET_MAX_TXQ_RING_MASK       (NET_MAX_TXQ_RING_NUM-1)
#define NET_GET_NEXT(n)             (((n)+1) & NET_MAX_TXQ_RING_MASK)
#define NET_MAX_TXQ_NUM             (2048)

#ifndef CONFIG_MAX_NET_TRANSFERS
    /* 发送数目的大小，根据宏来区别,STICK形态不需要net,所以设置的小 */
    #if 0
    #if (FEATURE_STICK == FEATURE_ON)
    #define CONFIG_MAX_NET_TRANSFERS 2    /*2*/
    #endif
    #endif
    #define CONFIG_MAX_NET_TRANSFERS 1024 /*2*/
#endif
#if CONFIG_MAX_NET_TRANSFERS < 1
#error "CONFIG_MAX_NET_TRANSFERS must be greater or equal to 1"
#endif

#define NCM_PACKETS_ACCUMULATION_TIME 2000
#define NCM_PACKET_NUM 4
#define MAX_ENCAP_CMD_SIZE   2048   /* MSP内部约束AT命令最大长度2048，非AT命令协议规定*/

#define MAX_SEG_SIZE 1514
#define MIN_SEG_SIZE 14
#define NCM_ALIGNMENT 4
#define MIN_NCM_TX 1
#define MIN_NCM_TX_SPEED_FULL 25
#define MAX_NCM_TX 64
#define MAX_HOST_OUT_TIMEOUT 20
#define NCM_TX_TIMEOUT 10
#define FIFO_SIZE  1024

#define NCM_USED_BUFFER_NUM ((NCM_RCV_BUFFER_NUM)-(NCM_PACKET_NUM))/* NCM可以缓存的包个数*/
#define NCM_REMAINDER 2

#define NCM_MAX_RATE   (1*1000*1000*1000)
#define NCM_MIN_RATE   (1*1000)

#define NCM_AT_ANSWER_LEN  (MAX_ENCAP_CMD_SIZE)
#define NCM_AT_RESPONSE_TIMEOUT   200

/* NCM正在发包个数门限，超过该门限返回内存不足，
   防止正在sending过程中的包个数过多，将内存池耗尽
   根据测试debug版本160Mbps，发包门限配置为4000
   门限   使用最大内存
   5000   2012458
   4000   1624260
   3500   1430464
   3000   1235702*/
#define NCM_MAX_SEND_PKT_NUM  4000

/* mac地址临时自己定义，后续产品化再申请*/
#define NCM_EX_MAC_ADDR1  { 0x0C, 0x5B, 0x8F, 0x27, 0x9A, 0x64 }
#define NCM_EX_MAC_ADDR2  { 0x0C, 0x5B, 0x8F, 0x27, 0x9A, 0x65 }
#define NCM_EX_MAC_ADDR3  { 0x0C, 0x5B, 0x8F, 0x27, 0x9A, 0x66 }
#define NCM_EX_MAC_ADDR4  { 0x0C, 0x5B, 0x8F, 0x27, 0x9A, 0x67 }

/* USB回调函数结构体 */
typedef struct tagUSB_CB_S
{
    USBUpLinkRxFunc rx_cb;           /* 上行收包回调函数 */
    USBFreePktEncap pkt_encap_free_cb;      /* 包封装释放回调函数 */
    USBNdisAtRecvFunc ndis_at_recv_cb;    /* AT命令处理回调*/
    USBNdisStusChgFunc ndis_stat_change_cb;  /* USB网卡状态变化处理回调函数*/
}USB_CB_S;

typedef enum tagNCM_RCV_BUF_STAT_E
{
    NCM_RCV_BUF_FREE,
    NCM_RCV_BUF_DONE,
    NCM_RCV_BUF_PROCESS,
    NCM_RCV_BUF_OUT
}NCM_RCV_BUF_STAT_E;

typedef struct tagNCM_DEBUG_INFO_S
{
    u32 u32RcvInNum;                 /* 收包函数进入次数*/
    u32 u32RcvInSize;                /* 收包函数进入字节数 */
    u32 u32RcvTooLongNum;            /* 收包错包计数 */
    u32 u32RcvErrPktLen;             /* 错包包长 */
    u32 u32AllocSkbFailNum;          /* 分配skb失败计数 */
    u32 u32RcvInErrNum;              /* 收包函数进入异常退出次数*/
    u32 u32PsRcvPktNum;              /* 收包送到PS的包个数*/
    u32 u32RcvUnusedNum;             /* 收包不符合PS要求丢弃包个数*/
    u32 u32NcmSendInNum;             /* 发包函数进入次数*/
    u32 u32NcmSendPktNum;            /* 发包个数*/
    u32 u32NcmSendNoPktNum;          /* 发包全部无效次数*/
    u32 u32NcmSendOkNum;             /* 发包函数成功次数*/
    u32 u32NcmSendFailNum;           /* 发包函数调用fd层write失败次数*/
    u32 u32NcmSendNoMemNum;          /* 发包函数申请不到内存退出次数*/
    u32 u32NcmSendDevNotOpenNum;     /* 发包函数通道没有打开退出次数*/
    u32 u32NcmSendDevNotEnableNum;   /* 发包函数设备没有使能退出次数*/
    u32 u32NcmSendNetNotConnectNum;  /* 发包函数网卡没有配置连接态退出次数*/
    u32 u32NcmSendPktTooLargeNum;    /* 发包函数包长超过1514退出次数*/
    u32 u32NcmSendPktInvalidNum;     /* 发包函数数据包无效次数*/
    u32 u32NcmSendDoneNum;           /* 发包完成次数*/
    u32 u32NcmSendDoneFailNum;       /* 发包回调失败次数*/
    u32 u32NcmAtInNum;              /* AT发包进入次数*/
    u32 u32NcmAtInOKNum;             /* AT发包成功次数*/
    u32 u32NcmAtInFailNum;             /* AT发包失败次数*/
    u32 u32NcmAtInTimeOutNum;        /* AT发包超时次数*/
    u32 u32NcmAtDoneNum;             /* AT发包完成次数*/
    u32 u32NcmAtCbNum;               /* AT request回调处理次数*/
}NCM_DEBUG_INFO_S;

/* NCM私有数据信息 */
typedef struct tag_ncm_app_ctx
{
    u32                         ncm_id;       /* 设备ID号*/
    bool                        bUsed;          /* 是否被使用*/
    bool                        bEnable;        /* 设备是否使能*/
    bool                        chnl_open[NCM_DEV_CTRL_TYPE+1];
    bool                        bDataConnect;   /* 数据通道是否连接上*/
    int32_t                     s32atResponseRet;  /* AT命令发送结果*/
    u32                         pkt_encap_info[3]; /* 包封装结构*/
    u32                         u32AccumulationTime;
    u32                         u32FlowCtrlStus;   /* 流控状态*/
    u32                         u32TxCurMinNum;               /* 当前发包个数阈值*/
    u32                         u32TxCurTimeout;              /* 当前发包超时时间*/
    bool                        bAtSendTimeOut;              /*响应AT命令超时*/
    USB_CB_S                    cbs;   /* NCM相关回调函数*/
    NCM_DEBUG_INFO_S            debug_info;      /* 调试信息*/
    NCM_IOCTL_PKT_STATISTICS_S  pkt_stats; /* 统计信息*/
    u8                          au8Ipv6Dns[BSP_NCM_IPV6_DNS_LEN];
    u32                         u32Ipv6DnsLen;
    ncm_vendor_ctx_t            *vendor_ctx;
}ncm_app_ctx_t;

typedef struct tagNCM_SEND_OTHER_DEBUG_S
{
    u32 u32SendCurCount;              /* NCM当前正在发送的包个数*/
    u32 u32SendMaxCount;              /* NCM正在发包个数门限，超过该门限返回内存不足*/
    u32 u32SendExceedMaxCount;        /* 超过发包门限返回次数*/
    u32 u32SendNULLCount;             /* 传入参数为空或设备ID非法*/
}NCM_SEND_OTHER_DEBUG_S;

#define USB_GET_BUF(s32NcmId,PktEncap)   \
    (*(u32 *)((u32)PktEncap + ncm_app_ctx_set[s32NcmId].pkt_encap_info[0]))
#define USB_GET_LEN(s32NcmId,PktEncap)   \
    (*(u32 *)((u32)PktEncap + ncm_app_ctx_set[s32NcmId].pkt_encap_info[1]))
#define USB_GET_NEXT(s32NcmId,PktEncap)  \
    (*(u32 *)((u32)PktEncap + ncm_app_ctx_set[s32NcmId].pkt_encap_info[2]))

s32 ncm_vendor_init(ncm_vendor_ctx_t *vendor_ctx);
s32 ncm_vendor_uninit(ncm_vendor_ctx_t *vendor_ctx);

s32 ncm_vendor_notify(void *app_ncm, u32 cmd, void *param);

s32 ncm_vendor_encap_cmd(ncm_app_ctx_t *app_ctx, u8 *command, u32 size);

s32 ncm_mbim_send_encap_cmd(void * app_ctx ,unsigned char *data, unsigned int size);

void ncm_mbim_send_encap_rsp_done(int status);

s32 ncm_vendor_add_response(void *app_ncm, u8 *buf, u32 len);
int ncm_bind_func(void *app_ctx);
void ncm_unbind_func(void *app_ctx);
#endif  /* CONFIG_ETH_BYPASS_MODE */

s32 ncm_vendor_open(NCM_DEV_TYPE_E dev_type, u32 dev_id);
s32 ncm_vendor_close(NCM_DEV_TYPE_E dev_type, u32 dev_id);
s32 ncm_vendor_write(u32 dev_id, void *pkt_encap, void *net_priv);
s32 ncm_vendor_ioctl(u32 dev_id, NCM_IOCTL_CMD_TYPE_E cmd, void * param);

#endif /* __NCM_BALONG_H__ */
