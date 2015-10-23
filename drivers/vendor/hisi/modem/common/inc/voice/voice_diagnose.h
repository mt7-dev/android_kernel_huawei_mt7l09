

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include "om_log.h"
#include "CodecInterface.h"


#ifndef __VOICE_DIAGNOSE_H__
#define __VOICE_DIAGNOSE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define VOICE_DIAG_CHECK_POINT_NUM              (8)                             /* 共设置8个语音监测点 */
#define VOICE_DIAG_AMR_SID_NUM                  (8)                             /* 网络DTX打开时，AMR声码器下静音帧后紧跟的BAD帧数(包含SID帧在内) */
#define VOICE_DIAG_FR_EFR_SID_NUM               (24)                            /* 网络DTX打开时，FR/EFR声码器下静音帧后紧跟的BAD帧数(包含SID帧在内) */
#define VOICE_DIAG_HR_SID_NUM                   (12)                            /* 网络DTX打开时，HR声码器下静音帧后紧跟的BAD帧数(包含SID帧在内) */
#define VOICE_DIAG_TIME_INTERVAL                (2)                             /* 每2s检测一次 */
#define VOICE_DIAG_FRAME_NUM_PER_SECOND         (50)                            /* 1s钟有50帧语音数据 */
#define VOICE_DIAG_MAX_SILENCE_NUM              (3)                             /* 连续3次检测到静音则上报单通 */
#define VOICE_DIAG_GOOD_FRAME_PERCENT_THD       (26213)                         /* Q15 (0.8*32767) */
#define VOICE_DIAG_CHANNEL_QUALITY_LEN          (16)
#define VOICE_DIAG_GSM_RX_LEVEL_OFFSET          (316)

#define VOICE_DIAG_REPORT_LINEIN                (0)
#define VOICE_DIAG_REPORT_LINEOUT               (1)
#define VOICE_DIAG_REPORT_MICIN                 (2)

/* 获取语音质量检测对象属性 */
#define VOICE_DiagGetCfgPtr()                   (&g_stVoiceDiagCfg)
#define VOICE_DiagGetFrameNumPtr()              (&g_stDiagFrameNum)
#define VOICE_DiagGetDLSilenceNum()             (g_uwDiagDLSilenceNum)
#define VOICE_DiagSetDLSilenceNum(uwValue)      (g_uwDiagDLSilenceNum = uwValue)
#define VOICE_DiagAddDLSilenceNum()             (g_uwDiagDLSilenceNum++)
#define VOICE_DiagGetReportFlag(uwValue)        (g_uhwVoiceDiagReportFalg[uwValue])
#define VOICE_DiagGetChannelQualityPtr()        (&g_stChannelQuality)
/*****************************************************************************
  3 枚举定义
*****************************************************************************/
/* 通路号枚举 */
enum VOICE_DIAG_MODEM_NUM_ENUM
{
    VOICE_DIAG_MODEM0              = 0,                 /* MODEM-0 */
    VOICE_DIAG_MODEM1,                                  /* MODEM-1 */
    VOICE_DIAG_MODEM_NUM_BUTT
};
typedef VOS_UINT16  VOICE_DIAG_MODEM_NUM_ENUM_UINT16;

/*****************************************************************************
  4 消息头定义
*****************************************************************************/


/*****************************************************************************
  5 消息定义
*****************************************************************************/


/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/
typedef struct
{
    VOS_UINT16              uhwGoodFrameNum;
    VOS_UINT16              uhwTotalFrameNum;
} VOICE_DIAG_FRAMENUM_STRU;

/* 下行LineIn处语音数据帧信息 */
typedef struct
{
    VCVOICE_DEV_MODE_ENUM_UINT16        enDevMode;                              /* 语音设备类型 */
    CODEC_NET_MODE_ENUM_UINT16          enNetMode;
    CODEC_ENUM_UINT16                   enCodecType;                            /* 声码器类型: 0-AMR, 1-EFR, 2-FR, 3-HR */
    VOS_UINT16                          usIsEncInited;                          /*编码器是否已初始化*/
    VOS_UINT16                          usIsDecInited;                          /*解码器是否已初始化*/
    CODEC_BFI_ENUM_UINT16               enBfi;                                  /* 坏帧标志,HR/FR/EFR使用 0 dis 1 enable,bad*/
    CODEC_SID_ENUM_UINT16               enSid;                                  /* HR/FR/EFR使用 */
    CODEC_AMR_TYPE_RX_ENUM_UINT16       enAmrFrameType;                         /* AMR帧类型, TX_SPEECH_GOOD/TX_SID_FIRST/TX_SID_UPDATA/TX_NO_DATA */
    CODEC_AMR_FQI_QUALITY_ENUM_UINT16   enQualityIdx;                           /* W/TD下帧类型 */
    VOS_UINT16                          uhwReserve;
} VOICE_DIAG_RX_DATA_STRU;

typedef struct
{
    VOS_UINT16 uhwIsBadCell;                                                    /* 当前小区信号是否很差 */
    VOS_UINT16 uhwReserve;
    VOS_UINT32 auwChannelQuality[VOICE_DIAG_CHANNEL_QUALITY_LEN];
}VOICE_DIAG_CHANNEL_QUALITY_STRU;
/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/
extern VOICE_DIAG_FRAMENUM_STRU   g_stDiagFrameNum;
extern VOICE_DIAG_NV_STRU         g_stVoiceDiagCfg;
extern VOS_UINT32                 g_uwDiagDLSilenceNum;
extern VOS_UINT16                 g_uhwVoiceDiagReportFalg[VOICE_DIAG_MODEM_NUM_BUTT][VOICE_DIAG_CHECK_POINT_NUM];
extern VOICE_DIAG_CHANNEL_QUALITY_STRU g_stChannelQuality;
/*****************************************************************************
  10 函数声明
*****************************************************************************/

extern VOS_VOID VOICE_DiagCalcFrameNum(VOICE_DIAG_RX_DATA_STRU *pstRxData);
extern VOS_VOID VOICE_DiagInit(VOICE_DIAG_NV_STRU *pstVoiceDiagCfg);
extern VOS_VOID VOICE_DiagLineIn(
                       VOICE_DIAG_MODEM_NUM_ENUM_UINT16 enActiveModemNo,
                       VOICE_DIAG_RX_DATA_STRU         *pstRxData);
extern VOS_VOID VOICE_DiagReset( VOS_VOID );
extern VOS_VOID VOICE_DiagMsgReport(
                       VOS_UINT16                               uhwCheckPoint,
                       VOS_UINT16                               uhwModemNo,
                       HIFI_ERROR_EVENT_ONE_WAY_NO_SOUND_STRU  *pstOneWayNoSound);

extern VOS_VOID VOICE_DiagGsmGoodFrmNum( VOICE_DIAG_RX_DATA_STRU *pstRxData );






#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of voice_diagnose.h */
