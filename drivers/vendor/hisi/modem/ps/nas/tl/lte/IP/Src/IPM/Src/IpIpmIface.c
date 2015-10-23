/******************************************************************************

   Copyright(C)2008,Hisilicon Co. LTD.

 ******************************************************************************
  File Name       : IpIpmIface.c
  Description     : IP模块的文件
  History         :
     1.hanlufeng 41410       2010-8-2   Draft Enact
******************************************************************************/


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
#include  "IpIpmIface.h"
#include  "IpIpmOm.h"
#include  "IpIpmGlobal.h"
#include  "IpIpmReg.h"
#include  "IpIpmMsgProc.h"
/*#include  "IpDhcpv4Server.h"*/

/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_IPIPMIFACE_C
/*lint +e767*/



/*****************************************************************************
  2 Declare the Global Variable
*****************************************************************************/



/*****************************************************************************
  3 Function
*****************************************************************************/
/*****************************************************************************
 Function Name   : IP_IsIpv4UdpPacket
 Description     : 判定是否为IPV4包，且上层协议为UDP
 Input           : pucIpMsg---------------------IP数据包首地址
                   ulIpMsgLen-------------------IP数据包长度
 Output          : VOS_VOID
 Return          : VOS_UINT32

 History         :
    1.lihong00150010      2010-11-29  Draft Enact

*****************************************************************************/
VOS_UINT32 IP_IsIpv4UdpPacket
(
    const VOS_UINT8                    *pucIpMsg,
    VOS_UINT32                          ulIpMsgLen
)
{
    VOS_UINT8                           ucProtocol      = IP_NULL;
    VOS_UINT8                           ucIpVersion     = IP_NULL;

    /* 长度合法检查 */
    if (ulIpMsgLen < IP_IPV4_HEAD_LEN)
    {
        return PS_FAIL;
    }

    /* 获取IP版本号 */
    ucIpVersion = IP_GetIpVersion(pucIpMsg);

    /* 如果版本号不是IPV4，则不是DHCPV4包 */
    if (IP_VERSION_4 != ucIpVersion)
    {
        return PS_FAIL;
    }

    /* 取出协议 */
    ucProtocol = IP_GetProtocol(pucIpMsg);

    /* 若协议不是UDP，则不是DHCPV4包 */
    if (IP_HEAD_PROTOCOL_UDP != ucProtocol)
    {
        return PS_FAIL;
    }

    return PS_SUCC;
}

/*****************************************************************************
 Function Name   : IP_IsDhcpv4ServerPacket
 Description     : 判定是否为DHCPV4 SERVER包
 Input           : pucIpMsg---------------------IP数据包首地址
                   ulIpMsgLen-------------------IP数据包长度
 Output          : VOS_VOID
 Return          : VOS_UINT32

 History         :
    1.lihong00150010      2010-11-29  Draft Enact

*****************************************************************************/
/*lint -e960*/
/*lint -e961*/
VOS_UINT32 IP_IsDhcpv4ServerPacket
(
    const VOS_UINT8                    *pucIpMsg,
    VOS_UINT32                          ulIpMsgLen
)
{
    VOS_UINT8                           ucIpHeadLen     = IP_NULL;
    VOS_UINT16                          usPortDes       = IP_NULL;

    /* 判定是否为IPV4包，且上层协议为UDP */
    if (PS_SUCC != IP_IsIpv4UdpPacket(pucIpMsg, ulIpMsgLen))
    {
        return PS_FAIL;
    }

    /* 取出ip头长度 */
    ucIpHeadLen = IP_GetIpv4HeaderLen(pucIpMsg);

    /* 判断端口号是否为67 */
    IP_GetUdpDesPort(pucIpMsg + ucIpHeadLen, usPortDes);
    if (IP_PORT_DHCPV4_SERVER != usPortDes)
    {
        return PS_FAIL;
    }

    return PS_SUCC;
}
VOS_VOID IP_Init(VOS_VOID)
{
    /* 打印进入该函数 */
    IP_LOG_INFO("IP_Init is entered.\n");

    /* IPM子模块初始化 */
    IP_IPM_Init();

    /* DHCPV4 SERVER子模块初始化 */
    /*IP_DHCPV4SEVER_Init();*/
}

/*****************************************************************************
 Function Name  : IP_PidInit
 Discription    : ip的PID初始化函数
 Input          :
 Output         : None
 Return         : None
 History:
      1.lihong00150010      2010-11-29  Draft Enact

*****************************************************************************/
VOS_UINT32 IP_PidInit ( enum VOS_INIT_PHASE_DEFINE ip )
{
    switch( ip )
    {
        case    VOS_IP_LOAD_CONFIG:
                {
                    IP_Init();
                }
                break;

        case    VOS_IP_FARMALLOC:
        case    VOS_IP_INITIAL:
        case    VOS_IP_ENROLLMENT:
        case    VOS_IP_LOAD_DATA:
        case    VOS_IP_FETCH_DATA:
        case    VOS_IP_STARTUP:
        case    VOS_IP_RIVAL:
        case    VOS_IP_KICKOFF:
        case    VOS_IP_STANDBY:
        case    VOS_IP_BROADCAST_STATE:
        case    VOS_IP_RESTART:
        case    VOS_IP_BUTT:
                break;

        default:
                break;
    }

    return VOS_OK;
}


/*****************************************************************************
 Function Name   : IP_TaskEntry
 Description     : 所有进入IP模块的消息，从这里进入IP模块，由此函数根据路由信息
                   进一步分发给IP的子模块，即调用各子模块的消息入口
 Input           : None
 Output          : None
 Return          : VOS_VOID

 History         :
    1.lihong00150010      2010-11-22  Draft Enact

*****************************************************************************/
VOS_VOID  IP_TaskEntry
(
    MsgBlock                           *pstMsg
)
{
    /* 定义消息头指针*/
    PS_MSG_HEADER_STRU                 *pHeader = IP_NULL_PTR;
    IP_ROUTER_MEG_ENTRY_FUN             pActFun = IP_NULL_PTR;
    VOS_UINT32                          ulMsgId = IP_NULL;
    IP_ERR_ENUM_UINT32                  enRslt  = IP_ERR_BUTT;

    /* 判断入口参数是否合法*/
    if( VOS_NULL_PTR == pstMsg)
    {
        IP_LOG_WARN("IP_TaskEntry:ERROR: No Msg!");
        return;
    }

    /* 获取消息头指针*/
    pHeader = (PS_MSG_HEADER_STRU *) pstMsg;

    /* 打印收到的消息内容 */
    IP_PrintIpRevMsg(pHeader,IP_GET_IP_PRINT_BUF());

    /* 获取消息ID */
    IP_GetNameFromMsg(ulMsgId, (VOS_VOID*)pHeader);

    /* 查找这条消息是否需要IPM模块处理 */
    pActFun = IP_LayerMsgFindFun(IP_ROUTER_IPM, ulMsgId);
    if (IP_NULL_PTR != pActFun)
    {
        enRslt = pActFun((VOS_VOID*)pstMsg);
        IP_LOG1_NORM("IP_TaskEntry:The result of IPM function : " , enRslt);
    }

    /* 查找这条消息是否需要DHCPV4 SERVER模块处理 */
    pActFun = IP_LayerMsgFindFun(IP_ROUTER_DHCPV4SERVER, ulMsgId);
    if (IP_NULL_PTR != pActFun)
    {
        enRslt = pActFun((VOS_VOID*)pstMsg);
        IP_LOG1_NORM("IP_TaskEntry:The result of DHCPV4SERVER function : " , enRslt);
    }
}
/*lint +e961*/
/*lint +e960*/

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

