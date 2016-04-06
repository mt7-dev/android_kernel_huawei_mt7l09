/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : med_com_vec_op.c
  版 本 号   : 初稿
  作    者   : 金剑
  生成日期   : 2011年8月20日
  最近修改   :
  功能描述   : 语音处理向量运算函数标准C实现
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
#include "codec_op_vec.h"
#include "codec_typedefine.h"
#include "codec_op_netsi.h"

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

/* 标准C代码中使用 */
#ifdef _MED_C89_
/*****************************************************************************
 函 数 名  : CODEC_OpVcAnd
 功能描述  : Y[i] = X[i]&Const
 输入参数  : Word16 *pshwX    - 向量
             Word32 swLen     - 向量长度
             Word16 shwConst  - 常数
             Word16 *pshwY    - 保存计算结果的向量
 输出参数  : Word16 *pshwY    - 计算结果
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月20日
    作    者   : 金剑
    修改内容   : 新生成函数

*****************************************************************************/
void CODEC_OpVcAnd(
                const Word16 *pshwX,
                const Word32  swLen,
                const Word16  shwConst,
                Word16 *pshwY)
{
    Word32 i = 0;

    for (i = 0; i < swLen; i++)
    {
        pshwY[i] = pshwX[i] & shwConst;
    }
}

/*****************************************************************************
 函 数 名  : CODEC_OpVcMac
 功能描述  : Sum = L_mac(sum, X[i], Const) = Sum + (X[i]*Const)<<1
 输入参数  : Word16 *pshwX    - 向量
             Word32 swLen     - 向量长度
             Word16 shwConst  - 常数
             Word32 swSum     - 累计和初始值
 输出参数  : 无
 返 回 值  : Word32           - 累加和
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月20日
    作    者   : 金剑
    修改内容   : 新生成函数

*****************************************************************************/
Word32 CODEC_OpVcMac(
                const Word16 *pshwX,
                const Word32  swLen,
                const Word16  shwConst,
                Word32  swSum)
{
    Word32 i = 0;

    for (i = 0; i < swLen; i++)
    {
        swSum = L_mac(swSum, pshwX[i], shwConst);
    }

    return swSum;

}

/*****************************************************************************
 函 数 名  : CODEC_OpVcMultQ15Add
 功能描述  : Z[i] = add(X[i], mult(Y[i],const)) = X[i] + (Y[i]*Const)>>15
 输入参数  : Word16 *pshwX      - 与积相加的向量
             Word16 *pshwY      - 与常数相乘的向量
             Word32  swLen      - 向量长度
             Word16  shwConst   - 常数
             Word16 *pshwZ      - 保存结果的向量
 输出参数  : Word16 *pshwZ      - 计算结果
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月20日
    作    者   : 金剑
    修改内容   : 新生成函数

*****************************************************************************/
void CODEC_OpVcMultQ15Add(
                const Word16 *pshwX,
                const Word16 *pshwY,
                const Word32  swLen,
                const Word16  shwConst,
                Word16       *pshwZ)
{
    Word32 i = 0;

    for (i = 0; i < swLen; i++)
    {
        pshwZ[i] = add(pshwX[i], mult(pshwY[i], shwConst));
    }
}

/*****************************************************************************
 函 数 名  : CODEC_OpVcMultScale
 功能描述  : Y[i] = L_mult(X[i], const) >> ScaleBIt， ScaleBIt>=0
 输入参数  : Word16 *pshwX         - 输入向量
             Word32  swLen         - 向量长度
             Word16  shwConst      - 常数
             Word16  shwScaleBit   - 右移bit数
             Word16 *pshwY         - 保存结果的向量
 输出参数  : Word16 *pshwY         - 计算结果
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月20日
    作    者   : 金剑
    修改内容   : 新生成函数

*****************************************************************************/
void CODEC_OpVcMultScale(
                const Word16 *pshwX,
                const Word32  swLen,
                const Word16  shwConst,
                Word16        shwScaleBit,
                Word16       *pshwY)
{
    Word32 i = 0;

    for (i = 0; i < swLen; i++)
    {
        pshwY[i] = saturate(L_shr(L_mult(pshwX[i], shwConst), shwScaleBit));
    }
}

/*****************************************************************************
函 数 名  : CODEC_OpVcMult
功能描述  : Z[i] = mult(X[i], k)
输入参数  : Word16 *pshwX      - 被乘数向量
            Word32  swLen      - 向量长度
            Word16  shwConst   - 乘数常量
            Word16 *pshwZ      - 保存结果的向量
输出参数  : Word16 *pshwZ      - 计算结果
返 回 值  : void
调用函数  :
被调函数  :

修改历史      :
1.日    期   : 2011年8月20日
作    者   : 金剑
修改内容   : 新生成函数

*****************************************************************************/
void CODEC_OpVcMult(
                const Word16 *pshwX,
                const Word32  swLen,
                const Word16  shwConst,
                Word16       *pshwZ)
{
    Word32 i = 0;

    for (i = 0; i < swLen; i++)
    {
        pshwZ[i] = mult(pshwX[i], shwConst);
    }
}

/*****************************************************************************
 函 数 名  : CODEC_OpVcMultR
 功能描述  : Z[i] = round(mult(X[i], k))
 输入参数  : Word16 *pshwX      - 被乘数向量
             Word32  swLen      - 向量长度
             Word16  shwConst   - 乘数常量
             Word16 *pshwZ      - 保存结果的向量
 输出参数  : Word16 *pshwZ      - 计算结果
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月20日
    作    者   : 金剑
    修改内容   : 新生成函数

*****************************************************************************/
void CODEC_OpVcMultR(
                const Word16 *pshwX,
                const Word32  swLen,
                const Word16  shwConst,
                Word16       *pshwZ)
{
    Word32 i = 0;

    for (i = 0; i < swLen; i++)
    {
        pshwZ[i] = mult_r(pshwX[i], shwConst);
    }
}


VOS_VOID CODEC_OpVcSub(
                const Word16           *pshwX,
                const Word32            swLen,
                const Word16            shwY,
                Word16                 *pshwZ)
{
    Word32      swCnt = 0;

    for (swCnt = 0; swCnt < swLen; swCnt++)
    {
        pshwZ[swCnt] = sub(pshwX[swCnt], shwY);
    }
}


VOS_VOID CODEC_OpVecAbs(
                      Word16           *pshwX,
                      Word32            swLen,
                      Word16           *pshwY)
{
    Word32      swCnt;

    for (swCnt = 0; swCnt < swLen; swCnt++)
    {
        pshwY[swCnt] = abs_s(pshwX[swCnt]);
    }
}

/*****************************************************************************
 函 数 名  : CODEC_OpVecCpy
 功能描述  : Des[i] =  Src[i]，向量复制，Src与Des向量内存可重复
 输入参数  : Word16 *pshwDes    - 目标向量
             Word16 *pshwSrc    - 源向量
             Word32 swLen       - 向量长度
 输出参数  : Word16 *pshwDes    - 复制结果
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月20日
    作    者   : 金剑
    修改内容   : 新生成函数

*****************************************************************************/
void CODEC_OpVecCpy(
                Word16       *pshwDes,
                const Word16 *pshwSrc,
                const Word32  swLen)
{
    Word32 i = 0;

    for (i = 0; i < swLen; i++)
    {
        pshwDes[i] = pshwSrc[i];
    }
}

/*****************************************************************************
 函 数 名  : CODEC_OpVecMax
 功能描述  : 寻找向量元素最大值
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
Word16 CODEC_OpVecMax(
                const Word16 *pshwX,
                const Word32  swLen,
                Word16       *pswMaxPos)
{
    Word32 i        = 0;
    Word16 shwMax   = 0;
    Word16 shwMaxId = 0;

    shwMax = pshwX[0];

    for (i = 1; i < swLen; i++)
    {
        if (shwMax <= pshwX[i])
        {
            shwMax   = pshwX[i];
            shwMaxId = (Word16)i;
        }
    }

    if (pswMaxPos != 0)
    {
        *pswMaxPos = shwMaxId;
    }

    return shwMax;

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
Word16 CODEC_OpVecMaxAbs(
                const Word16 *pshwX,
                const Word32  swLen,
                Word16       *pswMaxPos)
{
    Word32 i        = 0;
    Word16 shwMax   = 0;
    Word16 shwMaxId = 0;
    Word16 shwTemp  = 0;

    shwMax = abs_s(pshwX[0]);

    for (i = 1; i < swLen; i++)
    {
        shwTemp = abs_s(pshwX[i]);

        if (shwMax <= shwTemp)
        {
            shwMax   = shwTemp;
            shwMaxId = (Word16)i;
        }
    }

    if (pswMaxPos != 0)
    {
        *pswMaxPos = shwMaxId;
    }

    return shwMax;

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
Word16 CODEC_OpVecMin(
                const Word16 *pshwX,
                const Word32 swLen,
                Word16      *pswMinPos)
{
    Word32 i        = 0;
    Word16 shwMin   = 0;
    Word16 shwMinId = 0;

    shwMin = pshwX[0];

    for (i = 1; i < swLen; i++)
    {
        if (shwMin >= pshwX[i])
        {
            shwMin   = pshwX[i];
            shwMinId = (Word16)i;
        }
    }

    if (pswMinPos != 0)
    {
        *pswMinPos = shwMinId;
    }

    return shwMin;

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

*****************************************************************************/
void CODEC_OpVecSet(
                Word16       *pshwDes,
                const Word32  swLen,
                const Word16  shwValue)
{
    Word32 i = 0;

    for (i = 0; i < swLen; i++)
    {
        pshwDes[i] = shwValue;
    }
}

/*****************************************************************************
 函 数 名  : CODEC_OpVecShl
 功能描述  : Y[i] = shl(X[i], Bits)
 输入参数  : Word16 *pshwX     - 待移位向量
             Word32 swLen      - 向量长度
             Word16 shwBits    - 移位数
             Word16 *pshwY     - 保存结果的向量
 输出参数  : Word16 *pshwY     - 移位结果
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月20日
    作    者   : 金剑
    修改内容   : 新生成函数

*****************************************************************************/
void CODEC_OpVecShl(
                const Word16 *pshwX,
                const Word32  swLen,
                Word16        shwBits,
                Word16       *pshwY)
{
    Word32 i = 0;

    for (i = 0; i < swLen; i++)
    {
        pshwY[i] = shl(pshwX[i], shwBits);
    }
}

/*****************************************************************************
 函 数 名  : CODEC_OpVecShr
 功能描述  : Y[i] = shr(X[i], Bits)
 输入参数  : Word16 *pshwX      - 待移位向量
             Word32 swLen       - 向量长度
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

*****************************************************************************/
void CODEC_OpVecShr(
                const Word16 *pshwX,
                const Word32  swLen,
                Word16        shwBits,
                Word16       *pshwY)
{
    Word32 i = 0;

    if (shwBits< 0)
    {
        return;
    }

    for (i = 0; i < swLen; i++)
    {
        pshwY[i] = shr(pshwX[i], shwBits);
    }
}


VOS_VOID CODEC_OpVecShr_r(
                Word16 *pshwX,
                Word32  swLen,
                Word16  shwBits,
                Word16 *pshwY)
{
    Word32      swCnt = 0;

    if (shwBits< 0)
    {
        return;
    }

    for (swCnt = 0; swCnt < swLen; swCnt++)
    {
        pshwY[swCnt] = shr_r(pshwX[swCnt], shwBits);
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

*****************************************************************************/
Word16 CODEC_OpVecSum(
                const Word16 *pshwX,
                const Word32  swLen)
{
    Word32 i      = 0;
    Word16 shwSum = 0;

    for (i = 0; i < swLen; i++)
    {
        shwSum = add(shwSum, pshwX[i]);
    }

    return shwSum;

}

/*****************************************************************************
 函 数 名  : CODEC_OpVecScaleMac
 功能描述  : temp = shr(X[i], bits) ， Bits>=0
             sum = L_mac(sum, temp, temp)
 输入参数  : Word16 *pshwX    - 向量
             Word32 swLen     - 向量长度
             Word32 swSum     - 累加和初始值
             Word16 shwBits   - 缩放右移位数
 输出参数  : 无
 返 回 值  : Word32  累加结果
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月20日
    作    者   : 金剑
    修改内容   : 新生成函数

*****************************************************************************/
Word32 CODEC_OpVecScaleMac(
                const Word16 *pshwX,
                const Word32  swLen,
                Word32        swSum,
                Word16        shwBits)
{
    Word32 i       = 0;
    Word16 shwTemp = 0;

    for (i = 0; i < swLen; i++)
    {
        shwTemp = shr(pshwX[i], shwBits);

        swSum   = L_mac(swSum, shwTemp, shwTemp);
    }

    return swSum;

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

*****************************************************************************/
void CODEC_OpVvAdd(
                const Word16 *pshwX,
                const Word16 *pshwY,
                const Word32  swLen,
                Word16       *pshwZ)
{
    Word32 i = 0;

    for (i = 0; i < swLen; i++)
    {
        pshwZ[i] = add(pshwX[i], pshwY[i]);
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

*****************************************************************************/
void CODEC_OpVvAdd32(
                const Word32 *pswX,
                const Word32 *pswY,
                const Word32  swLen,
                Word32       *pswZ)
{
    Word32 i = 0;

    for (i = 0; i < swLen; i++)
    {
        pswZ[i] = L_add(pswX[i], pswY[i]);
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

*****************************************************************************/
void CODEC_OpVvSub(
                const Word16 *pshwX,
                const Word16 *pshwY,
                const Word32  swLen,
                Word16       *pshwZ)
{
    Word32 i = 0;

    for (i = 0; i < swLen; i++)
    {
        pshwZ[i] = sub(pshwX[i], pshwY[i]);
    }
}


void CODEC_OpVvMult(
                const Word16 *pshwX,
                const Word16 *pshwY,
                const Word32  swLen,
                Word16       *pshwZ)
{
    Word32 i = 0;

    for (i = 0; i < swLen; i++)
    {
        pshwZ[i] = mult(pshwX[i], pshwY[i]);
    }
}


void CODEC_OpVvMultR(
                const Word16 *pshwX,
                const Word16 *pshwY,
                const Word32  swLen,
                Word16       *pshwZ)
{
    Word32 i = 0;

    for (i = 0; i < swLen; i++)
    {
        pshwZ[i] = mult_r(pshwX[i], pshwY[i]);
    }
}


VOS_VOID CODEC_OpVvMultRD(
                Word16       *pshwX,
                Word16       *pshwY,
                Word32        swLen,
                Word16       *pshwZ)
{
    Word32      swCnt     = 0;

    for (swCnt = 0; swCnt < swLen; swCnt++)
    {
        pshwZ[swCnt] = mult_r(pshwX[swCnt], pshwY[-swCnt]);
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

*****************************************************************************/
Word32 CODEC_OpVvMac(
                const Word16 *pshwX,
                const Word16 *pshwY,
                const Word32  swLen,
                Word32        swSum)
{
    Word32 i = 0;

    for (i = 0; i < swLen; i++)
    {
        swSum = L_mac(swSum, pshwX[i], pshwY[i]);
    }

    return swSum;

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
  1.日    期   : 2011年8月20日
    作    者   : 金剑
    修改内容   : 新生成函数

*****************************************************************************/
Word32 CODEC_OpVvMacD(
                const Word16 *pshwX,
                const Word16 *pshwY,
                const Word32  swLen,
                Word32        swSum)
{
    Word32 i = 0;

    for (i = 0; i < swLen; i++)
    {
        swSum = L_mac(swSum, pshwX[i], pshwY[-i]);
    }

    return swSum;

}

/*****************************************************************************
 函 数 名  : CODEC_OpVvMsu
 功能描述  :Y = sum - sum(X[i]*Y[i])
 输入参数  : const Word16 *pshwX    - 16bit型输入向量
             const Word16 *pshwY    - 16bit型输入向量
             Word32 swLen           - 输入向量长度
             Word32 swSum           - 累加初始数值
 输出参数  : 无
 返 回 值  : Word32 向量点乘与累加初始数值相减的结果
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月20日
    作    者   : 金剑
    修改内容   : 新生成函数

*****************************************************************************/
Word32 CODEC_OpVvMsu(
                const Word16 *pshwX,
                const Word16 *pshwY,
                const Word32  swLen,
                Word32        swSum)
{
    Word32 i      = 0;

    for (i = 0; i < swLen; i++)
    {
        swSum = L_msu(swSum , pshwX[i], pshwY[i]);
    }

    return swSum;

}

/*****************************************************************************
 函 数 名  : CODEC_OpVvMsuD
 功能描述  :Y = sum - sum(X[i]*Y[-i])
 输入参数  : const Word16 *pshwX    - 16bit型输入向量
             const Word16 *pshwY    - 16bit型输入向量
             Word32 swLen           - 输入向量长度
             Word32 swSum           - 累加初始数值
 输出参数  : 无
 返 回 值  : Word32 向量点乘与累加初始数值相减的结果
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月20日
    作    者   : 金剑
    修改内容   : 新生成函数

*****************************************************************************/
Word32 CODEC_OpVvMsuD(
                const Word16 *pshwX,
                const Word16 *pshwY,
                const Word32  swLen,
                Word32        swSum)
{
    Word32 i      = 0;

    for (i = 0; i < swLen; i++)
    {
        swSum = L_msu(swSum , pshwX[i], pshwY[-i]);
    }

    return swSum;

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
 输出参数  : Word16 *pshwZ     - 计算结果
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月20日
    作    者   : 金剑
    修改内容   : 新生成函数

*****************************************************************************/
void CODEC_OpVvFormWithCoef(
                const Word16 *pshwX,
                const Word16 *pshwY,
                const Word32  swLen,
                const Word16  shwA,
                const Word16  shwB,
                Word16 *pshwZ)
{
    Word32 i = 0;

    for (i = 0; i < swLen; i++)
    {
        pshwZ[i] = add(mult(pshwX[i], shwA), mult(pshwY[i], shwB));
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
void CODEC_OpVvFormWithDimi(
                Word16 *pshwX,
                Word16 *pshwY,
                Word32  swLen,
                Word16 *pshwZ)
{
    Word32 i = 0;

    for (i = 0; i < swLen; i++)
    {
        pshwZ[i] = add(shr(pshwX[i], 1), shr(pshwY[i], 1));
    }
}

/*****************************************************************************
 函 数 名  : CODEC_OpVvFormWithQuar
 功能描述  : Z[i] = add(X[i]>>2, sub(Y[i],Y[i]>>2))
 输入参数  : Word16 *pshwX   - 加权系数为0.25的向量
             Word16 *pshwY   - 加权系数为0.75的向量
             Word32  swLen   - 向量长度
             Word16 *pshwZ   - 保存结果的向量
 输出参数  : Word16 *pshwZ  - 计算结果
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月20日
    作    者   : 金剑
    修改内容   : 新生成函数

*****************************************************************************/
void CODEC_OpVvFormWithQuar(
                Word16 *pshwX,
                Word16 *pshwY,
                Word32  swLen,
                Word16 *pshwZ)
{
    Word32 i        = 0;

    Word16 shwTempX = 0;
    Word16 shwTempY = 0;

    for (i = 0; i < swLen; i++)
    {
        shwTempX = shr(pshwX[i], 2);

        shwTempY = sub(pshwY[i], shr(pshwY[i], 2));

        pshwZ[i] = add(shwTempX, shwTempY);
    }
}


VOS_VOID CODEC_OpVecShr_r32(
                Word32        *pswX,
                Word32         swLen,
                Word16         shwBits,
                Word32        *pswY)
{
    Word32      swCnt = 0;

    for (swCnt = 0; swCnt < swLen; swCnt++)
    {
        pswY[swCnt] = L_shr_r(pswX[swCnt], shwBits);
    }
}

#endif /* _MED_C89_ */

Word16 CODEC_OpVecMeanAbs(Word16 *pshwArr, Word32 swArrLen)
{
    Word32          swSumVal = 0;                                               /* Q0 */
    Word32          swCnt;
    Word16          shwAverEng;
    Word16          ashwTempArr[320];                                           /* 最大支持的swArrLen为320，可根据需要修改 */

    CODEC_OpVecAbs(pshwArr, swArrLen, ashwTempArr);

    for (swCnt = 0; swCnt < swArrLen; swCnt++)
    {
        swSumVal = L_add(swSumVal, (Word32)(ashwTempArr[swCnt]));
    }

    /* 求平均值 */
    shwAverEng = saturate(swSumVal / swArrLen);

    return shwAverEng;
}
Word32 CODEC_OpVecMax32(Word32 *pswArr, Word32 swLen)
{
    Word32          swCnt;
    Word32          swMax = pswArr[0];

    for (swCnt = 1; swCnt < swLen; swCnt++)
    {
        if (swMax < pswArr[swCnt])
        {
            swMax = pswArr[swCnt];
        }
    }

    return swMax;
}


Word32 CODEC_OpVecMin32(Word32 *pswArr, Word32 swLen)
{
    Word32          swCnt;
    Word32          swMin = pswArr[0];

    for (swCnt = 1; swCnt < swLen; swCnt++)
    {
        if (swMin > pswArr[swCnt])
        {
            swMin = pswArr[swCnt];
        }
    }

    return swMin;
}


Word32 CODEC_OpVecMean32(Word32 *pswArr, Word32 swArrLen)
{
    Word32          swSumVal = 0;                                               /* Q0 */
    Word32          swCnt;
    Word32          swAverEng;

    for (swCnt = 0; swCnt < swArrLen; swCnt++)
    {
        swSumVal = L_add(swSumVal, pswArr[swCnt]);
    }

    /* 求平均值 */
    if (swSumVal >= 0)
    {
        swAverEng = swSumVal / swArrLen;
    }
    else
    {
        swAverEng = L_negate(swSumVal) / swArrLen;
        swAverEng = L_negate(swAverEng);
    }

    return swAverEng;
}


VOS_VOID CODEC_OpVecShr32(
                Word32 *pswX,
                Word32  swLen,
                Word16  shwBits,
                Word32 *pswY)
{
    Word32      swCnt = 0;

    for (swCnt = 0; swCnt < swLen; swCnt++)
    {
        pswY[swCnt] = L_shr(pswX[swCnt], shwBits);
    }
}


Word32 CODEC_OpVecSum32(Word32 *pswArray, Word32 swLen)
{
    Word32          swSum = 0;
    Word32          swCnt;

    for (swCnt=0; swCnt < swLen; swCnt++)
    {
        swSum = L_add(swSum, pswArray[swCnt]);
    }

    return swSum;
}



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

