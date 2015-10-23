

#ifndef __IPIPMREG_H__
#define __IPIPMREG_H__

/*****************************************************************************
  1 Include Headfile
*****************************************************************************/
#include "vos.h"
#include "IpComm.h"


/*****************************************************************************
  1.1 Cplusplus Announce
*****************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
/*****************************************************************************
  #pragma pack(*)    设置字节对齐方式
*****************************************************************************/
#if (VOS_OS_VER != VOS_WIN32)
#pragma pack(4)
#else
#pragma pack(push, 4)
#endif


/*****************************************************************************
  2 Macro
*****************************************************************************/
#define IP_MAX_USER_NUM                 (8)

#define IP_MAX_ROUTER_TER_NUM           (16)

#define IP_VOS_HEADER_LEN               (20)

/* 第一字节01表示层间消息头，后三字节表示层间消息的后24位 */
#define IP_ROUTER_LAYERMSG_HEADER       (0x01000000)

/* 第一字节02表示定时器消息头，后三字节表示定时器消息所属的模块 */
#define IP_ROUTER_TIMERMSG_HEADER       (0x02000000)


/* 获取路由表 */
#define IP_GetRouterTable()             (g_stIpRouterSubTable)

/* 获取路由表记录个数 */
#define IP_GetRouterTableItemNum()\
            (sizeof(g_stIpRouterSubTable) / sizeof(IP_ROUTER_SUB_ITEM_STRU))


#define IP_ROUTER_ITEM_REG(ulSubId, ulMsgId, pfFunMsgEntry)\
        {\
            ulSubId,\
            ulMsgId,\
            IP_VOS_HEADER_LEN,\
            pfFunMsgEntry\
        }

#define IP_ROUTER_LAYERMSG_ET(ulMsgId)\
           ((ulMsgId & 0x00ffffff) | IP_ROUTER_LAYERMSG_HEADER)

#define IP_ROUTER_TIMERMSG_ET(ulModuleId)\
           ((ulModuleId & 0x00ffffff) | IP_ROUTER_TIMERMSG_HEADER)


#define IP_ROUTER_LAYERMSG_REG(ulSubId, ulMsgId, pfFunMsgEntry)\
           IP_ROUTER_ITEM_REG(ulSubId, IP_ROUTER_LAYERMSG_ET(ulMsgId), pfFunMsgEntry)

#define IP_ROUTER_TIMERMSG_REG(ulSubId, ulMsgId, pfFunMsgEntry)\
           IP_ROUTER_ITEM_REG(ulSubId, IP_ROUTER_TIMERMSG_ET(ulMsgId), pfFunMsgEntry)


/*****************************************************************************
  3 Massage Declare
*****************************************************************************/


/*****************************************************************************
  4 Enum
*****************************************************************************/
/*****************************************************************************
 结构名    : IP_WHE_NEED_SER_ENUM
 结构说明  : 用户是否需要某种服务
 *****************************************************************************/
enum    IP_WHE_NEED_SER_ENUM
{
    IP_SERVICE_NEED,
    IP_SERVICE_NOT_NEED,
    IP_SERVICE_BUTT
};
typedef VOS_UINT32  IP_WHE_NEED_SER_ENUM_UINT32;



/*****************************************************************************
 结构名    : IP_SERVICE_TYPE_ENUM
 结构说明  : IP模块对用户提供的服务类型
 *****************************************************************************/
enum    IP_SERVICE_TYPE_ENUM
{
    IP_SER_TYPE_DHCPV4_STATUS_RPT,       /* DHCPV4的状态报告 */
    IP_SER_TYPE_BUTT
};
typedef VOS_UINT32  IP_SERVICE_TYPE_ENUM_UINT32;


/*****************************************************************************
  5 STRUCT
*****************************************************************************/

/*****************************************************************************
 结构名    : IP_USER_SUB_SERVICE_STRU
 结构说明  : IP模块的用户注册、撤销某项服务的入口结构，
             每种服务类型签约、撤销一次
 *****************************************************************************/
typedef struct
{
    /* 签约用户ID, ESM签约时，此ID填 EPSID */
    VOS_UINT32                          ulSubId;

    VOS_UINT32                          ulSubPid;

    /* 某项服务 */
    IP_SERVICE_TYPE_ENUM_UINT32         ulServiceType;

}IP_USER_SUB_SERVICE_STRU;



/*****************************************************************************
 结构名    : IP_USER_SUB_ITEM_STRU
 结构说明  : IP模块的用户注册的信息，每种服务类型签约一次
 *****************************************************************************/
typedef struct
{
    /* 签约用户ID, ESM签约时，此ID填 EPSID */
    VOS_UINT32                          ulSubId;

    VOS_UINT32                          ulSubPid;

    /* 是否签约使用的DHCPV4状态报告功能 */
    IP_WHE_NEED_SER_ENUM_UINT32         ulWheNeedDhcpv4StaRpt;

}IP_USER_SUB_ITEM_STRU;

/*****************************************************************************
 结构名    :IP_ROUTER_TERMINAL_ENUM
 结构说明  :IP模块枚举
 *****************************************************************************/
enum    IP_ROUTER_TERMINAL_ENUM
{
    IP_ROUTER_IPM                       = 1,
    IP_ROUTER_DHCPV4CLIENT              = 2,
    IP_ROUTER_DHCPV4SERVER              = 3,
    IP_ROUTER_BUTT
};
typedef VOS_UINT32  IP_ROUTER_TERMINAL_ENUM_UINT32;


/*****************************************************************************
 结构名    : IP_ROUTER_MEG_ENTRY_FUN
 结构说明  : 目的路由模块注册的消息包入口函数
 *****************************************************************************/
typedef IP_ERR_ENUM_UINT32 (*IP_ROUTER_MEG_ENTRY_FUN)
(
    VOS_VOID *  /* 消息指针，消息结构与子模块协商 */
);

/*****************************************************************************
 结构名    : IP_ROUTER_SUB_ITEM_STRU
 结构说明  : IP模块转发包的目的路由模块的签约信息
 *****************************************************************************/
typedef struct
{

    /* 签约ROUTER ID */
    IP_ROUTER_TERMINAL_ENUM_UINT32      ulSubId;

    /* 发给此函数的包的标签，
       例如: DOPRA的MsgId: 此处填 ESM_IP_CONFIG_PARA_REQ */
    VOS_UINT32                          ulMsgId;

    /* 此签约函数的包的标签在消息中的字节索引, 消息头字节位置索引为 0
       例如: DOPRA层间消息的MSGID的索引位置: 此处填 20 */
    VOS_UINT32                          ulTagOffset;


    /* 目的路由模块提供的消息入口函数 */
    IP_ROUTER_MEG_ENTRY_FUN             pfFunMsgEntry;

}IP_ROUTER_SUB_ITEM_STRU;



/*****************************************************************************
  6 UNION
*****************************************************************************/


/*****************************************************************************
  7 Extern Global Variable
*****************************************************************************/



/*****************************************************************************
  8 Fuction Extern
*****************************************************************************/
extern IP_ROUTER_MEG_ENTRY_FUN IP_LayerMsgFindFun
(
    IP_ROUTER_TERMINAL_ENUM_UINT32      ulSubId,
    VOS_UINT32                          ulTag
);

/*****************************************************************************
  9 OTHERS
*****************************************************************************/










#if (VOS_OS_VER != VOS_WIN32)
#pragma pack()
#else
#pragma pack(pop)
#endif




#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of IpIpmReg.h */

