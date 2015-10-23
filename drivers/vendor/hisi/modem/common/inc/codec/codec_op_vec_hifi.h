
#ifndef __CODEC_COM_VEC_HIFI_H__
#define __CODEC_COM_VEC_HIFI_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#ifndef _MED_C89_
#include <xtensa/tie/xt_hifi2.h>
#endif
#include "codec_op_etsi.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/

/* DSP平台定义 */
#ifndef _MED_C89_

#define CODEC_OpCheck4ByteAligned(swAddr) (0 == (((Word32)(swAddr)) & 0x03))    /* 判断变量地址是否为4字节对齐 */
#define CODEC_OpCheckBit0Set(swLen)       (0 != ((swLen)  & 0x01))               /* 判断向量长度是否为奇数长 */
#define CODEC_OpCheckBit1Set(swLen)       (0 != ((swLen)  & 0x02))               /* 判断向量长度的第1bit是否为1 */
#define CODEC_OP_MULFP24S_MAX_LEN         (64)                                    /* 保证AE_MULAAFP24S不溢出的最大计算长度 */

#endif

/*****************************************************************************
  3 全局变量声明
*****************************************************************************/


/*****************************************************************************
  4 内联函数定义
*****************************************************************************/

#ifndef _MED_C89_

/*****************************************************************************
 函 数 名  : CODEC_OpVcAnd
 功能描述  : Y[i] = X[i]&Const
 输入参数  : const Word16 *pshwX    - 向量
             const Word32 swLen     - 向量长度
             const Word16 shwConst  - 常数
             Word16 *pshwY          - 保存计算结果的向量
 输出参数  : Word16 *pshwY          - 计算结果
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月20日
    作    者   : 金剑
    修改内容   : 新生成函数
  2.日    期   : 2011年9月1日
    作    者   : 邵聃
    修改内容   : 提升运算效率

*****************************************************************************/
XT_INLINE void CODEC_OpVcAnd(
                const Word16 *pshwX,
                const Word32 swLen,
                const Word16 shwConst,
                Word16 *pshwY)
{
    Word32 k;
    ae_p24x2s aepX1, aepX2, aepY1, aepY2, aepC;

    if (swLen < 0)
    {
        return;
    }

    /* shwConst-->aepC.HL */
    aepC = AE_CVTP24A16(shwConst);

    /* 向量地址为4字节对齐时采用双操作 */
    if (CODEC_OpCheck4ByteAligned((Word32)pshwX|(Word32)pshwY))
    {
        /* 2组并行计算,避免nop指令 */
        for (k = 0; k < swLen>>2; k++)
        {
            aepX1 = *((ae_p16x2s *)&pshwX[4*k]);
            aepX2 = *((ae_p16x2s *)&pshwX[4*k + 2]);

            aepY1 = AE_ANDP48(aepX1, aepC);
            aepY2 = AE_ANDP48(aepX2, aepC);

            *((ae_p16x2s *)&pshwY[4*k]) = aepY1;
            *((ae_p16x2s *)&pshwY[4*k+2]) = aepY2;
        }

        /* 1组并行计算 */
        if (CODEC_OpCheckBit1Set(swLen))
        {
            k = swLen & 0x7FFFFFFC;

            aepX1 = *((ae_p16x2s *)&pshwX[k]);
            aepY1 = AE_ANDP48(aepX1, aepC);
            *((ae_p16x2s *)&pshwY[k]) = aepY1;
        }
    }
    else
    {
        /* 向量地址不为4字节对齐时采用单操作 */
        for (k = 0; k < swLen>>1; k++)
        {
            aepX1 = *((ae_p16s *)&pshwX[2*k]);
            aepX2 = *((ae_p16s *)&pshwX[2*k + 1]);

            aepY1 = AE_ANDP48(aepX1, aepC);
            aepY2 = AE_ANDP48(aepX2, aepC);

            *((ae_p16s *)&pshwY[2*k]) = aepY1;
            *((ae_p16s *)&pshwY[2*k + 1]) = aepY2;
        }
    }

    /* 向量长度为奇数时，对最后一个元素单操作 */
    if (CODEC_OpCheckBit0Set(swLen))
    {
        k = swLen - 1;
        aepX1 = *((ae_p16s *)&pshwX[k]);
        aepY1 = AE_ANDP48(aepX1, aepC);
        *((ae_p16s *)&pshwY[k]) = aepY1;
    }
}

/*****************************************************************************
 函 数 名  : CODEC_OpVcMac
 功能描述  : Sum = L_mac(swSum, X[i], Const)
 输入参数  : const Word16 *pshwX    - 向量
             const Word32 swLen     - 向量长度(不得超过64,否则有溢出风险)
             const Word16 shwConst  - 常数
             Word32 swSum           - 累计和初始值
 输出参数  : 无
 返 回 值  : Word32                 - 累加和
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月20日
    作    者   : 金剑
    修改内容   : 新生成函数
  2.日    期   : 2011年9月1日
    作    者   : 邵聃
    修改内容   : 提升运算效率

*****************************************************************************/
XT_INLINE Word32 CODEC_OpVcMac(
                const Word16 *pshwX,
                const Word32  swLen,
                const Word16  shwConst,
                Word32        swSum)
{
    Word32 k;
    ae_p24x2s aepX1, aepX2, aepC;
    ae_q56s aeqS1;

    /* shwConst-->aepC.HL */
    aepC = AE_CVTP24A16(shwConst);

    /* swSum-->aeqS(17.48) */
    aeqS1 = AE_CVTQ48A32S(swSum);

    if (CODEC_OpCheck4ByteAligned(pshwX))
    {
        /* 2组并行计算,避免nop指令 */
        for (k = 0; k < swLen>>2; k++)
        {
            aepX1 = *((ae_p16x2s *)&pshwX[4*k]);
            aepX2 = *((ae_p16x2s *)&pshwX[4*k + 2]);

            AE_MULAAFP24S_HH_LL(aeqS1, aepX1, aepC);
            AE_MULAAFP24S_HH_LL(aeqS1, aepX2, aepC);
        }

        /* 1组并行计算 */
        if (CODEC_OpCheckBit1Set(swLen))
        {
            k = (swLen & 0x7FFFFFFC);

            aepX1 = *((ae_p16x2s *)&pshwX[k]);
            AE_MULAAFP24S_HH_LL(aeqS1, aepX1, aepC);
        }
        /* 饱和到48bit */
        aeqS1 = AE_SATQ48S(aeqS1);

    }
    else
    {
        /* 向量地址不为4字节对齐时采用单操作 */
        for (k = 0; k < swLen>>1; k++)
        {
            aepX1 = *((ae_p16s *)&pshwX[2*k]);
            aepX2 = *((ae_p16s *)&pshwX[2*k + 1]);

            AE_MULAFS32P16S_LL(aeqS1, aepX1, aepC);
            AE_MULAFS32P16S_LL(aeqS1, aepX2, aepC);
        }
    }

    /* 向量长度为奇数时，对最后一个元素单操作 */
    if (CODEC_OpCheckBit0Set(swLen))
    {
        aepX1 = *((ae_p16s *)&pshwX[swLen - 1]);
        AE_MULAFS32P16S_LL(aeqS1, aepX1, aepC);
    }

    return AE_TRUNCA32Q48(aeqS1);

}

/*****************************************************************************
函 数 名  : CODEC_OpVcMult
功能描述  : Y[i] = mult(X[i], Const);
输入参数  : const Word16 *pshwX      - 被乘数向量
            const Word32  swLen      - 向量长度
            const Word16  shwConst   - 乘数常量
            Word16 *pshwY            - 保存结果的向量
输出参数  : Word16 *pshwY            - 计算结果
返 回 值  : void
调用函数  :
被调函数  :

修改历史      :
  1.日    期   : 2011年8月20日
    作    者   : 金剑
    修改内容   : 新生成函数
  2.日    期   : 2011年9月1日
    作    者   : 邵聃
    修改内容   : 提升运算效率

*****************************************************************************/
XT_INLINE void CODEC_OpVcMult(
                   const Word16 *pshwX,
                   const Word32  swLen,
                   const Word16  shwConst,
                   Word16       *pshwY)
{
    Word32 k = 0;
    ae_p24x2s aepX1, aepY1, aepX2, aepY2, aepC;
    ae_q56s aeqL1, aeqH1, aeqL2, aeqH2;

    /* shwConst-->aepC.HL */
    aepC = AE_CVTP24A16(shwConst);

    if (CODEC_OpCheck4ByteAligned((Word32)pshwX|(Word32)pshwY))
    {
        /* 2组并行计算 */
        for (k = 0; k < swLen>>2; k++)
        {
            aepX1 = *((ae_p16x2s *)&pshwX[4*k]);
            aepX2 = *((ae_p16x2s *)&pshwX[4*k + 2]);

            aeqH1 =  AE_MULFS32P16S_HH(aepX1, aepC);
            aeqL1 = AE_MULFS32P16S_LL(aepX1, aepC);
            aepY1 = AE_TRUNCP24Q48X2(aeqH1, aeqL1);

            aeqH2 =  AE_MULFS32P16S_HH(aepX2, aepC);
            aeqL2 = AE_MULFS32P16S_LL(aepX2, aepC);
            aepY2 = AE_TRUNCP24Q48X2(aeqH2, aeqL2);

            *((ae_p16x2s *)&pshwY[4*k])     = aepY1;
            *((ae_p16x2s *)&pshwY[4*k + 2]) = aepY2;
        }
        /* 1组并行计算 */
        if (CODEC_OpCheckBit1Set(swLen))
        {
            k = (swLen & 0x7FFFFFFC);

            aepX1 = *((ae_p16x2s *)&pshwX[k]);

            aeqH1 =  AE_MULFS32P16S_HH(aepX1, aepC);
            aeqL1 = AE_MULFS32P16S_LL(aepX1, aepC);
            aepY1 = AE_TRUNCP24Q48X2(aeqH1, aeqL1);

            *((ae_p16x2s *)&pshwY[k]) = aepY1;
        }
    }
    else
    {
        /* 向量地址不为4字节对齐时采用单操作 */
        for (k = 0; k < swLen>>1; k++)
        {
            aepX1 = *((ae_p16s *)&pshwX[2*k]);
            aepX2 = *((ae_p16s *)&pshwX[2*k + 1]);

            aeqL1 = AE_MULFS32P16S_LL(aepX1, aepC);
            aeqL2 = AE_MULFS32P16S_LL(aepX2, aepC);

            aepY1 = AE_TRUNCP24Q48(aeqL1);
            aepY2 = AE_TRUNCP24Q48(aeqL2);

            *((ae_p16s *)&pshwY[2*k]) = aepY1;
            *((ae_p16s *)&pshwY[2*k + 1]) = aepY2;
        }
    }

    /* 向量长度为奇数时，对最后一个元素单操作 */
    if (CODEC_OpCheckBit0Set(swLen))
    {
        aepX1 = *((ae_p16s *)&pshwX[swLen - 1]);

        aeqL1 = AE_MULFS32P16S_LL(aepX1, aepC);
        aepY1 = AE_TRUNCP24Q48(aeqL1);

        *((ae_p16s *)&pshwY[swLen - 1]) = aepY1;
    }
}

/*****************************************************************************
函 数 名  : CODEC_OpVcMultAlignedQuan
功能描述  : Y[i] = mult(X[i], Const);
输入参数  : const Word16 *pshwX      - 被乘数向量(4byte对齐)
            const Word32  swLen      - 向量长度(4的倍数)
            const Word16  shwConst   - 乘数常量
            Word16 *pshwY            - 保存结果的向量(4byte对齐)
输出参数  : Word16 *pshwY            - 计算结果
返 回 值  : void
调用函数  :
被调函数  :

修改历史      :
  1.日    期   : 2011年8月20日
    作    者   : 金剑
    修改内容   : 新生成函数
  2.日    期   : 2011年9月1日
    作    者   : 邵聃
    修改内容   : 提升运算效率

*****************************************************************************/
XT_INLINE void CODEC_OpVcMultAlignedQuan(
                   const Word16 *pshwX,
                   const Word32  swLen,
                   const Word16  shwConst,
                   Word16       *pshwY)
{
    Word32 k = 0;
    ae_p24x2s aepX1, aepY1, aepX2, aepY2, aepC;
    ae_q56s aeqL1, aeqH1, aeqL2, aeqH2;

    /* shwConst-->aepC.HL */
    aepC = AE_CVTP24A16(shwConst);

    /* 2组并行计算 */
    for (k = 0; k < swLen>>2; k++)
    {
        aepX1 = *((ae_p16x2s *)&pshwX[4*k]);
        aepX2 = *((ae_p16x2s *)&pshwX[4*k + 2]);

        aeqH1 =  AE_MULFS32P16S_HH(aepX1, aepC);
        aeqL1 = AE_MULFS32P16S_LL(aepX1, aepC);
        aepY1 = AE_TRUNCP24Q48X2(aeqH1, aeqL1);

        aeqH2 =  AE_MULFS32P16S_HH(aepX2, aepC);
        aeqL2 = AE_MULFS32P16S_LL(aepX2, aepC);
        aepY2 = AE_TRUNCP24Q48X2(aeqH2, aeqL2);

        *((ae_p16x2s *)&pshwY[4*k])     = aepY1;
        *((ae_p16x2s *)&pshwY[4*k + 2]) = aepY2;
    }
}

/*****************************************************************************
 函 数 名  : CODEC_OpVcMultQ15Add
 功能描述  : Z[i] = add(X[i], mult(Y[i], Const));
 输入参数  : const Word16 *pshwX      - 与积相加的向量
             const Word16 *pshwY      - 与常数相乘的向量
             const Word32  swLen      - 向量长度
             const Word16  shwConst   - 常数
             Word16 *pshwZ            - 保存结果的向量
 输出参数  : Word16 *pshwZ            - 计算结果
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月20日
    作    者   : 金剑
    修改内容   : 新生成函数
  2.日    期   : 2011年9月1日
    作    者   : 邵聃
    修改内容   : 提升运算效率

*****************************************************************************/
XT_INLINE void CODEC_OpVcMultQ15Add(
                const Word16 *pshwX,
                const Word16 *pshwY,
                const Word32  swLen,
                const Word16  shwConst,
                Word16       *pshwZ)
{
    Word32 k = 0;
    ae_p24x2s aepX1, aepY1, aepZ1, aepX2, aepY2, aepZ2, aepC;
    ae_q56s aeqL1, aeqH1, aeqL2, aeqH2;

    if(swLen < 0)
    {
        return;
    }

    /* shwConst-->aepC.HL */
    aepC = AE_CVTP24A16(shwConst);

    if (CODEC_OpCheck4ByteAligned((Word32)pshwX|(Word32)pshwY|(Word32)pshwZ))
    {
        /* 2组并行计算 */
        for (k = 0; k < swLen>>2; k++)
        {
            aepY1 = *((ae_p16x2s *)&pshwY[4*k]);
            aepY2 = *((ae_p16x2s *)&pshwY[4*k + 2]);

            aepX1 = *((ae_p16x2s *)&pshwX[4*k]);
            aepX2 = *((ae_p16x2s *)&pshwX[4*k + 2]);

            aeqH1 = AE_MULFS32P16S_HH(aepY1, aepC);
            aeqL1 = AE_MULFS32P16S_LL(aepY1, aepC);

            aeqH2 = AE_MULFS32P16S_HH(aepY2, aepC);
            aeqL2 = AE_MULFS32P16S_LL(aepY2, aepC);

            aepY1 = AE_TRUNCP24Q48X2(aeqH1, aeqL1);
            aepY1 = AE_TRUNCP16(aepY1);
            aepY2 = AE_TRUNCP24Q48X2(aeqH2, aeqL2);
            aepY2 = AE_TRUNCP16(aepY2);

            aepZ1 = AE_ADDSP24S(aepX1, aepY1);
            aepZ2 = AE_ADDSP24S(aepX2, aepY2);

            *((ae_p16x2s *)&pshwZ[4*k]) = aepZ1;
            *((ae_p16x2s *)&pshwZ[4*k + 2]) = aepZ2;
        }
        /* 1组并行计算 */
        if (CODEC_OpCheckBit1Set(swLen))
        {
            k = swLen & 0x7FFFFFFC;

            aepY1 = *((ae_p16x2s *)&pshwY[k]);
            aepX1 = *((ae_p16x2s *)&pshwX[k]);

            aeqH1 = AE_MULFS32P16S_HH(aepY1, aepC);
            aeqL1 = AE_MULFS32P16S_LL(aepY1, aepC);

            aepY1 = AE_TRUNCP24Q48X2(aeqH1, aeqL1);
            aepY1 = AE_TRUNCP16(aepY1);

            aepZ1 = AE_ADDSP24S(aepX1, aepY1);

            *((ae_p16x2s *)&pshwZ[k]) = aepZ1;
        }
    }
    else
    {
        /* 向量地址不为4字节对齐时采用单操作 */
        for (k = 0; k < swLen>>1; k++)
        {
            aepY1 = *((ae_p16s *)&pshwY[2*k]);
            aepY2 = *((ae_p16s *)&pshwY[2*k + 1]);

            aepX1 = *((ae_p16s *)&pshwX[2*k]);
            aepX2 = *((ae_p16s *)&pshwX[2*k + 1]);

            aeqL1 = AE_MULFS32P16S_LL(aepY1, aepC);
            aeqL2 = AE_MULFS32P16S_LL(aepY2, aepC);

            aepY1 = AE_TRUNCP24Q48(aeqL1);
            aepY1 = AE_TRUNCP16(aepY1);

            aepY2 = AE_TRUNCP24Q48(aeqL2);
            aepY2 = AE_TRUNCP16(aepY2);

            aepZ1 = AE_ADDSP24S(aepX1, aepY1);
            aepZ2 = AE_ADDSP24S(aepX2, aepY2);

            *((ae_p16s *)&pshwZ[2*k]) = aepZ1;
            *((ae_p16s *)&pshwZ[2*k + 1]) = aepZ2;

        }
    }

    /* 向量长度为奇数时，对最后一个元素单操作 */
    if (CODEC_OpCheckBit0Set(swLen))
    {
        k = swLen - 1;

        aepY1 = *((ae_p16s *)&pshwY[k]);
        aepX1 = *((ae_p16s *)&pshwX[k]);

        aeqL1 = AE_MULFS32P16S_LL(aepY1, aepC);

        aepY1 = AE_TRUNCP24Q48(aeqL1);
        aepY1 = AE_TRUNCP16(aepY1);

        aepZ1 = AE_ADDSP24S(aepX1, aepY1);

        *((ae_p16s *)&pshwZ[k]) = aepZ1;
    }
}

/*****************************************************************************
 函 数 名  : CODEC_OpVcMultR
 功能描述  : Z[i] = mult_r(X[i], Const)
 输入参数  : const Word16 *pshwX      - 被乘数向量
             const Word32  swLen      - 向量长度
             const Word16  shwConst   - 乘数常量
             Word16 *pshwY      - 保存结果的向量
 输出参数  : Word16 *pshwY      - 计算结果
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月20日
    作    者   : 金剑
    修改内容   : 新生成函数
  2.日    期   : 2011年9月1日
    作    者   : 邵聃
    修改内容   : 提升运算效率

*****************************************************************************/
XT_INLINE void CODEC_OpVcMultR(
                const Word16 *pshwX,
                const Word32  swLen,
                const Word16  shwConst,
                Word16       *pshwY)
{
    Word32 k = 0;
    ae_p24x2s aepX1, aepY1, aepX2, aepY2, aepC;
    ae_q56s aeqL1, aeqH1, aeqL2, aeqH2;

    /* shwConst-->aepC.HL */
    aepC = AE_CVTP24A16(shwConst);

    if (CODEC_OpCheck4ByteAligned((Word32)pshwX|(Word32)pshwY))
    {
        /* 2组并行计算 */
        for (k = 0; k < swLen>>2; k++)
        {
            aepX1 = *((ae_p16x2s *)&pshwX[4*k]);
            aepX2 = *((ae_p16x2s *)&pshwX[4*k + 2]);

            aeqH1  = AE_MULFS32P16S_HH(aepX1, aepC);
            aeqL1 = AE_MULFS32P16S_LL(aepX1, aepC);
            aepY1 = AE_TRUNCP24Q48X2(aeqH1, aeqL1);
            aepY1 = AE_ROUNDSP16ASYM(aepY1);

            *((ae_p16x2s *)&pshwY[4*k])     = aepY1;

            aeqH2  = AE_MULFS32P16S_HH(aepX2, aepC);
            aeqL2 = AE_MULFS32P16S_LL(aepX2, aepC);
            aepY2 = AE_TRUNCP24Q48X2(aeqH2, aeqL2);
            aepY2 = AE_ROUNDSP16ASYM(aepY2);

            *((ae_p16x2s *)&pshwY[4*k + 2]) = aepY2;
        }

        /* 1组并行计算 */
        if (CODEC_OpCheckBit1Set(swLen))
        {
            k = swLen & 0x7FFFFFFC;

            aepX1 = *((ae_p16x2s *)&pshwX[k]);

            aeqH1  = AE_MULFS32P16S_HH(aepX1, aepC);
            aeqL1 = AE_MULFS32P16S_LL(aepX1, aepC);
            aepY1 = AE_TRUNCP24Q48X2(aeqH1, aeqL1);
            aepY1 = AE_ROUNDSP16ASYM(aepY1);

            *((ae_p16x2s *)&pshwY[k]) = aepY1;
        }
    }
    else
    {
        /* 向量地址不为4字节对齐时采用单操作 */
        for (k = 0; k < swLen>>1; k++)
        {
            aepX1 = *((ae_p16s *)&pshwX[2*k]);
            aepX2 = *((ae_p16s *)&pshwX[2*k + 1]);

            aeqL1 = AE_MULFS32P16S_LL(aepX1, aepC);
            aeqL2 = AE_MULFS32P16S_LL(aepX2, aepC);

            aepY1 = AE_ROUNDSP16Q48ASYM(aeqL1);
            aepY2 = AE_ROUNDSP16Q48ASYM(aeqL2);

            *((ae_p16s *)&pshwY[2*k]) = aepY1;
            *((ae_p16s *)&pshwY[2*k + 1]) = aepY2;
        }
    }

    /* 向量长度为奇数时，对最后一个元素单操作 */
    if (CODEC_OpCheckBit0Set(swLen))
    {
        aepX1 = *((ae_p16s *)&pshwX[swLen - 1]);

        aeqL1 = AE_MULFS32P16S_LL(aepX1, aepC);
        aepY1 = AE_ROUNDSP16Q48ASYM(aeqL1);

        *((ae_p16s *)&pshwY[swLen - 1]) = aepY1;
    }
}

/*****************************************************************************
 函 数 名  : CODEC_OpVcMultScale
 功能描述  : Y[i] = saturate(L_shr(L_mult(X[i], Const), ScaleBit))
 输入参数  : const Word16 *pshwX         - 输入向量
             const Word32  swLen         - 向量长度
             const Word16  shwConst      - 常数
             Word16  shwScaleBit         - 右移bit数
             Word16 *pshwY               - 保存结果的向量
 输出参数  : Word16 *pshwY               - 计算结果
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月20日
    作    者   : 金剑
    修改内容   : 新生成函数
  2.日    期   : 2011年9月1日
    作    者   : 邵聃
    修改内容   : 提升运算效率

*****************************************************************************/
XT_INLINE void CODEC_OpVcMultScale(
                const Word16 *pshwX,
                const Word32  swLen,
                const Word16  shwConst,
                Word16        shwScaleBit,
                Word16       *pshwY)
{
    Word32 k = 0;
    ae_p24x2s aepX1, aepY1, aepX2, aepY2, aepC;
    ae_q56s aeqL1, aeqH1, aeqL2, aeqH2;

    /* shwConst-->aepC.HL */
    aepC = AE_CVTP24A16(shwConst);

    /* 将需要左移比特数写入移位寄存器 */
    WUR_AE_SAR(shwScaleBit);

    if (CODEC_OpCheck4ByteAligned((Word32)pshwX|(Word32)pshwY))
    {
        /* 向量地址为4字节对齐时采用双操作 */
        for (k = 0; k < swLen>>1; k++)
        {
            aepX1 = *((ae_p16x2s *)&pshwX[2*k]);

            aeqH1 = AE_MULFS32P16S_HH(aepX1, aepC);
            aeqL1 = AE_MULFS32P16S_LL(aepX1, aepC);

            aeqH1 = AE_SRASQ56(aeqH1);
            aeqL1 = AE_SRASQ56(aeqL1);

            aeqH1 = AE_SLLISQ56S(aeqH1, 16);
            aeqL1 = AE_SLLISQ56S(aeqL1, 16);

            aeqH1 = AE_SATQ48S(aeqH1);
            aeqL1 = AE_SATQ48S(aeqL1);

            aepY1 = AE_TRUNCP24Q48X2(aeqH1, aeqL1);

            *((ae_p16x2s *)&pshwY[2*k]) = aepY1;
        }
    }
    else
    {
        /* 向量地址不为4字节对齐时采用单操作 */
        for (k = 0; k < swLen>>1; k++)
        {
            aepX1 = *((ae_p16s *)&pshwX[2*k]);
            aepX2 = *((ae_p16s *)&pshwX[2*k + 1]);

            aeqL1 = AE_MULFS32P16S_LL(aepX1, aepC);
            aeqL2 = AE_MULFS32P16S_LL(aepX2, aepC);

            aeqL1 = AE_SRASQ56(aeqL1);
            aeqL2 = AE_SRASQ56(aeqL2);

            aeqL1 = AE_SLLISQ56S(aeqL1, 16);
            aeqL2 = AE_SLLISQ56S(aeqL2, 16);

            aeqL1 = AE_SATQ48S(aeqL1);
            aeqL2 = AE_SATQ48S(aeqL2);

            aepY1 = AE_TRUNCP24Q48(aeqL1);
            aepY2 = AE_TRUNCP24Q48(aeqL2);

            *((ae_p16s *)&pshwY[2*k]) = aepY1;
            *((ae_p16s *)&pshwY[2*k + 1]) = aepY2;
        }
    }

    /* 向量长度为奇数时，对最后一个元素单操作 */
    if (CODEC_OpCheckBit0Set(swLen))
    {
        aepX1 = *((ae_p16s *)&pshwX[swLen - 1]);

        aeqL1 = AE_MULFS32P16S_LL(aepX1, aepC);
        aeqL1 = AE_SRASQ56(aeqL1);
        aeqL1 = AE_SLLISQ56S(aeqL1, 16);
        aeqL1 = AE_SATQ48S(aeqL1);
        aepY1 = AE_TRUNCP24Q48(aeqL1);

        *((ae_p16s *)&pshwY[swLen - 1]) = aepY1;
    }
}
XT_INLINE void CODEC_OpVcSub(
        const Word16        *pshwX,
        const Word32         swLen,
        const Word16         shwY,
        Word16              *pshwZ)
{
    Word32 k = 0;
    ae_p24x2s aepX1, aepY1, aepZ1, aepX2, aepZ2;

    /* 初始化aepY1和aepY2 */
    aepY1 = *((ae_p16s *)&shwY);

    if (CODEC_OpCheck4ByteAligned((Word32)pshwX|(Word32)pshwZ))
    {
        /* 向量地址为4字节对齐时采用双操作 */
        for (k = 0; k < swLen>>2; k++)
        {
            aepX1 = *((ae_p16x2s *)&pshwX[4*k]);
            aepX2 = *((ae_p16x2s *)&pshwX[4*k + 2]);

            aepZ1 = AE_SUBSP24S(aepX1, aepY1);
            aepZ2 = AE_SUBSP24S(aepX2, aepY1);

            *((ae_p16x2s *)&pshwZ[4*k]) = aepZ1;
            *((ae_p16x2s *)&pshwZ[4*k + 2]) = aepZ2;
        }

        /* 1组并行计算 */
        if (CODEC_OpCheckBit1Set(swLen))
        {
            k = swLen & 0x7FFFFFFC;

            aepX1 = *((ae_p16x2s *)&pshwX[k]);

            aepZ1 = AE_SUBSP24S(aepX1, aepY1);

            *((ae_p16x2s *)&pshwZ[k]) = aepZ1;
        }
    }
    else
    {
        /* 向量地址不为4字节对齐时采用单操作 */
        for (k = 0; k < swLen>>1; k++)
        {
            aepX1 = *((ae_p16s *)&pshwX[2*k]);
            aepX2 = *((ae_p16s *)&pshwX[2*k + 1]);

            aepZ1 = AE_SUBSP24S(aepX1, aepY1);
            aepZ2 = AE_SUBSP24S(aepX2, aepY1);

            *((ae_p16s *)&pshwZ[2*k]) = aepZ1;
            *((ae_p16s *)&pshwZ[2*k + 1]) = aepZ2;
        }
    }

    /* 向量长度为奇数时，对最后一个元素单操作 */
    if (CODEC_OpCheckBit0Set(swLen))
    {
        aepX1 = *((ae_p16s *)&pshwX[swLen - 1]);

        aepZ1 = AE_SUBSP24S(aepX1, aepY1);

        *((ae_p16s *)&pshwZ[swLen - 1]) = aepZ1;
    }
}
XT_INLINE void CODEC_OpVecAbs(
                      Word16        *pshwX,
                      Word32         swLen,
                      Word16        *pshwZ)
{
    Word32 k = 0;
    ae_p24x2s aepX1, aepZ1, aepX2, aepZ2;


    if (CODEC_OpCheck4ByteAligned((Word32)pshwX|(Word32)pshwZ))
    {
        /* 向量地址为4字节对齐时采用双操作 */
        for (k = 0; k < swLen>>2; k++)
        {
            aepX1 = *((ae_p16x2s *)&pshwX[4*k]);
            aepX2 = *((ae_p16x2s *)&pshwX[4*k + 2]);

            aepZ1 = AE_ABSSP24S(aepX1);
            aepZ2 = AE_ABSSP24S(aepX2);

            *((ae_p16x2s *)&pshwZ[4*k]) = aepZ1;
            *((ae_p16x2s *)&pshwZ[4*k + 2]) = aepZ2;
        }

        /* 1组并行计算 */
        if (CODEC_OpCheckBit1Set(swLen))
        {
            k = swLen & 0x7FFFFFFC;

            aepX1 = *((ae_p16x2s *)&pshwX[k]);

            aepZ1 = AE_ABSSP24S(aepX1);

            *((ae_p16x2s *)&pshwZ[k]) = aepZ1;
        }
    }
    else
    {
        /* 向量地址不为4字节对齐时采用单操作 */
        for (k = 0; k < swLen>>1; k++)
        {
            aepX1 = *((ae_p16s *)&pshwX[2*k]);
            aepX2 = *((ae_p16s *)&pshwX[2*k + 1]);

            aepZ1 = AE_ABSSP24S(aepX1);
            aepZ2 = AE_ABSSP24S(aepX2);

            *((ae_p16s *)&pshwZ[2*k]) = aepZ1;
            *((ae_p16s *)&pshwZ[2*k + 1]) = aepZ2;
        }
    }

    /* 向量长度为奇数时，对最后一个元素单操作 */
    if (CODEC_OpCheckBit0Set(swLen))
    {
        aepX1 = *((ae_p16s *)&pshwX[swLen - 1]);

        aepZ1 = AE_ABSSP24S(aepX1);

        *((ae_p16s *)&pshwZ[swLen - 1]) = aepZ1;
    }
}

/*****************************************************************************
 函 数 名  : CODEC_OpVecCpy
 功能描述  : Des[i] =  Src[i]，向量复制，Src与Des向量内存不可重复
 输入参数  : Word16 *pshwDes          - 目标向量
             const Word16 *pshwSrc    - 源向量
             const Word32 swLen       - 向量长度
 输出参数  : Word16 *pshwDes          - 复制结果
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月20日
    作    者   : 金剑
    修改内容   : 新生成函数
  2.日    期   : 2011年9月1日
    作    者   : 邵聃
    修改内容   : 提升运算效率

*****************************************************************************/
XT_INLINE void CODEC_OpVecCpy(
                Word16       *pshwDes,
                const Word16 *pshwSrc,
                const Word32  swLen)
{
    Word32 k = 0;
    ae_p24x2s aepX1, aepX2;

    if(swLen < 0)
    {
        return;
    }

    if (CODEC_OpCheck4ByteAligned((Word32)pshwDes|(Word32)pshwSrc))
    {
        /* 向量地址为4字节对齐时采用双操作 */
        for (k = 0; k < swLen>>2; k++)
        {
            aepX1 = *((ae_p16x2s *)&pshwSrc[4*k]);
            aepX2 = *((ae_p16x2s *)&pshwSrc[4*k + 2]);

            *((ae_p16x2s *)&pshwDes[4*k]) = aepX1;
            *((ae_p16x2s *)&pshwDes[4*k + 2]) = aepX2;
        }

        /* 1组并行计算 */
        if (CODEC_OpCheckBit1Set(swLen))
        {
            k = swLen & 0x7FFFFFFC;

            aepX1 = *((ae_p16x2s *)&pshwSrc[k]);
            *((ae_p16x2s *)&pshwDes[k]) = aepX1;
        }
    }
    else
    {
        /* 向量地址不为4字节对齐时采用单操作 */
        for (k = 0; k < swLen>>1; k++)
        {
            aepX1 = *((ae_p16s *)&pshwSrc[2*k]);
            aepX2 = *((ae_p16s *)&pshwSrc[2*k + 1]);

            *((ae_p16s *)&pshwDes[2*k]) = aepX1;
            *((ae_p16s *)&pshwDes[2*k + 1]) = aepX2;
        }
    }

    /* 向量长度为奇数时，对最后一个元素单操作 */
    if (CODEC_OpCheckBit0Set(swLen))
    {
        aepX1 = *((ae_p16s *)&pshwSrc[swLen - 1]);

        *((ae_p16s *)&pshwDes[swLen - 1]) = aepX1;
    }
}

/*****************************************************************************
 函 数 名  : CODEC_OpVecAlign
 功能描述  : Des[i] =  Src[i]，向量对齐，Src与Des向量内存不可重复
 输入参数  : Word16 *pshwDes          - 目标向量(4字节对齐)
             const Word16 *pshwSrc    - 源向量(2字节对齐)
             const Word32 swLen       - 向量长度
 输出参数  : Word16 *pshwDes          - 复制结果
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月20日
    作    者   : 金剑
    修改内容   : 新生成函数
  2.日    期   : 2011年9月1日
    作    者   : 邵聃
    修改内容   : 提升运算效率

*****************************************************************************/
XT_INLINE void CODEC_OpVecAlign(
                Word16       *pshwDes,
                const Word16 *pshwSrc,
                const Word32  swLen)
{
    Word32 k = 0;
    ae_p24x2s aepX1, aepX2, aepY;

    aepX1 = *((ae_p16s *)&pshwSrc[2*k]);
    aepX2 = *((ae_p16s *)&pshwSrc[2*k + 1]);

    for (k = 0; k < swLen>>1; k++)
    {
        AE_MOVTP24X2(aepY, aepX2, 0x1);

        *((ae_p16x2s *)&pshwDes[2*k]) = aepY;

        aepX1 = *((ae_p16s *)&pshwSrc[2*k + 2]);
        aepX2 = *((ae_p16s *)&pshwSrc[2*k + 3]);
        AE_MOVTP24X2(aepY, aepX1, 0x2);
    }

    if (CODEC_OpCheckBit0Set(swLen))
    {
        aepX1 = *((ae_p16s *)&pshwSrc[swLen - 1]);

        *((ae_p16s *)&pshwDes[swLen - 1]) = aepX1;
    }

}

/*****************************************************************************
 函 数 名  : CODEC_OpVecMax
 功能描述  : 寻找向量元素中最后一个最大值
 输入参数  : Word16 *pshwX       - 输入向量
             Word32 swLen        - 向量长度
             Word16 *pswMaxPos   - 保存最大值位置的单元
 输出参数  : Word16 *pswMaxPos   - 最大值位置
 返 回 值  : 最大值
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月20日
    作    者   : 金剑
    修改内容   : 新生成函数

*****************************************************************************/
XT_INLINE Word16 CODEC_OpVecMax(
                const Word16 *pshwX,
                const Word32  swLen,
                Word16       *pswMaxPos)
{
    Word32 k = 0;
    ae_p24x2s aepX1, aepX2, aepMax, aepMaxId, aepId1, aepId2;
    xtbool2 xtbHL;

    /* -32768 -> Max.H / Max.L */
    aepMax = AE_CVTP24A16(-32768);

    if (CODEC_OpCheck4ByteAligned(pshwX))
    {
        /* 向量地址为4字节对齐时采用双操作 */
        for (k = 0; k < swLen>>2; k++)
        {
            aepX1  = *((ae_p16x2s *)&pshwX[4*k]);
            aepX2  = *((ae_p16x2s *)&pshwX[4*k + 2]);

            aepId1 = AE_CVTP24A16X2(4*k,      4*k + 1);
            aepId2 = AE_CVTP24A16X2(4*k + 2,  4*k + 3);

            xtbHL = AE_LEP24S(aepMax, aepX1);
            AE_MOVTP24X2(aepMax,    aepX1,   xtbHL);
            AE_MOVTP24X2(aepMaxId,  aepId1,  xtbHL);

            xtbHL = AE_LEP24S(aepMax, aepX2);
            AE_MOVTP24X2(aepMax,    aepX2,   xtbHL);
            AE_MOVTP24X2(aepMaxId,  aepId2,  xtbHL);
        }

        /* 1组并行计算 */
        if (CODEC_OpCheckBit1Set(swLen))
        {
            k = swLen & 0x7FFFFFFC;

            aepX1 = *((ae_p16x2s *)&pshwX[k]);
            aepId1 = AE_CVTP24A16X2(k, k + 1);

            xtbHL = AE_LEP24S(aepMax, aepX1);
            AE_MOVTP24X2(aepMax,    aepX1,  xtbHL);
            AE_MOVTP24X2(aepMaxId,  aepId1, xtbHL);
        }

        /* 从H和L中找出最大的值 */
        aepX1 = AE_SELP24_LH(aepMax, aepMax);
        aepId1 = AE_SELP24_LH(aepMaxId, aepMaxId);

        xtbHL = AE_LTP24S(aepMax, aepX1);
        AE_MOVTP24X2(aepMax, aepX1, xtbHL);
        AE_MOVTP24X2(aepMaxId, aepId1, xtbHL);

        aepId2 = AE_MAXP24S(aepMaxId, aepId1);
        xtbHL = AE_EQP24(aepMax, aepX1);
        AE_MOVTP24X2(aepMaxId, aepId2, xtbHL);
    }
    else
    {
        /* 向量地址不为4字节对齐时采用单操作 */
        for (k = 0; k < swLen>>1; k++)
        {
            aepX1 = *((ae_p16s *)&pshwX[2*k]);
            aepX2 = *((ae_p16s *)&pshwX[2*k + 1]);

            aepId1 = AE_CVTP24A16(2*k);
            aepId2 = AE_CVTP24A16(2*k + 1);

            xtbHL = AE_LEP24S(aepMax, aepX1);
            AE_MOVTP24X2(aepMax, aepX1, xtbHL);
            AE_MOVTP24X2(aepMaxId, aepId1, xtbHL);

            xtbHL = AE_LEP24S(aepMax, aepX2);
            AE_MOVTP24X2(aepMax, aepX2, xtbHL);
            AE_MOVTP24X2(aepMaxId, aepId2, xtbHL);

        }
    }

    /* 向量长度为奇数时，对最后一个元素单操作 */
    if (CODEC_OpCheckBit0Set(swLen))
    {
        aepX1   = *((ae_p16s *)&pshwX[swLen - 1]);
        aepId1  = AE_CVTP24A16(swLen - 1);

        xtbHL = AE_LEP24S(aepMax, aepX1);
        AE_MOVTP24X2(aepMax, aepX1, xtbHL);
        AE_MOVTP24X2(aepMaxId, aepId1, xtbHL);
    }

    if(pswMaxPos!=0)
    {
        *pswMaxPos = AE_TRUNCA16P24S_H(aepMaxId);
    }

    return AE_TRUNCA16P24S_H(aepMax);
}

/*****************************************************************************
 函 数 名  : CODEC_OpVecMaxAbs
 功能描述  : Y= max(abs(X[i]))
 输入参数  : Word16 *pshwX       - 输入向量
             Word32 swLen        - 向量长度
             Word16 *pswMaxPos   - 保存最大绝对值元素所在位置
 输出参数  : Word16  *pswMaxPos  - 最大绝对值元素所在位置
 返 回 值  : Word16 输入向量最大绝对值
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月20日
    作    者   : 金剑
    修改内容   : 新生成函数

*****************************************************************************/
XT_INLINE Word16 CODEC_OpVecMaxAbs(
                const Word16 *pshwX,
                const Word32  swLen,
                Word16       *pswMaxPos)
{
    Word32 k = 0;
    ae_p24x2s aepX1, aepX2, aepMax, aepMaxId, aepId1, aepId2;
    xtbool2 xtbHL;

    /* 0 -> Max.H / Max.L */
    aepMax = AE_CVTP24A16(0);

    if (CODEC_OpCheck4ByteAligned(pshwX))
    {
        /* 向量地址为4字节对齐时采用双操作 */
        for (k = 0; k < swLen>>2; k++)
        {
            aepX1 = *((ae_p16x2s *)&pshwX[4*k]);
            aepX2 = *((ae_p16x2s *)&pshwX[4*k + 2]);

            aepId1 = AE_CVTP24A16X2(4*k, 4*k + 1);
            aepId2 = AE_CVTP24A16X2(4*k + 2, 4*k + 3);

            aepX1 = AE_ABSSP24S(aepX1);
            aepX2 = AE_ABSSP24S(aepX2);

            xtbHL = AE_LEP24S(aepMax, aepX1);
            AE_MOVTP24X2(aepMax, aepX1, xtbHL);
            AE_MOVTP24X2(aepMaxId, aepId1, xtbHL);

            xtbHL = AE_LEP24S(aepMax, aepX2);
            AE_MOVTP24X2(aepMax, aepX2, xtbHL);
            AE_MOVTP24X2(aepMaxId, aepId2, xtbHL);
        }

        /* 1组并行计算 */
        if (CODEC_OpCheckBit1Set(swLen))
        {
            k = swLen & 0x7FFFFFFC;

            aepX1 = *((ae_p16x2s *)&pshwX[k]);
            aepId1 = AE_CVTP24A16X2(k, k + 1);

            aepX1 = AE_ABSSP24S(aepX1);

            xtbHL = AE_LEP24S(aepMax, aepX1);
            AE_MOVTP24X2(aepMax, aepX1, xtbHL);
            AE_MOVTP24X2(aepMaxId, aepId1, xtbHL);
        }

        /* 从H和L中找出最大的值 */
        aepX1 = AE_SELP24_LH(aepMax, aepMax);
        aepId1 = AE_SELP24_LH(aepMaxId, aepMaxId);

        xtbHL = AE_LTP24S(aepMax, aepX1);
        AE_MOVTP24X2(aepMax, aepX1, xtbHL);
        AE_MOVTP24X2(aepMaxId, aepId1, xtbHL);

        aepId2 = AE_MAXP24S(aepMaxId, aepId1);
        xtbHL = AE_EQP24(aepMax, aepX1);
        AE_MOVTP24X2(aepMaxId, aepId2, xtbHL);

    }
    else
    {
        /* 向量地址不为4字节对齐时采用单操作 */
        for (k = 0; k < swLen>>1; k++)
        {
            aepX1 = *((ae_p16s *)&pshwX[2*k]);
            aepX2 = *((ae_p16s *)&pshwX[2*k + 1]);

            aepId1 = AE_CVTP24A16(2*k);
            aepId2 = AE_CVTP24A16(2*k + 1);

            aepX1 = AE_ABSSP24S(aepX1);
            aepX2 = AE_ABSSP24S(aepX2);

            xtbHL = AE_LEP24S(aepMax, aepX1);
            AE_MOVTP24X2(aepMax, aepX1, xtbHL);
            AE_MOVTP24X2(aepMaxId, aepId1, xtbHL);

            xtbHL = AE_LEP24S(aepMax, aepX2);
            AE_MOVTP24X2(aepMax, aepX2, xtbHL);
            AE_MOVTP24X2(aepMaxId, aepId2, xtbHL);

        }
    }

    /* 向量长度为奇数时，对最后一个元素单操作 */
    if (CODEC_OpCheckBit0Set(swLen))
    {
        aepX1 = *((ae_p16s *)&pshwX[swLen - 1]);
        aepId1 = AE_CVTP24A16(swLen - 1);

        aepX1 = AE_ABSSP24S(aepX1);

        xtbHL = AE_LEP24S(aepMax, aepX1);
        AE_MOVTP24X2(aepMax, aepX1, xtbHL);
        AE_MOVTP24X2(aepMaxId, aepId1, xtbHL);
    }

    if (pswMaxPos!=0)
    {
        *pswMaxPos = AE_TRUNCA16P24S_H(aepMaxId);
    }

    return AE_TRUNCA16P24S_H(aepMax);
}

/*****************************************************************************
 函 数 名  : CODEC_OpVecMin
 功能描述  : 寻找向量元素最小值
 输入参数  : Word16 *pshwX        - 输入向量
             Word32 swLen         - 向量长度
             Word16 *pswMinPos    - 保存最小值位置的单元
 输出参数  : Word16 *pswMinPos    - 最小值位置
 返 回 值  : Word16 最小值
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月20日
    作    者   : 金剑
    修改内容   : 新生成函数

*****************************************************************************/
XT_INLINE Word16 CODEC_OpVecMin(
                const Word16 *pshwX,
                const Word32  swLen,
                Word16       *pswMinPos)
{
    Word32 k = 0;
    ae_p24x2s aepX1, aepX2, aepMin, aepMinId, aepId1, aepId2;
    xtbool2 xtbHL;

    /* 32767 -> Min.H / Min.L */
    aepMin = AE_CVTP24A16(32767);

    if (CODEC_OpCheck4ByteAligned(pshwX))
    {
        /* 向量地址为4字节对齐时采用双操作 */
        for (k = 0; k < swLen>>2; k++)
        {
            aepX1 = *((ae_p16x2s *)&pshwX[4*k]);
            aepX2 = *((ae_p16x2s *)&pshwX[4*k + 2]);

            aepId1 = AE_CVTP24A16X2(4*k, 4*k + 1);
            aepId2 = AE_CVTP24A16X2(4*k + 2, 4*k + 3);

            xtbHL = AE_LEP24S(aepX1, aepMin);
            AE_MOVTP24X2(aepMin, aepX1, xtbHL);
            AE_MOVTP24X2(aepMinId, aepId1, xtbHL);

            xtbHL = AE_LEP24S(aepX2, aepMin);
            AE_MOVTP24X2(aepMin, aepX2, xtbHL);
            AE_MOVTP24X2(aepMinId, aepId2, xtbHL);
        }

        /* 1组并行计算 */
        if (CODEC_OpCheckBit1Set(swLen))
        {
            k = swLen & 0x7FFFFFFC;

            aepX1 = *((ae_p16x2s *)&pshwX[k]);
            aepId1 = AE_CVTP24A16X2(k, k + 1);

            xtbHL = AE_LEP24S(aepX1, aepMin);
            AE_MOVTP24X2(aepMin, aepX1, xtbHL);
            AE_MOVTP24X2(aepMinId, aepId1, xtbHL);
        }

        /* 从H和L中找出最小的值 */
        aepX1 = AE_SELP24_LH(aepMin, aepMin);
        aepId1 = AE_SELP24_LH(aepMinId, aepMinId);

        xtbHL = AE_LTP24S(aepX1, aepMin);
        AE_MOVTP24X2(aepMin, aepX1, xtbHL);
        AE_MOVTP24X2(aepMinId, aepId1, xtbHL);

        aepId2 = AE_MAXP24S(aepMinId, aepId1);
        xtbHL = AE_EQP24(aepMin, aepX1);
        AE_MOVTP24X2(aepMinId, aepId2, xtbHL);
    }
    else
    {
        /* 向量地址不为4字节对齐时采用单操作 */
        for (k = 0; k < swLen>>1; k++)
        {
            aepX1 = *((ae_p16s *)&pshwX[2*k]);
            aepX2 = *((ae_p16s *)&pshwX[2*k + 1]);

            aepId1 = AE_CVTP24A16(2*k);
            aepId2 = AE_CVTP24A16(2*k + 1);

            xtbHL = AE_LEP24S(aepX1, aepMin);
            AE_MOVTP24X2(aepMin, aepX1, xtbHL);
            AE_MOVTP24X2(aepMinId, aepId1, xtbHL);

            xtbHL = AE_LEP24S(aepX2, aepMin);
            AE_MOVTP24X2(aepMin, aepX2, xtbHL);
            AE_MOVTP24X2(aepMinId, aepId2, xtbHL);

        }
    }

    /* 向量长度为奇数时，对最后一个元素单操作 */
    if (CODEC_OpCheckBit0Set(swLen))
    {
        aepX1 = *((ae_p16s *)&pshwX[swLen - 1]);
        aepId1 = AE_CVTP24A16(swLen - 1);

        xtbHL = AE_LEP24S(aepX1, aepMin);
        AE_MOVTP24X2(aepMin, aepX1, xtbHL);
        AE_MOVTP24X2(aepMinId, aepId1, xtbHL);
    }

    if (pswMinPos!=0)
    {
        *pswMinPos = AE_TRUNCA16P24S_H(aepMinId);
    }

    return AE_TRUNCA16P24S_H(aepMin);
}

/*****************************************************************************
 函 数 名  : CODEC_OpVecScaleMac
 功能描述  : temp = shr(X[i], bits) ， Bits>=0
             sum = L_mac(sum, temp, temp)
 输入参数  : const Word16 *pshwX    - 向量
             const Word32 swLen     - 向量长度
             Word32 swSum           - 累加和初始值
             Word16 shwBits         - 缩放右移位数
 输出参数  : 无
 返 回 值  : Word32  累加结果
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月20日
    作    者   : 金剑
    修改内容   : 新生成函数
  2.日    期   : 2011年9月1日
    作    者   : 邵聃
    修改内容   : 提升运算效率

*****************************************************************************/
XT_INLINE Word32 CODEC_OpVecScaleMac(
                const Word16 *pshwX,
                const Word32  swLen,
                Word32        swSum,
                Word16        shwBits)
{
    Word32 k = 0;
    ae_p24x2s aepX1, aepX2;
    ae_q56s aeqS;

    if(swLen < 0)
    {
        return swSum;
    }

    /* 将需要右移比特数写入移位寄存器 */
    WUR_AE_SAR(shwBits);

    /* swSum --> aeqS(17.48) */
    aeqS = AE_CVTQ48A32S(swSum);

    if (CODEC_OpCheck4ByteAligned(pshwX))
    {
        /* 向量地址为4字节对齐时采用双操作 */
        for (k = 0; k < swLen>>1; k++)
        {
            aepX1 = *((ae_p16x2s *)&pshwX[2*k]);

            aepX1 = AE_SRASP24(aepX1);
            aepX1 = AE_TRUNCP16(aepX1);
            AE_MULAFS32P16S_HH(aeqS, aepX1, aepX1);
            AE_MULAFS32P16S_LL(aeqS, aepX1, aepX1);
        }
    }
    else
    {
        /* 向量地址不为4字节对齐时采用单操作 */
        for (k = 0; k < swLen>>1; k++)
        {
            aepX1 = *((ae_p16s *)&pshwX[2*k]);
            aepX2 = *((ae_p16s *)&pshwX[2*k + 1]);

            aepX1 = AE_SRASP24(aepX1);
            aepX1 = AE_TRUNCP16(aepX1);
            aepX2 = AE_SRASP24(aepX2);
            aepX2 = AE_TRUNCP16(aepX2);

            AE_MULAFS32P16S_LL(aeqS, aepX1, aepX1);
            AE_MULAFS32P16S_LL(aeqS, aepX2, aepX2);
        }
    }

    /* 向量长度为奇数时，对最后一个元素单操作 */
    if (CODEC_OpCheckBit0Set(swLen))
    {
        aepX1 = *((ae_p16s *)&pshwX[swLen - 1]);

        aepX1 = AE_SRASP24(aepX1);
        aepX1 = AE_TRUNCP16(aepX1);

        AE_MULAFS32P16S_LL(aeqS, aepX1, aepX1);
    }

    return AE_TRUNCA32Q48(aeqS);
}

/*****************************************************************************
 函 数 名  : CODEC_OpVecSet
 功能描述  : 向量赋值, Des[i] = Value
 输入参数  : Word16 *pshwDes     - 待赋值向量
             Word32 swLen        - 向量长度
             Word16 pshwValue    - 赋值目标值
 输出参数  : Word16 *pshwDes     - 赋值结果
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月20日
    作    者   : 金剑
    修改内容   : 新生成函数
  2.日    期   : 2011年9月1日
    作    者   : 邵聃
    修改内容   : 提升运算效率

*****************************************************************************/
XT_INLINE void CODEC_OpVecSet(
                Word16       *pshwDes,
                const Word32  swLen,
                const Word16  shwValue)
{
    Word32 k = 0;
    ae_p24x2s aepV;

    if(swLen < 0)
    {
        return;
    }

    /* pshwValue-->aepV.HL */
    aepV = AE_CVTP24A16(shwValue);

    if (CODEC_OpCheck4ByteAligned(pshwDes))
    {
        /* 向量地址为4字节对齐时采用双操作 */
        for (k = 0; k < swLen>>2; k++)
        {
            *((ae_p16x2s *)&pshwDes[4*k]) = aepV;
            *((ae_p16x2s *)&pshwDes[4*k+2]) = aepV;
        }
        /* 1组并行计算 */
        if (CODEC_OpCheckBit1Set(swLen))
        {
            k = swLen & 0x7FFFFFFC;
            *((ae_p16x2s *)&pshwDes[k]) = aepV;
        }

    }
    else
    {
        /* 向量地址不为4字节对齐时采用单操作 */
        for (k = 0; k < swLen>>1; k++)
        {
            *((ae_p16s *)&pshwDes[2*k]) = aepV;
            *((ae_p16s *)&pshwDes[2*k + 1]) = aepV;
        }
    }

    /* 向量长度为奇数时，对最后一个元素单操作 */
    if (CODEC_OpCheckBit0Set(swLen))
    {
        *((ae_p16s *)&pshwDes[swLen - 1]) = aepV;
    }
}

/*****************************************************************************
 函 数 名  : CODEC_OpVecShl
 功能描述  : Y[i] = shl(X[i], Bits)
 输入参数  : const Word16 *pshwX     - 待移位向量
             const Word32 swLen      - 向量长度
             Word16 shwBits          - 移位数
             Word16 *pshwY           - 保存结果的向量
 输出参数  : Word16 *pshwY           - 移位结果
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月20日
    作    者   : 金剑
    修改内容   : 新生成函数
  2.日    期   : 2011年9月1日
    作    者   : 邵聃
    修改内容   : 提升运算效率

*****************************************************************************/
XT_INLINE void CODEC_OpVecShl(
                const Word16 *pshwX,
                const Word32  swLen,
                Word16        shwBits,
                Word16       *pshwY)
{
    Word32 k = 0;
    ae_p24x2s aepX1, aepY1, aepX2, aepY2;

    if(swLen < 0)
    {
        return;
    }

    /* 限制位移数 */
    shwBits = shwBits > 16 ? 16 : shwBits;

    /* 将需要左移比特数写入移位寄存器 */
    WUR_AE_SAR(shwBits);

    if (CODEC_OpCheck4ByteAligned((Word32)pshwX|(Word32)pshwY))
    {
        /* 向量地址为4字节对齐时采用双操作 */
        for (k = 0; k < swLen>>1; k++)
        {
            aepX1 = *((ae_p16x2s *)&pshwX[2*k]);

            aepY1 = AE_SLLSSP24S(aepX1);

            *((ae_p16x2s *)&pshwY[2*k]) = aepY1;
        }
    }
    else
    {
        /* 向量地址不为4字节对齐时采用单操作 */
        for (k = 0; k < swLen>>1; k++)
        {
            aepX1 = *((ae_p16s *)&pshwX[2*k]);
            aepX2 = *((ae_p16s *)&pshwX[2*k + 1]);

            aepY1 = AE_SLLSSP24S(aepX1);
            aepY2 = AE_SLLSSP24S(aepX2);

            *((ae_p16s *)&pshwY[2*k]) = aepY1;
            *((ae_p16s *)&pshwY[2*k + 1]) = aepY2;
        }
    }

    /* 向量长度为奇数时，对最后一个元素单操作 */
    if (CODEC_OpCheckBit0Set(swLen))
    {
        aepX1 = *((ae_p16s *)&pshwX[swLen - 1]);

        aepY1 = AE_SLLSSP24S(aepX1);

        *((ae_p16s *)&pshwY[swLen - 1]) = aepY1;
    }
}

/*****************************************************************************
 函 数 名  : CODEC_OpVecShr
 功能描述  : Y[i] = shr(X[i], Bits)
 输入参数  : const Word16 *pshwX      - 待移位向量
             const Word32 swLen       - 向量长度
             Word16 shwBits     - 移位数
             Word16 *pshwY      - 保存结果的向量
 输出参数  : Word16 *pshwY      - 移位结果
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月20日
    作    者   : 金剑
    修改内容   : 新生成函数
  2.日    期   : 2011年9月1日
    作    者   : 邵聃
    修改内容   : 提升运算效率

*****************************************************************************/
XT_INLINE void CODEC_OpVecShr(
                const Word16 *pshwX,
                const Word32  swLen,
                Word16        shwBits,
                Word16       *pshwY)
{
    Word32 k = 0;
    ae_p24x2s aepX1, aepY1, aepX2, aepY2;

    if(swLen < 0)
    {
        return;
    }

    /* 限制位移数 */
    shwBits = shwBits > 16 ? 16 : shwBits;

    /* 将需要右移比特数写入移位寄存器 */
    WUR_AE_SAR(shwBits);

    if (CODEC_OpCheck4ByteAligned((Word32)pshwX|(Word32)pshwY))
    {
        /* 向量地址为4字节对齐时采用双操作 */
        for (k = 0; k < swLen>>1; k++)
        {
            aepX1 = *((ae_p16x2s *)&pshwX[2*k]);
            aepY1 = AE_SRASP24(aepX1);
            *((ae_p16x2s *)&pshwY[2*k]) = aepY1;
        }
    }
    else
    {
        /* 向量地址不为4字节对齐时采用单操作 */
        for (k = 0; k < swLen>>1; k++)
        {
            aepX1 = *((ae_p16s *)&pshwX[2*k]);
            aepX2 = *((ae_p16s *)&pshwX[2*k + 1]);

            aepY1 = AE_SRASP24(aepX1);
            aepY2 = AE_SRASP24(aepX2);

            *((ae_p16s *)&pshwY[2*k]) = aepY1;
            *((ae_p16s *)&pshwY[2*k + 1]) = aepY2;
        }
    }

    /* 向量长度为奇数时，对最后一个元素单操作 */
    if (CODEC_OpCheckBit0Set(swLen))
    {
        aepX1 = *((ae_p16s *)&pshwX[swLen - 1]);
        aepY1 = AE_SRASP24(aepX1);
        *((ae_p16s *)&pshwY[swLen - 1]) = aepY1;
    }
}
XT_INLINE void CODEC_OpVecShr_r(
        Word16 *pshwX,
        Word32  swLen,
        Word16        shwBits,
        Word16       *pshwY)
{
    Word32 k = 0;
    ae_p24x2s aepX1, aepY1, aepX2, aepY2;

    if(swLen < 0)
    {
        return;
    }

    /* 将需要右移比特数写入移位寄存器 */
    WUR_AE_SAR(shwBits);

    if (CODEC_OpCheck4ByteAligned((Word32)pshwX|(Word32)pshwY))
    {
        /* 向量地址为4字节对齐时采用双操作 */
        for (k = 0; k < swLen>>1; k++)
        {
            aepX1 = *((ae_p16x2s *)&pshwX[2*k]);
            aepY1 = AE_SRASP24(aepX1);

            /* 对aepY1进行高16比特非对称取整(0.5->1;-0.5->0) --> aepY1*/
            aepY1 = AE_ROUNDSP16ASYM(aepY1);

            *((ae_p16x2s *)&pshwY[2*k]) = aepY1;
        }
    }
    else
    {
        /* 向量地址不为4字节对齐时采用单操作 */
        for (k = 0; k < swLen>>1; k++)
        {
            aepX1 = *((ae_p16s *)&pshwX[2*k]);
            aepX2 = *((ae_p16s *)&pshwX[2*k + 1]);

            aepY1 = AE_SRASP24(aepX1);
            aepY2 = AE_SRASP24(aepX2);

            /* 对aepY1\aepY2进行高16比特非对称取整(0.5->1;-0.5->0) --> aepY1\aepY2 */
            aepY1 = AE_ROUNDSP16ASYM(aepY1);
            aepY2 = AE_ROUNDSP16ASYM(aepY2);

            *((ae_p16s *)&pshwY[2*k]) = aepY1;
            *((ae_p16s *)&pshwY[2*k + 1]) = aepY2;
        }
    }

    /* 向量长度为奇数时，对最后一个元素单操作 */
    if (CODEC_OpCheckBit0Set(swLen))
    {
        aepX1 = *((ae_p16s *)&pshwX[swLen - 1]);
        aepY1 = AE_SRASP24(aepX1);

        /* 对aepY1进行高16比特非对称取整(0.5->1;-0.5->0) --> aepY1*/
        aepY1 = AE_ROUNDSP16ASYM(aepY1);

        *((ae_p16s *)&pshwY[swLen - 1]) = aepY1;
    }
}

/*****************************************************************************
 函 数 名  : CODEC_OpVecSum
 功能描述  : 向量所有元素累加和
 输入参数  : Word16 *pshwX   - 向量
             Word32 swLen    - 向量长度
 输出参数  : 无
 返 回 值  : Word16 累加和
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月20日
    作    者   : 金剑
    修改内容   : 新生成函数
  2.日    期   : 2011年9月1日
    作    者   : 邵聃
    修改内容   : 提升运算效率

*****************************************************************************/
XT_INLINE Word16 CODEC_OpVecSum(
                const Word16 *pshwX,
                const Word32  swLen)
{
    Word32 k = 0;
    ae_p24x2s aepX1, aepS1, aepX2;

    if(swLen < 0)
    {
        return 0;
    }

    aepS1 = AE_ZEROP48();

    for (k = 0; k < swLen>>1; k++)
    {
        aepX1 = *((ae_p16s *)&pshwX[2*k]);
        aepX2 = *((ae_p16s *)&pshwX[2*k + 1]);

        aepS1 = AE_ADDSP24S(aepX1, aepS1);
        aepS1 = AE_ADDSP24S(aepX2, aepS1);
    }

    /* 向量长度为奇数时，对最后一个元素单操作 */
    if (CODEC_OpCheckBit0Set(swLen))
    {
        aepX1 = *((ae_p16s *)&pshwX[swLen - 1]);
        aepS1 = AE_ADDSP24S(aepX1, aepS1);
    }

    /* aepS.L-->shwSum */
    return AE_TRUNCA16P24S_H(aepS1);
}

/*****************************************************************************
 函 数 名  : CODEC_OpVvAdd
 功能描述  : Z[i] = add(X[i], Y[i])
 输入参数  : Word16 *pshwX    - 被加数向量
             Word16 *pshwY    - 加数向量
             Word32 swLen     - 向量长度
             Word16 *pshwZ    - 保存结果的向量
 输出参数  : Word16 *pshwZ    - 计算结果
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月20日
    作    者   : 金剑
    修改内容   : 新生成函数
  2.日    期   : 2011年9月1日
    作    者   : 邵聃
    修改内容   : 提升运算效率

*****************************************************************************/
XT_INLINE void CODEC_OpVvAdd(
                const Word16 *pshwX,
                const Word16 *pshwY,
                const Word32  swLen,
                Word16       *pshwZ)
{
    Word32 k = 0;
    ae_p24x2s aepX1, aepY1, aepZ1, aepX2, aepY2, aepZ2;

    if(swLen < 0)
    {
        return;
    }

    if (CODEC_OpCheck4ByteAligned((Word32)pshwX|(Word32)pshwY|(Word32)pshwZ))
    {
        /* 向量地址为4字节对齐时采用双操作 */
        for (k = 0; k < swLen>>2; k++)
        {
            aepX1 = *((ae_p16x2s *)&pshwX[4*k]);
            aepY1 = *((ae_p16x2s *)&pshwY[4*k]);

            aepX2 = *((ae_p16x2s *)&pshwX[4*k + 2]);
            aepY2 = *((ae_p16x2s *)&pshwY[4*k + 2]);

            aepZ1 = AE_ADDSP24S(aepX1, aepY1);
            aepZ2 = AE_ADDSP24S(aepX2, aepY2);

            *((ae_p16x2s *)&pshwZ[4*k]) = aepZ1;
            *((ae_p16x2s *)&pshwZ[4*k + 2]) = aepZ2;
        }

        /* 1组并行计算 */
        if (CODEC_OpCheckBit1Set(swLen))
        {
            k = swLen & 0x7FFFFFFC;

            aepX1 = *((ae_p16x2s *)&pshwX[k]);
            aepY1 = *((ae_p16x2s *)&pshwY[k]);

            aepZ1 = AE_ADDSP24S(aepX1, aepY1);

            *((ae_p16x2s *)&pshwZ[k]) = aepZ1;
        }
    }
    else
    {
        /* 向量地址不为4字节对齐时采用单操作 */
        for (k = 0; k < swLen>>1; k++)
        {
            aepX1 = *((ae_p16s *)&pshwX[2*k]);
            aepX2 = *((ae_p16s *)&pshwX[2*k + 1]);

            aepY1 = *((ae_p16s *)&pshwY[2*k]);
            aepY2 = *((ae_p16s *)&pshwY[2*k + 1]);

            aepZ1 = AE_ADDSP24S(aepX1, aepY1);
            aepZ2 = AE_ADDSP24S(aepX2, aepY2);

            *((ae_p16s *)&pshwZ[2*k]) = aepZ1;
            *((ae_p16s *)&pshwZ[2*k + 1]) = aepZ2;
        }
    }

    /* 向量长度为奇数时，对最后一个元素单操作 */
    if (CODEC_OpCheckBit0Set(swLen))
    {
        aepX1 = *((ae_p16s *)&pshwX[swLen - 1]);
        aepY1 = *((ae_p16s *)&pshwY[swLen - 1]);

        aepZ1 = AE_ADDSP24S(aepX1, aepY1);

        *((ae_p16s *)&pshwZ[swLen - 1]) = aepZ1;
    }
}

/*****************************************************************************
 函 数 名  : CODEC_OpVvAdd32
 功能描述  : Z[i] = L_add(X[i], Y[i])
 输入参数  : Word32 *pshwX    - 被加数向量
             Word32 *pshwY    - 加数向量
             Word32 swLen     - 向量长度
             Word32 *pshwZ    - 保存结果的向量
 输出参数  : Word32 *pshwZ    - 计算结果
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月20日
    作    者   : 金剑
    修改内容   : 新生成函数
  2.日    期   : 2011年9月1日
    作    者   : 邵聃
    修改内容   : 提升运算效率

*****************************************************************************/
XT_INLINE void CODEC_OpVvAdd32(
                const Word32 *pswX,
                const Word32 *pswY,
                const Word32  swLen,
                Word32       *pswZ)
{
    Word32 k = 0;

    ae_q56s aeqX1, aeqY1, aeqZ1, aeqX2, aeqY2, aeqZ2;

    if(swLen < 0)
    {
        return;
    }

    for (k = 0; k < swLen>>1; k++)
    {
        aeqX1 = *((ae_q32s *)&pswX[2*k]);
        aeqX2 = *((ae_q32s *)&pswX[2*k + 1]);

        aeqY1 = *((ae_q32s *)&pswY[2*k]);
        aeqY2 = *((ae_q32s *)&pswY[2*k + 1]);

        aeqZ1 = AE_ADDSQ56S(aeqX1, aeqY1);
        aeqZ2 = AE_ADDSQ56S(aeqX2, aeqY2);

        aeqZ1 = AE_SATQ48S(aeqZ1);
        aeqZ2 = AE_SATQ48S(aeqZ2);

        *((ae_q32s *)&pswZ[2*k]) = aeqZ1;
        *((ae_q32s *)&pswZ[2*k + 1]) = aeqZ2;
    }

    /* 向量长度为奇数时，对最后一个元素单操作 */
    if (CODEC_OpCheckBit0Set(swLen))
    {
        aeqX1 = *((ae_q32s *)&pswX[swLen - 1]);
        aeqY1 = *((ae_q32s *)&pswY[swLen - 1]);

        aeqZ1 = AE_ADDSQ56S(aeqX1, aeqY1);
        aeqZ1 = AE_SATQ48S(aeqZ1);

        *((ae_q32s *)&pswZ[swLen - 1]) = aeqZ1;
    }
}
/*****************************************************************************
 函 数 名  : CODEC_OpVvFormWithCoef
 功能描述  : Z[i] = add(mult(X[i], a), mult(Y[i], b))
 输入参数  : Word16 *pshwX     - 向量1
             Word16 *pshwY     - 向量2
             Word32  swLen     - 向量长度
             Word16  shwA      - 向量1的加权系数
             Word16  shwB      - 向量2的加权系数
             Word16 *pshwZ     - 保存结果的向量
 输出参数  : Word16 *pshwZ  - 计算结果
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月20日
    作    者   : 金剑
    修改内容   : 新生成函数

*****************************************************************************/
XT_INLINE void CODEC_OpVvFormWithCoef(
                const Word16 *pshwX,
                const Word16 *pshwY,
                const Word32  swLen,
                const Word16  shwA,
                const Word16  shwB,
                Word16 *pshwZ)
{
    Word32 k = 0;
    Word32 swLastIndex = 0;

    ae_p24x2s aepX, aepY, aepZ, aepA, aepB;

    ae_q56s aeqL, aeqH;

    if(swLen < 0)
    {
        return;
    }

    /* shwA-->aepA.HL */
    aepA = *((ae_p16s *)&shwA);

    /* shwB-->shwB.HL */
    aepB = *((ae_p16s *)&shwB);

    if (CODEC_OpCheck4ByteAligned((Word32)pshwX|(Word32)pshwY|(Word32)pshwZ))
    {
        /* 向量地址为4字节对齐时采用双操作 */
        for (k = 0; k < swLen>>1; k++)
        {
            /* pshwX[2*k]-->pshwX.H; pshwX[2*k+1]-->pshwX.L */
            aepX = *((ae_p16x2s *)&pshwX[2*k]);

            /* aepX.H*aepA.H*2带饱和-->aeqH(1.48) */
            aeqH = AE_MULFS32P16S_HH(aepX, aepA);

            /* aepX.L*aepA.L*2带饱和-->aeqL(1.48) */
            aeqL = AE_MULFS32P16S_LL(aepX, aepA);

            /* aeqH(25.48)-->aepX.H; aeqL(25.48)-->aepX.L */
            aepX = AE_TRUNCP24Q48X2(aeqH, aeqL);
            aepX = AE_TRUNCP16(aepX);

            /* pshwY[2*k]-->aepY.H; pshwY[2*k+1]-->aepY.L */
            aepY = *((ae_p16x2s *)&pshwY[2*k]);

            /* aepY.H*aepB.H*2带饱和-->aeqH(1.48) */
            aeqH = AE_MULFS32P16S_HH(aepY, aepB);

            /* aepY.L*aepB.L*2带饱和-->aeqL(1.48) */
            aeqL = AE_MULFS32P16S_LL(aepY, aepB);

            /* aeqH(25.48)-->aepY.H; aeqL(25.48)-->aepY.L */
            aepY = AE_TRUNCP24Q48X2(aeqH, aeqL);
            aepY = AE_TRUNCP16(aepY);

            /* aepX.L+aepY.L-->aepZ.L; aepX.H+aepY.H-->aepZ.H */
            aepZ = AE_ADDSP24S(aepX, aepY);

            /* aepZ.H-->pshwZ[2*k]; aepZ.L-->pshwZ[2*k+1]*/
            *((ae_p16x2s *)&pshwZ[2*k]) = aepZ;
        }

        /* 向量长度为奇数时，对最后一个元素单操作 */
        if (CODEC_OpCheckBit0Set(swLen))
        {
            swLastIndex = swLen - 1;

            /* pshwX[swLen-1]-->pshwX.HL; */
            aepX = *((ae_p16s *)&pshwX[swLastIndex]);

            /* aepX.L*aepA.L*2带饱和-->aeqL(1.48) */
            aeqL = AE_MULFS32P16S_LL(aepX, aepA);

            /* aeqL(25.48)-->aepX.L */
            aepX = AE_TRUNCP24Q48(aeqL);
            aepX = AE_TRUNCP16(aepX);

            /* pshwY[swLen-1]-->aepY.HL */
            aepY = *((ae_p16s *)&pshwY[swLastIndex]);

            /* aepY.L*aepB.L*2带饱和-->aeqL(1.48) */
            aeqL = AE_MULFS32P16S_LL(aepY, aepB);

            /* aeqH(25.48)-->aepY.H; aeqL(25.48)-->aepY.L */
            aepY = AE_TRUNCP24Q48(aeqL);
            aepY = AE_TRUNCP16(aepY);

            /* aepX.L+aepY.L-->aepZ.L */
            aepZ = AE_ADDSP24S(aepX, aepY);

            /* aepZ.L-->pshwZ[swLen-1] */
            *((ae_p16s *)&pshwZ[swLastIndex]) = aepZ;
        }
    }
    else
    {
        /* 向量地址不为4字节对齐时采用单操作 */
        for (k = 0; k < swLen; k++)
        {
            /* pshwX[k]-->pshwX.HL; */
            aepX = *((ae_p16s *)&pshwX[k]);

            /* aepX.L*aepA.L*2带饱和-->aeqL(1.48) */
            aeqL = AE_MULFS32P16S_LL(aepX, aepA);

            /* aeqL(25.48)-->aepX.L */
            aepX = AE_TRUNCP24Q48(aeqL);
            aepX = AE_TRUNCP16(aepX);

            /* pshwY[k]-->aepY.HL */
            aepY = *((ae_p16s *)&pshwY[k]);

            /* aepY.L*aepB.L*2带饱和-->aeqL(1.48) */
            aeqL = AE_MULFS32P16S_LL(aepY, aepB);

            /* aeqH(25.48)-->aepY.H; aeqL(25.48)-->aepY.L */
            aepY = AE_TRUNCP24Q48(aeqL);
            aepY = AE_TRUNCP16(aepY);

            /* aepX.L+aepY.L-->aepZ.L */
            aepZ = AE_ADDSP24S(aepX, aepY);

            /* aepZ.L-->pshwZ[k] */
            *((ae_p16s *)&pshwZ[k]) = aepZ;
        }
    }
}

/*****************************************************************************
 函 数 名  : CODEC_OpVvFormWithDimi
 功能描述  : Z[i] = add(X[i]>>1, Y[i]>>1)
 输入参数  : Word16 *pshwX    - 向量1
             Word16 *pshwY    - 向量2
             Word32  swLen    - 向量长度
             Word16 *pshwZ    - 保存结果的向量
 输出参数  : Word16 *pshwZ    - 计算结果
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月20日
    作    者   : 金剑
    修改内容   : 新生成函数

*****************************************************************************/
XT_INLINE void CODEC_OpVvFormWithDimi(
                Word16 *pshwX,
                Word16 *pshwY,
                Word32  swLen,
                Word16 *pshwZ)
{
    Word32 k = 0;
    Word32 swLastIndex = 0;

    ae_p24x2s aepX, aepY, aepZ;

    ae_q56s aeqL, aeqH;

    if(swLen < 0)
    {
        return;
    }

    /* 将需要右移比特数写入移位寄存器 */
    WUR_AE_SAR(1);

    if (CODEC_OpCheck4ByteAligned((Word32)pshwX|(Word32)pshwY|(Word32)pshwZ))
    {
        /* 向量地址为4字节对齐时采用双操作 */
        for (k = 0; k < swLen>>1; k++)
        {
            /* pshwX[2*k]-->pshwX.H; pshwX[2*k+1]-->pshwX.L */
            aepX = *((ae_p16x2s *)&pshwX[2*k]);

            /* aepX.HL算术右移 --> aepX.HL */
            aepX = AE_SRASP24(aepX);
            aepX = AE_TRUNCP16(aepX);

            /* pshwY[2*k]-->aepY.H; pshwY[2*k+1]-->aepY.L */
            aepY = *((ae_p16x2s *)&pshwY[2*k]);

            /* aepY.HL算术右移 --> aepY.HL */
            aepY = AE_SRASP24(aepY);
            aepY = AE_TRUNCP16(aepY);

            /* aepX.L+aepY.L-->aepZ.L; aepX.H+aepY.H-->aepZ.H */
            aepZ = AE_ADDSP24S(aepX, aepY);

            /* aepZ.H-->pshwZ[2*k]; aepZ.L-->pshwZ[2*k+1]*/
            *((ae_p16x2s *)&pshwZ[2*k]) = aepZ;
        }

        /* 向量长度为奇数时，对最后一个元素单操作 */
        if (CODEC_OpCheckBit0Set(swLen))
        {
            swLastIndex = swLen - 1;

            /* pshwX[swLen-1]-->pshwX.HL; */
            aepX = *((ae_p16s *)&pshwX[swLastIndex]);

            /* aepX.HL算术右移 --> aepX.HL */
            aepX = AE_SRASP24(aepX);
            aepX = AE_TRUNCP16(aepX);

            /* pshwY[swLen-1]-->aepY.HL */
            aepY = *((ae_p16s *)&pshwY[swLastIndex]);

            /* aepY.HL算术右移 --> aepY.HL */
            aepY = AE_SRASP24(aepY);
            aepY = AE_TRUNCP16(aepY);

            /* aepX.L+aepY.L-->aepZ.L */
            aepZ = AE_ADDSP24S(aepX, aepY);

            /* aepZ.L-->pshwZ[swLen-1] */
            *((ae_p16s *)&pshwZ[swLastIndex]) = aepZ;
        }
    }
    else
    {
        /* 向量地址不为4字节对齐时采用单操作 */
        for (k = 0; k < swLen; k++)
        {
            /* pshwX[k]-->pshwX.HL; */
            aepX = *((ae_p16s *)&pshwX[k]);

            /* aepX.HL算术右移 --> aepX.HL */
            aepX = AE_SRASP24(aepX);
            aepX = AE_TRUNCP16(aepX);

            /* pshwY[k]-->aepY.HL */
            aepY = *((ae_p16s *)&pshwY[k]);

            /* aepY.HL算术右移 --> aepY.HL */
            aepY = AE_SRASP24(aepY);
            aepY = AE_TRUNCP16(aepY);

            /* aepX.L+aepY.L-->aepZ.L */
            aepZ = AE_ADDSP24S(aepX, aepY);

            /* aepZ.L-->pshwZ[k] */
            *((ae_p16s *)&pshwZ[k]) = aepZ;
        }
    }
}

/*****************************************************************************
 函 数 名  : CODEC_OpVvFormWithQuar
 功能描述  : Z[i] = add(X[i]>>2, sub(Y[i],Y[i]>>2))
 输入参数  : Word16 *pshwX   - 加权系数为0.25的向量
             Word16 *pshwY   - 加权系数为0.75的向量
             Word32  swLen   - 向量长度
             Word16 *pshwZ   - 保存结果的向量
 输出参数  : Word16 *pshwZ   - 计算结果
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月20日
    作    者   : 金剑
    修改内容   : 新生成函数

*****************************************************************************/
XT_INLINE void CODEC_OpVvFormWithQuar(
                Word16 *pshwX,
                Word16 *pshwY,
                Word32  swLen,
                Word16 *pshwZ)
{
    Word32 k = 0;
    Word32 swLastIndex = 0;

    ae_p24x2s aepX, aepY, aepT, aepZ;

    ae_q56s aeqL, aeqH;

    if(swLen < 0)
    {
        return;
    }

    /* 将需要右移比特数写入移位寄存器 */
    WUR_AE_SAR(2);
    if (CODEC_OpCheck4ByteAligned((Word32)pshwX|(Word32)pshwY|(Word32)pshwZ))
    {
        /* 向量地址为4字节对齐时采用双操作 */
        for (k = 0; k < swLen>>1; k++)
        {
            /* pshwX[2*k]-->pshwX.H; pshwX[2*k+1]-->pshwX.L */
            aepX = *((ae_p16x2s *)&pshwX[2*k]);

            /* aepX.HL算术右移 --> aepX.HL */
            aepX = AE_SRASP24(aepX);
            aepX = AE_TRUNCP16(aepX);

            /* pshwY[2*k]-->aepY.H; pshwY[2*k+1]-->aepY.L */
            aepY = *((ae_p16x2s *)&pshwY[2*k]);

            /* aepY.HL算术右移 --> aepT.HL */
            aepT = AE_SRASP24(aepY);
            aepT = AE_TRUNCP16(aepT);

            /* aepY.L-aepT.L-->aepY.L; aepY.H-aepT.H-->aepY.H */
            aepY = AE_SUBSP24S(aepY, aepT);

            /* aepX.L+aepY.L-->aepZ.L; aepX.H+aepY.H-->aepZ.H */
            aepZ = AE_ADDSP24S(aepX, aepY);

            /* aepZ.H-->pshwZ[2*k]; aepZ.L-->pshwZ[2*k+1]*/
            *((ae_p16x2s *)&pshwZ[2*k]) = aepZ;
        }

        /* 向量长度为奇数时，对最后一个元素单操作 */
        if (CODEC_OpCheckBit0Set(swLen))
        {
            swLastIndex = swLen - 1;

            /* pshwX[swLen-1]-->pshwX.HL; */
            aepX = *((ae_p16s *)&pshwX[swLastIndex]);

            /* aepX.HL算术右移 --> aepX.HL */
            aepX = AE_SRASP24(aepX);
            aepX = AE_TRUNCP16(aepX);

            /* pshwY[swLen-1]-->aepY.HL */
            aepY = *((ae_p16s *)&pshwY[swLastIndex]);

            /* aepY.HL算术右移 --> aepT.HL */
            aepT = AE_SRASP24(aepY);
            aepT = AE_TRUNCP16(aepT);

            /* aepY.L-aepT.L-->aepY.L; */
            aepY = AE_SUBSP24S(aepY, aepT);

            /* aepX.L+aepY.L-->aepZ.L */
            aepZ = AE_ADDSP24S(aepX, aepY);

            /* aepZ.L-->pshwZ[swLen-1] */
            *((ae_p16s *)&pshwZ[swLastIndex]) = aepZ;
        }
    }
    else
    {
        /* 向量地址不为4字节对齐时采用单操作 */
        for (k = 0; k < swLen; k++)
        {
            /* pshwX[k]-->pshwX.HL; */
            aepX = *((ae_p16s *)&pshwX[k]);

            /* aepX.HL算术右移 --> aepX.HL */
            aepX = AE_SRASP24(aepX);
            aepX = AE_TRUNCP16(aepX);

            /* pshwY[k]-->aepY.HL */
            aepY = *((ae_p16s *)&pshwY[k]);

            /* aepY.HL算术右移 --> aepT.HL */
            aepT = AE_SRASP24(aepY);
            aepT = AE_TRUNCP16(aepT);

            /* aepY.L-aepT.L-->aepY.L; */
            aepY = AE_SUBSP24S(aepY, aepT);

            /* aepX.L+aepY.L-->aepZ.L */
            aepZ = AE_ADDSP24S(aepX, aepY);

            /* aepZ.L-->pshwZ[k] */
            *((ae_p16s *)&pshwZ[k]) = aepZ;
        }
    }
}

/*****************************************************************************
 函 数 名  : CODEC_OpVvMac
 功能描述  : Y = sum(X[i]*Y[i])+sum
 输入参数  : Word16 *pshwX    - 输入向量X
             Word16 *pshwY    - 输入向量Y
             Word32 swLen     - 向量长度
             Word32 swSum     - 累加初始值
 输出参数  : 无
 返 回 值  : Word32 累加和
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月20日
    作    者   : 金剑
    修改内容   : 新生成函数
  2.日    期   : 2011年9月1日
    作    者   : 邵聃
    修改内容   : 提升运算效率

*****************************************************************************/

XT_INLINE Word32 CODEC_OpVvMac(
                const Word16 *pshwX,
                const Word16 *pshwY,
                const Word32  swLen,
                Word32        swSum)
{
    Word32 k;
    ae_p24x2s aepX1, aepX2, aepY1, aepY2;
    ae_q56s aeqS1;

    /* swSum-->aeqS(17.48) */
    aeqS1 = AE_CVTQ48A32S(swSum);

    if (CODEC_OpCheck4ByteAligned((Word32)pshwX|(Word32)pshwY))
    {
        /* 2组并行计算,避免nop指令 */
        for (k = 0; k < swLen>>2; k++)
        {
            aepX1 = *((ae_p16x2s *)&pshwX[4*k]);
            aepX2 = *((ae_p16x2s *)&pshwX[4*k + 2]);

            aepY1 = *((ae_p16x2s *)&pshwY[4*k]);
            aepY2 = *((ae_p16x2s *)&pshwY[4*k + 2]);

            AE_MULAAFP24S_HH_LL(aeqS1, aepX1, aepY1);
            AE_MULAAFP24S_HH_LL(aeqS1, aepX2, aepY2);

            /* 饱和到48bit */
            aeqS1 = AE_SATQ48S(aeqS1);
        }

        /* 1组并行计算 */
        if (CODEC_OpCheckBit1Set(swLen))
        {
            k = (swLen & 0x7FFFFFFC);

            aepX1 = *((ae_p16x2s *)&pshwX[k]);
            aepY1 = *((ae_p16x2s *)&pshwY[k]);

            AE_MULAAFP24S_HH_LL(aeqS1, aepX1, aepY1);
        }

        /* 饱和到48bit */
        aeqS1 = AE_SATQ48S(aeqS1);

    }
    else
    {
        /* 向量地址不为4字节对齐时采用单操作 */
        for (k = 0; k < swLen>>1; k++)
        {
            aepX1 = *((ae_p16s *)&pshwX[2*k]);
            aepX2 = *((ae_p16s *)&pshwX[2*k + 1]);

            aepY1 = *((ae_p16s *)&pshwY[2*k]);
            aepY2 = *((ae_p16s *)&pshwY[2*k + 1]);

            AE_MULAFS32P16S_LL(aeqS1, aepX1, aepY1);
            AE_MULAFS32P16S_LL(aeqS1, aepX2, aepY2);
        }
    }

    /* 向量长度为奇数时，对最后一个元素单操作 */
    if (CODEC_OpCheckBit0Set(swLen))
    {
        aepX1 = *((ae_p16s *)&pshwX[swLen - 1]);
        aepY1 = *((ae_p16s *)&pshwY[swLen - 1]);

        AE_MULAFS32P16S_LL(aeqS1, aepX1, aepY1);
    }

    return AE_TRUNCA32Q48(aeqS1);
}

/*****************************************************************************
 函 数 名  : CODEC_OpVvMacAlignless
 功能描述  : Y = sum(X[i]*Y[i])+sum
 输入参数  : Word16 *pshwX    - 输入向量X
             Word16 *pshwY    - 输入向量Y
             Word32 swLen     - 向量长度
             Word32 swSum     - 累加初始值
 输出参数  : 无
 返 回 值  : Word32 累加和
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月20日
    作    者   : 金剑
    修改内容   : 新生成函数
  2.日    期   : 2011年9月1日
    作    者   : 邵聃
    修改内容   : 提升运算效率

*****************************************************************************/

XT_INLINE Word32 CODEC_OpVvMacAlignless(
                const Word16 *pshwX,
                const Word16 *pshwY,
                const Word32  swLen,
                Word32        swSum)
{
    Word32 k;
    ae_p24x2s aepX1, aepX2, aepY1, aepY2;
    ae_q56s aeqS1;

    /* swSum-->aeqS(17.48) */
    aeqS1 = AE_CVTQ48A32S(swSum);

    /* 向量地址不为4字节对齐时采用单操作 */
    for (k = 0; k < swLen>>1; k++)
    {
        aepX1 = *((ae_p16s *)&pshwX[2*k]);
        aepX2 = *((ae_p16s *)&pshwX[2*k + 1]);

        aepY1 = *((ae_p16s *)&pshwY[2*k]);
        aepY2 = *((ae_p16s *)&pshwY[2*k + 1]);

        AE_MULAFS32P16S_LL(aeqS1, aepX1, aepY1);
        AE_MULAFS32P16S_LL(aeqS1, aepX2, aepY2);
    }

    /* 向量长度为奇数时，对最后一个元素单操作 */
    if (CODEC_OpCheckBit0Set(swLen))
    {
        aepX1 = *((ae_p16s *)&pshwX[swLen - 1]);
        aepY1 = *((ae_p16s *)&pshwY[swLen - 1]);

        AE_MULAFS32P16S_LL(aeqS1, aepX1, aepY1);
    }

    return AE_TRUNCA32Q48(aeqS1);
}

/*****************************************************************************
 函 数 名  : CODEC_OpVvMacAligned
 功能描述  : Y = sum(X[i]*Y[i])+sum
 输入参数  : Word16 *pshwX    - 输入向量X(4字节对齐)
             Word16 *pshwY    - 输入向量Y(4字节对齐)
             Word32 swLen     - 向量长度
             Word32 swSum     - 累加初始值
 输出参数  : 无
 返 回 值  : Word32 累加和
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月20日
    作    者   : 金剑
    修改内容   : 新生成函数
  2.日    期   : 2011年9月1日
    作    者   : 邵聃
    修改内容   : 提升运算效率

*****************************************************************************/
XT_INLINE Word32 CODEC_OpVvMacAligned(
                const Word16 *pshwX,
                const Word16 *pshwY,
                const Word32  swLen,
                Word32        swSum)
{
    Word32 k;
    ae_p24x2s aepX1, aepX2, aepY1, aepY2;
    ae_q56s aeqS1;

    /* swSum-->aeqS(17.48) */
    aeqS1 = AE_CVTQ48A32S(swSum);

    /* 2组并行计算,避免nop指令 */
    for (k = 0; k < swLen>>2; k++)
    {
        aepX1 = *((ae_p16x2s *)&pshwX[4*k]);

        aepY1 = *((ae_p16x2s *)&pshwY[4*k]);
        aeqS1 = AE_SATQ48S(aeqS1);

        aepX2 = *((ae_p16x2s *)&pshwX[4*k + 2]);
        aepY2 = *((ae_p16x2s *)&pshwY[4*k + 2]);

        AE_MULAAFP24S_HH_LL(aeqS1, aepX1, aepY1);
        AE_MULAAFP24S_HH_LL(aeqS1, aepX2, aepY2);

    }
    aeqS1 = AE_SATQ48S(aeqS1);

    /* 1组并行计算 */
    if (CODEC_OpCheckBit1Set(swLen))
    {
        k = (swLen & 0x7FFFFFFC);

        aepX1 = *((ae_p16x2s *)&pshwX[k]);
        aepY1 = *((ae_p16x2s *)&pshwY[k]);

        AE_MULAAFP24S_HH_LL(aeqS1, aepX1, aepY1);
    }

    /* 饱和到48bit */
    aeqS1 = AE_SATQ48S(aeqS1);

    /* 向量长度为奇数时，对最后一个元素单操作 */
    if (CODEC_OpCheckBit0Set(swLen))
    {
        aepX1 = *((ae_p16s *)&pshwX[swLen - 1]);
        aepY1 = *((ae_p16s *)&pshwY[swLen - 1]);

        AE_MULAFS32P16S_LL(aeqS1, aepX1, aepY1);
    }

    return AE_TRUNCA32Q48(aeqS1);
}

/*****************************************************************************
 函 数 名  : CODEC_OpVvMacAlignedEven
 功能描述  : Y = sum(X[i]*Y[i])+sum
 输入参数  : Word16 *pshwX    - 输入向量X(4字节对齐)
             Word16 *pshwY    - 输入向量Y(4字节对齐)
             Word32 swLen     - 向量长度(偶数)
             Word32 swSum     - 累加初始值
 输出参数  : 无
 返 回 值  : Word32 累加和
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月20日
    作    者   : 金剑
    修改内容   : 新生成函数
  2.日    期   : 2011年9月1日
    作    者   : 邵聃
    修改内容   : 提升运算效率

*****************************************************************************/

XT_INLINE Word32 CODEC_OpVvMacAlignedEven(
                const Word16 *pshwX,
                const Word16 *pshwY,
                const Word32  swLen,
                Word32        swSum)
{
    Word32 k;
    ae_p24x2s aepX1, aepX2, aepY1, aepY2;
    ae_q56s aeqS1;

    /* swSum-->aeqS(17.48) */
    aeqS1 = AE_CVTQ48A32S(swSum);

    /* 2组并行计算,避免nop指令 */
    for (k = 0; k < swLen>>2; k++)
    {
        aepX1 = *((ae_p16x2s *)&pshwX[4*k]);

        aepY1 = *((ae_p16x2s *)&pshwY[4*k]);
        aeqS1 = AE_SATQ48S(aeqS1);

        aepX2 = *((ae_p16x2s *)&pshwX[4*k + 2]);
        aepY2 = *((ae_p16x2s *)&pshwY[4*k + 2]);

        AE_MULAAFP24S_HH_LL(aeqS1, aepX1, aepY1);
        AE_MULAAFP24S_HH_LL(aeqS1, aepX2, aepY2);
    }
    aeqS1 = AE_SATQ48S(aeqS1);

    /* 1组并行计算 */
    if (CODEC_OpCheckBit1Set(swLen))
    {
        k = (swLen & 0x7FFFFFFC);

        aepX1 = *((ae_p16x2s *)&pshwX[k]);
        aepY1 = *((ae_p16x2s *)&pshwY[k]);

        AE_MULAAFP24S_HH_LL(aeqS1, aepX1, aepY1);
    }

    /* 饱和到48bit */
    aeqS1 = AE_SATQ48S(aeqS1);

    return AE_TRUNCA32Q48(aeqS1);
}

/*****************************************************************************
 函 数 名  : CODEC_OpVvMacAlignedQuan
 功能描述  : Y = sum(X[i]*Y[i])+sum
 输入参数  : Word16 *pshwX    - 输入向量X(4字节对齐)
             Word16 *pshwY    - 输入向量Y(4字节对齐)
             Word32 swLen     - 向量长度(4的整数倍)
             Word32 swSum     - 累加初始值
 输出参数  : 无
 返 回 值  : Word32 累加和
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月20日
    作    者   : 金剑
    修改内容   : 新生成函数
  2.日    期   : 2011年9月1日
    作    者   : 邵聃
    修改内容   : 提升运算效率

*****************************************************************************/

XT_INLINE Word32 CODEC_OpVvMacAlignedQuan(
                const Word16 *pshwX,
                const Word16 *pshwY,
                const Word32  swLen,
                Word32        swSum)
{
    Word32 k;
    ae_p24x2s aepX1, aepX2, aepY1, aepY2;
    ae_q56s aeqS1;

    aepX1 = *((ae_p16x2s *)pshwX);
    aepY1 = *((ae_p16x2s *)pshwY);

    /* swSum-->aeqS(17.48) */
    aeqS1 = AE_CVTQ48A32S(swSum);

    /* 2组并行计算,避免nop指令 */
    for (k = 0; k < swLen>>2; k++)
    {
        aeqS1 = AE_SATQ48S(aeqS1);
        aepX2 = *((ae_p16x2s *)&pshwX[4*k+2]);
        aepY2 = *((ae_p16x2s *)&pshwY[4*k+2]);

        AE_MULAAFP24S_HH_LL(aeqS1, aepX1, aepY1);
        aepX1 = *((ae_p16x2s *)&pshwX[4*k+4]);
        aepY1 = *((ae_p16x2s *)&pshwY[4*k+4]);

        AE_MULAAFP24S_HH_LL(aeqS1, aepX2, aepY2);
    }
    aeqS1 = AE_SATQ48S(aeqS1);

    return AE_TRUNCA32Q48(aeqS1);
}

/*****************************************************************************
 函 数 名  : CODEC_OpVvSelfMacAlignedQuan
 功能描述  : Y = sum(X[i]*X[i])+sum
 输入参数  : Word16 *pshwX    - 输入向量X(4字节对齐)
             Word32 swLen     - 向量长度(4的整数倍)
             Word32 swSum     - 累加初始值
 输出参数  : 无
 返 回 值  : Word32 累加和
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月20日
    作    者   : 金剑
    修改内容   : 新生成函数
  2.日    期   : 2011年9月1日
    作    者   : 邵聃
    修改内容   : 提升运算效率

*****************************************************************************/

XT_INLINE Word32 CODEC_OpVvSelfMacAlignedQuan(
                const Word16 *pshwX,
                const Word32  swLen,
                Word32        swSum)
{
    Word32 k;
    ae_p24x2s aepX1, aepX2;
    ae_q56s aeqS1;

    aepX1 = *((ae_p16x2s *)pshwX);

    /* swSum-->aeqS(17.48) */
    aeqS1 = AE_CVTQ48A32S(swSum);

    /* 2组并行计算,避免nop指令 */
    for (k = 0; k < swLen>>2; k++)
    {
        aeqS1 = AE_SATQ48S(aeqS1);
        aepX2 = *((ae_p16x2s *)&pshwX[4*k+2]);

        AE_MULAAFP24S_HH_LL(aeqS1, aepX1, aepX1);
        aepX1 = *((ae_p16x2s *)&pshwX[4*k+4]);

        AE_MULAAFP24S_HH_LL(aeqS1, aepX2, aepX2);
    }

    aeqS1 = AE_SATQ48S(aeqS1);

    return AE_TRUNCA32Q48(aeqS1);
}

/*****************************************************************************
 函 数 名  : CODEC_OpVvMacD
 功能描述  : Y = sum(X[i]*Y[-i])+sum
 输入参数  : Word16 *pshwX    - 输入向量X
             Word16 *pshwY    - 输入向量Y
             Word32 swLen     - 向量长度
             Word32 swSum     - 累加初始值
 输出参数  : 无
 返 回 值  : Word32 累加和
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年9月1日
    作    者   : 邵聃
    修改内容   : 新建函数

*****************************************************************************/
XT_INLINE Word32 CODEC_OpVvMacD(
                const Word16 *pshwX,
                const Word16 *pshwY,
                const Word32  swLen,
                Word32        swSum)
{
    Word32 k;
    ae_p24x2s aepX1, aepX2, aepY1, aepY2;
    ae_q56s aeqS1;

    /* swSum-->aeqS(17.48) */
    aeqS1 = AE_CVTQ48A32S(swSum);

    if (CODEC_OpCheck4ByteAligned((Word32)pshwX|(Word32)(pshwY-1)))
    {
        /* 2组并行计算,避免nop指令 */
        for (k = 0; k < swLen>>2; k++)
        {
            aepX1 = *((ae_p16x2s *)&pshwX[4*k]);
            aepX2 = *((ae_p16x2s *)&pshwX[4*k + 2]);

            aepY1 = *((ae_p16x2s *)&pshwY[- 4*k - 1]);
            aepY2 = *((ae_p16x2s *)&pshwY[- 4*k - 3]);

            AE_MULAAFP24S_HL_LH(aeqS1, aepX1, aepY1);
            AE_MULAAFP24S_HL_LH(aeqS1, aepX2, aepY2);
        }

        /* 1组并行计算 */
        if (CODEC_OpCheckBit1Set(swLen))
        {
            k = (swLen & 0x7FFFFFFC);

            aepX1 = *((ae_p16x2s *)&pshwX[k]);
            aepY1 = *((ae_p16x2s *)&pshwY[- k - 1]);

            AE_MULAAFP24S_HL_LH(aeqS1, aepX1, aepY1);
        }

        /* 饱和到48bit */
        aeqS1 = AE_SATQ48S(aeqS1);

    }
    else
    {

        /* 向量地址不为4字节对齐时采用单操作 */
        for (k = 0; k < swLen>>1; k++)
        {
            aepX1 = *((ae_p16s *)&pshwX[2*k]);
            aepX2 = *((ae_p16s *)&pshwX[2*k + 1]);

            aepY1 = *((ae_p16s *)&pshwY[- 2*k]);
            aepY2 = *((ae_p16s *)&pshwY[- 2*k - 1]);

            AE_MULAFS32P16S_LL(aeqS1, aepX1, aepY1);
            AE_MULAFS32P16S_LL(aeqS1, aepX2, aepY2);
        }
    }

    /* 向量长度为奇数时，对最后一个元素单操作 */
    if (CODEC_OpCheckBit0Set(swLen))
    {
        aepX1 = *((ae_p16s *)&pshwX[swLen - 1]);
        aepY1 = *((ae_p16s *)&pshwY[1 - swLen]);

        AE_MULAFS32P16S_LL(aeqS1, aepX1, aepY1);
    }

    return AE_TRUNCA32Q48(aeqS1);
}

/*****************************************************************************
 函 数 名  : CODEC_OpVvMsu
 功能描述  : Y = sum - sum(X[i]*Y[i])
 输入参数  : const Word16 *pshwX    - 16bit型输入向量
             const Word16 *pshwY    - 16bit型输入向量
             Word32 swLen           - 输入向量长度
             Word32 swSum           - 累减初始数值
 输出参数  : 无
 返 回 值  : Word32 向量点乘与累加初始数值相减的结果
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月20日
    作    者   : 金剑
    修改内容   : 新生成函数
  2.日    期   : 2011年9月1日
    作    者   : 邵聃
    修改内容   : 提升运算效率

*****************************************************************************/
XT_INLINE Word32 CODEC_OpVvMsu(
                const Word16 *pshwX,
                const Word16 *pshwY,
                const Word32  swLen,
                Word32        swSum)
{
    Word32 k, swPos, swPartLen;
    ae_p24x2s aepX1, aepX2, aepY1, aepY2;
    ae_q56s aeqS1, aeqS2;

    if(swLen < 0)
    {
        return swSum;
    }

    /* swSum-->aeqS(17.48) */
    aeqS1 = AE_CVTQ48A32S(swSum);

    if (CODEC_OpCheck4ByteAligned((Word32)pshwX|(Word32)pshwY))
    {
        /* 记录当前已累减的数据长度 */
        swPos = 0;

        do
        {
            /* 每次计算长度不能超过CODEC_OP_MULFP24S_MAX_LEN个,否则有溢出风险 */
            swPartLen = (swLen - swPos) > CODEC_OP_MULFP24S_MAX_LEN ? CODEC_OP_MULFP24S_MAX_LEN : (swLen - swPos);

            aeqS2 = AE_ZEROQ56();

            /* 2组并行计算,避免nop指令 */
            for (k = 0; k < swPartLen>>2; k++)
            {
                aepX1 = *((ae_p16x2s *)&pshwX[swPos + 4*k]);
                aepX2 = *((ae_p16x2s *)&pshwX[swPos + 4*k + 2]);

                aepY1 = *((ae_p16x2s *)&pshwY[swPos + 4*k]);
                aepY2 = *((ae_p16x2s *)&pshwY[swPos + 4*k + 2]);

                AE_MULSSFP24S_HH_LL(aeqS2, aepX1, aepY1);
                AE_MULSSFP24S_HH_LL(aeqS2, aepX2, aepY2);
            }

            /* 1组并行计算 */
            if (CODEC_OpCheckBit1Set(swPartLen))
            {
                k = swPos + (swPartLen & 0x7FFFFFFC);

                aepX1 = *((ae_p16x2s *)&pshwX[k]);
                aepY1 = *((ae_p16x2s *)&pshwY[k]);

                AE_MULSSFP24S_HH_LL(aeqS2, aepX1, aepY1);
            }

            /* 更新当前位置 */
            swPos += swPartLen;

            /* 进行饱和运算 */
            aeqS1 = AE_ADDSQ56S(aeqS1, aeqS2);

        } while (swPos < swLen);

        /* 饱和到48bit */
        aeqS1 = AE_SATQ48S(aeqS1);

    }
    else
    {
        /* 向量地址不为4字节对齐时采用单操作 */
        for (k = 0; k < swLen>>1; k++)
        {
            aepX1 = *((ae_p16s *)&pshwX[2*k]);
            aepX2 = *((ae_p16s *)&pshwX[2*k + 1]);

            aepY1 = *((ae_p16s *)&pshwY[2*k]);
            aepY2 = *((ae_p16s *)&pshwY[2*k + 1]);

            AE_MULSFS32P16S_LL(aeqS1, aepX1, aepY1);
            AE_MULSFS32P16S_LL(aeqS1, aepX2, aepY2);
        }
    }

    /* 向量长度为奇数时，对最后一个元素单操作 */
    if (CODEC_OpCheckBit0Set(swLen))
    {
        aepX1 = *((ae_p16s *)&pshwX[swLen - 1]);
        aepY1 = *((ae_p16s *)&pshwY[swLen - 1]);

        AE_MULSFS32P16S_LL(aeqS1, aepX1, aepY1);
    }

    return AE_TRUNCA32Q48(aeqS1);
}

/*****************************************************************************
 函 数 名  : CODEC_OpVvMsuD
 功能描述  : Y = sum - sum(X[i]*Y[-i])
 输入参数  : Word16 *pshwX    - 输入向量X
             Word16 *pshwY    - 输入向量Y
             Word32 swLen     - 向量长度
             Word32 swSum     - 累加初始值
 输出参数  : 无
 返 回 值  : Word32 累加和
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月20日
    作    者   : 金剑
    修改内容   : 新生成函数
  2.日    期   : 2011年9月1日
    作    者   : 邵聃
    修改内容   : 提升运算效率

*****************************************************************************/
XT_INLINE Word32 CODEC_OpVvMsuD(
                       const Word16 *pshwX,
                       const Word16 *pshwY,
                       const Word32  swLen,
                       Word32        swSum)
{
    Word32 k;
    ae_p24x2s aepX1, aepX2, aepY1, aepY2;
    ae_q56s aeqS1;

    /* swSum-->aeqS(17.48) */
    aeqS1 = AE_CVTQ48A32S(swSum);

    if (CODEC_OpCheck4ByteAligned((Word32)pshwX | (Word32)(pshwY-1)))
    {
        /* 2组并行计算,避免nop指令 */
        for (k = 0; k < swLen>>2; k++)
        {
            aepX1 = *((ae_p16x2s *)&pshwX[4*k]);
            aepX2 = *((ae_p16x2s *)&pshwX[4*k + 2]);

            aepY1 = *((ae_p16x2s *)&pshwY[- 4*k - 1]);
            aepY2 = *((ae_p16x2s *)&pshwY[- 4*k - 3]);

            AE_MULSSFP24S_HL_LH(aeqS1, aepX1, aepY1);
            AE_MULSSFP24S_HL_LH(aeqS1, aepX2, aepY2);
        }

        /* 1组并行计算 */
        if (CODEC_OpCheckBit1Set(swLen))
        {
            k = (swLen & 0x7FFFFFFC);

            aepX1 = *((ae_p16x2s *)&pshwX[k]);
            aepY1 = *((ae_p16x2s *)&pshwY[- k - 1]);

            AE_MULSSFP24S_HL_LH(aeqS1, aepX1, aepY1);
        }

        /* 饱和到48bit */
        aeqS1 = AE_SATQ48S(aeqS1);
    }
    else
    {
        /* 向量地址不为4字节对齐时采用单操作 */
        for (k = 0; k < swLen>>1; k++)
        {
            aepX1 = *((ae_p16s *)&pshwX[2*k]);
            aepX2 = *((ae_p16s *)&pshwX[2*k + 1]);

            aepY1 = *((ae_p16s *)&pshwY[-2*k]);
            aepY2 = *((ae_p16s *)&pshwY[-2*k - 1]);

            AE_MULSFS32P16S_LL(aeqS1, aepX1, aepY1);
            AE_MULSFS32P16S_LL(aeqS1, aepX2, aepY2);
        }
    }

    /* 向量长度为奇数时，对最后一个元素单操作 */
    if (CODEC_OpCheckBit0Set(swLen))
    {
        aepX1 = *((ae_p16s *)&pshwX[swLen - 1]);
        aepY1 = *((ae_p16s *)&pshwY[1 - swLen]);

        AE_MULSFS32P16S_LL(aeqS1, aepX1, aepY1);
    }

    return AE_TRUNCA32Q48(aeqS1);
}

/*****************************************************************************
 函 数 名  : CODEC_OpVvMsuDAlignedEven
 功能描述  : Y = sum - sum(X[i]*Y[-i])
 输入参数  : Word16 *pshwX    - 输入向量X(4字节对齐)
             Word16 *pshwY    - 输入向量Y(Y[-1]4字节对齐)
             Word32 swLen     - 向量长度(偶数)
             Word32 swSum     - 累加初始值
 输出参数  : 无
 返 回 值  : Word32 累加和
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月20日
    作    者   : 金剑
    修改内容   : 新生成函数
  2.日    期   : 2011年9月1日
    作    者   : 邵聃
    修改内容   : 提升运算效率

*****************************************************************************/
XT_INLINE Word32 CODEC_OpVvMsuDAlignedEven(
                       const Word16 *pshwX,
                       const Word16 *pshwY,
                       const Word32  swLen,
                       Word32        swSum)
{
    Word32 k;
    ae_p24x2s aepX1, aepX2, aepY1, aepY2;
    ae_q56s aeqS1;

    /* swSum-->aeqS(17.48) */
    aeqS1 = AE_CVTQ48A32S(swSum);

     /* 2组并行计算,避免nop指令 */
    for (k = 0; k < swLen>>2; k++)
    {
        aepX1 = *((ae_p16x2s *)&pshwX[4*k]);
        aepX2 = *((ae_p16x2s *)&pshwX[4*k + 2]);

        aepY1 = *((ae_p16x2s *)&pshwY[- 4*k - 1]);
        aepY2 = *((ae_p16x2s *)&pshwY[- 4*k - 3]);

        AE_MULSSFP24S_HL_LH(aeqS1, aepX1, aepY1);
        AE_MULSSFP24S_HL_LH(aeqS1, aepX2, aepY2);
     }

     /* 1组并行计算 */
     if (CODEC_OpCheckBit1Set(swLen))
     {
        k = (swLen & 0x7FFFFFFC);

        aepX1 = *((ae_p16x2s *)&pshwX[k]);
        aepY1 = *((ae_p16x2s *)&pshwY[- k - 1]);

        AE_MULSSFP24S_HL_LH(aeqS1, aepX1, aepY1);
     }

     /* 饱和到48bit */
     aeqS1 = AE_SATQ48S(aeqS1);

    return AE_TRUNCA32Q48(aeqS1);
}

/*****************************************************************************
 函 数 名  : CODEC_OpVvMsuDAlignedQuan
 功能描述  : Y = sum - sum(X[i]*Y[-i])
 输入参数  : Word16 *pshwX    - 输入向量X(4字节对齐)
             Word16 *pshwY    - 输入向量Y(Y[-1]4字节对齐)
             Word32 swLen     - 向量长度(4的倍数)
             Word32 swSum     - 累加初始值
 输出参数  : 无
 返 回 值  : Word32 累加和
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月20日
    作    者   : 金剑
    修改内容   : 新生成函数
  2.日    期   : 2011年9月1日
    作    者   : 邵聃
    修改内容   : 提升运算效率

*****************************************************************************/
XT_INLINE Word32 CODEC_OpVvMsuDAlignedQuan(
                       const Word16 *pshwX,
                       const Word16 *pshwY,
                       const Word32  swLen,
                       Word32        swSum)
{
    Word32 k;
    ae_p24x2s aepX1, aepX2, aepY1, aepY2;
    ae_q56s aeqS1;

    /* swSum-->aeqS(17.48) */
    aeqS1 = AE_CVTQ48A32S(swSum);

     /* 2组并行计算,避免nop指令 */
    for (k = 0; k < swLen>>2; k++)
    {
        aepX1 = *((ae_p16x2s *)&pshwX[4*k]);
        aepX2 = *((ae_p16x2s *)&pshwX[4*k + 2]);

        aepY1 = *((ae_p16x2s *)&pshwY[- 4*k - 1]);
        aepY2 = *((ae_p16x2s *)&pshwY[- 4*k - 3]);

        AE_MULSSFP24S_HL_LH(aeqS1, aepX1, aepY1);
        AE_MULSSFP24S_HL_LH(aeqS1, aepX2, aepY2);
     }

     /* 饱和到48bit */
     aeqS1 = AE_SATQ48S(aeqS1);

    return AE_TRUNCA32Q48(aeqS1);
}

/*****************************************************************************
 函 数 名  : CODEC_OpVvMsuDAlignedless
 功能描述  : Y = sum - sum(X[i]*Y[-i])
 输入参数  : Word16 *pshwX    - 输入向量X
             Word16 *pshwY    - 输入向量Y
             Word32 swLen     - 向量长度
             Word32 swSum     - 累加初始值
 输出参数  : 无
 返 回 值  : Word32 累加和
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月20日
    作    者   : 金剑
    修改内容   : 新生成函数
  2.日    期   : 2011年9月1日
    作    者   : 邵聃
    修改内容   : 提升运算效率

*****************************************************************************/
XT_INLINE Word32 CODEC_OpVvMsuDAlignedless(
                       const Word16 *pshwX,
                       const Word16 *pshwY,
                       const Word32  swLen,
                       Word32        swSum)
{
    Word32 k;
    ae_p24x2s aepX1, aepX2, aepY1, aepY2;
    ae_q56s aeqS1;

    /* swSum-->aeqS(17.48) */
    aeqS1 = AE_CVTQ48A32S(swSum);

    /* 向量地址不为4字节对齐时采用单操作 */
    for (k = 0; k < swLen>>1; k++)
    {
        aepX1 = *((ae_p16s *)&pshwX[2*k]);
        aepX2 = *((ae_p16s *)&pshwX[2*k + 1]);

        aepY1 = *((ae_p16s *)&pshwY[-2*k]);
        aepY2 = *((ae_p16s *)&pshwY[-2*k - 1]);

        AE_MULSFS32P16S_LL(aeqS1, aepX1, aepY1);
        AE_MULSFS32P16S_LL(aeqS1, aepX2, aepY2);
    }

    /* 向量长度为奇数时，对最后一个元素单操作 */
    if (CODEC_OpCheckBit0Set(swLen))
    {
        aepX1 = *((ae_p16s *)&pshwX[swLen - 1]);
        aepY1 = *((ae_p16s *)&pshwY[1 - swLen]);

        AE_MULSFS32P16S_LL(aeqS1, aepX1, aepY1);
    }

    return AE_TRUNCA32Q48(aeqS1);
}

/*****************************************************************************
 函 数 名  : CODEC_OpVvMsuDSemiAlignedQuan
 功能描述  : Y = sum - sum(X[i]*Y[-i])
 输入参数  : Word16 *pshwX    - 输入向量X(4字节对齐)
             Word16 *pshwY    - 输入向量Y(Y[-1]4字节不对齐)
             Word32 swLen     - 向量长度(4的倍数)
             Word32 swSum     - 累加初始值
 输出参数  : 无
 返 回 值  : Word32 累加和
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月20日
    作    者   : 金剑
    修改内容   : 新生成函数
  2.日    期   : 2011年9月1日
    作    者   : 邵聃
    修改内容   : 提升运算效率

*****************************************************************************/
XT_INLINE Word32 CODEC_OpVvMsuDSemiAlignedQuan(
                       const Word16 *pshwX,
                       const Word16 *pshwY,
                       const Word32  swLen,
                       Word32        swSum)
{
    Word32 k;
    ae_p24x2s aepX1, aepX2, aepY1, aepY2, aepY3, aepY4;
    ae_q56s aeqS1;

    /* swSum-->aeqS(17.48) */
    aeqS1 = AE_CVTQ48A32S(swSum);

     /* 2组并行计算,避免nop指令 */
    for (k = 0; k < swLen>>2; k++)
    {
        aepX1 = *((ae_p16x2s *)&pshwX[4*k]);
        aepX2 = *((ae_p16x2s *)&pshwX[4*k + 2]);

        aepY1 = *((ae_p16s *)&pshwY[- 4*k]);
        aepY2 = *((ae_p16s *)&pshwY[- 4*k - 1]);
        aepY3 = *((ae_p16s *)&pshwY[- 4*k - 2]);
        aepY4 = *((ae_p16s *)&pshwY[- 4*k - 3]);

        AE_MULSFS32P16S_HL(aeqS1, aepX1, aepY1);
        AE_MULSFS32P16S_LL(aeqS1, aepX1, aepY2);
        AE_MULSFS32P16S_HL(aeqS1, aepX2, aepY3);
        AE_MULSFS32P16S_LL(aeqS1, aepX2, aepY4);
     }

    return AE_TRUNCA32Q48(aeqS1);
}


XT_INLINE void CODEC_OpVvMult(
                const Word16 *pshwX,
                const Word16 *pshwY,
                const Word32  swLen,
                Word16       *pshwZ)
{
    Word32 k = 0;
    ae_p24x2s aepX1, aepY1, aepZ1, aepX2, aepY2, aepZ2;
    ae_q56s aeqL1, aeqH1, aeqL2, aeqH2;

    if(swLen < 0)
    {
        return;
    }

    if (CODEC_OpCheck4ByteAligned((Word32)pshwX|(Word32)pshwY|(Word32)pshwZ))
    {
        for (k = 0; k < swLen>>2; k++)
        {
            aepX1 = *((ae_p16x2s *)&pshwX[4*k]);
            aepX2 = *((ae_p16x2s *)&pshwX[4*k + 2]);

            aepY1 = *((ae_p16x2s *)&pshwY[4*k]);
            aepY2 = *((ae_p16x2s *)&pshwY[4*k + 2]);

            aeqH1 = AE_MULFS32P16S_HH(aepX1, aepY1);
            aeqH2 = AE_MULFS32P16S_HH(aepX2, aepY2);
            aeqL1 = AE_MULFS32P16S_LL(aepX1, aepY1);
            aeqL2 = AE_MULFS32P16S_LL(aepX2, aepY2);
            aepZ1 = AE_TRUNCP24Q48X2(aeqH1, aeqL1);
            aepZ2 = AE_TRUNCP24Q48X2(aeqH2, aeqL2);

            *((ae_p16x2s *)&pshwZ[4*k]) = aepZ1;
            *((ae_p16x2s *)&pshwZ[4*k + 2]) = aepZ2;
        }

        /* 1组并行计算 */
        if (CODEC_OpCheckBit1Set(swLen))
        {
            k = swLen & 0x7FFFFFFC;

            aepX1 = *((ae_p16x2s *)&pshwX[k]);
            aepY1 = *((ae_p16x2s *)&pshwY[k]);

            aeqH1 = AE_MULFS32P16S_HH(aepX1, aepY1);
            aeqL1 = AE_MULFS32P16S_LL(aepX1, aepY1);
            aepZ1 = AE_TRUNCP24Q48X2(aeqH1, aeqL1);

            *((ae_p16x2s *)&pshwZ[k]) = aepZ1;
        }
    }
    else
    {
        /* 向量地址不为4字节对齐时采用单操作 */
        for (k = 0; k < swLen>>1; k++)
        {
            aepX1 = *((ae_p16s *)&pshwX[2*k]);
            aepX2 = *((ae_p16s *)&pshwX[2*k + 1]);

            aepY1 = *((ae_p16s *)&pshwY[2*k]);
            aepY2 = *((ae_p16s *)&pshwY[2*k + 1]);

            aeqL1 = AE_MULFS32P16S_LL(aepX1, aepY1);
            aeqL2 = AE_MULFS32P16S_LL(aepX2, aepY2);
            aepZ1 = AE_TRUNCP24Q48(aeqL1);
            aepZ2 = AE_TRUNCP24Q48(aeqL2);

            *((ae_p16s *)&pshwZ[2*k]) = aepZ1;
            *((ae_p16s *)&pshwZ[2*k + 1]) = aepZ2;
        }
    }

    /* 向量长度为奇数时，对最后一个元素单操作 */
    if (CODEC_OpCheckBit0Set(swLen))
    {
        aepX1 = *((ae_p16s *)&pshwX[swLen - 1]);
        aepY1 = *((ae_p16s *)&pshwY[swLen - 1]);

        aeqL1 = AE_MULFS32P16S_LL(aepX1, aepY1);
        aepZ1 = AE_TRUNCP24Q48(aeqL1);

        *((ae_p16s *)&pshwZ[swLen - 1]) = aepZ1;
    }
}
XT_INLINE void CODEC_OpVvMultR(
                const Word16 *pshwX,
                const Word16 *pshwY,
                const Word32  swLen,
                Word16       *pshwZ)
{
    Word32 k = 0;
    ae_p24x2s aepX1, aepY1, aepZ1, aepX2, aepY2, aepZ2;
    ae_q56s aeqL1, aeqH1, aeqL2, aeqH2;

    if(swLen < 0)
    {
        return;
    }

    if (CODEC_OpCheck4ByteAligned((Word32)pshwX|(Word32)pshwY|(Word32)pshwZ))
    {
        for (k = 0; k < swLen>>2; k++)
        {
            aepX1 = *((ae_p16x2s *)&pshwX[4*k]);
            aepX2 = *((ae_p16x2s *)&pshwX[4*k + 2]);

            aepY1 = *((ae_p16x2s *)&pshwY[4*k]);
            aepY2 = *((ae_p16x2s *)&pshwY[4*k + 2]);

            aeqH1 = AE_MULFS32P16S_HH(aepX1, aepY1);
            aeqH2 = AE_MULFS32P16S_HH(aepX2, aepY2);
            aeqL1 = AE_MULFS32P16S_LL(aepX1, aepY1);
            aeqL2 = AE_MULFS32P16S_LL(aepX2, aepY2);

            aepZ1 = AE_TRUNCP24Q48X2(aeqH1, aeqL1);
            aepZ2 = AE_TRUNCP24Q48X2(aeqH2, aeqL2);

            aepZ1 = AE_ROUNDSP16ASYM(aepZ1);
            aepZ2 = AE_ROUNDSP16ASYM(aepZ2);

            *((ae_p16x2s *)&pshwZ[4*k]) = aepZ1;
            *((ae_p16x2s *)&pshwZ[4*k + 2]) = aepZ2;
        }

        /* 1组并行计算 */
        if (CODEC_OpCheckBit1Set(swLen))
        {
            k = swLen & 0x7FFFFFFC;

            aepX1 = *((ae_p16x2s *)&pshwX[k]);
            aepY1 = *((ae_p16x2s *)&pshwY[k]);

            aeqH1 = AE_MULFS32P16S_HH(aepX1, aepY1);
            aeqL1 = AE_MULFS32P16S_LL(aepX1, aepY1);

            aepZ1 = AE_TRUNCP24Q48X2(aeqH1, aeqL1);

            aepZ1 = AE_ROUNDSP16ASYM(aepZ1);

            *((ae_p16x2s *)&pshwZ[k]) = aepZ1;
        }
    }
    else
    {
        /* 向量地址不为4字节对齐时采用单操作 */
        for (k = 0; k < swLen>>1; k++)
        {
            aepX1 = *((ae_p16s *)&pshwX[2*k]);
            aepX2 = *((ae_p16s *)&pshwX[2*k + 1]);

            aepY1 = *((ae_p16s *)&pshwY[2*k]);
            aepY2 = *((ae_p16s *)&pshwY[2*k + 1]);

            aeqL1 = AE_MULFS32P16S_LL(aepX1, aepY1);
            aeqL2 = AE_MULFS32P16S_LL(aepX2, aepY2);
            aepZ1 = AE_TRUNCP24Q48(aeqL1);
            aepZ2 = AE_TRUNCP24Q48(aeqL2);

            aepZ1 = AE_ROUNDSP16ASYM(aepZ1);
            aepZ2 = AE_ROUNDSP16ASYM(aepZ2);

            *((ae_p16s *)&pshwZ[2*k]) = aepZ1;
            *((ae_p16s *)&pshwZ[2*k + 1]) = aepZ2;
        }
    }

    /* 向量长度为奇数时，对最后一个元素单操作 */
    if (CODEC_OpCheckBit0Set(swLen))
    {
        aepX1 = *((ae_p16s *)&pshwX[swLen - 1]);
        aepY1 = *((ae_p16s *)&pshwY[swLen - 1]);

        aeqL1 = AE_MULFS32P16S_LL(aepX1, aepY1);
        aepZ1 = AE_TRUNCP24Q48(aeqL1);

        aepZ1 = AE_ROUNDSP16ASYM(aepZ1);

        *((ae_p16s *)&pshwZ[swLen - 1]) = aepZ1;
    }
}
XT_INLINE void CODEC_OpVvMultRD(
        Word16 *pshwX,
        Word16 *pshwY,
        Word32  swLen,
        Word16       *pshwZ)
{
    Word32 k = 0;
    ae_p24x2s aepX1, aepY1, aepZ1, aepX2, aepY2, aepZ2;
    ae_q56s aeqL1, aeqH1, aeqL2, aeqH2;

    if(swLen < 0)
    {
        return;
    }

    if (CODEC_OpCheck4ByteAligned((Word32)pshwX|(Word32)(pshwY-1)|(Word32)pshwZ))
    {
        for (k = 0; k < swLen>>2; k++)
        {
            aepX1 = *((ae_p16x2s *)&pshwX[4*k]);
            aepX2 = *((ae_p16x2s *)&pshwX[4*k + 2]);

            aepY1 = *((ae_p16x2s *)&pshwY[- 4*k - 1]);
            aepY2 = *((ae_p16x2s *)&pshwY[- 4*k - 3]);

            aeqH1 = AE_MULFS32P16S_HL(aepX1, aepY1);
            aeqH2 = AE_MULFS32P16S_HL(aepX2, aepY2);
            aeqL1 = AE_MULFS32P16S_LH(aepX1, aepY1);
            aeqL2 = AE_MULFS32P16S_LH(aepX2, aepY2);

            aepZ1 = AE_TRUNCP24Q48X2(aeqH1, aeqL1);
            aepZ2 = AE_TRUNCP24Q48X2(aeqH2, aeqL2);

            aepZ1 = AE_ROUNDSP16ASYM(aepZ1);
            aepZ2 = AE_ROUNDSP16ASYM(aepZ2);

            *((ae_p16x2s *)&pshwZ[4*k]) = aepZ1;
            *((ae_p16x2s *)&pshwZ[4*k + 2]) = aepZ2;
        }

        /* 1组并行计算 */
        if (CODEC_OpCheckBit1Set(swLen))
        {
            k = swLen & 0x7FFFFFFC;

            aepX1 = *((ae_p16x2s *)&pshwX[k]);
            aepY1 = *((ae_p16x2s *)&pshwY[- k - 1]);

            aeqH1 = AE_MULFS32P16S_HH(aepX1, aepY1);
            aeqL1 = AE_MULFS32P16S_LL(aepX1, aepY1);

            aepZ1 = AE_TRUNCP24Q48X2(aeqH1, aeqL1);

            aepZ1 = AE_ROUNDSP16ASYM(aepZ1);

            *((ae_p16x2s *)&pshwZ[k]) = aepZ1;
        }
    }
    else
    {
        /* 向量地址不为4字节对齐时采用单操作 */
        for (k = 0; k < swLen>>1; k++)
        {
            aepX1 = *((ae_p16s *)&pshwX[2*k]);
            aepX2 = *((ae_p16s *)&pshwX[2*k + 1]);

            aepY1 = *((ae_p16s *)&pshwY[- 2*k]);
            aepY2 = *((ae_p16s *)&pshwY[- 2*k - 1]);

            aeqL1 = AE_MULFS32P16S_LL(aepX1, aepY1);
            aeqL2 = AE_MULFS32P16S_LL(aepX2, aepY2);
            aepZ1 = AE_TRUNCP24Q48(aeqL1);
            aepZ2 = AE_TRUNCP24Q48(aeqL2);

            aepZ1 = AE_ROUNDSP16ASYM(aepZ1);
            aepZ2 = AE_ROUNDSP16ASYM(aepZ2);

            *((ae_p16s *)&pshwZ[2*k]) = aepZ1;
            *((ae_p16s *)&pshwZ[2*k + 1]) = aepZ2;
        }
    }

    /* 向量长度为奇数时，对最后一个元素单操作 */
    if (CODEC_OpCheckBit0Set(swLen))
    {
        aepX1 = *((ae_p16s *)&pshwX[swLen - 1]);
        aepY1 = *((ae_p16s *)&pshwY[1 - swLen]);

        aeqL1 = AE_MULFS32P16S_LL(aepX1, aepY1);
        aepZ1 = AE_TRUNCP24Q48(aeqL1);

        aepZ1 = AE_ROUNDSP16ASYM(aepZ1);

        *((ae_p16s *)&pshwZ[swLen - 1]) = aepZ1;
    }
}

/*****************************************************************************
 函 数 名  : CODEC_OpVvSub
 功能描述  : Z[i] = sub(X[i], Y[i])
 输入参数  : Word16 *pshwX    - 被减数向量
             Word16 *pshwY    - 减数向量
             Word32 swLen     - 向量长度
             Word16 *pshwZ    - 保存结果的向量
 输出参数  : Word16 *pshwZ    - 计算结果
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月20日
    作    者   : 金剑
    修改内容   : 新生成函数
  2.日    期   : 2011年9月1日
    作    者   : 邵聃
    修改内容   : 提升运算效率

*****************************************************************************/
XT_INLINE void CODEC_OpVvSub(
                const Word16 *pshwX,
                const Word16 *pshwY,
                const Word32  swLen,
                Word16       *pshwZ)
{
    Word32 k = 0;
    ae_p24x2s aepX1, aepY1, aepZ1, aepX2, aepY2, aepZ2;

    if(swLen < 0)
    {
        return;
    }

    if (CODEC_OpCheck4ByteAligned((Word32)pshwX|(Word32)pshwY|(Word32)pshwZ))
    {
        /* 向量地址为4字节对齐时采用双操作 */
        for (k = 0; k < swLen>>2; k++)
        {
            aepX1 = *((ae_p16x2s *)&pshwX[4*k]);
            aepX2 = *((ae_p16x2s *)&pshwX[4*k + 2]);

            aepY1 = *((ae_p16x2s *)&pshwY[4*k]);
            aepY2 = *((ae_p16x2s *)&pshwY[4*k + 2]);

            aepZ1 = AE_SUBSP24S(aepX1, aepY1);
            aepZ2 = AE_SUBSP24S(aepX2, aepY2);

            *((ae_p16x2s *)&pshwZ[4*k]) = aepZ1;
            *((ae_p16x2s *)&pshwZ[4*k + 2]) = aepZ2;
        }

        /* 1组并行计算 */
        if (CODEC_OpCheckBit1Set(swLen))
        {
            k = swLen & 0x7FFFFFFC;

            aepX1 = *((ae_p16x2s *)&pshwX[k]);
            aepY1 = *((ae_p16x2s *)&pshwY[k]);

            aepZ1 = AE_SUBSP24S(aepX1, aepY1);

            *((ae_p16x2s *)&pshwZ[k]) = aepZ1;
        }
    }
    else
    {
        /* 向量地址不为4字节对齐时采用单操作 */
        for (k = 0; k < swLen>>1; k++)
        {
            aepX1 = *((ae_p16s *)&pshwX[2*k]);
            aepX2 = *((ae_p16s *)&pshwX[2*k + 1]);

            aepY1 = *((ae_p16s *)&pshwY[2*k]);
            aepY2 = *((ae_p16s *)&pshwY[2*k + 1]);

            aepZ1 = AE_SUBSP24S(aepX1, aepY1);
            aepZ2 = AE_SUBSP24S(aepX2, aepY2);

            *((ae_p16s *)&pshwZ[2*k]) = aepZ1;
            *((ae_p16s *)&pshwZ[2*k + 1]) = aepZ2;
        }
    }

    /* 向量长度为奇数时，对最后一个元素单操作 */
    if (CODEC_OpCheckBit0Set(swLen))
    {
        aepX1 = *((ae_p16s *)&pshwX[swLen - 1]);
        aepY1 = *((ae_p16s *)&pshwY[swLen - 1]);

        aepZ1 = AE_SUBSP24S(aepX1, aepY1);

        *((ae_p16s *)&pshwZ[swLen - 1]) = aepZ1;
    }
}



XT_INLINE Word16 CODEC_OpVecMeanAbsAlignedQuan(Word16 *pshwX, Word32 swLen)
{
    Word32          swSumVal = 0;                                               /* Q0 */
    Word32          swCnt;
    Word16          shwAverEng;

    Word32 k = 0;
    ae_p24x2s aepX1, aepX2, aep1;
    ae_q56s   aeqSum, aeqX1, aeqX2, aeqX3, aeqX4;

    aeqSum = AE_ZEROQ56();
    aep1   = AE_CVTP24A16(1);

    for (k = 0; k < swLen>>2; k++)
    {
        aepX1 = *((ae_p16x2s *)&pshwX[4*k]);
        aepX2 = *((ae_p16x2s *)&pshwX[4*k + 2]);

        aepX1 = AE_ABSSP24S(aepX1);
        aepX2 = AE_ABSSP24S(aepX2);

        AE_MULAAP24S_HH_LL(aeqSum, aepX1, aep1);
        AE_MULAAP24S_HH_LL(aeqSum, aepX2, aep1);

        aeqSum = AE_SATQ48S(aeqSum);
    }

    swSumVal = AE_TRUNCA32Q48(aeqSum);

    /* 求平均值 */
    shwAverEng = saturate(swSumVal / swLen);

    return shwAverEng;
}


XT_INLINE void CODEC_OpVecShr_r32(
                Word32        *pswX,
                Word32         swLen,
                Word16         shwBits,
                Word32        *pswY)
{
    Word32 k = 0;

    ae_q56s aeqX1, aeqX2;

    if(shwBits > 0)
    {
        /* 限制位移数 */
        shwBits = shwBits > 32 ? 32 : shwBits;

        for (k = 0; k < swLen>>1; k++)
        {
            aeqX1 = *((ae_q32s *)&pswX[2*k]);
            aeqX2 = *((ae_q32s *)&pswX[2*k + 1]);

            aeqX1 = AE_SRAAQ56(aeqX1, shwBits);
            aeqX1 = AE_ROUNDSQ32ASYM(aeqX1);

            aeqX2 = AE_SRAAQ56(aeqX2, shwBits);
            aeqX2 = AE_ROUNDSQ32ASYM(aeqX2);

            *((ae_q32s *)&pswY[2*k])        = aeqX1;
            *((ae_q32s *)&pswY[2*k + 1])    = aeqX2;
        }
    }
    else
    {
        /* 限制位移数 */
        shwBits = shwBits < -32 ? -32 : shwBits;

        for (k = 0; k < swLen>>1; k++)
        {
            aeqX1 = *((ae_q32s *)&pswX[2*k]);
            aeqX2 = *((ae_q32s *)&pswX[2*k + 1]);

            aeqX1 = AE_SLLASQ56S(aeqX1, -shwBits);
            aeqX1 = AE_SATQ48S(aeqX1);

            aeqX2 = AE_SLLASQ56S(aeqX2, -shwBits);
            aeqX2 = AE_SATQ48S(aeqX2);

            *((ae_q32s *)&pswY[2*k])        = aeqX1;
            *((ae_q32s *)&pswY[2*k + 1])    = aeqX2;
        }
    }

    /* 向量长度为奇数时，对最后一个元素单操作 */
    if (CODEC_OpCheckBit0Set(swLen))
    {
        aeqX1 = *((ae_q32s *)&pswX[swLen - 1]);

        if(shwBits > 0)
        {
            aeqX1 = AE_SRAAQ56(aeqX1, shwBits);
            aeqX1 = AE_ROUNDSQ32ASYM(aeqX1);
        }
        else
        {
            aeqX1 = AE_SLLASQ56S(aeqX1, -shwBits);
            aeqX1 = AE_SATQ48S(aeqX1);
        }

        *((ae_q32s *)&pswY[swLen - 1]) = aeqX1;
    }
}

#endif /* ifndef _MED_C89_ */


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of med_com_vec_hifi.h */
