

#ifndef _NASLMMPUBMOM_H
#define _NASLMMPUBMOM_H


/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include    "vos.h"
#include    "OmCommon.h"
#include    "NasEmmPubUGlobal.h"
#include    "NasEmmPubUMain.h"

#include    "NasEmmTauInterface.h"
#include    "NasEmmSerInterface.h"
#include    "LnasFtmInterface.h"
#include    "LnasErrlogInterface.h"


/*#include	"IpDhcpv4Server.h"*/
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
  2 macro
*****************************************************************************/

#define NAS_EMM_AIR_DATA_ARRAY_LEN      4

#define NAS_EMM_TP_CMD_HEADER           0x00000000

#define NAS_EMM_EVEN_IMSI               0x01
#define NAS_EMM_ODD_IMSI                0x09
#define MAS_EMM_IMSI_PAD_LEN            0x02

#define NAS_EMM_LOW_4_BIT               0x0F
#define NAS_EMM_HIGH_4_BIT              0xF0

#define NAS_EMM_MNC_LEN2                2
#define NAS_EMM_MNC_LEN3                3
#define NAS_EMM_NV_READ_FAIL            0

#define NAS_EMM_DHCP_SERVER_BUFFER_LEN  400

/* niuxiufan DT begin */
#define NAS_DT_RPT_CTRL_MAX_ITEM_NUM         10
/* niuxiufan DT end */

#define NAS_SAVE_RECEIVE_MSG_INFO_NUM       16

/*LTE CS*/
#define NAS_LMM_LTE_CS_SERVICE_NOT_SUPPORT    0
#define NAS_LMM_LTE_CS_SERVICE_SUPPORT        1

/* niuxiufan DT begin */
#define NAS_EMM_GetNasDtRptCtrlItem(ulIndex)   (&(gastNasDtRptCtrlTbl[ulIndex]))

/* niuxiufan DT end */

extern VOS_UINT32                  g_NasEmmOmInfoIndFlag;


#define NAS_EMM_GetDrxCycleLenChangeFlag()   (g_ulDrxCycleLenChange)
#define NAS_EMM_SetDrxCycleLenChangeFlag(DrxCycleLenChangeFlag)\
                             ((g_ulDrxCycleLenChange) = (DrxCycleLenChangeFlag))


#define EMM_COMP_OMT_MSG_HEADER(pMsg,MsgLenthNoHeader)\
        {\
            (pMsg)->ulSenderCpuId       = VOS_LOCAL_CPUID;\
            (pMsg)->ulSenderPid         = PS_PID_MM;\
            (pMsg)->ulReceiverCpuId     = VOS_LOCAL_CPUID;\
            (pMsg)->ulReceiverPid       = PS_PID_OM;\
            (pMsg)->ulLength            = (MsgLenthNoHeader);\
        }

#define EMM_COMP_APP_MSG_VOS_HEADER(pDestMsg,MsgLenthNoHeader,pSourMsg)\
        {\
            (pDestMsg)->ulSenderCpuId   = VOS_LOCAL_CPUID;\
            (pDestMsg)->ulSenderPid     = PS_PID_MM;\
            (pDestMsg)->ulReceiverCpuId = VOS_LOCAL_CPUID;\
            (pDestMsg)->ulReceiverPid   = PS_PID_APP;\
            (pDestMsg)->ulLength        = (MsgLenthNoHeader);\
            (pDestMsg)->usOriginalId    = (pSourMsg)->usTerminalId;\
            (pDestMsg)->usTerminalId    = (pSourMsg)->usOriginalId;\
            (pDestMsg)->ulTimeStamp     = (pSourMsg)->ulTimeStamp;\
            (pDestMsg)->ulSN            = (pSourMsg)->ulSN;\
        }

#define EMM_COMP_APP_DT_IND_MSG_HEADER(pMsg,MsgLenthNoHeader)\
        {\
            (pMsg)->ulSenderCpuId       = VOS_LOCAL_CPUID;\
            (pMsg)->ulSenderPid         = PS_PID_MM;\
            (pMsg)->ulReceiverCpuId     = VOS_LOCAL_CPUID;\
            (pMsg)->ulReceiverPid       = L3_MSP_PID_DT_IND;\
            (pMsg)->ulLength            = (MsgLenthNoHeader);\
        }

#define EMM_COMP_APP_DT_CNF_MSG_VOS_HEADER(pDestMsg,MsgLenthNoHeader,pSourMsg)\
        {\
            (pDestMsg)->ulSenderCpuId   = VOS_LOCAL_CPUID;\
            (pDestMsg)->ulSenderPid     = PS_PID_MM;\
            (pDestMsg)->ulReceiverCpuId = VOS_LOCAL_CPUID;\
            (pDestMsg)->ulReceiverPid   = L3_MSP_PID_DT;\
            (pDestMsg)->ulLength        = (MsgLenthNoHeader);\
            (pDestMsg)->usOriginalId    = (pSourMsg)->usTerminalId;\
            (pDestMsg)->usTerminalId    = (pSourMsg)->usOriginalId;\
            (pDestMsg)->ulTimeStamp     = (pSourMsg)->ulTimeStamp;\
            (pDestMsg)->ulSN            = (pSourMsg)->ulSN;\
        }

/*空口消息和关键事件填充APP消息头*/
#define EMM_COMP_OMT_APP_MSG_HEADER(pMsg)\
        {\
            (pMsg)->usOriginalId  = (UE_MODULE_MM_ID);\
            (pMsg)->usTerminalId  = (UE_APP_SUBSYS_ID);\
        }

#define EMM_COMP_MM_APP_KEY_AIR_MSG_HEADER(pMsg)\
        {\
            (pMsg)->usOriginalId        = (UE_MODULE_MM_ID);\
            (pMsg)->usTerminalId        = (UE_APP_SUBSYS_ID);\
        }

#if 0
/*普通命令和透明命令填充APP消息头，ulTimeStamp的获取可能需要改正*/
#define EMM_COMP_OMT_TP_APP_MSG_HEADER(pDestMsg, pSourMsg)\
        {\
            (pDestMsg)->usOriginalId    = (pSourMsg)->usTerminalId;\
            (pDestMsg)->usTerminalId    = (pSourMsg)->usOriginalId;\
            (pDestMsg)->ulTimeStamp     = (pSourMsg)->ulTimeStamp;\
            (pDestMsg)->ulSN            = (pSourMsg)->ulSN;\
        }

#define EMM_COMP_OMT_SET_CNF_MSG(pEmmOmSetCnfMsg,ID_EMM_OM_OT_CNF,OM_EMM_MAINTAIN_RST)\
        {\
            (pEmmOmSetCnfMsg)->ulMsgId  = (ID_EMM_OM_OT_CNF);\
            (pEmmOmSetCnfMsg)->ucRst    = (OM_EMM_MAINTAIN_RST);\
        }

#define EMM_COMP_MM_APP_MSG_HEADER(pDestMsg, pSourMsg)\
        {\
            (pDestMsg)->usOriginalId    = (pSourMsg)->usTerminalId;\
            (pDestMsg)->usTerminalId    = (pSourMsg)->usOriginalId;\
            (pDestMsg)->ulTimeStamp     = (pSourMsg)->ulTimeStamp;\
            (pDestMsg)->ulSN            = (pSourMsg)->ulSN;\
        }

#define EMM_COMP_APP_SET_CNF_MSG(pEmmOmSetCnfMsg,pSourMsg,OM_EMM_MAINTAIN_RST)\
        {\
            (pEmmOmSetCnfMsg)->ulMsgId  = (pSourMsg)->ulMsgId;\
            (pEmmOmSetCnfMsg)->ulRslt    = (OM_EMM_MAINTAIN_RST);\
            (pEmmOmSetCnfMsg)->ulOpId   = (pSourMsg)->ulOpId;\
        }
#endif

#define NAS_EMM_COMP_MM_2_APP_MSGID(pstEmmAppMsg, ID_EMM_APP_MSGID) \
            (pstEmmAppMsg->ulMsgId      = (ID_EMM_APP_MSGID))

#define EMM_TP_CMD_TYPE(enTpMsgType)    (enTpMsgType)


#define EMM_MSG_LENTH_NO_HEADER(EMM_XX_MSG_STRU)\
            (sizeof(EMM_XX_MSG_STRU)    - NAS_EMM_LEN_VOS_MSG_HEADER)


#define NAS_LMM_MEM_CMP(dest, src, count) PS_MEM_CMP((dest), (src), (count))


/*OMA头*/
#define OMA_HEADER        VOS_UINT16    usHeader;\
                          VOS_UINT16    usMsgLen;

#ifndef APP_MSG_HEADER

#define APP_MSG_HEADER    VOS_UINT16    usOriginalId;\
                          VOS_UINT16    usTerminalId;\
                          VOS_UINT32    ulTimeStamp; \
                          VOS_UINT32    ulSN;


#endif

/* 计算TP_GINFO_CNF_MSG的DOPRA申请的消息的部分长度，
该长度不包括u 中具体的全局变量的长度 */
#define NAS_EMM_TP_GINFO_MSG_LENGTH_EXCEPT_IE(pstTpGInfoCnfMsg) \
                ((sizeof(APP_MM_TP_G_INFO_CNF_STRU)\
                 - NAS_EMM_LEN_VOS_MSG_HEADER)\
                 - (sizeof(pstTpGInfoCnfMsg->u)))



/* xiongxianghui00253310 modify for ftmerrlog begin */
#define NAS_EMM_GetFtmInfoManageAddr()      (&(g_astEmmFtmInfoManage))
#define NAS_EMM_GetEmmInfoAddr()            (&(g_astEmmInfo))
#define NAS_EMM_GetFtmInfoActionFlag()      (NAS_EMM_GetFtmInfoManageAddr()->ulFtmActionFlag)
#define NAS_EMM_GetFtmInfoMsgSN()           (NAS_EMM_GetFtmInfoManageAddr()->ulMsgSN)
#define NAS_EMM_GetFtmInfoCnRej()           (NAS_EMM_GetFtmInfoManageAddr()->enEmmCnRejCause)
#define NAS_EMM_SetFtmOmManageFtmActionFlag(Flag)  (NAS_EMM_GetFtmInfoActionFlag() = Flag)

#define NAS_EMM_GetErrlogManageAddr()       (&(g_astEmmErrlogInfoManage))
#define NAS_EMM_GetErrlogActionFlag()       (NAS_EMM_GetErrlogManageAddr()->ulActionFlag)
#define NAS_EMM_SetErrlogActionFlag(Flag)   (NAS_EMM_GetErrlogActionFlag() = Flag)
#define NAS_EMM_GetErrlogMsgSN()            (NAS_EMM_GetErrlogManageAddr()->ulMsgSN)
#define NAS_EMM_GetErrlogAlmLevel()         (NAS_EMM_GetErrlogManageAddr()->usALMLevel)
#define NAS_EMM_SetErrlogAlmLevel(usLevel)  (NAS_EMM_GetErrlogAlmLevel() = usLevel)
#define NAS_EMM_GetErrlogAlmType()          (NAS_EMM_GetErrlogManageAddr()->usALMType)
#define NAS_EMM_GetErrlogAlmLowSlice()      (NAS_EMM_GetErrlogManageAddr()->ulAlmLowSlice)
#define NAS_EMM_GetErrlogAlmHighSlice()     (NAS_EMM_GetErrlogManageAddr()->ulAlmHighSlice)
#define NAS_EMM_GetErrlogAmount()           (NAS_EMM_GetErrlogManageAddr()->ulErrLogAmount)
#define NAS_EMM_GetErrlogNextNullPos()      (NAS_EMM_GetErrlogManageAddr()->ulNextNullPos)
#define NAS_EMM_GetErrlogInfo(ulIndex)      (NAS_EMM_GetErrlogManageAddr()->stLmmErrInfoDetail[ulIndex])
#define NAS_EMM_GetErrlogMsgQueueAddr()     (&(NAS_EMM_GetErrlogManageAddr()->stMsgQueueInfo))

#define NAS_EMM_ERRLOG_MAX_NUM              (4)

#define NAS_LMM_COMP_OM_MSG_HEADER(pMsg,MsgLenthNoHeader)\
        {\
            (pMsg)->ulSenderCpuId       = VOS_LOCAL_CPUID;\
            (pMsg)->ulSenderPid         = PS_PID_MM;\
            (pMsg)->ulReceiverCpuId     = VOS_LOCAL_CPUID;\
            (pMsg)->ulReceiverPid       = ACPU_PID_OM;\
            (pMsg)->ulLength            = (MsgLenthNoHeader);\
        }


/* xiongxianghui00253310 modify for ftmerrlog end   */


/*****************************************************************************
  3 Massage Declare
*****************************************************************************/



/*****************************************************************************
  4 Enum
*****************************************************************************/

enum    NAS_EMM_FTM_ACTION_FLAG_ENUM
{
    NAS_EMM_FTM_ACTION_FLAG_CLOSE = 0,
    NAS_EMM_FTM_ACTION_FLAG_OPEN = 1,
    NAS_EMM_FTM_ACTION_FLAG_BUTT
};
typedef VOS_UINT32  NAS_EMM_FTM_ACTION_FLAG_ENUM_UINT32;

enum NAS_EMM_ERRLOG_ACTION_FLAG_ENUM
{
    NAS_EMM_ERRLOG_ACTION_FLAG_CLOSE  = 0,
    NAS_EMM_ERRLOG_ACTION_FLAG_OPEN   = 1,
    NAS_EMM_ERRLOG_ACTION_FLAG_BUTT
};
typedef VOS_UINT32  NAS_EMM_ERRLOG_ACTION_FLAG_ENUM_UINT32;

enum NAS_EMM_ERRLOG_LEVEL_ENUM
{
    NAS_EMM_ERRLOG_LEVEL_CRITICAL   = 1,/*代表紧急*/
    NAS_EMM_ERRLOG_LEVEL_MAJOR      = 2,/*代表重要*/
    NAS_EMM_ERRLOG_LEVEL_MINOR      = 3,/*代表次要*/
    NAS_EMM_ERRLOG_LEVEL_WARING     = 4,/*代表提示，*/
    NAS_EMM_ERRLOG_LEVEL_BUTT
};
typedef VOS_UINT16  NAS_EMM_ERRLOG_LEVEL_ENUM_UINT16;

/* ERROR LOG故障&警告类型*/
enum NAS_EMM_ERRLOG_TYPE_ENUM
{
    NAS_EMM_ERRLOG_TYPE_COMMUNICATION       = 0x00, /* 通信     */
    NAS_EMM_ERRLOG_TYPE_SERVING_QUALITY     = 0x01, /* 业务质量 */
    NAS_EMM_ERRLOG_TYPE_PROCESS_ERROR       = 0x02, /* 处理出错 */
    NAS_EMM_ERRLOG_TYPE_EQUIPMENT_TROUBLE   = 0x03, /* 设备故障 */
    NAS_EMM_ERRLOG_TYPE_ENVIRONMENT_TROUBLE = 0x04, /* 环境故障 */
};
typedef VOS_UINT16 NAS_EMM_ERRLOG_TYPE_ENUM_UINT16;


enum    NAS_EMM_OMT_AIR_MSG_UP_DOWN_ENUM
{
    NAS_EMM_OMT_AIR_MSG_UP              = 0x00,             /* UPWARD */
    NAS_EMM_OMT_AIR_MSG_DOWN            = 0x01,             /* DOWNWARD */

    NAS_EMM_OMT_AIR_MSG_BUTT
};
typedef VOS_UINT8   NAS_LMM_OM_AIR_MSG_UP_DOWN_ENUM_UINT8;

enum    NAS_EMM_OMT_AIR_MSG_ID_ENUM
{
    NAS_EMM_ATTACH_REQ                  = MM_ATTACH_REQ,
    NAS_EMM_ATTACH_ACP                  = MM_ATTACH_ACP,
    NAS_EMM_ATTACH_CMP                  = MM_ATTACH_CMP,
    NAS_EMM_ATTACH_REJ                  = MM_ATTACH_REJ,

    NAS_EMM_DETACH_REQ_MT               = MM_DETACH_REQ_MT,
    NAS_EMM_DETACH_ACP_MT               = MM_DETACH_ACP_MT,
    NAS_EMM_DETACH_REQ_MO               = MM_DETACH_REQ_MO,
    NAS_EMM_DETACH_ACP_MO               = MM_DETACH_ACP_MO,

    NAS_EMM_TAU_REQ                     = MM_TAU_REQ,
    NAS_EMM_TAU_ACP                     = MM_TAU_ACP,
    NAS_EMM_TAU_CMP                     = MM_TAU_CMP,
    NAS_EMM_TAU_REJ                     = MM_TAU_REJ,

    NAS_EMM_SER_REQ                     = MM_SER_REQ,
    NAS_EMM_SER_REJ                     = MM_SER_REJ,
    NAS_EMM_SER_ACP                     = MM_SER_ACP,

    NAS_EMM_GUTI_CMD                    = MM_GUTI_CMD,
    NAS_EMM_GUTI_CMP                    = MM_GUTI_CMP,

    NAS_EMM_IDEN_REQ                    = MM_IDEN_REQ,
    NAS_EMM_IDEN_RSP                    = MM_IDEN_RSP,

    NAS_EMM_AUTH_REQ                    = MM_AUTH_REQ,
    NAS_EMM_AUTH_FAIL                   = MM_AUTH_FAIL,
    NAS_EMM_AUTH_RSP                    = MM_AUTH_RSP,
    NAS_EMM_AUTH_REJ                    = MM_AUTH_REJ,

    NAS_EMM_SECU_CMD                    = MM_SECU_CMD,
    NAS_EMM_SECU_CMP                    = MM_SECU_CMP,
    NAS_EMM_SECU_REJ                    = MM_SECU_REJ,

    NAS_EMM_STATUS                      = MM_EMM_STATUS,
    NAS_EMM_INFO                        = MM_EMM_INFO,

    NAS_EMM_DOWNLINK_NAS_TRANSPORT      = MM_EMM_DOWNLINK_NAS_TRANSPORT,
    NAS_EMM_UPLINK_NAS_TRANSPORT        = MM_EMM_UPLINK_NAS_TRANSPORT,

    NAS_EMM_EXTENDED_SER_REQ            = MM_EXTENDED_SER_REQ,                  /* 2012-02-24 */
    NAS_EMM_CS_SER_NOTIFICATION         = MM_CS_SER_NOTIFICATION,               /* 2012-02-24 */

    NAS_EMM_DOWNLINK_GENERIC_NAS_TRANSPORT = MM_EMM_DOWNLINK_GENERIC_NAS_TRANSPORT,
    NAS_EMM_UPLINK_GENERIC_NAS_TRANSPORT   = MM_EMM_UPLINK_GENERIC_NAS_TRANSPORT,
};
typedef VOS_UINT8   NAS_LMM_OM_AIR_MSG_ID_ENUM_UINT8;


enum    PS_EMM_OMT_KEY_EVENT_TYPE_ENUM
{   /* EMM关键事件取值范围100 - 199 */

    EMM_OMT_KE_PLMN_SPEC_SEARCH_START   = MM_KEY_EVENT_PLMN_SPEC_SEARCH_START,
    EMM_OMT_KE_PLMN_LIST_SEARCH_START   = MM_KEY_EVENT_PLMN_LIST_SEARCH_START,
    EMM_OMT_KE_PLMN_SEARCH_SUCC         = MM_KEY_EVENT_PLMN_SEARCH_SUCC,
    EMM_OMT_KE_PLMN_SEARCH_FAIL         = MM_KEY_EVENT_PLMN_SEARCH_SUCC,
    EMM_OMT_KE_START_AUTO_RESEL         = MM_KEY_EVENT_START_AUTO_RESEL,
    EMM_OMT_KE_START_MANUAL_RESEL       = MM_KEY_EVENT_START_MANUAL_RESEL,
    EMM_OMT_KE_PLMN_RESEL_CNF           = MM_KEY_EVENT_PLMN_RESEL_CNF,
    EMM_OMT_KE_COVERAGE_LOST            = MM_KEY_EVENT_COVERAGE_LOST,

    EMM_OMT_KE_ATTACH_REQ               = MM_KEY_EVENT_ATTACH_REQ,
    EMM_OMT_KE_ATTACH_ACP               = MM_KEY_EVENT_ATTACH_ACP,
    EMM_OMT_KE_ATTACH_CMP               = MM_KEY_EVENT_ATTACH_CMP,
    EMM_OMT_KE_ATTACH_REJ               = MM_KEY_EVENT_ATTACH_REJ,

    EMM_OMT_KE_DETACH_REQ_MT            = MM_KEY_EVENT_ATTACH_REJ,
    EMM_OMT_KE_DETACH_ACP_MT            = MM_KEY_EVENT_DETACH_REQ_MT,
    EMM_OMT_KE_DETACH_REQ_MO            = MM_KEY_EVENT_DETACH_REQ_MT,
    EMM_OMT_KE_DETACH_ACP_MO            = MM_KEY_EVENT_DETACH_ACP_MT,

    EMM_OMT_KE_TAU_REQ                  = MM_KEY_EVENT_TAU_REQ,
    EMM_OMT_KE_TAU_ACP                  = MM_KEY_EVENT_TAU_ACP,
    EMM_OMT_KE_TAU_CMP                  = MM_KEY_EVENT_TAU_CMP,
    EMM_OMT_KE_TAU_REJ                  = MM_KEY_EVENT_TAU_REJ,

    EMM_OMT_KE_SER_REQ                  = MM_KEY_EVENT_SER_REQ,
    EMM_OMT_KE_SER_REJ                  = MM_KEY_EVENT_SER_REJ,
    EMM_OMT_KE_SER_ACP                  = MM_KEY_EVENT_SER_ACP,

    EMM_OMT_KE_GUTI_CMD                 = MM_KEY_EVENT_GUTI_CMD,
    EMM_OMT_KE_GUTI_CMP                 = MM_KEY_EVENT_GUTI_CMP,

    EMM_OMT_KE_IDEN_REQ                 = MM_KEY_EVENT_IDEN_REQ,
    EMM_OMT_KE_IDEN_RSP                 = MM_KEY_EVENT_IDEN_RSP,

    EMM_OMT_KE_AUTH_REQ                 = MM_KEY_EVENT_AUTH_REQ,
    EMM_OMT_KE_AUTH_FAIL                = MM_KEY_EVENT_AUTH_FAIL,
    EMM_OMT_KE_AUTH_RSP                 = MM_KEY_EVENT_AUTH_RSP,
    EMM_OMT_KE_AUTH_REJ                 = MM_KEY_EVENT_AUTH_REJ,

    EMM_OMT_KE_SECU_CMD                 = MM_KEY_EVENT_SECU_CMD,
    EMM_OMT_KE_SECU_CMP                 = MM_KEY_EVENT_SECU_CMP,
    EMM_OMT_KE_SECU_REJ                 = MM_KEY_EVENT_SECU_REJ,

    EMM_OMT_KE_EMM_STATUS               = MM_KEY_EVENT_EMM_STATUS,
    EMM_OMT_KE_EMM_INFO                 = MM_KEY_EVENT_EMM_INFO,

    EMM_OMT_KE_EMM_DOWNLINK_NAS_TRANSPORT  = MM_KEY_EVENT_EMM_DOWNLINK_NAS_TRANSPORT,
    EMM_OMT_KE_EMM_UPLINK_NAS_TRANSPORT    = MM_KEY_EVENT_EMM_UPLINK_NAS_TRANSPORT,        /*2011-11-15*/

    EMM_OMT_KE_EXTENDED_SER_REQ         = MM_KEY_EVENT_EXTENDED_SER_REQ,        /*2012-02-24*/
    EMM_OMT_KE_CS_SER_NOTIFICATION      = MM_KEY_EVENT_CS_SER_NOTIFICATION,     /*2012-02-24*/

    EMM_OMT_KE_EMM_DOWNLINK_GENERIC_NAS_TRANSPORT = MM_KEY_EVENT_EMM_DOWNLINK_GENERIC_NAS_TRANSPORT,
    EMM_OMT_KE_EMM_UPLINK_GENERIC_NAS_TRANSPORT   = MM_KEY_EVENT_EMM_UPLINK_GENERIC_NAS_TRANSPORT,

    EMM_OMT_KE_TYPE_BUTT
};
typedef VOS_UINT8   PS_EMM_OMT_KEY_EVENT_TYPE_ENUM_UINT8;

/* OM_EMM_MSG_ID_ENUM_UINT32包括4部分:

-----------------------------------------------------------
  字节高位                                      字节低位
-----------------------------------------------------------
| BYTE1高4位 + 0000 |  BYTE2    |   BYTE3    |    BYTE4   |
-----------------------------------------------------------
0000：协议模块间消息|  源模块号 |  目的模块号| 消息号0-255
-----------------------------------------------------------
0001：L2 LOG        |           |            |
-----------------------------------------------------------
0010：空口消息      |           |            |
-----------------------------------------------------------
0011：普通命令      |           |            | OM_EMM_ORDER_TYPE_ENUM_UINT32
-----------------------------------------------------------
0100：实时监控命令  |           |            | OM_EMM_ORDER_TYPE_ENUM_UINT32
-----------------------------------------------------------
0101：关键事件      |           |            | 保留不用
-----------------------------------------------------------
*/
/*
typedef VOS_UINT32  OMT_EMM_MSG_ID_UINT32;
*/


/* OM和EMM间的维护类命令执行结果类型 */
enum    APP_EMM_MAINTAIN_RST_ENUM
{
    APP_EMM_MAINTAIN_RST_SUCC       = 0x00,
    APP_EMM_MAINTAIN_RST_FAIL,

    APP_EMM_MAINTAIN_RST_BUTT
};
typedef VOS_UINT32  APP_EMM_MAINTAIN_RST_ENUM_UINT32;



/*****************************************************************************
 枚举名    : NAS_EMM_TP_MSG_TYPE_ENUM
 枚举说明  : OM和EMM间的透明类命令类型
*****************************************************************************/

enum    NAS_EMM_TP_MSG_TYPE_ENUM
{
    NAS_EMM_TP_INQ_G_INFO_REQ           = 0x01,

    NAS_EMM_TP_MSG_TYPE_BUTT
};
typedef VOS_UINT32  NAS_EMM_TP_MSG_TYPE_ENUM_UINT32;

enum    APP_EMM_TP_CAUSE_ENUM
{
    APP_EMM_TP_CAUSE_TP_MSG_TYPE_UNKNOWN= 0x01,
    APP_EMM_TP_CAUSE_PARA_RANGE_ERR     = 0x02,
    APP_EMM_TP_CAUSE_BUTT               = 0xFF
};
typedef VOS_UINT32  APP_EMM_TP_CAUSE_ENUM_UINT32;


/*****************************************************************************
 枚举名    : EMM_OM_G_INFO_TYPE_ENUM
 枚举说明  : 查询的信息类型
*****************************************************************************/
enum    EMM_OM_G_INFO_TYPE_ENUM
{
    /*状态类*/
    EMM_OM_G_INFO_EMMUPSTATE           = 0x00000000,   /* */


    /*网络信息类*/
    EMM_OM_G_INFO_EPlMNLIST,
    EMM_OM_G_INFO_TAILIST,


    /*定时器*/
    EMM_OM_G_INFO_T3402,
    EMM_OM_G_INFO_T3412,
    EMM_OM_G_INFO_TAU_T3411,

    /*TAU*/
    EMM_TAU_ATTEMPT_COUNTER,
    EMM_TAU_START_CAUSE,

    /*SER*/
    EMM_SER_START_CAUSE,

    /* DHCPV4 SERVER */
    EMM_DHCPV4_SERVER_DEFT_EPSBID,
    EMM_DHCPV4_SERVER_TMP_STATUS,
    EMM_DHCPV4_SERVER_ALLOC_STATUS,
    EMM_DHCPV4_SERVER_NW_PARA,
    EMM_DHCPV4_SERVER_BUFFER,

    /* IPF */
    EMM_IPF_UL_FILTER_NUM,
    /*身份类*/
    /*安全类*/
    /*其他*/
    /*子模块控制参数: ATTACH, DETACH, TAU, SERIVCE, MMC*/
    /*leili modify for isr begin*/
    /*MML的TIN值信息*/
    EMM_TIN_TYPE,

    /*MML的承载信息*/
    EMM_PS_BEARER_INFO,
    /*leili modify for isr end*/
	/*L.V.R TAI*/
    EMM_LVR_TAI = 17,

    /*Reg Domain*/
    EMM_REG_DOMAIN = 18,
    EMM_OM_G_INFO_BUTT
};
typedef VOS_UINT32  EMM_OM_G_INFO_TYPE_ENUM_UINT32;

enum    NAS_EMM_DRX_CYCLE_LEN_CHANGE_ENUM
{
    NAS_EMM_DRX_CYCLE_LEN_CHANGED       = 0,
    NAS_EMM_DRX_CYCLE_LEN_NOT_CHANGED   = 1,
    NAS_EMM_DRX_CYCLE_LEN_CHANGE_BUTT
};
typedef VOS_UINT32   NAS_EMM_DRX_CYCLE_LEN_CHANGE_ENUM_UINT32;
/*leili modify for isr begin*/
enum NAS_LMM_TIN_TYPE_ENUM
{
    NAS_LMM_TIN_P_TMSI                  = 0x00000000,
    NAS_LMM_TIN_GUTI                    ,
    NAS_LMM_TIN_RAT_RELATED_TMSI        ,
    NAS_LMM_TIN_INVALID                 ,

    NAS_LMM_TIN_BUTT
};
typedef VOS_UINT32 NAS_LMM_TIN_TYPE_ENUM_UINT32;

/*leili modify for isr end*/
/*PC REPLAY MODIFY BY LEILI DELETE*/
/*****************************************************************************
  5 STRUCT
*****************************************************************************/
typedef struct
{
    NAS_LMM_MAIN_STATE_ENUM_UINT16       enMainState;
    NAS_LMM_SUB_STATE_ENUM_UINT16        enSubState;
    APP_EMM_MAIN_STATE_ENUM_UINT16      ulProtoclMainState;
    APP_EMM_SUB_STATE_ENUM_UINT16       ulProtoclSubState;
}NAS_EMM_PROTOCOL_STATE_MAP_STRU;


typedef struct
{
    VOS_MSG_HEADER                                  /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgId;    /*_H2ASN_Skip*/
}NAS_EMM_GENERAL_MSG_HEADER_STRU;




/*****************************************************************************
 结构名    : APP_MM_TP_G_INFO_REQ_STRU
 结构说明  : MM的透明命令的通用请求结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                  /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgId;    /*_H2ASN_Skip*/
    APP_MSG_HEADER

    NAS_EMM_TP_MSG_TYPE_ENUM_UINT32     ulMmTpCmdType;
    VOS_UINT8                           aucTransCmd[4];
}APP_MM_TP_CMD_REQ_STRU;
/*ID_APP_MM_TP_CMD_REQ*/


/*****************************************************************************
 结构名    : APP_MM_TP_CMD_CNF_STRU
 结构说明  : MM的透明命令的通用响应结构，若是查询G_INFO
                  的成功的CNF消息，见APP_MM_TP_G_INFO_CNF_STRU
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                  /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgId;    /*_H2ASN_Skip*/
    APP_MSG_HEADER

    APP_EMM_MAINTAIN_RST_ENUM_UINT32    ulRslt;
    APP_EMM_TP_CAUSE_ENUM_UINT32        ulCause;
}APP_MM_TP_CMD_CNF_STRU;




/*****************************************************************************
 结构名称  : APP_MM_TP_G_INFO_REQ_STRU
 结构说明  : OM查询MM层全局参数时，给MM的请求结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgId;            /*_H2ASN_Skip*/
    APP_MSG_HEADER


    NAS_EMM_TP_MSG_TYPE_ENUM_UINT32     enMmTpCmdType;      /*查询G_INFO命令*/
    EMM_OM_G_INFO_TYPE_ENUM_UINT32      enGInfoType;        /*待查询的数据类型*/
}APP_MM_TP_G_INFO_REQ_STRU;


/*****************************************************************************
 结构名    : APP_MM_TP_G_INFO_CNF_STRU
 结构说明  : 查询MM层全局参数时，给OM响应的结构
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                          /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgId;            /*_H2ASN_Skip*/
    APP_MSG_HEADER

    APP_EMM_MAINTAIN_RST_ENUM_UINT32    ulRslt ;            /* 查询是否成功 */
    EMM_OM_G_INFO_TYPE_ENUM_UINT32      enGInfoType;
    union
    {
        NAS_EMM_UPDATE_STATE_ENUM_UINT8 ucEmmUpStat;
        NAS_MM_PLMN_LIST_STRU           stEPlmnList;
        NAS_MM_TA_LIST_STRU             stTaiList;
        NAS_LMM_TIMER_CTRL_STRU          stTimer;
        VOS_UINT32                      ulTauAttemptCounter;
        NAS_EMM_TAU_START_CAUSE_ENUM_UINT8 enTauStartCause;
        NAS_EMM_SER_START_CAUSE_ENUM_UINT8 enSerStartCause;
        VOS_UINT8                       ucDhcpv4ServerDeftEpsbId;
        VOS_UINT8                       ucDhcpv4SeverTmpStatus;
        VOS_UINT8                       ucDhcpv4SeverAllocStatus;
        /*IP_DHCPV4SERVER_NWPARAM_STRU    stDhcpv4ServerNWParam;*/
        VOS_UINT8                       aucDhcpv4ServerBuffer[NAS_EMM_DHCP_SERVER_BUFFER_LEN];
        VOS_UINT32                      ulIpfUlFilterNum;
		/*leili modify for isr begin*/
        NAS_LMM_TIN_TYPE_ENUM_UINT32    ulTinType;
        NAS_MML_PS_BEARER_CONTEXT_STRU  astPsBearerCtx[EMM_ESM_MAX_EPS_BEARER_NUM];
        /*leili modify for isr end*/
		NAS_MM_NETWORK_ID_STRU          stLvrTai;
        NAS_LMM_REG_DOMAIN_ENUM_UINT32  enRegDomain;
	}u;

}APP_MM_TP_G_INFO_CNF_STRU;



/*原来写在interface.c*/
typedef struct
{
    VOS_UINT32                          ulNasMsgSize;       /* [1,RRC_NAS_MAX_SIZE_NAS_MSG],Byte */
    VOS_UINT8                           aucNasMsg[4];       /* 消息的前四个字节内容              */
    /******************************************************************************************************
        _H2ASN_Array2String
    ******************************************************************************************************/
}NAS_MSG_STRU;


/* OM动作处理函数的类型定义 */
typedef VOS_VOID    ( * NAS_LMM_OM_ACTION_FUN )( MsgBlock  *  /* 消息指针 */);


/* OM动作表结构 */
typedef struct
{
    APP_MM_MSG_TYPE_ENUM_UINT32         ulMsgId;     /* 消息ID */
    NAS_LMM_OM_ACTION_FUN                pfFun;       /* 设置 或 查询函数 */
}NAS_LMM_OM_ACT_STRU;


#if 0
typedef NAS_LMM_COMM_ACTION_FUN          NAS_LMM_OM_ACTION_FUN;
typedef NAS_LMM_COMM_ACT_STRU            NAS_LMM_OM_ACT_STRU;
#endif

typedef NAS_LMM_COMM_ACTION_FUN          NAS_LMM_PRE_PROC_ACTION_FUN;
typedef NAS_LMM_COMM_ACT_STRU            NAS_LMM_PRE_PROC_ACT_STRU;

typedef struct
{
    VOS_UINT8                                   ucRsrv;
    NAS_EMM_CN_MSG_TYPE_ENUM_UINT8              enCnMsgType;
    NAS_LMM_OM_AIR_MSG_ID_ENUM_UINT8             enAirMsg;
    PS_EMM_OMT_KEY_EVENT_TYPE_ENUM_UINT8        enKeyEvent;

}NAS_LMM_OM_MSG_ID_CHANGE_STRU;

typedef struct
{
    APP_MM_MSG_TYPE_ENUM_UINT32         ulMsgId;
    VOS_UINT32                          ulOpId;
    VOS_UINT16                          usOriginalId;
    VOS_UINT16                          usTerminalId;
    VOS_UINT32                          ulTimeStamp;
    VOS_UINT32                          ulSN;
    VOS_UINT32                          ulRslt;

}NAS_LMM_APP_PARA_SET_RESLT_STRU;

typedef struct
{
    VOS_MSG_HEADER                                          /*_H2ASN_Skip*/
    VOS_UINT32                          ulMsgId;            /*_H2ASN_Skip*/
    APP_MSG_HEADER
    VOS_UINT32                          ulOpId;             /*本次操作标识*/
    APP_MM_SET_TYPE_ENUM_UINT32         enSetType;          /*设置类型:0:设置,1:删除*/
}NAS_LMM_APP_PARA_SET_HEAD_STRU;

/* xiongxianghui00253310 modify for ftmerrlog begin */

typedef struct
{
    APP_EMM_MAIN_STATE_ENUM_UINT16       enMainState;
    APP_EMM_SUB_STATE_ENUM_UINT16        enSubState;

    NAS_EMM_PLMN_ID_STRU                 stPlmnId;
    NAS_EMM_GUTI_STRU                    stGuti;
    /* 单位ms，参考函数 NAS_LMM_ModifyPtlTimer()的处理 */
    VOS_UINT32                           ulTimerLen3412;
    NAS_EMM_TAC_STRU                      stTac;

    NAS_EMM_CN_CAUSE_ENUM_UINT8          enEmmCnRejCause;
    VOS_UINT8                            aucRsv[3];

}EMM_DATABASE_INFO_STRU;

typedef struct
{
    /* 打开或者关闭工程模式上报功能 0:close 1:open*/
    VOS_UINT32                          ulFtmActionFlag;
    /* EMM给OM发送的每个工程模式消息都有此编号，上电为0，然后依次加 1 */
    VOS_UINT32                          ulMsgSN;
    /* EMM工程模式管理结构临时存储CnCause，用于与本地数据库进行比对 */
    NAS_EMM_CN_CAUSE_ENUM_UINT8         enEmmCnRejCause;
    VOS_UINT8                           aucRsv[3];

}EMM_FTM_INFO_MANAGE_STRU;

typedef struct
{
    NAS_MM_CN_CAUSE_ENUM_UINT8            ulCauseId;    /*cause ID*/
    LNAS_OM_ERRLOG_ID_ENUM_UINT16         ulErrorlogID; /*error id*/
}NAS_LMM_CN_CAUSE_TRANS_STRU;

/*****************************************************************************
 结构名    : NAS_LMM_RECIVE_MSG_STRU
 结构说明  : LMM 入口消息的信息
*****************************************************************************/
typedef struct
{
    VOS_UINT32                   ulTimeStamp;  /* 时间戳 */
    VOS_UINT32                   ulSendPid;    /* 发送任务Pid */
    VOS_UINT32                   ulMsgName;    /* 发送消息ID */
}NAS_LMM_RECIVE_MSG_STRU;


/*****************************************************************************
 结构名    : NAS_LMM_SAVE_RECIVE_MSG_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 发生故障时，模块收到的最新消息
  ulNextIndex保存下一个收到消息索引，例如ulNextIndex = 1，则astReciveMsgInfo[0]
 是最新的收到的消息ID
*****************************************************************************/
typedef struct
{
    VOS_UINT32                         ulNextIndex;
    NAS_LMM_RECIVE_MSG_STRU            astReciveMsgInfo[NAS_SAVE_RECEIVE_MSG_INFO_NUM];
} NAS_LMM_MSG_SAVE_INFO_STRU;

typedef struct
{
    VOS_UINT32                             ulActionFlag;
    VOS_UINT32                             ulMsgSN;
     /* ERR LOG上报级别,每个模块的每个级别对应一个ERR LOG缓存数组
    故障&告警级别
    Warning： 0x04代表提示，
    Minor：   0x03代表次要
    Major：   0x02答标重要
    Critical：0x01代表紧急    */
    NAS_EMM_ERRLOG_LEVEL_ENUM_UINT16       usALMLevel;
    NAS_EMM_ERRLOG_TYPE_ENUM_UINT16        usALMType;
    /* EMM最新ERR LOG发生的时间戳 */
    VOS_UINT32                             ulAlmLowSlice;/*时间戳*/
    VOS_UINT32                             ulAlmHighSlice;

    VOS_UINT32                             ulErrLogAmount;
    VOS_UINT32                             ulNextNullPos;
    LMM_ERR_INFO_DETAIL_STRU               stLmmErrInfoDetail[NAS_EMM_ERRLOG_MAX_NUM];
    NAS_LMM_MSG_SAVE_INFO_STRU             stMsgQueueInfo;
}EMM_ERRLOG_INFO_MANAGE_STRU;

/* xiongxianghui00253310 modify for ftmerrlog end  */

/*MM模块需上报路测的信息，后续上报新内容也需在此添加*/
typedef struct
{
    NAS_MM_GUTI_STRU                         stGuti;           /*GUTI信息 */
    NAS_OM_IMSI_INFO_STRU                    stImsi;           /*IMSI信息 */
    NAS_OM_EMM_STATE_STRU                    stEmmState;        /*EMM状态信息 */
}APP_MM_DT_STRU;

/*****************************************************************************
  6 UNION
*****************************************************************************/


/*****************************************************************************
  7 Extern Global Variable
*****************************************************************************/
extern NAS_EMM_DRX_CYCLE_LEN_CHANGE_ENUM_UINT32    g_ulDrxCycleLenChange;
extern NAS_LMM_OM_MSG_ID_CHANGE_STRU g_stMmOmMsgIdStru[];
extern VOS_UINT32 g_ulSzieof_g_stMmOmMsgIdStru;
extern APP_MM_DT_REPORT_CTRL_STRU  gastNasDtRptCtrlTbl[NAS_DT_RPT_CTRL_MAX_ITEM_NUM];

extern APP_MS_CLASSMARK_STRU        g_aucMsClassMark;

/* xiongxianghui00253310 modify for ftmerrlog begin */
extern EMM_FTM_INFO_MANAGE_STRU              g_astEmmFtmInfoManage;
extern EMM_DATABASE_INFO_STRU                g_astEmmInfo;
extern EMM_ERRLOG_INFO_MANAGE_STRU           g_astEmmErrlogInfoManage;

/* xiongxianghui00253310 modify for ftmerrlog end   */



/*****************************************************************************
  8 Fuction Extern
*****************************************************************************/
extern  NAS_LMM_OM_ACTION_FUN  NAS_LMM_OmFindFun( VOS_UINT32          ulMsgId);
extern VOS_UINT32 NAS_LMM_DtCompareGuti(VOS_VOID);
extern VOS_UINT32 NAS_LMM_DtCompareImsi(VOS_VOID);
extern VOS_UINT32 NAS_LMM_DtCompareEmmState(VOS_VOID);
extern VOS_VOID NAS_LMM_DtJudgeifNeedRpt(VOS_VOID);
extern VOS_VOID  NAS_LMM_ReadImsaNvImsCapability( VOS_UINT8 *pucSmsOverIms, VOS_UINT8 *pucSrvcc);

/*
extern  VOS_VOID    NAS_LMM_MtTpSetOptUeNetCap ( MsgBlock    *pMsg);
extern  VOS_VOID    NAS_LMM_MtTpSetOptMsNetCap(  MsgBlock    *pMsg);
*/
extern  VOS_VOID  NAS_LMM_SetAppParaValue
(
    NAS_LMM_APP_PARA_SET_RESLT_STRU      *pstParaInfo,
    const NAS_LMM_APP_PARA_SET_HEAD_STRU       *pstAppMsg
);
extern  VOS_UINT32  NAS_LMM_OmMaintain(          MsgBlock    *pMsg);
extern VOS_VOID     NAS_LMM_PubmDaInqRegStat( MsgBlock    *pMsg );
extern VOS_UINT32 NAS_EMM_OmInfoIndMsgProc(MsgBlock    *pMsg );

extern VOS_UINT32  NAS_LMM_CompareEmmInfo( VOS_VOID );

extern VOS_VOID     NAS_LMM_PubmDaInqLteCs(   MsgBlock    *pMsg  );
extern VOS_VOID     NAS_LMM_PubmDaInqUeMode(   MsgBlock    *pMsg  );
extern  VOS_VOID    NAS_LMM_SendOmtKeyEvent(     PS_EMM_OMT_KEY_EVENT_TYPE_ENUM_UINT8  ucEmmOmtKeyEvent  );
extern  VOS_VOID    NAS_LMM_SendOmtAirMsg(       NAS_LMM_OM_AIR_MSG_UP_DOWN_ENUM_UINT8  ucDir,
                                                NAS_LMM_OM_AIR_MSG_ID_ENUM_UINT8       ucAirMsgId,
                                                NAS_MSG_STRU                          *pstNasMsg);
extern  VOS_VOID    NAS_LMM_FindAirMsgId(        VOS_UINT8 ucCnMsgType,
                                                VOS_UINT8 *ucAirMsg,
                                                VOS_UINT8 *ucKeyEvent);
/*extern  VOS_VOID    NAS_LMM_PubmDaSendSetCnf(
                                            VOS_UINT32                             ulRslt,
                                             const NAS_LMM_APP_PARA_STRU           *pAppPara);*/
VOS_VOID    NAS_LMM_PubmDaSendSetCnf( const NAS_LMM_APP_PARA_SET_RESLT_STRU *pstAppRslt);
extern  VOS_VOID NAS_EMM_SendIntraTauReq
(
    APP_MM_MSG_TYPE_ENUM_UINT32         ulOmMmMsgType,
    NAS_LMM_INTRA_TAU_TYPE_ENUM_UINT32  enIntraTauType
);

extern VOS_VOID    NAS_LMM_PubmGetAreaInfo
       (
           APP_GUTI_STRU                       *pstGuti,
           APP_TA_LIST_STRU                    *pstTaiList,
           APP_TA_STRU                         *pstLastTa
       );
extern VOS_VOID    NAS_LMM_PubmGetEmmInfo
       (
           APP_EMM_INFO_STRU                   *pstEmmInfoCnf
       );
extern VOS_VOID    NAS_LMM_PubmGetEmmState
       (
           APP_EMM_MAIN_STATE_ENUM_UINT16      *pulProtoclMainState,
           APP_EMM_SUB_STATE_ENUM_UINT16       *pulProtoclSubState,
           NAS_LMM_MAIN_STATE_ENUM_UINT16      enMainState,
           NAS_LMM_SUB_STATE_ENUM_UINT16       enSubState

       );
extern VOS_VOID    NAS_LMM_PubmGetEmmUpdateState
       (
           APP_EMM_UPDATE_STATE_ENUM_UINT32    *pulEmmUpState
       );
extern VOS_VOID    NAS_LMM_PubmIndEmmInfo(VOS_VOID);
extern VOS_UINT32  NAS_LMM_CheckAppDrxParam( CONST APP_DRX_STRU *pstAppDrxParam );
extern VOS_UINT32  NAS_LMM_IsDrxSplitOnCcchValid( VOS_UINT8 ucSplitOnCcch );
extern VOS_UINT32  NAS_LMM_IsDrxNonDrxTimerValid( VOS_UINT8 ucNonDrxTimer );
extern VOS_UINT32  NAS_LMM_IsDrxPgCycleCodeValid( VOS_UINT8 ucSplitPgCode );
extern VOS_UINT32  NAS_LMM_IsUeDrxCycleValid( VOS_UINT8 ucPsDrxLen );

/*PC REPLAY MODIFY BY LEILI DELETE*/
/* xiongxianghui00253310 modify for ftmerrlog begin */
extern VOS_VOID     NAS_LMM_OmInfoIndProc(VOS_VOID);
extern VOS_VOID     NAS_LMM_FtmInfoInit(VOS_VOID);
extern VOS_UINT32   NAS_LMM_RevOmFtmCtrlMsg(MsgBlock   *pMsgStru);
extern VOS_UINT32   NAS_LMM_CompareEmmDatabaseInfo(VOS_VOID);
extern VOS_VOID     NAS_LMM_UpdateEmmDatabaseInfo(VOS_VOID);
extern VOS_VOID     NAS_LMM_SendOmFtmMsg(VOS_VOID);

extern VOS_VOID     NAS_LMM_ErrlogInfoInit(VOS_VOID);
extern VOS_UINT32   NAS_LMM_RevOmErrlogCtrlMsg(MsgBlock   *pMsgStru);
extern VOS_UINT32   NAS_LMM_RevOmReadErrlogReq(const MsgBlock   *pMsgStru);
extern VOS_VOID     NAS_LMM_SendOmErrlogCnf(VOS_VOID);
extern VOS_VOID     NAS_LMM_ErrlogInfoProc(VOS_UINT8 ucCnCause);
extern LNAS_OM_ERRLOG_ID_ENUM_UINT16  NAS_LMM_CnCauseProc(NAS_EMM_CN_CAUSE_ENUM_UINT8 ucCnCause);
extern VOS_VOID NAS_LMM_SaveRevMsgInfo(MsgBlock *pMsg);
extern VOS_VOID NAS_LMM_ExportRevMsgQueque2ExcLog(VOS_UINT32* pulExcLogAddr, VOS_UINT32 ulSaveSize);

/* xiongxianghui00253310 modify for ftmerrlog end   */

/* niuxiufan DT begin */
extern VOS_VOID  NAS_LMM_PubmDaInqGuti(MsgBlock *pMsg);
extern VOS_VOID  NAS_LMM_PubmDaInqImsi(MsgBlock *pMsg);
extern VOS_VOID  NAS_LMM_PubmDaInqEmmState(MsgBlock *pMsg);
extern VOS_VOID  NAS_LMM_PubmIndGuti(VOS_VOID);
extern VOS_VOID  NAS_LMM_PubmIndImsi(VOS_VOID);
extern VOS_VOID  NAS_LMM_PubmIndEmmState(VOS_VOID);
/* niuxiufan DT end */


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

#endif /* end of NasLmmPubMOm.h*/
