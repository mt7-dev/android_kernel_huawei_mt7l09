

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include  "med_eanr.h"
#include  "med_pp_comm.h"
#include  "om.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
MED_EANR_STRU                            g_astMedEanrObj[MED_EANR_MAX_OBJ_NUM];     /* ANR模块全局控制实体 */
MED_OBJ_INFO                             g_stMedEanrObjInfo;                      /* ANR信息 */

/*信噪比最小值查找表*/
MED_INT16 PriSNR_Min_Tab[51] =
{
    128, 102, 81, 64, 51, 40, 32, 26, 20, 16,
    13 , 10 , 8 , 6 , 5 , 4 , 3 , 3 , 2 , 2 ,
    1  , 1  , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 ,
    1  , 1  , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 ,
    1  , 1  , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 ,
    1
};

/*增益最小值查找表*/
MED_INT16 PriSNRSqrt_Min_Tab[51] =
{
    32767, 29205, 26029, 23198, 20675, 18427, 16423, 14637, 13045, 11627,
    10362, 9235 , 8231 , 7336 , 6538 , 5827 , 5193 , 4629 , 4125 , 3677 ,
    3277 , 2920 , 2603 , 2320 , 2068 , 1843 , 1642 , 1464 , 1305 , 1163 ,
    1036 , 924  , 823  , 734  , 654  , 583  , 519  , 463  , 413  , 368  ,
    328  , 292  , 260  , 232  , 207  , 184  , 164  , 146  , 130  , 116  ,
    104
};

/*先验信噪比Global平滑窗系数*/
MED_INT16 MED_EANR_SNR_WIN_GLB[31] =
{
    20  , 78  , 173 , 300 , 455 , 632 , 824 , 1024, 1224, 1416,
    1593, 1748, 1875, 1970, 2028, 2048, 2028, 1970, 1875, 1748,
    1593, 1416, 1224, 1024, 824 , 632 , 455 , 300 , 173 , 78  ,
    20

};

/*子带下限索引*/
MED_INT16 MED_EANR_SUBBAND_FL[40] =
{
    0  ,2  ,3  ,4  ,5  ,6  ,7  ,8  ,9  ,10 ,11 ,12 ,13 ,14 ,15 ,16 ,17 ,18 ,20 ,24 ,
    29 ,34 ,40 ,47 ,54 ,60 ,67 ,75 ,84 ,93 ,102,115,
    128,141,153,168,183,200,219,237
};

/*子带上限索引*/
MED_INT16 MED_EANR_SUBBAND_FH[40] =
{
    1  ,2  ,3  ,4  ,5  ,6  ,7  ,8  ,9  ,10 ,11 ,12 ,13 ,14 ,15 ,16 ,17 ,19 ,23 ,28 ,
    33 ,39 ,46 ,53 ,59 ,66 ,74 ,83 ,92 ,101,114,127,
    140,152,167,182,199,218,236,255
};


/*****************************************************************************
  3 函数实现
*****************************************************************************/

VOS_VOID* MED_EANR_1MIC_Create( VOS_VOID )
{
    /* 创建单麦克降噪实体*/
    return (MED_PP_Create(
                    MED_EANR_GetObjInfoPtr(),
                    MED_EANR_GetObjPtr(),
                    MED_EANR_MAX_OBJ_NUM,
                    sizeof(MED_EANR_STRU)));
}
VOS_UINT32 MED_EANR_1MIC_SetPara(
                VOS_VOID                          *pInstance,
                VOS_INT16                         *pshwPara,
                VOS_INT16                          shwParaLen,
                CODEC_SAMPLE_RATE_MODE_ENUM_INT32  enSampleRate)
{
    VOS_UINT32           uwRet;
    MED_INT16            shwTmp;
    MED_INT16            shwI;

    MED_EANR_STRU       *pstObj = (MED_EANR_STRU *)pInstance;                    /* 临时ANR指针 */;

    /* 判断入参是否合法 */
    uwRet         = MED_PP_CheckPtrValid(MED_EANR_GetObjInfoPtr(), pstObj);

    /* 入参合法，则尝试对指针所在位置进行配置项读取操作 */
    if (UCOM_RET_SUCC == uwRet)
    {
        /* BUFFER空间不够，或结构体的使能项不正确，则出错中止操作 */
        if (   (shwParaLen  < CODEC_NV_EANR_1MIC_LEN)
            || (pshwPara[0] >= CODEC_SWITCH_BUTT))
        {
            uwRet = UCOM_RET_FAIL;
        }
        else
        {
            /* ANR实体状态清零 */
            UCOM_MemSet(pstObj, 0, sizeof(MED_EANR_STRU));

            pstObj->enIsUsed = CODEC_OBJ_USED;

            pstObj->shwEnable           = pshwPara[0];                          /*EANR使能控制，0为关闭，1为打开*/
            pstObj->shwReduceDB         = pshwPara[1];                          /*范围为-50~0*/
            pstObj->shwZeroFlagTh       = pshwPara[2];                          /*全0帧判决门限*/

            shwTmp                      = CODEC_OpNegate(pstObj->shwReduceDB);

            switch (enSampleRate)
            {
                case CODEC_SAMPLE_RATE_MODE_8000:
                {
                    pstObj->stCommon.shwSubBandNum   = 32;
                    pstObj->stCommon.shwOpertBandNum = 33;                               /*窄带语音子带数*/

                    break;
                }

                case CODEC_SAMPLE_RATE_MODE_16000:
                {
                    pstObj->stCommon.shwSubBandNum   = 40;
                    pstObj->stCommon.shwOpertBandNum = 41;                               /*宽带语音子带数*/

                    break;
                }
                default:
                {
                    return UCOM_RET_ERR_PARA;
                }
            }

            pstObj->stCommon.swIdx                   = 0;
            pstObj->stCommon.shwZfNum                = 1;
            pstObj->stCommon.shwIdxSwitch            = 0;

            pstObj->stSpeechProb.shwSNRMin           = PriSNR_Min_Tab[shwTmp];

            pstObj->stAbsenceProb.shwMeanPriSNR      = 0;
            pstObj->stAbsenceProb.shwPriSNR          = 0;

            for(shwI=0; shwI<MED_EANR_MAXBINNUM; shwI++)
            {
                pstObj->stGain.ashwPriSNRMid[shwI]   = 128;
            }

            pstObj->stGain.shwSNRSqrtMin             = PriSNRSqrt_Min_Tab[shwTmp];
        }
    }

    return uwRet;

}



VOS_UINT32  MED_EANR_1MIC_GetPara(
                VOS_VOID               *pInstance,
                VOS_INT16              *pshwPara,
                VOS_INT16               shwParaLen)
{
    VOS_UINT32                uwRet;
    MED_EANR_STRU            *pstObj = (MED_EANR_STRU *)pInstance;

    /* 判断入参是否合法 */
    uwRet         = MED_PP_CheckPtrValid(MED_EANR_GetObjInfoPtr(), pstObj);

    /* 入参合法，则尝试对指针所在位置进行配置项读取操作 */
    if (UCOM_RET_SUCC == uwRet)
    {
        /* BUFFER空间不够，或结构体的使能项不正确，则出错中止操作 */
        if (shwParaLen < MED_EANR_1MIC_PARAM_LEN)
        {
            uwRet = UCOM_RET_FAIL;
        }
        else
        {
            /* 获取NV至pshwPara */
            pshwPara[0] = pstObj->shwEnable;
            pshwPara[1] = pstObj->shwReduceDB;
            pshwPara[2] = pstObj->shwZeroFlagTh;
        }
    }

    return uwRet;

}


VOS_UINT32  MED_EANR_1MIC_Destroy(VOS_VOID  **ppInstance)
{
    VOS_UINT32              uwRet;
    MED_EANR_STRU          *pstObj;
    pstObj               = (MED_EANR_STRU *)(*ppInstance);

    /* 判断入参是否合法 */
    uwRet                = MED_PP_CheckPtrValid(MED_EANR_GetObjInfoPtr(), pstObj);

    /* 入参合法，则将指针对象释放，并将指针赋空 */
    if (UCOM_RET_SUCC == uwRet)
    {
       UCOM_MemSet(pstObj, 0, sizeof(MED_EANR_STRU));
       pstObj->enIsUsed  = CODEC_OBJ_UNUSED;                                      /* 实例化标志置为false */
       *ppInstance       = MED_NULL;
    }

    return uwRet;

}

/*************************************************************************************************
函数名称: MED_EANR_FastSmooth_32
功能描述: 32bit平滑滤波器。
修改历史: 1. 2013年7月27日，新生成函数
**************************************************************************************************/
void MED_EANR_FastSmooth_32(MED_INT32 *pswIn, MED_INT32 *pswOut, MED_INT16 shwLen)
{
    MED_INT16 shwI ;

    for (shwI = 1; shwI < (shwLen - 1); shwI++)
    {
        pswOut[shwI] = CODEC_OpL_add(CODEC_OpL_add(pswIn[shwI - 1], pswIn[shwI + 1]),
                                   (pswIn[shwI] << 1));

        pswOut[shwI] = CODEC_OpL_shr_r(pswOut[shwI], 2);
    }

    pswOut[0]   = CODEC_OpL_add((pswIn[0] << 1), pswIn[1]);
    pswOut[0]   = CODEC_OpL_shr_r(pswOut[0], 2);

    pswOut[shwLen - 1] = CODEC_OpL_add((pswIn[shwLen - 1] << 1), pswIn[shwLen - 2]);
    pswOut[shwLen - 1] = CODEC_OpL_shr_r(pswOut[shwLen - 1], 2);
}

/*************************************************************************************************
函数名称: MED_EANR_FastSmooth_16
功能描述: 16bit平滑滤波器。
修改历史: 1. 2013年7月27日，新生成函数
**************************************************************************************************/
void MED_EANR_FastSmooth_16(MED_INT16 *pshwIn, MED_INT16 *pshwOut, MED_INT16 shwLen)
{
    MED_INT16 shwI; 

    for (shwI = 1; shwI < (shwLen-1); shwI++)
    {
        pshwOut[shwI] = CODEC_OpAdd(CODEC_OpAdd(pshwIn[shwI - 1] >> 2, pshwIn[shwI + 1] >> 2),
                                  (pshwIn[shwI] >> 1));
    }

    pshwOut[0]          = CODEC_OpAdd(pshwIn[0] >> 1, pshwIn[1] >> 2);
    pshwOut[shwLen - 1] = CODEC_OpAdd(pshwIn[shwLen - 1] >> 1, pshwIn[shwLen - 2] >> 2);

}

/*************************************************************************************************
函数名称: MED_EANR_FastSmooth_Shl2
功能描述: 16bit平滑滤波器,结果放大2bit。
修改历史: 1. 2013年7月27日，新生成函数
**************************************************************************************************/
void MED_EANR_FastSmooth_Shl2(MED_INT16 *pshwIn, MED_INT16 *pshwOut, MED_INT16 shwLen)
{
    MED_INT16 shwI ;

    for(shwI = 1; shwI < (shwLen - 1); shwI++)
    {
        pshwOut[shwI]   = CODEC_OpAdd(CODEC_OpAdd(pshwIn[shwI - 1], pshwIn[shwI + 1]),
                                   (MED_INT16)(pshwIn[shwI] << 1));
    }

    pshwOut[0]          = CODEC_OpAdd((MED_INT16)(pshwIn[0] << 1), pshwIn[1]);
    pshwOut[shwLen - 1] = CODEC_OpAdd((MED_INT16)(pshwIn[shwLen - 1] << 1), pshwIn[shwLen - 2]);

}

/*************************************************************************************************
函数名称: MED_EANR_ConvSmooth
功能描述: 16bit卷积平滑滤波器。
修改历史: 1. 2013年7月27日，新生成函数
**************************************************************************************************/
void MED_EANR_ConvSmooth(MED_INT16 *ashwWin,
                         MED_INT16 *pshwIn,
                         MED_INT16 *pshwOut,
                         MED_INT16 shwBinNum)
{
    MED_INT16 ashwtemp[100];
    MED_INT16 shwTmp;
    MED_INT16 shwI;
    MED_INT16 shwJ;

    CODEC_OpVecSet(ashwtemp, shwBinNum + 31, 0);
    CODEC_OpVecCpy(ashwtemp+15, pshwIn, shwBinNum);

    for (shwI = 0; shwI < shwBinNum; shwI++)
    {
        shwTmp     = 0;

        for ( shwJ = 0; shwJ < 31; shwJ++)
        {
            shwTmp = CODEC_OpAdd(shwTmp, CODEC_OpMult(ashwtemp[shwI + shwJ], ashwWin[shwJ]));
        }
        pshwOut[shwI]  = shwTmp;
    }

}


VOS_UINT32  MED_EANR_1MIC_Main(
                VOS_VOID                    *pInstance,
                VOS_INT16                   *pshwFrmFreq,
                VOS_INT16                   *pshwFrmFreqRef,
                VOS_INT16                    shwNormShift)
{
    VOS_INT16              shwI;

    MED_EANR_STRU         *pstAnrObj = (MED_EANR_STRU *)pInstance;

    /* 使能判断 */
    if (pstAnrObj->shwEnable == 0)
    {
        return   UCOM_RET_SUCC;
    }

    pstAnrObj->stCommon.shwNormShift = shwNormShift;

    pstAnrObj->stCommon.swIdx++;

    /*时频域平滑*/
    MED_EANR_TFSmooth_FIX(&pstAnrObj->stCommon, &pstAnrObj->stTFSmooth, pshwFrmFreqRef);

    /*查找S最小值*/
    MED_EANR_MinTrack_FIX(&pstAnrObj->stCommon, pstAnrObj->stTFSmooth.aswPowSpctSmth);

    /*根据信号能量比进行有无语音的初始判断*/
    MED_EANR_SpeechDec_FIX(&pstAnrObj->stCommon,
                           &pstAnrObj->stSpeechDec,
                           pstAnrObj->stTFSmooth.aswPowSpctSmth,
                           pstAnrObj->stTFSmooth.aswPowSpct);

    /*根据判断结果对信号进行时域、频域平滑及功率谱最小值查找*/
    MED_EANR_CondiSmooth_FIX(&pstAnrObj->stCommon,
                             pstAnrObj->stTFSmooth.aswPowSpctSmth,
                             pstAnrObj->stTFSmooth.aswPowSpct,
                             pstAnrObj->stSpeechDec.ashwSpechIdx);


    /*根据判断结果计算语音存在概率*/
    MED_EANR_SpeechProb_FIX(&pstAnrObj->stCommon,
                            &pstAnrObj->stSpeechProb,
                            pstAnrObj->stGain.ashwPriSNRMid,
                            pstAnrObj->stTFSmooth.aswPowSpctSmth,
                            pstAnrObj->stTFSmooth.aswPowSpct);


    /*在一定时间窗内查找最小值,每MED_EANR_UPDATELEN帧更新一次，
      存储数组中共保留shwBINNUM*MED_EANR_DATAWINNUM个数据*/
    pstAnrObj->stCommon.shwIdxSwitch++;
    MED_EANR_MinTrackUpdate_FIX(&pstAnrObj->stCommon,
                                &pstAnrObj ->stMinTrackUpdate,
                                pstAnrObj->stTFSmooth.aswPowSpctSmth);

    if (MED_EANR_UPDATELEN == pstAnrObj->stCommon.shwIdxSwitch)
    {
        pstAnrObj->stCommon.shwIdxSwitch = 0;
    }

    /*噪声功率谱计算*/
    MED_EANR_NoiseSpect_FIX(&pstAnrObj->stCommon,
                            &pstAnrObj->stNoiseSpect,
                             pstAnrObj->stSpeechProb.ashwSpchProb,
                             pstAnrObj->stTFSmooth.aswPowSpct);

    /*以上是噪声估计*/

    /*先验信噪比长时平滑及频域在不同窗长范围内的平滑*/
    for (shwI = 0; shwI < pstAnrObj->stCommon.shwOpertBandNum; shwI++)
    {
        pstAnrObj->stSpeechProb.ashwPriSNR[shwI] = CODEC_OpMin(MED_EANR_PRISNRMAXLIMIT,
                                                          pstAnrObj->stSpeechProb.ashwPriSNR[shwI]);
    }

    MED_EANR_SNRSmooth_FIX(&pstAnrObj->stCommon,
                           &pstAnrObj->stSnrSmooth,
                           pstAnrObj->stSpeechProb.ashwPriSNR);


    /*语音存在概率计算*/
    MED_EANR_PresenceProb_FIX(&pstAnrObj->stCommon,
                              &pstAnrObj->stPresenceProb,
                              pstAnrObj->stNoiseSpect.aswNoisePowLong,
                              pstAnrObj->stSnrSmooth.ashwSNRSmthLcl,
                              pstAnrObj->stSnrSmooth.ashwSNRSmthGlb);


    /*语音不存在概率*/
    MED_EANR_AbsenceProb_FIX(&pstAnrObj->stCommon,
                             &pstAnrObj->stAbsenceProb,
                             pstAnrObj->stPresenceProb.ashwProbLcl,
                             pstAnrObj->stPresenceProb.ashwProbGlb);

    /*增益计算*/
    MED_EANR_Gain_FIX(&pstAnrObj->stCommon,
                      &pstAnrObj->stGain,
                      pstAnrObj->stSpeechProb.shwSNRMin,
                      pstAnrObj->stTFSmooth.aswPowSpct,
                      pstAnrObj->stAbsenceProb.ashwAbsenProb);

    for (shwI = 0; shwI < pstAnrObj->stCommon.shwOpertBandNum; shwI++)
    {
        pstAnrObj->stGain.ashwPriSNRMid[shwI] = CODEC_OpMin(MED_EANR_PRISNRMAXLIMIT,
                                                       pstAnrObj->stGain.ashwPriSNRMid[shwI]);
    }

    /*增益施加*/
    MED_EANR_DATA_FIX(&pstAnrObj->stCommon, pstAnrObj->stGain.ashwGain, pshwFrmFreq);

    return UCOM_RET_SUCC;

}

/*************************************************************************************************
函数名称: MED_EANR_TFSmooth_FIX
功能描述: 计算信号的功率谱，并在时频域上对功率谱进行时间帧间和频点间的平滑。
修改历史: 1. 2013年7月27日，新生成函数
**************************************************************************************************/
void MED_EANR_TFSmooth_FIX(MED_EANR_COMM_STRU *stCommon,
                           MED_EANR_TFSMOOTH_STRU *stTFSmooth,
                           MED_INT16 *shwDataBuf)
{
    MED_INT16   shwI;
    MED_INT16   shwJ;
    MED_INT16   shwTmpSub;
    MED_INT16   shwTmpNorm;                                                     /*功率谱归一化处理中间变量*/
    MED_INT16   shwBinHi;                                                       /*子带上限频点索引*/
    MED_INT16   shwBinLow;                                                      /*子带下限频点索引*/
    MED_INT32   swTmpPowS;                                                      /*时频平滑功率谱计算中间变量*/
    MED_INT32   aswPowSpec[MED_EANR_MAXBINNUM];                                 /*频点间平滑后的功率谱*/
    MED_INT32   aswTmp[MED_EANR_MAXBINNUM];
    MED_INT32   swCntI;

    shwTmpNorm   = CODEC_OpSub((MED_INT16)(stCommon->shwNormShift << 1), 5);                   /*对功率谱做时间帧间平滑所需右移位数，去除NormShift后放大5bit*/

    /*功率谱计算，首尾两个频点运算特殊单独计算*/
    aswTmp[0]    = CODEC_OpL_mult0(shwDataBuf[0], shwDataBuf[0] );
    aswTmp[0]    = CODEC_OpL_add(aswTmp[0], CODEC_OpL_mult0(shwDataBuf[2], shwDataBuf[2]));
    aswTmp[0]    = CODEC_OpL_add(aswTmp[0], CODEC_OpL_mult0(shwDataBuf[3], shwDataBuf[3]));

    stTFSmooth->aswPowSpct[0]    = aswTmp[0] >> 1;

    stTFSmooth->aswPowSpct[stCommon->shwSubBandNum]
                                 = CODEC_OpL_mult0(shwDataBuf[1], shwDataBuf[1]);

    /*功率谱计算，中间频点的计算*/
    for (shwI = 1; shwI < stCommon->shwSubBandNum; shwI++)
    {
        aswTmp[shwI]     = 0;

        shwBinHi         = MED_EANR_SUBBAND_FH[shwI];
        shwBinLow        = MED_EANR_SUBBAND_FL[shwI];

        for (shwJ = shwBinLow; shwJ <= shwBinHi; shwJ++)
        {
            aswTmp[shwI] = CODEC_OpL_add(aswTmp[shwI],
                                       CODEC_OpL_mult0(shwDataBuf[shwJ * 2], shwDataBuf[shwJ * 2]));

            aswTmp[shwI] = CODEC_OpL_add(aswTmp[shwI],
                                       CODEC_OpL_mult0(shwDataBuf[(shwJ * 2) + 1], shwDataBuf[(shwJ * 2) + 1]));
        }

        stTFSmooth->aswPowSpct[shwI]
                         = aswTmp[shwI] / CODEC_OpSub(CODEC_OpAdd(shwBinHi, 1), shwBinLow);
    }

    /*频点间的平滑*/
    MED_EANR_FastSmooth_32(stTFSmooth->aswPowSpct, aswPowSpec, stCommon->shwOpertBandNum);

    /*时间帧间平滑*/
    /*NormShift归一处理,aswPowSpec(Q5)*/
    CODEC_OpVecShr_r32(aswPowSpec, stCommon->shwOpertBandNum, shwTmpNorm,aswPowSpec);

    if (stCommon->swIdx == stCommon->shwZfNum)
    {
        /*第一个非全零帧之前直接将当前功率谱赋值给对应功率谱变量和噪声功率谱*/
        CODEC_OpVecShr_r32(aswPowSpec,
                         stCommon->shwOpertBandNum,
                         0,
                         stCommon->aswPowSpctCond);

        CODEC_OpVecShr_r32(aswPowSpec,
                         stCommon->shwOpertBandNum,
                         0,
                         stTFSmooth->aswPowSpctSmth);

        CODEC_OpVecShr_r32(stTFSmooth->aswPowSpct,
                         stCommon->shwOpertBandNum,
                         (MED_INT16)(stCommon->shwNormShift << 1),
                         stCommon->aswPowSpctSmth);

        shwTmpSub = CODEC_OpSub((MED_INT16)(stCommon->shwNormShift << 1), 6);

        for (swCntI = 0; swCntI < stCommon->shwOpertBandNum; swCntI++)
        {
            stCommon->aswNoisePow[swCntI]    = L_shr_r(stTFSmooth->aswPowSpct[swCntI], shwTmpSub);
            stCommon->aswNoisePowRud[swCntI] = L_shr_r(stTFSmooth->aswPowSpct[swCntI], shwTmpSub);
        }

    }
    else
    {
        /*各频点上时间帧间平滑,stTFSmooth->aswPowSpctSmth(Q5)*/
        for (shwI = 0; shwI < stCommon->shwOpertBandNum; shwI++)
        {
            swTmpPowS = CODEC_OpL_sub(stTFSmooth->aswPowSpctSmth[shwI],aswPowSpec[shwI] );

            swTmpPowS = CODEC_OpL_mpy_32_16_r(swTmpPowS,MED_EANR_ALPHA_COEF);

            stTFSmooth->aswPowSpctSmth[shwI]
                      = CODEC_OpL_add(swTmpPowS, aswPowSpec[shwI] );
        }
    }
}

/*************************************************************************************************
函数名称: MED_EANR_MinTrack_FIX
功能描述: 搜索时频域平滑后的信号功率谱的所有频点上的最小值。
修改历史: 1. 2013年7月27日，新生成函数
**************************************************************************************************/
void MED_EANR_MinTrack_FIX(MED_EANR_COMM_STRU *stCommon, MED_INT32 *aswPowSpctSmth)
{
    MED_INT16 shwI;

    if (stCommon->swIdx < (MED_EANR_SMTHSTARTFRM + stCommon->shwZfNum))
    {
        /* 最初的13个非全零帧不进行最小值搜索 */
        UCOM_MemCpy(stCommon->aswPowSpctMin ,
                    aswPowSpctSmth,
                    (VOS_UINT16)(stCommon->shwOpertBandNum) * sizeof(MED_INT32));

        UCOM_MemCpy(stCommon->aswPowSpctMinCurt,
                    aswPowSpctSmth,
                    (VOS_UINT16)(stCommon->shwOpertBandNum) * sizeof(MED_INT32));
    }
    else
    {
        for (shwI = 0; shwI < stCommon->shwOpertBandNum; shwI++)
        {
            stCommon->aswPowSpctMin[shwI]     = CODEC_OpL_min(aswPowSpctSmth[shwI],
                                                            stCommon->aswPowSpctMin[shwI]);

            stCommon->aswPowSpctMinCurt[shwI] = CODEC_OpL_min(aswPowSpctSmth[shwI],
                                                            stCommon->aswPowSpctMinCurt[shwI] );
        }
    }
}

/*************************************************************************************************
函数名称: MED_EANR_SpeechDec_FIX
功能描述: 初步的语音检测有音判决。
修改历史: 1. 2013年7月27日，新生成函数
**************************************************************************************************/
void MED_EANR_SpeechDec_FIX(MED_EANR_COMM_STRU *stCommon,
                            MED_EANR_SPEECHDEC_STRU *stSpeechDec,
                            MED_INT32*aswPowSpctSmth,
                            MED_INT32 *aswPowSpct)
{
    MED_INT16 shwI;
    MED_INT32 swPowTh;
    MED_INT32 swPowSmthTh;
    MED_INT32 aswPowSpctTh;
    MED_INT32 aswPowSpctSmthTh;

    MED_INT16 shwTmpNorm;

    shwTmpNorm      = CODEC_OpSub((MED_INT16)(stCommon->shwNormShift<<1), 5);   /*NormShift处理移位bit数*/

    for (shwI = 0; shwI < stCommon->shwOpertBandNum; shwI++)
    {
        stSpeechDec->ashwSpechIdx[shwI]     = 0;                                /*有音指示，0表示有音，1表示无音,默认有音*/

        swPowTh     = CODEC_OpL_mpy_32_16_r(stCommon->aswPowSpctMin[shwI] << 3,
                                          MED_EANR_SPEECHDEC_THRESHOLDA);       /*门限计算*/
        swPowSmthTh = CODEC_OpL_mpy_32_16_r(stCommon->aswPowSpctMin[shwI],
                                          MED_EANR_SPEECHDEC_THRESHOLDB);       /*门限计算*/

        aswPowSpctTh     = CODEC_OpL_shl(swPowTh, shwTmpNorm);
        aswPowSpctSmthTh = CODEC_OpL_shl(swPowSmthTh, 2);

        if( (aswPowSpct[shwI] < aswPowSpctTh)
         && (aswPowSpctSmth[shwI] < aswPowSpctSmthTh))
        {
            stSpeechDec->ashwSpechIdx[shwI] = 1;                                /*无音判决*/
        }
    }
}

/*************************************************************************************************
函数名称: MED_EANR_CondiSmooth_FIX
功能描述: 有音判决指示条件下的时频域平滑和最小值搜索。
修改历史: 1. 2013年7月27日，新生成函数
**************************************************************************************************/

void MED_EANR_CondiSmooth_FIX(MED_EANR_COMM_STRU *stCommon,
                              MED_INT32 *aswPowSpctSmth,
                              MED_INT32 *aswPowSpct,
                              MED_INT16 *ashwSpechIdx)
{
    MED_INT16     shwI;
    MED_INT16     shwTmpNorm;
    MED_INT32     swTmp;
    MED_INT32     aswTmp[MED_EANR_MAXBINNUM+2] = {0};
    MED_INT32     aswTmp1[MED_EANR_MAXBINNUM] = {0};
    MED_INT32     aswTmp2[MED_EANR_MAXBINNUM] = {0};
    MED_INT32     swMinPowerThrdTemp = 0;

    shwTmpNorm    = CODEC_OpSub((MED_INT16)(stCommon->shwNormShift << 1), 5);                  /*NormShift处理移位bit数*/

    /*对有音指示进行频点间的平滑处理*/
    MED_EANR_FastSmooth_Shl2(ashwSpechIdx,
                             stCommon->ashwPresenIndSmth,
                             stCommon->shwOpertBandNum);

    /*为了在for循环规整处理，对信号功率谱两端各加一个0*/
    aswTmp[0]                             = 0;
    aswTmp[stCommon->shwOpertBandNum + 1] = 0;

    for (shwI = 0; shwI< stCommon->shwOpertBandNum;shwI++)
    {
        aswTmp[shwI+1]                    = 0;

        if(1 == ashwSpechIdx[shwI])
        {
            aswTmp[shwI + 1]              = aswPowSpct[shwI];
        }
    }

    /*有音判决条件下的时频域频点间平滑*/
    for (shwI = 0; shwI < stCommon->shwOpertBandNum; shwI++)
    {
        if (1 == ashwSpechIdx[shwI])
        {
            /*无语音时频点间平滑*/
            aswTmp2[shwI] = CODEC_OpL_shl(aswTmp[shwI + 1], 1);

            aswTmp2[shwI] = CODEC_OpL_add(aswTmp2[shwI], aswTmp[shwI + 2]);

            aswTmp2[shwI] = CODEC_OpNormDiv_32(CODEC_OpL_add(aswTmp2[shwI], aswTmp[shwI]),
                                             stCommon->ashwPresenIndSmth[shwI],
                                             0);

            aswTmp1[shwI] = CODEC_OpL_shr_r(aswTmp2[shwI], shwTmpNorm);
        }
        else
        {
            /*有语音时不平滑*/
            aswTmp1[shwI] =  stCommon->aswPowSpctCond[shwI];
        }
    }

    swMinPowerThrdTemp = CODEC_OpL_shl(MED_EANR_MINPOWERTHD, (shwTmpNorm+2));

    /*时间帧间平滑*/
    if (stCommon->swIdx < (MED_EANR_SMTHSTARTFRM + stCommon->shwZfNum))
    {
        /*最初的13个非全零帧不进行平滑和最小值搜索*/
        UCOM_MemCpy(stCommon->aswPowSpctCond ,
                    aswPowSpctSmth ,
                    (MED_UINT16)(stCommon->shwOpertBandNum) * sizeof(MED_INT32));

        for (shwI = 0; shwI < stCommon->shwOpertBandNum; shwI++)
        {

            stCommon->aswPowSpctMinC[shwI]     = CODEC_OpL_min(stCommon->aswPowSpctCond[shwI], swMinPowerThrdTemp);
            stCommon->aswPowSpctMinCurtC[shwI] = CODEC_OpL_min(stCommon->aswPowSpctCond[shwI], swMinPowerThrdTemp);
        }
    }
    else
    {
        for (shwI = 0; shwI < stCommon->shwOpertBandNum; shwI++)
        {
            /*时间帧间平滑*/
            swTmp  = CODEC_OpL_sub(stCommon->aswPowSpctCond[shwI], aswTmp1[shwI]);

            swTmp  = CODEC_OpL_mpy_32_16_r(swTmp, MED_EANR_ALPHA_COEF);

            stCommon->aswPowSpctCond[shwI]     = CODEC_OpL_add(swTmp, aswTmp1[shwI]);

            /*平滑后最小值搜索*/
            stCommon->aswPowSpctMinC[shwI]     = CODEC_OpL_min(stCommon->aswPowSpctMinC[shwI] ,
                                                             stCommon->aswPowSpctCond[shwI]);

            stCommon->aswPowSpctMinCurtC[shwI] = CODEC_OpL_min(stCommon->aswPowSpctMinCurtC[shwI],
                                                             stCommon->aswPowSpctCond[shwI]);
        }
    }
}

/*************************************************************************************************
函数名称: MED_EANR_SpeechProb_FIX
功能描述: 先验信噪比,后验信噪比以及有音概率计算。
修改历史: 1. 2013年7月27日，新生成函数
**************************************************************************************************/
void MED_EANR_SpeechProb_FIX(MED_EANR_COMM_STRU       *stCommon,
                                        MED_EANR_SPEECHPROB_STRU *stSpeechProb,
                                        MED_INT16                *ashwPriSNRMid,
                                        MED_INT32                *aswPowSpctSmth,
                                        MED_INT32                *aswPowSpct)

{
    MED_INT16   shwFilterCoeff;
    MED_INT16   shwI;
    MED_INT16   shwTmpShift;
    MED_INT32   aswPstSNR[MED_EANR_MAXBINNUM]    = {0};
    MED_INT16   ashwSNRFact[MED_EANR_MAXBINNUM]  = {0};
    MED_INT32   aswPresenInd[MED_EANR_MAXBINNUM] = {0};
    MED_INT32   swThreshold;
    MED_INT16   ashwLackProb;
    MED_INT16   ashwTmp;
    MED_INT32   swTmp;
    MED_INT32   swTmp1;
    MED_INT16   ashwTmp2[MED_EANR_MAXBINNUM]     = {0};

    shwFilterCoeff = CODEC_OpSub(MED_EANR_MAX11, MED_EANR_SPEECHPROB_SNR_SMTH_COEF >> 4);
    shwTmpShift    = CODEC_OpSub((MED_INT16)(stCommon->shwNormShift << 1), 6);                 /*噪声左移位数，左移后与aswPowSpct放大形同倍数*/

    /*更新先验信噪比ashwPriSNR、计算后验信噪比swPstSNR、更新概率计算的必要变量shwSNRFact*/
    for(shwI = 0; shwI < stCommon->shwOpertBandNum; shwI++)
    {
        /*计算后验信噪比aswPstSNR(Q11)*/
        swTmp             = CODEC_OpL_shl(stCommon->aswNoisePow[shwI], shwTmpShift);

        aswPstSNR[shwI]   = CODEC_OpNormDiv_32(aswPowSpct[shwI],
                                             CODEC_OpL_max(1, swTmp),
                                             11);                               /*aswPstSNR(Q11)*/

        /*更新先验信噪比ashwPriSNR(Q7)*/
        stSpeechProb->ashwPriSNR[shwI]
                          = CODEC_OpMult_r(ashwPriSNRMid[shwI], MED_EANR_SPEECHPROB_SNR_SMTH_COEF);

        if (aswPowSpct[shwI] > CODEC_OpL_shl(stCommon->aswNoisePow[shwI], shwTmpShift))
        {
            /*如果aswPstSNR(Q11) > 2048,修正ashwPriSNR(Q7)*/
            swTmp         = CODEC_OpL_sub(aswPstSNR[shwI], MED_EANR_Q11);
            swTmp         = CODEC_OpL_min(swTmp, MED_EANR_PSTSNRMAX);

            ashwTmp       = CODEC_OpSaturate(CODEC_OpL_mpy_32_16_r(swTmp, shwFilterCoeff));

            stSpeechProb->ashwPriSNR[shwI] = CODEC_OpAdd(stSpeechProb->ashwPriSNR[shwI],
                                                       ashwTmp);
        }

        stSpeechProb->ashwPriSNR[shwI]     = CODEC_OpMax(stSpeechProb->ashwPriSNR[shwI],
                                                       stSpeechProb->shwSNRMin);/*对shwSNRMin进行最小值保护*/

        /*更新概率计算的必要变量shwSNRFact(Q11)*/
        swTmp             = ((VOS_INT32)stSpeechProb->ashwPriSNR[shwI]) << 15;
        ashwSNRFact[shwI] = CODEC_OpSaturate(swTmp / CODEC_OpAdd(MED_EANR_Q7, stSpeechProb->ashwPriSNR[shwI]));

        ashwTmp2[shwI] = CODEC_OpSaturate(CODEC_OpL_mpy_32_16_r(aswPstSNR[shwI], ashwSNRFact[shwI]));

        ashwSNRFact[shwI] = CODEC_OpMin(ashwTmp2[shwI], MED_EANR_SNRFACT_MAX);                    /* 最大值保护15.9424->32650(Q11)*/
    }

/* 此处代码与算法部交付代码不同，被注释掉，因为该分支始终不成立，无法过lint */
/*
    if (2 > MED_EANR_SPEECHPROB_STAT)
    {
        for(shwI = 0; shwI <stCommon->shwOpertBandNum ; shwI++)
        {
            swTmp              = CODEC_OpL_shl(stCommon->aswPowSpctMin[shwI],
                                             CODEC_OpAdd(shwTmpShift, 1));

            swTmp              = CODEC_OpL_mpy_32_16_r(swTmp, MED_EANR_SPEECHDEC_MIN_COMPENS);

            aswPresenInd[shwI] = CODEC_OpNormDiv_32(aswPowSpct[shwI],
                                                  CODEC_OpL_max(1, swTmp),
                                                  12);
        }
    }
    else
    {
*/
    for(shwI = 0; shwI < stCommon->shwOpertBandNum; shwI++)
    {
        swTmp              = CODEC_OpL_shl(stCommon->aswPowSpctMinC[shwI],
                                             CODEC_OpAdd(shwTmpShift, 1));

        swTmp              = CODEC_OpL_mpy_32_16_r(swTmp, MED_EANR_SPEECHDEC_MIN_COMPENS);

        aswPresenInd[shwI] = CODEC_OpNormDiv_32(aswPowSpct[shwI],
                                                  CODEC_OpL_max(1,swTmp),
                                                  12);                          /*aswPresenInd(Q13)*/
    }
/*
    }
*/
    /*计算语音存在与不存在的条件概率，并根据语音存在概率更新噪声*/
    for (shwI = 0; shwI < stCommon->shwOpertBandNum; shwI++)
    {
        ashwLackProb                      = MED_EANR_MAX16;
        stSpeechProb->ashwSpchProb[shwI]  = 0;

        swThreshold = CODEC_OpL_mpy_32_16_r(stCommon->aswPowSpctMinC[shwI] << 2,
                                          MED_EANR_SPEECHDEC_THRESHOLDB);

        /*语音存在，ashwSpchProb=0，反之ashwSpchProb=1；若不确定，采用软判决方式计算概率*/
        if ((aswPresenInd[shwI]      < MED_EANR_SPEECHPROB_THRESHOLDC)
            && (aswPowSpctSmth[shwI] < swThreshold)
            && (aswPresenInd[shwI]   > MED_EANR_Q13))
        {
           ashwLackProb             = CODEC_OpSub(MED_EANR_SPEECHPROB_THRESHOLDC,
                                                (MED_INT16)aswPresenInd[shwI]); /*语音不存在概率*/

           swTmp1  = CODEC_OpL_Exp(ashwSNRFact[shwI], 11, 8);                     /*ashwSNRFact(Q11),输出Q8*/

           swTmp1  = CODEC_OpL_mpy_32_16_r(swTmp1, CODEC_OpSub(MED_EANR_Q14, ashwLackProb));

           swTmp   = CODEC_OpL_mult(ashwLackProb,
                                  CODEC_OpAdd(MED_EANR_Q7, stSpeechProb->ashwPriSNR[shwI]));

           swTmp   = CODEC_OpL_add(swTmp1 , swTmp >> 15);

           stSpeechProb->ashwSpchProb[shwI]
                   = CODEC_OpSaturate(CODEC_OpNormDiv_32(swTmp1 , swTmp, 15));  /*语音存在概率(Q15)*/
        }

        if((aswPresenInd[shwI]   >= MED_EANR_SPEECHPROB_THRESHOLDC)
         ||(aswPowSpctSmth[shwI] >= swThreshold))
        {
            stSpeechProb->ashwSpchProb[shwI] = MED_EANR_MAX16;
        }
    }
}

/*************************************************************************************************
函数名称: MED_EANR_MinTrackUpdate_FIX
功能描述: 更新长度为MED_EANR_DATAWINLEN的时间窗内的功率谱数据并搜索这个间窗内各频点上的最小值。
修改历史: 1. 2013年7月27日，新生成函数
**************************************************************************************************/

void MED_EANR_MinTrackUpdate_FIX(MED_EANR_COMM_STRU *stCommon,
                                 MED_EANR_MINTRACKUPDATE_STRU *stMinTrackUpdate,
                                 MED_INT32 *aswPowSpctSmth)
{

    MED_INT16   shwI;
    MED_INT16   shwJ;
    MED_INT32   swTemp1;
    MED_INT32   swTemp2;

    if (stCommon->shwIdxSwitch == MED_EANR_UPDATELEN)
    {
        if(stCommon->swIdx == ((MED_EANR_UPDATELEN - 1) + stCommon->shwZfNum))
        {
            for(shwI = 0; shwI < MED_EANR_DATAWINNUM; shwI++)
            {
                UCOM_MemCpy(stMinTrackUpdate->aswPowStck + (stCommon->shwOpertBandNum * shwI),
                            aswPowSpctSmth,
                            (MED_UINT16)(stCommon->shwOpertBandNum) * sizeof(MED_INT32));

                UCOM_MemCpy(stMinTrackUpdate->aswCondPowStck + (stCommon->shwOpertBandNum * shwI),
                            stCommon->aswPowSpctCond,
                            (MED_UINT16)stCommon->shwOpertBandNum * sizeof(MED_INT32));
            }
        }
        else
        {
            UCOM_MemCpy(stMinTrackUpdate->aswPowStck,
                        stMinTrackUpdate->aswPowStck + stCommon->shwOpertBandNum,
                        (MED_UINT16)stCommon->shwOpertBandNum * (MED_EANR_DATAWINNUM - 1) * sizeof(MED_INT32));

            UCOM_MemCpy(stMinTrackUpdate->aswPowStck + (stCommon->shwOpertBandNum * (MED_EANR_DATAWINNUM - 1)),
                        stCommon->aswPowSpctMinCurt,
                        (MED_UINT16)stCommon->shwOpertBandNum * sizeof(MED_INT32));

            UCOM_MemCpy(stMinTrackUpdate->aswCondPowStck,
                        stMinTrackUpdate->aswCondPowStck + stCommon->shwOpertBandNum,
                        (MED_UINT16)stCommon->shwOpertBandNum * (MED_EANR_DATAWINNUM - 1) * sizeof(MED_INT32));

            UCOM_MemCpy(stMinTrackUpdate->aswCondPowStck + (stCommon->shwOpertBandNum * (MED_EANR_DATAWINNUM - 1)),
                        stCommon->aswPowSpctMinCurtC,
                        (MED_UINT16)stCommon->shwOpertBandNum * sizeof(MED_INT32));

            UCOM_MemCpy(stCommon->aswPowSpctMinCurt,
                        aswPowSpctSmth,
                        (MED_UINT16)stCommon->shwOpertBandNum * sizeof(MED_INT32));

            UCOM_MemCpy(stCommon->aswPowSpctMinCurtC,
                        stCommon->aswPowSpctCond,
                        (MED_UINT16)stCommon->shwOpertBandNum * sizeof(MED_INT32));

            for(shwI = 0; shwI < stCommon->shwOpertBandNum; shwI++)
            {
                swTemp1  = stMinTrackUpdate->aswPowStck[shwI];

                swTemp2  = stMinTrackUpdate->aswCondPowStck[shwI];

                for(shwJ = stCommon->shwOpertBandNum;
                    shwJ < (MED_EANR_DATAWINNUM * stCommon->shwOpertBandNum);
                    shwJ+=(stCommon->shwOpertBandNum))
                {
                    swTemp1 = CODEC_OpL_min(swTemp1,
                                          stMinTrackUpdate->aswPowStck[shwI + shwJ]);

                    swTemp2 = CODEC_OpL_min(swTemp2,
                                          stMinTrackUpdate->aswCondPowStck[shwI + shwJ]);
                }

                stCommon->aswPowSpctMin[shwI]  = swTemp1;

                stCommon->aswPowSpctMinC[shwI] = swTemp2;
            }
        }
    }
}

/*************************************************************************************************
函数名称: MED_EANR_NoiseSpect_FIX
功能描述: 噪声功率谱的计算。
修改历史: 1. 2013年7月27日，新生成函数
**************************************************************************************************/
void MED_EANR_NoiseSpect_FIX(MED_EANR_COMM_STRU *stCommon,
                             MED_EANR_NOISESPECT_STRU *stNoiseSpect,
                             MED_INT16 *ashwSpchProb,
                             MED_INT32 *aswPowSpct)
{
    MED_INT16 shwI;
    MED_INT16 shwFilterCoef;
    MED_INT32 swTmp;
    MED_INT16 ashwPresenProb[MED_EANR_MAXBINNUM];
    MED_INT16 ashwPresenProbL[MED_EANR_MAXBINNUM];
    MED_INT32 aswNormPowSpetrm[MED_EANR_MAXBINNUM] = {0};

    /*噪声功率谱平滑 */
    for(shwI = 0; shwI < stCommon->shwOpertBandNum; shwI++)
    {
        shwFilterCoef          = CODEC_OpSub(MED_EANR_MAX16, MED_EANR_NOISESPECT_ALPHA_COEF);

        aswNormPowSpetrm[shwI] = CODEC_OpL_shr_r(aswPowSpct[shwI],
                                               CODEC_OpSub((MED_INT16)(stCommon->shwNormShift << 1), 6));

        ashwPresenProb[shwI]   = CODEC_OpAdd(MED_EANR_NOISESPECT_ALPHA_COEF,
                                           CODEC_OpMult_r(shwFilterCoef, ashwSpchProb[shwI]));

        if(MED_EANR_NOISESPECT_PROBTHD < ashwPresenProb[shwI])
        {
            /*对于语音概率非常高的场景认为噪声与前一帧相同*/
            stCommon->aswNoisePowRud[shwI] = stCommon->aswNoisePowRud[shwI];
        }
        else
        {
            swTmp                          = CODEC_OpL_sub(stCommon->aswNoisePowRud[shwI],
                                                         aswNormPowSpetrm[shwI]);

            swTmp                          = CODEC_OpL_mpy_32_16_r(swTmp, ashwPresenProb[shwI]);

            stCommon->aswNoisePowRud[shwI] = CODEC_OpL_add(swTmp, aswNormPowSpetrm[shwI]);
        }

        stCommon->aswNoisePow[shwI]        = stCommon->aswNoisePowRud[shwI] << 1;
    }

    /*对噪声进行长时平滑*/
    if(stCommon->swIdx < (MED_EANR_SMTHSTARTFRM + stCommon->shwZfNum))
    {
        /*最初的13个非全零帧不进行平滑运算*/
        UCOM_MemCpy(stNoiseSpect->aswNoisePowLong,
                    stCommon->aswNoisePowRud,
                    (MED_UINT16)stCommon->shwOpertBandNum * sizeof(MED_INT32));
    }
    else
    {
        shwFilterCoef  = CODEC_OpSub(MED_EANR_MAX16, MED_EANR_NOISESPECT_ALPHA_COEF_L);

        for(shwI=0; shwI < stCommon->shwOpertBandNum; shwI++)
        {
            ashwPresenProbL[shwI]
                = CODEC_OpAdd(MED_EANR_NOISESPECT_ALPHA_COEF_L,
                            CODEC_OpMult_r(shwFilterCoef, ashwSpchProb[shwI]));

            if(MED_EANR_NOISESPECT_PROBTHD < ashwPresenProbL[shwI])
            {
                /*对于语音概率非常高的场景认为噪声与前一帧相同*/
                stNoiseSpect->aswNoisePowLong[shwI] = stNoiseSpect->aswNoisePowLong[shwI];
            }
            else
            {
                swTmp  = CODEC_OpL_sub(stNoiseSpect->aswNoisePowLong[shwI], aswNormPowSpetrm[shwI]);

                swTmp  = CODEC_OpL_mpy_32_16_r(swTmp, ashwPresenProbL[shwI]);

                stNoiseSpect->aswNoisePowLong[shwI]
                       = CODEC_OpL_add(swTmp, aswNormPowSpetrm[shwI]);
            }
        }
    }

    /*对噪声能量进行无语音补偿 此处代码与算法部交付代码不同，条件必然成立，因LINT不过需要注掉 */
/*
    if(MED_EANR_SPEECHPROB_STAT <= 2)
    {
*/
    for(shwI = 0; shwI < stCommon->shwOpertBandNum; shwI++)
    {
        stCommon->aswNoisePow[shwI] = CODEC_OpL_mpy_32_16_r(stCommon->aswNoisePow[shwI],
                                                            MED_EANR_NOISESPECT_NOISECOMPENS);
    }
/*
    }
*/
}

/*************************************************************************************************
函数名称: MED_EANR_SNRSmooth_FIX
功能描述: 先验信噪比平滑，先对每个子带做时域平滑，再分别做子带间的Local和Global平滑
修改历史: 1. 2013年7月27日，新生成函数
**************************************************************************************************/

void MED_EANR_SNRSmooth_FIX(MED_EANR_COMM_STRU *stCommon,
                            MED_EANR_SNRSMOOTH_STRU *stSnrSmooth,
                            MED_INT16 *ashwPriSNR)
{
    MED_INT16 ashwTmpSNR[MED_EANR_MAXBINNUM];
    MED_INT16 ashwTmp[MED_EANR_MAXBINNUM];

    CODEC_OpVvSub(stCommon->ashwSNRSmth,
                ashwPriSNR,
                stCommon->shwOpertBandNum,
                ashwTmp);

    /*计算先验信噪比时域平滑值，16bit Q7*/
    CODEC_OpVcMultQ15Add(ashwPriSNR,
                       ashwTmp,
                       stCommon->shwOpertBandNum,
                       MED_EANR_APSSMOOTH_ASNR_COEF,
                       stCommon->ashwSNRSmth);

    /*计算先验信噪比子带间的Local平滑值，16bit Q7*/
    MED_EANR_FastSmooth_16(stCommon->ashwSNRSmth,
                        stSnrSmooth->ashwSNRSmthLcl,
                        stCommon->shwOpertBandNum);

    CODEC_OpVecCpy(ashwTmpSNR, stCommon->ashwSNRSmth, stCommon->shwOpertBandNum);

    /*计算先验信噪比子带间的Global平滑值，16bit Q7*/
    MED_EANR_ConvSmooth(MED_EANR_SNR_WIN_GLB,
                        ashwTmpSNR,
                        stSnrSmooth->ashwSNRSmthGlb,
                        stCommon->shwOpertBandNum);

}

/*************************************************************************************************
函数名称: MED_EANR_PresenceProb_FIX
功能描述: 计算Local和Global语音存在概率
修改历史: 1. 2013年7月27日，新生成函数
**************************************************************************************************/

void MED_EANR_PresenceProb_FIX(MED_EANR_COMM_STRU *stCommon,
                               MED_EANR_PRESENCEPROB_STRU *stPresenceProb,
                               MED_INT32 *aswNoisePowLong,
                               MED_INT16 *ashwSNRSmthLcl,
                               MED_INT16 *ashwSNRSmthGlb)
{
    MED_INT16 shwI;
    MED_INT16 ashwLclPriSNR[MED_EANR_MAXBINNUM] = {0};
    MED_INT16 ashwGlbPriSNR[MED_EANR_MAXBINNUM] = {0};
    MED_INT16 shwMeanProb;
    MED_INT16 ashwTmp[MED_EANR_MAXBINNUM]       = {0};
    MED_INT32 swTmp;

    /*将Local和Global的先验信噪比平滑线性值(Q7)转换成对数值，16bit Q10*/
    for (shwI = 0; shwI < stCommon->shwOpertBandNum; shwI++)
    {
        ashwLclPriSNR[shwI] = CODEC_OpLog10(7, (MED_INT32)(CODEC_OpMax(ashwSNRSmthLcl[shwI], 1)));

        ashwGlbPriSNR[shwI] = CODEC_OpLog10(7, (MED_INT32)(CODEC_OpMax(ashwSNRSmthGlb[shwI], 1)));
    }

    /*根据Local和Global的先验信噪比平滑对数值估计有语音的可能性*/
    /*当Local先验信噪比在0.1~0.3162间时，Local有音概率如下计算，16bit Q15*/
    CODEC_OpVcMult(ashwLclPriSNR, stCommon->shwOpertBandNum, MED_EANR_SNRMAPCOEFA, ashwTmp);

    CODEC_OpVecShl(ashwTmp, stCommon->shwOpertBandNum, 5, ashwTmp);

    CODEC_OpVcSub(ashwTmp, stCommon->shwOpertBandNum, CODEC_OpNegate(MED_EANR_SNRMAPCOEFB), ashwTmp);

    CODEC_OpVecShl(ashwTmp, stCommon->shwOpertBandNum, 1, stPresenceProb->ashwProbLcl);

    /*计算Global有音概率，同Local有音概率*/
    CODEC_OpVcMult(ashwGlbPriSNR, stCommon->shwOpertBandNum, MED_EANR_SNRMAPCOEFA, ashwTmp);

    CODEC_OpVecShl(ashwTmp, stCommon->shwOpertBandNum, 5, ashwTmp);

    CODEC_OpVcSub(ashwTmp, stCommon->shwOpertBandNum, CODEC_OpNegate(MED_EANR_SNRMAPCOEFB), ashwTmp);

    CODEC_OpVecShl(ashwTmp, stCommon->shwOpertBandNum, 1, stPresenceProb->ashwProbGlb);

    for (shwI = 0; shwI < stCommon->shwOpertBandNum; shwI++)
    {
        /*当Local先验信噪比小于MED_EANR_PRESENCEPROB_TH1(0.1->13(Q7))，
        Local有音概率设为设定的最小值(0.005->164(Q15))*/
        if (ashwSNRSmthLcl[shwI] <= MED_EANR_PRESENCEPROB_TH1)
        {
            stPresenceProb->ashwProbLcl[shwI] = MED_EANR_PROB_MIN;
        }

        /*当Local先验信噪比大于MED_EANR_PRESENCEPROB_TH2(0.3162->40(Q7))，
        Local有音概率设为设定的最大值MED_EANR_MAX16(1.0->32767(Q15))*/
        if (ashwSNRSmthLcl[shwI] >= MED_EANR_PRESENCEPROB_TH2)
        {
            stPresenceProb->ashwProbLcl[shwI] = MED_EANR_MAX16;
        }

        /*计算Global有音概率，同Local有音概率*/
        if (ashwSNRSmthGlb[shwI] <= MED_EANR_PRESENCEPROB_TH1)
        {
            stPresenceProb->ashwProbGlb[shwI] = MED_EANR_PROB_MIN;
        }

        if (ashwSNRSmthGlb[shwI] >= MED_EANR_PRESENCEPROB_TH2)
        {
            stPresenceProb->ashwProbGlb[shwI] = MED_EANR_MAX16;
        }
    }

    /*计算子带1~Common->shwOpertBandNum-1的平均Local有音概率，16bit Q15*/
    shwMeanProb = CODEC_OpVecMeanAbs(stPresenceProb->ashwProbLcl + 1, stCommon->shwOpertBandNum - 2);

    /*当平均Local有音概率小于MED_EANR_Q13(0.25->8192 Q15)时，子带
    MED_EANR_PRO_BIN_IDXA~MED_EANR_PRO_BIN_IDXB的Local有音概率设为设定的最小值*/
    if(shwMeanProb < MED_EANR_Q13)
    {
        CODEC_OpVecSet(stPresenceProb->ashwProbLcl + MED_EANR_PRO_BIN_IDXA,
                     MED_EANR_PRO_BIN_IDXB - MED_EANR_PRO_BIN_IDXA,
                     MED_EANR_PROB_MIN);
    }

    /*当平均Local有音概率小于MED_EANR_Q14(0.5->16384 Q15)并且当前大于
    MED_EANR_PRESENCEPROB_FRMTH(120)帧时对子带4~Common->shwOpertBandNum-2执行下列处理*/
    if ((shwMeanProb < MED_EANR_Q14) && (stCommon->swIdx >= MED_EANR_PRESENCEPROB_FRMTH))
    {
        for(shwI = 4; shwI < (stCommon->shwOpertBandNum - 2); shwI++)
        {
            /*当当前子带噪声功率大于前后子带噪声功率之和的MED_EANR_PRESENCEPROB_POWTH(2.5->20480 Q13)
            倍时，当前和前后子带的Local有音概率设为设定的最小值*/
            swTmp = CODEC_OpL_add(aswNoisePowLong[shwI+1], aswNoisePowLong[shwI-1]);

            swTmp = CODEC_OpL_mpy_32_16(swTmp, MED_EANR_PRESENCEPROB_POWTH);

            if ((aswNoisePowLong[shwI] >> 2) > swTmp)
            {
                stPresenceProb->ashwProbLcl[shwI-1] = MED_EANR_PROB_MIN;

                stPresenceProb->ashwProbLcl[shwI]   = MED_EANR_PROB_MIN;

                stPresenceProb->ashwProbLcl[shwI+1] = MED_EANR_PROB_MIN;
            }
        }
    }
}

/*************************************************************************************************
函数名称: MED_EANR_AbsenceProb_FIX
功能描述: 计算语音不存在概率
修改历史: 1. 2013年7月27日，新生成函数
**************************************************************************************************/

void MED_EANR_AbsenceProb_FIX(MED_EANR_COMM_STRU *stCommon,
                              MED_EANR_ABSENCEPROB_STRU *stAbsenceProb,
                              MED_INT16 *ashwProbLcl,
                              MED_INT16 *ashwProbGlb)
{
    MED_INT16 shwMeanSNR;
    MED_INT16 shwSNR;
    MED_INT16 shwI;
    MED_INT16 shwPresenProb;
    MED_INT16 shwTmp;
    MED_INT16 ashwTmp[MED_EANR_MAXBINNUM];

    /*上一帧的频域先验信噪比均值，16bit Q7*/
    shwMeanSNR  = stAbsenceProb->shwMeanPriSNR;

    /*计算本帧频域先验信噪比均值，子带MED_EANR_PROABSENCE_BIN_IDXC-1
    ~Common->shwOpertBandNum-MED_EANR_PROABSENCE_BIN_IDXC+1，16bit Q7*/
    stAbsenceProb->shwMeanPriSNR
                = CODEC_OpVecMeanAbs(stCommon->ashwSNRSmth + (MED_EANR_PROABSENCE_BIN_IDXC - 1),
                                   stCommon->shwOpertBandNum - (MED_EANR_PROABSENCE_BIN_IDXC - 1));

    /*计算本帧和上一帧频域先验信噪比均值之差，16bit Q7*/
    shwMeanSNR  = CODEC_OpSub(stAbsenceProb->shwMeanPriSNR, shwMeanSNR);

    /*计算本帧频域先验信噪比均值的对数值16bit Q6(Q11+Q10-Q15); 10->20480(Q11)*/
    shwSNR      = CODEC_OpMult(20480,
                             CODEC_OpLog10(7, (MED_INT32)(CODEC_OpMax(stAbsenceProb->shwMeanPriSNR, 1))));

    /*如果本帧频域先验信噪比均值小于等于MED_EANR_ABSENCEPROB_SNRTH(-10dB->-640 Q6)，
    则平均有音概率设为设定的最小值MED_EANR_PROB_MIN(0.005->164 Q15)*/
    if (shwSNR <= MED_EANR_ABSENCEPROB_SNRTH)
    {
        shwPresenProb = MED_EANR_PROB_MIN;
    }
    /*如果本帧和上一帧频域先验信噪比均值之差大于等于0,则平均有音概率设为设定的
    最大值MED_EANR_MAX16(1.0->32767 Q15)同时计算本帧频域先验信噪比均值对数值的限幅值
    (MED_EANR_PROABSENCE_SNR_L_LIMIT与MED_EANR_PROABSENCE_SNR_H_LIMIT之间)*/
    else if(shwMeanSNR >= 0)
    {
        stAbsenceProb->shwPriSNR = CODEC_OpMin(CODEC_OpMax(shwSNR, MED_EANR_PROABSENCE_SNR_L_LIMIT),
                                             MED_EANR_PROABSENCE_SNR_H_LIMIT);
        shwPresenProb = MED_EANR_MAX16;
    }
    /*如果本帧频域先验信噪比均值大于等于限幅值减MED_EANR_ABSENCEPROB_SNRPENALTYA
    (5->320 Q6)，平均有音概率设为设定的最大值*/
    else if(shwSNR >= (stAbsenceProb->shwPriSNR - MED_EANR_ABSENCEPROB_SNRPENALTYA))
    {
        shwPresenProb = MED_EANR_MAX16;
    }
    /*如果本帧频域先验信噪比均值小于等于限幅值减MED_EANR_ABSENCEPROB_SNRPENALTYB
    (10dB->640 Q6)，平均有音概率设为设定的最小值*/
    else if(shwSNR <= (stAbsenceProb->shwPriSNR - MED_EANR_ABSENCEPROB_SNRPENALTYB))
    {
        shwPresenProb = MED_EANR_PROB_MIN;
    }
    /*如果不满足上述条件，平均有音概率如下计算，16bit Q15*/
    else
    {
        shwTmp        = CODEC_OpAdd(CODEC_OpSub(shwSNR, stAbsenceProb->shwPriSNR),
                                  MED_EANR_ABSENCEPROB_SNRPENALTYB);

        shwTmp        = CODEC_OpMult((MED_INT16)((shwTmp << 6) / MED_EANR_ABSENCEPROB_DIVISOR),
                                   CODEC_OpSub(MED_EANR_MAX16, MED_EANR_PROB_MIN));

        shwTmp        = CODEC_OpAdd(MED_EANR_PROB_MIN, CODEC_OpShl(shwTmp, 3));

        shwPresenProb = shwTmp;

    }

    /*计算各子带语音不存在概率(1-P_GLB[i]*P_LCL[i]*P_frame)，并设上限
    MED_EANR_PROABSENCE_ABSENCE_PRO_MAX，16bit Q15*/
    CODEC_OpVvMult(ashwProbLcl, ashwProbLcl, stCommon->shwOpertBandNum, ashwTmp);

    CODEC_OpVcMult(ashwTmp, stCommon->shwOpertBandNum, shwPresenProb, ashwTmp);

    for(shwI=0;shwI<stCommon->shwOpertBandNum;shwI++)
    {
        stAbsenceProb->ashwAbsenProb[shwI] = CODEC_OpSub(MED_EANR_MAX16, ashwTmp[shwI]);

        stAbsenceProb->ashwAbsenProb[shwI] = CODEC_OpMin(stAbsenceProb->ashwAbsenProb[shwI],
                                                       MED_EANR_PROABSENCE_ABSENCE_PRO_MAX);
    }

}

/*************************************************************************************************
函数名称: MED_EANR_Gain_FIX
功能描述: 计算增益
修改历史: 1. 2013年7月27日，新生成函数
**************************************************************************************************/

void MED_EANR_Gain_FIX(MED_EANR_COMM_STRU *stCommon,
                       MED_EANR_GAIN_STRU *stGain,
                       MED_INT16 shwSNRMin,
                       MED_INT32 *aswPowSpct,
                       MED_INT16 *ashwAbsenProb)
{
    MED_INT16 shwI;
    MED_INT16 shwTmp;
    MED_INT32 aswPstSNR[MED_EANR_MAXBINNUM]      = {0};
    MED_INT16 ashwPriSNR[MED_EANR_MAXBINNUM]     = {0};
    MED_INT16 ashwPresenProb[MED_EANR_MAXBINNUM] = {0};
    MED_INT16 ashwPresenGain[MED_EANR_MAXBINNUM] = {0};
    MED_INT16 ashwAbsenGain[MED_EANR_MAXBINNUM]  = {0};
    MED_INT16 ashwSNRFact[MED_EANR_MAXBINNUM]    = {0};
    MED_INT32 aswNoisePowSpt[MED_EANR_MAXBINNUM] = {0};
    MED_INT32 swTmp1;
    MED_INT32 aswTmp2[MED_EANR_MAXBINNUM]        = {0};
    MED_INT32 aswTmp3[MED_EANR_MAXBINNUM]        = {0};
    MED_INT16 ashwTmp4[MED_EANR_MAXBINNUM];
    MED_INT32 swTmp5;

    /*当后验信噪比小于1时，如下更新先验信噪比，16bit Q7*/
    CODEC_OpVcMult(stGain->ashwPriSNRMid,
                 stCommon->shwOpertBandNum,
                 MED_EANR_SPEECHPROB_SNR_SMTH_COEF,
                 ashwPriSNR);

    CODEC_OpVecShr32(stCommon->aswNoisePow,
                   stCommon->shwOpertBandNum,
                   CODEC_OpSub(6, (MED_INT16)(stCommon->shwNormShift << 1)),
                   aswTmp2);

    for(shwI = 0; shwI < stCommon->shwOpertBandNum; shwI++)
    {
        /*计算后验信噪比，32bit Q11*/
        aswPstSNR[shwI] = CODEC_OpNormDiv_32(aswPowSpct[shwI],
                                           CODEC_OpL_max(1, aswTmp2[shwI]),
                                           11);

        aswPstSNR[shwI] = CODEC_OpL_max(aswPstSNR[shwI], 1);

        /*当后验信噪比大于等于MED_EANR_Q11(1->2048 Q11)时,更新先验信噪比,16bit Q7*/
        if (aswPstSNR[shwI] >= MED_EANR_Q11)
        {
            swTmp5           = CODEC_OpL_sub(aswPstSNR[shwI], MED_EANR_Q11);
            swTmp1           = CODEC_OpL_min(swTmp5,
                                           (MED_INT32)MED_EANR_PSTSNRMAX);

            shwTmp           = CODEC_OpSub(MED_EANR_MAX16,
                                         MED_EANR_SPEECHPROB_SNR_SMTH_COEF);

            swTmp1           = CODEC_OpL_mpy_32_16_r(swTmp1, shwTmp >> 4);

            ashwPriSNR[shwI] = CODEC_OpAdd(ashwPriSNR[shwI], CODEC_OpSaturate(swTmp1));
        }

        /*给先验信噪比设下限shwSNRMin*/
        ashwPriSNR[shwI]     = CODEC_OpMax(ashwPriSNR[shwI], shwSNRMin);

        /*计算有音增益，16bit Q15*/
        swTmp5               = ((VOS_INT32)ashwPriSNR[shwI]) << 15;
        ashwPresenGain[shwI] = CODEC_OpSaturate(swTmp5 / CODEC_OpAdd(MED_EANR_Q7, ashwPriSNR[shwI]));

        /*计算积分下限，16bit Q11*/
        swTmp1               = CODEC_OpL_mpy_32_16_r(aswPstSNR[shwI], ashwPresenGain[shwI]);

        shwTmp               = CODEC_OpSaturate(swTmp1);
        ashwSNRFact[shwI]    = CODEC_OpMax(shwTmp, 1);

        ashwPresenProb[shwI] = 0;

        /*当无音概率小于MED_EANR_GAIN_ABSENCEPROBTH(0.9->29491 Q15)时
          如下计算有音概率，否则有音概率为0*/
        if(ashwAbsenProb[shwI] < MED_EANR_GAIN_ABSENCEPROBTH)
        {
            aswTmp2[shwI]        = CODEC_OpL_Exp(ashwSNRFact[shwI], 11, 8);

            shwTmp               = CODEC_OpSub(MED_EANR_MAX16, ashwAbsenProb[shwI]);

            aswTmp2[shwI]        = CODEC_OpL_mpy_32_16_r(aswTmp2[shwI],shwTmp);

            shwTmp               = CODEC_OpAdd(MED_EANR_Q7, ashwPriSNR[shwI]);

            swTmp1               = CODEC_OpL_mult(ashwAbsenProb[shwI], shwTmp) >> 15;

            ashwPresenProb[shwI] = CODEC_OpSaturate(CODEC_OpNormDiv_32(aswTmp2[shwI],
                                                    CODEC_OpL_add(aswTmp2[shwI], swTmp1),
                                                    15));
        }

        /*当积分下限小于MED_EANR_GAIN_SNRFACTORTH(5->10240 Q11)时，
          如下计算有音增益，16bit Q11*/
        ashwPresenGain[shwI]     = ashwPresenGain[shwI] >> 4;

        if(ashwSNRFact[shwI] <= MED_EANR_GAIN_SNRFACTORTH)
        {
            swTmp1 = CODEC_OpL_Integral(ashwSNRFact[shwI]);

            swTmp1 = CODEC_OpL_Exp(CODEC_OpRound(swTmp1), 12, 15);

            swTmp1 = CODEC_OpL_mpy_32_16(swTmp1, ashwPresenGain[shwI]);

            ashwPresenGain[shwI] = CODEC_OpSaturate(swTmp1);
        }

    }

    UCOM_MemCpy(aswNoisePowSpt,
                stCommon->aswNoisePow,
                (MED_UINT16)stCommon->shwOpertBandNum * sizeof(MED_INT32));

    /*对子带1~Common->shwSubBandNum，取当前子带和前后子带的最小值为当前子带的噪声功率，32bit Q6*/
    for(shwI = 1; shwI < stCommon->shwSubBandNum; shwI++)
    {
        aswNoisePowSpt[shwI] = CODEC_OpL_min( CODEC_OpL_min( stCommon->aswNoisePow[shwI-1] ,
                                                         stCommon->aswNoisePow[shwI+1]),
                                            stCommon->aswNoisePow[shwI]);
    }

    CODEC_OpVecShr32(stCommon->aswPowSpctSmth,
                   stCommon->shwOpertBandNum,
                   -2,
                   aswTmp2);

    CODEC_OpVecShr32(aswPowSpct,
                   stCommon->shwOpertBandNum,
                   (MED_INT16)(stCommon->shwNormShift << 1),
                   aswTmp3);

    CODEC_OpVvAdd32( aswTmp2,
                   aswTmp3,
                   stCommon->shwOpertBandNum,
                   stCommon->aswPowSpctSmth);

    /*计算无音增益，16bit Q11，同时更新信号功率的平滑值，32bit Q0*/
    for(shwI = 0; shwI < stCommon->shwOpertBandNum; shwI++)
    {
        stCommon->aswPowSpctSmth[shwI] = CODEC_OpL_mpy_32_16(stCommon->aswPowSpctSmth[shwI],
                                                           MED_EANR_GAIN_SMOOTHCOEF);

        swTmp1                         = CODEC_OpNormDiv_32(aswNoisePowSpt[shwI],
                                                          stCommon->aswPowSpctSmth[shwI] + 1,
                                                          16);

        ashwAbsenGain[shwI]            = CODEC_OpMult(stGain->shwSNRSqrtMin,
                                                    CODEC_OpSqrt(swTmp1));

        ashwAbsenGain[shwI]            = CODEC_OpMax(ashwAbsenGain[shwI], 1);
    }

    CODEC_OpVecShr32(aswPstSNR, stCommon->shwOpertBandNum, -4, aswTmp2);

    CODEC_OpVvMult(ashwPresenGain, ashwPresenGain, stCommon->shwOpertBandNum, ashwTmp4);

    /*计算增益，16bit Q12，上限截到MED_EANR_GAIN_UPLIMIT(1->4095, Q12)，
    同时更新先验信噪比计算的中间变量，16bit Q7*/
    for(shwI = 0;shwI < stCommon->shwOpertBandNum; shwI++)
    {
        shwTmp = ashwPresenGain[shwI] / CODEC_OpMax(ashwAbsenGain[shwI],1);

        swTmp1 = CODEC_OpL_Power(shwTmp, 0, ashwPresenProb[shwI], 15, 1);

        swTmp1 = CODEC_OpL_mult0(ashwAbsenGain[shwI], CODEC_OpSaturate(swTmp1));

        shwTmp = CODEC_OpSaturate(swTmp1);

        stGain->ashwGain[shwI]      = CODEC_OpMin(shwTmp, MED_EANR_GAIN_UPLIMIT);

        stGain->ashwPriSNRMid[shwI] = CODEC_OpSaturate(CODEC_OpL_mpy_32_16(aswTmp2[shwI],
                                                                       ashwTmp4[shwI]));
    }
}

/*************************************************************************************************
函数名称: MED_EANR_DATA_FIX
功能描述: 频域数据乘上增益
修改历史: 1. 2013年7月27日，新生成函数
**************************************************************************************************/

void MED_EANR_DATA_FIX(MED_EANR_COMM_STRU *stCommon,
                       MED_INT16 *ashwGain,
                       MED_INT16 *shwDataBuf)
{
    MED_INT16 shwI;
    MED_INT16 shwJ;

    for (shwI = 1; shwI < stCommon->shwSubBandNum; shwI++)
    {
        /*每个子带的各个频点同乘以该子带的增益，16bit Q0*/
        for (shwJ = MED_EANR_SUBBAND_FL[shwI];
             shwJ <= MED_EANR_SUBBAND_FH[shwI];
             shwJ++)
        {
            shwDataBuf[shwJ * 2]
                  = CODEC_OpSaturate(CODEC_OpL_mult0(shwDataBuf[shwJ * 2], ashwGain[shwI]) >> 12);

            shwDataBuf[(shwJ * 2) + 1]
                  = CODEC_OpSaturate(CODEC_OpL_mult0(shwDataBuf[(shwJ * 2)+ 1], ashwGain[shwI]) >> 12);
        }
    }

    /*除去直流和低频信号*/
    shwDataBuf[0] = 0;
    shwDataBuf[1] = 0;
    shwDataBuf[2] = 0;
    shwDataBuf[3] = 0;
    shwDataBuf[4] = 0;
    shwDataBuf[5] = 0;
    shwDataBuf[6] = 0;
    shwDataBuf[7] = 0;
}



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
