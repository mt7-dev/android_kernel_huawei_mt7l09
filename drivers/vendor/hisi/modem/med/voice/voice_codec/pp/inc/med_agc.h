
#ifndef __MED_AGC_H__
#define __MED_AGC_H__


/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "codec_typedefine.h"
#include "CodecInterface.h"
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
#define MED_AGC_CLEAR_RX_MUT           ((VOS_INT32)40960)                       /* AGC下行底噪清零阈值 -66dB 噪声值(160*16*16)*/
#define MED_AGC_INC_STEP_NORMAL         (19988)                                 /* 普通增加步长，Q14 */
#define MED_AGC_DEC_STEP_NORMAL         (27255)                                 /* 普通衰减步长，Q15 */
#define MED_AGC_DEC_STEP_FAST           (22670)                                 /* 快速衰减步长，Q15 */
#define MED_AGC_MAX_OBJ_NUM             (2)                                     /* AGC最大实体数目 */
#define MED_AGC_PARAM_LEN               (6)                                     /* 有效的配置项个数 */
#define MED_AGC_LASTGAIN_INIT           (1024)                                  /* 成员变量LastGain初始值 Q10 */
#define MED_AGC_AVERENG_MIN             (1)                                     /* 帧平均能量最小值 */

/* 获取全局变量 */
#define MED_AGC_GetObjPtr               (&g_astMedAgcObjPool[0])                /* 获取内存块全局变量地址 */
#define MED_AGC_GetObjInfoPtr           (&g_stMedAgcObjInfo)                    /* 获取内存块信息的全局变量地址 */

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
实体名称  : MED_AGC_NV_STRU
功能描述  : AGC NV项维护结构体
*****************************************************************************/
typedef struct
{
    CODEC_SWITCH_ENUM_UINT16              enEnable;                               /* 配置项 使能项 */
    VOS_INT16                           shwOffset;                              /* 配置项 偏移量，该变量在AGC计算中无作用，只供MED_AGC_GetPara查询用 */
    VOS_INT16                           shwMaxOut;                              /* 配置项 最大输出 */
    VOS_INT16                           shwFlrNoise;                            /* 配置项 底噪 */
    VOS_INT16                           shwMaxIn;                               /* 配置项 最大输入 */
    CODEC_SWITCH_ENUM_UINT16              enClearNoiseEn;                         /* 配置项 微小噪声清零使能项，该变量仅在AGC下行使用，通常关闭 */
}MED_AGC_NV_STRU;

/*****************************************************************************
实体名称  : MED_AGC_STRU
功能描述  : AGC项维护结构体
*****************************************************************************/
typedef struct
{
    MED_OBJ_HEADER
    MED_AGC_NV_STRU                     stNv;                                   /* NV项结构体 */
    VOS_INT16                           shwFrmLen;                              /* 帧的长度 */
    VOS_INT16                           shwLineK;                               /* 斜率K Q15 */
    VOS_INT16                           shwLineB;                               /* 上移幅度B */
    VOS_INT16                           shwLastGain;                            /* 上一帧的增益 */
} MED_AGC_OBJ_STRU;

/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/
extern MED_AGC_OBJ_STRU    g_astMedAgcObjPool[MED_AGC_MAX_OBJ_NUM];                    /* AGC实体资源池 */
extern MED_OBJ_INFO        g_stMedAgcObjInfo;                                          /* AGC信息 */

/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern VOS_VOID* MED_AGC_Create(VOS_VOID);
extern VOS_UINT32 MED_AGC_Destroy(VOS_VOID **ppstObj);
extern VOS_UINT32 MED_AGC_GetPara(
                       VOS_VOID               *pstInstance,
                       VOS_INT16              *pshwParam,
                       VOS_INT16               shwParaLen);
extern VOS_UINT32 MED_AGC_Main(
                       VOS_VOID               *pstAgcInstance,
                       VOS_INT16              *pshwIn,
                       VOS_INT16               shwVadFlag,
                       VOS_INT16              *pshwOut);
extern VOS_UINT32 MED_AGC_SetPara (
                       VOS_VOID               *pstInstance,
                       VOS_INT16              *pshwParam,
                       VOS_INT16               shwParaLen);
extern VOS_UINT32 MED_AGC_PreClearNoise(
                       VOS_VOID               *pstAgcInstance,
                       VOS_INT16              *psIn,
                       VOS_INT16              *psOut);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* med_agc.h */

