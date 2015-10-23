/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : RnicEntity.c
  版 本 号   : 初稿
  作    者   : 范晶
  生成日期   : 2011年12月06日
  最近修改   :
  功能描述   : RNIC实体操作
  函数列表   :

  修改历史   :
  1.日    期   : 2011年12月06日
    作    者   : 范晶
    修改内容   : 创建文件

******************************************************************************/
#ifdef  __cplusplus
  #if  __cplusplus
  extern "C"{
  #endif
#endif

/******************************************************************************
   1 头文件包含
******************************************************************************/
#include "v_typdef.h"
#include "ImmInterface.h"
#include "RnicProcMsg.h"
#include "RnicLog.h"
#include "RnicEntity.h"
#include "RnicDebug.h"
#include "RnicCtx.h"
#if (FEATURE_ON == FEATURE_CL_INTERWORK)
#include "SdioInterface.h"
#endif
#include "RnicConfigInterface.h"

/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/

#define THIS_FILE_ID PS_FILE_ID_RNIC_ENTITY_C

/******************************************************************************
   2 外部函数变量声明
******************************************************************************/

/******************************************************************************
   3 私有定义
******************************************************************************/

/******************************************************************************
   4 全局变量定义
*****************************************************************************/

/* 将网卡的操作进行静态映射 */
#if (VOS_OS_VER == VOS_WIN32)
static struct net_device_ops rnic_ops;
#else
static const struct net_device_ops rnic_ops = {
       .ndo_stop                = RNIC_StopNetCard,
       .ndo_open                = RNIC_OpenNetCard,
       .ndo_start_xmit          = RNIC_StartXmit,
       .ndo_set_mac_address     = RNIC_SetMacAddress,
       .ndo_change_mtu          = RNIC_ChangeMtu,
       .ndo_tx_timeout          = RNIC_ProcTxTimeout,
       .ndo_do_ioctl            = RNIC_Ioctrl,
       .ndo_get_stats           = RNIC_GetNetCardStats,
};
#endif

#if (FEATURE_ON == FEATURE_MULTI_MODEM)
const RNIC_NETCARD_ELEMENT_TAB_STRU           g_astRnicManageTbl[RNIC_NET_ID_MAX_NUM] = {
    { RNIC_RM_NET_ID_0,                      MODEM_ID_0,                           "eth_x",
      {{0x58, 0x02, 0x03, 0x04, 0x05, 0x06}, {0x00, 0x11, 0x09, 0x64, 0x01, 0x01}, 0x0008},
      {{0x58, 0x02, 0x03, 0x04, 0x05, 0x06}, {0x00, 0x11, 0x09, 0x64, 0x01, 0x01}, 0xDD86}},
    { RNIC_RM_NET_ID_1,                      MODEM_ID_0,                           "eth_x1",
      {{0x58, 0x02, 0x03, 0x04, 0x05, 0x07}, {0x00, 0x11, 0x09, 0x64, 0x01, 0x02}, 0x0008},
      {{0x58, 0x02, 0x03, 0x04, 0x05, 0x07}, {0x00, 0x11, 0x09, 0x64, 0x01, 0x02}, 0xDD86}},
    { RNIC_RM_NET_ID_2,                      MODEM_ID_0,                           "eth_x2",
      {{0x58, 0x02, 0x03, 0x04, 0x05, 0x08}, {0x00, 0x11, 0x09, 0x64, 0x01, 0x03}, 0x0008},
      {{0x58, 0x02, 0x03, 0x04, 0x05, 0x08}, {0x00, 0x11, 0x09, 0x64, 0x01, 0x03}, 0xDD86}},
    { RNIC_RM_NET_ID_3,                      MODEM_ID_1,                           "eth_x3",
      {{0x58, 0x02, 0x03, 0x04, 0x05, 0x09}, {0x00, 0x11, 0x09, 0x64, 0x01, 0x04}, 0x0008},
      {{0x58, 0x02, 0x03, 0x04, 0x05, 0x09}, {0x00, 0x11, 0x09, 0x64, 0x01, 0x04}, 0xDD86}},
    { RNIC_RM_NET_ID_4,                      MODEM_ID_1,                           "eth_x4",
      {{0x58, 0x02, 0x03, 0x04, 0x05, 0x0a}, {0x00, 0x11, 0x09, 0x64, 0x01, 0x05}, 0x0008},
      {{0x58, 0x02, 0x03, 0x04, 0x05, 0x0a}, {0x00, 0x11, 0x09, 0x64, 0x01, 0x05}, 0xDD86}}
};
#else
const RNIC_NETCARD_ELEMENT_TAB_STRU           g_astRnicManageTbl[RNIC_NET_ID_MAX_NUM] = {
    { RNIC_RM_NET_ID_0,                      MODEM_ID_0,                           "eth_x",
      {{0x58, 0x02, 0x03, 0x04, 0x05, 0x06}, {0x00, 0x11, 0x09, 0x64, 0x01, 0x01}, 0x0008},
      {{0x58, 0x02, 0x03, 0x04, 0x05, 0x06}, {0x00, 0x11, 0x09, 0x64, 0x01, 0x01}, 0xDD86}},
    { RNIC_RM_NET_ID_1,                      MODEM_ID_0,                           "eth_x1",
      {{0x58, 0x02, 0x03, 0x04, 0x05, 0x07}, {0x00, 0x11, 0x09, 0x64, 0x01, 0x02}, 0x0008},
      {{0x58, 0x02, 0x03, 0x04, 0x05, 0x07}, {0x00, 0x11, 0x09, 0x64, 0x01, 0x02}, 0xDD86}},
    { RNIC_RM_NET_ID_2,                      MODEM_ID_0,                           "eth_x2",
      {{0x58, 0x02, 0x03, 0x04, 0x05, 0x08}, {0x00, 0x11, 0x09, 0x64, 0x01, 0x03}, 0x0008},
      {{0x58, 0x02, 0x03, 0x04, 0x05, 0x08}, {0x00, 0x11, 0x09, 0x64, 0x01, 0x03}, 0xDD86}}
};
#endif


/*lint -e762*/
#if (FEATURE_OFF == FEATURE_SKB_EXP)
extern int netif_rx(struct sk_buff *skb);
extern int netif_rx_ni(struct sk_buff *skb);
#else
extern int netif_rx_ni_balong(struct sk_buff *skb);
#endif

/*lint +e762*/

/******************************************************************************
   5 函数实现
******************************************************************************/
/*****************************************************************************
 函 数 名  : RNIC_ShowDataFromIpStack
 功能描述  : 打印收到IP协议栈的数据
 输入参数  : pstSkb   :SKBUF数据首地址
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2013年04月25日
   作    者   : 范晶
   修改内容   : 新生成函数
*****************************************************************************/
VOS_VOID RNIC_ShowDataFromIpStack(
    struct sk_buff                     *pstSkb
)
{
    VOS_UINT32                          i;

    /* RNIC 上行数据打印开关，打印收到TCP/IP协议栈的数据 */
    if (VOS_TRUE == g_ulRnicPrintUlDataFlg)
    {
        vos_printf("RNIC_ShowDataFromIpStack: data len is %d. \r\n", pstSkb->len);

        vos_printf("RNIC_ShowDataFromIpStack: data content is: \r\n");

        for (i = 0; i < pstSkb->len; i++)
        {
            if (pstSkb->data[i] > 0xf)
            {
                vos_printf("%x", pstSkb->data[i]);
            }
            else
            {
                vos_printf("0%x", pstSkb->data[i]);
            }
        }

        vos_printf("\r\n");
    }

    return;
}

/*****************************************************************************
 函 数 名  : RNIC_ShowDataDelMacHead
 功能描述  : 打印去掉MAC头的数据
 输入参数  : pstSkb   :SKBUF数据首地址
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2013年04月25日
   作    者   : 范晶
   修改内容   : 新生成函数
*****************************************************************************/
VOS_VOID RNIC_ShowDataDelMacHead(
    struct sk_buff                     *pstSkb
)
{
    VOS_UINT32                          i;

    /* RNIC 上行数据打印开关，打印去mac头的数据 */
    if (VOS_TRUE == g_ulRnicPrintUlDataFlg)
    {
        vos_printf("RNIC_ShowDataDelMacHead after pull: data len is %d. \r\n", pstSkb->len);

        vos_printf("RNIC_ShowDataDelMacHead after pull: data content is: \r\n");

        for (i = 0; i < pstSkb->len; i++)
        {
            if (pstSkb->data[i] > 0xf)
            {
                vos_printf("%x", pstSkb->data[i]);
            }
            else
            {
                vos_printf("0%x", pstSkb->data[i]);
            }
        }
        vos_printf("\r\n");
    }

    return;
}
/*****************************************************************************
 函 数 名  : RNIC_StopNetCard
 功能描述  : RNIC被关闭时的处理
 输出参数  : pstNetDev:网卡设备指针
 输出参数  : 无
 返 回 值  : VOS_INT:RNIC_OK, RNIC_ERROR
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年12月06日
   作    者   : 范晶
   修改内容   : 新生成函数
*****************************************************************************/
VOS_INT RNIC_StopNetCard(
    struct net_device                  *pstNetDev
)
{
    RNIC_NETCARD_DEV_INFO_STRU         *pstPriv;

    /* 如果网卡为空，则返回错误 */
    if (VOS_NULL_PTR == pstNetDev)
    {
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_StopNetCard:net ptr is Null!");
        return RNIC_ERROR;
    }

    /* 获取网卡私有数据 */
    pstPriv = (RNIC_NETCARD_DEV_INFO_STRU *)netdev_priv(pstNetDev);
    if (VOS_NULL_PTR == pstPriv)
    {
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_StopNetCard:priv ptr is Null!");
        return RNIC_ERROR;
    }

    /* 使用netif_stop_queue()停止网卡的数据收发 */
    netif_stop_queue(pstNetDev);

    /* 将私有数据中的网卡状态标志置为关闭 */
    pstPriv->enStatus = RNIC_NETCARD_STATUS_CLOSED;

    return RNIC_OK;

}

/*****************************************************************************
 函 数 名  : RNIC_OpenNetCard
 功能描述  : RNIC被打开时的处理
 输入参数  : pstNetDev:网卡设备指针
 输出参数  : 无
 返 回 值  : VOS_INT:RNIC_OK, RNIC_ERR, RNIC_BUSY
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年12月06日
   作    者   : 范晶
   修改内容   : 新生成函数
*****************************************************************************/
VOS_INT RNIC_OpenNetCard(
    struct net_device                  *pstNetDev
)
{
    RNIC_NETCARD_DEV_INFO_STRU         *pstPriv;

    /* 如果网卡为空，则返回错误 */
    if (VOS_NULL_PTR == pstNetDev)
    {
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_OpenNetCard:net ptr is Null!");
        return RNIC_ERROR;
    }

    /* 获取网卡私有数据 */
    pstPriv = (RNIC_NETCARD_DEV_INFO_STRU *)netdev_priv(pstNetDev);
    if (VOS_NULL_PTR == pstPriv)
    {
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_OpenNetCard:priv ptr is Null!");
        return RNIC_ERROR;
    }

    /* 该设备如果已经打开 */
    if (RNIC_NETCARD_STATUS_OPENED == pstPriv->enStatus)
    {
        RNIC_WARNING_LOG(ACPU_PID_RNIC, "RNIC_OpenNetCard:NetCard is opened");
        /* 返回错误码EBUSY */
        return RNIC_BUSY;
    }

    /* 启动网卡接收数据 */
    netif_start_queue(pstNetDev);

    /* 将私有数据中的网卡状态标志置为打开 */
    pstPriv->enStatus = RNIC_NETCARD_STATUS_OPENED;

    return RNIC_OK;

}


VOS_VOID RNIC_SendULDataInPdpActive(
    IMM_ZC_STRU                        *pstImmZc,
    RNIC_NETCARD_DEV_INFO_STRU         *pstPriv,
    VOS_UINT8                           ucRabId,
    VOS_UINT8                           ucNetIndex,
    ADS_PKT_TYPE_ENUM_UINT8             enIpType
)
{
    RNIC_UL_CTX_STRU                   *pstUlCtx;
    VOS_UINT8                           ucSendAdsRabId;
    MODEM_ID_ENUM_UINT16                enModemId;

    pstUlCtx                            = RNIC_GET_UL_CTX_ADDR(ucNetIndex);
    enModemId                           = RNIC_GET_MODEM_ID_BY_NET_ID(ucNetIndex);

    /* 根据Modem Id组装RabId */
    if (MODEM_ID_0 == enModemId)
    {
        ucSendAdsRabId = ucRabId;
    }
#if (FEATURE_ON == FEATURE_MULTI_MODEM)
    else if (MODEM_ID_1 == enModemId)
    {
        ucSendAdsRabId = ucRabId | RNIC_RABID_TAKE_MODEM_1_MASK;
    }
#endif
    else
    {
        /* 不是Modem0和Modem1发来的数据，直接丢弃 */
        RNIC_DBG_MODEM_ID_UL_DISCARD_NUM(1, ucNetIndex);

        /* 释放内存 */
        IMM_ZcFree(pstImmZc);

        /* 丢包统计 */
        pstPriv->stStats.tx_dropped++;
        pstUlCtx->stULDataStats.ulULTotalDroppedPkts++;

        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_ProcUlDataInPdpActive:Modem Id failed");
        return;
    }

    if (VOS_OK != ADS_UL_SendPacketEx(pstImmZc, enIpType, ucSendAdsRabId))
    {
        RNIC_DBG_SEND_UL_PKT_FAIL_NUM(1, ucNetIndex);

        /* 释放内存 */
        IMM_ZcFree(pstImmZc);

        /* 丢包统计 */
        pstPriv->stStats.tx_dropped++;
        pstUlCtx->stULDataStats.ulULTotalDroppedPkts++;

        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_ProcUlDataInPdpActive:Send packet failed");
        return;
    }

    RNIC_DBG_SEND_UL_PKT_NUM(1, ucNetIndex);

    /* 统计网卡发送信息 */
    pstPriv->stStats.tx_packets++;
    pstPriv->stStats.tx_bytes += pstImmZc->len;

    /* 统计发送的上行数据包个数，用于按需拨号断开 */
    pstUlCtx->stULDataStats.ulULPeriodSndPkts++;

    /* 统计发送的数据字节数，用于流量上报 */
    pstUlCtx->stULDataStats.ulULPeriodSndBytes += pstImmZc->len;
    pstUlCtx->stULDataStats.ulULTotalSndBytes  += pstImmZc->len;

    return;
}
VOS_UINT32 RNIC_TransSkbToImmZC(
    IMM_ZC_STRU                       **pstImmZc,
    struct sk_buff                     *pstSkb,
    RNIC_NETCARD_DEV_INFO_STRU         *pstPriv,
    VOS_UINT8                           ucNetIndex
)
{
#if(FEATURE_ON == FEATURE_SKB_EXP)
    RNIC_UL_CTX_STRU                   *pstUlCtx = VOS_NULL_PTR;
    IMM_MEM_STRU                       *pstImmMem = VOS_NULL_PTR;
    pstUlCtx                            = RNIC_GET_UL_CTX_ADDR(ucNetIndex);
#endif

#if(FEATURE_OFF == FEATURE_SKB_EXP)
    *pstImmZc = (IMM_ZC_STRU *)pstSkb;

    return VOS_OK;
#else
    /* 如果是Linux系统提供的数据，需要转换成跨核内存 */
    if (MEM_TYPE_SYS_DEFINED == pstSkb->private_mem.enType)
    {
        *pstImmZc = IMM_ZcStaticCopy((IMM_ZC_STRU *)pstSkb);

        /* 内存搬移失败需要释放内存 */
        if (VOS_NULL_PTR == *pstImmZc)
        {
             RNIC_DBG_UL_CHANGE_IMMZC_FAIL_NUM(1, ucNetIndex);

            /* 丢包统计 */
            pstPriv->stStats.tx_dropped++;
            pstUlCtx->stULDataStats.ulULTotalDroppedPkts++;

            /* 释放内存, 该接口内部来区分内存的来源 */
            IMM_ZcFree((IMM_ZC_STRU *)pstSkb);
            RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_SkbTransToImmZC:Malloc IMM failed");
            return VOS_ERR;
        }

        /* 转换成功后，内核不释放LINUX系统提供的数据内存，由RNIC直接释放掉 */
        IMM_ZcFree((IMM_ZC_STRU *)pstSkb);
    }
    /* 如果是用户管理的内存提供的数据 */
    else
    {
        pstImmMem = (IMM_MEM_STRU *)pstSkb->private_mem.pMem;

        /* A核共享内存池的内存,不需要转换 */
        if (IMM_MEM_POOL_ID_SHARE == pstImmMem->enPoolId)
        {
            *pstImmZc = (IMM_ZC_STRU *)pstSkb;
        }
        /* A核共享内存池以外的内存，需要重新转换一次跨核内存 */
        else
        {
            *pstImmZc = IMM_ZcStaticCopy((IMM_ZC_STRU *)pstSkb);

            /* 内存搬移失败需要释放内存 */
            if (VOS_NULL_PTR == *pstImmZc)
            {
                 RNIC_DBG_UL_CHANGE_IMMZC_FAIL_NUM(1, ucNetIndex);

                /* 丢包统计 */
                pstPriv->stStats.tx_dropped++;
                pstUlCtx->stULDataStats.ulULTotalDroppedPkts++;

                /* 释放内存, 该接口内部来区分内存的来源 */
                IMM_ZcFree((IMM_ZC_STRU *)pstSkb);
                RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_SkbTransToImmZC:Malloc IMM failed");
                return VOS_ERR;
            }

            /* 转换成功后，内核不释放LINUX系统提供的数据内存，由RNIC直接释放掉 */
            IMM_ZcFree((IMM_ZC_STRU *)pstSkb);
        }
    }

    return VOS_OK;
#endif
}
VOS_VOID RNIC_SendULIpv4Data(
    struct sk_buff                     *pstSkb,
    struct net_device                  *pstNetDev,
    RNIC_NETCARD_DEV_INFO_STRU         *pstPriv,
    VOS_UINT8                           ucNetIndex
)
{
    VOS_UINT32                          ulRst;
    IMM_ZC_STRU                        *pstImmZc;
    VOS_UINT8                           ucRabId;

    pstImmZc                            = VOS_NULL_PTR;

    RNIC_DBG_RECV_UL_IPV4_PKT_NUM(1, ucNetIndex);

    ucRabId = RNIC_GET_SPEC_NET_IPV4_RABID(ucNetIndex);

    /* 非法RABID */
    if (RNIC_RAB_ID_INVALID == ucRabId)
    {
        RNIC_DBG_RAB_ID_ERR_NUM(1, ucNetIndex);

        /* 释放内存 */
        IMM_ZcFree((IMM_ZC_STRU *)pstSkb);

        /* 告警信息 */
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_SendULIpv4Data:ipv4 is not act!");
        return;
    }

    /* 将应用下发的数据转换成跨核内存 */
    ulRst = RNIC_TransSkbToImmZC(&pstImmZc, pstSkb, pstPriv, ucNetIndex);
    if (VOS_OK != ulRst)
    {
        /* 如果转换失败，直接返回，不做后续的数据处理 */
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_SendULIpv4Data: TransSkbToImmZC Fail!");
        return;
    }

    /* PDP激活的情况下数据的处理 */
    RNIC_SendULDataInPdpActive(pstImmZc, pstPriv, ucRabId, ucNetIndex, ADS_PKT_TYPE_IPV4);

    return;
}


VOS_VOID RNIC_SendULIpv6Data(
    struct sk_buff                     *pstSkb,
    struct net_device                  *pstNetDev,
    RNIC_NETCARD_DEV_INFO_STRU         *pstPriv,
    VOS_UINT8                           ucNetIndex
)
{
    VOS_UINT32                          ulRst;
    IMM_ZC_STRU                        *pstImmZc;
    VOS_UINT8                           ucRabId;

    pstImmZc                            = VOS_NULL_PTR;

    RNIC_DBG_RECV_UL_IPV6_PKT_NUM(1, ucNetIndex);

    ucRabId = RNIC_GET_SPEC_NET_IPV6_RABID(ucNetIndex);

    /* 非法RABID */
    if (RNIC_RAB_ID_INVALID == ucRabId)
    {
        RNIC_DBG_RAB_ID_ERR_NUM(1, ucNetIndex);

        /* 释放内存 */
        IMM_ZcFree((IMM_ZC_STRU *)pstSkb);

        /* 告警信息 */
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_SendULIpv6Data:ipv6 is net act!");
        return;
    }

     /* 将应用下发的数据转换成跨核内存 */
    ulRst = RNIC_TransSkbToImmZC(&pstImmZc, pstSkb, pstPriv, ucNetIndex);
    if (VOS_OK != ulRst)
    {
        /* 如果转换失败，直接返回，不做后续的数据处理 */
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_SendULIpv6Data: TransSkbToImmZC Fail!");
        return;
    }

    /* PDP激活的情况下数据的处理 */
    RNIC_SendULDataInPdpActive(pstImmZc, pstPriv, ucRabId, ucNetIndex, ADS_PKT_TYPE_IPV6);

    return;

}



VOS_UINT32 RNIC_ProcDemDial(
    struct sk_buff                     *pstSkb
)
{
    RNIC_DIAL_MODE_STRU                *pstDialMode;
    RNIC_TIMER_STATUS_ENUM_UINT8        enTiStatus;
    VOS_UINT32                          ulIpAddr;

    /* 获取IP地址 */
    ulIpAddr = *((VOS_UINT32 *)((pstSkb->data) + RNIC_IP_HEAD_DEST_ADDR_OFFSET));

    /* 获取按需拨号的模式以及时长的地址 */
    pstDialMode = RNIC_GetDialModeAddr();

    /*如果是广播包，则不发起按需拨号，直接过滤调*/
    if ( RNIC_IPV4_BROADCAST_ADDR == ulIpAddr )
    {
        RNIC_DBG_UL_RECV_IPV4_BROADCAST_NUM(1, RNIC_RM_NET_ID_0);

        /* 释放内存 */
        IMM_ZcFree((IMM_ZC_STRU *)pstSkb);
        return VOS_ERR;
    }

    /* 获取当前拨号保护定时器的状态 */
    enTiStatus  = RNIC_GetTimerStatus(TI_RNIC_DEMAND_DIAL_PROTECT);

    /*为了防止按需拨号上报太快，启动一个两秒定时器，*/
    if (RNIC_TIMER_STATUS_STOP == enTiStatus)
    {
        /* 通知应用进行拨号操作 */
        if (RNIC_ALLOW_EVENT_REPORT == pstDialMode->enEventReportFlag)
        {
            if (VOS_OK == RNIC_SendDialEvent(DEVICE_ID_WAN, RNIC_DAIL_EVENT_UP))
            {
                /* 启动拨号保护定时器  */
                RNIC_StartTimer(TI_RNIC_DEMAND_DIAL_PROTECT, TI_RNIC_DEMAND_DIAL_PROTECT_LEN);
                RNIC_DBG_SEND_APP_DIALUP_SUCC_NUM(1, RNIC_RM_NET_ID_0);
                RNIC_NORMAL_LOG(ACPU_PID_RNIC, "RNIC_ProcDemDial: Send dial event succ.");
            }
            else
            {
                RNIC_DBG_SEND_APP_DIALUP_FAIL_NUM(1, RNIC_RM_NET_ID_0);
                RNIC_WARNING_LOG(ACPU_PID_RNIC, "RNIC_ProcDemDial: Send dial event fail.");
            }

            RNIC_MNTN_TraceDialConnEvt();
        }
    }

    /* 释放内存 */
    IMM_ZcFree((IMM_ZC_STRU *)pstSkb);

    return VOS_OK;
}

#if (FEATURE_ON == FEATURE_CL_INTERWORK)
netdev_tx_t RNIC_RcvOutsideModemUlData(
    struct sk_buff                     *pstSkb,
    struct net_device                  *pstNetDev,
    RNIC_NETCARD_DEV_INFO_STRU         *pstPriv,
    VOS_UINT8                           ucNetIndex
)
{
    VOS_UINT8                           ucIpType;
    VOS_UINT8                           ucPdnId;
    VOS_UINT32                          ulRslt;
    VOS_UINT32                          ulDataLen;
    RNIC_UL_CTX_STRU                   *pstUlCtx    = VOS_NULL_PTR;

    ulRslt                              = SDIO_ERR;
    ulDataLen                           = pstSkb->len;
    pstUlCtx                            = RNIC_GET_UL_CTX_ADDR(ucNetIndex);

    /* 普通的IP首部为20字节 */
    /*-------------------------------------------------------------------
      |      0        |       1        |       2        |       3        |
      |---------------|----------------|----------------|----------------|
      |0|1|2|3|4|5|6|7||0|1|2|3|4|5|6|7||0|1|2|3|4|5|6|7||0|1|2|3|4|5|6|7|
      --------------------------------------------------------------------
      | 协议  | 首部  |    服务类型    |            总长度               |
      | 版本  | 长度  |     (TOS)      |           (字节数)              |
      --------------------------------------------------------------------
      |            16位标识            | 标志 |         13位片偏移       |
      --------------------------------------------------------------------
      | 生存时间(TTL) |    8位协议     |        16位首部校验和           |
      --------------------------------------------------------------------
      |                           32位源IP地址                           |
      --------------------------------------------------------------------
      |                           32位目的IP地址                         |
      --------------------------------------------------------------------
    */
    /* 获取协议版本号 */
    ucIpType    = RNIC_GET_IP_VERSION(pstSkb->data[0]);

    /* IP报文类型判断 */
    switch(ucIpType)
    {
        /* IPv4 报文  IPv6 报文 */
        case RNIC_IPV4_VERSION:
            ucPdnId = RNIC_GET_SPEC_NET_IPV4_PDNID(ucNetIndex);
            break;

        case RNIC_IPV6_VERSION:
            ucPdnId = RNIC_GET_SPEC_NET_IPV6_PDNID(ucNetIndex);
            break;

        default:
            /* 可维可测 */
            RNIC_DBG_RECV_UL_ERR_PKT_NUM(1, ucNetIndex);
            pstPriv->stStats.tx_dropped++;
            pstUlCtx->stULDataStats.ulULTotalDroppedPkts++;

            /* 释放内存 */
            IMM_ZcFree((IMM_ZC_STRU *)pstSkb);
            return NETDEV_TX_OK;
    }

    /* 非法Pdn Id */
    if (RNIC_PDN_ID_INVALID == ucPdnId)
    {
        RNIC_DBG_PDN_ID_ERR_NUM(1, ucNetIndex);

        /* 释放内存 */
        IMM_ZcFree((IMM_ZC_STRU *)pstSkb);

        return NETDEV_TX_OK;
    }

    ulRslt  = SDIO_UL_SendPacket(pstSkb, ucPdnId);

    if (SDIO_OK != ulRslt)
    {
        RNIC_DBG_SEND_UL_PKT_FAIL_NUM(1, ucNetIndex);
        pstPriv->stStats.tx_dropped++;
        pstUlCtx->stULDataStats.ulULTotalDroppedPkts++;

        /* 释放内存 */
        IMM_ZcFree(pstSkb);
    }
    else
    {
        RNIC_DBG_SEND_UL_PKT_NUM(1, ucNetIndex);

        /* 统计网卡发送信息 */
        pstPriv->stStats.tx_packets++;
        pstPriv->stStats.tx_bytes += ulDataLen;

        /* 统计发送的上行数据包个数 */
        pstUlCtx->stULDataStats.ulULPeriodSndPkts++;

        /* 统计发送的数据字节数，用于流量上报 */
        pstUlCtx->stULDataStats.ulULPeriodSndBytes += ulDataLen;
        pstUlCtx->stULDataStats.ulULTotalSndBytes  += ulDataLen;
    }

    return NETDEV_TX_OK;
}
#endif
netdev_tx_t RNIC_RcvInsideModemUlData(
    struct sk_buff                     *pstSkb,
    struct net_device                  *pstNetDev,
    RNIC_NETCARD_DEV_INFO_STRU         *pstPriv,
    VOS_UINT8                           ucNetIndex
)
{
    VOS_UINT8                           ucIpType;
#if(FEATURE_ON == FEATURE_SKB_EXP)
    VOS_UINT32                          ulSndPermitFlg;
#endif

#if (FEATURE_ON == FEATURE_SKB_EXP)
    /* 获取上行允许发送标识 */
    ulSndPermitFlg = ADS_UL_IsSendPermit();
#endif

    /* 流控启动时, 直接丢弃数据包 */
#if (FEATURE_ON == FEATURE_SKB_EXP)
    if ((RNIC_FLOW_CTRL_STATUS_START == RNIC_GET_FLOW_CTRL_STATUS(ucNetIndex))
     && (VOS_FALSE == ulSndPermitFlg))
#else
    if (RNIC_FLOW_CTRL_STATUS_START == RNIC_GET_FLOW_CTRL_STATUS(ucNetIndex))
#endif
    {
        RNIC_DBG_FLOW_CTRL_UL_DISCARD_NUM(1, ucNetIndex);

        /* 释放内存 */
        IMM_ZcFree((IMM_ZC_STRU *)pstSkb);

        return NETDEV_TX_OK;
    }

    /* 普通的IP首部为20字节 */
    /*-------------------------------------------------------------------
      |      0        |       1        |       2        |       3        |
      |---------------|----------------|----------------|----------------|
      |0|1|2|3|4|5|6|7||0|1|2|3|4|5|6|7||0|1|2|3|4|5|6|7||0|1|2|3|4|5|6|7|
      --------------------------------------------------------------------
      | 协议  | 首部  |    服务类型    |            总长度               |
      | 版本  | 长度  |     (TOS)      |           (字节数)              |
      --------------------------------------------------------------------
      |            16位标识            | 标志 |         13位片偏移       |
      --------------------------------------------------------------------
      | 生存时间(TTL) |    8位协议     |        16位首部校验和           |
      --------------------------------------------------------------------
      |                           32位源IP地址                           |
      --------------------------------------------------------------------
      |                           32位目的IP地址                         |
      --------------------------------------------------------------------
    */
    /* 获取协议版本号 */
    ucIpType    = RNIC_GET_IP_VERSION(pstSkb->data[0]);

    /* 只在网卡0上面才会触发按需拨号 */
    if ((RNIC_IPV4_VERSION == ucIpType)
     && (AT_RNIC_DIAL_MODE_DEMAND_DISCONNECT == RNIC_GET_DIAL_MODE())
     && (RNIC_PDP_REG_STATUS_DEACTIVE == RNIC_GET_SPEC_NET_IPV4_REG_STATE(ucNetIndex))
     && (RNIC_RM_NET_ID_0 == ucNetIndex))
    {

        /* 处理按需拨号 */
        if (VOS_ERR == RNIC_ProcDemDial(pstSkb))
        {
            RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_StartXmit, the data is discared!");
        }

        return NETDEV_TX_OK;
    }

    /* IP报文类型判断 */
    switch(ucIpType)
    {
        /* IPv4 报文 */
        case RNIC_IPV4_VERSION:
            RNIC_SendULIpv4Data(pstSkb, pstNetDev, pstPriv, ucNetIndex);
            break;

        /* IPv6 报文 */
        case RNIC_IPV6_VERSION:
            RNIC_SendULIpv6Data(pstSkb, pstNetDev, pstPriv, ucNetIndex);
            break;

        default:
            RNIC_DBG_RECV_UL_ERR_PKT_NUM(1, ucNetIndex);

            /* 释放内存 */
            IMM_ZcFree((IMM_ZC_STRU *)pstSkb);
            break;
    }

    return NETDEV_TX_OK;
}
netdev_tx_t RNIC_StartXmit(
    struct sk_buff                     *pstSkb,
    struct net_device                  *pstNetDev
)
{
    RNIC_NETCARD_DEV_INFO_STRU         *pstPriv = VOS_NULL_PTR;
    VOS_UINT8                           ucNetIndex;

    /* 追踪上行接收数据 */
    RNIC_MNTN_TraceRcvUlData();

    /* 检查数据有效性 */
    if (VOS_NULL == pstSkb)
    {
        /* 告警信息 */
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_StartXmit:skb is invalid!");

        return NETDEV_TX_OK;
    }

    /* 检查网卡是否为空 */
    if (VOS_NULL == pstNetDev)
    {
        /* 该处记录的是网卡错误丢包个数,如果网卡错误了是不知道哪个网卡错误的
           目前将这种丢包记录在网卡0上面 */
        RNIC_DBG_NETCAED_UL_DISCARD_NUM(1, RNIC_RM_NET_ID_0);

        /* 释放内存 */
        IMM_ZcFree((IMM_ZC_STRU *)pstSkb);

        /* 告警信息 */
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_StartXmit:net ptr is Null!");

        return NETDEV_TX_OK;
    }
    /* 根据网卡设备的私有数据获取对应网卡ID */
    pstPriv    = (RNIC_NETCARD_DEV_INFO_STRU *)netdev_priv(pstNetDev);

    ucNetIndex = pstPriv->enRmNetId;

    if (ucNetIndex >= RNIC_NET_ID_MAX_NUM)
    {
        /* 该处记录的是网卡错误丢包个数,如果网卡错误了是不知道哪个网卡错误的
           目前将这种丢包记录在网卡0上面 */
        RNIC_DBG_NET_ID_UL_DISCARD_NUM(1, RNIC_RM_NET_ID_0);

        /* 释放内存 */
        IMM_ZcFree((IMM_ZC_STRU *)pstSkb);

        /* 告警信息 */
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_StartXmit:net index is Null!");

        return NETDEV_TX_OK;
    }

    /* 打印收到TCP/IP协议栈的数据 */
    RNIC_ShowDataFromIpStack(pstSkb);

    skb_pull(pstSkb, RNIC_ETH_HDR_SIZE);

    /* 打印去mac头的数据 */
    RNIC_ShowDataDelMacHead(pstSkb);

    if (RNIC_MODEM_TYPE_INSIDE == RNIC_GET_SPEC_NET_MODEM_TYPE(ucNetIndex))
    {
        return RNIC_RcvInsideModemUlData(pstSkb, pstNetDev, pstPriv, ucNetIndex);
    }
#if (FEATURE_ON == FEATURE_CL_INTERWORK)
    else if (RNIC_MODEM_TYPE_OUTSIDE == RNIC_GET_SPEC_NET_MODEM_TYPE(ucNetIndex))
    {
        return RNIC_RcvOutsideModemUlData(pstSkb, pstNetDev, pstPriv, ucNetIndex);
    }
#endif
    else
    {
        /* 释放内存 */
        IMM_ZcFree((IMM_ZC_STRU *)pstSkb);
        /* 是否添加数据统计 */
        return NETDEV_TX_OK;
    }

}


VOS_UINT32 RNIC_AddMacHead (
    IMM_ZC_STRU                        *pstImmZc,
    const VOS_UINT8                    *pucAddData
)
{
    VOS_UINT8                          *pucDestAddr;


    if (VOS_NULL == pstImmZc)
    {
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_AddMacHead :pstImmZc ptr is null!");
        return VOS_ERR;
    }

    if (VOS_NULL == pucAddData)
    {
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_AddMacHead: pucData ptr is null!");
        return VOS_ERR;
    }

    if(RNIC_ETH_HDR_SIZE > (pstImmZc->data - pstImmZc->head))
    {
        RNIC_ERROR_LOG2(ACPU_PID_RNIC, "RNIC_AddMacHead : invalid data Len! data = 0x%x, head = 0x%x \n",
                    (VOS_INT32)pstImmZc->data, (VOS_INT32)pstImmZc->head);

        return VOS_ERR;
    }

    pucDestAddr = skb_push(pstImmZc, RNIC_ETH_HDR_SIZE);

    VOS_MemCpy(pucDestAddr, pucAddData, RNIC_ETH_HDR_SIZE);

    return VOS_OK;

}

#if (FEATURE_ON == FEATURE_CL_INTERWORK)

VOS_VOID RNIC_ShowSdioDlData(
    VOS_UINT8                           ucPdnId,
    IMM_ZC_STRU                        *pstImmZc
)
{
    VOS_UINT32                          i;

    /* RNIC 上行数据打印开关，打印去mac头的数据 */
    if (VOS_TRUE == g_ulRnicPrintDlDataFlg)
    {
        vos_printf("RNIC_ShowSdioDlData : dl data pdn id is %d. \r\n", ucPdnId);
        vos_printf("RNIC_ShowSdioDlData : dl data len is %d. \r\n", pstImmZc->len);
        vos_printf("RNIC_ShowSdioDlData : dl data content is: \r\n");

        for (i = 0; i < pstImmZc->len; i++)
        {
            if (pstImmZc->data[i] > 0xf)
            {
                vos_printf("%x", pstImmZc->data[i]);
            }
            else
            {
                vos_printf("0%x", pstImmZc->data[i]);
            }
        }
        vos_printf("\r\n");
    }

    return;
}
VOS_UINT32 RNIC_RcvSdioDlData(
    VOS_UINT8                           ucPdnId,
    IMM_ZC_STRU                        *pstImmZc
)
{
    VOS_UINT8                           ucNetIndex;
    ADS_PKT_TYPE_ENUM_UINT8             enPktType;
    VOS_UINT8                           ucIpType;
    VOS_UINT32                          ulRet;

    /* 可谓可测，打印数据包内容和pdn id */
    RNIC_ShowSdioDlData(ucPdnId, pstImmZc);

    /* 根据PDNid获取对应的RMNETid */
    ucNetIndex     = RNIC_GET_RM_NET_ID_BY_PDN_ID(ucPdnId);

    /* 获取协议版本号 */
    ucIpType    = RNIC_GET_IP_VERSION(pstImmZc->data[0]);

    if (RNIC_IPV4_VERSION == ucIpType)
    {
        enPktType = ADS_PKT_TYPE_IPV4;
    }
    else if (RNIC_IPV6_VERSION == ucIpType)
    {
        enPktType = ADS_PKT_TYPE_IPV6;
    }
    else    /* 数据包类型与承载支持类型不一致 */
    {
        RNIC_DBG_RECV_DL_ERR_PKT_NUM(1, ucNetIndex);
        RNIC_ERROR_LOG1(ACPU_PID_RNIC, "RNIC_RcvSdioDlData, Ip Type is !", ucIpType);

        /* 释放内存 */
        IMM_ZcFree(pstImmZc);

        return RNIC_ERROR;
    }

    ulRet = RNIC_SendDlData(ucNetIndex, pstImmZc, enPktType);
    return ulRet;
}
#endif
VOS_UINT32  RNIC_RcvAdsDlData(
    VOS_UINT8                           ucRabid,
    IMM_ZC_STRU                        *pstImmZc,
    ADS_PKT_TYPE_ENUM_UINT8             enPktType
)
{
    VOS_UINT8                           ucNetIndex;
    VOS_UINT8                           ucUseRabid;
    VOS_UINT16                          usModemId;
    VOS_UINT32                          ulRet;

    /* ADS带的RABID，是由ModemId和RABID组合而成 */
    usModemId  = (ucRabid & RNIC_RABID_TAKE_MODEM_1_MASK) > 6;
    ucUseRabid = ucRabid & RNIC_RABID_UNTAKE_MODEM_1_MASK;

    /* 根据RABID获取对应网卡ID */
    ucNetIndex     = RNIC_GET_RM_NET_ID_BY_MODEM_ID(usModemId, ucUseRabid);

    ulRet = RNIC_SendDlData(ucNetIndex, pstImmZc, enPktType);

    return ulRet;
}



VOS_UINT32 RNIC_SendDlData(
    VOS_UINT8                           ucNetIndex,
    IMM_ZC_STRU                        *pstImmZc,
    ADS_PKT_TYPE_ENUM_UINT8             enPktType
)
{
    RNIC_NETCARD_DEV_INFO_STRU         *pstPriv     = VOS_NULL_PTR;
    RNIC_DL_CTX_STRU                   *pstDlCtx    = VOS_NULL_PTR;
    VOS_UINT8                          *pucAddData  = VOS_NULL_PTR;

    /* 追踪下行接收数据 */
    RNIC_MNTN_TraceRcvDlData();

    if (ucNetIndex >= RNIC_RM_NET_ID_BUTT)
    {
        RNIC_DBG_NETID_DL_DISCARD_NUM(1, RNIC_RM_NET_ID_0);

        /* 释放内存 */
        IMM_ZcFree(pstImmZc);

        /* 打印错误LOG */
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_SendDlData:ucNetIndex is null!");

        return RNIC_INVAL;
    }

    pstDlCtx       = RNIC_GET_DL_CTX_ADDR(ucNetIndex);

    /* 获取网卡私有数据地址 */
    pstPriv        = RNIC_GET_SPEC_PRIV_INFO_ADDR(ucNetIndex);

    /* RNIC的私有数据为空 */
    if (VOS_NULL_PTR == pstPriv)
    {
        RNIC_DBG_NETCAED_DL_DISCARD_NUM(1, ucNetIndex);

        /* 释放内存 */
        IMM_ZcFree(pstImmZc);

        /* 打印错误LOG */
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_SendDlData:priv ptr is null!");

        return RNIC_INVAL;
    }

    /* 网卡未打开 */
    if (RNIC_NETCARD_STATUS_CLOSED == pstPriv->enStatus)
    {
        RNIC_DBG_DISCARD_DL_PKT_NUM(1, ucNetIndex);

        /* 丢包统计，释放内存 */
        pstPriv->stStats.rx_dropped++;
        pstDlCtx->stDLDataStats.ulDLTotalDroppedPkts++;
        IMM_ZcFree(pstImmZc);

        /* 打印错误LOG */
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_SendDlData:Netcard is closed!");

        return RNIC_OK;
    }

    /* skb数据长度超出有效值(不包含MAC头) */
    if ((pstImmZc->len) > RNIC_MAX_PACKET)
    {
        RNIC_DBG_RECV_DL_BIG_PKT_NUM(1, ucNetIndex);

        /* 统计丢包和错包信息 */
        pstPriv->stStats.rx_errors++;
        pstPriv->stStats.rx_length_errors++;
        pstDlCtx->stDLDataStats.ulDLTotalDroppedPkts++;

        /* 释放内存 */
        IMM_ZcFree(pstImmZc);

        /* 打印错误LOG */
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_SendDlData:Data is invalid!");

        return RNIC_OK;
    }

    /* 填充MAC帧头，调用ImmZc接口将MAC帧头填入ImmZc中 */
    if (ADS_PKT_TYPE_IPV4 == enPktType)
    {
        RNIC_DBG_RECV_DL_IPV4_PKT_NUM(1, ucNetIndex);
        pucAddData = (VOS_UINT8*)&g_astRnicManageTbl[ucNetIndex].stIpv4Ethhead;
    }
    else if (ADS_PKT_TYPE_IPV6 == enPktType)
    {
        RNIC_DBG_RECV_DL_IPV6_PKT_NUM(1, ucNetIndex);
        pucAddData = (VOS_UINT8*)&g_astRnicManageTbl[ucNetIndex].stIpv6Ethhead;
    }
    else   /* 数据包类型与承载支持类型不一致 */
    {
        RNIC_DBG_RECV_DL_ERR_PKT_NUM(1, ucNetIndex);
        RNIC_ERROR_LOG1(ACPU_PID_RNIC, "RNIC_SendDlData, Rab is different from PktType!", enPktType);

        /* 释放内存 */
        IMM_ZcFree(pstImmZc);

        return RNIC_ERROR;
    }

    if (VOS_OK != RNIC_AddMacHead(pstImmZc, pucAddData))
    {
        RNIC_DBG_ADD_DL_MACHEAD_FAIL_NUM(1, ucNetIndex);
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_SendDlData, IMM_ZcAddMacHead fail!");

        /* 释放内存 */
        IMM_ZcFree(pstImmZc);

        return RNIC_ERROR;
    }

    pstImmZc->protocol = eth_type_trans(pstImmZc, pstPriv->pstNetDev);

    /* 统计网卡接收数据信息 */
    pstPriv->stStats.rx_packets++;
    pstPriv->stStats.rx_bytes += pstImmZc->len;

    /* 调用 netif_rx()函数将接收的数据节点递给内核 */
#if (FEATURE_OFF == FEATURE_SKB_EXP)
    if (NET_RX_SUCCESS != netif_rx_ni((struct sk_buff *)pstImmZc))
#else
    if (NET_RX_SUCCESS != netif_rx_ni_balong((struct sk_buff *)pstImmZc))
#endif
    {
        RNIC_DBG_SEND_DL_PKT_FAIL_NUM(1, ucNetIndex);

        /* 丢包统计，释放内存 */
        pstPriv->stStats.rx_dropped++;
        pstDlCtx->stDLDataStats.ulDLTotalDroppedPkts++;
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_SendDlData:Send data failed!");
        return RNIC_ERROR;
    }

    /* 增加下行发送数据统计 */
    RNIC_DBG_SEND_DL_PKT_NUM(1, ucNetIndex);

    /* 追踪下行发送数据 */
    RNIC_MNTN_TraceSndDlData();

    /* 统计收到的下行数据字节数，用于流量上报 */
    pstDlCtx->stDLDataStats.ulDLPeriodRcvBytes += pstImmZc->len;
    pstDlCtx->stDLDataStats.ulDLTotalRcvBytes  += pstImmZc->len;

    return RNIC_OK;

}

/*****************************************************************************
 函 数 名  : RNIC_SetMacAddress
 功能描述  : RNIC的重新设置MAC地址的处理
 输入参数  : pstNetDev:网卡设备指针
             pMacAddr :MAC地址
 输出参数  : 无
 返 回 值  : VOS_INT:RNIC_OK, RNIC_ADDR_INVALID
 调用函数  :
 被调函数  :

 修改历史     :
 1.日    期   : 2011年12月06日
   作    者   : 范晶
   修改内容   : 新生成函数
*****************************************************************************/
VOS_INT RNIC_SetMacAddress(
    struct net_device                  *pstNetDev,
    void                               *pMacAddr
)
{
    struct sockaddr                    *pstAddr;
    VOS_INT                             lisValid;

    /* 类型转换 */
    pstAddr                             = (struct sockaddr *)pMacAddr;

    /* 网卡指针为空 */
    if (VOS_NULL_PTR == pstNetDev)
    {
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_SetMacAddress:net ptr is Null!");

        return RNIC_ERROR;
    }

    /* MAC地址为无效值 */
    lisValid = is_valid_ether_addr((VOS_UINT8 *)pstAddr->sa_data);
    if (VOS_FALSE == lisValid)
    {
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_SetMacAddress:Ether addr is invalid!");

        return RNIC_ADDR_INVALID;
    }

    /* 网卡指针的dev_addr为空 */
    if (VOS_NULL_PTR == pstNetDev->dev_addr)
    {
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_SetMacAddress:Net dev addr is null!");

        return RNIC_ERROR;
    }

    /* 将MAC地址赋值到网卡 */
    VOS_MemCpy(pstNetDev->dev_addr, pstAddr->sa_data, pstNetDev->addr_len);

    return RNIC_OK;

}

/*****************************************************************************
 函 数 名  : RNIC_ChangeMtu
 功能描述  : RNIC重新设置MTU大小
 输入参数  : pstNetDev:网卡设备指针
             lNewMtu  :MTU值
 输出参数  : 无
 返 回 值  : VOS_INT:RNIC_OK, RNIC_ERROR, RNIC_OUT_RANGE
 调用函数  :
 被调函数  :

 修改历史     :
 1.日    期   : 2011年12月06日
   作    者   : 范晶
   修改内容   : 新生成函数
*****************************************************************************/
VOS_INT RNIC_ChangeMtu(
    struct net_device                  *pstNetDev,
    VOS_INT                             lNewMtu
)
{
    /* 网卡指针为空 */
    if (VOS_NULL_PTR == pstNetDev)
    {
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_ChangeMtu:net ptr is Null!");

        return RNIC_ERROR;
    }

    /* new_mtu长度不在有效范围内 */
    if (lNewMtu > RNIC_MAX_PACKET)
    {
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_ChangeMtu:Mtu out of range!");

        return RNIC_OUT_RANGE;
    }

    /* 网卡mtu完成赋值 */
    pstNetDev->mtu = (VOS_UINT)lNewMtu;

    return RNIC_OK;

}


/*****************************************************************************
 函 数 名  : RNIC_Tx_Timeout
 功能描述  : RNIC发送超时处理函数,目前认为不会出现发送超时情况,暂不作任何处理
             留到以后拓展使用
 输入参数  : pstNetDev:网卡设备指针
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年12月06日
   作    者   : 范晶
   修改内容   : 新生成函数
*****************************************************************************/
VOS_VOID RNIC_ProcTxTimeout(
    struct net_device                  *pstNetDev
)
{
    if (VOS_NULL_PTR == pstNetDev)
    {
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_ProcTxTimeout:net ptr is Null!");

        return;
    }

    RNIC_NORMAL_LOG(ACPU_PID_RNIC, "RNIC_ProcTxTimeout:OK!");
    return;
}

/*****************************************************************************
 函 数 名  : RNIC_Ioctrl
 功能描述  : RNIC的ioctrl处理,目前没有特殊的ioctrl,留作以后扩展
 输入参数  : pstNetDev:网卡设备指针
             pstIfr   :用户请求
             lCmd     :操作命令
 输出参数  : 无
 返 回 值  : 操作不支持  RNIC_NOTSUPP
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年12月06日
   作    者   : 范晶
   修改内容   : 新生成函数
*****************************************************************************/
VOS_INT RNIC_Ioctrl(
    struct net_device                  *pstNetDev,
    struct ifreq                       *pstIfr,
    VOS_INT                             lCmd
)
{
    RNIC_INFO_LOG(ACPU_PID_RNIC, "RNIC_Ioctrl:Ioctrl not supported!");

    return RNIC_NOTSUPP;
}


VOS_VOID RNIC_DeinitNetCard(
    VOS_VOID                           *pNetDev
)
{
    struct net_device                  *pstNetDev;
    RNIC_NETCARD_DEV_INFO_STRU         *pstPriv;

    pstNetDev                           = (struct net_device *)pNetDev;

    if (VOS_NULL_PTR == pstNetDev)
    {
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_DeinitNetCard:net ptr is Null!");

        return;
    }

    /* 获取网卡私有数据 */
    pstPriv = (RNIC_NETCARD_DEV_INFO_STRU *)netdev_priv(pstNetDev);
    if (pstPriv->enRmNetId >= RNIC_NET_ID_MAX_NUM)
    {
        return;
    }

    /* 关闭载波 */
    netif_carrier_off(pstNetDev);

    /* 停止网卡接收数据 */
    netif_stop_queue(pstNetDev);

    /* 去注册网卡设备 */
    unregister_netdev(pstNetDev);

    /* 释放网卡设备 */
    RNIC_SetSpecNetCardPrivate(VOS_NULL_PTR, pstPriv->enRmNetId);

    free_netdev(pstNetDev);

    return;

}

/*****************************************************************************
 函 数 名  : RNIC_GetNetCardStats
 功能描述  : 获取RNIC网卡统计信息
 输入参数  : pstNetDev:网卡设备指针
 输出参数  : 无
 返 回 值  : net_device_stats
 调用函数  :
 被调函数  :

 修改历史     :
 1.日    期   : 2011年12月06日
   作    者   : 范晶
   修改内容   : 新生成函数
*****************************************************************************/
struct net_device_stats *RNIC_GetNetCardStats(
    struct net_device                  *pstNetDev
)

{
    RNIC_NETCARD_DEV_INFO_STRU         *pstPriv ;

    if (VOS_NULL_PTR == pstNetDev)
    {
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_GetNetCardStats:net ptr is Null!");

        return VOS_NULL_PTR;
    }

    /* 获取私有数据 */
    pstPriv = (RNIC_NETCARD_DEV_INFO_STRU *)netdev_priv(pstNetDev);
    if (VOS_NULL_PTR == pstPriv)
    {
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_GetNetCardStats:Priv ptr is Null!");

        return VOS_NULL_PTR;
    }

    /* 将设备的统计信息返回 */
    return &pstPriv->stStats;
}


VOS_INT RNIC_InitNetCard(VOS_VOID)
{
    struct net_device                  *pstNetDev   = VOS_NULL_PTR;
    VOS_INT                             lRegRst;
    RNIC_NETCARD_DEV_INFO_STRU         *pstPriv ;
    RNIC_NETCARD_DEV_INFO_STRU         *pstNetCardPrivate ;
    VOS_UINT8                           ucIndex;

#if (FEATURE_ON == FEATURE_MULTI_MODEM)
    VOS_UINT8                           aucAddr[RNIC_NET_ID_MAX_NUM][RNIC_MAC_ADDR_LEN] = {
                                               {0x58,0x02,0x03,0x04,0x05,0x06},
                                               {0x58,0x02,0x03,0x04,0x05,0x07},
                                               {0x58,0x02,0x03,0x04,0x05,0x08},
                                               {0x58,0x02,0x03,0x04,0x05,0x09},
                                               {0x58,0x02,0x03,0x04,0x05,0x0a}
                                               };
#else
    VOS_UINT8                           aucAddr[RNIC_NET_ID_MAX_NUM][RNIC_MAC_ADDR_LEN] = {
                                               {0x58,0x02,0x03,0x04,0x05,0x06},
                                               {0x58,0x02,0x03,0x04,0x05,0x07},
                                               {0x58,0x02,0x03,0x04,0x05,0x08}
                                               };
#endif

    for (ucIndex = 0 ; ucIndex < RNIC_NET_ID_MAX_NUM ; ucIndex++)
    {
        /* 创建网卡设备netdev和私有数据 */
        /* alloc_etherdev这个接口分配了net_device和 RNIC_NETCARD_DEV_INFO_STRU两个接口的内存
           RNIC_NETCARD_DEV_INFO_STRU结构挂接在net_device的后面 */
        pstNetDev = alloc_etherdev(sizeof(RNIC_NETCARD_DEV_INFO_STRU));

        /*创建网卡失败，返回设备指针为NULL */
        if (VOS_NULL_PTR == pstNetDev)
        {
            RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_InitNetCard:net ptr is Null!");
            return RNIC_NOMEM;
        }

        /* 网卡设备地址为空时 */
        if (VOS_NULL_PTR == pstNetDev->dev_addr)
        {
            RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_InitNetCard:net addr ptr is Null!");
            free_netdev(pstNetDev);
            return RNIC_NOMEM;
        }

        /* 给设备配置MAC地址,以后需要填固定值 */
        VOS_MemCpy(pstNetDev->dev_addr, aucAddr[ucIndex], RNIC_MAC_ADDR_LEN);

        /* 设置默认的MTU值 */
        pstNetDev->mtu = RNIC_DEFAULT_MTU;

        VOS_sprintf(pstNetDev->name, g_astRnicManageTbl[ucIndex].pucRnicNetCardName);

        /* 对申请到的net_device结构的设备指针的函数域进行赋值 */
#if (VOS_OS_VER == VOS_WIN32)

#else
        pstNetDev->netdev_ops = &rnic_ops;
#endif

        /* 获取网卡私有数据虚存地址，保存网卡上下文中 */
        pstPriv = (RNIC_NETCARD_DEV_INFO_STRU *)netdev_priv(pstNetDev);
        pstPriv->enRmNetId = ucIndex;
        RNIC_SetSpecNetCardPrivate(pstPriv, ucIndex);

        /* 将网卡虚存地址保存在私有数据中 */
        pstNetCardPrivate = RNIC_GetSpecNetCardPrivateAddr(ucIndex);
        pstNetCardPrivate->pstNetDev = pstNetDev;

        /* 关闭载波 */
        netif_carrier_off(pstNetDev);

        /* 停止网卡接收数据 */
        netif_stop_queue(pstNetDev);

        /* 调用register_netdev函数对Netcard进行注册 */
        lRegRst = register_netdev(pstNetDev);
        if (VOS_OK != lRegRst)
        {
            /*  注册失败释放创建的RNIC设备 */
            free_netdev(pstNetDev);

            RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_InitNetCard:Register netdev failed!");

            return RNIC_STATE;
        }

        /* 打开载波 */
        netif_carrier_on(pstNetDev);

        /* 启动网卡接收数据 */
        netif_start_queue(pstNetDev);
    }

    /* 返回成功 */
    return RNIC_OK;
}


unsigned long RNIC_StartFlowCtrl(unsigned char ucRmNetId)
{
    RNIC_SET_FLOW_CTRL_STATUS(RNIC_FLOW_CTRL_STATUS_START, ucRmNetId);

    return VOS_OK;
}


unsigned long RNIC_StopFlowCtrl(unsigned char ucRmNetId)
{
    RNIC_SET_FLOW_CTRL_STATUS(RNIC_FLOW_CTRL_STATUS_STOP, ucRmNetId);

    return VOS_OK;
}


unsigned long RNIC_ConfigRmnetStatus(
    RNIC_RMNET_CONFIG_STRU             *pstConfigInfo
)
{
    /*可维可测，输出配置信息*/
    RNIC_MNTN_SndRmnetConfigInfoMsg(pstConfigInfo);
    RNIC_DBG_CONFIGCHECK_ADD_TOTLA_NUM();

    /* 参数检查 */
    /* 内部modem需要检查 rab id是否异常 */
    if (RNIC_MODEM_TYPE_INSIDE == pstConfigInfo->enModemType)
    {
        if (!RNIC_RAB_ID_IS_VALID(pstConfigInfo->ucRabId))
        {
            RNIC_DBG_CONFIGCHECK_ADD_RABID_ERR_NUM();
            RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_ConfigRmnetStatus: invaild RAB id !");
            return VOS_ERR;
        }
    }
    /* 外部modem需要检查 pdn id是否异常 */
    else if (RNIC_MODEM_TYPE_OUTSIDE == pstConfigInfo->enModemType)
    {
        if (!RNIC_PDN_ID_IS_VALID(pstConfigInfo->ucPdnId))
        {
            RNIC_DBG_CONFIGCHECK_ADD_PDNID_ERR_NUM();
            RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_ConfigRmnetStatus: invaild Pdn id !");
            return VOS_ERR;
        }
    }
    /* 无效MODEM TYPE */
    else
    {
        RNIC_DBG_CONFIGCHECK_ADD_MODEMTYPE_ERR_NUM();
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_ConfigRmnetStatus: invaild modem type!");
        return VOS_ERR;
    }


    /* 网卡操作类型异常直接返回error */
    if (!RNIC_RMNET_STATUS_IS_VALID(pstConfigInfo->enRmnetStatus))
    {
        RNIC_DBG_CONFIGCHECK_ADD_RMNETSTATUS_ERR_NUM();
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_ConfigRmnetStatus: invaild Rmnet Status !");
        return VOS_ERR;
    }

    /* IP类型非法直接返回error */
    if (!RNIC_IP_TYPE_IS_VALID(pstConfigInfo->enIpType))
    {
        RNIC_DBG_CONFIGCHECK_ADD_IPTYPE_ERR_NUM();
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_ConfigRmnetStatus: invaild IP type !");
        return VOS_ERR;
    }

    /* modem id异常检查，目前只接收配置modem0 */
    if (MODEM_ID_0 != pstConfigInfo->usModemId)
    {
        RNIC_DBG_CONFIGCHECK_ADD_MODEMID_ERR_NUM();
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_ConfigRmnetStatus: invaild modem id !");
        return VOS_ERR;
    }

    /* ucRmNetId 异常检查 目前RNIC_RM_NET_ID_3和RNIC_RM_NET_ID_4给副卡使用*/
    if (!RNIC_MODEM_0_RMNET_ID_IS_VALID(pstConfigInfo->ucRmNetId))
    {
        RNIC_DBG_CONFIGCHECK_ADD_RMNETID_ERR_NUM();
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_ConfigRmnetStatus: invaild RmNet id !");
        return VOS_ERR;
    }

    /* 发送内部消息 */
    if (VOS_OK != RNIC_SndRnicRmnetConfigReq(pstConfigInfo))
    {
        RNIC_DBG_CONFIGCHECK_ADD_SND_ERR_NUM();
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_ConfigRmnetStatus: send pdp status ind fail !");
        return VOS_ERR;
    }

    RNIC_DBG_CONFIGCHECK_ADD_SUCC_NUM();
    return VOS_OK;
}

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
