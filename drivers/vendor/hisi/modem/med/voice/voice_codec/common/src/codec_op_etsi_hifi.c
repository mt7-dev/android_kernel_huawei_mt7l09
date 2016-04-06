

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "codec_op_etsi.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifndef _MED_C89_
/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
Flag Carry    = 0;

/*****************************************************************************
  3 函数实现
*****************************************************************************/

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : L_macNs                                                 |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Multiply shwVar1 by shwVar2 and shift the result left by 1. Add the 32  |
 |   bit result to swVar3 without saturation, return a 32 bit result.        |
 |   Generate carry and overflow values :                                    |
 |  L_macNs(swVar3,shwVar1,shwVar2) = L_add_c(swVar3,L_mult(shwVar1,shwVar2))|
 |                                                                           |
 |   Complexity weight : 1                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    swVar3   32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0x8000 0000 <= swVar3 <= 0x7fff ffff.                 |
 |                                                                           |
 |    shwVar1                                                                |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= shwVar1 <= 0x0000 7fff.                |
 |                                                                           |
 |    shwVar2                                                                |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= shwVar1 <= 0x0000 7fff.                |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    swVarOut                                                               |
 |             32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0x8000 0000 <= swVarOut <= 0x7fff ffff.               |
 |                                                                           |
 |   Caution :                                                               |
 |                                                                           |
 |    In some cases the Carry flag has to be cleared or set before using     |
 |    operators which take into account its value.                           |
 |___________________________________________________________________________|
*/
/* 代码中暂未使用，受限于HIFI不支持进位，使用原始ETSI的C代码实现 */
Word32 L_macNs(Word32 swVar3, Word16 shwVar1, Word16 shwVar2)
{
    Word32 swVarOut;

    swVarOut = L_mult(shwVar1, shwVar2);

    swVarOut = L_add_c(swVar3, swVarOut);

    return (swVarOut);
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : L_msuNs                                                 |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Multiply shwVar1 by shwVar2 and shift the result left by 1.Subtract the |
 |   32 bit result from swVar3 without saturation, return a 32 bit result.   |
 |   Generate carry and overflow values :                                    |
 |   L_msuNs(swVar3,shwVar1,shwVar2)=L_sub_c(swVar3,L_mult(shwVar1,shwVar2)).|
 |                                                                           |
 |   Complexity weight : 1                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    swVar3   32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0x8000 0000 <= swVar3 <= 0x7fff ffff.                 |
 |                                                                           |
 |    shwVar1                                                                |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= shwVar1 <= 0x0000 7fff.                |
 |                                                                           |
 |    shwVar2                                                                |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= shwVar1 <= 0x0000 7fff.                |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    swVarOut                                                               |
 |             32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0x8000 0000 <= swVarOut <= 0x7fff ffff.               |
 |                                                                           |
 |   Caution :                                                               |
 |                                                                           |
 |    In some cases the Carry flag has to be cleared or set before using     |
 |    operators which take into account its value.                           |
 |___________________________________________________________________________|
*/
/* 代码中暂未使用，受限于HIFI不支持进位，使用原始ETSI的C代码实现 */
Word32 L_msuNs(Word32 swVar3, Word16 shwVar1, Word16 shwVar2)
{
    Word32 swVarOut;

    swVarOut = L_mult(shwVar1, shwVar2);

    swVarOut = L_sub_c(swVar3, swVarOut);

    return (swVarOut);
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : L_add_c                                                 |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Performs 32 bits addition of the two 32 bits variables (swVar1+swVar2+C)|
 |   with carry. No saturation. Generate carry and Overflow values. The car- |
 |   ry and overflow values are binary variables which can be tested and as- |
 |   signed values.                                                          |
 |                                                                           |
 |   Complexity weight : 2                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    swVar1   32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0x8000 0000 <= swVar3 <= 0x7fff ffff.                 |
 |                                                                           |
 |    swVar2   32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0x8000 0000 <= swVar3 <= 0x7fff ffff.                 |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    swVarOut                                                               |
 |             32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0x8000 0000 <= swVarOut <= 0x7fff ffff.               |
 |                                                                           |
 |   Caution :                                                               |
 |                                                                           |
 |    In some cases the Carry flag has to be cleared or set before using     |
 |    operators which take into account its value.                           |
 |___________________________________________________________________________|
*/
/* 代码中暂未使用，受限于HIFI不支持进位，使用原始ETSI的C代码实现 */
Word32 L_add_c(Word32 swVar1, Word32 swVar2)
{
    Word32   swVarOut;
    Word32   swTest;
    Flag     swCarryT   = 0;
    Flag     swCarryG   = 0;
    Flag     swOverFlow = 0;

    swCarryG = CODEC_OpGetCarry();

    swVarOut = swVar1 + swVar2 + swCarryG;

    swTest   = swVar1 + swVar2;

    if ((swVar1 > 0) && (swVar2 > 0) && (swTest < 0))
    {
        swOverFlow = 1;
        swCarryT = 0;
    }
    else
    {
        if ((swVar1 < 0) && (swVar2 < 0))
        {
            if (swTest >= 0)
            {
                swOverFlow = 1;
                swCarryT   = 1;
            }
            else
            {
                swOverFlow = 0;
                swCarryT   = 1;
            }
        }
        else
        {
            if (((swVar1 ^ swVar2) < 0) && (swTest >= 0))
            {
                swOverFlow = 0;
                swCarryT   = 1;
            }
            else
            {
                swOverFlow = 0;
                swCarryT   = 0;
            }
        }
    }

    if (swCarryG)
    {
        if (MAX_32 == swTest)
        {
            swOverFlow = 1;
            swCarryG   = swCarryT;
        }
        else
        {
            if ((Word32)0xFFFFFFFFL == swTest)
            {
                swCarryG = 1;
            }
            else
            {
                swCarryG = swCarryT;
            }
        }
    }
    else
    {
        swCarryG = swCarryT;
    }

    CODEC_OpSetCarry(swCarryG);

    CODEC_OpSetOverflow(swOverFlow);

    return swVarOut;
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : L_sub_c                                                 |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Performs 32 bits subtraction of the two 32 bits variables with carry    |
 |   (borrow) : swVar1-swVar2-C. No saturation. Generate carry and Overflow  |
 |   values. The carry and overflow values are binary variables which can    |
 |   be tested and assigned values.                                          |
 |                                                                           |
 |   Complexity weight : 2                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    swVar1   32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0x8000 0000 <= swVar3 <= 0x7fff ffff.                 |
 |                                                                           |
 |    swVar2   32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0x8000 0000 <= swVar3 <= 0x7fff ffff.                 |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    swVarOut                                                               |
 |             32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0x8000 0000 <= swVarOut <= 0x7fff ffff.               |
 |                                                                           |
 |   Caution :                                                               |
 |                                                                           |
 |    In some cases the Carry flag has to be cleared or set before using     |
 |    operators which take into account its value.                           |
 |___________________________________________________________________________|
*/
/* 代码中暂未使用，受限于HIFI不支持进位，修改原始ETSI的C代码实现 */
Word32 L_sub_c(Word32 swVar1, Word32 swVar2)
{
    Word32   swVarOut;
    Word32   swTest;
    Flag     swCarryT   = 0;
    Flag     swCarryG   = 0;
    Flag     swOverFlow = 0;

    /* 获取进位标志 */
    swCarryG = CODEC_OpGetCarry();

    if (swCarryG)
    {
        swCarryG = 0;
        if (swVar2 != MIN_32)
        {
            swVarOut = L_add_c(swVar1, -swVar2);
        }
        else
        {
            swVarOut = swVar1 - swVar2;
            if (swVar1 > 0L)
            {
                swOverFlow = 1;
                swCarryG   = 0;
            }
        }
    }
    else
    {
        swVarOut = swVar1 - swVar2 - (Word32)0X00000001L;
        swTest   = swVar1 - swVar2;

        if ((swTest < 0) && (swVar1 > 0) && (swVar2 < 0))
        {
            swOverFlow = 1;
            swCarryT   = 0;
        }
        else if ((swTest > 0) && (swVar1 < 0) && (swVar2 > 0))
        {
            swOverFlow = 1;
            swCarryT   = 1;
        }
        else if ((swTest > 0) && ((swVar1 ^ swVar2) > 0))
        {
            swOverFlow = 0;
            swCarryT   = 1;
        }
        if (MIN_32 == swTest)
        {
            swOverFlow = 1;
            swCarryG   = swCarryT;
        }
        else
        {
            swCarryG = swCarryT;
        }
    }

    /* 设置进位标志 */
    CODEC_OpSetCarry(swCarryG);

    /* 设置溢出标志 */
    CODEC_OpSetOverflow(swOverFlow);

    return (swVarOut);
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : L_sat                                                   |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |    32 bit swVar1 is set to 2147483647 if an overflow occured or to        |
 |    -2147483648 if an underflow occured on the most recent L_add_c,        |
 |    L_sub_c, L_macNs or L_msuNs operations. The carry and overflow values  |
 |    are binary values which can be tested and assigned values.             |
 |                                                                           |
 |   Complexity weight : 4                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    swVar1                                                                 |
 |             32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0x8000 0000 <= shwVar1 <= 0x7fff ffff.                |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    swVarOut                                                               |
 |             32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0x8000 0000 <= shwVarOut <= 0x7fff ffff.              |
 |___________________________________________________________________________|
*/
Word32 L_sat (Word32 swVar1)
{
    Word32   swVarOut;
    Flag     swOverFlow;
    Flag     swCarry;

    /* 获取溢出标志 */
    swOverFlow = CODEC_OpGetOverflow();

    /* 获取进位标志 */
    swCarry    = CODEC_OpGetCarry();

    swVarOut   = swVar1;

    if (swOverFlow)
    {
        if (swCarry)
        {
            swVarOut = MIN_32;
        }
        else
        {
            swVarOut = MAX_32;
        }

        swCarry = 0;

        /* 设置溢出标志 */
        CODEC_OpSetOverflow(0);

        /* 设置进位标志 */
        CODEC_OpSetCarry(swCarry);
    }

    return (swVarOut);

}

/*****************************************************************************
 *                                                                           *
 *   Function Name : Div_32                                                  *
 *                                                                           *
 *   Purpose :                                                               *
 *             Fractional integer division of two 32 bit numbers.            *
 *             swNum / swDenom.                                              *
 *             swNum and swDenom must be positive and swNum < swDenom.       *
 *             swDenom = shwDenomHi<<16 + shwDenomLo<<1                      *
 *             shwDenomHi is a normalize number.                             *
 *                                                                           *
 *   Inputs :                                                                *
 *                                                                           *
 *    swNum                                                                  *
 *             32 bit long signed integer (Word32) whose value falls in the  *
 *             range : 0x0000 0000 < swNum < swDenom                         *
 *                                                                           *
 *    swDenom = shwDenomHi<<16 + shwDenomLo<<1      (DPF)                    *
 *                                                                           *
 *       shwDenomHi                                                          *
 *             16 bit positive normalized integer whose value falls in the   *
 *             range : 0x4000 < shwHi < 0x7fff                               *
 *       shwDenomLo                                                          *
 *             16 bit positive integer whose value falls in the              *
 *             range : 0 < shwLo < 0x7fff                                    *
 *                                                                           *
 *   Return Value :                                                          *
 *                                                                           *
 *    L_div                                                                  *
 *             32 bit long signed integer (Word32) whose value falls in the  *
 *             range : 0x0000 0000 <= L_div <= 0x7fff ffff.                  *
 *                                                                           *
 *  Algorithm:                                                               *
 *                                                                           *
 *  - find = 1/swDenom.                                                      *
 *      First approximation: approx = 1 / shwDenomHi                         *
 *      1/swDenom = approx * (2.0 - swDenom * approx )                       *
 *                                                                           *
 *  -  result = swNum * (1/swDenom)                                          *
 *****************************************************************************
*/
/* 修改ETSI标准C函数 */
Word32 Div_32 (Word32 swNum, Word16 shwDenomHi, Word16 shwDenomLo)
{
    Word16   shwApprox, shwHi, shwLo;
    Word16   shwNHi, shwNLo;
    Word32   swVar32;

    /* First approximation: 1 / swDenom = 1/shwDenomHi */
    shwApprox = div_s((Word16)0x3fff, shwDenomHi);

    /* 1/swDenom = approx * (2.0 - swDenom * approx) */
    swVar32   = Mpy_32_16(shwDenomHi, shwDenomLo, shwApprox);

    swVar32   = L_sub((Word32)0x7fffffffL, swVar32);

    L_Extract(swVar32, &shwHi, &shwLo);

    swVar32   = Mpy_32_16(shwHi, shwLo, shwApprox);

    /* swNum * (1/swDenom) */
    L_Extract(swVar32, &shwHi, &shwLo);
    L_Extract(swNum, &shwNHi, &shwNLo);

    swVar32   = Mpy_32(shwNHi, shwNLo, shwHi, shwLo);
    swVar32   = L_shl(swVar32, 2);

    return (swVar32);
}



#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
