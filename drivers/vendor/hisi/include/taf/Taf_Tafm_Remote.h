#ifndef __TAF_TAFM_REMOTE_H__
#define __TAF_TAFM_REMOTE_H__


#ifdef __cplusplus
extern "C" {
#endif
#pragma pack(4)
/*========================================================*/

#include "TafClientApi.h"

/*宏定义开始*/

/*定义Client的广播值*/
#define TAF_CLIENTID_BROADCAST          (0xFFFF)
#define MN_CLIENTID_INVALID             (0xFFFE)


/*CS或者PS最大支持的RAB数目*/
#define  TAF_MAX_RABID                15


#define TAF_MSG_HEADER_LEN  6

#ifndef TAF_SUCCESS
#define TAF_SUCCESS    TAF_ERR_NO_ERROR     /*函数执行成功*/
#endif
#ifndef TAF_FAILURE
#define TAF_FAILURE    TAF_ERR_ERROR        /*函数执行失败*/
#endif

#define PS_INIT_NOT_START  0
#define PS_INIT_START      1
#define PS_INIT_FINISH     2

/*宏定义结束*/

/*远端API与TAFM进行通信时使用的消息类型*/


typedef enum
{
    TAF_MSG_MMA_MS_CLASS,         /* 业务模式，扩展命令*/

    TAF_MSG_MMA_FUN_QUERY,              /* 新增，手机操作功能设置*/
    TAF_MSG_MMA_REG_PLMN,         /* 当前已注册PLMN*/
    TAF_MSG_MMA_HANDLE_PREF_PLMN_INFO, /* 查询PREF PLMN信息*/
    TAF_MSG_MMA_SET_QUICKSTART,   /*快速开机 */
    TAF_MSG_MMA_SET_AUTOATTACH,
    TAF_MSG_MMA_SET_LOADDEFAULT,    /* Load Default Value of NV Item */
    TAF_MSG_MMA_USIM_COMMAND,
    TAF_MSG_MMA_USIM_RESTRICTED_ACCESS, /*+CRSM*/
    TAF_MSG_MMA_SET_IMEI,
    TAF_MSG_MMA_FPLMN_HANDLE,
    TAF_MSG_MMA_SYSCFG_QUERY_REQ,

    TAF_MSG_MMA_CRPN_HANDLE,            /* AT命令AT^CRPN查询处理 */
    TAF_MSG_MM_TEST_HANDLE,             /* AT命令AT^CMM设置命令处理 */

    TAF_MSG_PARA_READ,            /* 通信参数查询*/

    /*电话管理*/
    TAF_MSG_MMA_PLMN_LIST,        /* 可用PLMN搜索，扩展命令*/

    TAF_MSG_MMA_PLMN_RESEL,       /* 自动、人工网络重选*/
    TAF_MSG_MMA_PLMN_USER_SEL,    /* 指定PLMN选择*/
    TAF_MSG_MMA_OP_PIN_REQ,       /* PIN操作请求*/
    TAF_MSG_MMA_ATTACH,           /* 启动附着过程*/
    TAF_MSG_MMA_DETACH,           /* 启动去附着过程*/
    TAF_MSG_MMA_PARA_QUERY,       /* 查询功能类型*/
    TAF_MSG_MMA_INFO_REPORT,      /* 启动主动上报状态信息*/
    TAF_MSG_MMA_ME_PERSONAL_REQ,   /* 锁机操作请求 */

    TAF_MSG_MMA_GET_CURRENT_ATTACH_STATUS,      /*请求获取CS和PS的注册状态*/
    TAF_MSG_MMA_MT_POWER_DOWN,                  /* 请求MT下电 */
    TAF_MSG_MMA_SET_PREF_PLMN_TYPE,             /* 设置优先网络列表文件 */
    TAF_MSG_MMA_SET_PREF_PLMN,                  /* 设置PREF PLMN信息*/
    TAF_MSG_MMA_TEST_PREF_PLMN,                 /* 测试PREF PLMN信息*/
    TAF_MSG_MMA_TEST_SYSCFG,                    /* SYSCFG测试命令消息*/
    /* Deleted SSA消息 */

    TAF_MSG_MMA_USIM_INFO,
    TAF_MSG_MMA_CPNN_INFO,

    TAF_MSG_MMA_SET_PIN,

    TAF_MSG_MMA_CIPHER_INFO,
    TAF_MSG_MMA_LOCATION_INFO,

    TAF_MSG_MMA_PLMN_LIST_ABORT_REQ,  /* PLMN LIST打断 */

    TAF_MSG_MMA_AC_INFO_QUERY_REQ, /* 小区接入禁止信息上报 */


    TAF_MSG_MMA_CERSSI_INFO_QUERY_REQ,     /* 小区信号质量查询 */

    TAF_MSG_MMA_COPN_INFO_QUERY_REQ,

    TAF_MSG_MMA_SIMINSERT_IND,   /*SIM卡插入或移除*/

    TAF_MSG_MMA_EOPLMN_SET_REQ,
    TAF_MSG_MMA_EOPLMN_QUERY_REQ,

    TAF_MSG_MMA_NET_SCAN_REQ,
    TAF_MSG_MMA_ABORT_NET_SCAN_REQ,

    TAF_MSG_MMA_CPOL_INFO_QUERY_REQ,
    TAF_MSG_BUTT
}TAF_MSG_TYPE;
typedef VOS_UINT16   TAF_MSG_TYPE_ENUM_U16;


/*TAF参数管理类子消息类型定义*/
typedef enum
{
    TAF_SUB_MSG_PARA_SET,     /*通信参数设置*/
    TAF_SUB_MSG_PARA_READ,    /*通信参数查询*/
    TAF_SUB_MSG_BUTT
}TAF_PRIM_MSG_TYPE;


/*全局变量定义开始*/
/*远端API所需全局变量定义*/
/*APP/AT回调函数记录表*/


/*全局变量定义结束*/

/*API辅助宏定义*/
#define TAF_ENCODE_MSG_HEADER(ptr, MsgType, ClientId, Id, IeMask)  \
                         *ptr++ = MsgType;                         \
                         *ptr++ = (VOS_UINT8)((ClientId >> 8) & 0xFF); \
                         *ptr++ = (VOS_UINT8)(ClientId & 0xFF);        \
                         *ptr++ = Id;                              \
                         *ptr++ = (VOS_UINT8)((IeMask >> 8) & 0xFF);   \
                         *ptr++ = (VOS_UINT8)(IeMask & 0xFF)


/*TAF是否已经在远端注册过MUX回调函数*/
#define TAF_REG_MUX_CALLBACK_NO    0   /*未注册过MUX回调函数*/
#define TAF_REG_MUX_CALLBACK_YES   1   /*已注册过MUX回调函数*/

VOS_VOID   Taf_EventReportProc(VOS_UINT16 usMuxId, VOS_UINT8 *pData, VOS_UINT16 usLen, VOS_UINT8 ucRegTabIndex);


#if ((VOS_OS_VER == VOS_WIN32) || (VOS_OS_VER == VOS_NUCLEUS))
#pragma pack()
#else
#pragma pack(0)
#endif

/*========================================================*/
#ifdef __cplusplus
}
#endif
/******************************************************************************/

/*============================================================================*/
#endif          /* __TAF_REMOTE_H__ */
