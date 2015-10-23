

#ifndef __RRM_H__
#define __RRM_H__


/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include "Ps.h"
#include "pslog.h"
#include "TtfNvInterface.h"
#include "RrmTimer.h"
#include "PsRrmInterface.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#pragma pack(4)

#if (FEATURE_ON == FEATURE_DSDS)
/*****************************************************************************
  2 宏定义
*****************************************************************************/
/* RRM RFID默认值 */
#define RRM_RFID_DEFAULT_VALUE              (0xFFFF)

/* RRM PID默认值 */
#define RRM_PID_DEFAULT_VALUE               (0xFFFFFFFF)

/* RRM 支持注册的最大个数 */
#define RRM_REG_MAX_NUMBER                  (6)

/* RFID索引掩码 */
#define RRM_RFID_INDEX_MASK                 (0x0FFF)

/* MODEMID索引掩码 */
#define RRM_MODEMID_INDEX_MASK              (0x0FFF)

/* REGID索引掩码 */
#define RRM_REGID_INDEX_MASK                (0x0FFF)

/* 根据RFID获取下标索引号 */
#define RRM_GET_RFID_INDEX(enRFID)          ((VOS_UINT16)((enRFID) & (RRM_RFID_INDEX_MASK)))

/* 根据MODEM ID获取下标索引号 */
#define RRM_GET_MODEMID_INDEX(enModemId)    ((VOS_UINT16)((enModemId) & (RRM_MODEMID_INDEX_MASK)))

/* 根据REGID获取下标索引号 */
#define RRM_GET_REGID_INDEX(usRegId)        ((VOS_UINT16)((usRegId) & (RRM_REGID_INDEX_MASK)))




/*****************************************************************************
  3 枚举定义
*****************************************************************************/


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

/*****************************************************************************
 结构名    : RRM_REG_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : RRM 注册信息结构
*****************************************************************************/
typedef struct
{
    PS_BOOL_ENUM_UINT8                  enRegisteredFlg;    /* 是否已注册的标识 */
    RRM_PS_RAT_TYPE_ENUM_UINT8          enRatType;          /* 注册的RAT type */
    MODEM_ID_ENUM_UINT16                enModemId;          /* 注册的MODEM ID */
    VOS_UINT32                          ulPid;              /* 注册的PID */
}RRM_REG_INFO_STRU;




typedef struct
{
    PS_BOOL_ENUM_UINT8                  enRFIDUsedFlg;          /* 是否已使用RFID的标识 */
    VOS_UINT8                           aucReserved[3];
    VOS_UINT16                          usUsingRFIDValue;       /* 正在使用的RFID值 */
    MODEM_ID_ENUM_UINT16                enUsingModemId;         /* 正在使用的MODEMID的值 */
    RRM_RFID_TIMER_CTRL_STRU            stRFIDTimer;            /* RRM Timer管理结构 */
}RRM_RFID_INFO_STRU;



typedef struct
{
    NV_MODEM_RF_SHARE_CFG_STRU          astModemCfgInfo[MODEM_ID_BUTT];     /* RRM模块MODEM的配置信息 */

    RRM_RFID_INFO_STRU                  astRFIDInfo[RRM_RFID_BUTT];         /* RRM模块RFID的信息 */

    RRM_REG_INFO_STRU                   astRrmRegInfo[RRM_REG_MAX_NUMBER];  /* RRM模块注册的信息 */

    /* 后续可能会扩展其它功能 */

}RRM_INFO_STRU;



/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/
extern   RRM_INFO_STRU                      g_stRrmInfo;


/* 获取RRM 模块信息地址 */
#define RRM_GetInfoAddr()                   (&g_stRrmInfo)

/* 获取RRM RF信息列表地址 */
#define RRM_GetRFIDInfoListAddr()           (((RRM_GetInfoAddr())->astRFIDInfo))

/* 获取RRM MODEM配置信息列表地址 */
#define RRM_GetModemCfgInfoListAddr()       (((RRM_GetInfoAddr())->astModemCfgInfo))


/* 根据MODEM ID索引号获取RRM MODEM配置信息地址 */
#define RRM_GetModemSupportFlg(enModemId)  \
    ((enModemId < MODEM_ID_BUTT) ? (RRM_GetModemCfgInfoListAddr()[RRM_GET_MODEMID_INDEX(enModemId)].usSupportFlag) : PS_FALSE)


/* 根据MODEM ID索引号获取RRM MODEM配置信息地址 */
#define RRM_GetModemCfgInfoAddr(enModemId)  \
    ((enModemId < MODEM_ID_BUTT) ? (&(RRM_GetModemCfgInfoListAddr()[RRM_GET_MODEMID_INDEX(enModemId)])) : VOS_NULL_PTR)

/* 根据RFID索引号获取RRM RF信息地址 */
#define RRM_GetRFIDInfoAddr(enRFIDIndex)    \
    ((enRFIDIndex < RRM_RFID_BUTT) ? (&(RRM_GetRFIDInfoListAddr()[RRM_GET_RFID_INDEX(enRFIDIndex)])) : VOS_NULL_PTR)

/* 获取RRM RFID Timer信息地址 */
#define RRM_GetRFIDTimerAddr(enRFIDIndex)   \
    ((enRFIDIndex < RRM_RFID_BUTT) ? (&(RRM_GetRFIDInfoListAddr()[RRM_GET_RFID_INDEX(enRFIDIndex)].stRFIDTimer)) : VOS_NULL_PTR)

/* 获取RRM 注册信息列表地址 */
#define RRM_GetRegInfoListddr()             \
    (((RRM_GetInfoAddr())->astRrmRegInfo))


/* 根据RFID索引号获取RRM RF信息地址 */
#define RRM_GetRegInfoAddr(usRegInfoIndex)    \
    ((usRegInfoIndex < RRM_REG_MAX_NUMBER) ? (&(RRM_GetRegInfoListddr()[RRM_GET_REGID_INDEX(usRegInfoIndex)])) : VOS_NULL_PTR)


/* 根据MODEM ID索引号获取GSM RFID值 */
#define RRM_GET_GSM_RFID(enModemId)         \
    ((enModemId < MODEM_ID_BUTT) ? (RRM_GetModemCfgInfoListAddr()[RRM_GET_MODEMID_INDEX(enModemId)].usGSMRFID) : RRM_RFID_DEFAULT_VALUE)

/* 根据MODEM ID索引号获取WCDMA RFID值 */
#define RRM_GET_WCDMA_RFID(enModemId)       \
    ((enModemId < MODEM_ID_BUTT) ? (RRM_GetModemCfgInfoListAddr()[RRM_GET_MODEMID_INDEX(enModemId)].usWCDMARFID) : RRM_RFID_DEFAULT_VALUE)

/* 根据MODEM ID索引号获取TDS RFID值 */
#define RRM_GET_TDS_RFID(enModemId)         \
    ((enModemId < MODEM_ID_BUTT) ? (RRM_GetModemCfgInfoListAddr()[RRM_GET_MODEMID_INDEX(enModemId)].usTDSRFID) : RRM_RFID_DEFAULT_VALUE)

/* 根据MODEM ID索引号获取LTE RFID值 */
#define RRM_GET_LTE_RFID(enModemId)         \
    ((enModemId < MODEM_ID_BUTT) ? (RRM_GetModemCfgInfoListAddr()[RRM_GET_MODEMID_INDEX(enModemId)].usLTERFID) : RRM_RFID_DEFAULT_VALUE)

/* 根据MODEM ID索引号获取CDMA RFID值 */
#define RRM_GET_CDMA_RFID(enModemId)        \
    ((enModemId < MODEM_ID_BUTT) ? (RRM_GetModemCfgInfoListAddr()[RRM_GET_MODEMID_INDEX(enModemId)].usCDMARFID) : RRM_RFID_DEFAULT_VALUE)

/* 根据MODEM ID索引号获取EVDO RFID值 */
#define RRM_GET_EVDO_RFID(enModemId)        \
    ((enModemId < MODEM_ID_BUTT) ? (RRM_GetModemCfgInfoListAddr()[RRM_GET_MODEMID_INDEX(enModemId)].usEVDORFID) : RRM_RFID_DEFAULT_VALUE)



/*****************************************************************************
  10 函数声明
*****************************************************************************/

VOS_VOID  RRM_Init(VOS_VOID);
VOS_UINT32 RRM_JudgeRegInfoIsReged
(
    MODEM_ID_ENUM_UINT16                enModemId,
    VOS_UINT32                          ulPid,
    PS_BOOL_ENUM_UINT8                 *enIsSameModem,
    VOS_UINT16                         *usRegInfoIndex,
    PS_BOOL_ENUM_UINT8                 *enRegInfoIsReged
);

VOS_UINT32 RRM_JudgeRFIDIsUsed
(
    MODEM_ID_ENUM_UINT16                enModemId,
    VOS_UINT16                          usRFIDValue,
    PS_BOOL_ENUM_UINT8                 *enIsSameModem,
    RRM_RFID_ENUM_UINT16               *enRFIDIndex,
    PS_BOOL_ENUM_UINT8                 *enRFIDIsUsed
);


VOS_VOID RRM_SetRegInfo
(
    PS_BOOL_ENUM_UINT8                  enRegisteredFlg,
    RRM_PS_RAT_TYPE_ENUM_UINT8          enRatType,
    MODEM_ID_ENUM_UINT16                enModemId,
    VOS_UINT32                          ulPid
);

VOS_UINT32 RRM_SetRFIDUsedInfo
(
    PS_BOOL_ENUM_UINT8                  enRFIDUsedFlg,
    VOS_UINT16                          usUsingRFIDValue,
    MODEM_ID_ENUM_UINT16                enUsingModemId
);


VOS_VOID RRM_ClearRegInfo
(
    VOS_UINT16                          usRegInfoIndex
);

VOS_VOID RRM_ClearRFIDUsedInfo
(
    RRM_RFID_ENUM_UINT16                enRFIDIndex
);

VOS_UINT16 RRM_GetRFIDCfgValue
(
    MODEM_ID_ENUM_UINT16                enModemId,
    RRM_PS_RAT_TYPE_ENUM_UINT8          enRatType
);


#endif /* FEATURE_ON == FEATURE_DSDS */


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

#endif /* end of Rrm.h */

