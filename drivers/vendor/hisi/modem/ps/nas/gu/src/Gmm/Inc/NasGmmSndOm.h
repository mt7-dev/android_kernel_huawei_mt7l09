
#ifndef _NAS_GMM_SND_OM_H
#define _NAS_GMM_SND_OM_H_

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


enum NAS_GMM_OM_MSG_ID_ENUM
{
    /* MM发送给OM的消息 */
    GMMOM_LOG_STATE_INFO_IND                       = 0x1000,      /*_H2ASN_MsgChoice  NAS_GMM_LOG_STATE_INFO_STRU */
    GMMOM_LOG_AUTH_INFO_IND                        = 0x1001,      /*_H2ASN_MsgChoice  NAS_GMM_LOG_AUTH_INFO_STRU */
    GMMOM_LOG_RADIAO_ACCESS_CAP                    = 0x1002,      /*_H2ASN_MsgChoice  NAS_GMM_LOG_GAS_RADIO_ACCESS_CAP_STRU */
    GMMOM_LOG_BUTT
};
typedef VOS_UINT32 NAS_GMM_OM_MSG_ID_ENUM_UINT32;


enum NAS_GMM_STATE_ID_ENUM
{
    STATE_GMM_ID_NULL                                     =  0x00,              /* GMM_NULL状态                             */
    STATE_GMM_REGISTERED_INITIATED                        =  0x01,              /* GMM-REGISTERED-INITIATED状态             */
    STATE_GMM_DEREGISTERED_INITIATED                      =  0x02,              /* GMM_DEREGISTERED_INITIATED               */
    STATE_GMM_ROUTING_AREA_UPDATING_INITIATED             =  0x03,              /* GMM_ROUTING_AREA_UPDATING_INITIATED      */
    STATE_GMM_SERVICE_REQUEST_INITIATED                   =  0x04,              /* GMM-SERVICE-REQUEST-INITIATED            */
    
    STATE_GMM_TC_ACTIVE                                   =  0x05,              /* GMM-TC-ACTIVE(进行TC时的状态)            */
    STATE_GMM_GPRS_SUSPENSION                             =  0x06,              /* (GSM only)GMM_GPRS_SUSPENSION */
    
    STATE_GMM_DEREGISTERED_NORMAL_SERVICE                 =  0x10,              /* GMM-DEREGISTERED.NORMAL-SERVICE          */
    STATE_GMM_DEREGISTERED_LIMITED_SERVICE                =  0x11,              /* GMM-DEREGISTERED.LIMITED-SERVICE         */
    STATE_GMM_DEREGISTERED_ATTACH_NEEDED                  =  0x12,              /* GMM-DEREGISTERED.ATTACH-NEEDED           */
    STATE_GMM_DEREGISTERED_ATTEMPTING_TO_ATTACH           =  0x13,              /* GMM-DEREGISTERED.ATTEMPTING-TO-ATTACH    */
    STATE_GMM_DEREGISTERED_NO_CELL_AVAILABLE              =  0x14,              /* GMM-DEREGISTERED.NO-CELL-AVAILABLE       */
    STATE_GMM_DEREGISTERED_PLMN_SEARCH                    =  0x15,              /* GMM-DEREGISTERED.PLMN-SEARCH             */
    STATE_GMM_DEREGISTERED_NO_IMSI                        =  0x16,              /* GMM-DEREGISTERED.NO-IMSI                 */
    
    STATE_GMM_REGISTERED_NORMAL_SERVICE                   =  0x20,              /* GMM-REGISTERED.NORMAL-SERVICE            */
    STATE_GMM_REGISTERED_LIMITED_SERVICE                  =  0x21,              /* GMM-REGISTERED.LIMITED-SERVICE           */
    STATE_GMM_REGISTERED_UPDATE_NEEDED                    =  0x22,              /* GMM-REGISTERED.UPDATE-NEEDED             */
    STATE_GMM_REGISTERED_ATTEMPTING_TO_UPDATE             =  0x23,              /* GMM-REGISTERED.ATTEMPTING-TO-UPDATE      */
    STATE_GMM_REGISTERED_NO_CELL_AVAILABLE                =  0x24,              /* GMM-REGISTERED.NO-CELL-AVAILABLE         */
    STATE_GMM_REGISTERED_PLMN_SEARCH                      =  0x25,              /* GMM-REGISTERED.PLMN-SEARCH               */
    STATE_GMM_REGISTERED_ATTEMPTING_TO_UPDATE_MM          =  0x26,              /* GMM-REGISTERED.ATTEMPTING-TO-UPDATE-MM   */
    STATE_GMM_REGISTERED_IMSI_DETACH_INITIATED            =  0x27,              /* GMM-REGISTERED.IMSI-DETACH-INITIATED     */
    
    STATE_GMM_SUSPENDED_NORMAL_SERVICE                    =  0x30,              /* GMM-SUSPENDED.NORMAL-SERVICE             */
    STATE_GMM_SUSPENDED_GPRS_SUSPENSION                   =  0x31,              /* GMM-SUSPENDED.GPRS-SUSPENSION            */
    STATE_GMM_SUSPENDED_WAIT_FOR_SYSINFO                  =  0x32,              /* GMM-SUSPENDED.WAIT-FOR-SYSINFO           */
    
    STATE_GMM_TYPE_BUTT
};
typedef VOS_UINT8 NAS_GMM_STATE_ID_ENUM_UINT8;

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

    NAS_GMM_STATE_ID_ENUM_UINT8         enGmmState;
    VOS_UINT8                           aucRsv3[3];
}NAS_GMM_LOG_STATE_INFO_STRU;

/*****************************************************************************
 结构名    : NAS_GMM_LOG_GAS_RADIO_ACCESS_CAP_STRU
 结构说明  : 勾GMM调用GAS接口获取的数据值

 修改记录  :
 1.日    期   : 2014年2月27日
   作    者   : w0024274
   修改内容   : 新增

*****************************************************************************/
typedef struct
{
    MSG_HEADER_STRU                     stMsgHeader;/* 消息头 */ /*_H2ASN_Skip*/

    VOS_UINT32                          ulRst;
    VOS_UINT8                           ucMsCapType;
    VOS_UINT8                           ucRsv;
    VOS_UINT16                          usSize;
    VOS_UINT8                           aucData[MS_RADIO_ACCESS_CAPABILITY_MAX_SIZE];
}NAS_GMM_LOG_GAS_RADIO_ACCESS_CAP_STRU;
typedef struct
{
    MSG_HEADER_STRU                     stMsgHeader;/* ??? */ /*_H2ASN_Skip*/
    VOS_UINT8                           ucExpectOpId;
    VOS_UINT8                           ucRcvOpId;
    VOS_UINT8                           aucRsv[2];
}NAS_GMM_LOG_AUTH_INFO_STRU;


/*****************************************************************************
  H2ASN顶级消息结构定义
*****************************************************************************/
typedef struct
{
    NAS_GMM_OM_MSG_ID_ENUM_UINT32       enMsgID;    /*_H2ASN_MsgChoice_Export NAS_GMM_OM_MSG_ID_ENUM_UINT32*/

    VOS_UINT8                           aucMsgBlock[4];
    /***************************************************************************
        _H2ASN_MsgChoice_When_Comment          NAS_GMM_OM_MSG_ID_ENUM_UINT32
    ****************************************************************************/
}NAS_GMM_SND_OM_MSG_DATA;
/*_H2ASN_Length UINT32*/

typedef struct
{
    VOS_MSG_HEADER
    NAS_GMM_SND_OM_MSG_DATA             stMsgData;
}NasGmmSndOm_MSG;

/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern VOS_VOID  NAS_GMM_LogGmmStateInfo(
    VOS_UINT8                           ucGmmState
);

extern VOS_VOID NAS_GMM_LogGasGmmRadioAccessCapability(
    VOS_UINT32                          ulRst,
    VOS_UINT8                           ucMsCapType,
    VOS_UINT16                          usSize,
    VOS_UINT8                          *pucData
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


