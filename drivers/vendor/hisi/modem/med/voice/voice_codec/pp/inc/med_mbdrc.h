


#ifndef __MED_MBDRC_H__
#define __MED_MBDRC_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "codec_typedefine.h"
#include "CodecInterface.h"
#include "med_pp_comm.h"
#include "med_eanr.h"
#include "med_aec_nlp.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define MED_MBDRC_RX_NB_FREQLEN         (256)
#define MED_MBDRC_RX_WB_FREQLEN         (512)
#define MED_MBDRC_RX_MU_FREQLEN         (1024)

#define MED_MBDRC_RX_LIMIT_THR          (24572)
#define MED_MBDRC_RX_LIMIT_MAKEUP_GAIN  (40)
#define MED_MBDRC_RX_0DBFS              (32767)
#define MED_MBDRC_RX_SUBBAND_NUM        (5)
#define MED_MBDRC_RX_ALPHA_NORM         (128)
#define MED_MBDRC_RX_SLOP_NORM          (128)
#define MED_MBDRC_RX_GAIN_30DB_NORM     (7706)
#define MED_MBDRC_RX_IFFT_NUM_IND       (3)
#define MED_MBDRC_RX_FFT_NUM_IND        (1)
#define MED_MBDRC_RX_NB_FRMLEN          (160)
#define MED_MBDRC_RX_NB_BANDWIDTH       (129)
#define MED_MBDRC_RX_MU_BANDWIDTH       (513)
#define MED_BGN_INIT_VALUE              (0)

#define MED_MBDRC_MAX_OBJ_NUM           (3)                                     /* MBDRC最大实体数目 */
#define MED_MBDRC_PARAM_LEN             (126)
#define MED_MBDRC_MAX_FFT_LEN           (1024)

#define MED_MBDRC_FRM_LEN_8K            (80)
#define MED_MBDRC_FFT_LEN_8K            (256)
#define MED_MBDRC_FRM_LEN_16K           (160)
#define MED_MBDRC_FFT_LEN_16K           (512)
#define MED_MBDRC_FRM_LEN_48K           (960)
#define MED_MBDRC_FFT_LEN_48K           (1024)

#define MED_MBDRC_NB_OVERLAP_LEN          (176)                                 /* 窄带窗叠长度 */
#define MED_MBDRC_WB_OVERLAP_LEN          (352)                                 /* 宽带重叠窗长度 */
#define MED_MBDRC_MB_OVERLAP_LEN          (32)                                  /* 音频重叠窗长度 */
#define MED_MBDRC_NLP_WB_POST_OVERLAP_LEN (192)                                 /* 宽带后重叠窗长度 */

#define MED_MBDRC_ANR_VOICE_SUB_FRAME_NUM (2)                                   /* NLP子帧个数 */
#define MED_MBDRC_ANR_AUDIO_SUB_FRAME_NUM (1)                                   /* NLP子帧个数 */

/* 访问全局变量 */
#define MED_MBDRC_GetObjPtr()           (&g_stMedMbdrcObj[0])                   /* 获取MBDRC全局变量 */
#define MED_MBDRC_GetObjInfoPtr()       (&g_stMedMbdrcObjInfo)                  /* 获取MBDRC全局对象信息 */

#define MED_MBDRC_GetNlpWinNb()         (&g_ashwMedAecNlpWinNb[0])              /* 窄带下时频变换窗系数 */
#define MED_MBDRC_GetNlpWinWb()         (&g_ashwMedAecNlpWinWb[0])              /* 宽带下时频变换窗系数 */
#define MED_MBDRC_GetNlpWinMb()         (&g_ashwMedMbdrcNlpWinMb[0])            /* 宽带下时频变换窗系数 */

/* 临时全局数组，供MBDRC各模块使用 */
extern  VOS_INT16                        g_ashwMedMbdrcTmp1Len640[MED_MBDRC_MAX_FFT_LEN];

#define MED_MBDRC_GetshwVecTmp640Ptr1()  (&g_ashwMedMbdrcTmp1Len640[0])         /* 临时全局数组指针 长度640 INT16 */



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
 实体名称  : MED_MBDRC_NV_STRU
 功能描述  : MBDRC_NV结构体
*****************************************************************************/
typedef struct
{
    VOS_INT16                   shwEnable;                                      /* MBDRC使能信号 */
    VOS_INT16                   shwSubbandNum;                                  /* 子带数 */
    VOS_INT16                   ashwSubbandMaxGain[MED_MBDRC_RX_SUBBAND_NUM];   /* 子带最大增益 */
    VOS_INT16                   ashwFreqBoundLow[MED_MBDRC_RX_SUBBAND_NUM];     /* 频带下边界 */
    VOS_INT16                   ashwFreqBoundHigh[MED_MBDRC_RX_SUBBAND_NUM];    /* 频带上边界 */
    VOS_INT16                   ashwDnCompThr[MED_MBDRC_RX_SUBBAND_NUM];        /* 各子带下压缩区门限（大于此门限为下压缩区），单位为dBfs */
    VOS_INT16                   ashwDnCompSlop[MED_MBDRC_RX_SUBBAND_NUM];       /* 各子带下压缩区增益曲线斜率 */
    VOS_INT16                   ashwUpCompThr[MED_MBDRC_RX_SUBBAND_NUM];        /* 各子带上压缩区门限（小于此门限为上压缩区），单位为dBfs */
    VOS_INT16                   ashwUpCompSlop[MED_MBDRC_RX_SUBBAND_NUM];       /* 各子带上压缩区增益曲线斜率 */
    VOS_INT16                   ashwDnExpThr[MED_MBDRC_RX_SUBBAND_NUM];         /* 各子带下扩展区门限（小于此门限为下扩展区），单位为dBfs */
    VOS_INT16                   ashwDnExpSlop[MED_MBDRC_RX_SUBBAND_NUM];        /* 各子带下扩展区增益曲线斜率 */
    VOS_INT16                   ashwDnCompAlphaUp[MED_MBDRC_RX_SUBBAND_NUM];    /* 各子带下压缩区RMS增益在信号功率相对于前一帧变大时的alpha滤波系数 */
    VOS_INT16                   ashwDnCompAlphaDn[MED_MBDRC_RX_SUBBAND_NUM];    /* 各子带下压缩区RMS增益在信号功率相对于前一帧变小时的alpha滤波系数 */
    VOS_INT16                   ashwLinearAlphaUp[MED_MBDRC_RX_SUBBAND_NUM];    /* 各子带线性区RMS增益在信号功率相对于前一帧变大时的alpha滤波系数 */
    VOS_INT16                   ashwLinearAlphaDn[MED_MBDRC_RX_SUBBAND_NUM];    /* 各子带线性区RMS增益在信号功率相对于前一帧变小时的alpha滤波系数 */
    VOS_INT16                   ashwUpCompAlphaUp[MED_MBDRC_RX_SUBBAND_NUM];    /* 各子带上压缩区RMS增益在信号功率相对于前一帧变大时的alpha滤波系数 */
    VOS_INT16                   ashwUpCompAlphaDn[MED_MBDRC_RX_SUBBAND_NUM];    /* 各子带上压缩区RMS增益在信号功率相对于前一帧变小时的alpha滤波系数 */
    VOS_INT16                   ashwDnExpAlphaUp[MED_MBDRC_RX_SUBBAND_NUM];     /* 各子带下扩展区RMS增益在信号功率相对于前一帧变大时的alpha滤波系数 */
    VOS_INT16                   ashwDnExpAlphaDn[MED_MBDRC_RX_SUBBAND_NUM];     /* 各子带下扩展区RMS增益在信号功率相对于前一帧变小时的alpha滤波系数 */
    VOS_INT16                   ashwDnExpMinGain[MED_MBDRC_RX_SUBBAND_NUM];     /* 各子带下扩展区最小增益 */
    VOS_INT16                   ashwAlphaUpDnThr[MED_MBDRC_RX_SUBBAND_NUM];     /* 各子带ALPHAUP和ALPHADN的选取门限参数 */
    VOS_INT16                   ashwMinPowerRMS[MED_MBDRC_RX_SUBBAND_NUM];      /* 最小RMS */
    VOS_INT16                   ashwLastPowerRMSInit[MED_MBDRC_RX_SUBBAND_NUM]; /* 上一帧功率RMS初始值 */
    VOS_INT16                   ashwLastGain[MED_MBDRC_RX_SUBBAND_NUM];         /* 上一帧增益 */
    VOS_INT16                   shwDRCMindB;                                    /* DRC最小dB值 */
    VOS_INT16                   ashwLastPowerRMSdBinInit[MED_MBDRC_RX_SUBBAND_NUM]; /* 上一帧功率RMS初始值 */
    VOS_INT16                   ashwGainMakeUp[MED_MBDRC_RX_SUBBAND_NUM];       /* 补偿增益 */
    VOS_INT16                   shwFirLen;                                      /* Fir长度 */
    VOS_INT16                   shwSpeechCntInit;
    VOS_INT16                   shwVADholdLenth;
} MED_MBDRC_NV_STRU;

/*****************************************************************************
 实体名称  : MED_MBDRC_SUBAND_STRU
 功能描述  : MBDRC_SUBAND结构体
*****************************************************************************/
typedef struct
{
    VOS_INT16                        ashwMinPowerRMS[MED_MBDRC_RX_SUBBAND_NUM]; /* 最小RMS */
    VOS_INT16                        ashwLastPowerRMS[MED_MBDRC_RX_SUBBAND_NUM];/* 上一帧RMS */
    VOS_INT16                        ashwLastGain[MED_MBDRC_RX_SUBBAND_NUM];    /* 上一帧增益 */
    VOS_INT16                        ashwLastPowerRMSdBin[MED_MBDRC_RX_SUBBAND_NUM];
    VOS_INT16                        shwLimitThr;                               /* 信号限幅门限，单位为dBfs */
    VOS_INT16                        shwLimitMakeUpGain;                        /* 各子带极限RMS补偿增益 */
    VOS_INT16                        shwFrameLenth;
    VOS_INT16                        shwFreqLenth;                              /* 频谱长度  */
    VOS_INT16                        shwSpeechCnt;
    VOS_INT16                        shwReserve;
    VOS_INT32                        swdBfs0;                                   /* DRC的RMS增益折线和输出信号限幅门限的0dBfs  */
} MED_MBDRC_SUBAND_STRU;

/*****************************************************************************
 实体名称  : MED_MBDRC_AIG_STRU
 功能描述  : MBDRC_AIG结构体
*****************************************************************************/
typedef struct
{
    VOS_INT16                           shwLastGain;                            /* 上一帧增益 */
    VOS_INT16                           shwFreqLenth;                           /* 频谱长度  */
    VOS_INT32                           swdBfs0;                                /* DRC的RMS增益折线和输出信号限幅门限的0dBfs */

} MED_MBDRC_AIG_STRU;


/*****************************************************************************
 实体名称  : MED_FFT_COM_CFG_STRU
 功能描述  : 描述FFT_COM_CFG结构体
*****************************************************************************/
typedef struct
{
    VOS_INT16                           shwFftLen;                              /* FFT长度 */
    VOS_INT16                           shwSubFrmLen;                           /* 子带帧长 */
} MED_FFT_COM_CFG_STRU;

/*****************************************************************************
 实体名称  : MED_FRM_PRE_STATUS_STRU
 功能描述  : 描述FRM_PRE_STATUS结构体
*****************************************************************************/
typedef struct
{
    VOS_INT16                           shwOverlapLen;                          /* 重叠数组长度 */
    VOS_INT16                           shwReserve;
    VOS_INT16                           ashwOverlap[MED_MBDRC_WB_OVERLAP_LEN]; /* 重叠数组  */
} MED_ERR_PRE_STATUS_STRU;

/*****************************************************************************
 实体名称  : MED_FRM_POST_STATUS_STRU
 功能描述  : 描述FRM_POST_STATUS结构体
*****************************************************************************/
typedef struct
{
    VOS_INT16                           shwOverlapLen;                          /* 重叠数组长度 */
    VOS_INT16                           shwReserve;
    VOS_INT16                           ashwOverlap[MED_MBDRC_WB_OVERLAP_LEN];  /* 重叠数组 */
} MED_ERR_POST_STATUS_STRU;

/*****************************************************************************
 实体名称  : MED_FFT_TRANSFORM_STRU
 功能描述  : 描述FFT_TRANSFORM结构体
*****************************************************************************/
typedef struct
{
    MED_ERR_PRE_STATUS_STRU             stErrPreStatus;                         /* 残差前处理状态 */
    MED_ERR_POST_STATUS_STRU            stErrPostStatus;                        /* 残差后处理状态 */
} MED_FFT_TRANSFORM_STRU;

/*****************************************************************************
 实体名称  : MED_ANR_FFT_STRU
 功能描述  : 描述ANR_FFT结构体
*****************************************************************************/
typedef struct
{
    MED_FFT_COM_CFG_STRU                stComCfg;                               /* FFT配置 结构体 */
    MED_FFT_TRANSFORM_STRU              stTransform;                            /* FFT转换 结构体*/
    VOS_INT16                          *pshwTrapewin;
} MED_ANR_FFT_STRU;

/*****************************************************************************
 实体名称  : MED_MBDRC_STRU
 功能描述  : MBDRC结构体
*****************************************************************************/
typedef struct
{
    MED_OBJ_HEADER
    MED_MBDRC_NV_STRU                   stNv;                                   /* NV项 */
    MED_MBDRC_SUBAND_STRU               stSuband;                               /* 子带项 */
    MED_ANR_FFT_STRU                    stAnrFft;                               /* FFT项 */
    VOS_INT16                           shwSubBandNum;                          /* 子块个数 */
    VOS_INT16                           shwReserve;
} MED_MBDRC_STRU;

/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/
extern MED_MBDRC_STRU                    g_stMedMbdrcObj[MED_MBDRC_MAX_OBJ_NUM];/* MBDRC模块全局控制实体 */
extern MED_OBJ_INFO                      g_stMedMbdrcObjInfo;                   /* MBDRC信息 */

/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern VOS_UINT32    MED_ANR_MBDRC_Rx_Main (
                       MED_EANR_STRU          *pstMedAnr,
                       MED_MBDRC_STRU         *pstMedMBDRC,
                       VOS_INT16              *pshwInput,
                       VOS_INT16              *pshwOutput,
                       VOS_INT16               shwVadFlag,
                       CODEC_SWITCH_ENUM_UINT16  enAnrEnable);
extern VOS_VOID    MED_ANR_RX_FFTProcess (
                              MED_ANR_FFT_STRU                  *pstAnrFft,
                              VOS_INT16                         *pshwInput,
                              VOS_INT16                          shwNormEn,
                              VOS_INT16                         *pshwSpecBin,
                              VOS_INT16                         *pshwNormShift);
extern  VOS_VOID MED_ANR_RX_IFFTProcess (
                               MED_ANR_FFT_STRU                  *pstAnrFft,
                               VOS_INT16                        *pshwErrBin,
                               VOS_INT16                        *pshwErrFrm,
                               VOS_INT16                         shwNormShift);
extern VOS_VOID  MED_DRC_GainFilter (
                       MED_MBDRC_STRU        *pstMedMBDRC,
                       VOS_INT16              shwPowerIn,
                       VOS_INT16              shwindex,
                       VOS_INT16              shwPowerInd,
                       VOS_INT16              shwNormShift,
                       VOS_INT16             *pshwGainLinear);
extern VOS_VOID*    MED_MBDRC_Create(VOS_VOID);
extern VOS_UINT32   MED_MBDRC_Destroy(VOS_VOID  **ppInstance);
extern VOS_UINT32    MED_MBDRC_GetPara(
                           VOS_VOID         *pInstance,
                           VOS_INT16        *pshwPara,
                           VOS_INT16         shwParaLen);
extern VOS_VOID    MED_MBDRC_Main(
                           MED_MBDRC_STRU          *pstMbdrc,
                           VOS_INT16               *pshwFrmFreq,
                           VOS_INT16               *pshwNormShift,
                           VOS_INT16                shwVadFlag);
extern VOS_UINT32    MED_MBDRC_SetPara (
                           VOS_VOID         *pInstance,
                           VOS_INT16        *psMBDRCNv,
                           VOS_INT16         shwParaLen,
                           VOS_INT32         swSampleRate);
extern VOS_VOID  MED_DRC_Interpolate(
                        MED_MBDRC_NV_STRU      *pstMbdrcNv,
                        VOS_INT32              *pswMakeupGain,
                        VOS_INT32              *pswInterpolateGain);
extern VOS_VOID MED_MBDRC_SubBandPowerCal (
                                        MED_MBDRC_STRU         *pstMedMBDRC,
                                        VOS_INT16              *pshwFrmFreqIn,
                                        VOS_INT16              *pshwSubbandPowerRMS,
                                        VOS_INT16              *pshwPowerInd);
extern VOS_VOID MED_MBDRC_SubBandProcess (
                       MED_MBDRC_STRU         *pstMedMBDRC,
                       VOS_INT16              *pshwFrmFreq,
                       VOS_INT16               shwNormShift);



#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of med_mbdrc.h */
