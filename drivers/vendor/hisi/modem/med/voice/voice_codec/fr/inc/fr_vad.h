/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : fr_vad.h
  版 本 号   : 初稿
  作    者   : 谢明辉 58441
  生成日期   : 2011年5月3日
  最近修改   :
  功能描述   : fr_vad.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2011年5月3日
    作    者   : 谢明辉 58441
    修改内容   : 创建文件

******************************************************************************/

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#ifndef __FR_VAD_H__
#define __FR_VAD_H__

#include "codec_op_etsi.h"
#include "fr_interface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define FR_VAD_MAX_VALUE_16BITS         (32767)                                 /*16比特有符号数最大值*/
#define FR_VAD_MAX_VALUE_15BITS         (16384)                                 /*15比特有符号数最大值*/
#define FR_VAD_MAX_VALUE_13BITS         (4095)                                  /*13比特有符号数最大值*/

#define FR_VAD_NUM_OF_RVAD              (9)                                     /*自适应滤波系数个数*/
#define FR_VAD_NUM_OF_SACF              (27)                                    /*自相关系数个数*/
#define FR_VAD_NUM_OF_SAV0              (36)                                    /*自相关系数均值个数*/

#define FR_VAD_INIT_VALUE_RVAD0         (24576)                                 /*第0个自适应滤波系数初始化值*/
#define FR_VAD_INIT_VALUE_RVAD1         (-16384)                                /*第1个自适应滤波系数初始化值*/
#define FR_VAD_INIT_VALUE_RVAD2         (4096)                                  /*第2个自适应滤波系数初始化值*/
#define FR_VAD_INIT_VALUE_NORM_RVAD     (7)                                     /*rvad比例因子初始化值*/
#define FR_VAD_INIT_VALUE_THVAD_E       (17)                                    /*VAD判定自适应阈值指数初始化值*/
#define FR_VAD_INIT_VALUE_THVAD_M       (31250)                                 /*VAD判定自适应阈值尾数初始化值*/
#define FR_VAD_INIT_VALUE_HANG_COUNT    (-1)                                    /*hangover帧数累计初始化值*/
#define FR_VAD_INIT_VALUE_OLD_LAG       (40)                                    /*LTP周期初始化值*/
#define FR_VAD_INIT_VALUE_TONE          (0)                                     /*TONE音初始化值*/

#define FR_VAD_SET_VALUE_ACF0_E         (32)                                    /*信号能量指数设定值*/
#define FR_VAD_SET_VALUE_SCAL           (10)                                    /*比例因子设定值*/
#define FR_VAD_SET_VALUE_COEF0          (16384)                                 /*LPC分析滤波系数0设定值*/
#define FR_VAD_SET_VALUE_SAV0           (4095)                                  /*自相关系数均值0设定值*/
#define FR_VAD_SET_VALUE_DM             (32768)                                 /*差异系数设定值*/
#define FR_VAD_SET_VALUE_DM_DIFF        (3277)                                  /*差异系数差分设定值*/
#define FR_VAD_SET_VALUE_ADAPT_COUNT    (9)                                     /*自适应帧数累计设定值*/
#define FR_VAD_SET_VALUE_HANG_COUNT     (5)                                     /*hangover帧数累计设定值*/
#define FR_VAD_SET_VALUE_BURST_COUNT    (3)                                     /*语音Burst帧数累计设定值*/

#define FR_VAD_THRESH_E_PTH             (19)                                    /*自相关系数ACF[0]最低阈值指数部分*/
#define FR_VAD_THRESH_M_PTH             (18750)                                 /*自相关系数ACF[0]最低阈值尾数部分*/
#define FR_VAD_THRESH_E_PLEV            (17)                                    /*VAD判决能量阈值指数部分*/
#define FR_VAD_THRESH_M_PLEV            (25000)                                 /*VAD判决能量阈值尾数部分*/
#define FR_VAD_THRESH_E_MARGIN          (27)                                    /*信号能量边缘阈值指数部分*/
#define FR_VAD_THRESH_M_MARGIN          (19531)                                 /*信号能量边缘阈值尾数部分*/

#define FR_AR_LEN                       (9)                                     /*自相关系数长度*/

/*****************************************************************************
  3 枚举定义
*****************************************************************************/
/*****************************************************************************
 实体名称  : FR_COMPARE_ENUM
 功能描述  : 复数比较大小取值定义
*****************************************************************************/
enum FR_COMPARE_ENUM
{
    FR_COMPARE_BIG                      = 0,
    FR_COMPARE_SMALL                    = 1,
    FR_COMPARE_BUTT
};
typedef UWord16 FR_COMPARE_ENUM_UINT16;

/*****************************************************************************
  4 消息头定义
*****************************************************************************/


/*****************************************************************************
  5 消息定义
*****************************************************************************/


/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/

/************************************************************************
实体名称: FR_FLOAT_STRU
功能描述: 浮点数据类型结构体定义
************************************************************************/
typedef struct
{
    Word16                              shwExp;                                 /* 浮点指数 */
    Word16                              shwMnts;                                /* 浮点尾数 */
}FR_FLOAT_STRU;

/************************************************************************
实体名称: FR_VAD_STATE_STRU
功能描述: FR上行处理VAD检测主结构体
************************************************************************/
typedef struct
{
    Word16                              shwNormRvad;                            /*rvad比例因子*/
    Word16                              shwReserved;
    FR_FLOAT_STRU                       stThvad;                               /*VAD判定自适应阈值*/
    Word16                              shwPtSacf;                              /*sacf延迟点数*/
    Word16                              shwPtSav0;                              /*sav0延迟点数*/
    Word32                              swLastDm;                               /*距离测量值*/
    Word16                              shwAdaptCnt;                            /*自适应帧数累计*/
    Word16                              shwBurstCnt;                            /*语音Burst帧数累计*/
    Word16                              shwHangCnt;                             /*hangover帧数累计*/
    Word16                              shwOldLagCnt;                           /*周期累计*/
    Word16                              shwVeryOldLagCnt;
    Word16                              shwOldLag;                              /*LTP周期*/
    Word16                              shwPtch;                                /*周期检测标志*/
    Word16                              ashwRvad[9];                            /*自适应滤波系数*/
    Word32                              aswAcf[27];                             /*自相关系数*/
    Word32                              aswSav0[36];                            /*自相关系数均值*/
} FR_VAD_STATE_STRU;

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

extern void FR_VadAdaptByPvadWithFac(FR_FLOAT_STRU stPvad, FR_FLOAT_STRU *pstThvad);
extern void FR_VadAdaptByPvadWithMargin(FR_FLOAT_STRU stPvad, FR_FLOAT_STRU *pstThvad);
extern void FR_VadAdaptThreshold(
                Word16                  shwSpecStat,
                Word16                  shwTone,
                Word16                  shwNormRav1,
                FR_FLOAT_STRU           stAcf0,
                FR_FLOAT_STRU           stPvad,
                Word16                  ashwRav1[],
                FR_VAD_STATE_STRU      *pstVadState);
extern void FR_VadAverAcf(
                const Word32            aswAcf[],
                const Word16            shwScalvad,
                Word32                  aswAv0[],
                Word32                  aswAv1[],
                FR_VAD_STATE_STRU      *pstVadState);
extern FR_COMPARE_ENUM_UINT16 FR_VadCompareFloat(
                FR_FLOAT_STRU           stFloatx,
                FR_FLOAT_STRU           stFloaty);
extern Word16 FR_VadCompareSpectral(
                Word16                  ashwRav1[],
                Word16                  shwNormRav1,
                Word32                  aswAv0[],
                FR_VAD_STATE_STRU      *pstVadState);
extern Word16 FR_VadComputeEnergy(
                const Word32            aswAcf[],
                Word16                  shwScalauto,
                FR_FLOAT_STRU          *pstAcf0,
                FR_FLOAT_STRU          *pstPvad,
                FR_VAD_STATE_STRU      *pstVadState);
extern void FR_VadComputeRav1(
                Word16                  ashwAav1[],
                Word16                  ashwRav1[],
                Word16                 *pshwNormRav1);
extern Word16 FR_VadDecideAdapt(Word16 shwPtch, Word16 shwSpecStat, Word16 shwTone);
extern FR_VAD_STATUS_ENUM_UINT16 FR_VadDeciVad(FR_FLOAT_STRU stPvad, FR_FLOAT_STRU stThvad);
extern FR_VAD_STATUS_ENUM_UINT16 FR_VadDetect(
                Word32                 *pswAcf,
                Word16                  shwScalauto,
                Word16                 *pshwNc,
                FR_VAD_STATE_STRU      *pstVadState);
extern void FR_VadDetectPeriod(FR_VAD_STATE_STRU *pstVadState);
extern FR_VAD_STATUS_ENUM_UINT16 FR_VadHangover(
                FR_VAD_STATUS_ENUM_UINT16                   enVvad,
                Word16                                     *pshwBurstCount,
                Word16                                     *pshwHangCount);
extern void FR_VadInit(FR_VAD_STATE_STRU *pstVadState);
extern void FR_VadPredictValues(
                Word32                  aswAv1[],
                Word16                  ashwRav1[],
                Word16                 *pshwNormrav1);
extern void FR_VadSchurRecursion(Word32 aswAv1[], Word16 ashwVpar[]);
extern void FR_VadUpdatePeriod(
                Word16                 *pshwLags,
                FR_VAD_STATE_STRU      *pstVadState);
extern void FR_VadUpdateStep(Word16 ashwVpar[], Word16 ashwAav1[]);

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of fr_vad.h */
