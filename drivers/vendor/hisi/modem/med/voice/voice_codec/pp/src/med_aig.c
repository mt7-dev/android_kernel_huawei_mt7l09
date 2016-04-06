

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "med_aig.h"
#include "med_pp_comm.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
MED_AIG_OBJ_STRU    g_astMedAigObjPool[MED_AIG_MAX_OBJ_NUM];                    /* AGC实体资源池 */
MED_OBJ_INFO        g_stMedAigObjInfo;                                          /* AGC信息 */



/*****************************************************************************
  3 函数实现
*****************************************************************************/

VOS_VOID* MED_AIG_Create(VOS_VOID)
{
    return (MED_PP_Create(
                    MED_AIG_GetObjInfoPtr,
                    MED_AIG_GetObjPtr,
                    MED_AIG_MAX_OBJ_NUM,
                    sizeof(MED_AIG_OBJ_STRU)));
}


VOS_UINT32 MED_AIG_Destroy(VOS_VOID **ppstObj)
{
    VOS_UINT32              uwRet;
    MED_AIG_OBJ_STRU       *pstObj;

    pstObj      = (MED_AIG_OBJ_STRU *)(*ppstObj);

    /* 判断入参是否合法 */
    uwRet       = MED_PP_CheckPtrValid(MED_AIG_GetObjInfoPtr, pstObj);

    /* 入参合法，则将指针对象释放，并将指针赋空 */
    if (UCOM_RET_SUCC == uwRet)
    {
        UCOM_MemSet(pstObj, 0, sizeof(MED_AIG_OBJ_STRU));
        pstObj->enIsUsed = CODEC_OBJ_UNUSED;
        *ppstObj = MED_NULL;
    }

    return uwRet;
}


VOS_UINT32 MED_AIG_GetPara(
                VOS_VOID               *pstInstance,
                VOS_INT16              *pshwParam,
                VOS_INT16               shwParaLen)
{
    VOS_UINT32              uwRet;
    MED_AIG_OBJ_STRU       *pstObj    = (MED_AIG_OBJ_STRU *)pstInstance;
    MED_AIG_NV_STRU        *pstNv     = &(pstObj->stNv);

    /* 判断入参是否合法 */
    uwRet       = MED_PP_CheckPtrValid(MED_AIG_GetObjInfoPtr, pstObj);

    /* 入参合法，则尝试对指针所在位置进行配置项读取操作 */
    if (UCOM_RET_SUCC == uwRet)
    {
        /* BUFFER空间不够，或结构体的使能项不正确，则出错中止操作 */
        if (    (shwParaLen < MED_AIG_PARAM_LEN)
            ||  (pstNv->enEnable >= CODEC_SWITCH_BUTT))
        {
            uwRet = UCOM_RET_FAIL;
        }
        else
        {
            /* 获取NV信息 */
            CODEC_OpVecSet(pshwParam, shwParaLen, 0);
            CODEC_OpVecCpy(pshwParam, (VOS_INT16 *)pstNv, MED_AIG_PARAM_LEN);
        }
    }

    return uwRet;


}


VOS_UINT32 MED_AIG_SetPara (
                VOS_VOID               *pstInstance,
                VOS_INT16              *pshwParam,
                VOS_INT16               shwParaLen,
                VOS_INT32               swSampleRate)
{
    VOS_UINT32              uwRet;
    MED_AIG_OBJ_STRU       *pstObj    = (MED_AIG_OBJ_STRU *)pstInstance;
    MED_AIG_NV_STRU        *pstNv     = &(pstObj->stNv);

    /* 判断入参是否合法 */
    uwRet       = MED_PP_CheckPtrValid(MED_AIG_GetObjInfoPtr, pstObj);

    /* 入参合法，则尝试对指针所在位置进行配置项写入操作 */
    if (UCOM_RET_SUCC == uwRet)
    {
        /* BUFFER空间不够，或入参的使能项不正确，则出错中止操作 */
        if (    (shwParaLen < MED_AIG_PARAM_LEN)
            ||  (pshwParam[0] >= CODEC_SWITCH_BUTT))
        {
            uwRet = UCOM_RET_FAIL;
        }
        else
        {
            /* 获取NV信息 */
            CODEC_OpVecCpy((VOS_INT16 *)pstNv, pshwParam, MED_AIG_PARAM_LEN);

            if (MED_SAMPLE_8K == swSampleRate)
            {
                pstObj->shwFrmLen = MED_FRM_LEN_8K;
            }
            else if (MED_SAMPLE_16K == swSampleRate)
            {
                pstObj->shwFrmLen = MED_FRM_LEN_16K;
            }
            else
            {
                pstObj->shwFrmLen = MED_FRM_LEN_48K;
            }

            pstObj->shwGainMaxRefThd    = 10*256;
            pstObj->shwGainMax          = 10*256;
            pstObj->swdBfs0             = MED_AIG_RX_0_DBFS;

            pstObj->shwMaxIn            = pstNv->shwMaxIn;
            pstObj->shwSpeechCnt        = pstNv->shwSpeechCntInit;

            pstObj->shwLastPowerIndBfs  = pstNv->shwLastPowerIndBfsInit;
            pstObj->shwLastGain         = pstNv->shwLastGainInit;
            pstObj->shwLastPowerOutdBfs = pstNv->shwLastPowerOutdBfsInit;
            pstObj->shwMakeupGain       = pstNv->shwMakeupGain;

        }
    }

    return uwRet;

}


VOS_UINT32 MED_AIG_Main(
                 MED_AIG_OBJ_STRU    *pstAig,
                 VOS_INT16           *pshwIn,
                 VOS_INT16           *pshwRef,
                 VOS_INT16           *pshwOut,
                 VOS_INT16            shwVadFlag)
{
    VOS_INT16                         shwCnt;
    VOS_INT16                         shwFrameLen;
    VOS_INT16                         shwSpeechCnt;

    VOS_INT16                         shwPowerRMS;
    VOS_INT16                         shwPowerRMSRef;
    VOS_INT16                         shwNormShift;
    VOS_INT16                         shwNormShiftRef;

    VOS_INT16                         shwGainAIG;
    VOS_INT16                         shwGainAIGtemp;
    VOS_INT16                         shwAlpha;
    VOS_INT16                         shwGainLast;

    VOS_INT32                         swGaintemp1;
    VOS_INT32                         swGaintemp2;
    VOS_INT32                         swGaintemp3;

    MED_AIG_NV_STRU                  *pstNv;

    /* 参数读取 */
    pstNv       = &(pstAig->stNv);
    shwFrameLen = pstAig->shwFrmLen;


    /* VAD hangover过程处理 */
    shwSpeechCnt = pstAig->shwSpeechCnt;

    if(shwVadFlag == 1)
    {
        shwSpeechCnt = pstNv->shwVADholdlenth;
    }
    else
    {
        shwSpeechCnt = shwSpeechCnt - 1;
        shwSpeechCnt = CODEC_OpMax(0, shwSpeechCnt);
    }

    pstAig->shwSpeechCnt = shwSpeechCnt;

    if ((CODEC_SWITCH_OFF == pstNv->enEnable)
     || (pstAig->shwSpeechCnt <= 0))
    {
        CODEC_OpVecCpy(pshwOut, pshwIn, shwFrameLen);

        return UCOM_RET_SUCC;
    }

    pstAig->shwMaxIn = CODEC_OpVecMaxAbs(pshwIn, shwFrameLen, MED_NULL);

    MED_AIG_PowerCal(pshwIn, &shwPowerRMS, &shwNormShift, shwFrameLen);

    MED_AIG_PowerCal(pshwRef, &shwPowerRMSRef, &shwNormShiftRef, shwFrameLen);

    MED_AIG_GainCal(pstAig,
                    shwPowerRMS,
                    shwNormShift-3,
                    shwPowerRMSRef,
                    shwNormShiftRef-3,
                    &shwGainAIG,
                    &shwAlpha);

    shwGainLast = pstAig->shwLastGain;

    for (shwCnt = 0; shwCnt < shwFrameLen; shwCnt++)
    {
        swGaintemp1     = CODEC_OpL_mult0(shwAlpha,shwGainAIG);
        swGaintemp2     = CODEC_OpL_mult0(CODEC_OpSub(MED_AIG_ALPHA_NORM,shwAlpha), shwGainLast);
        swGaintemp3     = CODEC_OpL_add(swGaintemp1,swGaintemp2);
        swGaintemp3     = CODEC_OpL_shl(swGaintemp3, -7);
        shwGainAIGtemp  = CODEC_OpSaturate(swGaintemp3);
        pshwOut[shwCnt] = CODEC_OpSaturate(CODEC_OpL_mpy_32_16(CODEC_OpL_shl(shwGainAIGtemp,5),pshwIn[shwCnt]));
        shwGainLast     = shwGainAIGtemp;
    }

    pstAig->shwLastGain = shwGainLast;

    return UCOM_RET_SUCC;

}


VOS_VOID MED_AIG_PowerCal(
                VOS_INT16               *pshwIn,
                VOS_INT16               *pshwPowerRMS,
                VOS_INT16               *pshwNormShift,
                VOS_INT16                shwFrameLen)
{
    VOS_INT16                            shwCntI;
    VOS_INT16                            shwInMax;
    VOS_INT16                            shwPowerRMS;
    VOS_INT16                            shwNormShift;
    VOS_INT16                           *pshwInNorm;

    VOS_INT32                            swPowerAIG;

    /* 内存分配 */
    pshwInNorm = (VOS_INT16 *)UCOM_MemAlloc((VOS_UINT16)shwFrameLen * sizeof(VOS_INT16));

    /* 归一化值计算 */
    shwInMax = CODEC_OpVecMaxAbs(pshwIn, shwFrameLen, MED_NULL);

    shwNormShift = CODEC_OpNorm_s(shwInMax);

    /* 输入信号归一化 */
    for (shwCntI = 0; shwCntI < shwFrameLen; shwCntI++)
    {
        pshwInNorm[shwCntI] = CODEC_OpShl(pshwIn[shwCntI], shwNormShift - 3);
    }

    /* 平均能量计算 */
    swPowerAIG = CODEC_OpVvMac(pshwInNorm, pshwInNorm, shwFrameLen, 0);

    swPowerAIG = CODEC_OpL_shr(swPowerAIG, 1);

    swPowerAIG = CODEC_OpNormDiv_32(swPowerAIG, shwFrameLen, 0);

    /* RMS计算 */
    shwPowerRMS = CODEC_OpSqrt(swPowerAIG);

    /* RMS和shwNormShift输出*/
    *pshwPowerRMS  = shwPowerRMS;
    *pshwNormShift = shwNormShift;

    /* 释放内存 */
    UCOM_MemFree(pshwInNorm);

}
VOS_VOID MED_AIG_GainCal(
                MED_AIG_OBJ_STRU        *pstAig,
                VOS_INT16                shwPowerRMS,
                VOS_INT16                shwNormShift,
                VOS_INT16                shwPowerRMSRef,
                VOS_INT16                shwNormShiftRef,
                VOS_INT16               *pshwGainAIG,
                VOS_INT16               *pshwAlpha)
{
    MED_AIG_NV_STRU                     *pstNv;

    VOS_INT16                            shwDnCompThr;
    VOS_INT16                            shwUpCompThr;
    VOS_INT16                            shwDnExpThr;
    VOS_INT16                            shwDnCompSlop;
    VOS_INT16                            shwUpCompSlop;
    VOS_INT16                            shwDnExpSlop;
    VOS_INT16                            shwLastPowerIndBfs;
    VOS_INT16                            shwAlphaThd;
    VOS_INT16                            shwGainAlphaattack;
    VOS_INT16                            shwLastPowerOutdBfs;
    VOS_INT16                            shwGainAlpharelease;
    VOS_INT16                            shwExpectMax;
    VOS_INT16                            shwGainMindB;
    VOS_INT16                            shwGainMaxRefThd;


    VOS_INT16                            shwLogshift;
    VOS_INT16                            shwPowerInDB;
    VOS_INT16                            shwPowerIn0dBfs;
    VOS_INT16                            shwPowerIndBfs;
    VOS_INT16                            shwPowerIndBfstemp;

    VOS_INT16                            shwLogshiftref;
    VOS_INT16                            shwPowerInDBref;
    VOS_INT16                            shwPowerIndBfsref;
    VOS_INT16                            shwPowerIndBfsreftemp;

    VOS_INT32                            swPowerIn;
    VOS_INT32                            swPowerInref;
    VOS_INT32                            swdBfs0;

    VOS_INT32                            swPowerouttemp1;
    VOS_INT32                            swPowerouttemp2;
    VOS_INT32                            swPowerouttemp3;
    VOS_INT32                            swPowerouttemp4;
    VOS_INT16                            shwPowerouttemp5;
    VOS_INT32                            swPowerouttemp6;

    VOS_INT32                            swMindB;
    VOS_INT32                            swMaxIn;

    VOS_INT16                            shwPoweroutdBfs;
    VOS_INT16                            shwPoweroutmin;
    VOS_INT16                            shwMaxIn;
    VOS_INT16                            shwLogshiftmax;
    VOS_INT16                            shwMaxInDB;
    VOS_INT16                            shwExpectMaxDB;
    VOS_INT16                            shwMaxAmGaindB;
    VOS_INT16                            shwMaxAmGaindBtemp;

    VOS_INT32                            swGaintemp;
    VOS_INT32                            swInd;

    VOS_INT16                            shwGain;
    VOS_INT16                            shwLastGaindB;
    VOS_INT16                            shwMaxGainDelta;
    VOS_INT16                            shwGainMax;
    VOS_INT16                            shwGainMaxRef;
    VOS_INT16                            shwGainLinear;
    VOS_INT16                            shwMakeupGain;

    /* NV参数读取 */
    pstNv               = &(pstAig->stNv);
    shwDnCompThr        = pstNv->shwDnCompThr;
    shwUpCompThr        = pstNv->shwUpCompThr;
    shwDnExpThr         = pstNv->shwDnExpThr;
    shwDnCompSlop       = pstNv->shwDnCompSlop;
    shwUpCompSlop       = pstNv->shwUpCompSlop;
    shwDnExpSlop        = pstNv->shwDnExpSlop;
    swMindB             = (VOS_INT32)(pstNv->shwMindB);
    shwAlphaThd         = pstNv->shwAlphaThd;
    shwGainAlphaattack  = pstNv->shwGainAlphaattack;
    shwGainAlpharelease = pstNv->shwGainAlpharelease;
    shwExpectMax        = pstNv->shwExpectMax;
    shwGainMindB        = pstNv->shwGainMindB;
    shwMaxGainDelta     = pstNv->shwMaxGainDelta;

    /* 非NV参数读取 */
    swdBfs0             = pstAig->swdBfs0;
    shwMaxIn            = pstAig->shwMaxIn;
    shwLastPowerIndBfs  = pstAig->shwLastPowerIndBfs;
    shwLastGaindB       = pstAig->shwLastGaindB;
    shwLastPowerOutdBfs = pstAig->shwLastPowerOutdBfs;
    shwGainMax          = pstAig->shwGainMax;
    shwGainMaxRefThd    = pstAig->shwGainMaxRefThd;
    shwMakeupGain       = pstAig->shwMakeupGain;

    /* 输入功率RMS转化成dB值 */
    swPowerIn             = CODEC_OpL_Comp(0, shwPowerRMS);
    swPowerIn             = CODEC_OpL_shl(swPowerIn, 2);
    shwLogshift           = shwNormShift + 1 + 2 ;
    shwPowerInDB          = CODEC_OpLog10(shwLogshift, swPowerIn);
    shwPowerIn0dBfs       = CODEC_OpLog10(0, swdBfs0);
    shwPowerIndBfs        = CODEC_OpSub(shwPowerInDB, shwPowerIn0dBfs);
    shwPowerIndBfstemp    = CODEC_OpShl(shwPowerIndBfs, 2);
    shwPowerIndBfs        = CODEC_OpAdd(shwPowerIndBfs, shwPowerIndBfstemp);

    /* 输入REF功率RMS转化成dB值 */
    swPowerInref          = CODEC_OpL_Comp(0, shwPowerRMSRef);
    swPowerInref          = CODEC_OpL_shl(swPowerInref, 2);
    shwLogshiftref        = shwNormShiftRef + 1 + 2 ;
    shwPowerInDBref       = CODEC_OpLog10(shwLogshiftref, swPowerInref);
    shwPowerIn0dBfs       = CODEC_OpLog10(0, swdBfs0);
    shwPowerIndBfsref     = CODEC_OpSub(shwPowerInDBref, shwPowerIn0dBfs);
    shwPowerIndBfsreftemp = CODEC_OpShl(shwPowerIndBfsref, 2);
    shwPowerIndBfsref     = CODEC_OpAdd(shwPowerIndBfsref, shwPowerIndBfsreftemp);

    /* 根据输入功率RMS选择在DRC的哪个区间进行处理，输出增益的dB值及alpha滤波系数 */
    if (shwPowerIndBfs >= shwDnCompThr)
    {
        swPowerouttemp1   = (CODEC_OpL_mult0(shwDnCompSlop, shwPowerIndBfs));
        swPowerouttemp2   = (CODEC_OpL_mult0(CODEC_OpSub(MED_AIG_SLOP_NORM, shwDnCompSlop), shwDnCompThr));
        swPowerouttemp3   = CODEC_OpL_add(swPowerouttemp1,swPowerouttemp2);
        swPowerouttemp3   = CODEC_OpL_shl(swPowerouttemp3,-7);
        shwPoweroutdBfs   = CODEC_OpSaturate(swPowerouttemp3);
        shwGain           = CODEC_OpSub(shwPoweroutdBfs,shwPowerIndBfs);
    }
    else if (shwPowerIndBfs >= shwUpCompThr)
    {
        shwGain = 0;
    }
    else if (shwPowerIndBfs >= shwDnExpThr)
    {
        swPowerouttemp1   = CODEC_OpL_mult0(shwUpCompSlop, shwPowerIndBfs);
        swPowerouttemp2   = CODEC_OpL_mult0(CODEC_OpSub(MED_AIG_SLOP_NORM, shwUpCompSlop), shwUpCompThr);
        swPowerouttemp3   = CODEC_OpL_add(swPowerouttemp1, swPowerouttemp2);
        swPowerouttemp3   = CODEC_OpL_shl(swPowerouttemp3, -7);
        shwPoweroutdBfs   = CODEC_OpSaturate(swPowerouttemp3);

        shwGain = CODEC_OpSub(shwPoweroutdBfs, shwPowerIndBfs);
    }
    else
    {
        swPowerouttemp1   = CODEC_OpL_mult0(shwUpCompSlop, shwDnExpThr);
        swPowerouttemp2   = CODEC_OpL_mult0(CODEC_OpSub(MED_AIG_SLOP_NORM, shwUpCompSlop), shwUpCompThr);
        swPowerouttemp3   = CODEC_OpL_add(swPowerouttemp1, swPowerouttemp2);
        swPowerouttemp3   = CODEC_OpL_shl(swPowerouttemp3, -7);

        shwPoweroutmin    = CODEC_OpSub(shwDnExpThr,
                                      CODEC_OpSaturate(CODEC_OpNormDiv_32(CODEC_OpL_shl(CODEC_OpL_sub(swPowerouttemp3, swMindB),
                                                                                  7),
                                                                      shwDnExpSlop,
                                                                      0)));

        if (shwPowerIndBfs >= shwPoweroutmin)
        {
            shwPowerouttemp5 = CODEC_OpSub(shwDnExpThr, shwPowerIndBfs);
            swPowerouttemp4  = CODEC_OpL_mult0(shwPowerouttemp5, shwDnExpSlop);
            swPowerouttemp4  = CODEC_OpL_shl(swPowerouttemp4, -7);
            swPowerouttemp6  = CODEC_OpL_sub(swPowerouttemp3, swPowerouttemp4);

            shwPoweroutdBfs  = CODEC_OpSaturate(swPowerouttemp6);
        }
        else
        {
            shwPoweroutdBfs = CODEC_OpSaturate(swMindB);
        }

        shwGain = CODEC_OpSub(shwPoweroutdBfs,shwPowerIndBfs);

    }

    /* 如果当前能量比前一帧能量大同时 前一帧能量较小，则快速滤波 */
    if (((shwPowerIndBfs - shwLastPowerIndBfs) >= shwAlphaThd)
     && (shwLastPowerIndBfs < (-40*256)))
    {
        *pshwAlpha = shwGainAlphaattack;
    }
    else if (((shwDnCompThr - (3*256))  - shwLastPowerOutdBfs) <= (shwPowerIndBfs - shwLastPowerIndBfs))
    {
        /* 如果当前帧能量比前一帧能量的增量 大于前一帧处理的输出量与下压缩门限的差 （余量3dB） 则快速滤波  */
        *pshwAlpha = shwGainAlphaattack;
    }
    else
    {
        /* 其它场景慢滤波 */
        *pshwAlpha = shwGainAlpharelease;
    }

    /* 根据期望最大值和当前帧最大值的差计算当前能施加的最大增益 */
    swMaxIn            = CODEC_OpL_Comp(0,shwMaxIn);
    swMaxIn            = CODEC_OpL_shl(swMaxIn,2);
    shwLogshiftmax     = 3;
    shwMaxInDB         = CODEC_OpLog10(shwLogshiftmax, swMaxIn);
    shwExpectMaxDB     = CODEC_OpLog10(0, shwExpectMax);
    shwMaxAmGaindB     = CODEC_OpSub(shwExpectMaxDB, shwMaxInDB);
    shwMaxAmGaindBtemp = CODEC_OpShl(shwMaxAmGaindB, 2);
    shwMaxAmGaindB     = CODEC_OpAdd(shwMaxAmGaindB, shwMaxAmGaindBtemp);

    /* 增益下限保护 */
    shwGain            = CODEC_OpMax(shwGain, shwGainMindB);

    /* 快速滤波场景不予考虑 */
    if (((shwPowerIndBfs - shwLastPowerIndBfs) >= shwAlphaThd)
     && (shwLastPowerIndBfs < (-40*256)))
    {

    }
    else
    {
        shwGain = CODEC_OpMax(shwGain, shwLastGaindB - shwMaxGainDelta);
        shwGain = CODEC_OpMin(shwGain, shwLastGaindB + shwMaxGainDelta);
    }

    /* 增益不能超过幅度计算得到的最大增益  */
    shwGain = CODEC_OpMin(shwGain, shwMaxAmGaindB);

    /* 保存历史值 */
    pstAig->shwLastPowerIndBfs  = shwPowerIndBfs;
    pstAig->shwLastGaindB       = shwGain;
    pstAig->shwLastPowerOutdBfs = shwGain + shwPowerIndBfs;

    /* 增益不能超过幅度计算得到的最大增益 */
    shwGain       = CODEC_OpMin(shwGainMax, shwGain);

    /* 增益不能超过MBDRC处理之前的信号能量的变化上限 但是不作为历史值保存 */
    shwGainMaxRef = CODEC_OpSub( CODEC_OpAdd(shwPowerIndBfsref, shwGainMaxRefThd), shwPowerIndBfs);

    shwGain = CODEC_OpMin(shwGainMaxRef, shwGain);

    /* 把经过alpha滤波的增益dB值转化成线性值 */
    shwGain       = CODEC_OpAdd(shwGain, shwMakeupGain);
    shwGain       = CODEC_OpSub(shwGain, MED_AIG_GAIN_30DB_NORM);                 /* 此处以＋30dB作为归一化输出 */
    swGaintemp   = CODEC_OpL_Comp(shwGain,0);
    swInd         = CODEC_OpL_shl(CODEC_OpL_mpy_32_16(swGaintemp, 1638), 2);
    swInd         = CODEC_OpL_min(-1, swInd);
    shwGainLinear = CODEC_OpExtract_h(CODEC_OpExp10(swInd));

    /* AIG增益输出 */
    *pshwGainAIG  = shwGainLinear;

}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

