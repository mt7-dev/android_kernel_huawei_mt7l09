
#ifndef __CODEC_COM_ETSI_HIFI_H__
#define __CODEC_COM_ETSI_HIFI_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#ifndef _MED_C89_
#include <xtensa/tie/xt_hifi2.h>
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 数据类型定义
*****************************************************************************/

/* DSP平台定义 */
#ifndef _MED_C89_

typedef char                            Char;
typedef signed char                     Word8;
typedef unsigned char                   UWord8;
typedef short                           Word16;
typedef unsigned short                  UWord16;
typedef long                            Word32;
typedef unsigned long                   UWord32;
typedef int                             Flag;

#endif


/*****************************************************************************
  3 宏定义
*****************************************************************************/
#ifndef _MED_C89_

#define XT_INLINE                       static inline

/* DSP平台定义 */
#define CODEC_OpSetOverflow(swFlag)       WUR_AE_OVERFLOW(swFlag)
#define CODEC_OpGetOverflow()             RUR_AE_OVERFLOW()
#define CODEC_OpSetCarry(swFlag)          (Carry = swFlag)
#define CODEC_OpGetCarry()                (Carry)

#endif

/*****************************************************************************
  4 全局变量声明
*****************************************************************************/
#ifndef _MED_C89_

/* DSP平台定义 */
extern Flag Carry;

#endif

/*****************************************************************************
  5 内联函数实现
*****************************************************************************/

#ifndef _MED_C89_

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : saturate                                                |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |    Limit the 32 bit input to the range of a 16 bit word.                  |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    swVar1                                                                 |
 |             32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0x8000 0000 <= swVar1 <= 0x7fff ffff.                 |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    shwVarOut                                                              |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= shwVarOut <= 0x0000 7fff.              |
 |___________________________________________________________________________|
*/

XT_INLINE Word16 saturate(Word32 swVar1)
{
    ae_p24x2s  p1;
    ae_q56s    q1;

    /* L_var1（1.32） --> q1(17.48) */
    q1 = AE_CVTQ48A32S(swVar1);

    /* q1带饱和左移16位 --> q1 */
    q1 = AE_SLLISQ56S(q1, 16);

    /* q0做48bit饱和运算 --> q1 */
    q1 = AE_SATQ48S(q1);

    /* q1(25.48) --> p1(1.24)*/
    p1 = AE_TRUNCP24Q48(q1);

    /* p1L(9.24) --> shwVarOut(1.16) */
    return AE_TRUNCA16P24S_L(p1);

}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : add                                                     |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |    Performs the addition (var1+var2) with overflow control and saturation;|
 |    the 16 bit result is set at +32767 when overflow occurs or at -32768   |
 |    when underflow occurs.                                                 |
 |                                                                           |
 |   Complexity weight : 1                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |    var2                                                                   |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    var_out                                                                |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                |
 |___________________________________________________________________________|
*/

XT_INLINE Word16 add(Word16 shwVar1, Word16 shwVar2)
{
    ae_p24x2s  p1, p2;

    /* var(1.16) --> p(9.24) */
    p1 = AE_CVTP24A16(shwVar1);
    p2 = AE_CVTP24A16(shwVar2);

    /* p1 + p2 --> p3 24bit饱和加 */
    p1 = AE_ADDSP24S(p1, p2);

    /* p3L(9.24) --> shwVarOut(1.16) */
    return AE_TRUNCA16P24S_L(p1);
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : sub                                                     |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |    Performs the subtraction (var1+var2) with overflow control and satu-   |
 |    ration; the 16 bit result is set at +32767 when overflow occurs or at  |
 |    -32768 when underflow occurs.                                          |
 |                                                                           |
 |   Complexity weight : 1                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |    var2                                                                   |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    var_out                                                                |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                |
 |___________________________________________________________________________|
*/

XT_INLINE Word16 sub(Word16 shwVar1, Word16 shwVar2)
{
    ae_p24x2s  p1, p2;

    /* var(1.16) --> p(9.24) */
    p1 = AE_CVTP24A16(shwVar1);
    p2 = AE_CVTP24A16(shwVar2);

    /* p1 - p2 --> p3 24bit饱和减 */
    p1 = AE_SUBSP24S(p1, p2);

    /* p3L(9.24) --> shwVarOut(1.16) */
    return  AE_TRUNCA16P24S_L(p1);
}


/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : abs_s                                                   |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |    Absolute value of var1; abs_s(-32768) = 32767.                         |
 |                                                                           |
 |   Complexity weight : 1                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    var_out                                                                |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0x0000 0000 <= var_out <= 0x0000 7fff.                |
 |___________________________________________________________________________|
*/

XT_INLINE Word16 abs_s(Word16 shwVar1)
{
    ae_p24x2s  p1;

    /* shwVar1(1.16) --> p1(9.24) */
    p1 = AE_CVTP24A16(shwVar1);

    /* p1做24bit带饱和绝对值运算 --> p1 */
    p1 = AE_ABSSP24S(p1);

    return  AE_TRUNCA16P24S_L(p1);
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : shl                                                     |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Arithmetically shift the 16 bit input var1 left var2 positions.Zero fill|
 |   the var2 LSB of the result. If var2 is negative, arithmetically shift   |
 |   var1 right by -var2 with sign extension. Saturate the result in case of |
 |   underflows or overflows.                                                |
 |                                                                           |
 |   Complexity weight : 1                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |    var2                                                                   |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    var_out                                                                |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                |
 |___________________________________________________________________________|
*/

XT_INLINE Word16 shl(Word16 shwVar1, Word16 shwVar2)
{
    ae_p24x2s  p1;

    /* shwVar1(1.16) --> p1(9.24) */
    p1 = AE_CVTP24A16(shwVar1);

    if (shwVar2 >= 0)
    {
        /* 限制位移数 */
        shwVar2 = shwVar2 > 16 ? 16 : shwVar2;

        /* 将需要左移比特数写入移位寄存器 */
        WUR_AE_SAR(shwVar2);

        /* p1做24bit带饱和左移 --> p1 */
        p1 = AE_SLLSSP24S(p1);
    }
    else
    {
        /* 限制位移数 */
        shwVar2 = shwVar2 < -16 ? -16 : shwVar2;

        /* 将需要右移比特数写入移位寄存器 */
        WUR_AE_SAR(-shwVar2);

        /* p1做24bit算术右移 --> p1 */
        p1 = AE_SRASP24(p1);
    }

    return AE_TRUNCA16P24S_L(p1);;
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : shr                                                     |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Arithmetically shift the 16 bit input var1 right var2 positions with    |
 |   sign extension. If var2 is negative, arithmetically shift var1 left by  |
 |   -var2 with sign extension. Saturate the result in case of underflows or |
 |   overflows.                                                              |
 |                                                                           |
 |   Complexity weight : 1                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |    var2                                                                   |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    var_out                                                                |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                |
 |___________________________________________________________________________|
*/

XT_INLINE  Word16 shr(Word16 shwVar1, Word16 shwVar2)
{
    ae_p24x2s  p1;

    /* shwVar1(1.16) --> p1(9.24) */
    p1 = AE_CVTP24A16(shwVar1);

    if (shwVar2 >= 0)
    {
        /* 限制位移数 */
        shwVar2 = shwVar2 > 16 ? 16 : shwVar2;

        /* 将需要右移比特数写入移位寄存器 */
        WUR_AE_SAR(shwVar2);

        /* p1做24bit算术右移 --> p1 */
        p1 = AE_SRASP24(p1);
    }
    else
    {
        /* 限制位移数 */
        shwVar2 = shwVar2 < -16 ? -16 : shwVar2;

        /* 将需要左移比特数写入移位寄存器 */
        WUR_AE_SAR(-shwVar2);

        /* p1做24bit带饱和左移 --> p1 */
        p1 = AE_SLLSSP24S(p1);
    }

    return AE_TRUNCA16P24S_L(p1);
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : mult                                                    |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |    Performs the multiplication of var1 by var2 and gives a 16 bit result  |
 |    which is scaled i.e.:                                                  |
 |             mult(var1,var2) = extract_l(L_shr((var1 times var2),15)) and  |
 |             mult(-32768,-32768) = 32767.                                  |
 |                                                                           |
 |   Complexity weight : 1                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |    var2                                                                   |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    var_out                                                                |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                |
 |___________________________________________________________________________|
*/

XT_INLINE Word16 mult(Word16 shwVar1, Word16 shwVar2)
{
    ae_p24x2s  p1, p2;
    ae_q56s    q1;

    /* var(1.16) --> p(9.24) */
    p1 = AE_CVTP24A16(shwVar1);
    p2 = AE_CVTP24A16(shwVar2);

    /* p1(1.24)*p2(1.24)*2 --> q1 */
    q1 = AE_MULFS32P16S_LL(p1, p2);

    /* q1(25.48) --> p1(1.24) */
    p1 = AE_TRUNCP24Q48(q1);

    /* p1L(9.24) --> shwVarOut(1.16) */
    return  AE_TRUNCA16P24S_L(p1);
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : L_mult                                                  |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   L_mult is the 32 bit result of the multiplication of var1 times var2    |
 |   with one shift left i.e.:                                               |
 |        L_mult(var1,var2) = L_shl((var1 times var2),1) and                   |
 |        L_mult(-32768,-32768) = 2147483647.                                |
 |                                                                           |
 |   Complexity weight : 1                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |    var2                                                                   |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    L_var_out                                                              |
 |             32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.              |
 |___________________________________________________________________________|
*/

XT_INLINE Word32 L_mult(Word16 shwVar1, Word16 shwVar2)
{
    ae_p24x2s  p1, p2;
    ae_q56s    q1;

    /* var(1.16) --> p(9.24) */
    p1 = AE_CVTP24A16(shwVar1);
    p2 = AE_CVTP24A16(shwVar2);

    /* p1(1.24)*p2(1.24)*2 --> q1 */
    q1 = AE_MULFS32P16S_LL(p1, p2);

    /* q1(17.48) --> shwVarOut */
    return AE_TRUNCA32Q48(q1);
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : negate                                                  |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Negate var1 with saturation, saturate in the case where input is -32768:|
 |                negate(var1) = sub(0,var1).                                |
 |                                                                           |
 |   Complexity weight : 1                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    var_out                                                                |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                |
 |___________________________________________________________________________|
*/

XT_INLINE Word16 negate(Word16 shwVar1)
{
    Flag       swOverFlow;
    ae_p24x2s  p1;

    /* 保存溢出标志 */
    swOverFlow = CODEC_OpGetOverflow();

    /* shwVar1(1.16) --> p1(9.24) */
    p1 = AE_CVTP24A16(shwVar1);

    /* p1做24比特饱和取相反数 --> p2 */
    p1 = AE_NEGSP24S(p1);

    /* 恢复溢出标志 */
    CODEC_OpSetOverflow(swOverFlow);

    return AE_TRUNCA16P24S_L(p1);
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : extract_h                                               |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Return the 16 MSB of L_var1.                                            |
 |                                                                           |
 |   Complexity weight : 1                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_var1                                                                 |
 |             32 bit long signed integer (Word32 ) whose value falls in the |
 |             range : 0x8000 0000 <= L_var1 <= 0x7fff ffff.                 |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    var_out                                                                |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                |
 |___________________________________________________________________________|
*/

XT_INLINE Word16 extract_h(Word32 swVar1)
{
    ae_p24x2s  p1;

    /* swVar1(8.32) --> p1(1.24) */
    p1 = AE_TRUNCP24A32X2(swVar1, swVar1);

    /* p1L(9.24) --> shwVarOut */
    return AE_TRUNCA16P24S_L(p1);
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : extract_l                                               |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Return the 16 LSB of L_var1.                                            |
 |                                                                           |
 |   Complexity weight : 1                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_var1                                                                 |
 |             32 bit long signed integer (Word32 ) whose value falls in the |
 |             range : 0x8000 0000 <= L_var1 <= 0x7fff ffff.                 |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    var_out                                                                |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                |
 |___________________________________________________________________________|
*/

XT_INLINE Word16 extract_l(Word32 swVar1)
{
    return (Word16)swVar1;
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : round                                                   |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Round the lower 16 bits of the 32 bit input number into the MS 16 bits  |
 |   with saturation. Shift the resulting bits right by 16 and return the 16 |
 |   bit number:                                                             |
 |               round(L_var1) = extract_h(L_add(L_var1,32768))              |
 |                                                                           |
 |   Complexity weight : 1                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_var1                                                                 |
 |             32 bit long signed integer (Word32 ) whose value falls in the |
 |             range : 0x8000 0000 <= L_var1 <= 0x7fff ffff.                 |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    var_out                                                                |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                |
 |___________________________________________________________________________|
*/

XT_INLINE Word16 round_etsi(Word32 swVar1)
{
    ae_p24x2s p1;
    ae_q56s   q1;

    /* L_var1(1.32) --> q1(17.48) */
    q1=AE_CVTQ48A32S(swVar1);

    /* 对q1进行32.48比特非对称取整(0.5->1;-0.5->0) --> p1*/
    p1 = AE_ROUNDSP16Q48ASYM(q1);

    /* p1L(9.24) --> shwVarOut */
    return AE_TRUNCA16P24S_L(p1);
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : L_mac                                                   |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Multiply var1 by var2 and shift the result left by 1. Add the 32 bit    |
 |   result to L_var3 with saturation, return a 32 bit result:               |
 |        L_mac(L_var3,var1,var2) = L_add(L_var3,L_mult(var1,var2)).         |
 |                                                                           |
 |   Complexity weight : 1                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_var3   32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |    var2                                                                   |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    L_var_out                                                              |
 |             32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.              |
 |___________________________________________________________________________|
*/

XT_INLINE Word32 L_mac(Word32 swVar3, Word16 shwVar1, Word16 shwVar2)
{
    ae_p24x2s   p1, p2;
    ae_q56s     q1;

    /* var(1.16) --> p(9.24) */
    p1 = AE_CVTP24A16(shwVar1);
    p2 = AE_CVTP24A16(shwVar2);

    /* L_var3（1.32） --> q1(17.48) */
    q1 = AE_CVTQ48A32S(swVar3);

    /* q1+p1(1.24)*p2(1.24)*2 --> q1(1.48) */
    AE_MULAFS32P16S_LL(q1, p1, p2);

    /* q1(17.48) --> swVarOut */
    return AE_TRUNCA32Q48(q1);
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : L_msu                                                   |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Multiply var1 by var2 and shift the result left by 1. Subtract the 32   |
 |   bit result to L_var3 with saturation, return a 32 bit result:           |
 |        L_msu(L_var3,var1,var2) = L_sub(L_var3,L_mult(var1,var2)).         |
 |                                                                           |
 |   Complexity weight : 1                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_var3   32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |    var2                                                                   |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    L_var_out                                                              |
 |             32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.              |
 |___________________________________________________________________________|
*/

XT_INLINE Word32 L_msu(Word32 swVar3, Word16 shwVar1, Word16 shwVar2)
{
    ae_p24x2s  p1, p2;
    ae_q56s    q1;

    /* var(1.16) --> p(9.24) */
    p1 = AE_CVTP24A16(shwVar1);
    p2 = AE_CVTP24A16(shwVar2);

    /* L_var3（1.32） --> q1(17.48) */
    q1 = AE_CVTQ48A32S(swVar3);

    /* q1-p1(1.24)*p2(1.24)*2 --> q1(1.48) */
    AE_MULSFS32P16S_LL(q1, p1, p2);

    /* q1(17.48) --> swVarOut */
    return  AE_TRUNCA32Q48(q1);
}

/* 不采用内联实现 */
extern Word32 L_macNs(Word32 swVar3, Word16 shwVar1, Word16 shwVar2);           /* Mac without sat */
extern Word32 L_msuNs(Word32 swVar3, Word16 shwVar1, Word16 shwVar2);           /* Msu without sat */

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : L_add                                                   |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   32 bits addition of the two 32 bits variables (L_var1+L_var2) with      |
 |   overflow control and saturation; the result is set at +2147483647 when  |
 |   overflow occurs or at -2147483648 when underflow occurs.                |
 |                                                                           |
 |   Complexity weight : 2                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_var1   32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
 |                                                                           |
 |    L_var2   32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    L_var_out                                                              |
 |             32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.              |
 |___________________________________________________________________________|
*/

XT_INLINE Word32 L_add(Word32 swVar1, Word32 swVar2)
{
    ae_q56s  q1, q2;

    /* L_var(1.32) --> q(17.48) */
    q1 = AE_CVTQ48A32S(swVar1);
    q2 = AE_CVTQ48A32S(swVar2);

    /* q1+q2=q3 56比特加法*/
    q1 = AE_ADDQ56(q1, q2);

    /* q3做48比特饱和处理 */
    q1 = AE_SATQ48S(q1);

    return AE_TRUNCA32Q48(q1);
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : L_sub                                                   |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   32 bits subtraction of the two 32 bits variables (L_var1-L_var2) with   |
 |   overflow control and saturation; the result is set at +2147483647 when  |
 |   overflow occurs or at -2147483648 when underflow occurs.                |
 |                                                                           |
 |   Complexity weight : 2                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_var1   32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
 |                                                                           |
 |    L_var2   32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    L_var_out                                                              |
 |             32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.              |
 |___________________________________________________________________________|
*/

XT_INLINE Word32 L_sub(Word32 swVar1, Word32 swVar2)
{
    ae_q56s  q1, q2;

    /* L_var(1.32) --> q(17.48) */
    q1 = AE_CVTQ48A32S(swVar1);
    q2 = AE_CVTQ48A32S(swVar2);

    /* q1-q2=q3 56比特减法*/
    q1 = AE_SUBQ56(q1, q2);

    /* q3做48比特饱和处理 */
    q1 = AE_SATQ48S(q1);

    /* q3(17.48) --> swVarOut */
    return AE_TRUNCA32Q48(q1);
}

/* 不采用内联实现 */
extern Word32 L_add_c(Word32 swVar1, Word32 swVar2);                            /* Long add with c */
extern Word32 L_sub_c(Word32 swVar1, Word32 swVar2);

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : L_negate                                                |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Negate the 32 bit variable L_var1 with saturation; saturate in the case |
 |   where input is -2147483648 (0x8000 0000).                               |
 |                                                                           |
 |   Complexity weight : 2                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_var1   32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    L_var_out                                                              |
 |             32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.              |
 |___________________________________________________________________________|
*/

XT_INLINE Word32 L_negate(Word32 swVar1)
{
    Word16   shwOverFlow;
    ae_q56s  q1;

    /* 保存溢出标志 */
    shwOverFlow = CODEC_OpGetOverflow();

    /* swVar1(1.32) --> q1(17.48) */
    q1 = AE_CVTQ48A32S(swVar1);

    /* q1=-q1 取相反数 */
    q1 = AE_NEGQ56(q1);

    q1 = AE_SATQ48S(q1);

    /* 恢复溢出标志 */
    CODEC_OpSetOverflow(shwOverFlow);

    return AE_TRUNCA32Q48(q1);
}
/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : mult_r                                                  |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Same as mult with rounding, i.e.:                                       |
 |     mult_r(var1,var2) = extract_l(L_shr(((var1 * var2) + 16384),15)) and  |
 |     mult_r(-32768,-32768) = 32767.                                        |
 |                                                                           |
 |   Complexity weight : 2                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |    var2                                                                   |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    var_out                                                                |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                |
 |___________________________________________________________________________|
*/
XT_INLINE Word16 mult_r(Word16 shwVar1, Word16 shwVar2)
{
    ae_p24x2s   p1, p2;
    ae_q56s     q1;

    /* var(1.16) --> p(9.24) */
    p1 = AE_CVTP24A16(shwVar1);
    p2 = AE_CVTP24A16(shwVar2);

    /* p1(1.24)*p2(1.24)*2 --> q1 */
    q1 = AE_MULFS32P16S_LL(p1, p2);

    /* 对q1进行32.48比特非对称取整(0.5->1;-0.5->0) --> p1 */
    p1 = AE_ROUNDSP16Q48ASYM(q1);

    /* p3H(9.24) --> shwVarOut */
    return AE_TRUNCA16P24S_H(p1);
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : L_shl                                                   |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Arithmetically shift the 32 bit input L_var1 left var2 positions. Zero  |
 |   fill the var2 LSB of the result. If var2 is negative, arithmetically    |
 |   shift L_var1 right by -var2 with sign extension. Saturate the result in |
 |   case of underflows or overflows.                                        |
 |                                                                           |
 |   Complexity weight : 2                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_var1   32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
 |                                                                           |
 |    var2                                                                   |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    L_var_out                                                              |
 |             32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.              |
 |___________________________________________________________________________|
*/
XT_INLINE Word32 L_shl(Word32 swVar1, Word16 shwVar2)
{
    ae_q56s  q1;

    /* L_var1（1.32） --> q1(17.48) */
    q1 = AE_CVTQ48A32S(swVar1);

    if (shwVar2 >= 0)
    {
        /* 限制位移数 */
        shwVar2 = shwVar2 > 32 ? 32 : shwVar2;

        /* q1做56bit带饱和左移 --> q2 */
        q1 = AE_SLLASQ56S(q1, shwVar2);

        /* q2做48比特饱和处理 */
        q1 = AE_SATQ48S(q1);
    }
    else
    {
        /* 限制位移数 */
        shwVar2 = shwVar2 < -32 ? -32 : shwVar2;

        /* q1做56bit算术右移 --> q2 */
        q1 = AE_SRAAQ56(q1, -shwVar2);
    }

    return AE_TRUNCA32Q48(q1);
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : L_shr                                                   |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Arithmetically shift the 32 bit input L_var1 right var2 positions with  |
 |   sign extension. If var2 is negative, arithmetically shift L_var1 left   |
 |   by -var2 and zero fill the -var2 LSB of the result. Saturate the result |
 |   in case of underflows or overflows.                                     |
 |                                                                           |
 |   Complexity weight : 2                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_var1   32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
 |                                                                           |
 |    var2                                                                   |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    L_var_out                                                              |
 |             32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.              |
 |___________________________________________________________________________|
*/
XT_INLINE Word32 L_shr(Word32 swVar1, Word16 shwVar2)
{
    ae_q56s  q1;
    /* L_var1（1.32） --> q1(17.48) */
    q1 = AE_CVTQ48A32S(swVar1);

    if (shwVar2 >= 0)
    {
        /* 限制位移数 */
        shwVar2 = shwVar2 > 32 ? 32 : shwVar2;

        /* q1做56bit算术右移 --> q2 */
        q1 = AE_SRAAQ56(q1, shwVar2);
    }
    else
    {
        /* 限制位移数 */
        shwVar2 = shwVar2 < -32 ? -32 : shwVar2;

        /* q1做56bit带饱和左移 --> q2 */
        q1 = AE_SLLASQ56S(q1, -shwVar2);

        /* q1做48比特饱和处理 */
        q1 = AE_SATQ48S(q1);
    }

    /* q2(17.48) --> swVarOut */
    return AE_TRUNCA32Q48(q1);
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : shr_r                                                   |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Same as shr(var1,var2) but with rounding. Saturate the result in case of|
 |   underflows or overflows :                                               |
 |    - If var2 is greater than zero :                                       |
 |          if (sub(shl(shr(var1,var2),1),shr(var1,sub(var2,1))))            |
 |          is equal to zero                                                 |
 |                     then                                                  |
 |                     shr_r(var1,var2) = shr(var1,var2)                     |
 |                     else                                                  |
 |                     shr_r(var1,var2) = add(shr(var1,var2),1)              |
 |    - If var2 is less than or equal to zero :                              |
 |                     shr_r(var1,var2) = shr(var1,var2).                    |
 |                                                                           |
 |   Complexity weight : 2                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |    var2                                                                   |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    var_out                                                                |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var_out <= 0x0000 7fff.                |
 |___________________________________________________________________________|
*/
XT_INLINE Word16 shr_r(Word16 shwVar1, Word16 shwVar2)
{
    ae_p24x2s  p1;

    /* shwVar1(1.16) --> p1(9.24) */
    p1 = AE_CVTP24A16(shwVar1);

    if (shwVar2 >= 0)
    {
        /* 限制位移数 */
        shwVar2 = shwVar2 > 16 ? 16 : shwVar2;

        /* 将需要右移比特数写入移位寄存器 */
        WUR_AE_SAR(shwVar2);

        /* p1做24bit算术右移 --> p2 */
        p1 = AE_SRASP24(p1);

        /* 对p1进行高16比特非对称取整(0.5->1;-0.5->0) --> p2*/
        p1 = AE_ROUNDSP16ASYM(p1);
    }
    else
    {
        /* 限制位移数 */
        shwVar2 = shwVar2 < -16 ? -16 : shwVar2;

        /* 将需要左移比特数写入移位寄存器 */
        WUR_AE_SAR(-shwVar2);

        /* p1做24bit带饱和左移 --> p1 */
        p1 = AE_SLLSSP24S(p1);

    }

    return  AE_TRUNCA16P24S_L(p1);
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : mac_r                                                   |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Multiply var1 by var2 and shift the result left by 1. Add the 32 bit    |
 |   result to L_var3 with saturation. Round the LS 16 bits of the result    |
 |   into the MS 16 bits with saturation and shift the result right by 16.   |
 |   Return a 16 bit result.                                                 |
 |            mac_r(L_var3,var1,var2) = round(L_mac(L_var3,var1,var2))       |
 |                                                                           |
 |   Complexity weight : 2                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_var3   32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |    var2                                                                   |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    var_out                                                                |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0x0000 8000 <= L_var_out <= 0x0000 7fff.              |
 |___________________________________________________________________________|
*/
XT_INLINE Word16 mac_r(Word32 swVar3, Word16 shwVar1, Word16 shwVar2)
{
    ae_p24x2s   p1, p2;
    ae_q56s     q1;

    /* var(1.16) --> p(9.24) */
    p1 = AE_CVTP24A16(shwVar1);
    p2 = AE_CVTP24A16(shwVar2);

    /* L_var3（1.32） --> q1(17.48) */
    q1 = AE_CVTQ48A32S(swVar3);

    /* q1+p1(1.24)*p2(1.24)*2 --> q1(1.48) */
    AE_MULAFS32P16S_LL(q1, p1, p2);

    /* 对q1进行32.48比特非对称取整(0.5->1;-0.5->0) --> p1 */
    p1 = AE_ROUNDSP16Q48ASYM(q1);

    return AE_TRUNCA16P24S_L(p1);
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : msu_r                                                   |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Multiply var1 by var2 and shift the result left by 1. Subtract the 32   |
 |   bit result to L_var3 with saturation. Round the LS 16 bits of the res-  |
 |   ult into the MS 16 bits with saturation and shift the result right by   |
 |   16. Return a 16 bit result.                                             |
 |            msu_r(L_var3,var1,var2) = round(L_msu(L_var3,var1,var2))       |
 |                                                                           |
 |   Complexity weight : 2                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_var3   32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |    var2                                                                   |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    var_out                                                                |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0x0000 8000 <= L_var_out <= 0x0000 7fff.              |
 |___________________________________________________________________________|
*/
XT_INLINE Word16 msu_r(Word32 swVar3, Word16 shwVar1, Word16 shwVar2)
{
    ae_p24x2s   p1, p2;
    ae_q56s     q1;

    /* var(1.16) --> p(9.24) */
    p1 = AE_CVTP24A16(shwVar1);
    p2 = AE_CVTP24A16(shwVar2);

    /* L_var3（1.32） --> q1(17.48) */
    q1 = AE_CVTQ48A32S(swVar3);

    /* q1-p1(1.24)*p2(1.24)*2 --> q1(1.48) */
    AE_MULSFS32P16S_LL(q1, p1, p2);

    /* 对q1进行32.48比特非对称取整(0.5->1;-0.5->0) --> p1 */
    p1 = AE_ROUNDSP16Q48ASYM(q1);

    return AE_TRUNCA16P24S_L(p1);
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : L_deposit_h                                             |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Deposit the 16 bit var1 into the 16 MS bits of the 32 bit output. The   |
 |   16 LS bits of the output are zeroed.                                    |
 |                                                                           |
 |   Complexity weight : 2                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    L_var_out                                                              |
 |             32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0x8000 0000 <= var_out <= 0x7fff 0000.                |
 |___________________________________________________________________________|
*/

XT_INLINE Word32 L_deposit_h(Word16 shwVar1)
{
    ae_p24x2s   p1;
    /* shwVar1(1.16) --> p1(9.24) */
    p1 = AE_CVTP24A16(shwVar1);
    /* p1(9.24)-->swVarOut(17.32); 0-->swVarOut(1.16) */
    return AE_CVTA32P24_H(p1);
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : L_deposit_l                                             |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Deposit the 16 bit var1 into the 16 LS bits of the 32 bit output. The   |
 |   16 MS bits of the output are sign extended.                             |
 |                                                                           |
 |   Complexity weight : 2                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    L_var_out                                                              |
 |             32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0xFFFF 8000 <= var_out <= 0x0000 7fff.                |
 |___________________________________________________________________________|
*/

XT_INLINE Word32 L_deposit_l(Word16 shwVar1)
{
    return (Word32)shwVar1;
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : L_shr_r                                                 |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Same as L_shr(L_var1,var2) but with rounding. Saturate the result in    |
 |   case of underflows or overflows :                                       |
 |    - If var2 is greater than zero :                                       |
 |          if (L_sub(L_shl(L_shr(L_var1,var2),1),L_shr(L_var1,sub(var2,1))))|
 |          is equal to zero                                                 |
 |                     then                                                  |
 |                     L_shr_r(L_var1,var2) = L_shr(L_var1,var2)             |
 |                     else                                                  |
 |                     L_shr_r(L_var1,var2) = L_add(L_shr(L_var1,var2),1)    |
 |    - If var2 is less than or equal to zero :                              |
 |                     L_shr_r(L_var1,var2) = L_shr(L_var1,var2).            |
 |                                                                           |
 |   Complexity weight : 3                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_var1                                                                 |
 |             32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0x8000 0000 <= var1 <= 0x7fff ffff.                   |
 |                                                                           |
 |    var2                                                                   |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    L_var_out                                                              |
 |             32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0x8000 0000 <= var_out <= 0x7fff ffff.                |
 |___________________________________________________________________________|
*/

XT_INLINE Word32 L_shr_r(Word32 swVar1, Word16 shwVar2)
{
    ae_q56s  q1;

    /* L_var1（1.32） --> q1(17.48) */
    q1 = AE_CVTQ48A32S(swVar1);

    if (shwVar2 >= 0)
    {
        /* 限制位移数 */
        shwVar2 = shwVar2 > 32 ? 32 : shwVar2;

        /* q1做56bit算术右移 --> q2 */
        q1 = AE_SRAAQ56(q1, shwVar2);

        /* 对q2(17.48)非对称取整(0.5->1;-0.5->0) --> q2*/
        q1 = AE_ROUNDSQ32ASYM(q1);
    }
    else
    {
        /* 限制位移数 */
        shwVar2 = shwVar2 < -32 ? -32 : shwVar2;

        /* q1做56bit带饱和左移 --> q2 */
        q1 = AE_SLLASQ56S(q1, -shwVar2);

        /* q2做48比特饱和处理 */
        q1 = AE_SATQ48S(q1);
    }

    return AE_TRUNCA32Q48(q1);
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : L_abs                                                   |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |    Absolute value of L_var1; Saturate in case where the input is          |
 |                                                               -214783648  |
 |                                                                           |
 |   Complexity weight : 3                                                   |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_var1                                                                 |
 |             32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0x8000 0000 <= var1 <= 0x7fff ffff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    L_var_out                                                              |
 |             32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0x0000 0000 <= var_out <= 0x7fff ffff.                |
 |___________________________________________________________________________|
*/

XT_INLINE Word32 L_abs(Word32 swVar1)
{
    Flag     swOverFlow;
    ae_q56s  q1;

    /* swVar1(1.32) --> q1(17.48) */
    q1 = AE_CVTQ48A32S(swVar1);

    /* 保存溢出标志 */
    swOverFlow = CODEC_OpGetOverflow();

    /* q1做56比特绝对值计算 --> q2 */
    q1 = AE_ABSQ56(q1);

    /* q1做48比特饱和处理 */
    q1 = AE_SATQ48S(q1);

    /* 恢复溢出标志 */
    CODEC_OpSetOverflow(swOverFlow);

    /* q2(17.48) --> swVarOut */
    return AE_TRUNCA32Q48(q1);
}

/* 不采用内联实现 */
extern Word32 L_sat(Word32 swVar1);                                             /* Long saturation */

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : norm_s                                                  |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Produces the number of left shift needed to normalize the 16 bit varia- |
 |   ble var1 for positive values on the interval with minimum of 16384 and  |
 |   maximum of 32767, and for negative values on the interval with minimum  |
 |   of -32768 and maximum of -16384; in order to normalize the result, the  |
 |   following operation must be done :                                      |
 |                    norm_var1 = shl(var1,norm_s(var1)).                    |
 |                                                                           |
 |   Complexity weight : 15                                                  |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    var1                                                                   |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    var_out                                                                |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0x0000 0000 <= var_out <= 0x0000 000f.                |
 |___________________________________________________________________________|
*/
XT_INLINE Word16 norm_s(Word16 shwVar1)
{
    ae_p24x2s  p1;
    ae_q56s    q1;

    if (0 == shwVar1)
    {
        return shwVar1;
    }

    /* shwVar1(1.16) --> p1(9.24) */
    p1 = AE_CVTP24A16(shwVar1);

    /* p1(1.24) --> q1(1.24)*/
    q1 = AE_CVTQ48P24S_L(p1);

    /* 计算q1 56比特归一化数值 */
    return (AE_NSAQ56S(q1) - 8);

}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : norm_l                                                  |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Produces the number of left shifts needed to normalize the 32 bit varia-|
 |   ble L_var1 for positive values on the interval with minimum of          |
 |   1073741824 and maximum of 2147483647, and for negative values on the in-|
 |   terval with minimum of -2147483648 and maximum of -1073741824; in order |
 |   to normalize the result, the following operation must be done :         |
 |                   norm_L_var1 = L_shl(L_var1,norm_l(L_var1)).             |
 |                                                                           |
 |   Complexity weight : 30                                                  |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    L_var1                                                                 |
 |             32 bit long signed integer (Word32) whose value falls in the  |
 |             range : 0x8000 0000 <= var1 <= 0x7fff ffff.                   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    var_out                                                                |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0x0000 0000 <= var_out <= 0x0000 001f.                |
 |___________________________________________________________________________|
*/
XT_INLINE Word16 norm_l(Word32 swVar1)
{
    ae_q56s  q1;

    if (0 == swVar1)
    {
        return 0;
    }

    /* L_var1（1.32） --> q1(17.48) */
    q1 = AE_CVTQ48A32S(swVar1);

    /* 计算q1 56比特归一化数值 */
    return (AE_NSAQ56S(q1) - 8);
}


/* ETSI : oper_32b.c */

/*****************************************************************************
 *                                                                           *
 *  Function L_Extract()                                                     *
 *                                                                           *
 *  Extract from a 32 bit integer two 16 bit DPF.                            *
 *                                                                           *
 *  Arguments:                                                               *
 *                                                                           *
 *   L_32      : 32 bit integer.                                             *
 *               0x8000 0000 <= L_32 <= 0x7fff ffff.                         *
 *   hi        : b16 to b31 of L_32                                          *
 *   lo        : (L_32 - hi<<16)>>1                                          *
 *****************************************************************************
*/

XT_INLINE void L_Extract(Word32 swVar32, Word16 *shwHi, Word16 *shwLo)
{
    ae_p24x2s   p1, p2;
    ae_q56s     q1;

    q1 = AE_CVTQ48A32S(swVar32);

    p1 = AE_TRUNCP24Q48(q1);
    *((ae_p16s *)shwHi) = p1;

    p2 = AE_CVTP24A16(16384);
    q1 = AE_SRAIQ56(q1, 1);
    AE_MULSFS32P16S_LL(q1, p1, p2);
    *shwLo = (Word16)AE_TRUNCA32Q48(q1);
}

/*****************************************************************************
 *                                                                           *
 *  Function L_Comp()                                                        *
 *                                                                           *
 *  Compose from two 16 bit DPF a 32 bit integer.                            *
 *                                                                           *
 *     L_32 = hi<<16 + lo<<1                                                 *
 *                                                                           *
 *  Arguments:                                                               *
 *                                                                           *
 *   hi        msb                                                           *
 *   lo        lsf (with sign)                                               *
 *                                                                           *
 *   Return Value :                                                          *
 *                                                                           *
 *             32 bit long signed integer (Word32) whose value falls in the  *
 *             range : 0x8000 0000 <= L_32 <= 0x7fff fff0.                   *
 *                                                                           *
 *****************************************************************************
*/

XT_INLINE Word32 L_Comp (Word16 shwHi, Word16 shwLo)
{
    ae_p24x2s  p1, p2;
    ae_q56s    q1;

    p1 = AE_CVTP24A16(shwHi);

    q1 = AE_CVTQ48P24S_L(p1);

    p1 = AE_CVTP24A16(1);
    p2 = AE_CVTP24A16(shwLo);
    AE_MULAFS32P16S_LL(q1, p1, p2);

    return AE_TRUNCA32Q48(q1);
}

/*****************************************************************************
 * Function Mpy_32()                                                         *
 *                                                                           *
 *   Multiply two 32 bit integers (DPF). The result is divided by 2**31      *
 *                                                                           *
 *   L_32 = (hi1*hi2)<<1 + ( (hi1*lo2)>>15 + (lo1*hi2)>>15 )<<1              *
 *                                                                           *
 *   This operation can also be viewed as the multiplication of two Q31      *
 *   number and the result is also in Q31.                                   *
 *                                                                           *
 * Arguments:                                                                *
 *                                                                           *
 *  hi1         hi part of first number                                      *
 *  lo1         lo part of first number                                      *
 *  hi2         hi part of second number                                     *
 *  lo2         lo part of second number                                     *
 *                                                                           *
 *****************************************************************************
*/
XT_INLINE Word32 Mpy_32 (Word16 shwHi1, Word16 shwLo1, Word16 shwHi2, Word16 shwLo2)
{
    ae_p24x2s  p1, p2, p3, p4;
    ae_q56s    q1, q2;

    p1 = AE_CVTP24A16X2(shwHi1, shwLo1);
    p2 = AE_CVTP24A16X2(shwHi2, shwLo2);
    p4 = AE_CVTP24A16(1);

    q1 = AE_MULFS32P16S_HH(p1, p2);

    q2 = AE_MULFS32P16S_HL(p1, p2);
    p3 = AE_TRUNCP24Q48(q2);
    p3 = AE_TRUNCP16(p3);

    AE_MULAFS32P16S_LL(q1, p3, p4);

    q2 = AE_MULFS32P16S_LH(p1, p2);
    p3 = AE_TRUNCP24Q48(q2);
    p3 = AE_TRUNCP16(p3);

    AE_MULAFS32P16S_LL(q1, p3, p4);

    return AE_TRUNCA32Q48(q1);
}

/*****************************************************************************
 * Function Mpy_32_16()                                                      *
 *                                                                           *
 *   Multiply a 16 bit integer by a 32 bit (DPF). The result is divided      *
 *   by 2**15                                                                *
 *                                                                           *
 *                                                                           *
 *   L_32 = (hi1*lo2)<<1 + ((lo1*lo2)>>15)<<1                                *
 *                                                                           *
 * Arguments:                                                                *
 *                                                                           *
 *  hi          hi part of 32 bit number.                                    *
 *  lo          lo part of 32 bit number.                                    *
 *  n           16 bit number.                                               *
 *                                                                           *
 *****************************************************************************
*/
XT_INLINE Word32 Mpy_32_16 (Word16 shwHi, Word16 shwLo, Word16 shwN)
{
    ae_p24x2s  p1, p2;
    ae_q56s    q1, q2;

    p1 = AE_CVTP24A16(shwHi);
    p2 = AE_CVTP24A16(shwN);
    q1 = AE_MULFS32P16S_LL(p1, p2);

    p1 = AE_CVTP24A16(shwLo);
    q2 = AE_MULFS32P16S_LL(p1, p2);

    p2 = AE_TRUNCP24Q48(q2);
    p1 = AE_CVTP24A16(1);
    AE_MULAFS32P16S_LL(q1, p1, p2);

    return AE_TRUNCA32Q48(q1);
}

/* 不采用内联实现 */
extern Word32 Div_32(Word32 swNum, Word16 denom_hi, Word16 denom_lo);

/* ETSI : mac_32.c */

/*****************************************************************************
 * Function Mac_32()                                                         *
 *                                                                           *
 *   Multiply two 32 bit integers (DPF) and accumulate with (normal) 32 bit  *
 *   integer. The multiplication result is divided by 2**31                  *
 *                                                                           *
 *   L_32 = L_32 + (hi1*hi2)<<1 + ( (hi1*lo2)>>15 + (lo1*hi2)>>15 )<<1       *
 *                                                                           *
 *   This operation can also be viewed as the multiplication of two Q31      *
 *   number and the result is also in Q31.                                   *
 *                                                                           *
 * Arguments:                                                                *
 *                                                                           *
 *  hi1         hi part of first number                                      *
 *  lo1         lo part of first number                                      *
 *  hi2         hi part of second number                                     *
 *  lo2         lo part of second number                                     *
 *                                                                           *
 *****************************************************************************
*/

XT_INLINE Word32 Mac_32 (Word32 swVar32, Word16 shwHi1, Word16 shwLo1, Word16 shwHi2, Word16 shwLo2)
{
    ae_p24x2s  p1, p2, p3, p4;
    ae_q56s    q1, q2;

    q1 = AE_CVTQ48A32S(swVar32);
    p1 = AE_CVTP24A16X2(shwHi1, shwLo1);
    p2 = AE_CVTP24A16X2(shwHi2, shwLo2);
    p4 = AE_CVTP24A16(1);

    AE_MULAFS32P16S_HH(q1, p1, p2);

    q2 = AE_MULFS32P16S_HL(p1, p2);
    p3 = AE_TRUNCP24Q48(q2);
    p3 = AE_TRUNCP16(p3);

    AE_MULAFS32P16S_LL(q1, p3, p4);

    q2 = AE_MULFS32P16S_LH(p1, p2);
    p3 = AE_TRUNCP24Q48(q2);
    p3 = AE_TRUNCP16(p3);

    AE_MULAFS32P16S_LL(q1, p3, p4);

    return AE_TRUNCA32Q48(q1);
}

/*****************************************************************************
 * Function Mac_32_16()                                                      *
 *                                                                           *
 *   Multiply a 16 bit integer by a 32 bit (DPF) and accumulate with (normal)*
 *   32 bit integer. The multiplication result is divided by 2**15           *
 *                                                                           *
 *                                                                           *
 *   L_32 = L_32 + (hi1*lo2)<<1 + ((lo1*lo2)>>15)<<1                         *
 *                                                                           *
 * Arguments:                                                                *
 *                                                                           *
 *  hi          hi part of 32 bit number.                                    *
 *  lo          lo part of 32 bit number.                                    *
 *  n           16 bit number.                                               *
 *                                                                           *
 *****************************************************************************
*/

XT_INLINE Word32 Mac_32_16(Word32 swVar32, Word16 shwHi, Word16 shwLo, Word16 shwN)
{
    ae_p24x2s  p1, p2, p3, p4;
    ae_q56s    q1, q2;

    p1 = AE_CVTP24A16(shwHi);
    p2 = AE_CVTP24A16(shwN);
    q1 = AE_CVTQ48A32S(swVar32);
    AE_MULAFS32P16S_LL(q1, p1, p2);

    p1 = AE_CVTP24A16(shwLo);
    q2 = AE_MULFS32P16S_LL(p1, p2);

    p2 = AE_TRUNCP24Q48(q2);
    p1 = AE_CVTP24A16(1);
    AE_MULAFS32P16S_LL(q1, p1, p2);

    return AE_TRUNCA32Q48(q1);
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : div_s                                                   |
 |                                                                           |
 |   Purpose :                                                               |
 |                                                                           |
 |   Produces a result which is the fractional integer division of shwVar1   |
 |   by shwVar2; shwVar1 and shwVar2 must be positive and shwVar2 must be    |
 |    greater or equal to shwVar1; the result is positive (leading bit equal |
 |    to 0) and truncated to 16 bits.                                        |
 |                                                                           |
 |   If shwVar1 = shwVar2 then div(shwVar1,shwVar2) = 32767.                 |
 |                                                                           |
 |   Complexity weight : 18                                                  |
 |                                                                           |
 |   Inputs :                                                                |
 |                                                                           |
 |    shwVar1                                                                |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0x0000 0000 <= shwVar1 <= shwVar2 and shwVar2 != 0.   |
 |                                                                           |
 |    shwVar2                                                                |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : shwVar1 <= shwVar2 <= 0x0000 7fff and shwVar2 != 0.   |
 |                                                                           |
 |   Outputs :                                                               |
 |                                                                           |
 |    none                                                                   |
 |                                                                           |
 |   Return Value :                                                          |
 |                                                                           |
 |    shwVarOut                                                              |
 |             16 bit short signed integer (Word16) whose value falls in the |
 |             range : 0x0000 0000 <= shwVarOut <= 0x0000 7fff.              |
 |             It's a Q15 value (point between b15 and b14).                 |
 |___________________________________________________________________________|
*/
/* 修改ETSI标准C函数 */
XT_INLINE Word16 div_s(Word16 shwVar1, Word16 shwVar2)
{
    Word16 shwVarOut = 0;
    Word32 swIter;
    Word32 swNum;
    Word32 swDenom;
    ae_p24x2s  p1, p2;
    ae_q56s  q1, q2;

    if ((shwVar1 > shwVar2) || (shwVar1 <= 0) || (shwVar2 <= 0))
    {
        return 0;
    }
    else
    {
        if (shwVar1 == shwVar2)
        {
            shwVarOut = 0x7fff;
        }
        else
        {
            swNum   = (Word32)shwVar1;
            swDenom = (Word32)shwVar2;

            p2 = AE_CVTP24A16(1);
            q2 = AE_CVTQ48A32S(swDenom);

            for (swIter = 0; swIter < 15; swIter++)
            {
                shwVarOut <<= 1;
                swNum     <<= 1;

                if (swNum >= swDenom)
                {
                    //swNum     = L_sub(swNum, swDenom);
                    q1 = AE_CVTQ48A32S(swNum);
                    q1 = AE_SUBQ56(q1, q2);
                    q1 = AE_SATQ48S(q1);
                    swNum =  AE_TRUNCA32Q48(q1);

                    //shwVarOut = add(shwVarOut, 1);
                    p1 = AE_CVTP24A16(shwVarOut);
                    p1 = AE_ADDSP24S(p1, p2);
                    shwVarOut = AE_TRUNCA16P24S_L(p1);
                }
            }
        }
    }

    return (shwVarOut);
}

#endif /* ifndef _MED_C89_ */


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of med_com_hifi_basicop.h */
