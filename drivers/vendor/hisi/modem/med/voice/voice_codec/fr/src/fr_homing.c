/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : fr_homing.c
  版 本 号   : 初稿
  作    者   : 谢明辉 58441
  生成日期   : 2011年5月10日
  最近修改   :
  功能描述   : FR Homing帧检测
  函数列表   :
               FR_DecDetectHomingFrame
               FR_EncDetectHomingFrame
  修改历史   :
  1.日    期   : 2011年5月10日
    作    者   : 谢明辉 58441
    修改内容   : 创建文件

******************************************************************************/

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "fr_homing.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
/* 解码Homing帧码流参数表，即协议3GPP TS 46.010 表4.1 */
const Word16 g_ashwFrDecHomingStreamMask[FR_NUM_OF_PRMS] =
{
    0x0009,                             /* LARc[1] */
    0x0017,                             /* LARc[2] */
    0x000F,                             /* LARc[3] */
    0x0008,                             /* LARc[4] */
    0x0007,                             /* LARc[5] */
    0x0003,                             /* LARc[6] */
    0x0003,                             /* LARc[7] */
    0x0002,                             /* LARc[8] */

    0x0028,                             /* LTP lag  1 */
    0x0000,                             /* LTP gain 1 */
    0x0000,                             /* RPE grid 1 */
    0x0000,                             /* Block amplitude 1*/
    0x0004,                             /* RPE pulse 1-0 */
    0x0004,                             /* RPE pulse 1-1 */
    0x0004,                             /* RPE pulse 1-2 */
    0x0004,                             /* RPE pulse 1-3 */
    0x0004,                             /* RPE pulse 1-4 */
    0x0004,                             /* RPE pulse 1-5 */
    0x0004,                             /* RPE pulse 1-6 */
    0x0004,                             /* RPE pulse 1-7 */
    0x0004,                             /* RPE pulse 1-8 */
    0x0004,                             /* RPE pulse 1-9 */
    0x0004,                             /* RPE pulse 1-10 */
    0x0004,                             /* RPE pulse 1-11 */
    0x0004,                             /* RPE pulse 1-12 */

    0x0028,                             /* LTP lag  2 */
    0x0000,                             /* LTP gain 2 */
    0x0000,                             /* RPE grid 2 */
    0x0000,                             /* Block amplitude 2 */
    0x0004,                             /* RPE pulse 2-0 */
    0x0004,                             /* RPE pulse 2-1 */
    0x0004,                             /* RPE pulse 2-2 */
    0x0004,                             /* RPE pulse 2-3 */
    0x0004,                             /* RPE pulse 2-4 */
    0x0004,                             /* RPE pulse 2-5 */
    0x0004,                             /* RPE pulse 2-6 */
    0x0004,                             /* RPE pulse 2-7 */
    0x0004,                             /* RPE pulse 2-8 */
    0x0004,                             /* RPE pulse 2-9 */
    0x0004,                             /* RPE pulse 2-10 */
    0x0004,                             /* RPE pulse 2-11 */
    0x0004,                             /* RPE pulse 2-12 */


    0x0028,                             /* LTP lag  3 */
    0x0000,                             /* LTP gain 3 */
    0x0000,                             /* RPE grid 3 */
    0x0000,                             /* Block amplitude 3 */
    0x0004,                             /* RPE pulse 3-0 */
    0x0004,                             /* RPE pulse 3-1 */
    0x0004,                             /* RPE pulse 3-2 */
    0x0004,                             /* RPE pulse 3-3 */
    0x0004,                             /* RPE pulse 3-4 */
    0x0004,                             /* RPE pulse 3-5 */
    0x0004,                             /* RPE pulse 3-6 */
    0x0004,                             /* RPE pulse 3-7 */
    0x0004,                             /* RPE pulse 3-8 */
    0x0004,                             /* RPE pulse 3-9 */
    0x0004,                             /* RPE pulse 3-10 */
    0x0004,                             /* RPE pulse 3-11 */
    0x0004,                             /* RPE pulse 3-12 */


    0x0028,                             /* LTP lag  4 */
    0x0000,                             /* LTP gain 4 */
    0x0000,                             /* RPE grid 4 */
    0x0000,                             /* Block amplitude 4*/
    0x0004,                             /* RPE pulse 4-0 */
    0x0004,                             /* RPE pulse 4-1 */
    0x0004,                             /* RPE pulse 4-2 */
    0x0004,                             /* RPE pulse 4-3 */
    0x0003,                             /* RPE pulse 4-4 */
    0x0004,                             /* RPE pulse 4-5 */
    0x0004,                             /* RPE pulse 4-6 */
    0x0004,                             /* RPE pulse 4-7 */
    0x0004,                             /* RPE pulse 4-8 */
    0x0004,                             /* RPE pulse 4-9 */
    0x0004,                             /* RPE pulse 4-10 */
    0x0004,                             /* RPE pulse 4-11 */
    0x0004,                             /* RPE pulse 4-12 */
};

/*****************************************************************************
  3 函数实现
*****************************************************************************/
/*****************************************************************************
 函 数 名  : FR_EncDetectHomingFrame
 功能描述  : FR上行Homing帧检测
 输入参数  : Word16 *pshwSpeechIn    --输入160点PCM语音帧
 输出参数  : 无
 返 回 值  : Word16
 调用函数  :
 被调函数  : FR_HOMING_STATUS_ENUM_UINT16  --Homing帧检测标志

 修改历史      :
  1.日    期   : 2011年5月7日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
FR_HOMING_STATUS_ENUM_UINT16 FR_EncDetectHomingFrame(Word16 *pshwSpeechIn)
{
    FR_HOMING_STATUS_ENUM_UINT16         enHomingStatus;
    Word32                               swCnt;

    enHomingStatus = FR_HOMING_STATUS_YES;

    for (swCnt = 0; swCnt < 160; swCnt++)
    {
        /* 如果当前样点不等于0x0008，置Homing帧检测标志为NOT */
        if (pshwSpeechIn[swCnt] != FR_HOMGING_ENC_PCM_VALUE)
        {
            enHomingStatus = FR_HOMING_STATUS_NOT;
            break;
        }
    }

    return enHomingStatus;
}

/*****************************************************************************
 函 数 名  : FR_DecDetectHomingFrame
 功能描述  : FR下行Homing帧检测
 输入参数  : Word16 *pshwParm      --输入帧码流
             Word16 shwNumOfParms  --检测参数个数
 输出参数  : 无
 返 回 值  : FR_HOMING_STATUS_ENUM_UINT16  --Homing帧检测标志
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年5月7日
    作    者   : 谢明辉 58441
    修改内容   : 新生成函数

*****************************************************************************/
FR_HOMING_STATUS_ENUM_UINT16 FR_DecDetectHomingFrame(Word16 *pshwParm, Word16 shwNumOfParms)
{
    FR_HOMING_STATUS_ENUM_UINT16         enHomingStatus;
    Word32                               swCnt;

    enHomingStatus = FR_HOMING_STATUS_YES;

    for (swCnt = 0; swCnt < shwNumOfParms; swCnt++)
    {
        /* 如果当前样点不等于Homing帧码流数组[i]，置Homing帧检测标志为NOT */
        if (pshwParm[swCnt] != g_ashwFrDecHomingStreamMask[swCnt])
        {
            enHomingStatus = FR_HOMING_STATUS_NOT;
            break;
        }
    }

    return enHomingStatus;
}

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

