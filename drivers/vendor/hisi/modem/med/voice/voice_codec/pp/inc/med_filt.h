
#ifndef __MED_FILT_H__
#define __MED_FILT_H__


/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "codec_typedefine.h"
#include "CodecInterface.h"
#include "codec_com_codec.h"
#include "med_pp_comm.h"
#include "CodecNvInterface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/
/* 基本信息 */
#define MED_FILT_NORM_MAX               (15)                                    /* 小数在16bit时保证不越界的Q */
#define MED_PREEMP_MAX_OBJ_NUM          (3)                                     /* 预加重滤波器模块最大实体数目 */
#define MED_PREEMP_PARAM_LEN            (2)                                     /* 预加重滤波器有效的配置项个数 */
#define MED_PREEMP_LEN                  (2)                                     /* 预加重滤波器的FIR滤波长度 */
#define MED_DEEMP_MAX_OBJ_NUM           (1)                                     /* 去加重滤波器模块最大实体数目 */
#define MED_DEEMP_PARAM_LEN             (2)                                     /* 去加重滤波器有效的配置项个数 */
#define MED_DEEMP_LEN                   (2)                                     /* 去加重滤波器滤波长度 */
#define MED_HPF_MAX_OBJ_NUM             (5)                                     /* 高通滤波器模块最大实体数目 */
#define MED_HPF_LEN                     (4)                                     /* 高通滤波器阶数 */
#define MED_HPF_PARAM_LEN               ((2 * (MED_HPF_LEN)) + 1)              /* 高通滤波器有效的配置项个数 */
#define MED_HPF_PARAM_VEC_BASE          (1)                                     /* 高通滤波器向量起始位置 */
#define MED_HPF_HISTORY_BUFF_LEN        ((MED_HPF_LEN) - 1)                     /* 高通滤波器的历史缓存长度 */
#define MED_COMP_MAX_OBJ_NUM            (6)                                     /* 频响补偿滤波器模块最大实体数目 */
#define MED_COMP_FILT_LEN_MAX           (65)                                    /* 频响补偿滤波器最大允许长度 */
#define MED_COMP_PRE_LEN_MAX            ((2*(MED_COMP_FILT_LEN_MAX)) - 1)      /* 频响补偿滤波器缓存上一帧信号的最大允许长度 */
#define MED_COMP_PARAM_VEC_BASE         (3)                                     /* 频响补偿滤波器的向量起始位置 */
#define MED_COMP_CHANNEL_MAX            (2)                                      /* 最大CHANNEL数 */
#define MED_COMP_CHANNEL_MONO           (1)                                      /* 单声道数据 */
#define MED_COMP_CHANNEL_STEREO         (2)                                      /* 立体声数据 */

/* PCM补偿滤波的滤波系数最大值，因为NV项的长度是有限的 */
#define AUDIO_PCM_COMP_COEF_LEN_MAX      (128)

/* 获取全局变量 */
#define MED_PREEMP_GetObjPtr            (&g_astMedPreempObjPool[0])             /* 获取内存块全局变量地址 */
#define MED_PREEMP_GetObjInfoPtr        (&g_stMedPreempObjInfo)                 /* 获取内存块信息的全局变量地址 */
#define MED_DEEMP_GetObjPtr             (&g_astMedDeempObjPool[0])              /* 获取内存块全局变量地址 */
#define MED_DEEMP_GetObjInfoPtr         (&g_stMedDeempObjInfo)                  /* 获取内存块信息的全局变量地址 */
#define MED_HPF_GetObjPtr               (&g_astMedHpfObjPool[0])                /* 获取内存块全局变量地址 */
#define MED_HPF_GetObjInfoPtr           (&g_stMedHpfObjInfo)                    /* 获取内存块信息的全局变量地址 */
#define MED_COMP_GetObjPtr              (&g_astMedCompObjPool[0])               /* 获取内存块全局变量地址 */
#define MED_COMP_GetObjInfoPtr          (&g_stMedCompObjInfo)                   /* 获取内存块信息的全局变量地址 */

/* 临时全局数组, 供滤波临时使用 */
extern VOS_INT32                        g_aswMedFiltTmpLen320[CODEC_FRAME_LENGTH_WB];

#define MED_FILT_GetswVecTmp320Ptr()    (&g_aswMedFiltTmpLen320[0])             /* 临时全局数组指针 长度320 INT32 */


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
 实体名称  : MED_IIR32_PARA_STRU
 功能描述  : 高精度IIR滤波器维护结构体
*****************************************************************************/
typedef struct
{
    VOS_INT32                          *pswOutFeedBack;                         /* 输出反馈信号缓存指针 */
    VOS_INT16                          *pshwDen;                                /* 滤波器系数指针 */
    VOS_INT16                          *pshwNum;                                /* 滤波器系数指针 */
    VOS_INT32                           swIirLen;                               /* IIR滤波器的长度 */
}MED_IIR32_PARA_STRU;

/*****************************************************************************
 实体名称  : MED_IIR_PARA_STRU
 功能描述  : IIR滤波器维护结构体
*****************************************************************************/
typedef struct
{
    VOS_INT16                          *pshwDen;                                /* 滤波器系数指针 */
    VOS_INT16                          *pshwNum;                                /* 滤波器系数指针 */
    VOS_INT32                           swIirLen;                               /* IIR滤波器的长度 */
}MED_IIR_PARA_STRU;

/*****************************************************************************
 实体名称  : MED_PREEMP_NV_OBJ_STRU
 功能描述  : 预加重NV项维护结构体
*****************************************************************************/
typedef struct
{
    CODEC_SWITCH_ENUM_UINT16            enEnable;                               /* 配置项 使能标志 */
    VOS_INT16                           shwCoef;                                /* 配置项 预加重的系数 */
} MED_PREEMP_NV_STRU;


/*****************************************************************************
 实体名称  : MED_PREEMP_OBJ_STRU
 功能描述  : 预加重维护结构体
*****************************************************************************/
typedef struct
{
    MED_OBJ_HEADER
    MED_PREEMP_NV_STRU                  stNv;                                   /* NV项 */
    VOS_INT16                           shwFrmLen;                              /* 帧的长度 */
    VOS_INT16                           shwPreSignal;                           /* 输入信号的最后一点 */
} MED_PREEMP_OBJ_STRU;

/*****************************************************************************
 实体名称  : MED_DEEMP_NV_OBJ_STRU
 功能描述  : 预加重NV项维护结构体
*****************************************************************************/
typedef struct
{
    CODEC_SWITCH_ENUM_UINT16            enEnable;                               /* 配置项 使能标志 */
    VOS_INT16                           shwCoef;                                /* 配置项 预加重的系数 */
} MED_DEEMP_NV_STRU;

/*****************************************************************************
 实体名称  : MED_DEEMP_OBJ_STRU
 功能描述  : 去加重维护结构体
*****************************************************************************/
typedef struct
{
    MED_OBJ_HEADER
    MED_DEEMP_NV_STRU                   stNv;                                   /* NV项 */
    VOS_INT16                           shwFrmLen;                              /* 帧的长度 */
    VOS_INT16                           shwPreOut;                              /* 输出信号的最后一点 */
} MED_DEEMP_OBJ_STRU;

/*****************************************************************************
 实体名称  : MED_HPF_NV_OBJ_STRU
 功能描述  : 高通滤波器NV项维护结构体
*****************************************************************************/
typedef struct
{
    CODEC_SWITCH_ENUM_UINT16            enEnable;                               /* 配置项 使能标志 */
    VOS_INT16                           ashwDen[MED_HPF_LEN];                   /* 滤波器系数 */
    VOS_INT16                           ashwNum[MED_HPF_LEN];                   /* 滤波器系数 */
    VOS_INT16                           shwReserved;
} MED_HPF_NV_STRU;

/*****************************************************************************
 实体名称  : MED_HPF_OBJ_STRU
 功能描述  : 高通滤波结构体
*****************************************************************************/
typedef struct
{
    MED_OBJ_HEADER
    MED_HPF_NV_STRU                     stNv;
    VOS_INT16                           shwFrmLen;                              /* 帧的长度 */
    VOS_INT16                           ashwPreIn[MED_HPF_HISTORY_BUFF_LEN];    /* 缓存上一帧滤波前信号 */
    VOS_INT32                           aswPreOut[MED_HPF_HISTORY_BUFF_LEN];    /* 缓存上一帧滤波前信号 */
    MED_IIR32_PARA_STRU                 stFiltPara;                             /* 用于滤波的结构体 */
} MED_HPF_OBJ_STRU;

/*****************************************************************************
 实体名称  : MED_COMP_NV_STRU
 功能描述  : 频响补偿NV项维护结构体
*****************************************************************************/
typedef struct
{
    CODEC_SWITCH_ENUM_UINT16            enEnable;                               /* 配置项 使能标志 */
    VOS_INT16                           shwCompGain;                            /* 配置项 施加增益（左移） */
    VOS_INT16                           shwCoefLen;                             /* 配置项 滤波器的长度 */
    VOS_INT16                           ashwCompCoef[(2 * MED_COMP_FILT_LEN_MAX) - 1];    /* 配置项 滤波器的系数 */
} MED_COMP_NV_STRU;

/*****************************************************************************
 实体名称  : MED_COMP_OBJ_STRU
 功能描述  : 频响补偿结构体
*****************************************************************************/
typedef struct
{
    MED_OBJ_HEADER
    MED_COMP_NV_STRU                    stNv;                                   /* NV项 */
    VOS_INT16                           shwFrmLen;                              /* 帧的长度 */
    VOS_INT16                           ashwPreIn[MED_COMP_PRE_LEN_MAX];        /* 缓存的上一帧信号 */
    VOS_INT16                           shwReserved2;
} MED_COMP_OBJ_STRU;

/*****************************************************************************
 实体名称  : PP_AUDIO_PCM_COMP_OBJ_STRU
 功能描述  : PCM补偿滤波控制结构体
*****************************************************************************/
typedef struct
{
    CODEC_SWITCH_ENUM_UINT16            enPcmCompEnable;                        /* PCM补偿滤波功能是否打开 */
    VOS_INT16                           shwCompGain;                            /* 增益 */
    VOS_INT16                           shwM;                                   /* 补偿滤波FIR滤波器的长度，单位byte */
    VOS_INT16                           shwN;                                   /* 补偿滤波输入PCM码流的长度，单位byte */
    VOS_INT16                          *pshwCoef;                               /* 滤波系数序列，由NV项决定 */
    VOS_INT16                          *pashwIn[MED_COMP_CHANNEL_MAX];          /* 输入BUFF指针，长度为N+M-1，包含历史信息M-1，和当前的长度N */
    VOS_INT16                          *pashwPreInBuff[MED_COMP_CHANNEL_MAX];   /* 先前输入的历史信息 */
    VOS_INT16                          *pashwOut[MED_COMP_CHANNEL_MAX];         /* 输出BUFF指针 */
}PP_AUDIO_PCM_COMP_OBJ_STRU;

/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/
extern MED_PREEMP_OBJ_STRU    g_astMedPreempObjPool[MED_PREEMP_MAX_OBJ_NUM];    /* 预加重滤波器实体资源池 */
extern MED_DEEMP_OBJ_STRU     g_astMedDeempObjPool[MED_DEEMP_MAX_OBJ_NUM];      /* 去加重滤波器实体资源池 */
extern MED_HPF_OBJ_STRU       g_astMedHpfObjPool[MED_HPF_MAX_OBJ_NUM];          /* 高通滤波器实体资源池 */
extern MED_COMP_OBJ_STRU      g_astMedCompObjPool[MED_COMP_MAX_OBJ_NUM];        /* 频响补偿滤波器实体资源池 */

/*****************************************************************************
  10 函数声明
*****************************************************************************/

#if 0
extern VOS_UINT32 MED_AUDIO_COMP_CreateAndSetPara (
                       VOS_VOID                   *pstInstance,
                       PP_AUDIO_PCM_COMP_NV_STRU  *pstNv);
extern VOS_VOID MED_AUDIO_COMP_DataDeinterleave(VOS_VOID *pvIn, VOS_INT16 shwInSize, VOS_VOID *pvOut0, VOS_VOID *pvOut1);
extern VOS_VOID MED_AUDIO_COMP_DataInterleave(VOS_VOID *pvIn0, VOS_VOID *pvIn1, VOS_INT16 shwInSize, VOS_VOID *pvOut);
extern VOS_UINT32 MED_AUDIO_COMP_Destroy(VOS_VOID *pstInstance);
extern VOS_UINT32 MED_AUDIO_COMP_GetPara(
                       VOS_VOID                   *pstInstance,
                       PP_AUDIO_PCM_COMP_NV_STRU  *pstNv);
extern VOS_UINT32 MED_AUDIO_COMP_Main(
                       VOS_VOID               *pstInstance,
                       VOS_INT16              *pshwIn,
                       VOS_INT16               shwInLen,
                       VOS_INT16              *pshwOut,
                       VOS_UINT32              uwChanNum);
#endif
extern VOS_VOID* MED_COMP_Create(VOS_VOID);
extern VOS_UINT32 MED_COMP_Destroy(VOS_VOID **ppstObj);
extern VOS_UINT32 MED_COMP_GetPara(
                       VOS_VOID               *pstInstance,
                       VOS_INT16              *pshwParam,
                       VOS_INT16               shwParaLen);
extern VOS_UINT32 MED_COMP_Main(
                       VOS_VOID               *pstInstance,
                       VOS_INT16              *pshwIn,
                       VOS_INT16              *pshwOut);
extern VOS_UINT32 MED_COMP_SetPara (
                       VOS_UINT32              swSampleRate,
                       VOS_VOID               *pstInstance,
                       VOS_INT16              *pshwParam,
                       VOS_INT16               shwParaLen);
extern VOS_VOID* MED_DEEMP_Create(VOS_VOID);
extern VOS_UINT32 MED_DEEMP_Destroy(VOS_VOID **ppstObj);
extern VOS_UINT32 MED_DEEMP_GetPara(
                       VOS_VOID               *pstInstance,
                       VOS_INT16              *pshwParam,
                       VOS_INT16               shwParaLen);
extern VOS_UINT32 MED_DEEMP_Main(
                       VOS_VOID               *pstInstance,
                       VOS_INT16              *pshwIn,
                       VOS_INT16              *pshwOut);
extern VOS_UINT32 MED_DEEMP_SetPara (
                       VOS_VOID               *pstInstance,
                       VOS_INT16              *pshwParam,
                       VOS_INT16               shwParaLen);
extern VOS_VOID MED_FILT_Fir (
                       VOS_INT16              *pshwIn,
                       VOS_INT16              *pshwCoef,
                       VOS_INT16               shwInLen,
                       VOS_INT32               swFirLen,
                       VOS_INT16               shwGain,
                       VOS_INT16              *pshwOut);
extern VOS_VOID MED_FILT_Iir (
                       MED_IIR_PARA_STRU      *pstFiltPara,
                       VOS_INT16              *pshwIn,
                       VOS_INT16               shwInLen,
                       VOS_INT16              *pshwOut);
extern VOS_VOID MED_FILT_Iir32 (
                       MED_IIR32_PARA_STRU    *pstFiltPara,
                       VOS_INT16              *pshwIn,
                       VOS_INT16               shwInLen,
                       VOS_INT16              *pshwOut);
extern VOS_VOID* MED_HPF_Create(VOS_VOID);
extern VOS_UINT32 MED_HPF_Destroy(VOS_VOID **ppstObj);
extern VOS_UINT32 MED_HPF_GetPara(
                       VOS_VOID               *pstInstance,
                       VOS_INT16              *pshwParam,
                       VOS_INT16               shwParaLen);
extern VOS_UINT32 MED_HPF_Main(
                       VOS_VOID               *pstInstance,
                       VOS_INT16              *pshwIn,
                       VOS_INT16              *pshwOut);
extern VOS_UINT32 MED_HPF_SetPara (
                       VOS_VOID               *pstInstance,
                       VOS_INT16              *pshwParam,
                       VOS_INT16               shwParaLen);
extern VOS_VOID* MED_PREEMP_Create(VOS_VOID);
extern VOS_UINT32 MED_PREEMP_Destroy(VOS_VOID **ppstObj);
extern VOS_UINT32 MED_PREEMP_GetPara(
                       VOS_VOID               *pstInstance,
                       VOS_INT16              *pshwParam,
                       VOS_INT16               shwParaLen);
extern VOS_UINT32 MED_PREEMP_Main(
                       VOS_VOID               *pstInstance,
                       VOS_INT16              *pshwIn,
                       VOS_INT16              *pshwOut);
extern VOS_UINT32 MED_PREEMP_SetPara (
                       VOS_VOID               *pstInstance,
                       VOS_INT16              *pshwParam,
                       VOS_INT16               shwParaLen);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* med_preemp.h */

