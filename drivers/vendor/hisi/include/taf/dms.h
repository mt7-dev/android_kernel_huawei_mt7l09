

/******************************************************************************
 PROJECT   :
 SUBSYSTEM :
 MODULE    :
 OWNER     :
******************************************************************************/




#ifndef __DMS_H__
#define __DMS_H__

#include "DrvInterface.h"
#include "vos.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/* 以下的枚举值不能随便修改*/
typedef enum
{
    EN_DMS_BEARER_TCP_20248 = 0,
    EN_DMS_BEARER_TCP_20249 ,
    EN_DMS_BEARER_UDP,
    EN_DMS_BEARER_ETH_UDP,
    EN_DMS_BEARER_USB_EP,
    EN_DMS_BEARER_USB_COM1_DIAG_CTRL,
    EN_DMS_BEARER_USB_COM2_DIAG_APP,
    EN_DMS_BEARER_USB_COM3_GU,
    EN_DMS_BEARER_USB_COM4_AT,
    EN_DMS_BEARER_USB_COM_CTRL,
    EN_DMS_BEARER_UART,
    EN_DMS_BEARER_USB_NCM,
    EN_DMS_BEARER_LAST
} DMS_PHY_BEAR_ENUM;

typedef enum
{
    EN_DMS_CHANNEL_DIAG = 0,            /* channel identifier, DIAG
 */
    EN_DMS_CHANNEL_AT,                  /* channel identifier, AT
 */
    EN_DMS_CHANNEL_GU,
    EN_DMS_CHANNEL_LAST
} DMS_LOGIC_CHAN_ENUM;


typedef enum
{
    EN_STATIC_BUF_STA_IDLE = 0,
    EN_STATIC_BUF_STA_BUSY,
    EN_STATIC_BUF_STA_BUTT
} ENUM_DMS_BUF_STATUS;

typedef enum
{
    EN_DMS_DYMIC_BUF_TYP = 0,
    EN_DMS_STATIC_BUF_TYP,
    EN_DMS_BUF_BUTT
} ENUM_DMS_BUF_TYP;

typedef enum
{
    EN_DMS_AT_CLIENT_ACM  = 0x01,
    EN_DMS_AT_CLIENT_CTRL = 0x41,
    EN_DMS_AT_CLIENT_UART,
    EN_DMS_AT_CLIENT_NCM,
    EN_DMS_AT_CLIENT_TCP,
    EN_DMS_AT_CLIENT_LAST
} DMS_AT_CLIENT_ENUM;


typedef enum
{

    EN_SDM_DMS_VCOM1_OPEN,
    EN_SDM_DMS_VCOM1_OPEN_ERR,
    EN_SDM_DMS_VCOM2_OPEN,
    EN_SDM_DMS_VCOM2_OPEN_ERR,
    EN_SDM_DMS_VCOM_AT_OPEN,
    EN_SDM_DMS_VCOM_AT_OPEN_ERR,
    EN_SDM_DMS_VCOM_CTRL_OPEN,
    EN_SDM_DMS_VCOM_CTRL_OPEN_ERR,
    EN_SDM_DMS_VCOM1_CLOSE,
    EN_SDM_DMS_VCOM1_CLOSE_ERR,
    EN_SDM_DMS_VCOM2_CLOSE,
    EN_SDM_DMS_VCOM2_CLOSE_ERR,
    EN_SDM_DMS_VCOM_AT_CLOSE,
    EN_SDM_DMS_VCOM_AT_CLOSE_ERR,
    EN_SDM_DMS_VCOM_CTRL_CLOSE,
    EN_SDM_DMS_VCOM_CTRL_CLOSE_ERR,
    EN_SDM_DMS_INIT,
    EN_SDM_DMS_INIT_ERR,
    EN_SDM_DMS_DISABLE,
    EN_SDM_DMS_DISABLE_ERR,
    EN_SDM_DMS_VCOM1_EVT,
    EN_SDM_DMS_VCOM2_EVT,
    EN_SDM_DMS_VCOM_AT_EVT,
    EN_SDM_DMS_VCOM1_WRT,
    EN_SDM_DMS_VCOM1_WRT_SUCC,
    EN_SDM_DMS_VCOM1_WRT_CB,
    EN_SDM_DMS_VCOM1_WRT_CB_ERR,
    EN_SDM_DMS_VCOM2_WRT,
    EN_SDM_DMS_VCOM2_WRT_SUCC,
    EN_SDM_DMS_VCOM2_WRT_CB,
    EN_SDM_DMS_VCOM2_WRT_CB_ERR,
    EN_SDM_DMS_VCOM_AT_WRT,
    EN_SDM_DMS_VCOM_AT_WRT_SUSS,
    EN_SDM_DMS_VCOM_CTRL_WRT,
    EN_SDM_DMS_VCOM_CTRL_WRT_SUSS,
    EN_SDM_DMS_VCOM_AT_WRT_CB,
    EN_SDM_DMS_VCOM_AT_WRT_CB_ERR,
    EN_SDM_DMS_VCOM_CTRL_WRT_CB,
    EN_SDM_DMS_VCOM_CTRL_WRT_CB_ERR,
    EN_SDM_DMS_DECODE_PACKET_SEND,
    EN_SDM_DMS_DECODE_PACKET_SEND_FAIL,
    EN_SDM_DMS_NCM_SEND_TO_DRV,
    EN_SDM_DMS_NCM_SEND_TO_DRV_SUCC,

    EN_SDM_DMS_UDI_READ_START,
    EN_SDM_DMS_UDI_READ_END,

    EN_SDM_DMS_SOCKET_RECV_ERROR,
    EN_SDM_DMS_SOCKET_SEND_ERROR,

    EN_SDM_DMS_SOCKET_ACP,
    EN_SDM_DMS_SOCKET_CLOSE,
    EN_SDM_DMS_SOCKET_RECV,
    EN_SDM_DMS_SOCKET_SEND,
    EN_SDM_DMS_TCP_RECONNECT,
    EN_SDM_DMS_TCP_RECONNECT_ERR,
    EN_SDM_DMS_SOCKET_CLOSE_FAIL,

    EN_SDM_DMS_DEBUG_INFO_MAX
} DMS_DEBUG_MSG_ID_ENUM;

typedef struct
{
    VOS_UINT32  ulCalledNum;  /* 调用次数或者消息次数,或者表明该函数是否被调用
 */
    VOS_UINT32 ulRserved1;   /* 表明该位置或者其他,HAL_DIAG_SYS_DEBUG_ENUM决定
 */
    VOS_UINT32 ulRserved2;   /* 表明特殊含义或者其他,HAL_DIAG_SYS_DEBUG_ENUM决定
 */
    VOS_UINT32 ulRserved3;   /* 表明特殊含义或者其他,HAL_DIAG_SYS_DEBUG_ENUM决定
 */
    VOS_UINT32 ulRtcTime;    /* 表明最近执行过的时间
 */
} DMS_DEBUG_INFO_TBL_STRU;

#define NVE_AT_SERVER_INDEX         (294)
#define NVE_READ_OPERATE            (1)
#define NVE_WRITE_OPERATE           (0)

typedef struct
{
    VOS_INT32  lOperatRet;    /* 0:读取成功     -1:读取失败 */
    VOS_UINT32 ulNvValue;     /* 0:无atserver   1:有atserver */
} DMS_DEBUG_AT_SERV_NV_STRU;

extern VOS_VOID dms_Debug(DMS_DEBUG_MSG_ID_ENUM ulType,\
                     VOS_UINT32 ulRserved1, VOS_UINT32 ulRserved2, VOS_UINT32 ulRserved3);


#define DMS_DEBUG_SDM_FUN(enType,ulRserved1,ulRserved2,ulRserved3)  \
dms_Debug(enType,ulRserved1,ulRserved2,ulRserved3)


typedef struct
{
    DMS_PHY_BEAR_ENUM aenPhyChan;
    DMS_AT_CLIENT_ENUM enAtClientID;
    VOS_BOOL bRegister;                             /*TRUE为已经完成注册，FALSE 还未注册
 */
} DMS_PHY_CLIENT_TABLE_STRU;


#define DMS_LOG_STATIC_BUF_NUM          (16)

#define DMS_LOG_STATIC_ONE_BUF_SIZE     (5120)

/* 通道断开连接通知机制
 */
#define DMS_DISNNECT      0
#define DMS_CONNECT       1

typedef VOS_UINT32 (*DMS_READ_DATA_PFN)(DMS_AT_CLIENT_ENUM enClientId, VOS_UINT8 * pDataBuf, VOS_UINT32 ulLen);

typedef VOS_VOID (*DMS_CONNECT_STA_PFN)(VOS_UINT8 flag);

typedef struct
{
    VOS_UINT8* buf;
    ENUM_DMS_BUF_STATUS enBusy;
} DMS_BUF_STA_STRU;


typedef struct
{
    ENUM_DMS_BUF_TYP enBufType;
    DMS_BUF_STA_STRU stBufSta[DMS_LOG_STATIC_BUF_NUM];
} DMS_STATIC_BUF_STRU;

enum PDP_STATUS_ENUM
{
    PDP_STATUS_DEACT                         = 0,
    PDP_STATUS_ACT                           = 1,
    PDP_STATUS_BUTT
};
typedef unsigned long  PDP_STATUS_ENUM_UINT32;

/* PDP状态上报函数使用的结构体*/
typedef struct
{
    unsigned long                    ulspeed;             /* 连接速度，单位bps */
    PDP_STATUS_ENUM_UINT32           enActiveSatus;       /* 激活结果，0为成功，其他为失败 */
} NAS_PRO_STRU;



extern VOS_UINT32 dms_WriteData(DMS_LOGIC_CHAN_ENUM enDmsLogicChan,VOS_UINT8 ucQueId, VOS_UINT8* aucDataBuf, VOS_UINT32 usLen);
extern VOS_VOID dms_Init(VOS_VOID);
extern VOS_VOID dms_VcomCtrlAtTask(VOS_VOID);
extern VOS_VOID dms_AtPcuiTask(VOS_VOID);
extern  VOS_VOID  Dms_FreeStaticBuf( VOS_UINT8 * buf);
extern   VOS_UINT8* Dms_GetStaticBuf(VOS_UINT32 ulLen);
extern  VOS_BOOL Dms_IsStaticBuf(VOS_UINT8 *buf);
extern  VOS_VOID Dms_StaticBufInit(VOS_VOID);
extern  VOS_VOID dms_DiagVcomTask(VOS_VOID);

typedef int (*USB_NAS_BRK)
(
    void
);

typedef int (*USB_NAS_AT_CMD_RECV)
(
    unsigned char *pBuff,   /*指向AT命令指针*/
    unsigned short usLen    /*AT命令长度*/
);

typedef unsigned long (*USB_NAS_GET_WWAN_MODE)	/*获取当前网络模式cdma/wcdma*/
(
    void
);

#define DMS_COM_STATUS_CALLBACK_REGI(uPortNo,pCallback)   NULL

extern VOS_INT32 DMS_ACMRecvFuncReg(pComRecv pCallback);

#define DMS_COM_RCV_CALLBACK_REGI(uPortNo,pCallback) DMS_ACMRecvFuncReg(pCallback)

extern int DMS_NCMExtFuncReg(USB_NAS_BRK connectBrk, USB_NAS_AT_CMD_RECV atCmdRcv);

#define DMS_USB_NAS_REGFUNC(brkFunc, atRecvFunc, getWWANMode)\
                  DMS_NCMExtFuncReg(brkFunc, atRecvFunc)

extern VOS_INT32 Dms_WriteAtData(VOS_UINT8 ucPortNo, VOS_UINT8* pData, VOS_UINT16 uslength);

#define DMS_COM_SEND(uPortNo,pData,uslength) Dms_WriteAtData(uPortNo,pData,uslength)

extern signed long DMS_NCMStatusChangeReg(NAS_PRO_STRU * pPdpStru);

#define DMS_USB_NAS_STATUS_CHANGE(pPdpStru) DMS_NCMStatusChangeReg(pPdpStru)

extern VOS_VOID Dms_NcmProcCbReg(USBNdisStusChgFunc connectBrk,USB_NAS_AT_CMD_RECV atCmdRcv);


extern VOS_VOID dmsAtPcuiTaskSetSleepFlag(VOS_BOOL ucSleepFlag);
extern VOS_BOOL dmsAtPcuiTaskGetSleepFlag(VOS_VOID);
extern VOS_VOID dmsAtCtrlTaskSetSleepFlag(VOS_BOOL ucSleepFlag);
extern VOS_BOOL dmsAtCtrlTaskGetSleepFlag(VOS_VOID);

extern VOS_UINT32 dms_UdiRead(DMS_PHY_BEAR_ENUM enCOM, VOS_UINT8 * ucRedBuf, VOS_UINT32 ulToReadSize, VOS_UINT32 * pulRealySize);
extern VOS_BOOL dms_GetPortHandle(DMS_PHY_BEAR_ENUM enChan);
extern VOS_UINT32 dmsVcomAtPcuiOpen(VOS_VOID);
extern VOS_UINT32 dmsVcomCtrolOpen(VOS_VOID);
extern VOS_UINT32 dmsVcomAtPcuiClose(VOS_VOID);
extern VOS_UINT32 dmsVcomCtrlClose(VOS_VOID);

extern VOS_UINT32 dms_NcmCfg(VOS_VOID);
extern VOS_UINT32 dms_NcmClose(VOS_VOID);
extern VOS_UINT32 dms_NcmSendData(VOS_UINT8 *pData, VOS_UINT32 ulLen);
extern VOS_UINT32 dms_VcomWriteAsync(VOS_UINT32 VcomId ,VOS_UINT8 *pucDataBuf,VOS_UINT32 ulLen);
extern VOS_UINT32 writeAtData(VOS_UINT32 ulClientId, VOS_UINT8 *aucDataBuf, VOS_UINT32 ulLen);

extern DMS_CONNECT_STA_PFN dmsGetConnStaFun(VOS_VOID);
extern VOS_VOID dms_UsbDisableEvtProc(VOS_VOID);
extern VOS_VOID dms_UsbEnableEvtProc(VOS_VOID);
extern DMS_READ_DATA_PFN dmsGetReadFun(VOS_VOID);
extern VOS_VOID dms_SetConnectStaCallback(DMS_CONNECT_STA_PFN pfnReg);


/*****************************************************************************
 函 数 名  : DMS_OpenHsicPort
 功能描述  : C51版本HSIC 可维可测通道打开接口，提供给GU模的OM模块使用
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 0-成功;其他-失败
 ****************************************************************************/
extern VOS_UINT32 DMS_OpenHsicPort(VOS_VOID);


/*****************************************************************************
 函 数 名  : DMS_GetHsicPortStatus
 功能描述  : 获取HSIC口打开状态
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 0-未打开状态;1-打开状态
 ****************************************************************************/
extern VOS_UINT32 DMS_GetHsicPortStatus(VOS_VOID);


extern VOS_VOID Dms_SetNdisChanStatus(ACM_EVT_E enStatus );


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif


#endif /*__DMS_H__*/

