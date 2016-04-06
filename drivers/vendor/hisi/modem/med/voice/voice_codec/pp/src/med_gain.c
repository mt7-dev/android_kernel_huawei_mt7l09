

/*****************************************************************************
  1 头文件包含
*****************************************************************************/

#include "codec_op_vec.h"
#include "med_pp_comm.h"
#include "med_gain.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
MED_SIDETONE_OBJ_STRU    g_astMedSideToneObjPool[MED_SIDETONE_MAX_OBJ_NUM];     /* 侧音叠加滤波器实体资源池 */
MED_DEVGAIN_OBJ_STRU     g_astMedDevGainObjPool[MED_DEVGAIN_MAX_OBJ_NUM];       /* 设备增益实体资源池 */
MED_OBJ_INFO             g_stMedSideToneObjInfo;                             /* 相关的信息 */
MED_OBJ_INFO             g_stMedDevGainObjInfo;                                 /* 相关的信息 */

/* dB与乘数因子映射表, Q13 */
VOS_INT16                g_ashwGainTab[93] =
{
     0,      1,      1,      1,      1,      1,      2,
     2,      2,      2,      3,      3,      3,      4,
     4,      5,      5,      6,      7,      7,      8,
     9,      10,     12,     13,     15,     16,     18,
     21,     23,     26,     29,     33,     37,     41,
     46,     52,     58,     65,     73,     82,     92,
     103,    116,    130,    146,    163,    183,    206,
     231,    259,    291,    326,    366,    411,    461,
     517,    580,    651,    730,    819,    919,    1031,
     1157,   1298,   1457,   1635,   1834,   2058,   2309,
     2591,   2907,   3261,   3659,   4106,   4607,   5169,
     5799,   6507,   7301,   8192,   9192,   10313,  11572,
     12983,  14568,  16345,  18340,  20577,  23088,  25905,
     29066,  32613
 };

/*****************************************************************************
  3 函数实现
*****************************************************************************/

VOS_VOID* MED_SIDETONE_Create(VOS_VOID)
{
    return (MED_PP_Create(
                    MED_SIDETONE_GetObjInfoPtr,
                    MED_SIDETONE_GetObjPtr,
                    MED_SIDETONE_MAX_OBJ_NUM,
                    sizeof(MED_SIDETONE_OBJ_STRU)));
}


VOS_UINT32 MED_SIDETONE_Destroy(VOS_VOID **ppstObj)
{
    VOS_UINT32              uwRet;
    MED_SIDETONE_OBJ_STRU  *pstObj;
    pstObj      = (MED_SIDETONE_OBJ_STRU *)(*ppstObj);

    /* 判断入参是否合法 */
    uwRet       = MED_PP_CheckPtrValid(MED_SIDETONE_GetObjInfoPtr, pstObj);

    /* 入参合法，则将指针对象释放，并将指针赋空 */
    if (UCOM_RET_SUCC == uwRet)
    {
        UCOM_MemSet(pstObj, 0, sizeof(MED_SIDETONE_OBJ_STRU));
        pstObj->enIsUsed = CODEC_OBJ_UNUSED;
        *ppstObj         = MED_NULL;
    }

    return uwRet;
}


VOS_UINT32 MED_SIDETONE_GetPara(
                VOS_VOID               *pstInstance,
                VOS_INT16              *pshwParam,
                VOS_INT16               shwParaLen)
{
    VOS_UINT32             uwRet;
    VOS_INT32              swCnt;
    MED_SIDETONE_OBJ_STRU *pstSideTone = (MED_SIDETONE_OBJ_STRU *)pstInstance;

    /* 判断入参是否合法 */
    uwRet       = MED_PP_CheckPtrValid(MED_SIDETONE_GetObjInfoPtr, pstInstance);

    /* 入参合法，则尝试对指针所在位置进行配置项读取操作 */
    if (UCOM_RET_SUCC == uwRet)
    {
        /* BUFFER空间不够,则出错中止操作 */
        if (shwParaLen < MED_SIDETONE_PARAM_LEN)
        {
            uwRet = UCOM_RET_FAIL;
        }
        else
        {
            /* 获取滤波系数的信息 */
            CODEC_OpVecSet(pshwParam, shwParaLen, 0);
            for (swCnt = MED_GAIN_MIN; swCnt <= MED_GAIN_MAX; swCnt++)
            {
                if (MED_GAIN_Db2Linear(swCnt) == pstSideTone->shwSetGain)
                {
                    break;
                }
            }

            pshwParam[0]    = (VOS_INT16)swCnt;                                        /* 施加增益 */
        }
    }

    return uwRet;

}
VOS_UINT32 MED_SIDETONE_SetPara (
                VOS_VOID               *pstInstance,
                VOS_INT16              *pshwParam,
                VOS_INT16               shwParaLen)
{
    VOS_UINT32             uwRet;
    MED_SIDETONE_OBJ_STRU *pstSideTone = (MED_SIDETONE_OBJ_STRU *)pstInstance;

    /* 判断入参是否合法 */
    uwRet       = MED_PP_CheckPtrValid(MED_SIDETONE_GetObjInfoPtr, pstInstance);

    /* 入参合法，则尝试对指针所在位置进行配置项设置操作 */
    if (UCOM_RET_SUCC == uwRet)
    {
        /* BUFFER空间不够，则出错中止操作 */
        if (shwParaLen < MED_SIDETONE_PARAM_LEN)
        {
            uwRet = UCOM_RET_FAIL;
        }
        else
        {
            /* 将ashwFrmDat成员变量全部设为0 */
            CODEC_OpVecSet(&(pstSideTone->ashwFrmDat[0]), (VOS_INT32)(pstSideTone->shwFrmLen), 0);

            /* dB增益转线性增益 */
            /* 1.判断入参是否合法 */
            if ((pshwParam[0] > MED_GAIN_MAX)||(pshwParam[0] < MED_GAIN_MIN))
            {
                return UCOM_RET_ERR_PARA;
            }

            /* 2.计算并保存线性增益 */
            pstSideTone->shwSetGain  = MED_GAIN_Db2Linear(pshwParam[0]);
            pstSideTone->shwCurGain  = MED_GAIN_Db2Linear(pshwParam[0]);

            /* 获取帧长信息 */
            pstSideTone->shwFrmLen= MED_PP_GetFrameLength();

        }
    }

    return uwRet;

}


VOS_UINT32 MED_SIDESTONE_UpdateTx (
                VOS_VOID               *pstSideToneInstance,
                VOS_INT16              *pshwTxIn)
{
    MED_SIDETONE_OBJ_STRU *pstSideTone = (MED_SIDETONE_OBJ_STRU*)pstSideToneInstance;/* 临时SIDETONE指针 */

    if((NULL == pshwTxIn) || (NULL == pstSideToneInstance))
    {
        return UCOM_RET_FAIL;
    }

    /* 将TX帧复制到侧音增益叠加结构体中 */
    CODEC_OpVecCpy(  &(pstSideTone->ashwFrmDat[0]),
                    pshwTxIn,
                    pstSideTone->shwFrmLen);
    return UCOM_RET_SUCC;
}


VOS_UINT32 MED_SIDETONE_Main(
                VOS_VOID               *pstInstance,
                VOS_INT16              *pshwRxIn,
                VOS_INT16              *pshwOut)
{
    VOS_INT16              shwTmp;

    /* 临时SIDETONE指针 */
    MED_SIDETONE_OBJ_STRU *pstSideTone = (MED_SIDETONE_OBJ_STRU*)pstInstance;
    VOS_INT32              swEnerSum;

    /* 计算当前帧的能量 */
    swEnerSum = 0;

    swEnerSum = CODEC_OpVvMac(pshwRxIn, pshwRxIn, pstSideTone->shwFrmLen,swEnerSum);
    swEnerSum = CODEC_OpL_shr(swEnerSum, 1);

    shwTmp    = MED_GAIN_MIN;

    /* 根据当前帧能量大小控制侧音增益 */
    if (swEnerSum > MED_SIDETONE_RX_LOUD)                                       /* 大于-30dBm0,则关闭sidetone */
    {

        pstSideTone->shwCurGain    = MED_GAIN_Db2Linear(shwTmp);
        pstSideTone->swSTMuteCount = 0;
    }
    else
    /* 如果此时处于sidetone关闭期(sideTone为最小增益)，则加计数并判断是否大于阈值 */
    {
        if (MED_GAIN_Db2Linear(shwTmp) == pstSideTone->shwCurGain)
        {
            pstSideTone->swSTMuteCount = pstSideTone->swSTMuteCount + 1;
            if(pstSideTone->swSTMuteCount > MED_PP_AGC_RX_DELAY)
            {
                pstSideTone->shwCurGain    = pstSideTone->shwSetGain;
                pstSideTone->swSTMuteCount = 0;
            }
        }
    }

    /* 侧音增益，即实现Out'(i) = Gain * SideSignal(i) */
    CODEC_OpVcMultScale(&(pstSideTone->ashwFrmDat[0]),
                      (VOS_INT32)(pstSideTone->shwFrmLen),
                      pstSideTone->shwCurGain,
                      MED_PP_SHIFT_BY_14,
                      &(pstSideTone->ashwFrmDat[0]));

    /* 侧音叠加，即实现Out(i) = RxIn(i) + Out'(i) */
    CODEC_OpVvAdd(pshwRxIn,
                &(pstSideTone->ashwFrmDat[0]),
                (VOS_INT32)(pstSideTone->shwFrmLen),
                pshwOut);

    return UCOM_RET_SUCC;
}
VOS_VOID* MED_DEVGAIN_Create(VOS_VOID)
{
    return (MED_PP_Create(
                    MED_DEVGAIN_GetObjInfoPtr,
                    MED_DEVGAIN_GetObjPtr,
                    MED_DEVGAIN_MAX_OBJ_NUM,
                    sizeof(MED_DEVGAIN_OBJ_STRU)));
}


VOS_UINT32 MED_DEVGAIN_Destroy(VOS_VOID **ppstObj)
{
    VOS_UINT32               uwRet;
    MED_DEVGAIN_OBJ_STRU    *pstObj;
    pstObj      = (MED_DEVGAIN_OBJ_STRU *)(*ppstObj);

    /* 判断入参是否合法 */
    uwRet       = MED_PP_CheckPtrValid(MED_DEVGAIN_GetObjInfoPtr, pstObj);

    /* 入参合法，则将指针对象释放，并将指针赋空 */
    if (UCOM_RET_SUCC == uwRet)
    {
        UCOM_MemSet(pstObj, 0, sizeof(MED_DEVGAIN_OBJ_STRU));
        pstObj->enIsUsed = CODEC_OBJ_UNUSED;
        *ppstObj         = MED_NULL;
    }

    return uwRet;
}


VOS_UINT32 MED_DEVGAIN_GetPara(
                VOS_VOID               *pstInstance,
                VOS_INT16              *pshwParam,
                VOS_INT16               shwParaLen)
{
    VOS_UINT32             uwRet;
    VOS_INT32              swCnt;
    MED_DEVGAIN_OBJ_STRU  *pstDevGain = (MED_DEVGAIN_OBJ_STRU *)pstInstance;

    /* 判断入参是否合法 */
    uwRet       = MED_PP_CheckPtrValid(MED_DEVGAIN_GetObjInfoPtr, pstInstance);

    /* 入参合法，则尝试对指针所在位置进行配置项读取操作 */
    if (UCOM_RET_SUCC == uwRet)
    {
        /* BUFFER空间不够，则出错中止操作 */
        if (shwParaLen < MED_DEVGAIN_PARAM_LEN)
        {
            uwRet = UCOM_RET_FAIL;
        }
        else
        {
            /* 获取线性增益 */
            CODEC_OpVecSet(pshwParam, shwParaLen, 0);
            for (swCnt = MED_GAIN_MIN; swCnt <= MED_GAIN_MAX; swCnt++)
            {
                if (MED_GAIN_Db2Linear(swCnt) == pstDevGain->shwGain)
                {
                    break;
                }
            }

            pshwParam[0]    = (VOS_INT16)swCnt;                                            /* 施加增益 */
        }
    }

    return uwRet;

}
VOS_UINT32 MED_DEVGAIN_SetPara (
                VOS_VOID               *pstInstance,
                VOS_INT16              *pshwParam,
                VOS_INT16               shwParaLen)
{
    VOS_UINT32             uwRet;
    MED_DEVGAIN_OBJ_STRU  *pstDevGain = (MED_DEVGAIN_OBJ_STRU *)pstInstance;

    /* 判断入参是否合法 */
    uwRet       = MED_PP_CheckPtrValid(MED_DEVGAIN_GetObjInfoPtr, pstInstance);

    /* 入参合法，则尝试对指针所在位置进行配置项设置操作 */
    if (UCOM_RET_SUCC == uwRet)
    {
        /* BUFFER空间不够，则出错中止操作 */
        if (shwParaLen < MED_DEVGAIN_PARAM_LEN)
        {
            uwRet = UCOM_RET_FAIL;
        }
        else
        {
            /* dB增益转线性增益 */
            /* 判断入参是否合法 */
            if ((pshwParam[0] > MED_GAIN_MAX)||(pshwParam[0] < MED_GAIN_MIN))
            {
                return UCOM_RET_ERR_PARA;
            }

            /* 计算并保存线性增益 */
            pstDevGain->shwGain  = MED_GAIN_Db2Linear(pshwParam[0]);

            /* 获取帧长信息 */
            pstDevGain->shwFrmLen= MED_PP_GetFrameLength();

        }
    }

    return uwRet;

}
VOS_UINT32 MED_DEVGAIN_Main(
                VOS_VOID               *pstInstance,
                VOS_INT16              *pshwIn,
                VOS_INT16              *pshwOut)
{
    /* 临时DEVGAIN指针 */
    MED_DEVGAIN_OBJ_STRU *pstDevGain = (MED_DEVGAIN_OBJ_STRU*)pstInstance;

    /* 设备增益运算，即实现Out(i) = Gain * In(i) */
    CODEC_OpVcMultScale(  pshwIn,
                        (VOS_INT32)(pstDevGain->shwFrmLen),
                        pstDevGain->shwGain,
                        MED_PP_SHIFT_BY_14,
                        pshwOut);

    return UCOM_RET_SUCC;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
