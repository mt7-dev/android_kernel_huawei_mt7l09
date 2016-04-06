

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include  "med_anr_2mic_td.h"
#include  "om.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
MED_2MIC_TDPROCESS_STRU         g_stTdProcess;

/* 滤波器系数数组 */
const VOS_INT16 g_ashwFilterNum[4] = {8729,24252,24252,8729};
const VOS_INT16 g_ashwFilterDen[4] = {32767,16892,18901,-2597};

const VOS_INT16 g_ashwDelayCoef[MED_2MIC_SPACESPLIT_DELAYCOEF_LENGH] =                                           \
                  { -141,   308,   -623,   1134,  -1959, 3388, -6471, 20748,    \
                   20748, -6471,  3388,  -1959,   1134, -623,  308,   -141};


/*****************************************************************************
  3 函数实现
*****************************************************************************/

VOS_UINT32 MED_ANR_2MIC_TdInit(
                MED_2MIC_NV_STRU                    *pst2MICParam,
                CODEC_SAMPLE_RATE_MODE_ENUM_INT32      enSampleRate)

{
    MED_2MIC_TDPROCESS_STRU     *pstTdProcess;
    MED_2MIC_CALIB_STRU         *pstCalib;
    MED_2MIC_SPACESPLIT_STRU    *pstSpaceSplit;
    MED_2MIC_ANC_STRU           *pstAnc;
    CODEC_SWITCH_ENUM_UINT16     enCalibEnable;
    VOS_INT16                    shwRcDelay;
    VOS_INT16                    shwVadRatioThld;
    VOS_INT16                    shwFarMaxThd;
    VOS_UINT32                   uwCalibRet;
    VOS_UINT32                   uwSpaceRet;
    VOS_UINT32                   uwAncRet;

    /* 异常入参检查 */
    if (   (VOS_NULL == pst2MICParam)
        || (CODEC_SAMPLE_RATE_MODE_BUTT <= enSampleRate))
    {
        return UCOM_RET_ERR_PARA;
    }

    /* 获取相应结构体指针 */
    pstTdProcess    = MED_2MIC_GetTdProcessPtr();
    pstCalib        = MED_2MIC_GetTdCalibPtr();
    pstSpaceSplit   = MED_2MIC_GetTdSpaceSplitPtr();
    pstAnc          = MED_2MIC_GetTdAncPtr();

    /* 获取相应NV项 */
    enCalibEnable   = pst2MICParam->enCalibEnable;
    shwRcDelay      = pst2MICParam->shwRcDelay;
    shwVadRatioThld = pst2MICParam->shwVadRatioThld;
    shwFarMaxThd    = pst2MICParam->shwFarMaxThd1;

    /* 初始化TD流程控制结构体 */
    UCOM_MemSet(pstTdProcess, 0, sizeof(MED_2MIC_TDPROCESS_STRU));

    /* 设置采样率 */
    MED_2MIC_SetTdSampleRate(enSampleRate);

    /* Calib模块初始化 */
    uwCalibRet  = MED_ANR_2MIC_TdCalibInit(enCalibEnable, pstCalib);

    /* SpaceSplit模块初始化 */
    uwSpaceRet  = MED_ANR_2MIC_TdSpaceSplitInit(shwRcDelay, pstSpaceSplit);

    /* Anc模块初始化 */
    uwAncRet    = MED_ANR_2MIC_TdAncInit(shwVadRatioThld, shwFarMaxThd, pstAnc);

    /* 返回初始化结果 */
    if (   (UCOM_RET_SUCC == uwCalibRet)
        && (UCOM_RET_SUCC == uwSpaceRet)
        && (UCOM_RET_SUCC == uwAncRet))
    {
        return UCOM_RET_SUCC;
    }
    else
    {
        return UCOM_RET_FAIL;
    }

}
VOS_UINT32 MED_ANR_2MIC_TdCalibInit(
                CODEC_SWITCH_ENUM_UINT16       enEnable,
                MED_2MIC_CALIB_STRU         *pstCalib)
{
    pstCalib->enCalibEnable         = enEnable;                                 /* 校准模块使能标志 */
    pstCalib->shwLastGain           = MED_2MIC_MAX_Q14;                         /* 上一帧的校准增益 */

    return UCOM_RET_SUCC;
}


VOS_UINT32 MED_ANR_2MIC_TdSpaceSplitInit(
                VOS_INT16                           shwRcDelay,
                MED_2MIC_SPACESPLIT_STRU            *pstSpaceSplit)
{
    MED_2MIC_SPACESPLIT_MC_STRU         *pstSpaceMc;
    MED_2MIC_SPACESPLIT_RC_STRU         *pstSpaceRc;
    CODEC_SAMPLE_RATE_MODE_ENUM_INT32      enSampleRate;

    /* 获取采样率 */
    enSampleRate    = MED_2MIC_GetTdSampleRate();

    /* 空间分离主Mic初始化 */
    pstSpaceMc      = &(pstSpaceSplit->stMainNormMode);

    /* 计算主麦克延迟长度 */
    pstSpaceMc->shwDelayValue   = CODEC_OpExtract_l(CODEC_OpL_mult0((VOS_INT16)enSampleRate / (VOS_INT16)CODEC_SAMPLE_RATE_MODE_8000,
                                                                MED_2MIC_SPACESPLIT_MC_DELAY));

    /* 空间分离辅Mic初始化 */
    pstSpaceRc      = &(pstSpaceSplit->stRefMode);

    pstSpaceRc->shwDelayValue   = shwRcDelay;
    pstSpaceRc->shwNeGain       = MED_2MIC_SPACESPLIT_MC_NE_GAIN;

    return UCOM_RET_SUCC;

}


VOS_UINT32 MED_ANR_2MIC_TdAncInit(
                VOS_INT16                           shwVadRatioThld,
                VOS_INT16                           shwFarMaxThd,
                MED_2MIC_ANC_STRU                  *pstAnc)
{
    MED_2MIC_VAD_STRU                   *pstAncVad;
    MED_2MIC_VAD_WB_STRU                *pstAncVadWb;
    MED_2MIC_LAF_STRU                   *pstAncLaf;
    CODEC_SAMPLE_RATE_MODE_ENUM_INT32     enSampleRate;

    /* 采样率获取 */
    enSampleRate    = MED_2MIC_GetTdSampleRate();

    /* VAD初始化 */
    pstAncVad                   = &(pstAnc->stVad);
    pstAncVadWb                 = &(pstAnc->stVad.stVadWb);

    pstAncVad->shwVadRatioThd   = shwVadRatioThld;
    pstAncVad->shwPsPnRatio     = MED_2MIC_ANC_VAD_PS_PN_RATIO_INIT;
    pstAncVadWb->shwFilterOrder = MED_2MIC_ANC_VAD_FILTER_ORDER;

    pstAncVadWb->pshwFilterNum = (VOS_INT16 *)MED_2MIC_GetTdFilterNumPtr();
    pstAncVadWb->pshwFilterDen = (VOS_INT16 *)MED_2MIC_GetTdFilterDenPtr();

    /* LAF初始化 */
    pstAncLaf                   = &(pstAnc->stLaf);

    /* 由采样率确定采样率 */
    if (CODEC_SAMPLE_RATE_MODE_8000 == enSampleRate)
    {
        pstAncLaf->shwN         = MED_2MIC_LAF_FILTER_ORDER_NB;
        pstAncLaf->swInShortThd = MED_2MIC_LAF_WEIGHT_SHORT_POWER_THD_NB;
    }
    else
    {
        pstAncLaf->shwN         = MED_2MIC_LAF_FILTER_ORDER_WB;
        pstAncLaf->swInShortThd = MED_2MIC_LAF_WEIGHT_SHORT_POWER_THD_WB;
    }

    pstAncLaf->swInShortPower   = 0;
    pstAncLaf->shwAlpha         = MED_2MIC_LAF_WEIGHT_SHORT_POWER_ALPHA;

    pstAncLaf->shwStep          = MED_2MIC_LAF_STEP_INIT;
    pstAncLaf->swRefPowerThd    = MED_2MIC_LAF_STEP_REF_POWER_THD;
    pstAncLaf->shwFarMaxThd     = shwFarMaxThd;

    /* ANR初始化 */
    pstAnc->shwMainDelayQ       = CODEC_OpShl(pstAncLaf->shwN, MED_PP_SHIFT_BY_7);

    return UCOM_RET_SUCC;

}
VOS_UINT32 MED_ANR_2MIC_TdProcess(
                CODEC_SWITCH_ENUM_UINT16         enEnable,
                CODEC_VAD_FLAG_ENUM_INT16        enVadFlag,
                VOS_INT16                       *pshwMcIn,
                VOS_INT16                       *pshwRcIn,
                VOS_INT16                       *pshwMcOut,
                VOS_INT16                       *pshwRcOut,
                VOS_INT16                       *pshwRcCalib)
{
    MED_2MIC_CALIB_STRU                     *pstCalib;
    MED_2MIC_SPACESPLIT_STRU                *pstSpaceSplit;
    MED_2MIC_ANC_STRU                       *pstAnc;
    VOS_INT16                               *pshwMcCalib;
    VOS_INT16                               *pshwMcSpace;
    VOS_INT16                               *pshwRcSpace;
    VOS_INT16                                shwFrameLen = MED_PP_GetFrameLength();
    VOS_UINT32                               uwCalibRet;
    VOS_UINT32                               uwSpaceRet;
    VOS_UINT32                               uwAncRet = UCOM_RET_SUCC;

    /* 如果ANR不使能 */
    if (CODEC_SWITCH_OFF == enEnable)
    {
        CODEC_OpVecCpy(pshwMcOut, pshwMcIn, shwFrameLen);
        CODEC_OpVecCpy(pshwRcOut, pshwRcIn, shwFrameLen);
        CODEC_OpVecCpy(pshwRcCalib, pshwRcIn, shwFrameLen);

        return UCOM_RET_ERR_PARA;
    }

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_ANR_2MIC_TdProcess);

    /* 分配内存空间 */
    pshwMcCalib = (VOS_INT16 *)UCOM_MemAlloc(CODEC_FRAME_LENGTH_WB * sizeof(VOS_INT16));
    pshwMcSpace = (VOS_INT16 *)UCOM_MemAlloc(CODEC_FRAME_LENGTH_WB * sizeof(VOS_INT16));
    pshwRcSpace = (VOS_INT16 *)UCOM_MemAlloc(CODEC_FRAME_LENGTH_WB * sizeof(VOS_INT16));

    /* 获取各模块指针 */
    pstCalib            = MED_2MIC_GetTdCalibPtr();
    pstSpaceSplit       = MED_2MIC_GetTdSpaceSplitPtr();
    pstAnc              = MED_2MIC_GetTdAncPtr();

    /* 幅度校准 */
    uwCalibRet = MED_ANR_2MIC_TdCalib(      enVadFlag,
                                            pstCalib,
                                            pshwMcIn,
                                            pshwRcIn,
                                            pshwMcCalib,
                                            pshwRcCalib);

    /* 空间分离 */
    uwSpaceRet = MED_ANR_2MIC_TdSpaceSplit( pstSpaceSplit,
                                            pshwMcCalib,
                                            pshwRcCalib,
                                            pshwMcSpace,
                                            pshwRcSpace);

    /* 对主麦克信号进行自适应噪声消除 */
    uwAncRet = MED_ANR_2MIC_TdAncNormalMode(pstAnc,
                                            pshwMcSpace,
                                            pshwRcSpace,
                                            pshwRcCalib,
                                            pshwMcOut);

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_ANR_2MIC_TdProcess);

    if (   (UCOM_RET_SUCC == uwCalibRet)
        && (UCOM_RET_SUCC == uwSpaceRet)
        && (UCOM_RET_SUCC == uwAncRet))
    {
        /* 输出TD处理辅麦克信号 */
        UCOM_MemCpy(pshwRcOut, pshwRcSpace, (VOS_UINT16)shwFrameLen * sizeof(VOS_INT16));

        /* 释放分配的空间 */
        UCOM_MemFree(pshwMcCalib);
        UCOM_MemFree(pshwMcSpace);
        UCOM_MemFree(pshwRcSpace);

        return UCOM_RET_SUCC;
    }
    else
    {
        /* 释放分配的空间 */
        UCOM_MemFree(pshwMcCalib);
        UCOM_MemFree(pshwMcSpace);
        UCOM_MemFree(pshwRcSpace);

        return UCOM_RET_FAIL;
    }

}
VOS_UINT32 MED_ANR_2MIC_TdCalib(
                CODEC_VAD_FLAG_ENUM_INT16     enVadFlag,
                MED_2MIC_CALIB_STRU             *pstCalib,
                VOS_INT16                       *pshwMcIn,
                VOS_INT16                       *pshwRcIn,
                VOS_INT16                       *pshwMcOut,
                VOS_INT16                       *pshwRcOut)
{
    VOS_INT16                       shwLastGain;
    VOS_INT16                       shwFrmLen;
    VOS_INT32                       swMainPower;
    VOS_INT16                       shwIndex;
    VOS_INT32                       swRefpower;
    VOS_INT16                       shwCurrentGain;
    VOS_INT16                       shwMaxGainThld;
    VOS_INT16                       shwMinGainThld;
    VOS_INT16                       shwRestrictGain;
    VOS_INT32                       swTemp;
    VOS_INT16                       shwTemp;
    VOS_INT16                       shwSubTemp;
    VOS_INT16                       shwSmoothGain;
    VOS_INT32                       swRcOut;

    /* 获取帧长 */
    shwFrmLen   = MED_PP_GetFrameLength();

    /* 获取上一帧校准增益 */
    shwLastGain = pstCalib->shwLastGain;


    /* 若校准模块不使能，返回 */
    if (CODEC_SWITCH_OFF == pstCalib->enCalibEnable)
    {
        /* 辅麦克信号校准后保持不变 */
        UCOM_MemCpy(pshwMcOut, pshwMcIn, (VOS_UINT16)shwFrmLen * sizeof(VOS_INT16));

        UCOM_MemCpy(pshwRcOut, pshwRcIn, (VOS_UINT16)shwFrmLen * sizeof(VOS_INT16));

        return UCOM_RET_SUCC;
    }

    swMainPower = 0;

    /* 计算主麦克信号能量 */
    for (shwIndex = 0; shwIndex < shwFrmLen; shwIndex++)
    {
        swTemp              = CODEC_OpL_mult0(pshwMcIn[shwIndex], pshwMcIn[shwIndex]);
        swMainPower         = CODEC_OpL_add(swTemp, swMainPower);
    }

    swMainPower = CODEC_OpL_max(swMainPower, MED_2MIC_MIN_POWER_VALUE);
#ifdef _MED_MAX_CYCLE_
    if (1)
#else
    /* 计算校准增益:主麦克信号不为电路噪声且不为语音时 */
    if ((MED_2MIC_CALIB_FLOOR_MIN_POWER < swMainPower)
        && (CODEC_VAD_FLAG_NOISE == enVadFlag))
#endif

    {
            /* 计算辅麦克信号能量 */
            swRefpower = 0;

            for (shwIndex = 0; shwIndex < shwFrmLen; shwIndex++)
            {
                swTemp      = CODEC_OpL_mult0(pshwRcIn[shwIndex], pshwRcIn[shwIndex]);
                swRefpower  = CODEC_OpL_add(swTemp, swRefpower);
            }

            swRefpower      = CODEC_OpL_max(swRefpower, MED_2MIC_MIN_POWER_VALUE);

            /* 初步计算校准增益并限制大小 */
            shwCurrentGain  = CODEC_OpSqrt(CODEC_OpNormDiv_32(swMainPower , swRefpower, MED_PP_SHIFT_BY_28));
            shwMaxGainThld  = CODEC_OpAdd(shwLastGain, MED_2MIC_CALIB_GAIN_DELTA_MAX);
            shwSubTemp      = CODEC_OpSub(shwLastGain, MED_2MIC_CALIB_GAIN_DELTA_MAX);
            shwMinGainThld  = CODEC_OpMax(shwSubTemp, 0);
            shwRestrictGain = CODEC_OpMax(shwMinGainThld, CODEC_OpMin(shwCurrentGain, shwMaxGainThld));

            /* 对校准增益进行平滑并限制大小 */
            shwTemp          = CODEC_OpMult(CODEC_OpSub(MED_2MIC_CALIB_GAIN_MAX, MED_2MIC_CALIB_ALPHA), shwRestrictGain);
            shwSmoothGain   = CODEC_OpAdd(CODEC_OpMult(MED_2MIC_CALIB_ALPHA, shwLastGain), shwTemp);
            shwSmoothGain   = CODEC_OpMax(MED_2MIC_CALIB_GAIN_MIN, CODEC_OpMin(shwSmoothGain, MED_2MIC_CALIB_GAIN_MAX));

            /* 校准增益计算完成 */
            pstCalib->shwLastGain= shwSmoothGain;

    }

    /* 主麦克信号校准保持不变，即差异由辅麦克校准结果进行补偿 */
    UCOM_MemCpy(pshwMcOut, pshwMcIn, (VOS_UINT16)shwFrmLen * sizeof(VOS_INT16));

    for (shwIndex = 0; shwIndex < shwFrmLen; shwIndex++)
    {
        /* 对辅麦克信号校准 */
        swRcOut = CODEC_OpL_shl( CODEC_OpL_mult(pshwRcIn[shwIndex], pstCalib->shwLastGain), 1);
        pshwRcOut[shwIndex] = CODEC_OpRound(swRcOut);
    }

    return UCOM_RET_SUCC;

}


VOS_UINT32 MED_ANR_2MIC_TdSpaceSplit(
                MED_2MIC_SPACESPLIT_STRU        *pstSpaceSplit,
                VOS_INT16                       *pshwMcIn,
                VOS_INT16                       *pshwRcIn,
                VOS_INT16                       *pshwMcOut,
                VOS_INT16                       *pshwRcOut)
{
    MED_2MIC_SPACESPLIT_MC_STRU         *pstMainNormMode;
    MED_2MIC_SPACESPLIT_RC_STRU         *pstRefMode;
    VOS_INT16                           *pshwMainState;
    VOS_INT16                            shwDelayValue;
    VOS_UINT32                           uwRet = UCOM_RET_SUCC;

    /* 异常入参检查 */
    if ((VOS_NULL_PTR == pstSpaceSplit)
     || (VOS_NULL_PTR == pshwMcIn)
     || (VOS_NULL_PTR == pshwRcIn)
     || (VOS_NULL_PTR == pshwMcOut)
     || (VOS_NULL_PTR == pshwRcOut))
    {
        return UCOM_RET_ERR_PARA;
    }

    /* 获取主麦克分离结构体信息 */
    pstMainNormMode     = &(pstSpaceSplit->stMainNormMode);
    shwDelayValue       = pstMainNormMode->shwDelayValue;
    pshwMainState       = pstMainNormMode->ashwMainState;

    /* 获取辅麦克分离结构体信息 */
    pstRefMode          = &(pstSpaceSplit->stRefMode);

    /* 主麦克信号空间分离:采用延迟透传方法 */
    uwRet += MED_ANR_2MIC_TdSpaceIntegerDelay(pshwMcIn,
                                             shwDelayValue,
                                             pshwMainState,
                                             pshwMcOut);

    /* 辅麦克信号空间分离:采用延迟块矩阵方法 */
    uwRet += MED_ANR_2MIC_TdBlockMatrix(pstRefMode,
                                       pshwMcIn,
                                       pshwRcIn,
                                       pshwRcOut);

    return uwRet;

}
VOS_UINT32 MED_ANR_2MIC_TdSpaceIntegerDelay(
                VOS_INT16                       *pshwIn,
                VOS_INT16                        shwDelay,
                VOS_INT16                       *pshwState,
                VOS_INT16                       *pshwOut)
{
    VOS_INT16                       shwDataLen;
    VOS_INT16                       shwLen;

    /* 获取帧长 */
    shwDataLen = MED_PP_GetFrameLength();

    /* 比较低八位，判断shwDelayValue是否为小数 */
    if (0 != (shwDelay & 0xFF))
    {
        return UCOM_RET_ERR_STATE;
    }

    /* 整数延迟 */
    shwDelay    = CODEC_OpShr(shwDelay, MED_PP_SHIFT_BY_8);

    /* 拷贝上一帧保存的数据，长度为延迟点数  */
    UCOM_MemCpy(pshwOut, pshwState, (VOS_UINT16)shwDelay * sizeof(VOS_INT16));

    /* 拷贝本帧数据，长度为帧长中去除延迟点数的剩余长度 */
    shwLen      = CODEC_OpSub(shwDataLen, shwDelay);
    UCOM_MemCpy((pshwOut + shwDelay), pshwIn, (VOS_UINT16)shwLen * sizeof(VOS_INT16));

    /* 更新状态信息，用于下一帧延迟处理 */
    UCOM_MemCpy(pshwState, (pshwIn + shwLen), (VOS_UINT16)shwDelay * sizeof(VOS_INT16));

    return UCOM_RET_SUCC;

}


VOS_UINT32 MED_ANR_2MIC_TdBlockMatrix(
                MED_2MIC_SPACESPLIT_RC_STRU     *pstRefMode,
                VOS_INT16                       *pshwMcIn,
                VOS_INT16                       *pshwRcIn,
                VOS_INT16                       *pshwRcOut)
{
    VOS_INT16                   *pshwRefState;
    VOS_INT16                   *pshwMainState;
    VOS_INT16                    shwDelayValue;
    VOS_INT16                    shwTemp;
    VOS_INT16                    shwNeGain;
    VOS_INT16                    shwFrameLen;
    VOS_INT16                   *pshwMcDelay;
    VOS_INT16                   *pshwRcDelay;
    VOS_INT16                    shwCnt;
    VOS_UINT32                   uwRet;

    /* 分配内存 */
    pshwMcDelay = (VOS_INT16 *)UCOM_MemAlloc(CODEC_FRAME_LENGTH_WB * sizeof(VOS_INT16));
    pshwRcDelay = (VOS_INT16 *)UCOM_MemAlloc(CODEC_FRAME_LENGTH_WB * sizeof(VOS_INT16));

    /* 获取语音帧帧长度 */
    shwFrameLen     = MED_PP_GetFrameLength();

    /* 获取历史信息 */
    pshwRefState    = pstRefMode->ashwRefState;
    pshwMainState   = pstRefMode->ashwMainState;

    /* 获取主麦克信号延迟数值(支持分数)*/
    shwDelayValue   = pstRefMode->shwDelayValue;

    /* 对主麦克信号进行延迟(支持分数) */
    uwRet = MED_ANR_2MIC_TdSpaceFractionDelay(pshwMcIn,
                                              pshwRcIn,
                                              shwDelayValue,
                                              pshwMainState,
                                              pshwRefState,
                                              pshwMcDelay,
                                              pshwRcDelay);

    /* 主麦克信号到达辅麦克的固定增益 */
    shwNeGain       = pstRefMode->shwNeGain;

    /* 消除辅麦克中的语音信号 */
    for (shwCnt = 0; shwCnt < shwFrameLen; shwCnt++)
    {
        shwTemp        = CODEC_OpRound(CODEC_OpL_mult(pshwMcDelay[shwCnt], shwNeGain));

        pshwRcOut[shwCnt] = CODEC_OpSub(pshwRcDelay[shwCnt],shwTemp);
    }

    /* 分配内存释放 */
    UCOM_MemFree(pshwMcDelay);
    UCOM_MemFree(pshwRcDelay);

    return uwRet;

}

#ifndef _MED_C89_

VOS_UINT32 MED_ANR_2MIC_TdSpaceFractionDelay(
                VOS_INT16                       *pshwMcIn,
                VOS_INT16                       *pshwRcIn,
                VOS_INT16                        shwDelayValue,
                VOS_INT16                       *pshwMainState,
                VOS_INT16                       *pshwRefState,
                VOS_INT16                       *pshwMcOut,
                VOS_INT16                       *pshwRcOut)
{
    VOS_INT16   shwMcStateLen;
    VOS_INT16   shwRcStateLen;
    VOS_INT16   shwFrameLen;
    VOS_INT16   shwIntegerDelayValue;
    VOS_INT16   shwCntM;
    VOS_INT32   swTemp;
    VOS_INT16  *pshwNewMicIn;

    /* 分配内存 */
    pshwNewMicIn = (VOS_INT16 *)UCOM_MemAlloc((MED_2MIC_SPACESPLIT_MAX_DELAY + CODEC_FRAME_LENGTH_WB) \
                                               * sizeof(VOS_INT16));

    /* 获取当前语音帧帧长度 */
    shwFrameLen      = MED_PP_GetFrameLength();

    /* 计算滤波器的初始值 */
    shwMcStateLen   = MED_2MIC_SPACESPLIT_MAX_DELAY;
    shwMcStateLen   = CODEC_OpShl(shwMcStateLen, MED_PP_SHIFT_BY_8);
    shwRcStateLen   = MED_2MIC_SPACESPLIT_MAX_DELAY;
    shwRcStateLen   = CODEC_OpShl(shwRcStateLen, MED_PP_SHIFT_BY_8);

    if ((shwDelayValue > shwMcStateLen)
        || (shwDelayValue > shwRcStateLen))
    {
        /* 释放内存 */
        UCOM_MemFree(pshwNewMicIn);

        return UCOM_RET_ERR_PARA;
    }

    /* 取小数位的值进行判断 */
    if (0 == (shwDelayValue & 0xFF))
    {
        /* 若延迟为整数，则仅对主麦克进行整数延迟 */
        MED_ANR_2MIC_TdSpaceIntegerDelay(pshwMcIn, shwDelayValue, pshwMainState, pshwMcOut);

        UCOM_MemCpy(pshwRcOut, pshwRcIn, (VOS_UINT16)shwFrameLen * sizeof(VOS_INT16));
    }

    /* 如果小数部分是0.5 (128计算方法: 0.5 * 2^8) */
    else if (128 == (shwDelayValue & 0xFF))
    {
        /* 当前信号和历史的点组合成新的信号帧 */
        UCOM_MemSet(pshwNewMicIn,
                    0,
                    (MED_2MIC_SPACESPLIT_MAX_DELAY + CODEC_FRAME_LENGTH_WB)*sizeof(VOS_INT16));

        UCOM_MemCpy(pshwNewMicIn,
                    pshwMainState,
                    (MED_2MIC_SPACESPLIT_DELAYCOEF_LENGH - 1) * sizeof(VOS_INT16));

        UCOM_MemCpy(pshwNewMicIn + (MED_2MIC_SPACESPLIT_DELAYCOEF_LENGH - 1),
                    pshwMcIn,
                    (VOS_UINT16)shwFrameLen * sizeof(VOS_INT16));

        fir_bk(pshwMcOut,
               pshwNewMicIn,
               g_ashwDelayCoef,
               shwFrameLen,
               MED_2MIC_SPACESPLIT_DELAYCOEF_LENGH);
        UCOM_MemCpy(pshwMainState,
                    pshwNewMicIn + shwFrameLen,
                    (MED_2MIC_SPACESPLIT_DELAYCOEF_LENGH - 1) * sizeof(VOS_INT16));
        shwIntegerDelayValue = CODEC_OpSub(1920, shwDelayValue);
        MED_ANR_2MIC_TdSpaceIntegerDelay(pshwRcIn, shwIntegerDelayValue, pshwRefState, pshwRcOut);
    }
    else
    {
        UCOM_MemFree(pshwNewMicIn);
        return UCOM_RET_ERR_STATE;
    }
    UCOM_MemFree(pshwNewMicIn);
    return UCOM_RET_SUCC;
}
#else
VOS_UINT32 MED_ANR_2MIC_TdSpaceFractionDelay(
                VOS_INT16                       *pshwMcIn,
                VOS_INT16                       *pshwRcIn,
                VOS_INT16                        shwDelayValue,
                VOS_INT16                       *pshwMainState,
                VOS_INT16                       *pshwRefState,
                VOS_INT16                       *pshwMcOut,
                VOS_INT16                       *pshwRcOut)
{
    VOS_INT16   shwMcStateLen;
    VOS_INT16   shwRcStateLen;
    VOS_INT16   shwFrameLen;
    VOS_INT16   shwIntegerDelayValue;
    VOS_INT16   shwCntM;
    VOS_INT32   swTemp;
    VOS_INT16  *pshwNewMicIn;
    pshwNewMicIn = (VOS_INT16 *)UCOM_MemAlloc((MED_2MIC_SPACESPLIT_MAX_DELAY + CODEC_FRAME_LENGTH_WB) \
                                               * sizeof(VOS_INT16));
    shwFrameLen      = MED_PP_GetFrameLength();
    shwMcStateLen   = MED_2MIC_SPACESPLIT_MAX_DELAY;
    shwMcStateLen   = CODEC_OpShl(shwMcStateLen, MED_PP_SHIFT_BY_8);
    shwRcStateLen   = MED_2MIC_SPACESPLIT_MAX_DELAY;
    shwRcStateLen   = CODEC_OpShl(shwRcStateLen, MED_PP_SHIFT_BY_8);
    if ((shwDelayValue > shwMcStateLen)
        || (shwDelayValue > shwRcStateLen))
    {
        UCOM_MemFree(pshwNewMicIn);
        return UCOM_RET_ERR_PARA;
    }
    if (0 == (shwDelayValue & 0xFF))
    {
        MED_ANR_2MIC_TdSpaceIntegerDelay(pshwMcIn, shwDelayValue, pshwMainState, pshwMcOut);
        UCOM_MemCpy(pshwRcOut, pshwRcIn, (VOS_UINT16)shwFrameLen * sizeof(VOS_INT16));
    }
    else if (128 == (shwDelayValue & 0xFF))
    {
        UCOM_MemSet(pshwNewMicIn,
                    0,
                    (MED_2MIC_SPACESPLIT_MAX_DELAY + CODEC_FRAME_LENGTH_WB)*sizeof(VOS_INT16));
        UCOM_MemCpy(pshwNewMicIn,
                    pshwMainState,
                    (MED_2MIC_SPACESPLIT_DELAYCOEF_LENGH - 1) * sizeof(VOS_INT16));
        UCOM_MemCpy(pshwNewMicIn + (MED_2MIC_SPACESPLIT_DELAYCOEF_LENGH - 1),
                    pshwMcIn,
                    (VOS_UINT16)shwFrameLen * sizeof(VOS_INT16));
        /* 分数延迟滤波 FIR (滤波器设计为延迟点数为7.5) */
        for (shwCntM = 0; shwCntM < shwFrameLen; shwCntM++)
        {
            swTemp = 0;

            /* 滤波 */
            swTemp = CODEC_OpVvMac(pshwNewMicIn + shwCntM, g_ashwDelayCoef, MED_2MIC_SPACESPLIT_DELAYCOEF_LENGH, 0);

            pshwMcOut[shwCntM] = CODEC_OpRound(swTemp);
        }

        /* 更新主麦克历史信息，本帧最后(滤波器长度-1)点，用于下一帧FIR滤波 */
        UCOM_MemCpy(pshwMainState,
                    pshwNewMicIn + shwFrameLen,
                    (MED_2MIC_SPACESPLIT_DELAYCOEF_LENGH - 1) * sizeof(VOS_INT16));

        /* 对辅麦克进行补偿 用于抵消主麦克7.5点延迟多余点(1920是由7.5 * 2^8计算得来) */
        shwIntegerDelayValue = CODEC_OpSub(1920, shwDelayValue);

        MED_ANR_2MIC_TdSpaceIntegerDelay(pshwRcIn, shwIntegerDelayValue, pshwRefState, pshwRcOut);

    }

    else
    {
        /* 释放内存 */
        UCOM_MemFree(pshwNewMicIn);

        return UCOM_RET_ERR_STATE;
    }

    /* 释放内存 */
    UCOM_MemFree(pshwNewMicIn);

    return UCOM_RET_SUCC;

}
#endif

VOS_UINT32 MED_ANR_2MIC_TdAncNormalMode(
                MED_2MIC_ANC_STRU                       *pstAnc,
                VOS_INT16                               *pshwMcIn,
                VOS_INT16                               *pshwRcIn,
                VOS_INT16                               *pshwCalib,
                VOS_INT16                               *pshwMcOut)
{
    MED_2MIC_VAD_STRU               *pstVad;
    MED_2MIC_LAF_STRU               *pstLaf;
    VOS_INT16                       *pshwMainDelayState;
    VOS_INT16                        shwMainDelayQ;
    VOS_INT16                       *pshwDelayedMc;
    CODEC_VAD_FLAG_ENUM_INT16     enVadFlag;
    VOS_UINT32                       uwSpaceRet;
    VOS_UINT32                       uwLafRet;

    /* 分配内存空间 */
    pshwDelayedMc = (VOS_INT16 *)UCOM_MemAlloc(CODEC_FRAME_LENGTH_WB * sizeof(VOS_INT16));

    /* 获取状态 */
    pstVad                  = &(pstAnc->stVad);
    pstLaf                  = &(pstAnc->stLaf);
    shwMainDelayQ           = pstAnc->shwMainDelayQ;
    pshwMainDelayState      = pstAnc->ashwMainDelayState;

    /* 获得延迟的主麦克信号 */
    uwSpaceRet  = MED_ANR_2MIC_TdSpaceIntegerDelay(pshwMcIn, shwMainDelayQ, pshwMainDelayState, pshwDelayedMc);

    /* VAD检测 */
    enVadFlag   = MED_ANR_2MIC_TdNoiseVadDetect(pstVad, pshwMcIn, pshwCalib);

    /* LAF */
    uwLafRet    = MED_ANR_2MIC_TdLaf(pstLaf, pshwDelayedMc, pshwRcIn, enVadFlag, pshwMcOut);

    if (  (UCOM_RET_SUCC == uwSpaceRet)
        &&(UCOM_RET_SUCC == uwLafRet))
    {
        /* 释放分配的内存 */
        UCOM_MemFree(pshwDelayedMc);

        return UCOM_RET_SUCC;
    }
    else
    {
        /* 释放分配的内存 */
        UCOM_MemFree(pshwDelayedMc);

        return UCOM_RET_FAIL;
    }

}
CODEC_VAD_FLAG_ENUM_INT16 MED_ANR_2MIC_TdNoiseVadDetect(
                MED_2MIC_VAD_STRU                       *pstVad,
                VOS_INT16                               *pshwMcIn,
                VOS_INT16                               *pshwRcIn)
{
    CODEC_SAMPLE_RATE_MODE_ENUM_INT32     enSampleRate;
    MED_2MIC_VAD_WB_STRU               *pstVadWb;
    VOS_INT16                           shwVadAlpha;
    VOS_INT16                           shwVadRatioThd;
    VOS_INT16                           shwPsPnRatioLast;
    VOS_INT16                           shwFrameLen;
    VOS_INT16                          *pshwMcXState;
    VOS_INT16                          *pshwMcYState;
    VOS_INT16                          *pshwRcXState;
    VOS_INT16                          *pshwRcYState;
    VOS_INT32                           swPsPower;
    VOS_INT32                           swPnPower;
    VOS_INT16                           shwTemp;
    VOS_INT16                           shwPsPnRatio;

    CODEC_VAD_FLAG_ENUM_INT16        enVadFlag;

    /* 获取当前采样率 */
    enSampleRate        = MED_2MIC_GetTdSampleRate();

    /* 获取当前帧长 */
    shwFrameLen         = MED_PP_GetFrameLength();

    /* 设置参数 */
    shwVadAlpha         = MED_2MIC_ANR_VAD_ALPHA;
    pstVadWb            = &(pstVad->stVadWb);
    shwVadRatioThd      = pstVad->shwVadRatioThd;
    shwPsPnRatioLast    = pstVad->shwPsPnRatio;

    /* 分别计算主辅麦克平均能量 */
    if (CODEC_SAMPLE_RATE_MODE_16000 == enSampleRate)
    {
        pshwMcXState      = pstVadWb->ashwMcXState;
        pshwMcYState      = pstVadWb->ashwMcYState;
        pshwRcXState      = pstVadWb->ashwRcXState;
        pshwRcYState      = pstVadWb->ashwRcYState;

        /* 计算主辅麦克信号的低频带能量 */
        MED_ANR_2MIC_TdIIRLowPass(pstVadWb, pshwMcIn, pshwMcXState, pshwMcYState, &swPsPower);

        MED_ANR_2MIC_TdIIRLowPass(pstVadWb, pshwRcIn, pshwRcXState, pshwRcYState, &swPnPower);
    }
    else
    {
        /* 计算主麦克信号的平均能量 */
        swPsPower = MED_ANR_2MIC_TdCalcMeanPower(pshwMcIn, shwFrameLen);

        /* 计算辅麦克信号的平均能量 */
        swPnPower = MED_ANR_2MIC_TdCalcMeanPower(pshwRcIn, shwFrameLen);
    }

    /* 计算主辅能量比值  (精度要求较高) */
    shwTemp         = CODEC_OpRound(CODEC_OpNormDiv_32(swPsPower, swPnPower, MED_PP_SHIFT_BY_28));
    shwTemp         = CODEC_OpMult(CODEC_OpSub(CODEC_OP_INT16_MAX, shwVadAlpha), shwTemp);
    shwPsPnRatio    = CODEC_OpAdd(CODEC_OpMult(shwVadAlpha, shwPsPnRatioLast), shwTemp);

    /* 根据主辅麦克能量比获得VAD标志 */
    if (shwPsPnRatio > shwVadRatioThd)
    {
        enVadFlag   = CODEC_VAD_FLAG_SPEECH;
    }
    else
    {
        enVadFlag   = CODEC_VAD_FLAG_NOISE;
    }

    /* 保存功率谱比值，用于下次计算 */
    pstVad->shwPsPnRatio  = shwPsPnRatio;

    return enVadFlag;

}


VOS_UINT32 MED_ANR_2MIC_TdIIRLowPass(
                MED_2MIC_VAD_WB_STRU                    *pstVadWb,
                VOS_INT16                               *pshwMcIn,
                VOS_INT16                               *pshwMcXState,
                VOS_INT16                               *pshwMcYState,
                VOS_INT32                               *pswPowerMean)
{
    VOS_INT16                       shwFilterOrder;
    VOS_INT16                      *pshwFilterNum;
    VOS_INT16                      *pshwFilterDen;
    VOS_INT16                      *pshwTmpMicX;
    VOS_INT16                      *pshwTmpMicY;
    VOS_INT16                      *pshwTmp;
    VOS_INT16                       shwFrameLength;
    VOS_INT16                       shwCntK;
    VOS_INT16                       shwSwSum;

    /* 分配内存空间 */
    pshwTmpMicX     = (VOS_INT16 *)UCOM_MemAlloc((MED_2MIC_ANC_VAD_FILTER_ORDER + CODEC_FRAME_LENGTH_WB) \
                                                 * sizeof(VOS_INT16));
    pshwTmpMicY     = (VOS_INT16 *)UCOM_MemAlloc((MED_2MIC_ANC_VAD_FILTER_ORDER + CODEC_FRAME_LENGTH_WB) \
                                                 * sizeof(VOS_INT16));

    /* 参数获取 */
    shwFilterOrder  = pstVadWb->shwFilterOrder;
    pshwFilterNum   = pstVadWb->pshwFilterNum;
    pshwFilterDen   = pstVadWb->pshwFilterDen;
    shwFrameLength  = MED_PP_GetFrameLength();

    /* 分配临时空间 */
    UCOM_MemCpy(pshwTmpMicX, pshwMcXState, MED_2MIC_ANC_VAD_FILTER_ORDER * sizeof(VOS_INT16));
    UCOM_MemCpy((pshwTmpMicX + MED_2MIC_ANC_VAD_FILTER_ORDER), pshwMcIn, CODEC_FRAME_LENGTH_WB * sizeof(VOS_INT16));

    UCOM_MemCpy(pshwTmpMicY, pshwMcYState, MED_2MIC_ANC_VAD_FILTER_ORDER * sizeof(VOS_INT16));
    UCOM_MemSet((pshwTmpMicY + MED_2MIC_ANC_VAD_FILTER_ORDER), 0, CODEC_FRAME_LENGTH_WB * sizeof(VOS_INT16));

    for (shwCntK = 0; shwCntK < shwFrameLength; shwCntK++)
    {
        shwSwSum = CODEC_OpMult(pshwFilterNum[0], pshwTmpMicX[shwCntK + shwFilterOrder]);

        shwSwSum = CODEC_OpAdd(shwSwSum, CODEC_OpMult(pshwFilterNum[1], pshwTmpMicX[(shwCntK + shwFilterOrder) - 1]));
        shwSwSum = CODEC_OpSub(shwSwSum, CODEC_OpMult(pshwFilterDen[1], pshwTmpMicY[(shwCntK + shwFilterOrder) - 1]));
        shwSwSum = CODEC_OpAdd(shwSwSum, CODEC_OpMult(pshwFilterNum[2], pshwTmpMicX[(shwCntK + shwFilterOrder) - 2]));
        shwSwSum = CODEC_OpSub(shwSwSum, CODEC_OpMult(pshwFilterDen[2], pshwTmpMicY[(shwCntK + shwFilterOrder) - 2]));
        shwSwSum = CODEC_OpAdd(shwSwSum, CODEC_OpMult(pshwFilterNum[3], pshwTmpMicX[(shwCntK + shwFilterOrder) - 3]));
        shwSwSum = CODEC_OpSub(shwSwSum, CODEC_OpMult(pshwFilterDen[3], pshwTmpMicY[(shwCntK + shwFilterOrder) - 3]));

        pshwTmpMicY[shwCntK + shwFilterOrder] = shwSwSum;
    }

    /* 计算当前帧信号的平均能量 */
    pshwTmp = pshwTmpMicY + shwFilterOrder;

    *pswPowerMean = MED_ANR_2MIC_TdCalcMeanPower(pshwTmp, shwFrameLength);

    /* 更新滤波器状态 */
    UCOM_MemCpy(pshwMcXState,
                pshwMcIn + (shwFrameLength - shwFilterOrder),
                ((VOS_UINT16)shwFilterOrder - 1) * sizeof(VOS_INT16));

    UCOM_MemCpy(pshwMcYState,
                pshwTmp + (shwFrameLength - shwFilterOrder),
                ((VOS_UINT16)shwFilterOrder - 1) * sizeof(VOS_INT16));

    /* 释放分配的内存 */
    UCOM_MemFree(pshwTmpMicX);
    UCOM_MemFree(pshwTmpMicY);

    return UCOM_RET_SUCC;

}




VOS_INT32 MED_ANR_2MIC_TdCalcMeanPower(
                VOS_INT16                       *pshwMicIn,
                VOS_INT16                        shwFrameLength)
{
    VOS_INT16                       shwCnt;
    VOS_INT32                       swTemp;
    VOS_INT32                       swPowerMean;

    /* 初始化 */
    swPowerMean = 0;

    /* 计算平方和 */
    for (shwCnt = 0; shwCnt < shwFrameLength; shwCnt++)
    {
        swTemp        = CODEC_OpL_mult0(pshwMicIn[shwCnt], pshwMicIn[shwCnt]);
        swPowerMean   = CODEC_OpL_add(swTemp, swPowerMean);
    }

    /* 计算平均值 */
    swPowerMean       = CODEC_OpNormDiv_32(swPowerMean, (VOS_INT32)(shwFrameLength), 0);
    swPowerMean       = CODEC_OpL_max(swPowerMean, MED_2MIC_MIN_POWER_VALUE);

    return swPowerMean;

}
VOS_UINT32 MED_ANR_2MIC_TdLaf(
                    MED_2MIC_LAF_STRU                       *pstLaf,
                    VOS_INT16                               *pshwMcIn,
                    VOS_INT16                               *pshwRcIn,
                    VOS_INT16                                shwVadFlag,
                    VOS_INT16                               *pshwMcOut)
{
    VOS_INT16               *pshwWeight;
    VOS_INT16               *pshwRcBuf;
    VOS_INT16               *pshwRefShortBuf = MED_NULL;
    VOS_INT16                shwN;
    VOS_INT16                shwStep;
    VOS_INT16                shwUpdateLen;
    VOS_INT32                swInShortPower;
    VOS_INT32                swInShortThd;
    VOS_INT16                shwAlpha;
    VOS_INT32                swMaxRefPower;
    VOS_INT32                swRefShortPower;
    VOS_INT16                shwFrameLen;
    VOS_INT16                shwIndex;
    VOS_INT16                shwCntI;
    VOS_INT16                shwCntK;
    VOS_INT16                shwTmpError = 0;
    VOS_INT16                shwTemp1;
    VOS_INT32                swTemp2;
    VOS_INT32                swTmp3;
    VOS_INT16                shwTmp4;
    VOS_INT32                swTempDiv;
    VOS_INT16                shwTmpIndex;
    VOS_INT32                swTmpPower;
    VOS_INT32               *pswRefPower;

    /* 内存分配 */
    pswRefPower     = (VOS_INT32 *)UCOM_MemAlloc((2*CODEC_FRAME_LENGTH_WB) * sizeof(VOS_INT32));
    UCOM_MemSet(pswRefPower, 0, (2*CODEC_FRAME_LENGTH_WB) * sizeof(VOS_INT32));

    /* 获取帧长 */
    shwFrameLen     = MED_PP_GetFrameLength();

    pshwWeight      = pstLaf->ashwWeight;                                       /* 滤波器系数 */
    pshwRcBuf       = pstLaf->ashwRcBuf;                                        /* 上一帧辅麦克信号 */
    shwN            = pstLaf->shwN;                                             /* 滤波器阶数 */
    shwStep         = pstLaf->shwStep;                                          /* 滤波器学习步长 */
    swInShortPower  = pstLaf->swInShortPower;                                   /* 上一帧短时能量 */
    shwAlpha        = pstLaf->shwAlpha;                                         /* 平滑系数 */
    swInShortThd    = pstLaf->swInShortThd;                                     /* 短时能量阈值 */

    /* 缓存辅麦克信号的2帧数据，便于后面进行LAF滤波(这里存在冗余) */
    UCOM_MemCpy(pshwRcBuf, pshwRcBuf + shwFrameLen, (VOS_UINT16)shwFrameLen * sizeof(VOS_INT16));
    UCOM_MemCpy(pshwRcBuf + shwFrameLen, pshwRcIn, (VOS_UINT16)shwFrameLen * sizeof(VOS_INT16));

    /* 学习步长更新 */
    swMaxRefPower   = pstLaf->swRefPowerThd;

    /*计算辅麦克信号的平均能量*/
    swRefShortPower = MED_ANR_2MIC_TdCalcMeanPower(pshwRcIn, shwFrameLen);

    /* 辅麦克能量越大，更新越快 */
    if (swRefShortPower <= swMaxRefPower)
    {
        shwTemp1 = CODEC_OpRound(CODEC_OpNormDiv_32(swRefShortPower,
                                swMaxRefPower, MED_PP_SHIFT_BY_31));
        shwStep = CODEC_OpMult(shwStep, shwTemp1);
    }

    /* 计算短时平均能量 */
    swTmpPower  = MED_ANR_2MIC_TdCalcMeanPower(pshwMcIn, shwFrameLen);

    /* 主麦克短时能量进行平滑 */
    swTemp2 = CODEC_OpL_mpy_32_16(swInShortPower, CODEC_OpSub(CODEC_OP_INT16_MAX, shwAlpha));
    swInShortPower = CODEC_OpL_add(swTemp2, CODEC_OpL_mpy_32_16(swTmpPower, shwAlpha));

    /* 缓存辅麦克信号的2帧数据的能量(这里存在内存冗余) */
    for (shwCntK = 0; shwCntK < (2 * shwFrameLen); shwCntK++)
    {
        pswRefPower[shwCntK] = CODEC_OpL_mult0(pshwRcBuf[shwCntK] , pshwRcBuf[shwCntK]);
    }

    shwUpdateLen = (VOS_INT16)MED_2MIC_LAF_WEIGHT_UPDATE_LEN;

    pshwRefShortBuf = pshwRcBuf;

    for (shwIndex = 0; shwIndex < (shwFrameLen / shwUpdateLen); shwIndex++)
    {
        for (shwCntI = 0; shwCntI < shwUpdateLen; shwCntI++)
        {
            shwTmpIndex = (VOS_INT16)(shwIndex * shwUpdateLen) + shwCntI;
            pshwRefShortBuf = (((pshwRcBuf + shwTmpIndex) + shwFrameLen) - shwN) + 1;

            if (pstLaf->shwMaxFar < pstLaf->shwFarMaxThd )
            {
                /* LAF滤波 */
                shwTemp1 = CODEC_OpRound(CODEC_OpVvMacD(pshwWeight, (pshwRefShortBuf+ shwN) -1, (VOS_INT32)shwN, 0));
            }
            else
            {
                shwTemp1 = 0;

            }

            shwTmpError = CODEC_OpSub(pshwMcIn[shwTmpIndex], shwTemp1);
            pshwMcOut[shwTmpIndex] = shwTmpError;
        }

#ifndef _MED_MAX_CYCLE_
        if ((0 == shwVadFlag) && (swInShortPower > swInShortThd))
#else
        if(1)
#endif
        {
            /* LAF滤波系数更新 */
            swTmpPower = 0;

            for (shwCntK = 0; shwCntK < shwN; shwCntK++)
            {
                shwCntI    = ((VOS_INT16)(shwIndex * shwUpdateLen) + (shwFrameLen - shwN))+ shwCntK;

                swTmpPower = CODEC_OpL_add(swTmpPower, pswRefPower[shwCntI]);
            }

            swTmpPower = CODEC_OpL_max(swTmpPower , MED_2MIC_MIN_POWER_VALUE);

            swTmp3     = CODEC_OpL_abs((VOS_INT32)shwTmpError);

            swTempDiv  = CODEC_OpNormDiv_32(swTmp3,
                                       swTmpPower,
                                       (VOS_INT16)MED_PP_SHIFT_BY_30);

            if (shwTmpError < 0)
            {
                swTempDiv = CODEC_OpL_negate(swTempDiv);
            }

            for (shwCntI = 0; shwCntI < shwN; shwCntI++)
            {

                swTmp3      = CODEC_OpL_mpy_32_16(swTempDiv , pshwRefShortBuf[(shwN - shwCntI) - 1]);
                shwTmp4     = CODEC_OpSaturate(swTmp3);
                swTmp3      = CODEC_OpL_mult(shwTmp4, shwStep);
                shwTmp4     = CODEC_OpRound(swTmp3);
                pshwWeight[shwCntI] = CODEC_OpAdd(pshwWeight[shwCntI], shwTmp4);
            }

         }
    }

    /* 更新状态 */
    pstLaf->swInShortPower = swInShortPower;

    /* 内存释放 */
    UCOM_MemFree(pswRefPower);

    return UCOM_RET_SUCC;

}
#if 0

VOS_INT16 MED_ANR_2MIC_FirConvolute(
                VOS_INT16               *pshwWeight,
                VOS_INT16               *pshwRcIn,
                VOS_INT16                shwLen )
{
    VOS_INT32               swSum = 0;
    VOS_INT16               shwCntI;
    VOS_INT16               shwOut;

    /* 滤波 */
    swSum = CODEC_OpVvMacD(pshwWeight, pshwRcIn+shwLen-1, (VOS_INT32)shwLen, 0);
    /*
    for (shwCntI = 0; shwCntI < shwLen; shwCntI++)
    {
       swSum = CODEC_OpL_add(CODEC_OpL_mult(pshwWeight[shwCntI],
                                        pshwRcIn[(shwLen - shwCntI) - 1]), swSum);

    }
    */

    shwOut = CODEC_OpRound(swSum);

    shwOut  = CODEC_OpRound(CODEC_OpVvMacD(pshwWeight, pshwRcIn+shwLen-1, (VOS_INT32)shwLen, 0));

    return shwOut;

}
#endif


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

