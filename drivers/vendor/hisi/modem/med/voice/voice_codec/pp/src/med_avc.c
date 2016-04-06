

/*****************************************************************************
  1 头文件包含
*****************************************************************************/

#include "codec_op_lib.h"
#include "codec_op_vec.h"
#include "med_gain.h"
#include "med_pp_comm.h"
#include "med_avc.h"

//#include "ucom_comm.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
/* AVC实体资源池 */
MED_AVC_OBJ_STRU    g_astMedAvcObjPool[MED_AVC_MAX_OBJ_NUM];

/* 噪声分级相关, Q15 */
VOS_INT16           g_ashwMedAvcStepLag[MED_AVC_NOISE_STEPS_NUM] = {16423, 16423, 16423, 16423};

/* 增益对照表, Q11 */
VOS_INT16           g_ashwMedAvcGainTab[MED_AVC_NOISE_STEPS_NUM] = {16268, 8153, 4086, 2048};

/* 相关的信息 */
MED_OBJ_INFO        g_stMedAvcObjInfo;

/*****************************************************************************
  3 函数实现
*****************************************************************************/

VOS_VOID* MED_AVC_Create(VOS_VOID)
{
    return (MED_PP_Create(
                    MED_AVC_GetObjInfoPtr,
                    MED_AVC_GetObjPtr,
                    MED_AVC_MAX_OBJ_NUM,
                    sizeof(MED_AVC_OBJ_STRU)));
}


VOS_UINT32 MED_AVC_Destroy(VOS_VOID **ppstObj)
{
    VOS_UINT32              uwRet;
    MED_AVC_OBJ_STRU       *pstObj;
    pstObj      = (MED_AVC_OBJ_STRU *)(*ppstObj);

    /* 判断入参是否合法 */
    uwRet       = MED_PP_CheckPtrValid(MED_AVC_GetObjInfoPtr, pstObj);

    /* 入参合法，则将指针对象释放，并将指针赋空 */
    if (UCOM_RET_SUCC == uwRet)
    {
        UCOM_MemSet(pstObj, 0, sizeof(MED_AVC_OBJ_STRU));
        pstObj->enIsUsed = CODEC_OBJ_UNUSED;
        *ppstObj = MED_NULL;
    }

    return uwRet;
}


VOS_UINT32 MED_AVC_GetPara(
                VOS_VOID               *pstInstance,
                VOS_INT16              *pshwParam,
                VOS_INT16               shwParaLen)
{
    VOS_UINT32                uwRet;
    MED_AVC_OBJ_STRU         *pstAvc    = (MED_AVC_OBJ_STRU*)pstInstance;       /* 临时AVC指针 */
    MED_AVC_NV_STRU          *pstNv     = &(pstAvc->stNv);                      /* NV项指针 */

    /* 判断入参是否合法 */
    uwRet       = MED_PP_CheckPtrValid(MED_AVC_GetObjInfoPtr, pstAvc);

    /* 入参合法，则尝试对指针所在位置进行配置项读取操作 */
    if (UCOM_RET_SUCC == uwRet)
    {
        /* BUFFER空间不够，或结构体的使能项不正确，则出错中止操作 */
        if (    (shwParaLen < MED_AVC_PARAM_LEN)
            ||  (pstNv->enEnable >= CODEC_SWITCH_BUTT))
        {
            uwRet = UCOM_RET_FAIL;
        }
        else
        {
            /* 获取使能项、滤波系数的信息 */
            CODEC_OpVecSet(pshwParam, shwParaLen, 0);
            pshwParam[0]    = (VOS_INT16)(pstNv->enEnable);                     /* 使能项 */
            pshwParam[1]    = pstNv->shwGainMaxInc;                             /* 最大上升 */
            pshwParam[2]    = pstNv->shwGainMaxDec;                             /* 最大下降 */
            pshwParam[3]    = pstNv->shwSensitivityOffset;                      /* 上行麦克风灵敏度偏置 */
            pshwParam[4]    = pstNv->shwAntiSatMaxLevel;                        /* 抗饱和处理最大增益 */
            CODEC_OpVecCpy(&pshwParam[5], pstNv->ashwNoiseStep, MED_AVC_NOISE_STEPS_NUM);
        }
    }

    return uwRet;

}
VOS_UINT32 MED_AVC_SetPara (
                VOS_VOID                        *pstInstance,
                VOS_INT16                       *pshwParam,
                VOS_INT16                        shwParaLen,
                MED_PP_MIC_NUM_ENUM_UINT16       enPpMicNum)
{
    VOS_UINT32           uwRet;
    MED_AVC_OBJ_STRU    *pstAvc         = (MED_AVC_OBJ_STRU*)pstInstance;       /* 临时AVC指针 */
    MED_AVC_NV_STRU     *pstNv          = &(pstAvc->stNv);                      /* NV项指针 */
    VOS_INT32            swCnt;                                                 /* 计数器 */

    /* 判断入参是否合法 */
    uwRet       = MED_PP_CheckPtrValid(MED_AVC_GetObjInfoPtr, pstAvc);

    /* 入参合法，则尝试对指针所在位置进行配置项读取操作 */
    if (UCOM_RET_SUCC == uwRet)
    {
        /* BUFFER空间不够，或结构体的使能项不正确，则出错中止操作 */
        if (    (shwParaLen < MED_AVC_PARAM_LEN)
            ||  (pshwParam[0] >= CODEC_SWITCH_BUTT))
        {
            uwRet = UCOM_RET_FAIL;
        }
        else
        {
            /* 赋值使能项，门限判决系数、人工语音检测门限，门限平滑系数 */
            pstNv->enEnable                    = (VOS_UINT16)pshwParam[0];      /* 使能项 */
            pstNv->shwGainMaxInc               = pshwParam[1];                  /* 最大上升,Q14 */
            pstNv->shwGainMaxDec               = pshwParam[2];                  /* 最大下降,Q14 */
            pstNv->shwSensitivityOffset        = pshwParam[3];                  /* 最大下降 */
            pstNv->shwAntiSatMaxLevel          = pshwParam[4];                  /* 抗饱和处理最大增益 */
            CODEC_OpVecCpy(pstNv->ashwNoiseStep, &pshwParam[5], MED_AVC_NOISE_STEPS_NUM);

            for (swCnt = 0; swCnt < MED_AVC_NOISE_STEPS_NUM; swCnt++)
            {
                pstAvc->aswMedAvcStep[swCnt]    = CODEC_OpL_mult0(pshwParam[5 + swCnt],
                                                                pshwParam[5 + swCnt]);  /* 噪声分级表，NV项 */
            }

            /* 获取帧长信息 */
            pstAvc->shwFrmLen                   = MED_PP_GetFrameLength();

            /* 根据帧长，确定成员变量的值 */
            /* 窄带 */
            if(MED_PP_MIC_NUM_1 == enPpMicNum)
            {
                if (CODEC_FRAME_LENGTH_NB == pstAvc->shwFrmLen)
                {
                    pstAvc->pshwWeightTab           = &g_ashwMedAvcWeightTab8k[0];          /* 听觉加权表 */
                    pstAvc->enFreqLen               = MED_AVC_FFT_LEN_8K;                   /* FFT的长度 */
                }
                /* 宽带 */
                else
                {
                    pstAvc->pshwWeightTab           = &g_ashwMedAvcWeightTab16k[0];         /* 听觉加权表 */
                    pstAvc->enFreqLen               = MED_AVC_FFT_LEN_16K;                  /* FFT的长度 */
                }
            }
            else
            {
                if (CODEC_FRAME_LENGTH_NB == pstAvc->shwFrmLen)
                {
                    pstAvc->pshwWeightTab           = &g_ashwMedAvcWeightTab8k[0];          /* 听觉加权表 */
                    pstAvc->enFreqLen               = MED_AVC_FFT_LEN_2MIC_8K;                   /* FFT的长度 */
                }
                /* 宽带 */
                else
                {
                    pstAvc->pshwWeightTab           = &g_ashwMedAvcWeightTab16k[0];         /* 听觉加权表 */
                    pstAvc->enFreqLen               = MED_AVC_FFT_LEN_2MIC_16K;                  /* FFT的长度 */
                }
            }



            CODEC_OpVecSet((VOS_INT16*)(&(pstAvc->aenVadFlag[0])),
                         MED_AVC_BUF_LEN,
                         CODEC_VAD_FLAG_SPEECH);                                     /* 初始化时，历史信息全为语音 */
            UCOM_MemSet(pstAvc->aswFrmLvl, 0, MED_AVC_BUF_LEN * sizeof(VOS_INT32));     /* 初始化时，历史信息全为0 */
            pstAvc->shwLastGain                 = MED_AVC_GAIN_INITIAL;                 /* 初始化Gain的值 */
            pstAvc->shwLastStep                 = MED_AVC_NOISE_STEPS_NUM;              /* 噪声级别数 */
            pstAvc->swNoiseLvl                  = MED_AVC_NOISE_LVL_INITIAL;            /* 估计的噪声能量 */
            pstAvc->enVadFlag                   = CODEC_VAD_FLAG_NOISE;
        }
    }

    return uwRet;

}
VOS_VOID MED_AVC_UpdateTx(
                MED_AVC_OBJ_STRU       *pstAvc,
                VOS_INT16              *pshwTxFreq,
                VOS_INT16               enVadFlag)
{

    VOS_INT16       swCnt               = 0;                                    /* 计数器 */
    VOS_INT32       swTmp;                                                      /* 临时变量 */
    VOS_INT32       swFrmLvl            = 0;                                    /* 帧能量，Q0 */

    /* 更新VAD标志 */
    pstAvc->enVadFlag                   = enVadFlag;

    /* 计算当前帧的能量水平 */
    for (swCnt = 1; swCnt < (pstAvc->enFreqLen >> 1); swCnt++)
    {
        swTmp       = CODEC_OpL_add(CODEC_OpL_mult0(pshwTxFreq[2 * swCnt],
                                                pshwTxFreq[2 * swCnt]),
                                  CODEC_OpL_mult0(pshwTxFreq[(2 * swCnt) + 1],
                                                pshwTxFreq[(2 * swCnt) + 1]));  /* 计算帧的能量，忽略直流 */
        swTmp       = CODEC_OpL_mpy_32_16(swTmp, pstAvc->pshwWeightTab[swCnt - 1]);/* pshwWeightTab: Q13，结果: Q-2 */
        swFrmLvl    = CODEC_OpL_add(swFrmLvl,
                                  CODEC_OpL_shl(swTmp, MED_PP_SHIFT_BY_2));      /* 帧的能量水平，Q0 */
    }
    swFrmLvl        = CODEC_OpL_shl(swFrmLvl, 1);                                 /* 对称，所以需要*2 */

    /* 将当前帧的能量水平存到AVC结构体内 */
    pstAvc->swTxFrmLvl = swFrmLvl;
}


VOS_VOID MED_AVC_EstBgNoise(MED_AVC_OBJ_STRU *pstAvc)
{
    VOS_INT16           shwIndex;                                               /* 噪声级别序列的下标 */
    MED_AVC_NV_STRU    *pstNv          = &(pstAvc->stNv);                       /* NV项指针 */


    /* 麦克风灵敏度偏置 */
    pstAvc->swTxFrmLvl = CODEC_OpL_shl( CODEC_OpL_mpy_32_16((pstAvc->swTxFrmLvl),
                                                         pstNv->shwSensitivityOffset),
                                      (MED_PP_SHIFT_BY_3));

    /* 缓存当前Tx帧能量 */
    UCOM_MemCpy(&(pstAvc->aswFrmLvl[0]),
                &(pstAvc->aswFrmLvl[1]),
                (MED_AVC_BUF_LEN - 1) * sizeof(VOS_INT32));
    pstAvc->aswFrmLvl[MED_AVC_BUF_LEN - 1] = pstAvc->swTxFrmLvl;

    /* 缓存当前Tx帧的VAD标志 */
    UCOM_MemCpy(&(pstAvc->aenVadFlag[0]),
                &(pstAvc->aenVadFlag[1]),
                (MED_AVC_BUF_LEN - 1) * sizeof(CODEC_VAD_FLAG_ENUM_INT16));
    pstAvc->aenVadFlag[MED_AVC_BUF_LEN - 1] = pstAvc->enVadFlag;

    if (CODEC_OpVecSum((VOS_INT16*)(pstAvc->aenVadFlag), MED_AVC_BUF_LEN) < MED_AVC_NOISE_UPDATE_MAX_VOICE_ACTIVE)
    {
        /* 更新噪声级别 */
        shwIndex           = CODEC_OpShr(MED_AVC_BUF_LEN,1);
        pstAvc->swNoiseLvl = CODEC_OpL_add (CODEC_OpL_mpy_32_16(pstAvc->swNoiseLvl,
                                                            MED_AVC_NOISE_UPDATE_ALPHA),
                                          CODEC_OpL_mpy_32_16(pstAvc->aswFrmLvl[shwIndex - 1],
                                                           (CODEC_OP_INT16_MAX - MED_AVC_NOISE_UPDATE_ALPHA)));
    }
}


VOS_VOID MED_AVC_GainRxVol(
                MED_AVC_OBJ_STRU       *pstAvc,
                VOS_INT16              *pshwRxFrm,
                VOS_INT16               shwRxDevLinearGain)                     /* Q12 */
{
    MED_AVC_NV_STRU *pstNv              = &(pstAvc->stNv);                      /* NV项指针 */
    VOS_INT32       swCnt;                                                      /* 计数器 */
    VOS_INT16       shwStep;                                                    /* 目标增益级别 */
    VOS_INT16       shwLastStep         = pstAvc->shwLastStep;                  /* 历史增益级别 */
    VOS_INT16       shwLastGain         = pstAvc->shwLastGain;                  /* 历史增益 */
    VOS_INT32       swStepUpdateThd;                                            /* 目标增益级别更新门限 */
    VOS_INT16       shwTemp;                                                    /* 临时变量 */
    VOS_INT32       swTemp;                                                     /* 临时变量 */
    VOS_INT16       shwRxMax;                                                   /* 下行信号帧中的幅度最大值 */
    VOS_INT16       shwRealMax;                                                 /* 下行信号帧中的幅度最大值的修正值 */
    VOS_INT16       shwGain;                                                    /* AVC的增益 */
    VOS_INT16       shwPermitGain;                                              /* PERMIT GAIN, Q11 */

    /* 搜索目标增益级别 */
    shwStep             = MED_AVC_NOISE_STEPS_NUM - 1;
    for (swCnt = 0; swCnt < MED_AVC_NOISE_STEPS_NUM; swCnt++)
    {
       if (pstAvc->swNoiseLvl > pstAvc->aswMedAvcStep[swCnt])
       {
           shwStep      = (VOS_INT16)swCnt;
           break;
       }
    }

    /* 抗抖动处理 */
    if ((shwStep - 1) == shwLastStep)
    {
       swStepUpdateThd = CODEC_OpL_mpy_32_16(pstAvc->aswMedAvcStep[shwLastStep],
                                           MED_AVC_GetNoiseStepLag(shwLastStep));
       if (pstAvc->swNoiseLvl  > swStepUpdateThd)
       {
           shwStep      = shwLastStep;
       }
    }
    pstAvc->shwLastStep = shwStep;

    /* 更新增益,shwGain, 16bit Q11 */
    shwGain     = CODEC_OpAdd(CODEC_OpMult(MED_AVC_GAIN_UPDATE_ALPHA, shwLastGain),
                            CODEC_OpMult(CODEC_OP_INT16_MAX - MED_AVC_GAIN_UPDATE_ALPHA,
                                       MED_AVC_GetGainFromTab(shwStep)));

    /* 对增益进行最大限幅 */
    shwTemp     = CODEC_OpMult_shr(shwLastGain,
                                 pstNv->shwGainMaxInc,
                                 MED_PP_SHIFT_BY_14);                          /* Q11 */
    shwGain     = CODEC_OpMin(shwGain, shwTemp);

    /* 对增益进行最小限幅 */
    shwTemp     = CODEC_OpMult_shr(shwLastGain,
                                 pstNv->shwGainMaxDec,
                                 MED_PP_SHIFT_BY_14);                          /* Q11 */
    shwGain     = CODEC_OpMax(shwGain, shwTemp);

    /* 取Rx序列的最大值 */
    shwRxMax    = CODEC_OpVecMax(pshwRxFrm, (VOS_INT32)(pstAvc->shwFrmLen), 0);

    /* 当下行匹配增益小于1时,不考虑下行匹配增益的效果 */
    shwRxDevLinearGain= CODEC_OpMax(MED_AVC_RX_DEV_GAIN_MIN, shwRxDevLinearGain);

    /* 增益抗饱和 */
    shwRealMax  =  CODEC_OpMult_shr(CODEC_OpMult_shr(shwRxMax, shwGain, MED_PP_SHIFT_BY_11),
                                  shwRxDevLinearGain,
                                  MED_PP_SHIFT_BY_12);
    if  (shwRealMax > pstNv->shwAntiSatMaxLevel)
    {
        swTemp  = CODEC_OpL_mult0(shwRxMax, shwRxDevLinearGain);                  /* swTemp: Q12 */
        shwPermitGain = CODEC_OpSaturate((CODEC_OpNormDiv_32((VOS_INT32)(pstNv->shwAntiSatMaxLevel),
                                                     swTemp,
                                                     MED_PP_SHIFT_BY_11
                                                     + MED_PP_SHIFT_BY_12)));   /* shwPermitGain: 16bit Q11 */
        shwGain = CODEC_OpAdd(CODEC_OpMult(MED_AVC_ANTI_SAT_ALPHA, shwGain) ,
                            CODEC_OpMult((CODEC_OP_INT16_MAX - MED_AVC_ANTI_SAT_ALPHA),
                                       shwPermitGain));
    }

    /* AVC 不会衰减原有的音量 */
    shwGain     = CODEC_OpMax(MED_AVC_GAIN_MIN, shwGain);                         /* 16bit Q11 */
    pstAvc->shwLastGain = shwGain;

    /* 施加增益 */
    CODEC_OpVcMultScale(pshwRxFrm,
                      (signed)MED_PP_GetFrameLength(),
                      shwGain,
                      MED_PP_SHIFT_BY_12,
                      pshwRxFrm);
}


VOS_UINT32 MED_AVC_Main(
                VOS_VOID                *pstMedAvcObj,
                VOS_INT16               *pshwRxFrm,
                VOS_INT16                shwRxDevGain)
{
    MED_AVC_NV_STRU     *pstNv;                                                 /* NV项指针 */
    VOS_INT16            shwRxDevLinearGain;                                    /* 线性增益，Q12 */
    MED_AVC_OBJ_STRU    *pstMedAvc;

    pstMedAvc = (MED_AVC_OBJ_STRU*)pstMedAvcObj;
    pstNv     = &(pstMedAvc->stNv);

    if (CODEC_SWITCH_OFF == pstNv->enEnable)
    {
        /* AVC不使能 */
        return UCOM_RET_SUCC;
    }

    /* 噪声水平估计 */
    MED_AVC_EstBgNoise(pstMedAvc);

    /* 增益控制 */
    shwRxDevLinearGain = CODEC_OpShr_r(MED_AVC_Db2Linear(shwRxDevGain), 1);       /* MED_AVC_Db2Linear的输出为Q13，将其移到Q12 */
    MED_AVC_GainRxVol(pstMedAvc, pshwRxFrm, shwRxDevLinearGain);

    return UCOM_RET_SUCC;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

