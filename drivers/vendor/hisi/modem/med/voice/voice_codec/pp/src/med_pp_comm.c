

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "med_pp_comm.h"
#include "codec_typedefine.h"
#include "CodecInterface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
VOS_INT16                               g_shwMedPpFrameLength = 0;                  /* PP数据帧长度 */
VOS_INT16                               g_shwAudioPpFrameLength = 0;                /* AUDIO PP数据帧长度 */

/* 子带划分表1 包括窄带和宽带 */
const VOS_INT16 g_ashwMedPpBandTable1[40][2] =
{
    {1,     2},
    {3,     4},
    {5,     6},
    {7,     8},
    {9,    11},
    {12,   14},
    {15,   17},
    {18,   20},
    {21,   23},
    {24,   26},
    {27,   29},
    {30,   32},
    {33,   35},
    {36,   38},
    {39,   41},
    {42,   44},
    {45,   49},
    {50,   54},
    {55,   59},
    {60,   64},
    {65,   69},
    {70,   74},
    {75,   79},
    {80,   86},
    {87,   93},
    {94,  101},
    {102, 110},
    {111, 119},
    {120, 128},
    {129, 139},
    {140, 150},
    {151, 161},
    {162, 176},
    {177, 191},
    {192, 210},
    {211, 229},
    {230, 250},
    {251, 273},
    {274, 296},
    {297, 321}
};

/*****************************************************************************
  3 函数实现
*****************************************************************************/


VOS_VOID* MED_PP_Create(
                MED_OBJ_INFO           *pstInfo,
                VOS_VOID               *pvObjArray,
                VOS_INT32               swObjNum,
                VOS_INT32               swObjLen)
{
    VOS_INT32            swCnt;
    VOS_UINT8           *pucObjs        = (VOS_UINT8 *)pvObjArray;
    MED_OBJ_HEADER_STRU *pstCurrTarget  = MED_NULL;
    VOS_VOID            *pvTargetObj    = MED_NULL;

    for (swCnt = 0; swCnt < swObjNum; swCnt++)
    {
        pstCurrTarget = (MED_OBJ_HEADER_STRU*)pucObjs;

        if ( CODEC_OBJ_UNUSED == pstCurrTarget->enIsUsed )
        {
            pstCurrTarget->enIsUsed     = CODEC_OBJ_USED;
            pvTargetObj                 = (VOS_VOID *)pstCurrTarget;

            /* 结构体信息赋值 */
            pstInfo->pvObjArray         = pvObjArray;
            pstInfo->swObjLen           = swObjLen;
            pstInfo->swObjNum           = swObjNum;

            break;
        }
        pucObjs += swObjLen;
    }

    return pvTargetObj;

}
VOS_UINT32 MED_PP_CheckPtrValid(
                MED_OBJ_INFO           *pstInfo,
                VOS_VOID               *pvTargetObj)
{
    /* 获取(*pstInfo)中的信息 */
    VOS_VOID            *pvObjArray     = pstInfo->pvObjArray;
    VOS_INT32            swObjNum       = pstInfo->swObjNum;
    VOS_INT32            swObjLen       = pstInfo->swObjLen;

    /* 其他信息初始化 */
    VOS_INT32            swCnt;
    VOS_UINT8           *pucObjs        = (VOS_UINT8 *)pvObjArray;
    MED_OBJ_HEADER_STRU *pstCurrTarget  = MED_NULL;
    VOS_UINT32           uwRet          = UCOM_RET_FAIL;

    /* 需要释放的指针为空 */
    if (   (MED_NULL == pvTargetObj)
        || (MED_NULL == pvObjArray) )
    {
        return UCOM_RET_NUL_PTR;
    }

    for (swCnt = 0; swCnt < swObjNum; swCnt++)
    {
        pstCurrTarget = (MED_OBJ_HEADER_STRU *)pucObjs;

        if ((MED_OBJ_HEADER_STRU *)pvTargetObj == pstCurrTarget)
        {
            if (   CODEC_OBJ_USED == pstCurrTarget->enIsUsed)
            {
                uwRet = UCOM_RET_SUCC;
            }
            break;
        }
        pucObjs += swObjLen;
    }

    return uwRet;

}
VOS_VOID MED_PP_CalcPsdBin(
                VOS_INT16           *pshwFreqBin,
                VOS_INT16            shwFreqBinLen,
                VOS_INT32           *pswPsdBin)
{
    VOS_INT16      shwCnt;

    /* 计算第一根功率谱和最后一根功率谱 */
    pswPsdBin[0] = CODEC_OpL_mult(pshwFreqBin[0], pshwFreqBin[0]);              /* [int32 Q1]  （功率谱）*/
    pswPsdBin[0] = CODEC_OpL_shl(pswPsdBin[0], MED_PP_SHIFT_BY_5);              /* Q1 -> Q6 */

    pswPsdBin[shwFreqBinLen-1] = CODEC_OpL_mult(pshwFreqBin[1], pshwFreqBin[1]);
    pswPsdBin[shwFreqBinLen-1] = CODEC_OpL_shl(pswPsdBin[shwFreqBinLen-1], MED_PP_SHIFT_BY_5);

    /* 计算其它功率谱 */
    for (shwCnt = 1; shwCnt < (shwFreqBinLen - 1); shwCnt++)
    {
        pswPsdBin[shwCnt] = 0;
        pswPsdBin[shwCnt] = CODEC_OpL_mac(pswPsdBin[shwCnt],
                                        pshwFreqBin[2*shwCnt],
                                        pshwFreqBin[2*shwCnt]);
        pswPsdBin[shwCnt] = CODEC_OpL_mac(pswPsdBin[shwCnt],
                                        pshwFreqBin[(2*shwCnt)+1],
                                        pshwFreqBin[(2*shwCnt)+1]);
        pswPsdBin[shwCnt] = CODEC_OpL_shl(pswPsdBin[shwCnt], MED_PP_SHIFT_BY_5);
    }

}


VOS_VOID MED_PP_CalcBandPsd(
                VOS_INT32           *pswPsdBin,
                VOS_INT16            shwBandLen,
                VOS_INT32           *pswPsdBand)
{
    VOS_INT16   shwCntI;
    VOS_INT16   shwCntJ;

    VOS_INT16   shwChLo;
    VOS_INT16   shwChHi;

    /* 子带功率谱 */
    UCOM_MemSet(pswPsdBand, 0, (VOS_UINT16)shwBandLen * sizeof(VOS_INT32));

    for (shwCntI = 0; shwCntI < shwBandLen; shwCntI++)
    {
        /* 子带范围 */
        shwChLo = g_ashwMedPpBandTable1[shwCntI][0];
        shwChHi = g_ashwMedPpBandTable1[shwCntI][1];

        /* 子带范围内的功率谱求和 */
        for (shwCntJ = (shwChLo - 1); shwCntJ < shwChHi; shwCntJ++)
        {
            pswPsdBand[shwCntI] = CODEC_OpL_add(pswPsdBand[shwCntI],
                                              pswPsdBin[shwCntJ]);
        }

        /* 子带内功率谱取平均 */
        pswPsdBand[shwCntI] /= ((shwChHi - shwChLo) + 1);

        /* 限定最小值 */
        pswPsdBand[shwCntI] = CODEC_OpL_max(pswPsdBand[shwCntI],
                                          MED_PP_MIN_PSD_VALUE);
    }
}
VOS_VOID MED_PP_CalcSmoothPsd(
                VOS_INT32               *pswPsdLast,
                VOS_INT16                shwLen,
                VOS_INT16                shwAlpha,
                VOS_INT32               *pswPsd)
{
    VOS_INT16 shwCnt;
    VOS_INT16 shwSubAlpha;

    for (shwCnt = 0; shwCnt < shwLen; shwCnt++)
    {
        shwSubAlpha    = CODEC_OpSub(CODEC_OP_INT16_MAX, shwAlpha);

        pswPsd[shwCnt] = CODEC_OpL_add(CODEC_OpL_mpy_32_16(pswPsdLast[shwCnt], shwAlpha),
                                     CODEC_OpL_mpy_32_16(pswPsd[shwCnt], shwSubAlpha));  /* [int32 Q8] */

        /* 设置下限，防止除0 */
        pswPsd[shwCnt] = CODEC_OpL_max(pswPsd[shwCnt],
                                     MED_PP_MIN_PSD_VALUE);
    }
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif





