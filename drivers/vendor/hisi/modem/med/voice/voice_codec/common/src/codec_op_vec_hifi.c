/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : med_com_vec_hifi.c
  版 本 号   : 初稿
  作    者   : 金剑
  生成日期   : 2011年8月20日
  最近修改   :
  功能描述   : 语音处理基于HiFi加速指令的向量运算函数实现
  函数列表   :CODEC_OpVcAnd
              CODEC_OpVcMac
              CODEC_OpVcMultQ15Add
              CODEC_OpVcMultR
              CODEC_OpVcMultScale
              CODEC_OpVecCpy
              CODEC_OpVecMax
              CODEC_OpVecMaxAbs
              CODEC_OpVecMin
              CODEC_OpVecScaleMac
              CODEC_OpVecSet
              CODEC_OpVecShl
              CODEC_OpVecShr
              CODEC_OpVecSum
              CODEC_OpVvAdd
              CODEC_OpVvAdd32
              CODEC_OpVvFormWithCoef
              CODEC_OpVvFormWithDimi
              CODEC_OpVvFormWithQuar
              CODEC_OpVvMac
              CODEC_OpVvMsu
              CODEC_OpVvSub

  修改历史   :
  1.日    期   : 2011年8月20日
    作    者   : 金剑
    修改内容   : 创建文件

******************************************************************************/

/*****************************************************************************
  1 头文件包含
*****************************************************************************/

#ifndef _MED_C89_
#include <xtensa/tie/xt_hifi2.h>
#include "codec_op_vec.h"
#endif


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 全局变量定义
*****************************************************************************/


/*****************************************************************************
  3 函数实现
*****************************************************************************/

/* HIFI平台加速使用 */
#ifndef _MED_C89_

#endif /* _MED_C89_ */



#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

