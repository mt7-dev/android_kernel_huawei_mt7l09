/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : VOICE_mc.h
  版 本 号   : 初稿
  作    者   : 谢明辉 58441
  生成日期   : 2011年5月6日
  最近修改   :
  功能描述   : VOICE_mc.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2011年5月6日
    作    者   : 谢明辉 58441
    修改内容   : 创建文件

******************************************************************************/
#ifndef __VOICE_MC_H__
#define __VOICE_MC_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "codec_typedefine.h"
#include "HifiOmInterface.h"
#include "OmCodecInterface.h"
#include "VcCodecInterface.h"
#include "med_drv_timer_hifi.h"
#include "voice_proc.h"
#include "med_drv_dma.h"

#include <stdlib.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/
typedef VOS_UINT32 (*VOICE_MC_CALLBACK)(
                VOS_INT16 *pshwTx,
                VOS_INT16 *pshwRx,
                VOS_UINT32 uwSmpRate);                      /* 通知hifi录音通道更新buff的回调函数 */

#define VOICE_TX_TO_RX_FRAME_THD        (3)                 /*下行相对上行丢帧检测帧数差别阈值*/

#define VOICE_MC_DMAC_CHN_FR_RING_BUF   (0)
#define VOICE_MC_DMAC_CHN_TO_RING_BUF   (1)

#ifdef _MED_ERRORLOG
#define VOICE_ERRLOG_FQI_QUALITY_BAD    (0)
#endif

#define VOICE_UMTS_TX_BEGIN_TIMESTAMP   (130)                /*UMTS下偶数帧12ms中断距离奇数帧头的间隔，单位0.1ms*/
#define VOICE_UMTS_RX_BEGIN_TIMESTAMP   (100)                /*UMTS下偶数帧11ms中断距离奇数帧头的间隔，单位0.1ms*/
#define VOICE_TDS_TX_BEGIN_TIMESTAMP    (80)                 /*TDS-CDMA下偶数帧上行中断距离偶数帧头的间隔，单位0.1ms*/
#define VOICE_TDS_RX_BEGIN_TIMESTAMP    (20)                 /*TDS-CDMA下偶数帧下行中断距离偶数帧头的间隔，单位0.1ms*/
#define VOICE_IMS_TX_BEGIN_TIMESTAMP    (40)                 /*IMS下偶数帧5ms中断距离奇数帧头的间隔，单位0.1ms*/
#define VOICE_IMS_RX_BEGIN_TIMESTAMP    (50)                 /*IMS下偶数帧4ms中断距离奇数帧头的间隔，单位0.1ms*/

#define VOICE_GSM_TX_DMA_TIMESTAMP      (120)                                   /*GSM上行起DMA的时间，单位0.1ms */
#define VOICE_GSM_RX_DMA_TIMESTAMP      (180)                                   /*GSM下行起DMA的时间，单位0.1ms */

#define VOICE_GSM_QB_PARA               (4615)                                  /*GSM下通过QB计算时间的参数，单位0.1ms(原单位为μs，所以要额外除以100)*/
#define VOICE_GSM_MIN_OFFSET            (60)                                    /*GSM下时隙的最大偏移，单位0.1ms*/
#define VOICE_GSM_MAX_OFFSET            (170)                                   /*GSM下上报数据距GPHY要求上报时间的左向最大偏移，单位0.1ms 4*46*/
#define VOICE_GSM_ONE_FRAME_OFFSET      (200)                                   /*GSM的帧长(单位0.1ms)*/

#define VOICE_SLOT_NUMBER_BY_FRAME      (15)                 /*UMTS或TD下每帧的slot数*/
#define VOICE_CHIP_NUMBER_BY_FRAME      (38400)              /*UMTS或TD下每帧的chip数*/
#define VOICE_CHIP_NUMBER_BY_SLOT       (2560)               /*UMTS或TD下每时隙的chip数*/
#define VOICE_UMTS_MAX_ENC_OFFSET       (195)                /*UMTS下偶数帧上报帧类型最晚时刻距离上个奇数帧帧头的时间,单位0.1ms */
#define VOICE_TDS_MAX_ENC_OFFSET        (155)                /*TDS-CDMA下奇数帧上报帧类型最晚时刻距离上个偶数帧帧头的时间,单位0.1ms */
#define VOICE_TIMESTAMP_MS_RATIO        (10000)              /*1s与0.1ms的转换*/
#define VOICE_LAST_FRAME_OFFSET         (300)                /*与缓存的前一帧时间差，单位0.1ms*/

#define VOICE_MC_MAX_PRIM               (26)                 /*最大原语处理函数数*/
#define VOICE_MC_MSG_BUF_MAX_LEN        (10)                 /*缓存消息最大长度  */

/*用来在SIO和DTCM间搬运数据的DMA通道号*/
#define VOICE_MC_DMAC_CHN_MIC           (DRV_DMA_GetSioTxChn())
#define VOICE_MC_DMAC_CHN_SPK           (DRV_DMA_GetSioRxChn())

#define VOICE_SMART_PA_L_CHAN_VALID     (0)                  /* SMART PA的输出I2S数据中，左声道为有效数据 */
#define VOICE_SMART_PA_R_CHAN_VALID     (1)                  /* SMART PA的输出I2S数据中，右声道为有效数据 */

#define VOICE_McGetMcObjsPtr()          (&g_stVoiceMcObjs)
#define VOICE_McGetInoutPtr()           (&g_stVoiceMcObjs.stInout)                     /* 获取当前上下行输入输出，如手持、免提、PC VOICE等 */
#define VOICE_McGetDmaFlagPtr()         (&g_stVoiceMcObjs.stDMAFlag)
#define VOICE_McGetTimerPtr()           (&g_stVoiceMcObjs.stTimer)
#define VOICE_McGetDeviceMode()         (g_stVoiceMcObjs.enDevMode)                    /* 获取语音设备类型 */
#define VOICE_McSetDeviveMode(uhwDevMode)  (g_stVoiceMcObjs.enDevMode = uhwDevMode)    /* 设置语音设备类型 */
#define VOICE_McGetCaptureCallBackfunc()   (g_stVoiceMcObjs.pfunCallBack)                 /* 获取通知hifi录音通道更新buff的回调函数 */
#define VOICE_McSetCaptureCallBack(pVar) (g_stVoiceMcObjs.pfunCallBack = pVar)         /* 设置通知hifi录音通道更新buff的回调函数 */
#define VOICE_McGetModemObjPtr(uhwModemNo) &(g_stVoiceMcObjs.stModemMcObj[uhwModemNo]) /* 获取通路对象 */
#define VOICE_McGetForeGroundNum()      (g_stVoiceMcObjs.enActiveModemNo)           /* 获取前台通路号 */
#define VOICE_McSetForeGroundNum(uhwVar) \
                (g_stVoiceMcObjs.enActiveModemNo = uhwVar)                             /* 获取前台通路号 */
#define VOICE_McGetModemVoiceState(uhwModemNo) \
                (g_stVoiceMcObjs.stModemMcObj[uhwModemNo].enState)                     /* 获取语音状态 */
#define VOICE_McSetVoiceState(uhwModemNo, uhwVar) \
                (g_stVoiceMcObjs.stModemMcObj[uhwModemNo].enState = uhwVar)            /* 设置语音状态 */
#define VOICE_McGetModemNetMode(uhwModemNo) \
                (g_stVoiceMcObjs.stModemMcObj[uhwModemNo].enNetMode)                   /* 获取网络模式 */
#define VOICE_McGetModemCodecPtr(uhwModemNo) \
                (&g_stVoiceMcObjs.stModemMcObj[uhwModemNo].stCodec)                    /* 获取编解码对象指针 */
#define VOICE_McSetModemNetMode(uhwModemNo, uhwVar) \
                (g_stVoiceMcObjs.stModemMcObj[uhwModemNo].enNetMode = uhwVar)          /* 设置网络模式 */
#define VOICE_McSetGsmCodecTypeMismatch(uhwModemNo, uhwVar) \
                (g_stVoiceMcObjs.stModemMcObj[uhwModemNo].uhwGsmCodecTypeMismatch = uhwVar) /* GSM声码器类型是否需要更新而没有更新 */
#define VOICE_McGetGsmCodecTypeMismatch(uhwModemNo) \
                (g_stVoiceMcObjs.stModemMcObj[uhwModemNo].uhwGsmCodecTypeMismatch)     /* 获取GSM声码器类型是否需要更新而没有更新 */
#define VOICE_McGetModemState(uhwModemNo) \
                (g_stVoiceMcObjs.stModemMcObj[uhwModemNo].enModemState)                 /* 获取通路状态 */
#define VOICE_McSetModemState(uhwModemNo, uhwVar) \
                (g_stVoiceMcObjs.stModemMcObj[uhwModemNo].enModemState = uhwVar)        /* 设置通路状态 */

#define VOICE_McGetSmartPaEn()          (g_stVoiceMcObjs.stSmartPaCfg.enEnable)         /* Smart PA是否使能 */
#define VOICE_McGetSmartPaValidChan()   (g_stVoiceMcObjs.stSmartPaCfg.uhwValidChannel)  /* Smart PA Valid Chn */
#define VOICE_McGetSmartPaMaster()      (g_stVoiceMcObjs.stSmartPaCfg.uhwIsMaster)      /* Smart PA Is Master Or Slave */

/*****************************************************************************
  3 枚举定义
*****************************************************************************/
/* 运行状态枚举 */
enum VOICE_MC_STATE_ENUM
{
    VOICE_MC_STATE_IDLE              = 0,          /* IDLE状态 */
    VOICE_MC_STATE_RUNNING,                        /* RUNNING状态 */
    VOICE_MC_STATE_PAUSE,                          /* 双模切换中挂起态 */
    VOICE_MC_STATE_BUTT
};
typedef VOS_UINT16  VOICE_MC_STATE_ENUM_UINT16;

/* 通路操作枚举 */
enum VOICE_MC_MODEM_OPT_ENUM
{
    VOICE_MC_FOREGROUND_OPT   = 0,                  /* 退出静默 */
    VOICE_MC_BACKGROUND_OPT,                        /* 进入静默 */
    VOICE_MC_MODEM_OPT_BUTT
};
typedef VOS_UINT16  VOICE_MC_MODEM_OPT_ENUM_UINT16;

/* 通路状态枚举 */
enum VOICE_MC_MODEM_STATE_ENUM
{
    VOICE_MC_MODEM_STATE_FOREGROUND   = 0,            /* 前台状态,与VOICE_MC_FOREGROUND_OPT对应 */
    VOICE_MC_MODEM_STATE_BACKGROUND,                  /* 后台状态,与VOICE_MC_BACKGROUND_OPT对应 */
    VOICE_MC_MODEM_STATE_UNSET,                       /* 未设置状态 */
    VOICE_MC_MODEM_STATE_BUTT
};
typedef VOS_UINT16  VOICE_MC_MODEM_STATE_ENUM_UINT16;

/* 通路号枚举 */
enum VOICE_MC_MODEM_NUM_ENUM
{
    VOICE_MC_MODEM0              = 0,                 /* MODEM-0 */
    VOICE_MC_MODEM1,                                  /* MODEM-1 */
    VOICE_MC_MODEM_NUM_BUTT
};
typedef VOS_UINT16  VOICE_MC_MODEM_NUM_ENUM_UINT16;

/* PID范围枚举 */
enum VOICE_MC_PID_RANGE_ENUM
{
    VOICE_MC_MODEM0_RANGE              = 0,            /* 与MODEM-0对应 */
    VOICE_MC_MODEM1_RANGE,                             /* 与MODEM-1对应 */
    VOICE_MC_CODEC_RANGE,
    VOICE_MC_OM_RANGE,
    VOICE_MC_PID_RANGE_NUM_BUTT
};
typedef VOS_UINT16  VOICE_MC_PID_RANGE_ENUM_UINT16;

/* PID转换表下标枚举 */
enum VOICE_MC_PIDTBL_INDEX_ENUM
{
    VOICE_MC_NAS_INDEX                = 0,              /* NAS的PID */
    VOICE_MC_GPHY_INDEX,                                /* G物理层的PID */
    VOICE_MC_WPHY_INDEX,                                /* W物理层的PID */
    VOICE_MC_TDPHY_INDEX,                               /* TD物理层的PID */
    VOICE_MC_WTTF_INDEX,                                /* WTTF的PID */
    VOICE_MC_TDMAC_INDEX,                               /* TdMac的PID */
    VOICE_MC_IMSA_INDEX,                                /* IMSA的PID */
    VOICE_MC_INDEX_BUTT
};
typedef VOS_UINT16  VOICE_MC_PIDTBL_INDEX_ENUM_UINT16;

/* EC Reference数据来源 */
enum VOICE_SMART_PA_EN_ENUM
{
    VOICE_SMART_PA_EN_DISABLE         = 0,                /* Smart PA不可用，HIFI下行处理数据 */
    VOICE_SMART_PA_EN_ENABLE,                             /* Smart PA可用，允许选择使用Smart PA最终输出数据 */
    VOICE_SMART_PA_EC_BUTT
};
typedef VOS_UINT16 VOICE_SMART_PA_EN_UINT16;

/*****************************************************************************
  4 消息头定义
*****************************************************************************/


/*****************************************************************************
  5 消息定义
*****************************************************************************/


/*****************************************************************************
 实体名称  : VOICE_VOICE_TX_PROC_REQ_STRU
 功能描述  : G/U上行处理指示，为DMA中断处理函数中发送至PID_VOICE的编码消息
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT16                          uhwMsgId;                               /*0xDD03*/ /*_H2ASN_Skip*/
    VOS_UINT16                          uhwReserved;
}VOICE_VOICE_TX_PROC_REQ_STRU;

/*****************************************************************************
 实体名称  : VOICE_VOICE_RX_PROC_REQ_STRU
 功能描述  : G/U下行处理指示，为DMA中断处理函数中发送至PID_VOICE的解码消息
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT16                          uhwMsgId;                               /*0xDD03*/ /*_H2ASN_Skip*/
    VOS_UINT16                          uhwReserved;
}VOICE_VOICE_RX_PROC_REQ_STRU;

/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/
/*定时器管理结构体*/
typedef struct
{
    VOS_UINT32                          uwTimer3ms;                             /*W偶数帧3ms定时器 */
    VOS_UINT32                          uwTimer6ms;                             /*W偶数帧6ms定时器 */
    VOS_UINT32                          uwTimer20ms;                            /*20ms定时器*/
    VOS_UINT32                          uwTimer20msRx;                          /*20ms定时器*/
    VOS_UINT32                          uwTimer01msTx;                          /*0.1ms定时器*/
    VOS_UINT32                          uwTimer01msRx;                          /*0.1ms定时器*/
    VOS_UINT32                          uwTdTxTimer;                            /*TD语音上行定时器*/
    VOS_UINT32                          uwTdRxTimer;                            /*TD语音下行定时器*/
}VOICE_MC_TIMER_STRU;

/*DMA搬运flag结构体*/
typedef struct
{
    VOS_UINT16                          usMicInRun;                             /* SIO->dTCM MIC_DMA工作指示  */
    VOS_UINT16                          usSpkOutRun;                            /* dTCM->SIO SPK_DMA工作指示  */
    VOS_UINT16                          usMicInSwEnable;                        /* SIO->dTCM MIC_DMA切换缓冲允许标志 */
    VOS_UINT16                          usSpkOutSwEnable;                       /* dTCM->SIO SPK_DMA切换缓冲允许标志 */
    VOS_UINT32                          uwSyncOffset;                           /* UMTS下时序同步时刻偏置,单位0.1ms */
    VOS_UINT32                          uwSyncTimestamp;                        /* UMTS下时序同步时刻系统时戳,单位1s/32767 */
    VOS_UINT32                          uwChanCodeTime;                         /* GPHY要求上报上行数据的时刻偏差，单位0.1ms */
}VOICE_MC_DMA_FLAG_STRU;

/*****************************************************************************
 实体名称  : VOICE_MC_MODEM_STRU
 功能描述  : 描述语音软件控制模块主要状态，区分 modem(两个modem特性不同)
*****************************************************************************/
typedef struct
{
    VOICE_MC_MODEM_STATE_ENUM_UINT16    enModemState;              /* 通路状态 */
    VOS_UINT16                          uhwGsmCodecTypeMismatch;  /* GSM声码器类型是否需要更新而没有更新 */
    VOICE_MC_STATE_ENUM_UINT16          enState;                  /* 语音软件运行状态 */
    /* 从PROC移入 */
    CODEC_NET_MODE_ENUM_UINT16          enNetMode;                /* 网络制式 */
    VOS_INT16                           shwVolumeTx;              /* 上行音量 */
    VOS_INT16                           shwVolumeRx;              /* 下行音量 */
    VOICE_PROC_CODEC_STRU               stCodec;                  /* 编解码对象 */
} VOICE_MC_MODEM_STRU;

/*****************************************************************************
 实体名称  : VOICE_MC_OBJS_STRU
 功能描述  : 描述语音软件控制模块主要状态，不区分 modem(两个modem共有的一些特性)
*****************************************************************************/
typedef struct
{
    VCVOICE_DEV_MODE_ENUM_UINT16        enDevMode;                /* 语音设备类型 */
    VOICE_MC_MODEM_NUM_ENUM_UINT16      enActiveModemNo;          /* 激活状态通道号 */

    CODEC_INOUT_STRU                    stInout;                  /* 上下行输入输出 */
    VOICE_MC_DMA_FLAG_STRU              stDMAFlag;                /* DMA搬运播放/录音数据标志*/
    VOICE_MC_TIMER_STRU                 stTimer;                  /* 定时器相关结构 */
    /* Modem 运行状态全局变量 */
    VOICE_MC_MODEM_STRU                 stModemMcObj[VOICE_MC_MODEM_NUM_BUTT];
    /* 回调函数，通知Hifi录音通道的Buff内容更新 */
    VOICE_MC_CALLBACK                   pfunCallBack;
    VOICE_SMART_PA_CFG_NV_STRU          stSmartPaCfg;
} VOICE_MC_OBJS_STRU;

/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/
extern VOICE_MC_OBJS_STRU            g_stVoiceMcObjs;

/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern VOS_UINT32 VOICE_McApiIsIdle(VOS_VOID);
extern VOS_VOID VOICE_McChkMicConflick(VOS_VOID);
extern VOS_VOID VOICE_McChkSpkConflick(VOS_VOID);
extern VOS_VOID VOICE_McDestory( );
extern VOS_UINT16 VOICE_McGetForeGroundNetMode( VOS_UINT16 uhwActiveModemNo );
extern VOICE_MC_MODEM_STRU* VOICE_McGetForeGroundObj(VOS_VOID);
extern VOICE_MC_STATE_ENUM_UINT16 VOICE_McGetForeGroundVoiceState(VOS_VOID);
extern VOS_UINT16 VOICE_McGetModemNum( VOS_UINT32 uwSenderPid );
extern CODEC_NET_MODE_ENUM_UINT16 VOICE_McGetNetMode(VOS_UINT32 uwSenderPid);
extern VOICE_MC_STATE_ENUM_UINT16 VOICE_McGetVoiceState(VOS_UINT32 uwSenderPid);
extern VOS_UINT32 VOICE_McHandleFsmErr(VOS_UINT16 uhwSenderPid, VOS_UINT16 uhwMsgId);
extern VOS_UINT32 VOICE_McHandleRtFsmErr(VOS_UINT16 uhwSenderPid, VOS_UINT16 uhwMsgId);
extern VOS_VOID VOICE_McInformGphySetCodec(VOS_UINT32 uwReceiverPid);
extern VOS_VOID VOICE_McInit(VOS_VOID);
extern VOS_VOID VOICE_McModemInformToPhy(
                       CODEC_NET_MODE_ENUM_UINT16 enNetMode,
                       VOICE_MC_MODEM_NUM_ENUM_UINT16 enModemNo,
                       VOICE_MC_MODEM_OPT_ENUM_UINT16 enModemOpt
                       );
extern VOS_VOID VOICE_McModemObjInit( VOICE_MC_MODEM_STRU *pstMcModemObj );
extern VOS_UINT32 VOICE_McPlay(VOS_VOID);
extern VOS_VOID VOICE_McPostCfgCodecType(VOS_UINT16 uhwCodecType,VOS_UINT16 uhwModemNo);
extern VOS_UINT32 VOICE_McPostTxProc( VOS_VOID );
extern VOS_UINT32 VOICE_McPreCfgCodecType(VOS_UINT16 uhwNetMode, VOS_UINT16 *puhwCodecType);
extern VOS_UINT32 VOICE_McPreTxProc( VOS_VOID );
extern VOS_UINT32 VOICE_McRecord(VOS_VOID);
extern VOS_UINT32 VOICE_McRegisterCaptureCallBack(VOICE_MC_CALLBACK pfunCallBack);
extern VOS_VOID VOICE_McResume(VCVOICE_TYPE_ENUM_UINT16 enCodecType, VOS_UINT16 uhwModemNo);
extern VOS_UINT32 VOICE_McRxPreProc(VOS_UINT16 *puhwRxFrmLost, VOS_INT16 *pshwVolumeRx, VOS_INT16 **ppshwDecData);
extern VOS_UINT32 VOICE_McRxProc(VOS_VOID);
extern VOS_VOID VOICE_McSendCnfMsgToVc(VOS_UINT16 uhwMsgId,VOS_UINT32 uwReceiverPid, VOS_UINT32 uwRet);
extern VOS_VOID VOICE_McSendModemMsgToGUPhy(
                       VOICE_MC_MODEM_OPT_ENUM_UINT16 enLinkOpt,
                       VOS_UINT32 uwReceiverPid,
                       VOS_UINT16 uhwMsgId);
extern VOS_VOID VOICE_McSendModemMsgToTDPhy(
                       VOICE_MC_MODEM_OPT_ENUM_UINT16 enLinkOpt,
                       VOS_UINT32                     uwReceiverPid,
                       VOS_UINT16                     uhwMsgId
                       );
extern VOS_VOID VOICE_McSendQryCnfMsgToVc(VOS_UINT16 uhwMsgId,VOS_UINT32 uwReceiverPid, VOS_UINT32 uwRet);
extern VOS_UINT32 VOICE_McSetCodec(VCVOICE_SET_CODEC_REQ_STRU *pstMsg, VOS_UINT16 uhwModemNo);
extern VOS_VOID VOICE_McSetCodecType(VOS_UINT16 uhwNetMode, VOS_UINT16 uhwCodecType, VOS_UINT16 uhwModemNo);
extern VOS_UINT32 VOICE_McSetDev(VOS_UINT16 uhwDevMode, VOS_UINT32 uwUpdate, VOS_UINT16 uhwModemNo);
extern VOS_VOID VOICE_McSetDevChannel(VOS_UINT16 uhwDevMode);
extern VOS_UINT32 VOICE_McSetLoopDisable(VOS_VOID);
extern VOS_UINT32 VOICE_McSetLoopEnable(VOS_VOID);
extern VOS_UINT32 VOICE_McStart(VCVOICE_START_REQ_STRU *pstMsg);
extern VOS_VOID VOICE_McStop(VOS_UINT16 uhwModemNo);
extern VOS_VOID VOICE_McTdChkMicConflick(VOS_VOID);
extern VOS_VOID VOICE_McTxDataToGphy(VOS_VOID);
extern VOS_VOID VOICE_McTxDataToTDphy(VOS_VOID);
extern VOS_VOID VOICE_McTxDataToWphy(VOS_VOID);
extern VOS_UINT32 VOICE_McTxProc(VOS_VOID);
extern VOS_VOID VOICE_McUnRegisterCaptureCallBack(VOS_VOID);
extern VOS_UINT32 VOICE_McUpdate(VOS_UINT16 uhwModemNo);
extern VOS_UINT32 VOICE_MsgApNoteCCPUResetRP(VOS_VOID *pstOsaMsg);
extern VOS_UINT32 VOICE_MsgGphyPlayIndR(VOS_VOID *pstOsaMsg);
extern VOS_UINT32 VOICE_MsgGphyRecordIndR(VOS_VOID *pstOsaMsg);
extern VOS_UINT32 VOICE_MsgGphyRxDataIndR(VOS_VOID *pstOsaMsg);
extern VOS_UINT32 VOICE_MsgGphyUpdateParaCmdRP(VOS_VOID *pstOsaMsg);
extern VOS_UINT32 VOICE_MsgGUPhyBackGroundCnfRP( VOS_VOID *pstOsaMsg );
extern VOS_UINT32 VOICE_MsgGUPhyForeGroundCnfRP( VOS_VOID *pstOsaMsg );
extern VOS_UINT32 VOICE_MsgOmGetNvReqRP(VOS_VOID *pstOsaMsg);
extern VOS_UINT32 VOICE_MsgOmQueryStatusReqIRP(VOS_VOID *pstOsaMsg);
extern VOS_UINT32 VOICE_MsgOmSetNvReqRP(VOS_VOID *pstOsaMsg);
extern VOS_UINT32 VOICE_MsgPhySuspendCmdR(VOS_VOID *pstOsaMsg);
extern VOS_UINT32 VOICE_MsgTdPhyBackGroundCnfRP( VOS_VOID *pstOsaMsg );
extern VOS_UINT32 VOICE_MsgTdPhyForeGroundCnfRP( VOS_VOID *pstOsaMsg );
extern VOS_UINT32 VOICE_MsgTDphyRxDataIndR(VOS_VOID *pstOsaMsg);
extern VOS_UINT32 VOICE_MsgTdSyncIndR(VOS_VOID *pstOsaMsg);
extern VOS_UINT32 VOICE_MsgVcBackGroundReqRP( VOS_VOID *pstOsaMsg  );
extern VOS_UINT32 VOICE_MsgVcForeGroundReqRP( VOS_VOID *pstOsaMsg  );
extern VOS_UINT32 VOICE_MsgVcGroundQryReqRP( VOS_VOID *pstOsaMsg );
extern VOS_UINT32 VOICE_MsgVcSetCodecReqP(VOS_VOID *pstOsaMsg);
extern VOS_UINT32 VOICE_MsgVcSetCodecReqR(VOS_VOID *pstOsaMsg);
extern VOS_UINT32 VOICE_MsgVcSetDevReqI(VOS_VOID *pstOsaMsg);
extern VOS_UINT32 VOICE_MsgVcSetDevReqRP(VOS_VOID *pstOsaMsg);
extern VOS_UINT32 VOICE_MsgVcSetVolReqIRP(VOS_VOID *pstOsaMsg);
extern VOS_UINT32 VOICE_MsgVcStartReqI(VOS_VOID *pstOsaMsg);
extern VOS_UINT32 VOICE_MsgVcStopReqRP(VOS_VOID *pstOsaMsg);
extern VOS_UINT32 VOICE_MsgVoiceLoopReqIRP(VOS_VOID *pstOsaMsg);
extern VOS_UINT32 VOICE_MsgVoicePlayIndR(VOS_VOID *pstOsaMsg);
extern VOS_UINT32 VOICE_MsgVoiceRecordIndR(VOS_VOID *pstOsaMsg);
extern VOS_UINT32 VOICE_MsgVoiceTxProcReqR(VOS_VOID *pstOsaMsg);
extern VOS_UINT32 VOICE_MsgVoiceRxProcReqR(VOS_VOID *pstOsaMsg);
extern VOS_UINT32 VOICE_MsgWphyRxDataIndR(VOS_VOID *pstOsaMsg);
extern VOS_UINT32 VOICE_MsgWphySyncIndR(VOS_VOID *pstOsaMsg);
extern VOS_UINT32 VOICE_MsgImsaRxDataIndR( VOS_VOID *pstOsaMsg );
extern VOS_VOID VOICE_SyncTdSubFrm5msIsr(VOS_VOID);
extern VOS_VOID VOICE_TdsRxTimerIsr(VOS_UINT32 uwTimer, VOS_UINT32 uwPara);
extern VOS_VOID VOICE_TdsTxTimerIsr(VOS_UINT32 uwTimer, VOS_UINT32 uwPara);
extern VOS_VOID VOICE_Timer20msIsr(VOS_UINT32 uwTimer, VOS_UINT32 uwPara);
extern VOS_VOID VOICE_Timer3msIsr(VOS_UINT32 uwTimer, VOS_UINT32 uwPara);
extern VOS_VOID VOICE_Timer6msIsr(VOS_UINT32 uwTimer, VOS_UINT32 uwPara);
extern VOS_UINT32 VOICE_MsgGphySyncIndR(VOS_VOID *pstOsaMsg);
extern VOS_UINT16 VOICE_McDetectRxFrmLost(VOS_VOID);
extern VOS_VOID VOICE_Timer20msRxIsr(VOS_UINT32 uwTimer, VOS_UINT32 uwPara);
extern VOS_UINT32 VOICE_McTxEnc( VOS_VOID );
extern VOS_VOID VOICE_McGsmChkMicConflick(VOS_VOID);
extern VOS_VOID VOICE_McSetBackGround(VOICE_MC_MODEM_NUM_ENUM_UINT16      enModemNo);
extern VOS_VOID VOICE_McTxDataToImsa(VOS_VOID);
extern VOS_UINT32 VOICE_MsgImsaCfgReqR( VOS_VOID *pstOsaMsg );
extern VOS_UINT32 VOICE_MsgGphyChannelQualityIndR( VOS_VOID *pstOsaMsg  );
extern VOS_UINT32 VOICE_MsgWphyChannelQualityIndR( VOS_VOID *pstOsaMsg  );

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif /* end of VOICE_mc.h */
