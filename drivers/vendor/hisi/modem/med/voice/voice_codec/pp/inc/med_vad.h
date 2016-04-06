

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "codec_typedefine.h"
#include "CodecInterface.h"
#include "med_pp_comm.h"
#include "codec_op_float.h"

#ifndef __MED_VAD_H__
#define __MED_VAD_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/
/* 基本信息 */
#define MED_VAD_MAX_OBJ_NUM             (2)                                     /* VAD最大实体数目 */
#define MED_VAD_PARAM_LEN               (3)                                     /* VAD配置项的个数 */
#define MED_VAD_FRQ_BAND_MAX            (256)                                   /* 最大频带数 */
#define MED_VAD_STARTING_FRM_LEN        (5)                                     /* 门限开始自适应更新的起始帧数 */
#define MED_VAD_ARTI_VOICE_DETECT_AMP_THD (30)                                  /* 人工语音判决使能的幅度入口门限 */
#define MED_VAD_ARTI_SPCH_FREQ_BAND_MIN (9)                                     /* 最小频带 */
#define MED_VAD_ARTI_SPCH_FREQ_BAND_DIFF (5)                                    /* 频带差 */
#define MED_VAD_SMOOTH_FRM_AMP_ALPHA    (22937)                                 /* 加权系数，0.7 Q15 */
#define MED_VAD_CURR_FRM_ALPHA          (9830)                                  /* 加权系数，0.3 Q15 */
#define MED_VAD_HALFFFT_8K              (128)                                   /* FFT长度的一半，数值上等于 CODEC_OpShr(stSV.shwFftLen, 1); */
#define MED_VAD_HALFFFT_SHIFT_8K        (7)                                     /* FFT长度一半用二进制表示，所占的位数 floor(log2(double(stSV.shwHalfFftLen))); */
#define MED_VAD_MAX_FREQ_ENG_8K         ((VOS_INT32)16777215)                   /* 最大频域能量门限，数值上等于 CODEC_OpNormDiv_32(1, stSV.shwHalfFftLen, 31); */
#define MED_VAD_MAX_STD_DEVI_THD_8K     (4096)                                  /* 最大频域能量均方差门限，数值上等于 CODEC_OpSqrt(stSV.swMaxFreqEngThd); */
#define MED_VAD_FREQ_ENG_NORM_PARA_8K   (7)                                     /* 频域能量均方差归一化参数，数值上等于 CODEC_OpNorm_l(stSV.swMaxFreqEngThd); */
#define MED_VAD_STD_DEVI_NORM_PARA_8K   (18)                                    /* 频域能量均方差归一化参数，数值上等于 CODEC_OpNorm_l(stSV.swMaxStandardDeviThd); */
#define MED_VAD_HALFFFT_16K             (256)                                   /* FFT长度的一半，数值上等于 CODEC_OpShr(stSV.shwFftLen, 1); */
#define MED_VAD_HALFFFT_SHIFT_16K       (8)                                     /* FFT长度一半用二进制表示，所占的位数 floor(log2(double(stSV.shwHalfFftLen))); */
#define MED_VAD_MAX_FREQ_ENG_16K        ((VOS_INT32)8388607)                    /* 最大频域能量门限，数值上等于 CODEC_OpNormDiv_32(1, stSV.shwHalfFftLen, 31); */
#define MED_VAD_MAX_STD_DEVI_THD_16K    (2896)                                  /* 最大频域能量均方差门限，数值上等于 CODEC_OpSqrt(stSV.swMaxFreqEngThd); */
#define MED_VAD_FREQ_ENG_NORM_PARA_16K  (8)                                     /* 频域能量均方差归一化参数，数值上等于 CODEC_OpNorm_l(stSV.swMaxFreqEngThd); */
#define MED_VAD_STD_DEVI_NORM_PARA_16K  (19)                                    /* 频域能量均方差归一化参数，数值上等于 CODEC_OpNorm_l(stSV.swMaxStandardDeviThd); */
#define MED_VAD_PSEUDOFLOAT_ONE_M       (16384)                                 /* 伪浮点数 1 的m分量 */
#define MED_VAD_PSEUDOFLOAT_ONE_E       (-14)                                   /* 伪浮点数 1 的e分量 */
#define MED_VAD_1_Q9                    (512)                                   /* 1 用 Q9来表示 */
#define MED_VAD_1_Q19                   ((VOS_INT32)524288)                     /* 1 用 Q19来表示 */
#define MED_VAD_0P7_Q15                 (22938)                                 /* 0.7 用 Q9来表示 */
#define MED_VAD_0P3_Q15                 (9830)                                  /* 0.3 用 Q9来表示 */
#define MIN_SMOOTH_ALPHA                (1)                                     /* 平滑系数，Q15 */
#define MED_VAD_SMOOTH_AMP_THD          (25)                                    /* 平滑幅度门限 */
#define MED_VAD_SMOOTH_AMP_UPDATE_THD   (40)                                    /* 平均幅度平滑门限 */
#define MED_VAD_AVE_AMP_CURR_FRM_THD    (40)                                    /* 平均幅度门限 */

/* 缓存相关 */
#define MED_VAD_FRM_NUM_1P5_SECOND      (75)                                    /* 1.5秒帧数 */
#define MED_VAD_BUFF_LEN                (256)                                   /* 缓存长度 */

/* 获取全局变量 */
#define MED_VAD_GetObjPtr()             (&g_astMedVadObjPool[0])                /* 获取内存块全局变量地址 */
#define MED_VAD_GetObjInfoPtr()         (&g_stMedVadObjInfo)                    /* 获取内存块信息的全局变量地址 */

/*****************************************************************************
  3 枚举定义
*****************************************************************************/

/*****************************************************************************
 实体名称  : MED_VAD_FFT_LEN_ENUM
 功能描述  : FFT长度枚举定义
*****************************************************************************/
enum MED_VAD_FFT_LEN_ENUM
{
    MED_VAD_FFT_LEN_8K                  = 256,                                  /* 8k采样率 */
    MED_VAD_FFT_LEN_16K                 = 512,                                  /* 16k采样率 */
    MED_VAD_FFT_LEN_BUTT
};
typedef VOS_INT16 MED_VAD_FFT_LEN_ENUM_INT16;

/*****************************************************************************
 实体名称  : MED_VAD_PREVFRM_LEN_ENUM
 功能描述  : 加窗时上一帧末尾的长度枚举定义（加窗用）
*****************************************************************************/
enum MED_VAD_PREVFRM_LEN_ENUM
{
    /* 8k采样率 */
    MED_VAD_PREVFRM_LEN_8K              = MED_VAD_FFT_LEN_8K - CODEC_FRAME_LENGTH_NB,
    /* 16k采样率 */
    MED_VAD_PREVFRM_LEN_16K             = MED_VAD_FFT_LEN_16K - CODEC_FRAME_LENGTH_WB,
    MED_VAD_PREVFRM_LEN_BUTT
};
typedef VOS_INT16 MED_VAD_PREVFRM_LEN_ENUM_INT16;


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
    VOS_INT16                           shwAlpha;                               /* 配置项 门限判决系数 */
    VOS_INT16                           shwArtiSpeechThd;                       /* 配置项 人工语音检测门限 */
    VOS_INT16                           shwChopNumThd;                          /* 配置项 人工语音中，频带能量跳变次数 */
    VOS_INT16                           shwReserved;
}MED_VAD_NV_STRU;

/*****************************************************************************
实体名称  : MED_VAD_STRU
功能描述  : VAD维护结构体
*****************************************************************************/
typedef struct
{
    /* 基本信息 */
    MED_OBJ_HEADER
    MED_VAD_NV_STRU                     stNv;                                   /* NV项维护结构体 */
    MED_VAD_FFT_LEN_ENUM_INT16          enFftLen;                               /* FFT长度 */
    MED_VAD_PREVFRM_LEN_ENUM_INT16      enPrevFrmLen;                           /* 上一帧数据的有效长度 */
    VOS_INT16                           shwFrmLen;                              /* 帧长 */
    VOS_INT16                           shwFrmCount;                            /* 前5帧的帧计数 */

    /* FFT相关 */
    VOS_INT16                          *pshwHammingWindow;                      /* 窗的指针 */
    VOS_INT16                           shwHalfFftLen;                          /* FFT长度的一半 */
    VOS_INT16                           shwHalfFftShift;                        /* FFT长度一半用二进制表示，所占的位数 */

    /* 计算相关 */
    VOS_INT16                           shwStdDeviNormPara;                     /* 频域能量均方差归一化参数 */
    VOS_INT16                           shwFreqEngNormPara;                     /* 频域能量均方差归一化参数 */
    VOS_INT32                           swMaxFreqEngThd;                        /* 最大频域能量门限 */
    VOS_INT32                           swMaxStandardDeviThd;                   /* 最大频域能量均方差门限 */
    VOS_INT16                           shwSmoothedValue;                       /* 平滑后的能量值 */
    VOS_INT16                           shwModifiedAlpha;                       /* 平滑参数 */

    /* 缓存相关 */
    VOS_INT16                           shwRingCnt;                             /* 环形队列，内存储部分控制信息 */
    VOS_INT16                           shwReserved2;
    CODEC_OP_FLOAT_STRU                   stFloatMinTarget;                       /* 缓存BUFF中的最小值 */
    CODEC_OP_FLOAT_STRU                   stFloatSmoothedTarget;                  /* 平滑后的当前谱方差对象 */
    CODEC_OP_FLOAT_STRU                   stFloatEmbryonicThd;                    /* 历史帧队列初始5帧的更新 */
    CODEC_OP_FLOAT_STRU                   astFloatTargetBuf[MED_VAD_FRM_NUM_1P5_SECOND];/* 一段时间内平滑后的谱方差存在BUFF中 */
    VOS_INT16                           ashwFrmDat[MED_VAD_FFT_LEN_16K];        /* 输入帧和历史帧的结合 */
    VOS_INT32                           aswBuff[MED_VAD_BUFF_LEN];              /* 缓存 */
} MED_VAD_OBJ_STRU;

/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/
extern VOS_INT16           g_ashwMedVadHammingWin8k[128];                              /* Hamming窗8k采样 */
extern VOS_INT16           g_ashwMedVadHammingWin16k[256];                             /* Hamming窗16k采样 */
extern MED_VAD_OBJ_STRU    g_astMedVadObjPool[MED_VAD_MAX_OBJ_NUM];                    /* VAD实体资源池 */
extern MED_OBJ_INFO        g_stMedVadObjInfo;                                          /* 相关的信息 */


/*****************************************************************************
  10 函数声明
*****************************************************************************/

extern VOS_INT16 MED_VAD_ArtiSpchDetect(
                           MED_VAD_OBJ_STRU   *pstVad,
                           VOS_INT16           shwAveCurrFrm,
                           VOS_INT32          *aswFreqEng);
extern VOS_VOID* MED_VAD_Create(VOS_VOID);
extern VOS_UINT32 MED_VAD_Destroy(VOS_VOID **ppstObj);
extern VOS_UINT32 MED_VAD_GetPara(
                       VOS_VOID               *pstInstance,
                       VOS_INT16              *pshwParam,
                       VOS_INT16               shwParaLen);
extern VOS_UINT32 MED_VAD_Main(
                       VOS_VOID               *pstInstance,
                       VOS_INT16              *pshwIn,
                       VOS_INT16              *penVadFlag);
extern VOS_UINT32 MED_VAD_SetPara (
                       VOS_VOID               *pstInstance,
                       VOS_INT16              *pshwParam,
                       VOS_INT16               shwParaLen);
extern VOS_VOID MED_VAD_UpdateThd(
                       MED_VAD_OBJ_STRU       *pstVad,
                       CODEC_OP_FLOAT_STRU      *pstFloatCurrTarget);
extern VOS_VOID MED_VAD_FreqEngCal(
                       MED_VAD_OBJ_STRU         *pstVad,
                       VOS_INT16                *pshwCurrFrm,
                       VOS_INT32                *pswFreqEng);











#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of med_vad.h */


