

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include  "med_anr_2mic_interface.h"
#include  "med_pp_comm.h"
#include  "med_fft.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
MED_ANR_2MIC_STRU   g_stMed2MicObj;                                             /* 双麦克降噪模块全局控制实体 */
MED_OBJ_INFO        g_stMed2MicObjInfo;                                         /* 双麦克降噪模块信息 */
VOS_INT16           g_ashwMedAnr2MicTmp1Len640[MED_ANR_2MIC_FFT_LEN_WB];       /* ANR_2MIC全局Buffer */

/* 窄带下2MIC时频变换窗系数 */
VOS_INT16 g_ashwMedAnr2MicWinNb[160] =
    {    2621,   2624,   2633,   2648,   2668,   2694,   2726,   2764,   2808,   2858,
         2913,   2974,   3040,   3113,   3191,   3274,   3364,   3458,   3559,   3665,
         3776,   3892,   4014,   4142,   4274,   4412,   4555,   4703,   4856,   5014,
         5177,   5345,   5517,   5695,   5877,   6063,   6254,   6450,   6650,   6854,
         7062,   7275,   7491,   7712,   7936,   8164,   8396,   8632,   8871,   9113,
         9359,   9608,   9860,  10115,  10373,  10634,  10898,  11164,  11433,  11704,
        11978,  12253,  12531,  12811,  13093,  13377,  13662,  13949,  14237,  14526,
        14817,  15109,  15402,  15696,  15991,  16286,  16582,  16878,  17175,  17472,
        17768,  18065,  18362,  18658,  18954,  19250,  19545,  19839,  20133,  20425,
        20717,  21007,  21296,  21583,  21869,  22154,  22437,  22717,  22996,  23273,
        23548,  23820,  24090,  24358,  24623,  24885,  25145,  25401,  25655,  25906,
        26153,  26397,  26638,  26875,  27108,  27339,  27565,  27787,  28006,  28220,
        28431,  28637,  28839,  29037,  29230,  29419,  29603,  29783,  29958,  30128,
        30293,  30454,  30609,  30760,  30905,  31046,  31181,  31311,  31436,  31555,
        31669,  31777,  31880,  31978,  32070,  32156,  32237,  32312,  32382,  32446,
        32504,  32556,  32603,  32644,  32679,  32708,  32731,  32749,  32760,  32766,
    };

/* 宽带下2MIC时频变换窗系数 */
VOS_INT16 g_ashwMedAnr2MicWinWb[320] =
    {    2621,   2622,   2624,   2628,   2633,   2640,   2648,   2657,   2668,   2680,
         2694,   2709,   2726,   2744,   2764,   2785,   2808,   2831,   2857,   2884,
         2912,   2942,   2973,   3005,   3039,   3074,   3111,   3149,   3189,   3230,
         3272,   3316,   3361,   3408,   3456,   3505,   3556,   3608,   3661,   3716,
         3772,   3830,   3889,   3949,   4010,   4073,   4137,   4203,   4269,   4337,
         4407,   4477,   4549,   4622,   4697,   4772,   4849,   4927,   5007,   5087,
         5169,   5252,   5337,   5422,   5509,   5597,   5686,   5776,   5867,   5959,
         6053,   6148,   6244,   6340,   6439,   6538,   6638,   6739,   6841,   6945,
         7049,   7155,   7261,   7369,   7477,   7586,   7697,   7808,   7921,   8034,
         8148,   8263,   8379,   8496,   8614,   8733,   8853,   8973,   9094,   9216,
         9339,   9463,   9588,   9713,   9839,   9966,  10094,  10222,  10351,  10481,
        10611,  10743,  10874,  11007,  11140,  11274,  11408,  11543,  11679,  11815,
        11952,  12089,  12227,  12365,  12504,  12644,  12783,  12924,  13065,  13206,
        13348,  13490,  13632,  13775,  13918,  14062,  14206,  14350,  14495,  14640,
        14785,  14931,  15077,  15223,  15369,  15516,  15663,  15809,  15957,  16104,
        16251,  16399,  16547,  16695,  16842,  16990,  17139,  17287,  17435,  17583,
        17731,  17879,  18028,  18176,  18324,  18472,  18620,  18768,  18916,  19063,
        19211,  19358,  19505,  19652,  19799,  19946,  20092,  20239,  20384,  20530,
        20676,  20821,  20966,  21110,  21254,  21398,  21542,  21685,  21827,  21970,
        22112,  22253,  22394,  22535,  22675,  22815,  22954,  23092,  23230,  23368,
        23505,  23642,  23777,  23913,  24047,  24182,  24315,  24448,  24580,  24711,
        24842,  24972,  25102,  25231,  25359,  25486,  25612,  25738,  25863,  25987,
        26111,  26233,  26355,  26476,  26596,  26715,  26833,  26951,  27067,  27183,
        27297,  27411,  27524,  27636,  27747,  27857,  27966,  28074,  28181,  28287,
        28392,  28495,  28598,  28700,  28801,  28900,  28999,  29096,  29193,  29288,
        29382,  29475,  29567,  29658,  29747,  29836,  29923,  30009,  30094,  30178,
        30260,  30341,  30421,  30500,  30578,  30654,  30729,  30803,  30875,  30947,
        31017,  31085,  31153,  31219,  31283,  31347,  31409,  31470,  31529,  31588,
        31644,  31700,  31754,  31807,  31858,  31908,  31957,  32004,  32050,  32094,
        32137,  32179,  32219,  32258,  32296,  32332,  32366,  32400,  32431,  32462,
        32491,  32518,  32544,  32569,  32592,  32614,  32634,  32653,  32671,  32687,
        32701,  32714,  32726,  32736,  32745,  32752,  32758,  32762,  32765,  32767,
};
/*****************************************************************************
  3 函数实现
*****************************************************************************/

VOS_VOID* MED_ANR_2MIC_Create( VOS_VOID )
{
    /* 创建双麦克降噪实体*/
    return (MED_PP_Create(
                    MED_ANR_2MIC_GetObjInfoPtr(),
                    MED_ANR_2MIC_GetObjPtr(),
                    MED_ANR_2MIC_MAX_OBJ_NUM,
                    sizeof(MED_ANR_2MIC_STRU)));
}
VOS_UINT32 MED_ANR_2MIC_Destroy( VOS_VOID **ppInstance )
{
    VOS_UINT32              uwRet;
    MED_ANR_2MIC_STRU      *pstObj;
    pstObj               = (MED_ANR_2MIC_STRU *)(*ppInstance);

    /* 判断入参是否合法 */
    uwRet                = MED_PP_CheckPtrValid(MED_ANR_2MIC_GetObjInfoPtr(), pstObj);

    /* 入参合法，则将指针对象释放，并将指针赋空 */
    if (UCOM_RET_SUCC == uwRet)
    {
       UCOM_MemSet(pstObj, 0, sizeof(MED_ANR_2MIC_STRU));
       pstObj->enIsUsed  = CODEC_OBJ_UNUSED;                                      /* 实例化标志置为false */
       *ppInstance       = MED_NULL;
    }

    return uwRet;

}
VOS_UINT32  MED_ANR_2MIC_SetPara(
                VOS_VOID                         *pInstance,
                VOS_INT16                        *pshwPara,
                VOS_INT16                         shwParaLen,
                CODEC_SAMPLE_RATE_MODE_ENUM_INT32   enSampleRate)
{
    VOS_UINT32           uwRet;
    MED_ANR_2MIC_STRU   *pstObj        = (MED_ANR_2MIC_STRU*)pInstance;         /* ANR_2MIC对象指针 */
    MED_2MIC_NV_STRU    *pst2MicNv         = &(pstObj->st2MicNv);               /* 2MIC模块内部的NV项结构体  */
    VOS_INT16            shwFftLen;

    /* 判断入参是否合法 */
    if (MED_PP_CheckPtrValid(MED_ANR_2MIC_GetObjInfoPtr(), pstObj) != UCOM_RET_SUCC)
    {
        return UCOM_RET_FAIL;
    }

    /* 入参合法，则尝试对指针所在位置进行配置项读取操作 */

    /* BUFFER空间不够，或结构体的使能项不正确，则出错中止操作 */
    if (   (shwParaLen  < MED_ANR_2MIC_PARAM_LEN)
        || (pshwPara[0] >= CODEC_SWITCH_BUTT))
    {
        uwRet = UCOM_RET_FAIL;
    }
    else
    {
        /* 初始化缓冲区及内部状态变量 */
        UCOM_MemSet(pstObj, 0, sizeof(MED_ANR_2MIC_STRU));
        pstObj->enIsUsed      = CODEC_OBJ_USED;

        /* 拷贝入参到pInstance->stNv */
        CODEC_OpVecCpy((VOS_INT16 *)pst2MicNv, pshwPara, MED_ANR_2MIC_PARAM_LEN);

        pstObj->enEnable = (VOS_UINT16)pshwPara[0];

        /* 2MIC_Td初始化 */
        pstObj->pstTdProcess = MED_2MIC_GetTdProcessPtr();
        uwRet = MED_ANR_2MIC_TdInit(pst2MicNv, enSampleRate);

        if(CODEC_SAMPLE_RATE_MODE_8000 == enSampleRate)
        {
            pstObj->stComCfg.shwFftLen      = MED_ANR_2MIC_FFT_LEN_NB;
            pstObj->stComCfg.enFftNumIndex  = MED_FFT_NUM_320;
            pstObj->stComCfg.enIfftNumIndex = MED_IFFT_NUM_320;
        }
        else
        {
            pstObj->stComCfg.shwFftLen      = MED_ANR_2MIC_FFT_LEN_WB;
            pstObj->stComCfg.enFftNumIndex  = MED_FFT_NUM_640;
            pstObj->stComCfg.enIfftNumIndex = MED_IFFT_NUM_640;
        }

        /* 2MIC_Fd初始化 */
        pstObj->pstPostFilter = MED_2MIC_GetFdProcessPtr();
        shwFftLen = pstObj->stComCfg.shwFftLen;
        uwRet += MED_ANR_2MIC_FdInit(pst2MicNv, enSampleRate, shwFftLen);
    }


    return uwRet;
}
VOS_UINT32 MED_ANR_2MIC_GetPara(
                VOS_VOID                *pInstance,
                VOS_INT16               *pshwPara,
                VOS_INT16                shwParaLen)
{
    VOS_UINT32                uwRet;
    MED_ANR_2MIC_STRU        *pstObj    = (MED_ANR_2MIC_STRU *)pInstance;
    MED_2MIC_NV_STRU         *pst2MicNv = &(pstObj->st2MicNv);                  /* 2MIC模块内部的NV项结构体 */

    /* 判断入参是否合法 */
    uwRet         = MED_PP_CheckPtrValid(MED_ANR_2MIC_GetObjInfoPtr(), pstObj);

    /* 入参合法，则尝试对指针所在位置进行配置项读取操作 */
    if (UCOM_RET_SUCC == uwRet)
    {
        /* BUFFER空间不够，或结构体的使能项不正确，则出错中止操作 */
        if (   (shwParaLen      < MED_ANR_2MIC_PARAM_LEN)
            || (pst2MicNv->enEnable >= CODEC_SWITCH_BUTT))
        {
            uwRet = UCOM_RET_FAIL;
        }
        else
        {
            /* 拷贝pInstance的参数到pshwPara */
            CODEC_OpVecSet(pshwPara, shwParaLen, 0);
            CODEC_OpVecCpy(pshwPara, (VOS_INT16 *)pst2MicNv, MED_ANR_2MIC_PARAM_LEN);
        }
    }

    return uwRet;
}


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

