/******************************************************************************

   Copyright(C)2008,Hisilicon Co. LTD.

 ******************************************************************************
  File Name       : IpIpmReg.c
  Description     : IP模块的注册表
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
#include  "IpIpmReg.h"
#include  "NasIpInterface.h"
#include  "IpIpmMsgProc.h"
#include  "IpIpmGlobal.h"
/*#include  "IpDhcpv4Server.h"*/

/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_IPIPMREG_C
/*lint +e767*/


/*****************************************************************************
  2 Declare the Global Variable
*****************************************************************************/
/* 路由目的模块注册表,一种消息标签注册一次 */
IP_ROUTER_SUB_ITEM_STRU        g_stIpRouterSubTable[]=
{
    /* IPM注册处理的层间消息 */
    IP_ROUTER_LAYERMSG_REG(         IP_ROUTER_IPM,
                                    ID_ESM_IP_START_REQ,
                                    IP_IPM_MsgEntry),

    IP_ROUTER_LAYERMSG_REG(         IP_ROUTER_IPM,
                                    ID_ESM_IP_STOP_REQ,
                                    IP_IPM_MsgEntry),

    IP_ROUTER_LAYERMSG_REG(         IP_ROUTER_IPM,
                                    ID_ESM_IP_REL_IND,
                                    IP_IPM_MsgEntry),

#if 0
    /* DHCPV4 SERVER注册处理的层间消息 */
    IP_ROUTER_LAYERMSG_REG(         IP_ROUTER_DHCPV4SERVER,
                                    ID_IP_RABM_DATA_IND,
                                    IP_DHCPV4SERVER_TaskEntry),

    IP_ROUTER_LAYERMSG_REG(         IP_ROUTER_DHCPV4SERVER,
                                    ID_ESM_IP_NW_PARA_IND,
                                    IP_DHCPV4SERVER_TaskEntry),

    IP_ROUTER_LAYERMSG_REG(         IP_ROUTER_DHCPV4SERVER,
                                    ID_ESM_IP_PDP_RELEASE_IND,
                                    IP_DHCPV4SERVER_TaskEntry),

    IP_ROUTER_LAYERMSG_REG(         IP_ROUTER_DHCPV4SERVER,
                                    ID_ESM_IP_REL_IND,
                                    IP_DHCPV4SERVER_TaskEntry),
#endif
};



/*****************************************************************************
  3 Function
*****************************************************************************/
/*****************************************************************************
 Function Name   : IP_LayerMsgFindFun
 Description     : 对收到的层间消息包，寻找到入口函数
 Input           : ulSubId----------------------模块ID
                   ulTag------------------------消息标识
 Output          : None
 Return          : IP_ROUTER_MEG_ENTRY_FUN

 History         :
    1.lihong00150010      2010-11-23  Draft Enact

*****************************************************************************/
IP_ROUTER_MEG_ENTRY_FUN IP_LayerMsgFindFun
(
    IP_ROUTER_TERMINAL_ENUM_UINT32      ulSubId,
    VOS_UINT32                          ulTag
)
{
    VOS_UINT32                          ulRouterTableItemNum    = IP_GetRouterTableItemNum();
    VOS_UINT32                          ulCnt                   = IP_NULL;
    IP_ROUTER_SUB_ITEM_STRU            *pstRouterTable          = IP_GetRouterTable();

    /* 遍历路由表，查找模块号为ulSubId，消息表示为ulTag的记录 */
    for (ulCnt = IP_NULL; ulCnt < ulRouterTableItemNum; ulCnt++)
    {
        if ((pstRouterTable[ulCnt].ulSubId == ulSubId)
            && (pstRouterTable[ulCnt].ulMsgId == ulTag))
        {
            return pstRouterTable[ulCnt].pfFunMsgEntry;
        }
    }

    return IP_NULL_PTR;
}


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

