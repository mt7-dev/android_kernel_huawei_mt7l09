

#ifndef __MED_GAIN_H__
#define __MED_GAIN_H__


/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "CodecInterface.h"
#include "codec_com_codec.h"
#include "med_pp_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  2 宏定义
*****************************************************************************/
/* 基本信息 */
#define MED_SIDETONE_MAX_OBJ_NUM        (1)                                     /* 侧音增益最大实体数目 */
#define MED_SIDETONE_PARAM_LEN          (1)                                     /* 配置项长度 */
#define MED_GAIN_MAX                    (12)                                    /* 最大侧音增益 */
#define MED_GAIN_MIN                    (-80)                                   /* 最小侧音增益 */
#define MED_GAIN_BASE                   (80)                                    /* 0 dB对应的乘法因子位置 */
#define MED_DEVGAIN_MAX_OBJ_NUM         (2)                                     /* 设备增益最大实体数目 */
#define MED_DEVGAIN_PARAM_LEN           (1)                                     /* 配置项长度 */
#define MED_SIDETONE_RX_LOUD            ((VOS_INT32)166553600)                 /* 下行大信号门限，对应-30dB */
#define MED_PP_AGC_RX_DELAY             (27)                                    /* 非大信号时侧音从关闭至打开的最大延迟时间， 540ms */

/* 获取全局变量 */
#define MED_SIDETONE_GetObjPtr          (&g_astMedSideToneObjPool[0])           /* 获取内存块全局变量地址 */
#define MED_GAIN_Db2Linear(shwIdx)      (g_ashwGainTab[(MED_GAIN_BASE + (shwIdx))]) /* dB增益转为线性增益 */
#define MED_DEVGAIN_GetObjPtr           (&g_astMedDevGainObjPool[0])            /* 获取内存块全局变量地址 */
#define MED_SIDETONE_GetObjInfoPtr      (&g_stMedSideToneObjInfo)               /* 获取内存块信息的全局变量地址 */
#define MED_DEVGAIN_GetObjInfoPtr       (&g_stMedDevGainObjInfo)                /* 获取内存块信息的全局变量地址 */

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
 实体名称  : MED_SIDETONE_STRU
 功能描述  : 侧音增益结构体
*****************************************************************************/
typedef struct
{
    MED_OBJ_HEADER
    VOS_INT16                           shwSetGain;                             /* 配置项 侧音增益，Q15 */
    VOS_INT16                           shwCurGain;                             /* 当前侧音增益，Q15 */
    VOS_INT16                           shwFrmLen;                              /* 帧长 */
    VOS_INT16                           ashwFrmDat[CODEC_FRAME_LENGTH_WB];  /* TX帧缓存 */
    VOS_INT16                           shwReserved2;
    VOS_INT32                           swSTMuteCount;

} MED_SIDETONE_OBJ_STRU;

/*****************************************************************************
 实体名称  : MED_DEVGAIN_STRU
 功能描述  : 设备增益维护结构体
*****************************************************************************/
typedef struct
{
    MED_OBJ_HEADER
    VOS_INT16                           shwGain;                                /* 配置项 增益 */
    VOS_INT16                           shwFrmLen;                              /* 帧的长度 */
} MED_DEVGAIN_OBJ_STRU;


/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/
extern VOS_INT16 g_ashwGainTab[93];
extern MED_SIDETONE_OBJ_STRU    g_astMedSideToneObjPool[MED_SIDETONE_MAX_OBJ_NUM];/* 侧音叠加滤波器实体资源池 */
extern MED_DEVGAIN_OBJ_STRU     g_astMedDevGainObjPool[MED_DEVGAIN_MAX_OBJ_NUM];

/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern VOS_VOID* MED_DEVGAIN_Create(VOS_VOID);
extern VOS_UINT32 MED_DEVGAIN_Destroy(VOS_VOID **ppstObj);
extern VOS_UINT32 MED_DEVGAIN_GetPara(
                       VOS_VOID               *pstInstance,
                       VOS_INT16              *pshwParam,
                       VOS_INT16               shwParaLen);
extern VOS_UINT32 MED_DEVGAIN_Main(
                       VOS_VOID               *pstInstance,
                       VOS_INT16              *pshwIn,
                       VOS_INT16              *pshwOut);
extern VOS_UINT32 MED_DEVGAIN_SetPara (
                       VOS_VOID               *pstInstance,
                       VOS_INT16              *pshwParam,
                       VOS_INT16               shwParaLen);
extern VOS_UINT32 MED_SIDESTONE_UpdateTx (
                       VOS_VOID               *pstSideToneInstance,
                       VOS_INT16              *pshwTxIn);
extern VOS_VOID* MED_SIDETONE_Create(VOS_VOID);
extern VOS_UINT32 MED_SIDETONE_Destroy(VOS_VOID **ppstObj);
extern VOS_UINT32 MED_SIDETONE_GetPara(
                       VOS_VOID               *pstInstance,
                       VOS_INT16              *pshwParam,
                       VOS_INT16               shwParaLen);
extern VOS_UINT32 MED_SIDETONE_Main(
                       VOS_VOID               *pstInstance,
                       VOS_INT16              *pshwRxIn,
                       VOS_INT16              *pshwOut);
extern VOS_UINT32 MED_SIDETONE_SetPara (
                       VOS_VOID               *pstInstance,
                       VOS_INT16              *pshwParam,
                       VOS_INT16               shwParaLen);

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of med_gain.h */


