/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名      : Om.h
  版 本 号      : 初稿
  作    者      : 甘兰47350
  生成日期      : 2008年5月3日
  最近修改      :
  功能描述      : 该头文件为OM的头文件，主要提供给外部部件使用
  函数列表      :
  修改历史      :
  1.日    期    : 2008年5月3日
    作    者    : 甘兰47350
    修改内容    : 创建文件

******************************************************************************/

#ifndef _OM_APP_H_
#define _OM_APP_H_

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include "pslog.h"
#include "DrvInterface.h"
#include "om.h"


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif

#if  (FEATURE_OFF == FEATURE_MERGE_OM_CHAN)
/*****************************************************************************
  2 宏定义
*****************************************************************************/

#define MNTN_RecordErrorLog(ulErrNo,pBuf,ulLen) \
        MNTN_ErrorLog(__FILE__,THIS_FILE_ID,__LINE__,ulErrNo,pBuf,ulLen)

/*记录USB发送错误的信息的门限值*/
#define USB_SEND_DATA_ERROR_MAX             512
#if (FEATURE_ON == FEATURE_MULTI_FS_PARTITION) /* SFT board*/
#define OM_USB_LOG_FILE                     "/data/modemlog/Exc/OmUSBLog.bin"
#define OM_USB_UNITARY_LOG_FILE             "/modem_log/Exc/OmUSBLog.bin"
#define OM_CONFIG_PATH                      ("/mnvm3:0/SystemCmd.cmf")
#define OM_CONFIG_UNITARY_PATH              ("/modem_log/SystemCmd.cmf")
#else
#define OM_USB_LOG_FILE                     "/yaffs0/Exc/OmUSBLog.bin"
#define OM_USB_UNITARY_LOG_FILE             "/modem_log/Exc/OmUSBLog.bin"
#define OM_CONFIG_PATH                      ("/yaffs0/SystemCmd.cmf")
#define OM_CONFIG_UNITARY_PATH              ("/modem_log/SystemCmd.cmf")
#endif

#define    OAM_MODELE_NAME                  "OAM"

#define    OAM_CCORE_RESET_CBFUN_PRI        (49)

/*****************************************************************************
  3 枚举定义
*****************************************************************************/

enum
{
    OM_ACPU_OK = 0,
    OM_ACPU_PARA_ERR = 100,
    OM_ACPU_ALLOC_FAIL,
    OM_ACPU_SYNC_TIMEOUT,
    OM_ACPU_SEND_FAIL,
    OM_ACPU_CNF_TIMEOUT,
    OM_ACPU_CNF_ERR,
    OM_ACPU_RUN_IRQ
};

enum OM_PROT_HANDLE_ENUM
{
    OM_APP_PORT_HANDLE      =   0,
    OM_CTRL_PORT_HANDLE     =   1,
    OM_HSIC_PORT_HANDLE     =   2,
    OM_PORT_HANDLE_BUTT
};

typedef VOS_UINT32  OM_PROT_HANDLE_ENUM_UINT32;


enum OM_CPU_STATUS_ENUM
{
    OM_CPU_STATUS_RESET      = 0,       /* CPU复位 */
    OM_CPU_STATUS_OK,                   /* CPU已经启动OK */
    OM_CPU_STATUS_BUTT
};
typedef VOS_UINT32 OM_CPU_STATUS_ENUM_UINT32;

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
    VOS_UINT32                      ulUSBWriteNum1;
    VOS_UINT32                      ulUSBWriteNum2;
    VOS_UINT32                      ulUSBWriteCBNum;
    VOS_UINT32                      ulUSBWriteMaxTime;
    VOS_UINT32                      ulUSBINNum;
    VOS_UINT32                      ulUSBINTime;
    VOS_UINT32                      ulUSBOutNum;
    VOS_UINT32                      ulUSBOutTime;
    VOS_UINT32                      ulUSBStateErrNum;
    VOS_UINT32                      ulUSBStateErrTime;
    VOS_UINT32                      ulUSBCBErrNum;
    VOS_UINT32                      ulUSBCBErrTime;
    VOS_UINT32                      ulUSBWriteErrTime;
    VOS_UINT32                      ulUSBWriteErrNum;
    VOS_UINT32                      ulUSBWriteErrValue;
    VOS_UINT32                      ulUSBWriteErrLen;

    VOS_UINT32                      ulUSBOpenNum;
    VOS_UINT32                      ulUSBOpenSlice;
    VOS_UINT32                      ulUSBOpenOkNum;
    VOS_UINT32                      ulUSBOpenOkSlice;
    VOS_UINT32                      ulUSBOpenOk2Num;
    VOS_UINT32                      ulUSBOpenOk2Slice;
    VOS_UINT32                      ulUSBCloseNum;
    VOS_UINT32                      ulUSBCloseSlice;
    VOS_UINT32                      ulUSBCloseOkNum;
    VOS_UINT32                      ulUSBCloseOkSlice;
}OM_ACPU_PORT_INFO;

/*用于记录当前OM通道的信息*/
typedef struct
{
    VOS_UINT32                          ulSocpCBParaErrNum;
    VOS_UINT32                          ulUSBSendNum;
    VOS_UINT32                          ulUSBSendLen;
    VOS_UINT32                          ulUSBSendRealNum;
    VOS_UINT32                          ulUSBSendErrNum;
    VOS_UINT32                          ulUSBSendErrLen;

    VOS_UINT32                          ulOmDiscardNum;
    VOS_UINT32                          ulOmDiscardLen;
    VOS_UINT32                          ulSocpReadDoneErrNum;
    VOS_UINT32                          ulSocpReadDoneErrLen;

    OM_ACPU_PORT_INFO                   astPortInfo[OM_PORT_HANDLE_BUTT];
}OM_ACPU_DEBUG_INFO;

/* 用于记录VCOM发送信息 */
typedef struct
{
    VOS_UINT32                          ulVCOMSendSn;
    VOS_UINT32                          ulVCOMSendNum;
    VOS_UINT32                          ulVCOMSendLen;
    VOS_UINT32                          ulVCOMSendErrNum;
    VOS_UINT32                          ulVCOMSendErrLen;

    VOS_UINT32                          ulVCOMRcvSn;
    VOS_UINT32                          ulVCOMRcvNum;
    VOS_UINT32                          ulVCOMRcvLen;
    VOS_UINT32                          ulVCOMRcvErrNum;
    VOS_UINT32                          ulVCOMRcvErrLen;
    VOS_UINT32                          ulVCOMInNum;
    VOS_UINT32                          ulVCOMInTime;
    VOS_UINT32                          ulVCOMOutNum;
    VOS_UINT32                          ulVCOMOutTime;

    VOS_UINT32                          ulVCOMCloseNum;
    VOS_UINT32                          ulVCOMCloseTime;
    VOS_UINT32                          ulVCOMOpenNum;
    VOS_UINT32                          ulVCOMOpenTime;
}OM_VCOM_DEBUG_INFO;

/* 用于记录VCOM 27发送信息 */
typedef struct
{
    VOS_UINT32                          ulVCOMHdlcEnNum;
    VOS_UINT32                          ulVCOMHdlcEnFailNum;
    VOS_UINT32                          ulVCOMSendNum;
    VOS_UINT32                          ulVCOMSendLen;
    VOS_UINT32                          ulVCOMSendErrNum;
    VOS_UINT32                          ulVCOMSendErrLen;
}OM_VCOM_CBT_LOG_DEBUG_INFO;

/* 用于记录Error Log收到和发送给Ap侧消息 */
typedef struct
{
    VOS_UINT32                          ulFTMRcvNum;
    VOS_UINT32                          ulFTMRcvLen;
    VOS_UINT32                          ulErrLogRcvNum;
    VOS_UINT32                          ulErrLogRcvLen;

    VOS_UINT32                          ulFTMSendNum;
    VOS_UINT32                          ulFTMSendLen;
    VOS_UINT32                          ulErrLogSendNum;
    VOS_UINT32                          ulErrLogSendLen;
}OM_ERR_LOG_DEBUG_INFO;

/* OM收到AP需要在全局变量中记录内容 */
typedef struct
{
    VOS_UINT32                         *pulErrorLogModule;
    VOS_UINT32                         *pulFTMModule;
    VOS_UINT32                          ulErrLogReportSend;/* 记录Err Log需要上报组件 */
    VOS_UINT32                          ulFTMReportSend;   /* 记录FTM需要上报组件 */
    VOS_UINT16                          usModemId;
    VOS_UINT8                           aucRsv[6];
}OM_APP_MSG_RECORD_STRU;

typedef struct
{
    VOS_UINT32                          ulResetNum;
    VOS_UINT32                          ulSlice;
}OM_CPU_STATUS_INFO;

#define    SAVE_MAX_NV_ID              (8)
#define    SAVE_MAX_SEND_INFO          (10)
#define    NV_MNTN_SAVE_MAX_MSG_LEN    (40)

typedef struct
{
    VOS_UINT32                          ulFlag;
    VOS_UINT32                          ulSlice;
    VOS_UINT32                          ulNVNum;
    VOS_UINT16                          usNVID[SAVE_MAX_NV_ID];
}OM_NV_OPERATE_RECORD_STRU;

typedef struct
{
    VOS_UINT32                          ulFlag;
    VOS_UINT32                          ulSlice;
    VOS_UINT8                           aucVaule[NV_MNTN_SAVE_MAX_MSG_LEN];
}OM_NV_MNTN_RECORD_STRU;


/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern VOS_VOID   OM_AcpuAddSNTime(VOS_UINT32 *pulSN, VOS_UINT32 *pulTimeStamp);
extern VOS_UINT32 OM_AcpuEvent(PS_OM_EVENT_IND_STRU *pstEvent);
extern VOS_UINT32 OM_AcpuSendData(OM_RSP_PACKET_STRU *pucMsg, VOS_UINT16 usMsgLen);
extern VOS_VOID   OM_AcpuSendResult(VOS_UINT8 ucFuncType,
                            VOS_UINT32 ulResult, VOS_UINT16 usReturnPrimId);
extern VOS_VOID   OM_AcpuSendContent(VOS_UINT8 ucFuncType,
                             OM_RSP_PACKET_STRU *pstOmToAppMsg, VOS_UINT16 usReturnPrimId);
extern VOS_UINT32 OM_AppGreenChannel(VOS_UINT8 ucFuncType, VOS_UINT16 usPrimId,VOS_UINT8 *pucData, VOS_UINT16 usLen);

extern unsigned int MNTN_ErrorLog(char * cFileName, unsigned int ulFileId, unsigned int ulLine,
                    unsigned int ulErrNo, void *pRecord, unsigned int ulLen);

extern VOS_UINT32 OM_Acpu_WriteLogFile(VOS_CHAR * cFileName, void *pRecord, VOS_UINT32 ulLen);

extern VOS_UINT32 OM_AcpuGetLogPath(VOS_CHAR *pucBuf, VOS_CHAR *pucOldPath, VOS_CHAR *pucUnitaryPath);

#else
/*****************************************************************************
  2 宏定义
*****************************************************************************/

#define MNTN_RecordErrorLog(ulErrNo,pBuf,ulLen) \
        MNTN_ErrorLog(__FILE__,THIS_FILE_ID,__LINE__,ulErrNo,pBuf,ulLen)

/*记录USB发送错误的信息的门限值*/
#define USB_SEND_DATA_ERROR_MAX             512
#if (FEATURE_ON == FEATURE_MULTI_FS_PARTITION) /* SFT board*/
#define OM_USB_LOG_FILE                     "/data/modemlog/Exc/OmUSBLog.bin"
#define OM_USB_UNITARY_LOG_FILE             "/modem_log/Exc/OmUSBLog.bin"
#define OM_CONFIG_PATH                      ("/mnvm3:0/SystemCmd.cmf")
#define OM_CONFIG_UNITARY_PATH              ("/modem_log/SystemCmd.cmf")
#else
#define OM_USB_LOG_FILE                     "/yaffs0/Exc/OmUSBLog.bin"
#define OM_USB_UNITARY_LOG_FILE             "/modem_log/Exc/OmUSBLog.bin"
#define OM_CONFIG_PATH                      ("/yaffs0/SystemCmd.cmf")
#define OM_CONFIG_UNITARY_PATH              ("/modem_log/SystemCmd.cmf")
#endif

#define    OAM_MODELE_NAME                  "OAM"

#define    OAM_CCORE_RESET_CBFUN_PRI        (49)

/*****************************************************************************
  3 枚举定义
*****************************************************************************/

enum
{
    OM_ACPU_OK = 0,
    OM_ACPU_PARA_ERR = 100,
    OM_ACPU_ALLOC_FAIL,
    OM_ACPU_SYNC_TIMEOUT,
    OM_ACPU_SEND_FAIL,
    OM_ACPU_CNF_TIMEOUT,
    OM_ACPU_CNF_ERR,
    OM_ACPU_RUN_IRQ
};


enum OM_CPU_STATUS_ENUM
{
    OM_CPU_STATUS_RESET      = 0,       /* CPU复位 */
    OM_CPU_STATUS_OK,                   /* CPU已经启动OK */
    OM_CPU_STATUS_BUTT
};
typedef VOS_UINT32 OM_CPU_STATUS_ENUM_UINT32;

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

/* 用于记录VCOM发送信息 */
typedef struct
{
    VOS_UINT32                          ulVCOMSendSn;
    VOS_UINT32                          ulVCOMSendNum;
    VOS_UINT32                          ulVCOMSendLen;
    VOS_UINT32                          ulVCOMSendErrNum;
    VOS_UINT32                          ulVCOMSendErrLen;

    VOS_UINT32                          ulVCOMRcvSn;
    VOS_UINT32                          ulVCOMRcvNum;
    VOS_UINT32                          ulVCOMRcvLen;
    VOS_UINT32                          ulVCOMRcvErrNum;
    VOS_UINT32                          ulVCOMRcvErrLen;
    VOS_UINT32                          ulVCOMInNum;
    VOS_UINT32                          ulVCOMInTime;
    VOS_UINT32                          ulVCOMOutNum;
    VOS_UINT32                          ulVCOMOutTime;

    VOS_UINT32                          ulVCOMCloseNum;
    VOS_UINT32                          ulVCOMCloseTime;
    VOS_UINT32                          ulVCOMOpenNum;
    VOS_UINT32                          ulVCOMOpenTime;
}OM_VCOM_DEBUG_INFO;

/* 用于记录VCOM 27发送信息 */
typedef struct
{
    VOS_UINT32                          ulVCOMHdlcEnNum;
    VOS_UINT32                          ulVCOMHdlcEnFailNum;
    VOS_UINT32                          ulVCOMSendNum;
    VOS_UINT32                          ulVCOMSendLen;
    VOS_UINT32                          ulVCOMSendErrNum;
    VOS_UINT32                          ulVCOMSendErrLen;
}OM_VCOM_CBT_LOG_DEBUG_INFO;

/* 用于记录Error Log收到和发送给Ap侧消息 */
typedef struct
{
    VOS_UINT32                          ulFTMRcvNum;
    VOS_UINT32                          ulFTMRcvLen;
    VOS_UINT32                          ulErrLogRcvNum;
    VOS_UINT32                          ulErrLogRcvLen;

    VOS_UINT32                          ulFTMSendNum;
    VOS_UINT32                          ulFTMSendLen;
    VOS_UINT32                          ulErrLogSendNum;
    VOS_UINT32                          ulErrLogSendLen;
}OM_ERR_LOG_DEBUG_INFO;

/* OM收到AP需要在全局变量中记录内容 */
typedef struct
{
    VOS_UINT32                         *pulErrorLogModule;
    VOS_UINT32                         *pulFTMModule;
    VOS_UINT32                          ulErrLogReportSend;/* 记录Err Log需要上报组件 */
    VOS_UINT32                          ulFTMReportSend;   /* 记录FTM需要上报组件 */
    VOS_UINT16                          usModemId;
    VOS_UINT8                           aucRsv[6];
}OM_APP_MSG_RECORD_STRU;

typedef struct
{
    VOS_UINT32                          ulResetNum;
    VOS_UINT32                          ulSlice;
}OM_CPU_STATUS_INFO;


/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern VOS_VOID   OM_AcpuAddSNTime(VOS_UINT32 *pulSN, VOS_UINT32 *pulTimeStamp);
extern VOS_UINT32 OM_AcpuEvent(PS_OM_EVENT_IND_STRU *pstEvent);
extern VOS_UINT32 OM_AcpuSendData(OM_RSP_PACKET_STRU *pucMsg, VOS_UINT16 usMsgLen);
extern VOS_VOID   OM_AcpuSendResult(VOS_UINT8 ucFuncType,
                            VOS_UINT32 ulResult, VOS_UINT16 usReturnPrimId);
extern VOS_VOID   OM_AcpuSendContent(VOS_UINT8 ucFuncType,
                             OM_RSP_PACKET_STRU *pstOmToAppMsg, VOS_UINT16 usReturnPrimId);
extern VOS_UINT32 OM_AppGreenChannel(VOS_UINT8 ucFuncType, VOS_UINT16 usPrimId,VOS_UINT8 *pucData, VOS_UINT16 usLen);

extern unsigned int MNTN_ErrorLog(char * cFileName, unsigned int ulFileId, unsigned int ulLine,
                    unsigned int ulErrNo, void *pRecord, unsigned int ulLen);

extern VOS_UINT32 OM_Acpu_WriteLogFile(VOS_CHAR * cFileName, void *pRecord, VOS_UINT32 ulLen);

extern VOS_UINT32 OM_AcpuGetLogPath(VOS_CHAR *pucBuf, VOS_CHAR *pucOldPath, VOS_CHAR *pucUnitaryPath);

extern VOS_VOID OM_AcpuInitAuthVariable(VOS_VOID);
#endif //(FEATURE_OFF == FEATURE_MERGE_OM_CHAN)
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

