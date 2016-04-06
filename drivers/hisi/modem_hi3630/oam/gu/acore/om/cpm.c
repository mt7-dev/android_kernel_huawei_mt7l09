/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : cpm.c
  版 本 号   : 初稿
  作    者   : 甘兰 47350
  生成日期   : 2011年9月29日
  最近修改   :
  功能描述   : 实现OAM通道的管理功能,channel port manager.
  函数列表   :
  修改历史   :
  1.日    期   : 2011年9月29日
    作    者   : 甘兰 47350
    修改内容   : 创建文件

******************************************************************************/


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
#define    THIS_FILE_ID        PS_FILE_ID_CPM_C


/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "cpm.h"

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

CPM_PHY_PORT_CFG_STRU   g_astCPMPhyPortCfg[CPM_PORT_BUTT - CPM_APP_PORT];
CPM_LOGIC_PORT_CFG_STRU g_astCPMLogicPortCfg[CPM_COMM_BUTT];

/*****************************************************************************
  3 函数体申明
*****************************************************************************/

extern VOS_UINT32 OM_SDInit(VOS_VOID);
extern VOS_VOID GU_OamPortInit(VOS_VOID);
extern VOS_UINT32 Sock_PortInit(VOS_VOID);
extern VOS_UINT32 OM_LogFileInit(VOS_VOID);

/*****************************************************************************
  4 函数体定义
*****************************************************************************/

/*****************************************************************************
 函 数 名  : CPM_PhySendReg
 功能描述  : 提供给外部的注册函数，用来物理通道接收到数据的处理
 输入参数  : enPhyPort：  注册的物理通道号
             pRecvFunc：  数据接收函数
 输出参数  : 无
 返 回 值  : 无

*****************************************************************************/
VOS_VOID CPM_PhySendReg(CPM_PHY_PORT_ENUM_UINT32 enPhyPort, CPM_FUNC pSendFunc)
{
    if (CPM_PORT_BUTT > enPhyPort)
    {
        CPM_PHY_SEND_FUNC(enPhyPort - CPM_APP_PORT) = pSendFunc;
    }

    return;
}

/*****************************************************************************
 函 数 名  : CPM_LogicRcvReg
 功能描述  : 给逻辑通道注册接收函数
 输入参数  : enLogicPort： 注册的逻辑通道号
             pRecvFunc：   数据接收函数
 输出参数  : 无
 返 回 值  : 无

*****************************************************************************/
VOS_VOID CPM_LogicRcvReg(CPM_LOGIC_PORT_ENUM_UINT32 enLogicPort, CPM_FUNC pRcvFunc)
{
    if (CPM_COMM_BUTT > enLogicPort)
    {
        CPM_LOGIC_RCV_FUNC(enLogicPort) = pRcvFunc;
    }

    return;
}

/*****************************************************************************
 函 数 名  : CPM_QueryPhyPort
 功能描述  : 查询当前逻辑通道使用的物理端口
 输入参数  : enLogicPort：  逻辑通道号
 输出参数  : 无
 返 回 值  : 物理通道号

*****************************************************************************/
CPM_PHY_PORT_ENUM_UINT32 CPM_QueryPhyPort(CPM_LOGIC_PORT_ENUM_UINT32 enLogicPort)
{
    return CPM_LOGIC_PHY_PORT(enLogicPort);
}

/*****************************************************************************
 函 数 名  : CPM_ConnectPorts
 功能描述  : 将物理通道和逻辑通道连接上
 输入参数  : enPhyPort：    物理通道号
             enLogicPort：  逻辑通道号
 输出参数  : 无
 返 回 值  : 无

*****************************************************************************/
VOS_VOID CPM_ConnectPorts(CPM_PHY_PORT_ENUM_UINT32 enPhyPort, CPM_LOGIC_PORT_ENUM_UINT32 enLogicPort)
{
    if ((CPM_PORT_BUTT <= enPhyPort) || (CPM_COMM_BUTT <= enLogicPort))
    {
        return;
    }

    if (CPM_APP_PORT > enPhyPort)
    {
        /* 从AT获得发送数据的函数指针 */
        CPM_LOGIC_SEND_FUNC(enLogicPort) = AT_QuerySndFunc(enPhyPort);

        /* 将逻辑通道的接收函数给AT模块 */
        AT_RcvFuncReg(enPhyPort, CPM_LOGIC_RCV_FUNC(enLogicPort));
    }
    else
    {
        /* 连接发送通道 */
        CPM_LOGIC_SEND_FUNC(enLogicPort)= CPM_PHY_SEND_FUNC(enPhyPort - CPM_APP_PORT);

        /* 连接接收通道 */
        CPM_PHY_RCV_FUNC(enPhyPort - CPM_APP_PORT) = CPM_LOGIC_RCV_FUNC(enLogicPort);
    }

    /* 将物理发送函数注册给逻辑通道 */
    CPM_LOGIC_PHY_PORT(enLogicPort) = enPhyPort;

    return;
}

/*****************************************************************************
 函 数 名  : CPM_DisconnectPorts
 功能描述  : 断开物理通道和逻辑通道连接
 输入参数  : enPhyPort：    物理通道号
             enLogicPort：  逻辑通道号
 输出参数  : 无
 返 回 值  : 无

*****************************************************************************/
VOS_VOID CPM_DisconnectPorts(CPM_PHY_PORT_ENUM_UINT32 enPhyPort, CPM_LOGIC_PORT_ENUM_UINT32 enLogicPort)
{
    if ((CPM_PORT_BUTT <= enPhyPort) || (CPM_COMM_BUTT <= enLogicPort))
    {
        return;
    }

    /* 假如当前逻辑通道并没有使用此物理通道，则不用处理 */
    if (enPhyPort != CPM_LOGIC_PHY_PORT(enLogicPort))
    {
        return;
    }

    if (CPM_APP_PORT > enPhyPort)
    {
        /* 向AT去注册接收函数 */
        AT_RcvFuncReg(enPhyPort, VOS_NULL_PTR);
    }
    else
    {
        /* 断开接收通道 */
        CPM_PHY_RCV_FUNC(enPhyPort - CPM_APP_PORT) = VOS_NULL_PTR;
    }

    /* 断开发送通道 */
    CPM_LOGIC_SEND_FUNC(enLogicPort)= VOS_NULL_PTR;
    CPM_LOGIC_PHY_PORT(enLogicPort) = CPM_PORT_BUTT;

    return;
}

/*****************************************************************************
 函 数 名  : CPM_NotifyChangePort
 功能描述  : 提供给ACPU AT模块调用，用来通知OM进行端口切换
 输入参数  : enLogicPort：逻辑通道号
             enPhyPort：  物理通道号
 输出参数  : 无
 返 回 值  : VOS_OK:成功，其他为失败

*****************************************************************************/
VOS_UINT32 CPM_NotifyChangePort(AT_PHY_PORT_ENUM_UINT32 enPhyPort,
                                       CPM_LOGIC_PORT_ENUM_UINT32 enLogicPort)
{
    /* 参数检测 */
    if ((enPhyPort >= CPM_PORT_BUTT) || (enLogicPort>= CPM_COMM_BUTT))
    {
        return VOS_ERR;
    }

    /* 判断当前OM通道是否为WIFI */
    if (CPM_WIFI_OM_PORT == CPM_LOGIC_PHY_PORT(enLogicPort))
    {
        /* 关闭SOCKET连接 */
    }

    /* 断开当前通道 */
    CPM_DisconnectPorts(CPM_LOGIC_PHY_PORT(enLogicPort), enLogicPort);

    CPM_ConnectPorts(enPhyPort, enLogicPort);

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : CPM_PnpAppPortCB
 功能描述  : 注册给DRV的插拔回调函数
 输入参数  : bPortState： VOS_TRUE通道已连接、VOS_FALSE通道已断开
 输出参数  : 无
 返 回 值  : VOS_OK:成功，其他为失败

*****************************************************************************/
VOS_VOID CPM_PnpAppPortCB(CPM_PHY_PORT_ENUM_UINT32 enPhyPort, VOS_BOOL bPortState)
{
    CPM_PHY_PORT_ENUM_UINT32 enCurPhyPort;

    /* USB通道已打开 */
    if (VOS_TRUE == bPortState)
    {
        enCurPhyPort = CPM_LOGIC_PHY_PORT(CPM_OM_COMM);

        if ((CPM_APP_PORT != enCurPhyPort) && (CPM_CTRL_PORT != enCurPhyPort))
        {
            CPM_DisconnectPorts(enCurPhyPort, CPM_OM_COMM);

            /* 连接APP口和OM通道 */
            CPM_ConnectPorts(enPhyPort, CPM_OM_COMM);
        }

        return ;
    }

    /* USB通道已关闭，断开APP口与OM通道 */
    CPM_DisconnectPorts(CPM_APP_PORT, CPM_OM_COMM);

    return ;
}
VOS_UINT32 CPM_InitPhyPort(VOS_VOID)
{
    if (VOS_OK != OM_LogFileInit())
    {
        PS_LOG(WUEPS_PID_OM, 0, PS_PRINT_WARNING, "OM_Init: OM_SDInit failed.\n");
        return VOS_ERR;
    }

    GU_OamPortInit();

#if ((VOS_OS_VER == VOS_WIN32) || (FEATURE_HISOCKET == FEATURE_ON))
    if (VOS_OK != Sock_PortInit())
    {
        return VOS_ERR;
    }
#endif

    return VOS_OK;
}
/*****************************************************************************
 函 数 名  : CPM_ComSend
 功能描述  : 发送数据函数，提供给逻辑通道使用
 输入参数  : enLogicPort：逻辑通道号
             pucData：    发送数据的指针
             ulLen:       发送数据的长度

 输出参数  : 无
 返 回 值  : VOS_OK:成功，其他为失败

*****************************************************************************/
VOS_UINT32 CPM_ComSend(CPM_LOGIC_PORT_ENUM_UINT32 enLogicPort, VOS_UINT8 *pucData, VOS_UINT32 ulLen)
{
    /* 参数检测 */
    if ((CPM_COMM_BUTT <= enLogicPort) || (VOS_NULL_PTR == pucData) || (0 == ulLen))
    {
        return CPM_SEND_PARA_ERR;
    }

    if (VOS_NULL_PTR == CPM_LOGIC_SEND_FUNC(enLogicPort))
    {
        return CPM_SEND_FUNC_NULL;
    }

    return CPM_LOGIC_SEND_FUNC(enLogicPort)(pucData, ulLen);
}

/*****************************************************************************
 函 数 名  : CPM_ComRcv
 功能描述  : 接收数据函数，提供给物理通道使用
 输入参数  : enPhyPort：  物理通道号
             pucData：    接收数据的指针
             ulLen:       接收数据的长度
 输出参数  : 无
 返 回 值  : VOS_OK:成功，其他为失败

*****************************************************************************/
VOS_UINT32 CPM_ComRcv(CPM_PHY_PORT_ENUM_UINT32 enPhyPort, VOS_UINT8 *pucData, VOS_UINT32 ulLen)
{
    /* 参数检测 */
    if ((CPM_PORT_BUTT <= enPhyPort) || (VOS_NULL_PTR == pucData) || (0 == ulLen))
    {
        return VOS_ERR;
    }

    if (VOS_NULL_PTR == CPM_PHY_RCV_FUNC(enPhyPort - CPM_APP_PORT))
    {
        LogPrint1("\r\nCPM_ComRcv The Phy Port %d Rec Func is NULL", (VOS_INT)enPhyPort);

        return VOS_ERR;
    }

    return CPM_PHY_RCV_FUNC(enPhyPort - CPM_APP_PORT)(pucData, ulLen);
}

/*****************************************************************************
 函 数 名  : CPM_Show
 功能描述  : 显示当前的逻辑和物理端口对应关系
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无

*****************************************************************************/
VOS_VOID CPM_Show(VOS_VOID)
{
    CPM_PHY_PORT_ENUM_UINT32    enPhyPort;
    CPM_LOGIC_PORT_ENUM_UINT32  enLogicPort;

    vos_printf("\r\nCPM_Show The Logic and Phy Relation is :");

    for(enLogicPort=CPM_AT_COMM; enLogicPort<CPM_COMM_BUTT; enLogicPort++)
    {
        enPhyPort = CPM_QueryPhyPort(enLogicPort);

        vos_printf("\r\nThe Logic Port %d is connnect PHY Port %d.", enLogicPort, enPhyPort);
    }

    vos_printf("\r\nCPM_Show The Phy Info is :");

    for(enPhyPort=0; enPhyPort<(CPM_PORT_BUTT - CPM_APP_PORT); enPhyPort++)
    {
        vos_printf("\r\nThe Phy %d Port's Rec Func is 0x%x, Send Func is 0x%x",
                        enPhyPort,
                        g_astCPMPhyPortCfg[enPhyPort].pRcvFunc,
                        g_astCPMPhyPortCfg[enPhyPort].pSendFunc);
    }

    for(enLogicPort=0; enLogicPort<CPM_COMM_BUTT; enLogicPort++)
    {
        vos_printf("\r\nThe Logic %d Port's Rec Func is 0x%x, Send Func is 0x%x",
                        enLogicPort,
                        g_astCPMLogicPortCfg[enLogicPort].pRcvFunc,
                        g_astCPMLogicPortCfg[enLogicPort].pSendFunc);
    }

    return;
}

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
