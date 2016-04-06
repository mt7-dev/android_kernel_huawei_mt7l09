/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : Med_com_netsi_op.c
  版 本 号   : 初稿
  作    者   : 金剑
  生成日期   : 2011年9月1日
  最近修改   :
  功能描述   : 非ETSI基本运算函数扩展，标准C实现
  函数列表   :
              L_mac0
              L_mpy_ls
              L_mult0
              CODEC_OpExp10
              CODEC_OpfnExp2
              CODEC_OpfnLog10
              CODEC_OpfnLog2
              CODEC_OpLog10
              CODEC_OpL_divide
              CODEC_OpL_mpy_32
              CODEC_OpL_mpy_32_16_r
              CODEC_OpMult_shr
              CODEC_OpNormDiv_16
              CODEC_OpNormDiv_32
              CODEC_OpSqrt
              sqroot
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
#include "codec_op_lib.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/* 标准C代码中使用 */

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
VOS_INT32 log10_Qn_tab[] =
{
    0,
    20201781,
    40403562,
    60605343,
    80807124,
    101008905,
    121210686,
    141412467,
    161614248,
    181816029,
    202017810,
    222219591,
    242421372,
    262623153,
    282824934,
    303026715,
    323228496,
    343430277,
    363632058,
    383833839,
    404035620,
    424237401,
    444439182,
    464640963,
    484842744,
    505044525,
    525246307,
    545448088,
    565649869,
    585851650,
    606053431,
    626255212
};

/*****************************************************************************
  3 函数实现
*****************************************************************************/
#ifdef _MED_C89_

/*****************************************************************************
 函 数 名  : L_mult0
 功能描述  : return (long)shwVar1*shwVar2
 输入参数  : Word16 shwVar1  - 乘数1
             Word16 shwVar2  - 乘数2
 输出参数  : 无
 返 回 值  : Word32          - 乘积
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年9月2日
    作    者   : 苏庄銮 59026
    修改内容   : 新生成函数

*****************************************************************************/
Word32 L_mult0(Word16 shwVar1, Word16 shwVar2)
{
    Word32 swOut;

    swOut = (Word32)shwVar1 * (Word32)shwVar2;

    return (swOut);
}

/*****************************************************************************
 函 数 名  : L_mac0
 功能描述  : 16bit*16bit+32bit=32bit，16bit乘时不做移位，累加做饱和
 输入参数  : Word32 swVar3       --累加初值
             Word16 shwVar1      --乘数
             Word16 shwVar2      --乘数
 输出参数  : 无
 返 回 值  : Word32              --返回累加值
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年9月1日
    作    者   : 金剑
    修改内容   : 新生成函数

*****************************************************************************/
Word32 L_mac0(Word32 swVar3, Word16 shwVar1, Word16 shwVar2)
{
    Word32 swVarOut;

    swVarOut = (Word32)shwVar1 * (Word32)shwVar2;

    swVarOut = L_add(swVar3, swVarOut);

    return swVarOut;
}


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
  Word32 swVarOut;
  Word16 shwTemp;

  shwTemp = shr(extract_l(swVar1), 1);
  shwTemp = (short)32767 & (short)shwTemp;

  swVarOut = L_mult(shwVar2, shwTemp);
  swVarOut = L_shr(swVarOut, 15);
  swVarOut = L_mac(swVarOut, shwVar2, extract_h(swVar1));

  return (swVarOut);
}


VOS_INT32 CODEC_OpL_mpy_32(VOS_INT32 L_var1, VOS_INT32 L_var2)
{
    VOS_INT16       shwLow1;
    VOS_INT16       shwLow2;
    VOS_INT16       shwHigh1;
    VOS_INT16       shwHigh2;
    VOS_INT32       swVarOut;
    VOS_INT32       swLow;
    VOS_INT32       swMid1;
    VOS_INT32       swMid2;
    VOS_INT32       swMid;

    shwLow1 = shr(extract_l(L_var1), 1);
    shwLow1 = MAX_16 & shwLow1;

    shwLow2 = shr(extract_l(L_var2), 1);
    shwLow2 = MAX_16 & shwLow2;
    shwHigh1 = extract_h(L_var1);
    shwHigh2 = extract_h(L_var2);

    swLow = L_mult(shwLow1, shwLow2);
    swLow = L_shr(swLow, 16);

    swMid1 = L_mult(shwHigh1, shwLow2);
    swMid1 = L_shr(swMid1, 1);
    swMid  = L_add(swMid1, swLow);

    swMid2 = L_mult(shwHigh2, shwLow1);
    swMid2 = L_shr(swMid2, 1);
    swMid  = L_add(swMid, swMid2);

    swMid    = L_shr(swMid, 14);
    swVarOut = L_mac(swMid, shwHigh1, shwHigh2);

    return (swVarOut);
}


VOS_INT32 CODEC_OpNormDiv_32(VOS_INT32 swNum, VOS_INT32 swDenom, VOS_INT16 shwQ)
{
    VOS_INT16       shwShift1;
    VOS_INT16       shwShift2;
    VOS_INT32       swTmp1;
    VOS_INT32       swTmp2;
    VOS_INT32       swTmp;

    shwShift1 = norm_l(swNum);
    swTmp1    = L_shl(swNum, shwShift1-1);

    shwShift2 = norm_l(swDenom);
    swTmp2    = L_shl(swDenom, shwShift2);

    swTmp = CODEC_OpL_divide(swTmp1, swTmp2);

    swTmp = L_shr(swTmp, ((31-shwQ) + (shwShift1-1)) - shwShift2);

    return swTmp;
}

#endif  /* 标准C代码中使用 */

MED_INT16 CODEC_OpNormDiv_16 (Word16 shwNum, Word16 shwDenom, Word16 shwQn)
{
    MED_INT16 shwShift1, shwShift2;
    MED_INT16 shwVar1, shwVar2;
    MED_INT16 shwVarOut = 0;
    MED_INT16 shwCnt;
    MED_INT32 shwLnum;
    MED_INT32 shwLdenom;

    shwShift1 = norm_s(shwNum);
    shwVar1   = shl(shwNum, shwShift1-1);

    shwShift2 = norm_s(shwDenom);
    shwVar2   = shl(shwDenom, shwShift2);

    if (shwVar1 == shwVar2)
    {
        shwVarOut = MAX_16;
    }
    else
    {
        shwLnum   = L_deposit_l (shwVar1);
        shwLdenom = L_deposit_l (shwVar2);

        for (shwCnt = 0; shwCnt < 15; shwCnt++)
        {
            shwVarOut <<= 1;
            shwLnum   <<= 1;
            if (shwLnum >= shwLdenom)
            {
                shwLnum   = L_sub (shwLnum, shwLdenom);
                shwVarOut = add (shwVarOut, 1);
            }
        }
    }

    shwVarOut = shr(shwVarOut, ((15-shwQn) + (shwShift1-1)) - shwShift2);

    return (shwVarOut);
}


VOS_INT32 CODEC_OpL_mpy_32_16_r(VOS_INT32 swA, VOS_INT16 shwB)
{
  VOS_INT32         swVarOut;
  VOS_INT16         swtemp;

  swtemp = shr(extract_l(swA), 1);
  swtemp = ((VOS_INT16) 32767) & ((VOS_INT16) swtemp);

  swVarOut = L_mult(shwB, swtemp);
  swVarOut = L_add(swVarOut, 32768);
  swVarOut = L_shr(swVarOut, 15);
  swVarOut = L_mac(swVarOut, shwB, extract_h(swA));

  return (swVarOut);
}


VOS_INT16 CODEC_OpMult_shr(VOS_INT16 shwA, VOS_INT16 shwB, VOS_INT16 shwShift)
{
    VOS_INT16       shwVarOut;
    VOS_INT32       swProduct;

    swProduct = ((VOS_INT32)shwA) *((VOS_INT32)shwB);

    swProduct = swProduct>> shwShift;

    shwVarOut = saturate(swProduct);

    return (shwVarOut);
}


VOS_INT32 CODEC_OpL_divide(VOS_INT32 swNum, VOS_INT32 swDenom)
{
    VOS_INT16       shwApprox;
    VOS_INT32       swDiv;

    if (((swNum < 0) || (swDenom < 0)) || (swNum > swDenom))
    {
        return (0);
    }

    /* First approximation: 1 / L_denom = 1/extract_h(L_denom) */

    shwApprox = div_s((VOS_INT16) 0x3fff, extract_h(swDenom));

    /* 1/L_denom = approx * (2.0 - L_denom * approx) */

    swDiv = L_mpy_ls(swDenom, shwApprox);

    swDiv = L_sub((VOS_INT32) 0x7fffffffL, swDiv);

    swDiv = L_mpy_ls(swDiv, shwApprox);

    /* L_num * (1/L_denom) */

    swDiv = CODEC_OpL_mpy_32(swNum, swDiv);
    swDiv = L_shl(swDiv, 2);

    return (swDiv);
}
VOS_INT32 CODEC_OpfnLog2(VOS_INT32 swInput)
{

    VOS_INT16       shwC0 = -0x2b2a;
    VOS_INT16       shwC1 = 0x7fc5;
    VOS_INT16       shwC2 = -0x54d0;

    VOS_INT16       shwShiftCnt;
    VOS_INT16       shwInSqrd;
    VOS_INT16       shwIn;
    VOS_INT32       swLwIn;

    /* normalize input and store shifts required */
    shwShiftCnt = norm_l(swInput);
    swLwIn      = L_shl(swInput, shwShiftCnt);
    shwShiftCnt = add(shwShiftCnt, 1);
    shwShiftCnt = negate(shwShiftCnt);

    /* calculate x*x*c0 */
    shwIn     = extract_h(swLwIn);
    shwInSqrd = mult_r(shwIn, shwIn);
    swLwIn    = L_mult(shwInSqrd, shwC0);

    /* add x*c1 */
    swLwIn = L_mac(swLwIn, shwIn, shwC1);

    /* add c2 */
    swLwIn = L_add(swLwIn, L_deposit_h(shwC2));

    /* apply *(4/32) */
    swLwIn = L_shr(swLwIn, 3);
    swLwIn = swLwIn & 0x03ffffff;
    shwShiftCnt = shl(shwShiftCnt, 10);
    swLwIn      = L_add(swLwIn, L_deposit_h(shwShiftCnt));

    /* return log2 */
    return (swLwIn);
}


VOS_INT32 CODEC_OpfnLog10(VOS_INT32 swInput)
{

    VOS_INT16       shwScale = 9864;  /* 0.30103 = log10(2) */
    VOS_INT32       swLwIn;

    /* 0.30103*log2(x) */
    swLwIn = CODEC_OpfnLog2(swInput);
    swLwIn = L_mpy_ls(swLwIn, shwScale);

    return (swLwIn);
}


VOS_INT16 CODEC_OpLog10(VOS_INT16 shwQ, VOS_INT32 swA)
{
    VOS_INT32       swTmp;

    swTmp = CODEC_OpfnLog10(swA);
    swTmp = L_add(swTmp, CODEC_OP_LOG_OFFSET - log10_Qn_tab[shwQ]);

    return extract_h(swTmp);
}


VOS_INT16 CODEC_OpSqroot(VOS_INT32 swSqrtIn)
{
    VOS_INT32       swL_Temp0;
    VOS_INT32       swL_Temp1;
    VOS_INT16       shwTemp;
    VOS_INT16       shwTemp2;
    VOS_INT16       shwTemp3;
    VOS_INT16       shwTemp4;
    VOS_INT16       shwSqrtOut;

    /* determine 2nd term x/2 = (y-1)/2 */
    swL_Temp1 = L_shr(swSqrtIn, 1);                                             /* L_Temp1 = y/2 */
    swL_Temp1 = L_sub(swL_Temp1, CODEC_OP_SQROOT_PLUS_HALF);                      /* L_Temp1 = (y-1)/2 */
    shwTemp   = extract_h(swL_Temp1);                                           /* swTemp = x/2 */

    /* add contribution of 2nd term */
    swL_Temp1 = L_sub(swL_Temp1, CODEC_OP_SQROOT_MINUS_ONE);                      /* L_Temp1 = 1 + x/2 */

    /* determine 3rd term */
    swL_Temp0 = L_msu(0L, shwTemp, shwTemp);                                    /* L_Temp0 = -(x/2)^2 */
    shwTemp2 = extract_h(swL_Temp0);                                            /* swTemp2 = -(x/2)^2 */
    swL_Temp0 = L_shr(swL_Temp0, 1);                                            /* L_Temp0 = -0.5*(x/2)^2 */

    /* add contribution of 3rd term */
    swL_Temp0 = L_add(swL_Temp1, swL_Temp0);                                    /* L_Temp0 = 1 + x/2 - 0.5*(x/2)^2 */

    /* determine 4rd term */
    swL_Temp1 = L_msu(0L, shwTemp, shwTemp2);                                   /* L_Temp1 = (x/2)^3 */
    shwTemp3 = extract_h(swL_Temp1);                                            /* swTemp3 = (x/2)^3 */
    swL_Temp1 = L_shr(swL_Temp1, 1);                                            /* L_Temp1 = 0.5*(x/2)^3 */

    /* add contribution of 4rd term */
    /* L_Temp1 = 1 + x/2 - 0.5*(x/2)^2 + 0.5*(x/2)^3 */
    swL_Temp1 = L_add(swL_Temp0, swL_Temp1);

    /* determine partial 5th term */
    swL_Temp0 = L_mult(shwTemp, shwTemp3);                                      /* L_Temp0 = (x/2)^4 */
    shwTemp4 = round(swL_Temp0);                                                /* swTemp4 = (x/2)^4 */

    /* determine partial 6th term */
    swL_Temp0 = L_msu(0L, shwTemp2, shwTemp3);                                  /* L_Temp0 = (x/2)^5 */
    shwTemp2 = round(swL_Temp0);   /* swTemp2 = (x/2)^5 */

    /* L_Temp0 = -0.625*(x/2)^4 */
    swL_Temp0 = L_msu(0L, CODEC_OP_SQROOT_TERM5_MULTIPLER, shwTemp4);

    /* L_Temp1 = 1 + x/2 - 0.5*(x/2)^2 + 0.5*(x/2)^3 - 0.625*(x/2)^4 */
    swL_Temp1 = L_add(swL_Temp0, swL_Temp1);

    /* determine 6th term and add its contribution */
    /* swSqrtOut = 1 + x/2 - 0.5*(x/2)^2 + 0.5*(x/2)^3 */
    /* - 0.625*(x/2)^4 + 0.875*(x/2)^5     */
    shwSqrtOut = mac_r(swL_Temp1, CODEC_OP_SQROOT_TERM6_MULTIPLER, shwTemp2);

    /* return output */
    return (shwSqrtOut);
}


VOS_INT16 CODEC_OpSqrt(VOS_INT32 swA)
{
    VOS_INT16       shwNormPara;
    VOS_INT32       swNormedIn;
    VOS_INT16       shwResult;
    VOS_INT16       shwQShift;

    /* sqrt(2^(1-shwNormPara))系数表（已压缩） */
    VOS_INT16 ashwPara[2] = {32767, 23170};

    if(0 == swA)
    {
        return 0;
    }

    shwNormPara = norm_l(swA);
    shwQShift   = shr((shwNormPara + 1), 1) + 14;
    swNormedIn  = L_shl(swA, shwNormPara);

    shwResult = mult_r(CODEC_OpSqroot(swNormedIn), ashwPara[1 - (shwNormPara & 0x0001)]);   /* Q0, Qx -> Q(x-15) */
    shwResult = shr_r(shwResult, shwQShift - 15);                                   /* Q(x-15) -> Q0 */

    return shwResult;
}
VOS_INT32 CODEC_OpfnExp2(VOS_INT32 swInput)
{
    VOS_INT16          shwPCoefE[3] = {0x15ef, 0x556f, 0x7fbd};                 /* c2,   c1,    c0 */
    VOS_INT16          shwTemp1;
    VOS_INT16          shwTemp2;
    VOS_INT16          shwTemp3;
    VOS_INT16          shwTemp4;
    VOS_INT32          swLwIn;

    /* initialize */
    shwTemp3 = 0x0020;

    /* determine normlization shift count */
    shwTemp1 = extract_h(swInput);
    swLwIn   = L_mult(shwTemp1, shwTemp3);
    shwTemp2 = extract_h(swLwIn);

    /* determine un-normalized shift count */
    shwTemp3 = -0x0001;
    shwTemp4 = sub(shwTemp3, shwTemp2);

    /* normalize input */
    swLwIn = swLwIn & 0xffff;
    swLwIn = L_add(swLwIn, L_deposit_h(shwTemp3));

    swLwIn   = L_shr(swLwIn, 1);
    shwTemp1 = extract_l(swLwIn);

    /* calculate x*x*c2 */
    shwTemp2 = mult_r(shwTemp1, shwTemp1);
    swLwIn   = L_mult(shwTemp2, shwPCoefE[0]);

    /* calculate x*x*c2 + x*c1 */
    swLwIn   = L_mac(swLwIn, shwTemp1, shwPCoefE[1]);

    /* calculate x*x*c2 + x*c1 + c0 */
    swLwIn   = L_add(swLwIn, L_deposit_h(shwPCoefE[2]));

    /* un-normalize exponent if its requires it */
    if (shwTemp4 > 0)
    {
        swLwIn = L_shr(swLwIn, shwTemp4);
    }

    return (swLwIn);
}
VOS_INT32 CODEC_OpExp10(VOS_INT32 swA)
{
    VOS_INT16       shwInvScale = 27213;                                        /* (1/log10(2))/4 */
    VOS_INT32       swLwIn;

    swLwIn = L_mpy_ls(swA, shwInvScale);
    swLwIn = L_shl(swLwIn, 2);
    swLwIn = CODEC_OpfnExp2(swLwIn);

    return (swLwIn);
}
VOS_INT16 CODEC_OpLog2(VOS_INT32 swX)
{
    VOS_INT16  shwY = 30;
    if (swX <= 0)
    {
        if (0 == swX)
        {
            return -32768;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        shwY = shwY - norm_l(swX);
        return shwY;
    }
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif


