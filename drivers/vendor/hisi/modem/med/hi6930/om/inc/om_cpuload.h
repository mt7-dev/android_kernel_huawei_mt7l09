/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : om_cpuload.h
  版 本 号   : 初稿
  作    者   :
  生成日期   : 2013年12月28日
  最近修改   :
  功能描述   : om_cpuload.h 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2013年12月28日
    作    者   :
    修改内容   : 创建文件

******************************************************************************/
#ifndef __OM_CPULOAD_H__
#define __OM_CPULOAD_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include  "vos.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define OM_CPULOAD_MAX_FUNC_NUM  (1024)

/*****************************************************************************
  3 枚举定义
*****************************************************************************/
/* 性能统计函数枚举 */
enum OM_CPULOAD_PERFORMANCE_CAL_ENUM
{
    OM_CPULOAD_PERFORMANCE_TxPp_1MIC            = 0,        //上行
    OM_CPULOAD_PERFORMANCE_TxPp_2MIC,
    OM_CPULOAD_PERFORMANCE_1MIC_AECANR_Main,
    OM_CPULOAD_PERFORMANCE_2MIC_AECANR_Main,
    OM_CPULOAD_PERFORMANCE_EANR_1MIC_Main_TX,
    OM_CPULOAD_PERFORMANCE_AIG_Main_TX,
    OM_CPULOAD_PERFORMANCE_COMP_Main_TX,
    OM_CPULOAD_PERFORMANCE_PREEMP_Main_TX,
    OM_CPULOAD_PERFORMANCE_DEEMP_Main_TX,
    OM_CPULOAD_PERFORMANCE_HPF_Main_TX,
    OM_CPULOAD_PERFORMANCE_ANR_2MIC_TdProcess,
    OM_CPULOAD_PERFORMANCE_AEC_2MIC_Time2Freq,
    OM_CPULOAD_PERFORMANCE_AEC_2MIC_NlpMain,
    OM_CPULOAD_PERFORMANCE_ANR_2MIC_FdProcess,
    OM_CPULOAD_PERFORMANCE_AEC_2MIC_Freq2Time,
    OM_CPULOAD_PERFORMANCE_AEC_Time2Freq_TX,
    OM_CPULOAD_PERFORMANCE_AEC_Freq2Time_TX,
    OM_CPULOAD_PERFORMANCE_AEC_1MIC_NlpMain,
    OM_CPULOAD_PERFORMANCE_AEC_DtdMuteDetect,
    OM_CPULOAD_PERFORMANCE_AEC_AfMain,
    OM_CPULOAD_PERFORMANCE_AEC_Dtd,
    OM_CPULOAD_PERFORMANCE_AEC_DtdLine,
    OM_CPULOAD_PERFORMANCE_AEC_DtdNline,
    OM_CPULOAD_PERFORMANCE_AEC_HF_AfMain,
    OM_CPULOAD_PERFORMANCE_AEC_HF_NlpMain,
    OM_CPULOAD_PERFORMANCE_AEC_UpdateTx,
    OM_CPULOAD_PERFORMANCE_AEC_UpdateRx,

    OM_CPULOAD_PERFORMANCE_RxPp,       //下行
    OM_CPULOAD_PERFORMANCE_HPF_Main_RX,
    OM_CPULOAD_PERFORMANCE_ANR_MBDRC_Rx_Main,
    OM_CPULOAD_PERFORMANCE_AIG_Main_RX,
    OM_CPULOAD_PERFORMANCE_COMP_Main_RX,
    OM_CPULOAD_PERFORMANCE_DEVGAIN_Main_RX,
    OM_CPULOAD_PERFORMANCE_ANR_RX_FFTProcess,
    OM_CPULOAD_PERFORMANCE_EANR_1MIC_Main_RX,
    OM_CPULOAD_PERFORMANCE_MBDRC_Main_RX,
    OM_CPULOAD_PERFORMANCE_ANR_RX_IFFTProcess,

    OM_CPULOAD_PERFORMANCE_BUTT

};

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
    VOS_UINT32  auwMaxTime[OM_CPULOAD_MAX_FUNC_NUM];             /* 最大消耗时间 */
    VOS_UINT32  auwMinTime[OM_CPULOAD_MAX_FUNC_NUM];             /* 最小消耗时间 */
    VOS_UINT32  auwEnterTS[OM_CPULOAD_MAX_FUNC_NUM];             /* 进入函数时的时刻 */
    VOS_UINT32  auwEnterTimes[OM_CPULOAD_MAX_FUNC_NUM];          /* 调用次数 */
}OM_CPULOAD_COUNT_STRU;

/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern void OM_CPULOAD_FuncEnter(VOS_UINT32 uwFuncId);
extern void OM_CPULOAD_FuncExit(VOS_UINT32 uwFuncId);
extern void OM_CPULOAD_Init();



#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of om_cpuload.h */
