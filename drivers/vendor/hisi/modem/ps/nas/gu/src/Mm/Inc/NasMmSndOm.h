
#ifndef _NAS_MM_SND_OM_H
#define _NAS_MM_SND_OM_H_

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include  "vos.h"
#include  "PsTypeDef.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/


/*****************************************************************************
  3 枚举定义
*****************************************************************************/


enum NAS_MM_OM_MSG_ID_ENUM
{
    /* MM发送给OM的消息 */
    MMOM_LOG_STATE_INFO_IND                       = 0x1000,      /*_H2ASN_MsgChoice  NAS_MM_LOG_STATE_INFO_STRU */
    MMOM_LOG_AUTH_INFO_IND                        = 0x1001,      /*_H2ASN_MsgChoice  NAS_MM_LOG_AUTH_INFO_STRU */
    MMOM_LOG_BUTT
};
typedef VOS_UINT32 NAS_MM_OM_MSG_ID_ENUM_UINT32;
enum NAS_MM_STATE_ID_ENUM
{
    STATE_MM_STATE_NULL                                   = 0,
    STATE_MM_IDLE_NO_CELL_AVAILABLE                       = 1,
    STATE_MM_IDLE_PLMN_SEARCH                             = 2,
    STATE_MM_IDLE_NORMAL_SERVICE                          = 3,
    STATE_MM_IDLE_LIMITED_SERVICE                         = 4,
    STATE_MM_IDLE_ATTEMPTING_TO_UPDATE                    = 5,
    STATE_MM_IDLE_LOCATION_UPDATE_NEEDED                  = 6,
    STATE_MM_IDLE_PLMN_SEARCH_NORMAL_SERVICE              = 7,
    STATE_MM_IDLE_NO_IMSI                                 = 8,
    STATE_WAIT_FOR_OUTGOING_MM_CONNECTION                 = 9,
    STATE_MM_CONNECTION_ACTIVE                            = 10,
    STATE_WAIT_FOR_NETWORK_COMMAND                        = 11,
    STATE_WAIT_FOR_RR_CONNECTION_MM_CONNECTION            = 12,
    STATE_WAIT_FOR_REESTABLISH_WAIT_FOR_REEST_REQ         = 13,
    STATE_WAIT_FOR_REESTABLISH_WAIT_FOR_EST_CNF           = 14,
    STATE_WAIT_FOR_RR_ACTIVE                              = 15,
    STATE_WAIT_FOR_ADDITIONAL_OUTGOING_MM_CONNECTION      = 16,
    STATE_LOCATION_UPDATING_PENDING                       = 17,
    STATE_IMSI_DETACH_PENDING                             = 18,
    STATE_MM_WAIT_FOR_ATTACH                              = 19,
    STATE_WAIT_FOR_RR_CONNECTION_LOCATION_UPDATING        = 22,
    STATE_LOCATION_UPDATING_INITIATED                     = 23,
    STATE_LOCATION_UPDATE_REJECTED                        = 24,
    STATE_WAIT_FOR_RR_CONNECTION_IMSI_DETACH              = 25,
    STATE_IMSI_DETACH_INITIATED                           = 26,
    STATE_PROCESS_CM_SERVICE_PROMPT                       = 27,
    STATE_TEST_CONTROL_ACTIVE                             = 31,
    STATE_MM_INTER_RAT_CHANGE                             = 32,
};
typedef VOS_UINT8 NAS_MM_STATE_ID_ENUM_UINT8;

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
    MSG_HEADER_STRU                     stMsgHeader;/* 消息头 */ /*_H2ASN_Skip*/
    NAS_MM_STATE_ID_ENUM_UINT8          enMmState;
    VOS_UINT8                           aucRsv3[3];
}NAS_MM_LOG_STATE_INFO_STRU;
typedef struct
{
    MSG_HEADER_STRU                     stMsgHeader;/* 消息头 */ /*_H2ASN_Skip*/
    VOS_UINT8                           ucExpectOpId;
    VOS_UINT8                           ucRcvOpId;
    VOS_UINT8                           aucRsv[2];
}NAS_MM_LOG_AUTH_INFO_STRU;


/*****************************************************************************
  H2ASN顶级消息结构定义
*****************************************************************************/
typedef struct
{
    NAS_MM_OM_MSG_ID_ENUM_UINT32        enMsgID;    /*_H2ASN_MsgChoice_Export NAS_MM_OM_MSG_ID_ENUM_UINT32*/
                                                    /* 为了兼容NAS的消息头定义，所以转换ASN.1使用NAS_MM_OM_MSG_ID_ENUM_UINT32 */

    VOS_UINT8                           aucMsgBlock[4];
    /***************************************************************************
        _H2ASN_MsgChoice_When_Comment          NAS_MM_OM_MSG_ID_ENUM_UINT32
    ****************************************************************************/
}NAS_MM_SND_OM_MSG_DATA;
/*_H2ASN_Length UINT32*/

typedef struct
{
    VOS_MSG_HEADER
    NAS_MM_SND_OM_MSG_DATA              stMsgData;
}NasMmSndOm_MSG;

/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern VOS_VOID  NAS_MM_LogMmStateInfo(
    VOS_UINT8                           ucMmState
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

