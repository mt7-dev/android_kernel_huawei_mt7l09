

/*****************************************************************************
  1 头文件包含
*****************************************************************************/

#include "codec_op_lib.h"
#include "codec_op_vec.h"
#include "med_agc.h"
#include "med_pp_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
MED_AGC_OBJ_STRU    g_astMedAgcObjPool[MED_AGC_MAX_OBJ_NUM];                    /* AGC实体资源池 */
MED_OBJ_INFO        g_stMedAgcObjInfo;                                          /* AGC信息 */

/*****************************************************************************
  3 函数实现
*****************************************************************************/


VOS_VOID* MED_AGC_Create(VOS_VOID)
{
    return (MED_PP_Create(
                    MED_AGC_GetObjInfoPtr,
                    MED_AGC_GetObjPtr,
                    MED_AGC_MAX_OBJ_NUM,
                    sizeof(MED_AGC_OBJ_STRU)));
}


VOS_UINT32 MED_AGC_Destroy(VOS_VOID **ppstObj)
{
    VOS_UINT32              uwRet;
    MED_AGC_OBJ_STRU       *pstObj;
    pstObj      = (MED_AGC_OBJ_STRU *)(*ppstObj);

    /* 判断入参是否合法 */
    uwRet       = MED_PP_CheckPtrValid(MED_AGC_GetObjInfoPtr, pstObj);

    /* 入参合法，则将指针对象释放，并将指针赋空 */
    if (UCOM_RET_SUCC == uwRet)
    {
        UCOM_MemSet(pstObj, 0, sizeof(MED_AGC_OBJ_STRU));
        pstObj->enIsUsed = CODEC_OBJ_UNUSED;
        *ppstObj = MED_NULL;
    }

    return uwRet;
}


VOS_UINT32 MED_AGC_GetPara(
                VOS_VOID               *pstInstance,
                VOS_INT16              *pshwParam,
                VOS_INT16               shwParaLen)
{
    VOS_UINT32              uwRet;
    MED_AGC_OBJ_STRU       *pstObj    = (MED_AGC_OBJ_STRU *)pstInstance;
    MED_AGC_NV_STRU        *pstNv     = &(pstObj->stNv);

    /* 判断入参是否合法 */
    uwRet       = MED_PP_CheckPtrValid(MED_AGC_GetObjInfoPtr, pstObj);

    /* 入参合法，则尝试对指针所在位置进行配置项读取操作 */
    if (UCOM_RET_SUCC == uwRet)
    {
        /* BUFFER空间不够，或结构体的使能项不正确，则出错中止操作 */
        if (    (shwParaLen < MED_AGC_PARAM_LEN)
            ||  (pstNv->enEnable >= CODEC_SWITCH_BUTT))
        {
            uwRet = UCOM_RET_FAIL;
        }
        else
        {
            /* 获取使能项、最大输入、最大输出、底噪、偏移量、底噪清零使能项的信息 */
            CODEC_OpVecSet(pshwParam, shwParaLen, 0);
            pshwParam[0] = (VOS_INT16)(pstNv->enEnable);
            pshwParam[1] = pstNv->shwMaxIn;
            pshwParam[2] = pstNv->shwMaxOut;
            pshwParam[3] = pstNv->shwFlrNoise;
            pshwParam[4] = pstNv->shwOffset;
            pshwParam[5] = (VOS_INT16)pstNv->enClearNoiseEn;
        }
    }

    return uwRet;

}


VOS_UINT32 MED_AGC_SetPara (
                VOS_VOID               *pstInstance,
                VOS_INT16              *pshwParam,
                VOS_INT16               shwParaLen)
{
    VOS_UINT32              uwRet;
    MED_AGC_OBJ_STRU       *pstObj    = (MED_AGC_OBJ_STRU *)pstInstance;
    MED_AGC_NV_STRU        *pstNv     = &(pstObj->stNv);
    VOS_INT16               shwTemp1, shwTemp2;
    VOS_INT32               swTemp3,  swTemp4;

    /* 判断入参是否合法 */
    uwRet       = MED_PP_CheckPtrValid(MED_AGC_GetObjInfoPtr, pstObj);
    /* 防止除零错误*/
    if(0 == (pshwParam[1] - pshwParam[3]))
    {
        uwRet = UCOM_RET_FAIL;
    }

    /* 入参合法，则尝试对指针所在位置进行配置项写入操作 */
    if (UCOM_RET_SUCC == uwRet)
    {
        /* BUFFER空间不够，或入参的使能项不正确，则出错中止操作 */
        if (    (shwParaLen < MED_AGC_PARAM_LEN)
            ||  (pshwParam[0] >= CODEC_SWITCH_BUTT))
        {
            uwRet = UCOM_RET_FAIL;
        }
        else
        {
            /* 获取使能项、最大输入、最大输出、底噪、偏移量、底噪清零使能项的信息 */
            pstNv->enEnable       = (VOS_UINT16)pshwParam[0];
            pstNv->shwMaxIn       = pshwParam[1];
            pstNv->shwMaxOut      = pshwParam[2];
            pstNv->shwFlrNoise    = pshwParam[3];
            pstNv->shwOffset      = pshwParam[4];
            pstNv->enClearNoiseEn = (VOS_UINT16)pshwParam[5];

            /* 历史增益信息初始化 */
            pstObj->shwLastGain = MED_AGC_LASTGAIN_INIT;

            /* 获取帧长信息 */
            pstObj->shwFrmLen   = MED_PP_GetFrameLength();

            /* 计算直线参数 */
            /* 计算斜率LineK = (MaxOut - Offset)/(MaxIn - FlrNoise) */
            shwTemp1        = CODEC_OpSub(pstNv->shwMaxOut, pstNv->shwOffset);
            shwTemp2        = CODEC_OpSub(pstNv->shwMaxIn, pstNv->shwFlrNoise);
            pstObj->shwLineK= CODEC_OpDiv_s(shwTemp1, shwTemp2);                          /* 16bit Q15 */

            /* 计算上移幅度LineB = (MaxIn*Offset - MaxOut*FlrNoise)/(MaxIn - FlrNoise) */
            swTemp3         = CODEC_OpL_mult0(pstNv->shwMaxIn, pstNv->shwOffset);         /* 临时变量，Q0 */
            swTemp4         = CODEC_OpL_mult0(pstNv->shwMaxOut, pstNv->shwFlrNoise);      /* 临时变量，Q0 */
            swTemp3         = CODEC_OpL_sub(swTemp3, swTemp4);                            /* 临时变量，Q0 */
            swTemp4         = CODEC_OpL_deposit_l(CODEC_OpSub(pstNv->shwMaxIn,
                                                pstNv->shwFlrNoise));                   /* 临时变量，Q0 */

            pstObj->shwLineB= CODEC_OpSaturate(swTemp3 / swTemp4);

        }
    }

    return uwRet;

}
VOS_UINT32 MED_AGC_Main(
                VOS_VOID               *pstAgcInstance,
                VOS_INT16              *pshwIn,
                VOS_INT16               shwVadFlag,
                VOS_INT16              *pshwOut)
{
    /* 局部变量初始化 */
    MED_AGC_OBJ_STRU   *pstAgc          = (MED_AGC_OBJ_STRU*)pstAgcInstance;
    MED_AGC_NV_STRU    *pstNv           = &(pstAgc->stNv);                      /* NV项 */
    VOS_INT16           shwLastGain     = pstAgc->shwLastGain;                  /* 上一帧的AGC增益 */
    VOS_INT16           shwFrmLen       = pstAgc->shwFrmLen;                    /* 帧长 */
    VOS_INT16           shwMaxOut       = pstNv->shwMaxOut;                     /* 最大输出 */
    VOS_INT16           shwAverEng;                                             /* 平均能量 */
    VOS_INT16           shwIncStep;                                             /* 上升步长 */
    VOS_INT16           shwDecStep;                                             /* 下降步长 */
    VOS_INT16           shwGain;                                                /* 当前 AGC 增益 */
    VOS_INT32           swCnt;                                                  /* 内部循环计数器 */
    VOS_INT32           swFrmOutQ11;                                            /* 临时变量 32bit,Q11 */
    VOS_INT16           shwGainMax;                                             /* 当前 AGC 增益允许的最大值 */
    VOS_INT16           shwGainMin;                                             /* 当前 AGC 增益允许的最小值 */

    /* 判断AGC模块是否已被创建 */
    if (CODEC_OBJ_UNUSED == pstAgc->enIsUsed)
    {
        /* 未初始化，返回错误 */
        return UCOM_RET_FAIL;
    }

    /* 判断AGC模块是否使能 */
    if (CODEC_SWITCH_OFF == pstNv->enEnable)
    {
        /* AGC不使能 */
        CODEC_OpVecCpy(pshwOut, pshwIn, shwFrmLen);
        return UCOM_RET_SUCC;
    }

    /* 计算帧平均能量 */
    shwAverEng      = CODEC_OpVecMeanAbs(pshwIn, shwFrmLen);
    shwAverEng      = CODEC_OpMax(shwAverEng, MED_AGC_AVERENG_MIN);

    /* 步长以及增益计算 */
    shwIncStep      = MED_AGC_INC_STEP_NORMAL;                                  /* 增益增加步长 16bit Q14 */

    /* 非语音帧或者能量太小 */
    if ((CODEC_VAD_FLAG_NOISE == shwVadFlag) || (shwAverEng < pstNv->shwFlrNoise))
    {
        shwGain     = MED_AGC_LASTGAIN_INIT;                                    /* 增益 16bit Q10 */
        shwDecStep  = MED_AGC_DEC_STEP_NORMAL;                                  /* 增益衰减步长 16bit Q15 */
    }
    /* 语音帧能量太大 */
    else if (shwAverEng > pstNv->shwMaxIn)
    {
        shwGain     = CODEC_OpSaturate(CODEC_OpNormDiv_32(CODEC_OpL_deposit_l(shwMaxOut),
                                     CODEC_OpL_deposit_l(shwAverEng), MED_PP_SHIFT_BY_10));
        shwDecStep  = MED_AGC_DEC_STEP_FAST;
    }
    /* 线性调节区 */
    else
    {
        shwGain     = CODEC_OpAdd(CODEC_OpShr_r(pstAgc->shwLineK, MED_PP_SHIFT_BY_5),
                                (VOS_INT16)CODEC_OpNormDiv_32((VOS_INT32)pstAgc->shwLineB, (VOS_INT32)shwAverEng,
                                MED_PP_SHIFT_BY_10));
        shwDecStep  = MED_AGC_DEC_STEP_NORMAL;                                  /* 下降步长，16bitQ15 */
    }

    /* 增益限制，增益的变化控制在下降步长以上，上升步长以下 */
    shwGainMax      = CODEC_OpSaturate(CODEC_OpL_shr_r(CODEC_OpL_mult0(shwLastGain, shwIncStep), MED_PP_SHIFT_BY_14));
    shwGainMin      = CODEC_OpMult_r(shwLastGain, shwDecStep);
    shwGain         = CODEC_OpMin(shwGain, shwGainMax);
    shwGain         = CODEC_OpMax(shwGain, shwGainMin);

    /* 更新历史增益 */
    pstAgc->shwLastGain = shwGain;

    /* 当前增益施加 */
    for (swCnt = 0; swCnt < shwFrmLen; swCnt++)
    {
        swFrmOutQ11     = CODEC_OpL_mult(pshwIn[swCnt], shwGain);
        pshwOut[swCnt] = CODEC_OpSaturate(CODEC_OpL_shr_r(swFrmOutQ11, MED_PP_SHIFT_BY_11));
    }

    return UCOM_RET_SUCC;
}
VOS_UINT32 MED_AGC_PreClearNoise(
                VOS_VOID               *pstAgcInstance,
                VOS_INT16              *psIn,
                VOS_INT16              *psOut)
{
    VOS_INT32           swPow           = 0;
    MED_AGC_OBJ_STRU   *pstAgc          = (MED_AGC_OBJ_STRU*)pstAgcInstance;
    MED_AGC_NV_STRU    *pstNv           = &(pstAgc->stNv);
    VOS_INT16           shwFrameLength  = MED_PP_GetFrameLength();

    /* 判断AGC前底噪清零是否使能 */
    if (CODEC_SWITCH_OFF == pstNv->enClearNoiseEn)
    {
        /* 底噪清零不使能 */
        CODEC_OpVecCpy(psOut, psIn, (VOS_INT32)shwFrameLength);
        return UCOM_RET_SUCC;
    }

    /*计算当前帧的能量*/
    swPow = CODEC_OpVvMac(psIn, psIn, (VOS_INT32)shwFrameLength, swPow);

    if (CODEC_FRAME_LENGTH_WB == shwFrameLength)
    {
        swPow = CODEC_OpL_shr(swPow, 2);
    }
    else
    {
        swPow = CODEC_OpL_shr(swPow, 1);
    }

    /*下行噪声优化*/
    /*如果能量小于-66dBm0，则清零本帧*/
    if(swPow < MED_AGC_CLEAR_RX_MUT)
    {
        CODEC_OpVecSet(psOut, (VOS_INT32)shwFrameLength, 0);
    }
    else
    {
        CODEC_OpVecCpy(psOut, psIn, (VOS_INT32)shwFrameLength);
    }

    return UCOM_RET_SUCC;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif





