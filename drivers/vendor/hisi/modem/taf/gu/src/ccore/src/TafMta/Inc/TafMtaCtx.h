

#ifndef __TAFMTACTX_H__
#define __TAFMTACTX_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "TafMtaTimerMgmt.h"
#include "AtMtaInterface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/
/* 等待消息结果的队列长度。由于起定时器时，会同时保存消息，故该值与TAF_MTA_CTX_MAX_TIMER_NUM一致 */
#define TAF_MTA_MAX_CMD_BUFFER_QUEUE_SIZE                   (5)

#define TAF_MTA_REFCLOCK_DEFAULT_FREQ                       (19200000)
#define TAF_MTA_REFCLOCK_DEFAULT_PRECISION                  (100)

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

typedef struct
{
    VOS_CHAR                           *pcBufHead;                              /* 存放XML码流内存的首地址 */
    VOS_CHAR                           *pcBufCur;                               /* 当前空闲内存的首地址 */
}TAF_MTA_XML_TEXT_STRU;


typedef struct
{
    AT_MTA_CMD_RPT_FLG_ENUM_UINT8       enRptFlg;                               /* 命令上报开关标志 */
    VOS_UINT8                           aucReserved1[3];                        /* 保留位 */
    VOS_UINT32                          ulFreq;                                 /* GPS参考时钟的频率值，单位Hz */
    VOS_UINT32                          ulPrecision;                            /* 当前GPS参考时钟的精度，单位ppb */
    PHY_MTA_REFCLOCK_STATUS_ENUM_UINT16 enStatus;                               /* 时钟频率锁定状态 */
    VOS_UINT8                           aucReserved2[2];                        /* 保留位 */
} TAF_MTA_REFCLOCK_INFO_STRU;


typedef struct
{
    TAF_MTA_XML_TEXT_STRU               stXmlText;                              /* 存放XML码流的内存 */
    TAF_MTA_REFCLOCK_INFO_STRU          stRefClockInfo;                         /* GPS参考时钟状态信息 */
}TAF_MTA_AGPS_CTX_STRU;


typedef struct
{
    TAF_MTA_TIMER_ID_ENUM_UINT32        enTimerId;
    VOS_UINT8                          *pucMsgInfo;
    VOS_UINT32                          ulMsgInfoLen;
}TAF_MTA_CMD_BUFFER_STRU;


typedef struct
{
    VOS_UINT16                          usNewMipiClk;                           /* 保存最新的MIPICLK值 */
    VOS_UINT16                          usReportMipiClk;                        /* 保存已经上报过的的MIPICLK值 */
    VOS_UINT32                          ulTimerInterVal;                        /* 保存MIPICLK上报的时间间隔 */
    VOS_UINT16                          usEnableBitMap;                         /* 使能开关，每个bit代表不同的算法，非0代表使能 */
    VOS_UINT8                           aucReserved2[2];                        /* 保留位 */
}TAF_MTA_RF_LCD_CTX_STRU;
typedef struct
{
    /* 记录AGPS XML码流的上下文资源 */
    TAF_MTA_AGPS_CTX_STRU           stAgpsCtx;

    /* MTA当前正在运行的定时器资源 */
    TAF_MTA_TIMER_CTX_STRU          astTimerCtx[TAF_MTA_CTX_MAX_TIMER_NUM];

    /* MTA命令消息缓存 */
    TAF_MTA_CMD_BUFFER_STRU         astCmdBufferQueue[TAF_MTA_MAX_CMD_BUFFER_QUEUE_SIZE];

    TAF_MTA_RF_LCD_CTX_STRU         stMtaRfLcdCtx;

}TAF_MTA_CONTEXT_STRU;

/*****************************************************************************
  8 UNION定义
*****************************************************************************/
/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/
/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern TAF_MTA_AGPS_CTX_STRU*  TAF_MTA_GetMtaAgpsCtxAddr( VOS_VOID );

extern TAF_MTA_CONTEXT_STRU*  TAF_MTA_GetMtaCtxAddr( VOS_VOID );

extern TAF_MTA_TIMER_CTX_STRU*  TAF_MTA_GetTimerCtxAddr( VOS_VOID );

extern VOS_VOID  TAF_MTA_InitCmdBufferQueue(
            TAF_MTA_CMD_BUFFER_STRU            *pstCmdBufferQueue);

extern TAF_MTA_CMD_BUFFER_STRU  *TAF_MTA_GetCmdBufferQueueAddr( VOS_VOID );

extern VOS_VOID  TAF_MTA_DelItemInCmdBufferQueue(
            TAF_MTA_TIMER_ID_ENUM_UINT32        enTimerId);

extern VOS_VOID  TAF_MTA_SaveItemInCmdBufferQueue(
            TAF_MTA_TIMER_ID_ENUM_UINT32        enTimerId,
            VOS_UINT8                          *pucMsgInfo,
            VOS_UINT32                          ulMsgInfoLen);

extern TAF_MTA_CMD_BUFFER_STRU  *TAF_MTA_GetItemFromCmdBufferQueue(
            TAF_MTA_TIMER_ID_ENUM_UINT32                enTimerId);


extern VOS_VOID TAF_MTA_InitRefClockInfo(
    TAF_MTA_REFCLOCK_INFO_STRU         *pstRefClockInfo
);

extern TAF_MTA_REFCLOCK_INFO_STRU* TAF_MTA_GetRefClockInfo(VOS_VOID);

VOS_VOID  TAF_MTA_InitRfLcdIntrusionCtx(
    TAF_MTA_RF_LCD_CTX_STRU            *pstRfLcdCtx
);
TAF_MTA_RF_LCD_CTX_STRU* TAF_MTA_GetRfLcdCtxAddr( VOS_VOID );




#if ((VOS_OS_VER == VOS_WIN32) || (VOS_OS_VER == VOS_NUCLEUS))
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


