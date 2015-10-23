

#ifndef __PSRRMINTERFACE_H__
#define __PSRRMINTERFACE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include  "vos.h"
#include  "PsTypeDef.h"
#include  "vos_Id.h"

/*****************************************************************************
  #pragma pack(*)    设置字节对齐方式
*****************************************************************************/
#if (VOS_OS_VER != VOS_WIN32)
#pragma pack(4)
#else
#pragma pack(push, 4)
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/


/*****************************************************************************
  3 枚举定义
*****************************************************************************/

enum PS_RRM_MSG_TYPE_ENUM
{
    ID_PS_RRM_RADIO_RESOURCE_APPLY_REQ                          = 0x0001,       /* _H2ASN_MsgChoice PS_RRM_RADIO_RESOURCE_APPLY_REQ_STRU */
    ID_RRM_PS_RADIO_RESOURCE_APPLY_CNF                          = 0x0002,       /* _H2ASN_MsgChoice RRM_PS_RADIO_RESOURCE_APPLY_CNF_STRU */

    ID_PS_RRM_RADIO_RESOURCE_RELEASE_IND                        = 0x0003,       /* _H2ASN_MsgChoice PS_RRM_RADIO_RESOURCE_RELEASE_IND_STRU */

    ID_PS_RRM_REGISTER_IND                                      = 0x0004,       /* _H2ASN_MsgChoice PS_RRM_REGISTER_IND_STRU */
    ID_PS_RRM_DEREGISTER_IND                                    = 0x0005,       /* _H2ASN_MsgChoice PS_RRM_DEREGISTER_IND_STRU */

    ID_RRM_PS_STATUS_IND                                        = 0x0006,       /* _H2ASN_MsgChoice RRM_PS_STATUS_IND_STRU */

    ID_PS_RRM_MSG_TYPE_BUTT
};
typedef VOS_UINT32 PS_RRM_MSG_TYPE_ENUM_UINT32;


enum RRM_PS_RAT_TYPE_ENUM
{
    RRM_PS_RAT_TYPE_NONE                    = 0,
    RRM_PS_RAT_TYPE_GSM                     = 1,
    RRM_PS_RAT_TYPE_WCDMA                   = 2,
    RRM_PS_RAT_TYPE_TDS                     = 3,
    RRM_PS_RAT_TYPE_LTE                     = 4,
    RRM_PS_RAT_TYPE_1X                      = 5,
    RRM_PS_RAT_TYPE_EVDO                    = 6,
    RRM_PS_RAT_TYPE_BUTT
};
typedef VOS_UINT8 RRM_PS_RAT_TYPE_ENUM_UINT8;



enum RRM_PS_TASK_TYPE_ENUM
{
    /* 初始状态无任务 */
    RRM_PS_TASK_TYPE_NONE                          = 0,

    /* 1-100用于定义NAS任务 */
    RRM_PS_TASK_TYPE_NAS_SEARCH                    = 1,

    /* 101-200用于定义GSM任务 */
    RRM_PS_TASK_TYPE_GSM_NAS_SPEC_SEARCH           = 111,
    RRM_PS_TASK_TYPE_GSM_NAS_LIST_SEARCH           = 112,

    /* 201-300用于定义WCDMA任务 */
    RRM_PS_TASK_TYPE_WCDMA_NAS_SPEC_SEARCH         = 210,
    RRM_PS_TASK_TYPE_WCDMA_NAS_LIST_SEARCH         = 211,

    RRM_PS_TASK_TYPE_BUTT
};
typedef VOS_UINT16 RRM_PS_TASK_TYPE_ENUM_UINT16;


enum PS_RRM_RESULT_ENUM
{
    PS_RRM_RESULT_SUCCESS               = 0,
    PS_RRM_RESULT_FAIL                  = 1,
    PS_RRM_RESULT_BUTT
};
typedef VOS_UINT8 PS_RRM_RESULT_ENUM_UINT8;

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
    MSG_HEADER_STRU                             stMsgHeader;                    /* 消息头 */ /*_H2ASN_Skip*/
    MODEM_ID_ENUM_UINT16                        enModemId;                      /* _H2ASN_Replace VOS_UINT16 enModemId; */
    RRM_PS_RAT_TYPE_ENUM_UINT8                  enRatType;                      /* 需要使用射频资源的模式，如果是异系统互操作，填写目标模式 */
    VOS_UINT8                                   aucReserved[1];
}PS_RRM_RADIO_RESOURCE_APPLY_REQ_STRU;
typedef struct
{
    MSG_HEADER_STRU                             stMsgHeader;                    /* 消息头 */ /*_H2ASN_Skip*/
    PS_RRM_RESULT_ENUM_UINT8                    enResult;
    VOS_UINT8                                   aucReserved[3];
} RRM_PS_RADIO_RESOURCE_APPLY_CNF_STRU;
typedef struct
{
    MSG_HEADER_STRU                             stMsgHeader;                    /* 消息头 */ /*_H2ASN_Skip*/
    MODEM_ID_ENUM_UINT16                        enModemId;                      /* _H2ASN_Replace VOS_UINT16 enModemId; */
    RRM_PS_RAT_TYPE_ENUM_UINT8                  enRatType;                      /* 释放射频资源的模式，如果是异系统互操作，填写目标模式 */
    VOS_UINT8                                   aucReserved[1];
}PS_RRM_RADIO_RESOURCE_RELEASE_IND_STRU;
typedef struct
{
    MSG_HEADER_STRU                             stMsgHeader;                    /* 消息头 */ /*_H2ASN_Skip*/
    MODEM_ID_ENUM_UINT16                        enModemId;                      /* _H2ASN_Replace VOS_UINT16 enModemId; */
    RRM_PS_TASK_TYPE_ENUM_UINT16                enTaskType;                     /* 有效任务则注册，BUTT则去注册 */
    RRM_PS_RAT_TYPE_ENUM_UINT8                  enRatType;
    VOS_UINT8                                   aucReserved[3];
} PS_RRM_REGISTER_IND_STRU;
typedef struct
{
    MSG_HEADER_STRU                             stMsgHeader;                    /* 消息头 */ /*_H2ASN_Skip*/
    MODEM_ID_ENUM_UINT16                        enModemId;                      /* _H2ASN_Replace VOS_UINT16 enModemId; */
    RRM_PS_TASK_TYPE_ENUM_UINT16                enTaskType;                     /* 有效任务则注册，BUTT则去注册 */
    RRM_PS_RAT_TYPE_ENUM_UINT8                  enRatType;
    VOS_UINT8                                   aucReserved[3];
} PS_RRM_DEREGISTER_IND_STRU;
typedef struct
{
    MSG_HEADER_STRU                             stMsgHeader;                    /* 消息头 */ /*_H2ASN_Skip*/
    RRM_PS_TASK_TYPE_ENUM_UINT16                enTaskType;
    VOS_UINT8                                   aucReserved[2];
} RRM_PS_STATUS_IND_STRU;

/*****************************************************************************
  8 UNION定义
*****************************************************************************/

/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/

/*****************************************************************************
  H2ASN顶级消息结构定义
*****************************************************************************/

/*****************************************************************************
 结构名    : PS_RRM_MSG_DATA
 协议表格  :
 ASN.1描述 :
 结构说明  : PS_RRM_MSG_DATA数据结构，用于生成ASN文件
*****************************************************************************/
typedef struct
{
    PS_RRM_MSG_TYPE_ENUM_UINT32                 enMsgId;                        /*_H2ASN_MsgChoice_Export PS_RRM_MSG_TYPE_ENUM_UINT32 */
    VOS_UINT8                                   aucMsg[4];
    /***************************************************************************
        _H2ASN_MsgChoice_When_Comment          PS_RRM_MSG_TYPE_ENUM_UINT32
    ****************************************************************************/
}PS_RRM_MSG_DATA;

/*_H2ASN_Length UINT32*/

/*****************************************************************************
 结构名    : PsRrmInterface_MSG
 协议表格  :
 ASN.1描述 :
 结构说明  : PsRrmInterface_MSG数据结构,用于生成ASN文件
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    PS_RRM_MSG_DATA                             stMsgData;
}PsRrmInterface_MSG;

/*****************************************************************************
  6 UNION
*****************************************************************************/


/*****************************************************************************
  7 Extern Global Variable
*****************************************************************************/


/*****************************************************************************
  8 Fuction Extern
*****************************************************************************/


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

#endif /* end of PsRrmInterface.h */


