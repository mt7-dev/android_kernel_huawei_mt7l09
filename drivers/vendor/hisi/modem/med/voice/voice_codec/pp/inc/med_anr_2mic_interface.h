
#ifndef __MED_ANR_2MIC_INTERFACE_H__
#define __MED_ANR_2MIC_INTERFACE_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "codec_typedefine.h"
#include "codec_com_codec.h"
#include "med_anr_2mic_td.h"
#include "med_anr_2mic_fd.h"
#include "med_anr_2mic_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* 访问全局变量 */
#define MED_ANR_2MIC_GetObjPtr()        (&g_stMed2MicObj)                       /* 获取2MIC全局变量*/
#define MED_ANR_2MIC_GetObjInfoPtr()    (&g_stMed2MicObjInfo)                   /* 获取2MIC全局对象信息*/
#define MED_ANR_2MIC_GetWinNb()         (g_ashwMedAnr2MicWinNb)                 /* 窄带时频变换窗系数 */
#define MED_ANR_2MIC_GetWinWb()         (g_ashwMedAnr2MicWinWb)                 /* 宽带时频变换窗系数 */

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define MED_ANR_2MIC_MAX_OBJ_NUM             (1)                                /* 前后处理中双麦克ANR的个数*/
#define MED_ANR_2MIC_PARAM_LEN               (13)                                /* ANR_2MIC的NV项个数 */

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
 实体名称  : MED_ANR_2MIC_STRU
 功能描述  : 2MIC内部状态结构
*****************************************************************************/
typedef struct
{
    MED_OBJ_HEADER
    CODEC_SWITCH_ENUM_UINT16            enEnable;                               /* 2MIC使能开关 */
    MED_2MIC_NV_STRU                    st2MicNv;                               /* 2MIC的Nv项 */
    MED_2MIC_TDPROCESS_STRU            *pstTdProcess;                           /* 2MIC时域处理模块*/
    MED_2MIC_COMM_STRU                  stComCfg;                               /* 2MIC公共模块*/
    MED_2MIC_DATAFLOW_MC_STRU           stDataFlowMc;                           /* 2MIC主麦克信号数据流结构体*/
    MED_2MIC_DATAFLOW_RC_STRU           stDataFlowRc;                           /* 2MIC辅麦克信号数据流结构体*/
    VOS_INT16                           shwReserve;
    MED_2MIC_POSTFILTER_STRU           *pstPostFilter;                          /* 2MIC频域后滤波模块结构体*/
} MED_ANR_2MIC_STRU;

/****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/
extern MED_ANR_2MIC_STRU   g_stMed2MicObj;                                             /* 双麦克降噪模块全局控制实体 */
extern MED_OBJ_INFO        g_stMed2MicObjInfo;
extern VOS_INT16           g_ashwMedAnr2MicWinNb[160];
extern VOS_INT16           g_ashwMedAnr2MicWinWb[320];
/*****************************************************************************
  10 函数声明
*****************************************************************************/

extern VOS_VOID* MED_ANR_2MIC_Create( VOS_VOID );
extern VOS_UINT32 MED_ANR_2MIC_Destroy( VOS_VOID **ppInstance );
extern VOS_UINT32 MED_ANR_2MIC_GetPara(
                       VOS_VOID                *pInstance,
                       VOS_INT16               *pshwPara,
                       VOS_INT16                shwParaLen);
extern VOS_UINT32  MED_ANR_2MIC_SetPara(
                       VOS_VOID                         *pInstance,
                       VOS_INT16                        *pshwPara,
                       VOS_INT16                         shwParaLen,
                       CODEC_SAMPLE_RATE_MODE_ENUM_INT32   enSampleRate);










#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of med_anr_2mic_interface.h */
