

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "codec_com_codec.h"

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
/*****************************************************************************
 函 数 名  : CODEC_ComCodecBits2Prm
 功能描述  : 将对应比特数转换为参数
 输入参数  : VOS_INT16 *pshwBitStream      --比特流指针
             VOS_INT16 shwNumOfBits        --待转换比特个数
 输出参数  : VOS_INT16 *pshwPrmValue       --转换获得的参数指针
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年6月9日
    作    者   : 金剑
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID CODEC_ComCodecBits2Prm(VOS_INT16 *pshwBitStream, VOS_INT16 shwNumOfBits, VOS_INT16 *pshwPrmValue)
{
    VOS_INT16   i;
    VOS_INT16   shwValue = 0;
    VOS_INT16   shwBit;

    /* 将shwNumOfBits个比特放入双字节数的低shwNumOfBits位，高位为0 */
    for (i = 0; i < shwNumOfBits; i++)
    {
        shwBit  = *pshwBitStream++;
        shwBit  = shwBit & BIT0_MASK;
        shwBit  = shl(shwBit, i);
        shwValue= add(shwValue, shwBit);
    }

    *pshwPrmValue = shwValue;
}
VOS_VOID CODEC_ComCodecPrm2Bits(VOS_INT16 shwValue, VOS_INT16 shwNumOfBits, VOS_INT16 *pshwBitStream)
{
    VOS_INT16   shwCnt;
    VOS_INT16   shwBit;

    VOS_INT16  *pshwStream;

    pshwStream = pshwBitStream;

    /*将shwValue低shwNumOfBits个比特依次放入pshwBitStream中*/
    for (shwCnt = 0; shwCnt < shwNumOfBits; shwCnt++)
    {
        /*获取参数第i比特值*/
        shwBit = shwValue & BIT0_MASK;

        if (0 == shwBit)
        {
            *(pshwStream++) = 0;
        }
        else
        {
            *(pshwStream++) = 1;
        }

        /*右移1位获取次之比特*/
        shwValue = shr(shwValue, 1);
    }

}
VOS_VOID CODEC_ComCodecBits2Bytes(
                VOS_INT16 *pshwBits,
                VOS_INT16  shwBitsNum,
                VOS_INT8  *pscBytes)
{
    VOS_INT16    i;
    VOS_INT16    shwBit;
    VOS_INT16    shwBitCnt;

    VOS_INT8    *pscByteTemp;

    pscByteTemp = pscBytes;
    shwBitCnt   = 0;

    do
    {
        *pscByteTemp = 0;
        for (i = 0; (i < CODEC_BYTE_BITS_NUM) && (shwBitCnt < shwBitsNum); i++)
        {
            shwBit       = *(pshwBits + shwBitCnt);
            shwBit       = shwBit & BIT0_MASK;
            shwBit       = shl(shwBit, (7-i));                                  /*7表示从高比特向低比特逐个放置比特流*/
            *pscByteTemp = *pscByteTemp + (VOS_INT8)shwBit;
            shwBitCnt++;
        }
        pscByteTemp++;

    }while(shwBitCnt < shwBitsNum);

}
VOS_VOID CODEC_ComCodecBytes2Bits(
                VOS_INT8                *pscBytes,
                VOS_INT16                shwBitsNum,
                VOS_INT16               *pshwBits)
{
    VOS_INT16       i;
    VOS_INT16       shwBitCnt;
    VOS_INT16      *pshwBitsTemp;

    VOS_INT8        scByte;
    VOS_INT8        scByteMask;

    pshwBitsTemp = pshwBits;
    shwBitCnt    = 0;

    do
    {
        scByte     = *(pscBytes++);

        for (i = CODEC_BYTE_BITS_NUM; (i > 0) && (shwBitCnt < shwBitsNum); i--)
        {
            scByteMask                                = scByte >> (i-1);
            *(pshwBitsTemp + (CODEC_BYTE_BITS_NUM - i)) = scByteMask & BIT0_MASK;

            shwBitCnt++;
        }

        pshwBitsTemp += CODEC_BYTE_BITS_NUM;

    }while(shwBitCnt < shwBitsNum);

}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

