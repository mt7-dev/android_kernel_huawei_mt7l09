/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : med_com_netsi_hifi.c
  版 本 号   : 初稿
  作    者   : 金剑
  生成日期   : 2011年9月1日
  最近修改   :
  功能描述   : 非ETSI基本运算函数扩展，HIFI加速实现
  函数列表   :
  修改历史   :
  1.日    期   : 2011年9月1日
    作    者   : 金剑
    修改内容   : 创建文件

******************************************************************************/

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "codec_op_etsi.h"
#include "codec_op_netsi.h"

#ifndef _MED_C89_
#include <xtensa/tie/xt_hifi2.h>
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
#ifndef _MED_C89_

/*****************************************************************************
 函 数 名  : L_mpy_ls
 功能描述  : 16bi*32bit=32bit，右移15位
 输入参数  : Word32 swVar1      --乘数
             Word16 shwVar2     --被乘数
 输出参数  : 无
 返 回 值  : Word32             --相乘结果
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年9月1日
    作    者   : 金剑
    修改内容   : 新生成函数

*****************************************************************************/
Word32 L_mpy_ls(Word32 swVar1, Word16 shwVar2)
{
    ae_p24x2s   p0, p1, p2;
    ae_q56s     q0;

    /*
     * shwTemp = shr(extract_l(swVar1), 1);
     * shwTemp = (short)32767 & (short)shwTemp; */
    p0 = AE_CVTP24A16(swVar1);
    p0 = AE_SRLIP24(p0, 1);
    /* extract_h(swVar1) */
    p1 = AE_CVTP24A16X2_HH(swVar1, swVar1);

    p2 = AE_CVTP24A16(shwVar2);

    /*
     * swVarOut = L_mult(shwVar2, shwTemp) */
    q0 = AE_MULFS32P16S_LL(p0, p2);
    /* swVarOut = L_shr(swVarOut, 15); */
    q0 = AE_SRAIQ56(q0, 15);

    AE_MULAFS32P16S_LL(q0, p1, p2);

    return AE_TRUNCA32Q48(q0);

}
VOS_INT32 CODEC_OpL_mpy_32(VOS_INT32 L_var1, VOS_INT32 L_var2)
{
    ae_p24x2s p0, p1, p2, p3;
    ae_q56s   q0, q1, q2;

    p2 = AE_CVTP24A16(MAX_16);
    /* shwLow1 = shr(extract_l(L_var1), 1);*/
    p0 = AE_CVTP24A16(L_var1);
    p0 = AE_SRAIP24(p0, 1);
    /* shwLow1 = MAX_16 & shwLow1; */
    p0 = AE_ANDP48(p0, p2);

    /* shwLow2 = shr(extract_l(L_var2), 1);*/
    p1 = AE_CVTP24A16(L_var2);
    p1 = AE_SRAIP24(p1, 1);
    /* shwLow2 = MAX_16 & shwLow2;*/
    p1 = AE_ANDP48(p1, p2);

    /* shwHigh1 = extract_h(L_var1);*/
    p2 = AE_TRUNCP24A32X2(L_var1, L_var1);
    /* shwHigh2 = extract_h(L_var2);*/
    p3 = AE_TRUNCP24A32X2(L_var2, L_var2);

    /* swLow = L_mult(shwLow1, shwLow2);*/
    q0 = AE_MULFS32P16S_LL(p0, p1);
    /* swLow = L_shr(swLow, 16);*/
    q0 = AE_SRAIQ56(q0, 16);

    /* swMid1 = L_mult(shwHigh1, shwLow2);*/
    q1 = AE_MULFS32P16S_LL(p2, p1);
    /* swMid1 = L_shr(swMid1, 1);*/
    q1 = AE_SRAIQ56(q1, 1);
    /* swMid  = L_add(swMid1, swLow);*/
    q1 = AE_ADDQ56(q1, q0);

    /* swMid2 = L_mult(shwHigh2, shwLow1);*/
    q2 = AE_MULFS32P16S_LL(p3, p0);
    /* swMid2 = L_shr(swMid2, 1);*/
    q2 = AE_SRAIQ56(q2, 1);
    /* swMid  = L_add(swMid, swMid2);*/
    q1 = AE_ADDQ56(q1, q2);

    /* swMid    = L_shr(swMid, 14);*/
    q1 = AE_SRAIQ56(q1, 14);

    /* swVarOut = L_mac(swMid, shwHigh1, shwHigh2);*/
    AE_MULAFS32P16S_LL(q1, p2, p3);

    return AE_TRUNCA32Q48(q1);
}


VOS_INT32 CODEC_OpNormDiv_32(VOS_INT32 swNum, VOS_INT32 swDenom, VOS_INT16 shwQ)
{
    VOS_INT16       shwShift1;
    VOS_INT16       shwShift2;
    VOS_INT32       swTmp1;
    VOS_INT32       swTmp2;
    VOS_INT32       swTmp;
    VOS_INT32       swFrac;
    VOS_INT32       swExp;

    if (((swNum <= 0) || (swDenom <= 0)))
    {
        return (0);
    }

    shwShift1 = norm_l(swNum);
    swTmp1    = L_shl(swNum, shwShift1-1);

    shwShift2 = norm_l(swDenom);
    swTmp2    = L_shl(swDenom, shwShift2);

    /* 获取tensilica库函数输出 */
    swTmp     = scl_divide(swTmp1, swTmp2);

    /* 获取frac部分 */
    swFrac    = swTmp & 0X00FFFFFF;

    /* 获取exp部分 */
    swExp     = (swTmp & 0xFF000000) >> 24;

    swTmp     = L_shr(swFrac, (((23 - swExp)-shwQ) + (shwShift1-1)) - shwShift2);

    return swTmp;

}

#endif




#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

