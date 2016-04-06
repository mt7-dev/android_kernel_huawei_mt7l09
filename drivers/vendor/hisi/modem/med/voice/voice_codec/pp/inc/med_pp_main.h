

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "codec_com_codec.h"
#include "med_pp_comm.h"
#include "med_fft.h"
#include "med_avc.h"
#include "med_filt.h"
#include "med_gain.h"
#include "med_aec_main.h"
#include "med_anr_2mic_interface.h"
#include "OmCodecInterface.h"
#include "med_agc.h"
#include "med_vad.h"
#include "med_mbdrc.h"
#include "med_aig.h"

#ifndef __MED_PP_MAIN_H__
#define __MED_PP_MAIN_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define MED_PP_GetObjPtr()               (&g_stMedPpObj)             /* 获取PP全局变量 */
#define MED_PP_GetSampleRate()           (g_stMedPpObj.enSampleRate)   /* 获取当前采样率模式 */
#define MED_PP_SetSampleRate(enSmpRate)  (g_stMedPpObj.enSampleRate = (enSmpRate))   /* 设置当前采样率模式 */

#define AUDIO_PP_GetObjPtr()             (&g_stAudioPpObj)             /* 获取音频PP全局变量 */
/*****************************************************************************
  3 枚举定义
*****************************************************************************/


/*****************************************************************************
  4 消息头定义
*****************************************************************************/


/*****************************************************************************
  5 消息定义
*****************************************************************************/


/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/
/*****************************************************************************
 结构名    : VOICE_NV_STRU
 结构说明  : NV参数结构体
*****************************************************************************/
typedef struct
{
    VOS_INT16                           ashwVol[CODEC_NV_VOL_LEN];                /* 发送、接收音量，dB值, -80~12 */
    VOS_INT16                           ashwPreEmp[CODEC_NV_PREEMP_LEN];          /* 预加重使能、预加重系数[15]、去加重系数[15] */
    VOS_INT16                           ashwDeEmp[CODEC_NV_DEEMP_LEN];            /* 预加重使能、预加重系数[15]、去加重系数[15] */
    VOS_INT16                           ashwHpfTx[CODEC_NV_HPF_TX_LEN];           /* 上行高通滤波使能、滤波器系数[16] */
    VOS_INT16                           ashwHpfRx[CODEC_NV_HPF_RX_LEN];           /* 下行高通滤波使能、滤波器系数[16] */
    VOS_INT16                           ashwDevGain[CODEC_NV_DEVGAIN_LEN];        /* 设备发送增益、设备接收增益*/
    VOS_INT16                           ashwSTGain[CODEC_NV_SIDEGAIN_LEN];        /* 侧音增益 */
    VOS_INT16                           ashwCompTx[CODEC_NV_COMP_TX_LEN];         /* 上行补偿滤波使能、滤波器系数[53] */
    VOS_INT16                           ashwCompRx[CODEC_NV_COMP_RX_LEN];         /* 下行补偿滤波使能、滤波器系数[53] */
    VOS_INT16                           ashwAec[CODEC_NV_AEC_LEN];                /* AEC使能、EC参数[20] */
    VOS_INT16                           ashwAnr1MicTx[CODEC_NV_EANR_1MIC_LEN];     /* ANR使能、NS_1MIC参数[10] */
    VOS_INT16                           ashwAnr1MicRx[CODEC_NV_EANR_1MIC_LEN];     /* ANR使能、NS_1MIC参数[10] */
    VOS_INT16                           ashwAnr2Mic[CODEC_NV_ANR_2MIC_LEN];       /* ANR使能、NS_2MIC参数[10] */
    VOS_INT16                           ashwAgcTx[CODEC_NV_AGC_TX_LEN];           /* 上行AGC使能、AGC参数[6] */
    VOS_INT16                           ashwAgcRx[CODEC_NV_AGC_RX_LEN];           /* 下行AGC使能、AGC参数[6] */
    VOS_INT16                           ashwMbdrc[CODEC_NV_MBDRC_LEN];            /* MBDRC使能、MBDRC参数[224] */
    VOS_INT16                           ashwAigTx[CODEC_NV_AIG_TX_LEN];           /* 上行AIG使能、AIG参数[41] */
    VOS_INT16                           ashwAigRx[CODEC_NV_AIG_RX_LEN];           /* 下行AIG使能、AIG参数[41] */
    VOS_INT16                           ashwAvc[CODEC_NV_AVC_LEN];                /* AVC使能、AVC参数[10] */
    VOS_INT16                           ashwVadTx[CODEC_NV_VAD_TX_LEN];           /* 上行VAD参数 */
    VOS_INT16                           ashwVadRx[CODEC_NV_VAD_RX_LEN];           /* 下行VAD参数 */
    VOS_INT16                           ashwReserve1[CODEC_NV_PP_RESERVE_LEN];    /* PP参数预留数组1 */
    VOS_INT16                           ashwReserve2[CODEC_NV_PP_RESERVE_LEN];    /* PP参数预留数组2 */
    VOS_INT16                           ashwReserve3[CODEC_NV_PP_RESERVE_LEN];    /* PP参数预留数组3 */
    VOS_INT16                           ashwReserve4[CODEC_NV_PP_RESERVE_LEN];    /* PP参数预留数组4 */
    VOS_INT16                           ashwReserve5[CODEC_NV_PP_RESERVE_LEN];    /* PP参数预留数组5 */
}VOICE_NV_STRU;

/*****************************************************************************
 结构名    : AUDIO_NV_STRU
 结构说明  : NV参数结构体
*****************************************************************************/
typedef struct
{
    VOS_INT16                           ashwCompTx[AUDIO_NV_COMP_TX_LEN];       /* 上行补偿滤波使能、滤波器系数[68] */
    VOS_INT16                           ashwCompRx[AUDIO_NV_COMP_RX_LEN];       /* 下行补偿滤波使能、滤波器系数[68] */
    VOS_INT16                           ashwMbdrc[AUDIO_NV_MBDRC_LEN];          /* MBDRC使能、MBDRC参数[126] */
}AUDIO_NV_STRU;

/*****************************************************************************
实体名称  : MED_PP_STRU
功能描述  : 结构体对象信息
*****************************************************************************/
typedef struct
{
    CODEC_SAMPLE_RATE_MODE_ENUM_INT32   enSampleRate;                           /* 采样率 */
    MED_PP_MIC_NUM_ENUM_UINT16          enMicNum;                               /* Mic数目 */
    VOS_UINT16                          uhwFrameId;                            /*时间帧计数指示*/
    VOS_VOID                           *pstDevGainTx;                           /* 上行匹配增益 */
    VOS_VOID                           *pstDevGainRx;                           /* 下行匹配增益 */
    VOS_VOID                           *pstSTGain;                              /* 侧音增益 */
    VOS_VOID                           *pstAec;                                 /* AEC 对象 */
    VOS_VOID                           *pstAgcTx;                               /* 上行AGC 对象 */
    VOS_VOID                           *pstAgcRx;                               /* 下行AGC 对象 */
    VOS_VOID                           *pstMbdrc;                               /* MBDRC对象 */
    VOS_VOID                           *pstAigTx;                               /* 上行AIG 对象 */
    VOS_VOID                           *pstAigRx;                               /* 下行AIG 对象 */
    VOS_VOID                           *pstVadTx;                               /* 上行VAD对象 */
    VOS_VOID                           *pstVadRx;                               /* 下行VAD对象 */
    VOS_VOID                           *pstAvc;                                 /* AVC 对象 */
    VOS_VOID                           *pstAnrTx;                               /* ANR单麦克 上行对象 */
    VOS_VOID                           *pstAnrRx;                               /* ANR单麦克 下行对象 */
    VOS_VOID                           *pstAnr2Mic;                             /* ANR双麦克 对象 */
    VOS_VOID                           *pstPreEmpMc;                            /* 主麦克信号预加重滤波器*/
    VOS_VOID                           *pstPreEmpRc;                            /* 辅麦克信号预加重滤波器*/
    VOS_VOID                           *pstPreEmpFar;                           /* 远端信号预加重滤波器*/
    VOS_VOID                           *pstDeEmpMc;                             /* 近端信号去加重滤波器*/
    VOS_VOID                           *pstHpfTxMc;                             /* 上行AF模块前主麦克高通滤波器 */
    VOS_VOID                           *pstHpfTxRc;                             /* 上行AF模块前辅麦克高通滤波器 */
    VOS_VOID                           *pstHpfAecMc;                            /* 上行AF模块后主麦克高通滤波器 */
    VOS_VOID                           *pstHpfAecRc;                            /* 上行AF模块后辅麦克高通滤波器 */
    VOS_VOID                           *pstHpfRx;                               /* 下行高通滤波器 */
    VOS_VOID                           *pstCompTx;                              /* 上行频响补偿滤波器 */
    VOS_VOID                           *pstCompRx;                              /* 下行频响补偿滤波器 */
}MED_PP_STRU;

/*****************************************************************************
实体名称  : AUDIO_PP_STRU
功能描述  : 结构体对象信息
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          uwSampleRate;                           /* 采样率 */
    VOS_VOID                           *pstMbdrcL;                              /* 左声道MBDRC对象 */
    VOS_VOID                           *pstMbdrcR;                              /* 右声道MBDRC对象 */
    VOS_VOID                           *pstCompTxL;                             /* 上行左声道频响补偿滤波器 */
    VOS_VOID                           *pstCompTxR;                             /* 上行右声道频响补偿滤波器 */
    VOS_VOID                           *pstCompRxL;                             /* 下行左声道频响补偿滤波器 */
    VOS_VOID                           *pstCompRxR;                             /* 下行右声道频响补偿滤波器 */
}AUDIO_PP_STRU;

/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/
extern MED_PP_STRU                            g_stMedPpObj;
extern AUDIO_PP_STRU                          g_stAudioPpObj;

/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern VOS_UINT32 MED_PpDestroy(VOS_VOID);
extern VOS_UINT32 MED_PpGetPara(VOICE_NV_STRU *pstNv);
extern VOS_UINT32 MED_PpSetPara(
                       VOICE_NV_STRU                     *pstNv,
                       MED_PP_MIC_NUM_ENUM_UINT16         enPpMicNum );
extern VOS_UINT32 MED_PpTx_1Mic(
                       VOS_INT16                         *pshwMicIn,
                       VOS_INT16                         *pshwLineOut);
extern VOS_UINT32 MED_PpTx_2Mic(
                       VOS_INT16                         *pshwMcIn,
                       VOS_INT16                         *pshwRcIn,
                       VOS_INT16                         *pshwLineOut);
extern VOS_INT16 MED_PP_Init(VOS_VOID);
extern VOS_UINT32 MED_RxPp( VOS_INT16               *pshwLineIn);
extern VOS_UINT32 MED_TxPp(
                       VOS_INT16                         *pshwMcIn,
                       VOS_INT16                         *pshwRcIn,
                       VOS_INT16                         *pshwLineOut,
                       VOS_INT16                         *pshwEcRef);
extern VOS_INT16 AUDIO_PP_Init(VOS_VOID);
extern VOS_UINT32 AUDIO_PpSetPara(
                        AUDIO_NV_STRU                     *pstNv,
                        VOS_UINT32                         uwSampleRate);
extern VOS_UINT32 AUDIO_PpGetPara(AUDIO_NV_STRU     *pstNv);
extern VOS_UINT32 AUDIO_ProcPpTx(
                        VOS_INT16                         *pshwMicInL,
                        VOS_INT16                         *pshwMicInR,
                        VOS_INT16                         *pshwLineOutL,
                        VOS_INT16                         *pshwLineOutR,
                        AUDIO_PP_CHANNEL_ENUM_UINT16       uhwChannel);
extern VOS_UINT32 AUDIO_ProcPpRx(
                        VOS_INT16                         *pshwLineInL,
                        VOS_INT16                         *pshwLineInR,
                        VOS_INT16                         *pshwLineOutL,
                        VOS_INT16                         *pshwLineOutR,
                        AUDIO_PP_CHANNEL_ENUM_UINT16       uhwChannel);
extern VOS_UINT32 AUDIO_PpDestroy(VOS_VOID);

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of med_pp_main.h */
