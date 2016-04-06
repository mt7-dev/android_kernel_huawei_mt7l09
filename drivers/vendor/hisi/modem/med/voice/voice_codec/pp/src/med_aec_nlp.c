

/*****************************************************************************
   1 头文件包含
*****************************************************************************/
#include "med_aec_nlp.h"
#include "med_pp_main.h"
#include "med_anr_2mic_comm.h"
#include "med_anr_2mic_interface.h"
#include "om.h"

#ifdef  __cplusplus
#if  __cplusplus
extern "C"{
#endif
#endif

/*****************************************************************************
   2 全局变量定义
*****************************************************************************/
/* 窄带下NLP时频变换窗系数 */
VOS_INT16 g_ashwMedAecNlpWinNb[256] =
{
    2    , 3    , 4    , 6    , 8    , 12   , 16   , 21   , 27   , 35   , 43   ,
    53   , 64   , 76   , 90   , 106  , 123  , 143  , 164  , 188  , 214  , 242  ,
    273  , 307  , 343  , 383  , 426  , 473  , 523  , 577  , 635  , 698  , 765  ,
    837  , 914  , 997  , 1086 , 1180 , 1281 , 1389 , 1504 , 1626 , 1756 , 1895 ,
    2043 , 2200 , 2366 , 2543 , 2731 , 2930 , 3142 , 3366 , 3603 , 3854 , 4119 ,
    4400 , 4696 , 5009 , 5339 , 5688 , 6055 , 6441 , 6847 , 7273 , 7721 , 8190 ,
    8680 , 9193 , 9729 , 10287, 10866, 11468, 12091, 12735, 13398, 14079, 14777,
    15490, 16215, 16950, 17692, 18439, 19187, 19933, 20673, 21404, 22123, 22826,
    23510, 24173, 24812, 25424, 26008, 26563, 27088, 27581, 28043, 28475, 28875,
    29246, 29588, 29903, 30191, 30454, 30694, 30911, 31109, 31287, 31448, 31592,
    31722, 31838, 31941, 32034, 32116, 32188, 32252, 32308, 32358, 32400, 32436,
    32467, 32493, 32514, 32531, 32543, 32551, 32555, 32555, 32551, 32543, 32531,
    32514, 32493, 32467, 32436, 32400, 32358, 32308, 32252, 32188, 32116, 32034,
    31941, 31838, 31722, 31592, 31448, 31287, 31109, 30911, 30694, 30454, 30191,
    29903, 29588, 29246, 28875, 28475, 28043, 27581, 27088, 26563, 26008, 25424,
    24812, 24173, 23510, 22826, 22123, 21404, 20673, 19933, 19187, 18439, 17692,
    16950, 16215, 15490, 14777, 14079, 13398, 12735, 12091, 11468, 10866, 10287,
    9729 , 9193 , 8680 , 8190 , 7721 , 7273 , 6847 , 6441 , 6055 , 5688 , 5339 ,
    5009 , 4696 , 4400 , 4119 , 3854 , 3603 , 3366 , 3142 , 2930 , 2731 , 2543 ,
    2366 , 2200 , 2043 , 1895 , 1756 , 1626 , 1504 , 1389 , 1281 , 1180 , 1086 ,
    997  , 914  , 837  , 765  , 698  , 635  , 577  , 523  , 473  , 426  , 383  ,
    343  , 307  , 273  , 242  , 214  , 188  , 164  , 143  , 123  , 106  , 90   ,
    76   , 64   , 53   , 43   , 35   , 27   , 21   , 16   , 12   , 8    , 6    ,
    4    , 3    , 2
};

/* 宽带下NLP时频变换窗系数 */
VOS_INT16 g_ashwMedAecNlpWinWb[512] =
{
    2,2,3,3,4,5,6,7,8,10,12,14,16,18,21,24,27,31,34,38,43,47,52,58,63,69,76,83,90,
    97,105,114,123,132,142,153,163,175,187,200,213,227,241,256,272,288,305,323,342,
    361,381,402,424,447,471,495,521,547,574,603,632,663,695,727,761,797,833,871,910,
    950,992,1035,1080,1126,1174,1223,1274,1327,1381,1437,1495,1555,1617,1680,1746,
    1814,1884,1956,2030,2107,2186,2267,2351,2437,2526,2618,2712,2810,2910,3013,3119,
    3229,3341,3457,3576,3698,3824,3954,4087,4224,4365,4510,4658,4811,4968,5129,5295,
    5465,5640,5819,6002,6191,6384,6583,6786,6994,7208,7426,7650,7879,8114,8354,8599,
    8850,9107,9369,9636,9909,10188,10472,10762,11057,11357,11663,11974,12290,12611,
    12937,13268,13603,13943,14287,14635,14987,15342,15700,16061,16425,16792,17160,
    17530,17901,18273,18645,19018,19390,19761,20131,20500,20866,21230,21592,21949,
    22304,22654,22999,23340,23676,24006,24330,24648,24960,25265,25564,25855,26139,
    26415,26685,26946,27200,27446,27685,27915,28138,28354,28561,28761,28954,29139,
    29318,29489,29653,29810,29961,30105,30243,30374,30500,30620,30735,30844,30948,
    31047,31141,31230,31315,31396,31472,31545,31614,31679,31741,31799,31854,31906,
    31956,32002,32046,32087,32126,32163,32197,32229,32259,32288,32314,32339,32362,
    32383,32403,32422,32439,32454,32469,32482,32493,32504,32514,32522,32529,32535,
    32540,32544,32547,32549,32550,32550,32549,32547,32544,32540,32535,32529,32522,
    32514,32504,32493,32482,32469,32454,32439,32422,32403,32383,32362,32339,32314,
    32288,32259,32229,32197,32163,32126,32087,32046,32002,31956,31906,31854,31799,
    31741,31679,31614,31545,31472,31396,31315,31230,31141,31047,30948,30844,30735,
    30620,30500,30374,30243,30105,29961,29810,29653,29489,29318,29139,28954,28761,
    28561,28354,28138,27915,27685,27446,27200,26946,26685,26415,26139,25855,25564,
    25265,24960,24648,24330,24006,23676,23340,22999,22654,22304,21949,21592,21230,
    20866,20500,20131,19761,19390,19018,18645,18273,17901,17530,17160,16792,16425,
    16061,15700,15342,14987,14635,14287,13943,13603,13268,12937,12611,12290,11974,
    11663,11357,11057,10762,10472,10188,9909,9636,9369,9107,8850,8599,8354,8114,
    7879,7650,7426,7208,6994,6786,6583,6384,6191,6002,5819,5640,5465,5295,5129,4968,
    4811,4658,4510,4365,4224,4087,3954,3824,3698,3576,3457,3341,3229,3119,3013,2910,
    2810,2712,2618,2526,2437,2351,2267,2186,2107,2030,1956,1884,1814,1746,1680,1617,
    1555,1495,1437,1381,1327,1274,1223,1174,1126,1080,1035,992,950,910,871,833,797,
    761,727,695,663,632,603,574,547,521,495,471,447,424,402,381,361,342,323,305,288,
    272,256,241,227,213,200,187,175,163,153,142,132,123,114,105,97,90,83,76,69,63,58,
    52,47,43,38,34,31,27,24,21,18,16,14,12,10,8,7,6,5,4,3,3,2,2
};

/* 窄带下NLP子带抑制各频点增益调制系数 */
VOS_INT16 g_ashwMedAecNlpGainModulTabNB[129] =
{
    32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,
    32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,
    32767,32767,32767,32767,32767,32767,32434,32099,31765,31431,31096,30762,30427,
    30093,29759,29424,29090,28756,28421,28087,27752,27418,27084,26749,26415,26081,
    25746,25412,25078,24743,24409,24074,23740,23406,23071,22737,22403,22068,21734,
    21400,21065,20731,20396,20062,19728,19393,19059,18725,18390,18056,17721,17387,
    17053,16718,16384,16050,15715,15381,15047,14712,14378,14043,13709,13375,13040,
    12706,12372,12037,11703,11368,11034,10700,10365,10031,9697,9362,9028,8694,
    8359,8025,7690,7356,7022,6687,6353,6019,5684,5350,5016,4681,4347,
    4012,3678,3344,3009,2675,2341,2006,1672,1337,1003,669,334
};

/* 窄带下2MIC的NLP子带抑制各频点增益调制系数 */
VOS_INT16 g_ashwMed2MicAecNlpGainModulTabNB[161] =
{
    32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,
    32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,
    32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,
    32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,
    32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,
    32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,
    32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,
    32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,
    32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,
    32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,
    32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,
    32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,
    32767,32767,32767,32767,32767
};

/* 宽带下NLP子带抑制各频点增益调制系数 */
VOS_INT16 g_ashwMedAecNlpGainModulTabWB[257] =
{
    32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,
    32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,
    32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,
    32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,
    32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,
    32767,32767,32767,32767,32767,32767,32767,32767,32767,32599,32430,
    32261,32092,31923,31755,31586,31417,31248,31079,30910,30741,30572,
    30403,30234,30065,29897,29728,29559,29390,29221,29052,28883,28714,
    28545,28376,28208,28039,27870,27701,27532,27363,27194,27025,26856,
    26687,26518,26350,26181,26012,25843,25674,25505,25336,25167,24998,
    24829,24660,24492,24323,24154,23985,23816,23647,23478,23309,23140,
    22971,22802,22634,22465,22296,22127,21958,21789,21620,21451,21282,
    21113,20944,20776,20607,20438,20269,20100,19931,19762,19593,19424,
    19255,19087,18918,18749,18580,18411,18242,18073,17904,17735,17566,
    17397,17229,17060,16891,16722,16553,16384,16215,16046,15877,15708,
    15539,15371,15202,15033,14864,14695,14526,14357,14188,14019,13850,
    13681,13513,13344,13175,13006,12837,12668,12499,12330,12161,11992,
    11824,11655,11486,11317,11148,10979,10810,10641,10472,10303,10134,
    9966,9797,9628,9459,9290,9121,8952,8783,8614,8445,8276,8108,7939,
    7770,7601,7432,7263,7094,6925,6756,6587,6418,6250,6081,5912,5743,
    5574,5405,5236,5067,4898,4729,4560,4392,4223,4054,3885,3716,3547,
    3378,3209,3040,2871,2703,2534,2365,2196,2027,1858,1689,1520,1351,
    1182,1013,845,676,507,338,169
};

/* 宽带下2MIC NLP子带抑制各频点增益调制系数 */
VOS_INT16 g_ashwMed2MicAecNlpGainModulTabWB[321] =
{
    32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,
    32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,
    32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,
    32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,
    32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,
    32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,
    32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,
    32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,
    32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,
    32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,
    32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,
    32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,
    32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,
    32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,
    32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,
    32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,
    32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,
    32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,
    32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,
    32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,
    32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,
    32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,
    32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,
    32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,32767,
    32767,32767,32767,32767,32767,32767,32767,32767,32767
};

/* NLP子带抑制频点映射表 DTD中FFT长度与NLP不同导致 */
VOS_INT16 g_ashwMedAecNlpDtd2NlpBinTransTab[257] =
{
    1,3,4,5,6,8,9,10,11,13,14,15,16,18,19,20,21,23,24,25,26,28,29,30,31,33,
    34,35,36,38,39,40,41,43,44,45,46,48,49,50,51,53,54,55,56,58,59,60,61,63,
    64,65,66,68,69,70,71,73,74,75,76,78,79,80,81,83,84,85,86,88,89,90,91,93,
    94,95,96,98,99,100,101,103,104,105,106,108,109,110,111,113,114,115,116,
    118,119,120,121,123,124,125,126,128,129,130,131,133,134,135,136,138,139,
    140,141,143,144,145,146,148,149,150,151,153,154,155,156,158,159,160,161,
    163,164,165,166,168,169,170,171,173,174,175,176,178,179,180,181,183,184,
    185,186,188,189,190,191,193,194,195,196,198,199,200,201,203,204,205,206,
    208,209,210,211,213,214,215,216,218,219,220,221,223,224,225,226,228,229,
    230,231,233,234,235,236,238,239,240,241,243,244,245,246,248,249,250,251,
    253,254,255,256,258,259,260,261,263,264,265,266,268,269,270,271,273,274,
    275,276,278,279,280,281,283,284,285,286,288,289,290,291,293,294,295,296,
    298,299,300,301,303,304,305,306,308,309,310,311,313,314,315,316,318,319,
    320,321
};

/*****************************************************************************
   3 函数实现
*****************************************************************************/

VOS_VOID MED_AEC_NlpInit(
                MED_AEC_NV_STRU                *pstAecParam,
                CODEC_SAMPLE_RATE_MODE_ENUM_INT32 enSampleRate,
                MED_AEC_NLP_STRU               *pstNlp,
                MED_PP_MIC_NUM_ENUM_UINT16      enPpMicNum)
{
    VOS_INT32                           swCntI;
    VOS_INT32                           swTmp;

    UCOM_MemSet((VOS_INT16*)pstNlp, 0, sizeof(MED_AEC_NLP_STRU));

    /* 公共配置 */
    pstNlp->stConfig.enNlpEnable                = pstAecParam->enAecEnable;
    pstNlp->stConfig.shwDt2StLvl                = pstAecParam->shwNlpDt2StLvl;
    pstNlp->stConfig.shwSt2DtLvl                = pstAecParam->shwNlpSt2DtLvl;
    pstNlp->stConfig.shwCngInsertLvl            = pstAecParam->shwNlpCngInsertLvl;
    pstNlp->stConfig.shwNoiseFloorValue         = pstAecParam->shwNlpNoiseFloorThd;
    pstNlp->stConfig.shwMaxSuppress             = pstAecParam->shwNlpMaxSuppressLvl;
    pstNlp->stConfig.shwNonlinearThd            = pstAecParam->shwNlpNonlinearThd;
    pstNlp->stConfig.shwNlpBandProbSupFastAlpha = pstAecParam->shwNlpBandProbSupFastAlpha;
    pstNlp->stConfig.shwNlpBandProbSupSlowAlpha = pstAecParam->shwNlpBandProbSupSlowAlpha;

    /* 单双麦克NLP模块分别采用不同长度的Fft变换 */
    if (MED_PP_MIC_NUM_1 == enPpMicNum)
    {
        /* 窄带FFT变换长度 */
        if (CODEC_SAMPLE_RATE_MODE_8000 == enSampleRate)
        {
            pstNlp->shwFrameLen = MED_AEC_1MIC_NLP_NB_FRM_LEN;
            pstNlp->shwFftLen   = MED_1MIC_AEC_NLP_FFT_LEN_NB;
        }
        else
        {
            pstNlp->shwFrameLen = MED_AEC_1MIC_NLP_WB_FRM_LEN;
            pstNlp->shwFftLen   = MED_1MIC_AEC_NLP_FFT_LEN_WB;
        }
    }
    else
    {
        if (CODEC_SAMPLE_RATE_MODE_8000 == enSampleRate)
        {
            pstNlp->shwFrameLen = MED_AEC_2MIC_NLP_NB_FRM_LEN;
            pstNlp->shwFftLen   = MED_2MIC_AEC_NLP_FFT_LEN_NB;
        }
        else
        {
            pstNlp->shwFrameLen = MED_AEC_2MIC_NLP_WB_FRM_LEN;
            pstNlp->shwFftLen   = MED_2MIC_AEC_NLP_FFT_LEN_WB;
        }
    }

    /* 切换控制 */

    /* 前一帧 Gamma */
    pstNlp->stSwCtrl.shwGammaPre   = MED_AEC_NLP_GAMMA_FIRST;

    /* DT -> ST handover */
    CODEC_OpVecSet(pstNlp->stSwCtrl.ashwDt2StGradsTab,
                 MED_AEC_NLP_GRAD_NUM,
                 CODEC_OP_INT16_MAX);
    pstNlp->stSwCtrl.shwDt2StGradsLen      = pstNlp->stConfig.shwDt2StLvl;      /* 实际使用的handover曲线表 最大级别20 */

    /* 根据抽样索引表对于原始handover曲线进行抽样 */
    for (swCntI = 0; swCntI < pstNlp->stSwCtrl.shwDt2StGradsLen; swCntI++)
    {
        pstNlp->stSwCtrl.ashwDt2StGradsTab[swCntI]
            = CODEC_OpSub(MED_AEC_NLP_GAMMA_MAX,
                        CODEC_OpSaturate(CODEC_OpL_mult0((VOS_INT16)swCntI + 1,
                                                     (VOS_INT16)(MED_AEC_NLP_GAMMA_MAX - MED_AEC_NLP_GAMMA_MIN)) /
                                       CODEC_OpL_deposit_l(pstNlp->stConfig.shwDt2StLvl)));
    }

    /* ST -> DT handover */
    CODEC_OpVecSet(pstNlp->stSwCtrl.ashwSt2DtGradsTab,
                 MED_AEC_NLP_GRAD_NUM,
                 CODEC_OP_INT16_MAX);                                             /* 实际使用的handover曲线表 最大级别20 */
    pstNlp->stSwCtrl.shwSt2DtGradsLen    = pstNlp->stConfig.shwSt2DtLvl;

    /* 实际使用的曲线表长度 */
    /* 根据抽样索引表对于原始handover曲线进行抽样 */
    for (swCntI = 0; swCntI < pstNlp->stSwCtrl.shwSt2DtGradsLen; swCntI++)
    {
        pstNlp->stSwCtrl.ashwSt2DtGradsTab[swCntI]
            = CODEC_OpAdd(MED_AEC_NLP_GAMMA_MIN,
                        CODEC_OpSaturate(CODEC_OpL_mult0((VOS_INT16)swCntI + 1,
                                                     (VOS_INT16)(MED_AEC_NLP_GAMMA_MAX - MED_AEC_NLP_GAMMA_MIN)) /
                                       CODEC_OpL_deposit_l(pstNlp->stConfig.shwSt2DtLvl)));
    }

    /* 切换计数器, 初始化为处于DT状态 */
    pstNlp->stSwCtrl.shwDt2StCnt         = 0;                                   /* DT -> ST 计数 */
    pstNlp->stSwCtrl.shwSt2DtCnt         = pstNlp->stSwCtrl.shwSt2DtGradsLen + 1;/* ST -> DT 计数 */

    /* 随机序列 */
    pstNlp->stRandSn.ashwSeed[0]         = MED_AEC_NLP_SEED1;                   /* 随机序列种子 */
    pstNlp->stRandSn.ashwSeed[1]         = MED_AEC_NLP_SEED2;
    pstNlp->stRandSn.ashwSeed[2]         = MED_AEC_NLP_SEED3;

    /* Res 残留回声抑制 */
    pstNlp->stRes.shwWienerFulEn         = MED_AEC_MAX_WIENER_DEC;              /* 完全使能阶段 */
    pstNlp->stRes.shwWienerPrtEn         = MED_AEC_MAX_WIENER_OFF;              /* 部分使能阶段 */

    swTmp = MED_AEC_NLP_RES_POW_THD / CODEC_OpL_mult0(pstNlp->shwFftLen, pstNlp->shwFftLen);

    /* ST\DT过渡阶段抑制修正阈值。高频能量较少，因此宽带和窄带阈值相同*/
    pstNlp->stRes.shwPowThd               = (VOS_INT16)swTmp;

    /* 高频残留回声抑制 */
    pstNlp->stRes.shwMaxSuppress         = pstNlp->stConfig.shwMaxSuppress;
    pstNlp->stRes.shwNonlinearThd        = pstNlp->stConfig.shwNonlinearThd;

    /* 背景噪声估计 */
    pstNlp->stBgnStru.shwBgnBinLen       = (pstNlp->shwFftLen / 2) + 1;         /* 背景噪声频谱长度 */
    pstNlp->stBgnStru.swBgnValue         = MED_AEC_NLP_BGN_INIT_VALUE;          /* 背景噪声幅度初始值 */
    pstNlp->stBgnStru.shwModifiedALpha   = MED_AEC_NLP_MODIFIED_ALPHA;          /* 背景噪声估计修正系数初始值 */

    swTmp = pstNlp->stConfig.shwNoiseFloorValue;
    pstNlp->stConfig.shwNoiseFloorValue  = CODEC_OpLog2(swTmp);

    /* 子带残差抑制 */
    pstNlp->stBandSupress.shwNlpBandProbSupFastAlpha = pstNlp->stConfig.shwNlpBandProbSupFastAlpha;
    pstNlp->stBandSupress.shwNlpBandProbSupSlowAlpha = pstNlp->stConfig.shwNlpBandProbSupSlowAlpha;


    for(swCntI = 0; swCntI < (1 + (MED_AEC_2MIC_NLP_MAX_FFT_LEN/2)); swCntI++)
    {
        pstNlp->stBandSupress.ashwNlpFreqBinGain[swCntI] = 32767;
    }
}
VOS_VOID MED_AEC_1MIC_NlpMain(
                MED_AEC_NLP_STRU              *pstNlp,
                MED_AEC_DTD_NLINE_STRU        *pstDtdNline,
                VOS_INT16                     *pshwErr,
                MED_AEC_DTD_FLAG_ENUM_INT16    enDtdFlag,
                VOS_INT16                     *pshwMcBin,
                VOS_INT16                     *pshwMcErrBinNorm,
                VOS_INT16                      shwNormShift)
{
    VOS_INT16       shwGamma;
    VOS_INT16       shwTmp1;
    VOS_INT32       swCntJ;
    VOS_INT16       shwErrMean;
    VOS_INT16      *pshwMcErrBin;                                               /* size MED_AEC_NLP_MAX_FFT_LEN */
    VOS_INT16      *pshwMcErrBinTmp;                                            /* size MED_AEC_NLP_MAX_FFT_LEN */
    VOS_INT16      *pshwWhiteNoise;                                             /* size MED_AEC_NLP_MAX_FFT_LEN */
    VOS_INT32       swSubErrBinSum = 0;
    VOS_INT32       swBgnValueTmp;
    VOS_INT16       ashwWienerCoef[(MED_AEC_NLP_MAX_FFT_LEN / 2) + 1] = {0};

    /* 如果PASS或不使能，则AEC不进行处理，直通信号 */
    if (CODEC_SWITCH_OFF == pstNlp->stConfig.enNlpEnable)
    {
        return;
    }

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_AEC_1MIC_NlpMain);

    pshwMcErrBin     = MED_AEC_GetshwVecTmp640Ptr1();
    pshwMcErrBinTmp  = MED_AEC_GetshwVecTmp640Ptr2();
    pshwWhiteNoise   = MED_AEC_GetshwVecTmp640Ptr2();

    /* 非线性抑制系数计算 定点实现 */
    MED_AEC_NlpCtrl(&(pstNlp->stSwCtrl), pshwErr, enDtdFlag, &shwGamma);

   /* 反归一化 */
   CODEC_OpVecShr_r(pshwMcErrBinNorm,
                    pstNlp->shwFftLen,
                    shwNormShift,
                    pshwMcErrBin);

   /* 背景噪声估计 */
   if ( MED_AEC_DTD_FLAG_PASS == enDtdFlag )
   {
        /* 当Mic中检测为非语音时, 进行背景噪声估计 */
        MED_AEC_NlpBgnEst(&(pstNlp->stBgnStru),
                          pshwErr,
                          pshwMcErrBin,
                          pstNlp->shwFrameLen,
                          pstNlp->stConfig.shwNoiseFloorValue);

        pstNlp->stRes.shwDtCnt = 0;
        pstNlp->stBandSupress.swPsdBinLo2Hi = 0;
    }
    else
    {
        /* 对残差中的残留回声进行进一步抑制 */
        MED_AEC_NlpRes(&(pstNlp->stRes),
                       pshwMcBin,
                       pshwMcErrBin,
                       pshwMcErrBinNorm,
                       &ashwWienerCoef[0],
                       pstNlp->shwFftLen,
                       shwGamma);

        /* 对高频非线性回声进一步抑制 */
        shwErrMean = CODEC_OpVecMeanAbs(pshwErr, pstNlp->shwFrameLen);

        if (   (MED_AEC_DTD_FLAG_DT == enDtdFlag)
            && (shwErrMean < pstNlp->stConfig.shwNonlinearThd)
            && (pstNlp->stBgnStru.swBgnValue < MED_AEC_NLP_SUPP_EN_THD))
        {
            MED_AEC_NlpSlopSuppress(pshwMcErrBinNorm,
                                    pstNlp->shwFftLen,
                                    pstNlp->stConfig.shwMaxSuppress,
                                    pshwMcErrBinNorm);
        }

        /* 根据子带单双讲语音存在概率进行非线性回声抑制 */
        MED_AEC_NlpBandProbSuppress(pstDtdNline,
                                    &(pstNlp->stBandSupress),
                                    pshwMcErrBinNorm,
                                    shwNormShift,
                                    pstNlp->shwFftLen);

        /* 舒适噪声处理, 计算剩余能量 */
        CODEC_OpVecShr_r(pshwMcErrBinNorm,
                       pstNlp->shwFftLen,
                       shwNormShift,
                       pshwMcErrBinTmp);
        CODEC_OpVecAbs(pshwMcErrBinTmp,
                     pstNlp->shwFftLen,
                     pshwMcErrBinTmp);

        swSubErrBinSum = 0;
        for (swCntJ = 0; swCntJ < pstNlp->shwFftLen; swCntJ++)
        {
            swSubErrBinSum = CODEC_OpL_add(swSubErrBinSum, (VOS_INT32)pshwMcErrBinTmp[swCntJ]);
        }

        shwTmp1 = CODEC_OpSaturate(swSubErrBinSum);
        if (shwTmp1 < pstNlp->stConfig.shwCngInsertLvl)
        {
            /* 产生白噪声, 舒适噪声不进行归一化 */
            MED_AEC_NlpCngWhiteGen(&pstNlp->stRandSn,
                                   pstNlp->shwFftLen,
                                   pshwWhiteNoise);

            /* 根据输入噪声信号频谱，对白噪声整形 */
            MED_AEC_NlpShapeCng(pshwWhiteNoise,
                                pstNlp->stBgnStru.aswBgnBin,
                                pstNlp->shwFftLen);

            /* 插入舒适噪声 */
            swBgnValueTmp = CODEC_OpL_shl(pstNlp->stBgnStru.swBgnValue,
                                        CODEC_OpShl(shwNormShift, 1));

            MED_AEC_NlpInsertCn(pshwMcErrBinNorm,
                                shwGamma,
                                pshwWhiteNoise,
                                swBgnValueTmp,
                                pstNlp->shwFftLen);

        }
    }

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_AEC_1MIC_NlpMain);

}
VOS_VOID  MED_AEC_2MIC_NlpMain(
                MED_2MIC_DATAFLOW_MC_STRU        *pstDataFlowMc,
                MED_2MIC_DATAFLOW_RC_STRU        *pstDataFlowRc,
                MED_AEC_NLP_STRU                 *pstNlp,
                MED_AEC_DTD_NLINE_STRU           *pstDtdNline,
                VOS_INT16                        *pshwMcErr,
                MED_AEC_DTD_FLAG_ENUM_INT16       enDtdFlag)
{
    VOS_INT16                           shwGamma;
    VOS_INT16                           shwTmp1;
    VOS_INT32                           swCntJ;
    VOS_INT16                           shwMcErrMean;
    VOS_INT16                          *pshwMcErrBin;                           /* size MED_AEC_NLP_MAX_FFT_LEN */
    VOS_INT16                          *pshwMcErrBinTmp;                        /* size MED_AEC_NLP_MAX_FFT_LEN */
    VOS_INT16                          *pshwWhiteNoise;                         /* size MED_AEC_NLP_MAX_FFT_LEN */
    VOS_INT32                           swMcErrBinSum = 0;
    VOS_INT32                           swBgnValueTmpMc;
    VOS_INT32                           swBgnValueTmpRc;
    VOS_INT32                           swBgnValueTmpRcs;
    VOS_INT16                          *pshwMcBin;
    VOS_INT16                          *pshwMcErrBinNorm;
    VOS_INT16                           shwMcNormShift;
    VOS_INT16                          *pshwRcErrBinNorm;
    VOS_INT16                           shwRcNormShift;
    VOS_INT16                          *pshwRcsBinNorm;
    VOS_INT16                           shwRcsNormShift;
    VOS_INT16                           shwFftLen;
    VOS_INT16                          *pshwWienerCoef;

    /* FFT变换长度 */
    shwFftLen      = pstNlp->shwFftLen;

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_AEC_2MIC_NlpMain);

    /* 申请内存 */
    pshwWienerCoef = UCOM_MemAlloc((VOS_UINT16)((shwFftLen/2) + 1) * sizeof(VOS_INT16));

    /* 主麦克频谱、残差频谱、归一化系数 */
    pshwMcBin           = pstDataFlowMc->stOriMc.ashwFreq;
    pshwMcErrBinNorm    = pstDataFlowMc->stMc.ashwFreq;
    shwMcNormShift      = pstDataFlowMc->stMc.shwNormShift;

    /* 辅麦克残差频谱、校准频谱、归一化系数*/
    pshwRcErrBinNorm    = pstDataFlowRc->stRc.ashwFreq;
    shwRcNormShift      = pstDataFlowRc->stRc.shwNormShift;
    pshwRcsBinNorm      = pstDataFlowRc->stOriRc.ashwFreq;
    shwRcsNormShift     = pstDataFlowRc->stOriRc.shwNormShift;

    /* 如果PASS或不使能，则AEC不进行处理，直通信号 */
    if (CODEC_SWITCH_OFF == pstNlp->stConfig.enNlpEnable)
    {
        UCOM_MemFree(pshwWienerCoef);
        return;
    }

    /* 存储中间的临时结果 */
    pshwMcErrBin       = MED_AEC_GetshwVecTmp640Ptr1();
    pshwMcErrBinTmp    = MED_AEC_GetshwVecTmp640Ptr2();
    pshwWhiteNoise     = MED_AEC_GetshwVecTmp640Ptr2();

    /* 非线性抑制系数计算 定点实现 */
    MED_AEC_NlpCtrl(&(pstNlp->stSwCtrl), pshwMcErr, enDtdFlag, &shwGamma);

    /* 反归一化 */
    CODEC_OpVecShr_r(pshwMcErrBinNorm,
                   shwFftLen,
                   shwMcNormShift,
                   pshwMcErrBin);

    /* 背景噪声估计 */
    if (MED_AEC_DTD_FLAG_PASS == enDtdFlag)
    {
        /* 当Mic中检测为非语音时, 进行背景噪声估计 */
        MED_AEC_NlpBgnEst(&(pstNlp->stBgnStru),
                          pshwMcErr,
                          pshwMcErrBin,
                          pstNlp->shwFrameLen,
                          pstNlp->stConfig.shwNoiseFloorValue);
    }

    if (MED_AEC_DTD_FLAG_PASS == enDtdFlag)
    {
        pstNlp->stRes.shwDtCnt = 0;
    }
    else
    {
        /* 对残差中的残留回声进行进一步抑制 */
        MED_AEC_NlpRes(&(pstNlp->stRes),
                       pshwMcBin,
                       pshwMcErrBin,
                       pshwMcErrBinNorm,
                       pshwWienerCoef,
                       shwFftLen,
                       shwGamma);
        MED_AEC_2MIC_NlpRc(pshwRcErrBinNorm,
                           pshwRcsBinNorm,
                           pshwWienerCoef,
                           shwFftLen);

        /* 对高频非线性回声进一步抑制 */
        shwMcErrMean =  CODEC_OpVecMeanAbs(pshwMcErr, pstNlp->shwFrameLen);

        if (   (MED_AEC_DTD_FLAG_DT == enDtdFlag)
                && (shwMcErrMean < pstNlp->stConfig.shwNonlinearThd)
                && (pstNlp->stBgnStru.swBgnValue < MED_AEC_NLP_SUPP_EN_THD))
        {
            MED_AEC_NlpSlopSuppress( pshwMcErrBinNorm,
                                     shwFftLen,
                                     pstNlp->stConfig.shwMaxSuppress,
                                     pshwMcErrBinNorm);
            MED_AEC_NlpSlopSuppress( pshwRcErrBinNorm,
                                     shwFftLen,
                                     pstNlp->stConfig.shwMaxSuppress,
                                     pshwRcErrBinNorm);
            MED_AEC_NlpSlopSuppress( pshwRcsBinNorm,
                                     shwFftLen,
                                     pstNlp->stConfig.shwMaxSuppress,
                                     pshwRcsBinNorm);
        }

        /* 根据子带单双讲语音存在概率进行非线性回声抑制 */
        MED_AEC_NlpBandProbSuppress(pstDtdNline,
                                    &(pstNlp->stBandSupress),
                                    pshwMcErrBinNorm,
                                    shwMcNormShift,
                                    shwFftLen);

        /* 舒适噪声处理, 计算剩余能量 */
        CODEC_OpVecShr_r(pshwMcErrBinNorm,
                         shwFftLen,
                         shwMcNormShift,
                         pshwMcErrBinTmp);
        CODEC_OpVecAbs(pshwMcErrBinTmp,
                       shwFftLen,
                       pshwMcErrBinTmp);

        swMcErrBinSum = 0;
        for (swCntJ = 0; swCntJ < shwFftLen; swCntJ++)
        {
            swMcErrBinSum = CODEC_OpL_add(swMcErrBinSum, (VOS_INT32)pshwMcErrBinTmp[swCntJ]);
        }

        shwTmp1 = CODEC_OpSaturate(swMcErrBinSum);
        if (shwTmp1 < pstNlp->stConfig.shwCngInsertLvl)
        {
            /* 产生白噪声, 舒适噪声不进行归一化 */
            MED_AEC_NlpCngWhiteGen(&(pstNlp->stRandSn),
                                    shwFftLen,
                                    pshwWhiteNoise);

            /* 根据输入噪声信号频谱，对白噪声整形 */
            MED_AEC_NlpShapeCng(pshwWhiteNoise,
                                pstNlp->stBgnStru.aswBgnBin,
                                shwFftLen);

            /* 主辅麦克插入同样的舒适噪声 */
            swBgnValueTmpMc  = CODEC_OpL_shl(pstNlp->stBgnStru.swBgnValue,
                                           CODEC_OpShl(shwMcNormShift, 1));

            swBgnValueTmpRc  = CODEC_OpL_shl(pstNlp->stBgnStru.swBgnValue,
                                           CODEC_OpShl(shwRcNormShift, 1));
            swBgnValueTmpRcs = CODEC_OpL_shl(pstNlp->stBgnStru.swBgnValue,
                                           CODEC_OpShl(shwRcsNormShift, 1));
            MED_AEC_NlpInsertCn(pshwMcErrBinNorm,
                                shwGamma,
                                pshwWhiteNoise,
                                swBgnValueTmpMc >> MED_NLP_NLP_CNG_SHIFT,
                                shwFftLen);
            MED_AEC_NlpInsertCn(pshwRcErrBinNorm,
                                shwGamma,
                                pshwWhiteNoise,
                                swBgnValueTmpRc >> MED_NLP_NLP_CNG_SHIFT,
                                shwFftLen);
            MED_AEC_NlpInsertCn(pshwRcsBinNorm,
                                shwGamma,
                                pshwWhiteNoise,
                                swBgnValueTmpRcs >> MED_NLP_NLP_CNG_SHIFT,
                                shwFftLen);

        }
    }

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_AEC_2MIC_NlpMain);

    /* 释放内存 */
    UCOM_MemFree(pshwWienerCoef);

}
VOS_VOID MED_AEC_2MIC_NlpRc(
                VOS_INT16              *pshwRcErrBinNorm,
                VOS_INT16              *pshwRcsBinNorm,
                VOS_INT16              *pshwWienerCoef,
                VOS_INT16               shwFftLen )
{
    VOS_INT32               swCntI;

    /* 对辅麦克残差信号施加增益 */
    for (swCntI = 1; swCntI < (shwFftLen >> 1); swCntI++)
    {

        pshwRcErrBinNorm[2 * swCntI]       = CODEC_OpMult_r(pshwRcErrBinNorm[2 * swCntI],
                                                     pshwWienerCoef[swCntI + 1]);
        pshwRcErrBinNorm[(2 * swCntI) + 1] = CODEC_OpMult_r(pshwRcErrBinNorm[(2 * swCntI) + 1],
                                                     pshwWienerCoef[swCntI + 1]);
    }
    pshwRcErrBinNorm[1] = CODEC_OpMult_r(pshwRcErrBinNorm[1], pshwWienerCoef[1]);
    pshwRcErrBinNorm[0] = CODEC_OpMult_r(pshwRcErrBinNorm[0], pshwWienerCoef[0]);

    /* 对辅麦克校准信号施加增益 */
    for (swCntI = 1; swCntI < (shwFftLen >> 1); swCntI++)
    {

        pshwRcsBinNorm[2 * swCntI]       = CODEC_OpMult_r(pshwRcsBinNorm[2 * swCntI],
                                                     pshwWienerCoef[swCntI + 1]);
        pshwRcsBinNorm[(2 * swCntI) + 1] = CODEC_OpMult_r(pshwRcsBinNorm[(2 * swCntI) + 1],
                                                     pshwWienerCoef[swCntI + 1]);
    }
    pshwRcsBinNorm[1] = CODEC_OpMult_r(pshwRcsBinNorm[1], pshwWienerCoef[1]);
    pshwRcsBinNorm[0] = CODEC_OpMult_r(pshwRcsBinNorm[0], pshwWienerCoef[0]);

}


VOS_VOID MED_AEC_NlpCtrl(
                MED_AEC_NLP_SWITCH_STRU     *pstSwCtrl,
                VOS_INT16                   *pshwdErr,
                MED_AEC_DTD_FLAG_ENUM_INT16  enDtd,
                VOS_INT16                   *pshwGamma)
{
    VOS_INT16                   shwGamma;

    /* DT -> ST 过渡 */
    if (MED_AEC_DTD_FLAG_ST == enDtd)
    {
        /* DT -> ST 阶段计数器增 */
        pstSwCtrl->shwDt2StCnt = CODEC_OpAdd(pstSwCtrl->shwDt2StCnt, 1);

        /* 处于过渡期间 */
        if (pstSwCtrl->shwDt2StCnt <= pstSwCtrl->shwDt2StGradsLen)
        {
            /* 计算 Gamma */
            shwGamma = pstSwCtrl->ashwDt2StGradsTab[pstSwCtrl->shwDt2StCnt - 1];

            /* 通过 Gamma 同步 shwSt2DtCnt */
            pstSwCtrl->shwSt2DtCnt = MED_AEC_NlpSyncCnt(shwGamma,
                                                        pstSwCtrl->ashwSt2DtGradsTab,
                                                        pstSwCtrl->shwSt2DtGradsLen);

        }
        else /* 已完成过渡 */
        {
            /* 准备 ST -> DT 过渡 */
            pstSwCtrl->shwSt2DtCnt = 0;

            /* DT -> ST 已完成过渡 */
            pstSwCtrl->shwDt2StCnt = CODEC_OpAdd(pstSwCtrl->shwDt2StGradsLen, 1);

            /* ST 状态 */
            shwGamma = 1;
        }
    }
    else /* ST -> DT(PASS) 过渡 */
    {
        /* ST -> DT 阶段计数器增 1 */
        pstSwCtrl->shwSt2DtCnt = CODEC_OpAdd(pstSwCtrl->shwSt2DtCnt, 1);

        /* 处于过渡期间 */
        if (pstSwCtrl->shwSt2DtCnt <= pstSwCtrl->shwSt2DtGradsLen)
        {
            /* 计算 Gamma */
            shwGamma = pstSwCtrl->ashwSt2DtGradsTab[pstSwCtrl->shwSt2DtCnt - 1];
            /* 通过 Gamma 同步 shwDt2StCnt */
            pstSwCtrl->shwDt2StCnt = MED_AEC_NlpSyncCnt(shwGamma,
                                                        pstSwCtrl->ashwDt2StGradsTab,
                                                        pstSwCtrl->shwDt2StGradsLen);

        }
        else /* 已完成过渡 */
        {
            /* 准备 DT -> ST 过渡 */
            pstSwCtrl->shwDt2StCnt = 0;
            /* ST -> DT 已完成过渡 */
            pstSwCtrl->shwSt2DtCnt = CODEC_OpAdd(pstSwCtrl->shwSt2DtGradsLen, 1);

            /* DT 状态 */
            shwGamma = MED_AEC_NLP_GAMMA_MAX;
        }
    }

    *pshwGamma = shwGamma;
}



VOS_VOID MED_AEC_NlpBgnEst (
                MED_AEC_NLP_BGN_STRU *pstBgnStru,
                VOS_INT16            *pshwFrm,
                VOS_INT16            *pshwBin,
                VOS_INT16             shwFrmLen,
                VOS_INT16             shwNoiseFloorValue)
{
    VOS_INT16                  *pshwNormFrm;                                    /* size MED_AEC_NLP_WB_FRM_LEN */
    VOS_INT32                   swNormFrmPow;
    VOS_INT32                   swCntI;
    VOS_INT32                   swNormFrmSum;
    VOS_INT32                   swMeanPower;
    VOS_INT32                   swTmp1;
    VOS_INT32                   swTmp2;
    VOS_INT32                   swTmp3;
    VOS_INT16                   shwTmp1;
    VOS_INT16                   shwTmp2;
    VOS_INT16                   shwOptAlpha;
    VOS_INT16                   shwTmpAlpha;
    VOS_INT16                   shwLastSmoothAlpha;
    VOS_INT32                   swEstedNoise;

    pshwNormFrm = (VOS_INT16 *)UCOM_MemAlloc((VOS_UINT16)shwFrmLen * sizeof(VOS_INT16));

    /* 输入信号平均值 */
    swNormFrmSum             = 0;
    for (swCntI = 0; swCntI < shwFrmLen; swCntI++)
    {
        pshwNormFrm[swCntI] = CODEC_OpShr(CODEC_OpAbs_s(pshwFrm[swCntI]), shwNoiseFloorValue);
        swNormFrmPow         = CODEC_OpL_mult0(pshwNormFrm[swCntI], pshwNormFrm[swCntI]);
        swNormFrmSum         = CODEC_OpL_add(swNormFrmSum, swNormFrmPow);
    }

    swMeanPower = CODEC_OpNormDiv_32(swNormFrmSum, shwFrmLen, MED_PP_SHIFT_BY_4);

    /* 计算最优平滑系数 */
    swTmp1     = CODEC_OpNormDiv_32(pstBgnStru->swSmoothedPower,                  /* Q3 */
                                  CODEC_OpL_add(pstBgnStru->swMinNoisePower,
                                  CODEC_OpShl(1, MED_PP_SHIFT_BY_4)), MED_PP_SHIFT_BY_3);
    shwTmp2    = CODEC_OpSaturate(CODEC_OpL_sub(swTmp1, CODEC_OpShl(1, MED_PP_SHIFT_BY_3)));

    shwOptAlpha= CODEC_OpSaturate(CODEC_OpNormDiv_32(1, CODEC_OpL_add(
                 CODEC_OpShl(1, MED_PP_SHIFT_BY_6),
                 CODEC_OpL_mult0(shwTmp2, shwTmp2)), MED_PP_SHIFT_BY_21));       /* Q15 */

    /* 计算修正平滑系数 */
    swTmp1     = CODEC_OpNormDiv_32(pstBgnStru->swSmoothedPower,                  /* Q3 */
                                  CODEC_OpL_add(swMeanPower,
                                  CODEC_OpShl(1, MED_PP_SHIFT_BY_4)), MED_PP_SHIFT_BY_3);
    shwTmp2    = CODEC_OpSaturate(CODEC_OpL_sub(swTmp1, CODEC_OpShl(1, MED_PP_SHIFT_BY_3)));

    shwTmpAlpha= CODEC_OpSaturate(CODEC_OpNormDiv_32(1, CODEC_OpL_add(
                 CODEC_OpShl(1, MED_PP_SHIFT_BY_6),
                 CODEC_OpL_mult0(shwTmp2, shwTmp2)), MED_PP_SHIFT_BY_21));       /* Q15 */

    shwTmp1    = CODEC_OpMult_r(MED_AEC_NLP_BGN_ONE_SUB_SMOOTH_COFE1,
                              CODEC_OpMax(shwTmpAlpha, MED_AEC_NLP_BGN_SMOOTH_COFE1));
    shwTmp2    = CODEC_OpMult_r(MED_AEC_NLP_BGN_SMOOTH_COFE1, pstBgnStru->shwModifiedALpha);
    pstBgnStru->shwModifiedALpha = CODEC_OpAdd(shwTmp2, shwTmp1);

    /* 计算最终平滑系数 */
    shwLastSmoothAlpha = CODEC_OpMult_r(shwOptAlpha, pstBgnStru->shwModifiedALpha);
    shwLastSmoothAlpha = CODEC_OpMax(MED_AEC_NLP_BGN_MIN_SMOOTH_ALPHA, shwLastSmoothAlpha);

    pstBgnStru->swSmoothedPower                                                 /* Q4 */
               = CODEC_OpL_add(CODEC_OpL_mpy_32_16_r(pstBgnStru->swSmoothedPower, shwLastSmoothAlpha),
                             CODEC_OpL_mpy_32_16_r(swMeanPower,
                             CODEC_OpSub(CODEC_OP_INT16_MAX,shwLastSmoothAlpha)));

    /* 搜索最小值 */
    for (swCntI = 0; swCntI < (MED_AEC_NLP_CNG_EST_LEN - 1); swCntI++)
    {
        pstBgnStru->swPowerBuf[swCntI] = pstBgnStru->swPowerBuf[swCntI + 1];
    }
    pstBgnStru->swPowerBuf[MED_AEC_NLP_CNG_EST_LEN - 1]
                 = pstBgnStru->swSmoothedPower;
    swEstedNoise = CODEC_OpVecMin32(pstBgnStru->swPowerBuf,
                                MED_AEC_NLP_CNG_EST_LEN);
    pstBgnStru->swMinNoisePower = swEstedNoise;

    /* 得到背景噪声能量估计 */
    swTmp3 = swEstedNoise;
    swTmp1 = CODEC_OpL_shl(1, shwNoiseFloorValue);
    swTmp3 = CODEC_OpL_mult0(CODEC_OpSaturate(swTmp3),
             (VOS_INT16)CODEC_OpL_mult0((VOS_INT16)swTmp1, (VOS_INT16)swTmp1));
    swTmp3 = CODEC_OpL_shr_r(swTmp3,MED_PP_SHIFT_BY_4);
    swTmp3 = CODEC_OpL_min(swTmp3, MED_AEC_NLP_BGN_MEAN_POW_MAX);

    swTmp1 = CODEC_OpL_mpy_32_16_r(pstBgnStru->swBgnValue,
                                 MED_AEC_NLP_BGN_ONE_SUB_AMP_ALPHA);
    swTmp2 = CODEC_OpL_mpy_32_16_r(swTmp3, MED_AEC_NLP_BGN_AMP_ALPHA);
    pstBgnStru->swBgnValue = CODEC_OpL_add(swTmp1, swTmp2);

    if (swMeanPower <= CODEC_OpL_shl(pstBgnStru->swMinNoisePower, MED_PP_SHIFT_BY_2))
    {
        /* 计算背景噪声频谱 */
        for (swCntI = 1; swCntI < (pstBgnStru->shwBgnBinLen - 1); swCntI++)
        {
             swTmp1 = CODEC_OpL_mpy_32_16(pstBgnStru->aswBgnBin[swCntI],
                                        MED_AEC_NLP_BGN_ONE_SUB_FREQ_ALPHA);
             swTmp2 = CODEC_OpL_add(
                          CODEC_OpL_mult0(pshwBin[2 * swCntI], pshwBin[2 * swCntI]),
                          CODEC_OpL_mult0(pshwBin[(2 * swCntI) + 1], pshwBin[(2 * swCntI) + 1]));
             swTmp2 = CODEC_OpL_shl(swTmp2, MED_NLP_NLP_CNG_BIN_SHIFT);
             swTmp2 = CODEC_OpL_mpy_32_16(swTmp2, MED_AEC_NLP_BGN_FREQ_ALPHA);
             pstBgnStru->aswBgnBin[swCntI] =  CODEC_OpL_add(swTmp1, swTmp2);
         }
    }

    UCOM_MemFree(pshwNormFrm);
}
VOS_INT16 MED_AEC_NlpSyncCnt(
                VOS_INT16               shwGamma,
                VOS_INT16              *pashwGrads,
                VOS_INT16               shwGradsLen)
{
    VOS_INT32           swCntI;

    for (swCntI = 0; swCntI < (shwGradsLen - 1); swCntI++)
    {
        /* 下降曲线 */
        if (   (shwGamma <= pashwGrads[swCntI])
            && (shwGamma >= pashwGrads[swCntI + 1]))
        {
            return (VOS_INT16)(swCntI + 1);
        }

        /* 上升曲线 */
        if (   (shwGamma >= pashwGrads[swCntI])
            && (shwGamma <= pashwGrads[swCntI + 1]))
        {
            return (VOS_INT16)(swCntI + 1);
        }
    }
    return (VOS_INT16)shwGradsLen;
}


VOS_VOID MED_AEC_NlpShapeCng (
                VOS_INT16       *pshwWhiteNoise,
                VOS_INT32       *pswNoiseBin,
                VOS_INT16        shwFftLen)
{
    VOS_INT32        swCntI;
    VOS_INT32        swMaxValue;
    VOS_INT16       *pshwTmpBuf;                                                /* size MED_AEC_NLP_MAX_FFT_LEN */
    VOS_INT32       *pswNoise;
    VOS_UINT32       uswMemSize;

    /* 动态内存申请 */
    uswMemSize  = (VOS_UINT16)(1 + (VOS_UINT16)(shwFftLen/2)) * (VOS_UINT16)sizeof(VOS_INT32);
    pswNoise   = (VOS_INT32 *)UCOM_MemAlloc(uswMemSize);

    CODEC_OpVecCpy((VOS_INT16 *)pswNoise, (VOS_INT16 *)pswNoiseBin, (2 + shwFftLen));

    pshwTmpBuf = MED_AEC_GetshwVecTmp640Ptr1();

    /* 计算背景噪声最大频谱增益 */
    swMaxValue = pswNoise[0];
    for (swCntI = 1; swCntI <= (shwFftLen >> 1); swCntI++)
    {
        if (swMaxValue <= pswNoise[swCntI])
        {
            swMaxValue   = pswNoise[swCntI];
        }
    }

    swMaxValue  = CODEC_OpL_max(swMaxValue,1);

    /* 归一化频谱增益 */
    for (swCntI = 1; swCntI < (shwFftLen >> 1); swCntI++)
    {
        pswNoise[swCntI] = CODEC_OpL_max(pswNoise[swCntI], 1);
        pshwTmpBuf[swCntI]   = CODEC_OpSqrt(CODEC_OpNormDiv_32(pswNoise[swCntI],
                                                           swMaxValue,
                                                           MED_PP_SHIFT_BY_30));/* Q15 */
    }

    /* 根据频带，在白噪声上施加频谱增益 */
    for (swCntI = 1; swCntI < (shwFftLen >> 1); swCntI++)
    {
        pshwWhiteNoise[2 * swCntI]     = CODEC_OpMult_r(pshwWhiteNoise[2 * swCntI],
                                                       pshwTmpBuf[swCntI]);
        pshwWhiteNoise[(2 * swCntI) + 1] = pshwWhiteNoise[2 * swCntI];
    }

    pshwWhiteNoise[0] = 0;                                                      /* 将DC置最小值 */
    pshwWhiteNoise[1] = 0;

    /* 释放内存 */
    UCOM_MemFree(pswNoise);

}


VOS_VOID MED_AEC_NlpInsertCn (
                VOS_INT16               *pshwErrBin,
                VOS_INT16                shwGamma,
                VOS_INT16               *pshwWhiteNoise,
                VOS_INT32                swBgnValue,
                VOS_INT16                shwLen)
{
    VOS_INT32               swSum;
    VOS_INT32               swNWPow;
    VOS_INT16               shwNW;
    VOS_INT16               shwTmp1;
    VOS_INT16               shwTmp2;
    VOS_INT32               swCntI;

    swSum       = 1;

    /* 计算总的能量 */
    for (swCntI = 0; swCntI < shwLen; swCntI++)
    {
        shwTmp1 = CODEC_OpShr_r(pshwWhiteNoise[swCntI], 1);                      /* 防swSum饱和 */
        swSum   = CODEC_OpL_add(CODEC_OpL_mult0 (shwTmp1, shwTmp1), swSum);
    }
    swNWPow     = CODEC_OpNormDiv_32(swBgnValue, swSum, MED_PP_SHIFT_BY_28);      /* Q30  前面防swSum饱和移了2位 */
    shwNW       = CODEC_OpSqrt(swNWPow);                                          /* Q15 */

    shwTmp2     = CODEC_OpSub(CODEC_OP_INT16_MAX, shwGamma);
    shwNW       = CODEC_OpMult_r(shwNW, shwTmp2);
    shwNW       = CODEC_OpMult_r(shwNW, MED_NLP_NLP_CNG_INSERT_COFE);             /* 乘以0.6 */

    /* 根据学习的频谱产生残差信号 */
    for (swCntI = 0; swCntI < (shwLen >> 1); swCntI++)
    {
        shwTmp1 = CODEC_OpMult_r(pshwWhiteNoise[2 * swCntI], shwNW);
        shwTmp2 = CODEC_OpMult_r(pshwWhiteNoise[(2 * swCntI) + 1], shwNW);
        pshwErrBin[2 * swCntI]
                = CODEC_OpAdd(CODEC_OpMult_r(shwGamma, pshwErrBin[2 * swCntI]), shwTmp1);
        pshwErrBin[(2 * swCntI) + 1]
                = CODEC_OpAdd(CODEC_OpMult_r(shwGamma, pshwErrBin[(2 * swCntI) + 1]), shwTmp2);
    }
}
#ifndef _MED_C89_

VOS_VOID MED_AEC_NlpCngWhiteGen(
                MED_AEC_NLP_RANDOM_STRU     *pstRandomState,
                VOS_INT16                    shwGenLen,
                VOS_INT16                   *pshwWhiteSn)
{
    VOS_INT32       swCnt               = 0;                                  /* 临时变量 */

    /* 使用产生的信号作为白噪声频域信号 */
    pshwWhiteSn[0] = 0;
    pshwWhiteSn[1] = 0;

    /* 虚部与实部相同，可以降低方差 */
    for (swCnt = 1; swCnt < (shwGenLen >> 1); swCnt++)
    {
        pshwWhiteSn[2 * swCnt]       = (VOS_INT16) (rand() & 0xffff);;
        pshwWhiteSn[(2 * swCnt) + 1] = pshwWhiteSn[2 * swCnt];
    }

}
VOS_VOID MED_AEC_NlpRes(
                MED_AEC_NLP_RES_STRU *pstRes,
                VOS_INT16            *pshwMicBin,
                VOS_INT16            *pshwErrBin,
                VOS_INT16            *pshwErrBinNorm,
                VOS_INT16            *pshwWienerCoef,
                VOS_INT16             shwFftLen,
                VOS_INT16             shwGamma)
{

    VOS_INT32             swCntI;
    VOS_INT16            *pshwMicBinAbs  = MED_NULL;
    VOS_INT16            *pshwErrBinAbs  = MED_NULL;
    VOS_INT16            *pshwEstEchoAbs = MED_NULL;
    VOS_UINT32            uwMemSize;
    VOS_INT16             shwBinLen;
    VOS_INT16             ashwMaxH;
    VOS_INT16             ashwMinH;
    VOS_INT32             swSeeSum;
    VOS_INT16             shwTmp;
    VOS_INT32             swTmp1;
    VOS_INT16             shwWienerInc;
    ae_p24x2s aepX1, aepX2, aepY1, aepY2;
    ae_q56s aeqS1, aeqS2;

    /* 申请内存 */
    uwMemSize = (VOS_UINT16)((shwFftLen / 2) + 1) * sizeof(VOS_INT16);
    pshwMicBinAbs  = (VOS_INT16 *)UCOM_MemAlloc(uwMemSize);
    pshwErrBinAbs  = (VOS_INT16 *)UCOM_MemAlloc(uwMemSize);
    pshwEstEchoAbs = (VOS_INT16 *)UCOM_MemAlloc(uwMemSize);

    pshwMicBinAbs[0] = pshwMicBin[0];
    pshwMicBinAbs[1] = pshwMicBin[1];
    pshwErrBinAbs[0] = pshwErrBin[0];
    pshwErrBinAbs[1] = pshwErrBin[1];

    for (swCntI = 1; swCntI < (shwFftLen >> 1); swCntI++)
    {
        aepX1 = *((ae_p16x2s *)&pshwMicBin[2 * swCntI ]);
        aepX2 = *((ae_p16x2s *)&pshwMicBin[2 * swCntI ]);

        aepY1 = *((ae_p16x2s *)&pshwErrBin[2 * swCntI]);
        aepY2 = *((ae_p16x2s *)&pshwErrBin[2 * swCntI]);

        aeqS1 = AE_MULZAAP24S_HH_LL(aepX1, aepX2);
        aeqS2 = AE_MULZAAP24S_HH_LL(aepY1, aepY2);

        /* 饱和到48bit */
        aeqS1 = AE_SATQ48S(aeqS1);
        aeqS2 = AE_SATQ48S(aeqS2);

        /* aeqS1[bit9 - bit47] -> pswPsdBin */
        pshwMicBinAbs[swCntI + 1] = CODEC_OpSqrt(AE_TRUNCA32Q48(aeqS1));
        pshwErrBinAbs[swCntI + 1] = CODEC_OpSqrt(AE_TRUNCA32Q48(aeqS2));

    }

    shwBinLen           = CODEC_OpAdd(CODEC_OpShr(shwFftLen, 1), 1);                /* 使用的频带数 */

    /* 回声估计信号频谱 */
    CODEC_OpVvSub(pshwMicBinAbs, pshwErrBinAbs, shwBinLen, pshwEstEchoAbs);

    ashwMaxH              = CODEC_OP_INT16_MAX;                                   /* 增益上限 */
    ashwMinH              = MED_AEC_NLP_WIENER_MIN_COEF;                        /* 增益下限 */

    /* 更新功率谱 */
    swSeeSum  = 0;
    shwTmp    = (CODEC_OP_INT16_MAX - MED_AEC_NLP_PSD_SMOOTH) + 1;
    aepX1 = AE_CVTP24A16(MED_AEC_NLP_PSD_SMOOTH);
    aepX2 = AE_CVTP24A16(shwTmp);

    for (swCntI = 0; swCntI < shwBinLen; swCntI++)
    {
        /*      pstRes->aswSme[swCntI]
                    = CODEC_OpL_add(CODEC_OpL_mpy_32_16(pstRes->aswSme[swCntI], MED_AEC_NLP_PSD_SMOOTH),
                      CODEC_OpL_mpy_32_16(CODEC_OpL_mult0(pshwMicBinAbs[swCntI], pshwErrBinAbs[swCntI]),
                                        shwTmp));*/
        aeqS1 = *((ae_q32s *)&pstRes->aswSme[swCntI]);
        aeqS1 = AE_MULFQ32SP16S_H(aeqS1, aepX1);

        swTmp1 = (VOS_INT32)pshwMicBinAbs[swCntI] * (VOS_INT32)pshwErrBinAbs[swCntI];
        aeqS2 = *((ae_q32s *)&swTmp1);
        aeqS2 = AE_MULFQ32SP16S_H(aeqS2, aepX2);

        aeqS1 = AE_ADDSQ56S(aeqS1, aeqS2);
        aeqS1 = AE_SATQ48S(aeqS1);
        pstRes->aswSme[swCntI] = AE_TRUNCA32Q48(aeqS1);

        /*  pstRes->aswSmm[swCntI]
                    = CODEC_OpL_add(CODEC_OpL_mpy_32_16(pstRes->aswSmm[swCntI], MED_AEC_NLP_PSD_SMOOTH),
                      CODEC_OpL_mpy_32_16(CODEC_OpL_mult0(pshwMicBinAbs[swCntI], pshwMicBinAbs[swCntI]),
                                        shwTmp));*/
        aeqS1 = *((ae_q32s *)&pstRes->aswSmm[swCntI]);
        aeqS1 = AE_MULFQ32SP16S_H(aeqS1, aepX1);

        swTmp1 = (VOS_INT32)pshwMicBinAbs[swCntI] * (VOS_INT32)pshwMicBinAbs[swCntI];
        aeqS2 = *((ae_q32s *)&swTmp1);
        aeqS2 = AE_MULFQ32SP16S_H(aeqS2, aepX2);

        aeqS1 = AE_ADDSQ56S(aeqS1, aeqS2);
        aeqS1 = AE_SATQ48S(aeqS1);
        pstRes->aswSmm[swCntI] = AE_TRUNCA32Q48(aeqS1);

        /*  pstRes->aswSyy[swCntI]
                    = CODEC_OpL_add(CODEC_OpL_mpy_32_16(pstRes->aswSyy[swCntI], MED_AEC_NLP_PSD_SMOOTH),
                      CODEC_OpL_mpy_32_16(CODEC_OpL_mult0(pshwEstEchoAbs[swCntI], pshwEstEchoAbs[swCntI]),
                                        shwTmp)); */
        aeqS1 = *((ae_q32s *)&pstRes->aswSyy[swCntI]);
        aeqS1 = AE_MULFQ32SP16S_H(aeqS1, aepX1);

        swTmp1 = (VOS_INT32)pshwEstEchoAbs[swCntI] * (VOS_INT32)pshwEstEchoAbs[swCntI];
        aeqS2 = *((ae_q32s *)&swTmp1);
        aeqS2 = AE_MULFQ32SP16S_H(aeqS2, aepX2);

        aeqS1 = AE_ADDSQ56S(aeqS1, aeqS2);
        aeqS1 = AE_SATQ48S(aeqS1);
        pstRes->aswSyy[swCntI] = AE_TRUNCA32Q48(aeqS1);

        /*  pstRes->aswSee[swCntI]
                    = CODEC_OpL_add(CODEC_OpL_mpy_32_16(pstRes->aswSee[swCntI], MED_AEC_NLP_PSD_SMOOTH),
                      CODEC_OpL_mpy_32_16(CODEC_OpL_mult0(pshwErrBinAbs[swCntI], pshwErrBinAbs[swCntI]),
                                        shwTmp)); */
        aeqS1 = *((ae_q32s *)&pstRes->aswSee[swCntI]);
        aeqS1 = AE_MULFQ32SP16S_H(aeqS1, aepX1);

        swTmp1 = (VOS_INT32)pshwErrBinAbs[swCntI] * (VOS_INT32)pshwErrBinAbs[swCntI];
        aeqS2 = *((ae_q32s *)&swTmp1);
        aeqS2 = AE_MULFQ32SP16S_H(aeqS2, aepX2);

        aeqS1 = AE_ADDSQ56S(aeqS1, aeqS2);
        aeqS1 = AE_SATQ48S(aeqS1);
        pstRes->aswSee[swCntI] = AE_TRUNCA32Q48(aeqS1);
     /*   pstRes->aswSee[swCntI]
            = CODEC_OpL_add(CODEC_OpL_mpy_32_16(pstRes->aswSee[swCntI], MED_AEC_NLP_PSD_SMOOTH),
              CODEC_OpL_mpy_32_16(CODEC_OpL_mult0(pshwErrBinAbs[swCntI], pshwErrBinAbs[swCntI]),
                                shwTmp)); */
        swSeeSum = CODEC_OpL_add(swSeeSum, pstRes->aswSee[swCntI]);
    }

    /* 更新滤波器系数 */

    /* 处于ST阶段 */
    if (shwGamma <= MED_AEC_NLP_GAMMA_MIN)
    {
        /* 采用 Gamma 计算增益 H */
        CODEC_OpVecSet(pshwWienerCoef, shwBinLen, shwGamma);

        pstRes->shwDtCnt = 0;
    }
    /* 处于ST\DT过渡阶段 */
    else if (shwGamma < MED_AEC_NLP_GAMMA_MAX)
    {
        /* AF 残留能量太高，按照双讲处理，避免误切近端语音 */
        if (swSeeSum > pstRes->shwPowThd)
        {
            /* 增益H等于1 */
            CODEC_OpVecSet(pshwWienerCoef, shwBinLen, MED_AEC_NLP_GAMMA_MAX);
        }
        else
        {
            /* 采用 wiener 滤波计算增益 H */
            MED_AecNlpWienerCofe(pstRes->aswSme,
                                 pstRes->aswSmm,
                                 pstRes->aswSyy,
                                 MED_AEC_NLP_WIENER_LVL,
                                 ashwMaxH,
                                 ashwMinH,
                                 shwFftLen,
                                 pshwWienerCoef);

            /* 和Gamma比较，采用抑制效果最强的 */
            for (swCntI = 0; swCntI < shwBinLen; swCntI++)
            {
                pshwWienerCoef[swCntI] = CODEC_OpMin(pshwWienerCoef[swCntI],
                                                    shwGamma);
            }

        }
    }
    /* DT 阶段 */
    else
    {
#ifndef _MED_MAX_CYCLE_
        pstRes->shwDtCnt = CODEC_OpAdd(pstRes->shwDtCnt, 1);

        if (pstRes->shwDtCnt < pstRes->shwWienerFulEn)
        {
            /* 采用 wiener 滤波计算增益 H 完全使能 */
             MED_AecNlpWienerCofe(pstRes->aswSme,
                                  pstRes->aswSmm,
                                  pstRes->aswSyy,
                                  MED_AEC_NLP_WIENER_LVL,
                                  ashwMaxH,
                                  ashwMinH,
                                  shwFftLen,
                                  pshwWienerCoef);
        }
        else if (pstRes->shwDtCnt < pstRes->shwWienerPrtEn)
        {
            /* 采用 wiener 滤波计算增益 H 部分使能 */
            MED_AecNlpWienerCofe(pstRes->aswSme,
                                 pstRes->aswSmm,
                                 pstRes->aswSyy,
                                 MED_AEC_NLP_WIENER_LVL,
                                 ashwMaxH,
                                 ashwMinH,
                                 shwFftLen,
                                 pshwWienerCoef);
            shwWienerInc   = CODEC_OpSaturate(CODEC_OpNormDiv_32(
                             CODEC_OpSub(pstRes->shwWienerPrtEn, pstRes->shwDtCnt),
                             CODEC_OpSub(pstRes->shwWienerPrtEn, pstRes->shwWienerFulEn),
                             MED_PP_SHIFT_BY_15));

            /* 逐步降低wiener 滤波作用 */
            for (swCntI = 0; swCntI < shwBinLen; swCntI++)
            {
                pshwWienerCoef[swCntI] = CODEC_OpAdd(pshwWienerCoef[swCntI], shwWienerInc);
            }
        }
        else
        {
            /* 增益H等于1 */
            CODEC_OpVecSet(pshwWienerCoef, shwBinLen, MED_AEC_NLP_GAMMA_MAX);
        }
#else
            /* 采用 wiener 滤波计算增益 H 部分使能 */
            MED_AecNlpWienerCofe(pstRes->aswSme,
                                 pstRes->aswSmm,
                                 pstRes->aswSyy,
                                 MED_AEC_NLP_WIENER_LVL,
                                 ashwMaxH,
                                 ashwMinH,
                                 shwFftLen,
                                 pshwWienerCoef);
            shwWienerInc   = CODEC_OpSaturate(CODEC_OpNormDiv_32(
                             CODEC_OpSub(pstRes->shwWienerPrtEn, pstRes->shwDtCnt),
                             CODEC_OpSub(pstRes->shwWienerPrtEn, pstRes->shwWienerFulEn),
                             MED_PP_SHIFT_BY_15));

            /* 逐步降低wiener 滤波作用 */
            for (swCntI = 0; swCntI < shwBinLen; swCntI++)
            {
                pshwWienerCoef[swCntI] = CODEC_OpAdd(pshwWienerCoef[swCntI], shwWienerInc);
            }
#endif

    }


    /* 对残差信号施加增益 */

    for (swCntI = 1; swCntI < (shwFftLen >> 1); swCntI++)
    {

        pshwErrBinNorm[2 * swCntI]       = CODEC_OpMult_r(pshwErrBinNorm[2 * swCntI],
                                                     pshwWienerCoef[swCntI + 1]);
        pshwErrBinNorm[(2 * swCntI) + 1] = CODEC_OpMult_r(pshwErrBinNorm[(2 * swCntI) + 1],
                                                     pshwWienerCoef[swCntI + 1]);
    }
    pshwErrBinNorm[1] = CODEC_OpMult_r(pshwErrBinNorm[1], pshwWienerCoef[1]);
    pshwErrBinNorm[0] = CODEC_OpMult_r(pshwErrBinNorm[0], pshwWienerCoef[0]);

    /* 释放内存 */
    UCOM_MemFree(pshwMicBinAbs);
    UCOM_MemFree(pshwErrBinAbs);
    UCOM_MemFree(pshwEstEchoAbs);

}
#else
VOS_VOID MED_AEC_NlpCngWhiteGen(
                MED_AEC_NLP_RANDOM_STRU     *pstRandomState,
                VOS_INT16                    shwGenLen,
                VOS_INT16                   *pshwWhiteSn)
{
    VOS_INT32       swCnt               = 0;                                    /* 循环变量,Q(32,0) */
    VOS_INT32       swTmp               = 0;
    VOS_INT32       swWhiteTmp;                                                 /* 临时变量 */
    VOS_INT16      *pshwTmpBuf;                                                 /* size MED_AEC_NLP_MAX_FFT_LEN */

    pshwTmpBuf = MED_AEC_GetshwVecTmp640Ptr1();

    /* 产生长度为uhwGenLen的白噪声序列*/
    for (swCnt = 0; swCnt < shwGenLen; swCnt++)
    {
        /* 更新种子 */
        pstRandomState->ashwSeed[0]
            = (VOS_INT16)CODEC_OpMod32(CODEC_OpL_mult0(pstRandomState->ashwSeed[0],
                                     MED_AEC_NLP_RAND_A1), MED_AEC_NLP_RAND_M1);
        pstRandomState->ashwSeed[1]
            = (VOS_INT16)CODEC_OpMod32(CODEC_OpL_mult0(pstRandomState->ashwSeed[1],
                                     MED_AEC_NLP_RAND_A2), MED_AEC_NLP_RAND_M2);
        pstRandomState->ashwSeed[2]
            = (VOS_INT16)CODEC_OpMod32(CODEC_OpL_mult0(pstRandomState->ashwSeed[2],
                                     MED_AEC_NLP_RAND_A3), MED_AEC_NLP_RAND_M3);

        swTmp
            = CODEC_OpL_add(CODEC_OpL_add(CODEC_OpL_mult0(pstRandomState->ashwSeed[0], MED_AEC_NLP_RAND_ALPHA),
              CODEC_OpL_mult0(pstRandomState->ashwSeed[1], MED_AEC_NLP_RAND_BETA)),
              CODEC_OpL_mult0(pstRandomState->ashwSeed[2], MED_AEC_NLP_RAND_GAMMA));

        swWhiteTmp        = CODEC_OpL_shr_r(swTmp, MED_PP_SHIFT_BY_15);
        pshwTmpBuf[swCnt] = CODEC_OpSub((VOS_INT16)swWhiteTmp, MED_AEC_NLP_15200_Q0);

    }

    /* 使用产生的信号作为白噪声频域信号 */
    pshwWhiteSn[0] = pshwTmpBuf[0];
    pshwWhiteSn[1] = pshwTmpBuf[1];

    /* 虚部与实部相同，可以降低方差 */
    for (swCnt = 1; swCnt < (shwGenLen >> 1); swCnt++)
    {
        pshwWhiteSn[2 * swCnt]       = pshwTmpBuf[swCnt];
        pshwWhiteSn[(2 * swCnt) + 1] = pshwTmpBuf[swCnt];
    }

}
VOS_VOID MED_AEC_NlpRes(
                MED_AEC_NLP_RES_STRU *pstRes,
                VOS_INT16            *pshwMicBin,
                VOS_INT16            *pshwErrBin,
                VOS_INT16            *pshwErrBinNorm,
                VOS_INT16            *pshwWienerCoef,
                VOS_INT16             shwFftLen,
                VOS_INT16             shwGamma)
{

    VOS_INT32             swCntI;
    VOS_INT16            *pshwMicBinAbs  = MED_NULL;
    VOS_INT16            *pshwErrBinAbs  = MED_NULL;
    VOS_INT16            *pshwEstEchoAbs = MED_NULL;
    VOS_UINT32            uwMemSize;
    VOS_INT16             shwBinLen;
    VOS_INT16             ashwMaxH;
    VOS_INT16             ashwMinH;
    VOS_INT32             swSeeSum;
    VOS_INT16             shwTmp;
    VOS_INT16             shwWienerInc;

    /* 申请内存 */
    uwMemSize = (VOS_UINT16)((shwFftLen / 2) + 1) * sizeof(VOS_INT16);
    pshwMicBinAbs  = (VOS_INT16 *)UCOM_MemAlloc(uwMemSize);
    pshwErrBinAbs  = (VOS_INT16 *)UCOM_MemAlloc(uwMemSize);
    pshwEstEchoAbs = (VOS_INT16 *)UCOM_MemAlloc(uwMemSize);

    pshwMicBinAbs[0] = pshwMicBin[0];
    pshwMicBinAbs[1] = pshwMicBin[1];
    pshwErrBinAbs[0] = pshwErrBin[0];
    pshwErrBinAbs[1] = pshwErrBin[1];

    for (swCntI = 1; swCntI < (shwFftLen >> 1); swCntI++)
    {

        pshwMicBinAbs[swCntI + 1]
            = CODEC_OpSqrt(CODEC_OpL_add(CODEC_OpL_mult0(pshwMicBin[2 * swCntI],
                                                   pshwMicBin[2 * swCntI]),
                                     CODEC_OpL_mult0(pshwMicBin[(2 * swCntI) + 1],
                                                   pshwMicBin[(2 * swCntI)+1])));
        pshwErrBinAbs[swCntI + 1]
            = CODEC_OpSqrt(CODEC_OpL_add(CODEC_OpL_mult0(pshwErrBin[2 * swCntI],
                                                   pshwErrBin[2 * swCntI]),
                                     CODEC_OpL_mult0(pshwErrBin[(2 * swCntI) + 1],
                                                   pshwErrBin[(2 * swCntI) + 1])));

    }

    shwBinLen           = CODEC_OpAdd(CODEC_OpShr(shwFftLen, 1), 1);                /* 使用的频带数 */

    /* 回声估计信号频谱 */
    CODEC_OpVvSub(pshwMicBinAbs, pshwErrBinAbs, shwBinLen, pshwEstEchoAbs);

    ashwMaxH              = CODEC_OP_INT16_MAX;                                   /* 增益上限 */
    ashwMinH              = MED_AEC_NLP_WIENER_MIN_COEF;                        /* 增益下限 */

    /* 更新功率谱 */
    swSeeSum  = 0;
    shwTmp    = (CODEC_OP_INT16_MAX - MED_AEC_NLP_PSD_SMOOTH) + 1;
    for (swCntI = 0; swCntI < shwBinLen; swCntI++)
    {
        /* 计算本帧 Sme */
        pstRes->aswSme[swCntI]
            = CODEC_OpL_add(CODEC_OpL_mpy_32_16(pstRes->aswSme[swCntI], MED_AEC_NLP_PSD_SMOOTH),
              CODEC_OpL_mpy_32_16(CODEC_OpL_mult0(pshwMicBinAbs[swCntI], pshwErrBinAbs[swCntI]),
                                shwTmp));
        /* 计算本帧 Smm */
        pstRes->aswSmm[swCntI]
            = CODEC_OpL_add(CODEC_OpL_mpy_32_16(pstRes->aswSmm[swCntI], MED_AEC_NLP_PSD_SMOOTH),
              CODEC_OpL_mpy_32_16(CODEC_OpL_mult0(pshwMicBinAbs[swCntI], pshwMicBinAbs[swCntI]),
                                shwTmp));
        /* 计算本帧 Syy */
        pstRes->aswSyy[swCntI]
            = CODEC_OpL_add(CODEC_OpL_mpy_32_16(pstRes->aswSyy[swCntI], MED_AEC_NLP_PSD_SMOOTH),
              CODEC_OpL_mpy_32_16(CODEC_OpL_mult0(pshwEstEchoAbs[swCntI], pshwEstEchoAbs[swCntI]),
                                shwTmp));
        /* 计算本帧 See */
        pstRes->aswSee[swCntI]
            = CODEC_OpL_add(CODEC_OpL_mpy_32_16(pstRes->aswSee[swCntI], MED_AEC_NLP_PSD_SMOOTH),
              CODEC_OpL_mpy_32_16(CODEC_OpL_mult0(pshwErrBinAbs[swCntI], pshwErrBinAbs[swCntI]),
                                shwTmp));
        swSeeSum = CODEC_OpL_add(swSeeSum, pstRes->aswSee[swCntI]);
    }

    /* 更新滤波器系数 */

    /* 处于ST阶段 */
    if (shwGamma <= MED_AEC_NLP_GAMMA_MIN)
    {
        /* 采用 Gamma 计算增益 H */
        CODEC_OpVecSet(pshwWienerCoef, shwBinLen, shwGamma);

        pstRes->shwDtCnt = 0;
    }
    /* 处于ST\DT过渡阶段 */
    else if (shwGamma < MED_AEC_NLP_GAMMA_MAX)
    {
        /* AF 残留能量太高，按照双讲处理，避免误切近端语音 */
        if (swSeeSum > pstRes->shwPowThd)
        {
            /* 增益H等于1 */
            CODEC_OpVecSet(pshwWienerCoef, shwBinLen, MED_AEC_NLP_GAMMA_MAX);
        }
        else
        {
            /* 采用 wiener 滤波计算增益 H */
            MED_AecNlpWienerCofe(pstRes->aswSme,
                                 pstRes->aswSmm,
                                 pstRes->aswSyy,
                                 MED_AEC_NLP_WIENER_LVL,
                                 ashwMaxH,
                                 ashwMinH,
                                 shwFftLen,
                                 pshwWienerCoef);

            /* 和Gamma比较，采用抑制效果最强的 */
            for (swCntI = 0; swCntI < shwBinLen; swCntI++)
            {
                pshwWienerCoef[swCntI] = CODEC_OpMin(pshwWienerCoef[swCntI],
                                                    shwGamma);
            }

        }
    }
    /* DT 阶段 */
    else
    {
        pstRes->shwDtCnt = CODEC_OpAdd(pstRes->shwDtCnt, 1);

        if (pstRes->shwDtCnt < pstRes->shwWienerFulEn)
        {
            /* 采用 wiener 滤波计算增益 H 完全使能 */
             MED_AecNlpWienerCofe(pstRes->aswSme,
                                  pstRes->aswSmm,
                                  pstRes->aswSyy,
                                  MED_AEC_NLP_WIENER_LVL,
                                  ashwMaxH,
                                  ashwMinH,
                                  shwFftLen,
                                  pshwWienerCoef);
        }
        else if (pstRes->shwDtCnt < pstRes->shwWienerPrtEn)
        {
            /* 采用 wiener 滤波计算增益 H 部分使能 */
            MED_AecNlpWienerCofe(pstRes->aswSme,
                                 pstRes->aswSmm,
                                 pstRes->aswSyy,
                                 MED_AEC_NLP_WIENER_LVL,
                                 ashwMaxH,
                                 ashwMinH,
                                 shwFftLen,
                                 pshwWienerCoef);
            shwWienerInc   = CODEC_OpSaturate(CODEC_OpNormDiv_32(
                             CODEC_OpSub(pstRes->shwWienerPrtEn, pstRes->shwDtCnt),
                             CODEC_OpSub(pstRes->shwWienerPrtEn, pstRes->shwWienerFulEn),
                             MED_PP_SHIFT_BY_15));

            /* 逐步降低wiener 滤波作用 */
            for (swCntI = 0; swCntI < shwBinLen; swCntI++)
            {
                pshwWienerCoef[swCntI] = CODEC_OpAdd(pshwWienerCoef[swCntI], shwWienerInc);
            }
        }
        else
        {
            /* 增益H等于1 */
            CODEC_OpVecSet(pshwWienerCoef, shwBinLen, MED_AEC_NLP_GAMMA_MAX);
        }

    }


    /* 对残差信号施加增益 */

    for (swCntI = 1; swCntI < (shwFftLen >> 1); swCntI++)
    {

        pshwErrBinNorm[2 * swCntI]       = CODEC_OpMult_r(pshwErrBinNorm[2 * swCntI],
                                                     pshwWienerCoef[swCntI + 1]);
        pshwErrBinNorm[(2 * swCntI) + 1] = CODEC_OpMult_r(pshwErrBinNorm[(2 * swCntI) + 1],
                                                     pshwWienerCoef[swCntI + 1]);
    }
    pshwErrBinNorm[1] = CODEC_OpMult_r(pshwErrBinNorm[1], pshwWienerCoef[1]);
    pshwErrBinNorm[0] = CODEC_OpMult_r(pshwErrBinNorm[0], pshwWienerCoef[0]);

    /* 释放内存 */
    UCOM_MemFree(pshwMicBinAbs);
    UCOM_MemFree(pshwErrBinAbs);
    UCOM_MemFree(pshwEstEchoAbs);

}
#endif
VOS_VOID MED_AecNlpWienerCofe(
                VOS_INT32          *paswSme,
                VOS_INT32          *paswSmm,
                VOS_INT32          *paswSyy,
                VOS_INT16           shwPow,
                VOS_INT16           ashwMaxH,
                VOS_INT16           ashwMinH,
                VOS_INT16           shwFftLen,
                VOS_INT16          *pshwWienerCoef)
{
    VOS_INT32          swCntI;
    VOS_INT32          swTmp;

    /* 采用wiener滤波计算增益H */
    for (swCntI = 0; swCntI < ((shwFftLen >> 1) + 1); swCntI++)
    {
        swTmp = CODEC_OpL_add(paswSmm[swCntI], CODEC_OpL_shl(paswSyy[swCntI], shwPow));
        swTmp = CODEC_OpL_max(swTmp, 1);
        pshwWienerCoef[swCntI]
              = CODEC_OpSaturate(CODEC_OpNormDiv_32(paswSme[swCntI],
                                                swTmp,
                                                MED_PP_SHIFT_BY_15));

        /* 对增益进行限幅 */
        pshwWienerCoef[swCntI] = CODEC_OpMin(pshwWienerCoef[swCntI], ashwMaxH);
        pshwWienerCoef[swCntI] = CODEC_OpMax(pshwWienerCoef[swCntI], ashwMinH);
    }
}

VOS_VOID MED_AEC_NlpSlopSuppress(
                VOS_INT16       *pshwInput,
                VOS_INT16        shwFftLen,
                VOS_INT16        shwMaxSuppress,
                VOS_INT16       *pshwOutput)
{
    VOS_INT16           shwBeginBin;
    VOS_INT16           shwEndBin;
    VOS_INT16           shwK;
    VOS_INT32           swCntI;
    VOS_INT16           shwGain;

    /* 抑制频带 */
    shwEndBin   = CODEC_OpShr(shwFftLen, 1);
    shwBeginBin = 1;

    pshwOutput[0] = pshwInput[0];
    pshwOutput[1] = pshwInput[1];

    /* 计算增益斜率 */
    shwK = (VOS_INT16)(CODEC_OpSub(CODEC_OP_INT16_MAX,shwMaxSuppress) / CODEC_OpSub(shwEndBin, shwBeginBin));

    /* 计算增益斜率 */
    for (swCntI = shwBeginBin; swCntI < shwEndBin; swCntI++)
    {
        shwGain = CODEC_OpSub(CODEC_OP_INT16_MAX,
                            (VOS_INT16)CODEC_OpL_mult0(shwK, (VOS_INT16)swCntI));
        pshwOutput[swCntI << 1]
                = CODEC_OpMult(pshwInput[swCntI << 1], shwGain);
        pshwOutput[(swCntI << 1) + 1]
                = CODEC_OpMult(pshwInput[(swCntI << 1) + 1], shwGain);
    }
}
VOS_VOID MED_AEC_NlpBandProbSuppress(
                MED_AEC_DTD_NLINE_STRU         *pstDtdNline,
                MED_AEC_NLP_BAND_SUPRESS_STRU  *pstBandSupress,
                VOS_INT16                      *pshwErrBinNorm,
                VOS_INT16                       shwNormShift,
                VOS_INT16                       shwFftLen)
{
    VOS_INT16                 shwCnt;
    VOS_INT16                 shwIndex;
    VOS_INT16                 shwDtdFreqBinLen;
    VOS_INT16                 shwNlpFreqBinLen;

    VOS_INT16                 shwFastAlpha;
    VOS_INT16                 shwFastSubAlpha;
    VOS_INT16                 shwSlowAlpha;
    VOS_INT16                 shwSlowSubAlpha;

    VOS_INT16                *pshwDtdSpBandGain;
    VOS_INT16                *pshwDtdFreqBinGain;
    VOS_INT16                *pshwNlpFreqBinGain;
    VOS_INT16                *pshwNlpFreqBinGainLast;

    VOS_INT16                *pshwBinTransTab;
    VOS_INT16                *pshwGainModulTab;
    VOS_INT16                 shwNlpProbBandLen;


    CODEC_SAMPLE_RATE_MODE_ENUM_INT32 enPpSampleRate;
    MED_PP_STRU               *pstPp = MED_PP_GetObjPtr();

    /* 获取PP模块采样率*/
    enPpSampleRate = MED_PP_GetSampleRate();

    /* 获取结构体参数 */
    pshwDtdSpBandGain = pstDtdNline->ashwSpBandProb;
    shwDtdFreqBinLen  = pstDtdNline->shwFreqBinLen;

    /* 计算NLP频点长度 */
    shwNlpFreqBinLen  = CODEC_OpAdd(CODEC_OpShr(shwFftLen, 1), 1);

    /* 1. 根据DTD中计算的子带语音存在概率对残差进行抑制 */

    /* 动态内存分配 */
    pshwDtdFreqBinGain = (VOS_INT16 *)UCOM_MemAlloc((VOS_UINT16)shwDtdFreqBinLen * sizeof(VOS_INT16));
    pshwNlpFreqBinGain = (VOS_INT16 *)UCOM_MemAlloc((VOS_UINT16)shwNlpFreqBinLen * sizeof(VOS_INT16));

    MED_AEC_NlpGainModulTab(enPpSampleRate, pstPp->enMicNum, &shwNlpProbBandLen, &pshwGainModulTab);

    /* 将DTD中子带增益扩展至频点增益 高子带置0 */
    MED_1MIC_ExtendBand2FreqBinGain(pshwDtdSpBandGain,
                                    shwDtdFreqBinLen,
                                    shwNlpProbBandLen,
                                    pshwDtdFreqBinGain);

    /* 将DTD中频点增益映射至NLP中频点增益 并 施加增益调制表 */
    pshwBinTransTab  = MED_AEC_GetNlpBinTansTab();

    if (shwNlpFreqBinLen == shwDtdFreqBinLen)
    {
        for (shwCnt = 0; shwCnt < shwNlpFreqBinLen; shwCnt++)
        {
            pshwNlpFreqBinGain[shwCnt] = CODEC_OpMult_r(pshwDtdFreqBinGain[shwCnt],
                                                      pshwGainModulTab[shwCnt]);
        }
    }
    else
    {
        for (shwCnt = 0; shwCnt < shwNlpFreqBinLen; shwCnt++)
        {
            shwIndex = pshwBinTransTab[shwCnt];
            pshwNlpFreqBinGain[shwCnt] = CODEC_OpMult_r(pshwDtdFreqBinGain[shwIndex-1],
                                                      pshwGainModulTab[shwCnt]);
        }
    }

    /* 减小语音小概率频点，保留语音大概率频点 */
    for (shwCnt = 0; shwCnt < shwNlpFreqBinLen; shwCnt++)
    {
        /* 利用power(x,alpha)，x在0-1间变化，alpha选大于1整数，函数曲线） 这里alpha选2 */
        pshwNlpFreqBinGain[shwCnt] = CODEC_OpMult(pshwNlpFreqBinGain[shwCnt],
                                                  pshwNlpFreqBinGain[shwCnt]);        /* 16bit Q15=Q15+Q15-Q15 */
    }

    /* 时域平滑 递增慢变，递减快变 */
    pshwNlpFreqBinGainLast = pstBandSupress->ashwNlpFreqBinGain;
    shwFastAlpha           = pstBandSupress->shwNlpBandProbSupFastAlpha;
    shwFastSubAlpha        = CODEC_OpSub(CODEC_OP_INT16_MAX, shwFastAlpha);
    shwSlowAlpha           = pstBandSupress->shwNlpBandProbSupSlowAlpha;
    shwSlowSubAlpha        = CODEC_OpSub(CODEC_OP_INT16_MAX, shwSlowAlpha);

    for (shwCnt = 0; shwCnt < shwNlpFreqBinLen; shwCnt++)
    {
        if (pshwNlpFreqBinGain[shwCnt] > pshwNlpFreqBinGainLast[shwCnt])
        {
            pshwNlpFreqBinGain[shwCnt] =  CODEC_OpAdd(CODEC_OpMult(pshwNlpFreqBinGain[shwCnt], shwFastAlpha),
                                                    CODEC_OpMult(pshwNlpFreqBinGainLast[shwCnt], shwFastSubAlpha));
        }
        else
        {
            pshwNlpFreqBinGain[shwCnt] =  CODEC_OpAdd(CODEC_OpMult(pshwNlpFreqBinGain[shwCnt], shwSlowAlpha),
                                                    CODEC_OpMult(pshwNlpFreqBinGainLast[shwCnt], shwSlowSubAlpha));
        }

        pstBandSupress->ashwNlpFreqBinGain[shwCnt] =  pshwNlpFreqBinGain[shwCnt];
    }

    /* 对残差信号施加NLP增益 */
    pshwErrBinNorm[0] = CODEC_OpMult_r(pshwErrBinNorm[0], pshwNlpFreqBinGain[0]);
    pshwErrBinNorm[1] = CODEC_OpMult_r(pshwErrBinNorm[1], pshwNlpFreqBinGain[shwNlpFreqBinLen-1]);

    for (shwCnt = 1; shwCnt < (shwNlpFreqBinLen - 1); shwCnt++)
    {
        pshwErrBinNorm[shwCnt<<1]   = CODEC_OpMult_r(pshwErrBinNorm[shwCnt<<1], pshwNlpFreqBinGain[shwCnt]);
        pshwErrBinNorm[(shwCnt<<1)+1] = CODEC_OpMult_r(pshwErrBinNorm[(shwCnt<<1)+1], pshwNlpFreqBinGain[shwCnt]);
    }

    /* 动态内存释放 */
    UCOM_MemFree(pshwDtdFreqBinGain);
    UCOM_MemFree(pshwNlpFreqBinGain);
}
VOS_VOID MED_AEC_TransInit(
                MED_AEC_NV_STRU                *pstAecParam,
                CODEC_SAMPLE_RATE_MODE_ENUM_INT32 enSampleRate,
                MED_AEC_TRANS_STRU             *pstTrans)
{
    UCOM_MemSet((VOS_INT16*)pstTrans, 0, sizeof(MED_AEC_TRANS_STRU));

    if (CODEC_SAMPLE_RATE_MODE_8000 == enSampleRate)
    {
        pstTrans->pshwTrapeWin                  = MED_AEC_GetNlpWinNb();
        pstTrans->stErrPreStatus.shwOverlapLen  = MED_AEC_NLP_NB_OVERLAP_LEN;
        pstTrans->stMicPreStatus.shwOverlapLen  = MED_AEC_NLP_NB_OVERLAP_LEN;
        pstTrans->stErrPostStatus.shwOverlapLen = MED_AEC_NLP_NB_OVERLAP_LEN;
        pstTrans->stErrPreStatus.shwFrmLen      = MED_AEC_1MIC_NLP_NB_FRM_LEN;
        pstTrans->stMicPreStatus.shwFrmLen      = MED_AEC_1MIC_NLP_NB_FRM_LEN;
        pstTrans->stErrPostStatus.shwFrmLen     = MED_AEC_1MIC_NLP_NB_FRM_LEN;
        pstTrans->shwFftLen                     = MED_AEC_NLP_MAX_FFT_LEN >> 1;
        pstTrans->enFftNumIndex                 = MED_FFT_NUM_256;
        pstTrans->enIfftNumIndex                = MED_IFFT_NUM_256;
    }
    else
    {
        pstTrans->pshwTrapeWin                  = MED_AEC_GetNlpWinWb();
        pstTrans->stErrPreStatus.shwOverlapLen  = MED_AEC_NLP_WB_OVERLAP_LEN;
        pstTrans->stMicPreStatus.shwOverlapLen  = MED_AEC_NLP_WB_OVERLAP_LEN;
        pstTrans->stErrPostStatus.shwOverlapLen = MED_AEC_NLP_WB_OVERLAP_LEN;
        pstTrans->stErrPreStatus.shwFrmLen      = MED_AEC_1MIC_NLP_WB_FRM_LEN;
        pstTrans->stMicPreStatus.shwFrmLen      = MED_AEC_1MIC_NLP_WB_FRM_LEN;
        pstTrans->stErrPostStatus.shwFrmLen     = MED_AEC_1MIC_NLP_WB_FRM_LEN;
        pstTrans->shwFftLen                     = MED_AEC_NLP_MAX_FFT_LEN;
        pstTrans->enFftNumIndex                 = MED_FFT_NUM_512;
        pstTrans->enIfftNumIndex                = MED_IFFT_NUM_512;
    }
}


VOS_VOID MED_AEC_Time2Freq(
                MED_AEC_TRANS_STRU       *pstTrans,
                VOS_INT16                *pshwNear,
                VOS_INT16                *pshwErr,
                VOS_INT16                *pshwMcBin,
                VOS_INT16                *pshwMcErrBinNorm,
                VOS_INT16                *pshwNormShiftNear)
{

    VOS_INT16               shwNormShift  = 0;

    /* Mic信号数据重叠、加窗、FFT */
    MED_AEC_TransPreProcess(&(pstTrans->stMicPreStatus),
                            pshwNear,
                            CODEC_SWITCH_OFF,
                            pshwMcBin,
                            &shwNormShift,
                            pstTrans->enFftNumIndex);

    /* 残差信号数据重叠、加窗、FFT */
    MED_AEC_TransPreProcess(&(pstTrans->stErrPreStatus),
                            pshwErr,
                            CODEC_SWITCH_ON,
                            pshwMcErrBinNorm,
                            pshwNormShiftNear,
                            pstTrans->enFftNumIndex);

}


VOS_VOID MED_EAEC_Time2Freq(
                MED_AEC_TRANS_STRU       *pstTrans,
                VOS_INT16                *pshwNear,
                VOS_INT16                *pshwErr,
                VOS_INT16                *pshwMcBin,
                VOS_INT16                *pshwMcErrBinNorm,
                VOS_INT16                *pshwNormShiftNear,
                VOS_INT16                *pshwNormShiftErr)
{
    /* Mic信号数据重叠、加窗、FFT */
    MED_AEC_TransPreProcess(&(pstTrans->stMicPreStatus),
                            pshwNear,
                            CODEC_SWITCH_ON,
                            pshwMcBin,
                            pshwNormShiftNear,
                            pstTrans->enFftNumIndex);

    /* 残差信号数据重叠、加窗、FFT */
    MED_AEC_TransPreProcess(&(pstTrans->stErrPreStatus),
                            pshwErr,
                            CODEC_SWITCH_ON,
                            pshwMcErrBinNorm,
                            pshwNormShiftErr,
                            pstTrans->enFftNumIndex);

}


VOS_VOID MED_AEC_2MIC_Time2Freq(
                MED_2MIC_DATAFLOW_MC_STRU        *pstFreqMc,
                MED_2MIC_DATAFLOW_RC_STRU        *pstFreqRc,
                MED_2MIC_COMM_STRU                stComCfg,
                VOS_INT16                        *pshwMcTd,
                VOS_INT16                        *pshwRcTd,
                VOS_INT16                        *pshwMcIn,
                VOS_INT16                        *pshwRcCalib)
{
    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_AEC_2MIC_Time2Freq);

    /* 分别对主麦克信号(pshwMcTd、pshwMcIn)和辅麦克信号(pshwRcTd、pshwRcCalib)进行时频域转换 */
    MED_AEC_2MIC_Fft(&(pstFreqMc->stMc),    &stComCfg, pshwMcTd, CODEC_SWITCH_ON);
    MED_AEC_2MIC_Fft(&(pstFreqRc->stRc),    &stComCfg, pshwRcTd, CODEC_SWITCH_ON);
    MED_AEC_2MIC_Fft(&(pstFreqMc->stOriMc), &stComCfg, pshwMcIn, CODEC_SWITCH_OFF);
    MED_AEC_2MIC_Fft(&(pstFreqRc->stOriRc), &stComCfg, pshwRcCalib, CODEC_SWITCH_OFF);

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_AEC_2MIC_Time2Freq);
}
VOS_VOID MED_AEC_2MIC_Fft(
                MED_2MIC_FREQ_STRU      *pstFreqOut,
                MED_2MIC_COMM_STRU      *pstComCfg,
                VOS_INT16               *pshwTimeIn,
                CODEC_SWITCH_ENUM_UINT16   enNormEnable)
{
    VOS_INT16                           shwInMax     = 0;
    VOS_INT16                          *pswMaxPos    = MED_NULL;
    VOS_INT16                           shwNormShift = 0;
    VOS_INT16                          *pshwHamWin;
    VOS_INT16                           shwFrameLen;
    VOS_INT16                           shwFftLen;
    VOS_INT16                          *pshwOverLap;
    VOS_INT16                           shwOverlapLen;
    VOS_INT16                          *pshwTransIn  = MED_ANR_2MIC_GetshwVecTmp640Ptr1();
    MED_FFT_NUM_ENUM_INT16              enFftNumIndex;

    /* 获取FFT变换相关参数 */
    shwFrameLen     = MED_PP_GetFrameLength();
    shwFftLen       = pstComCfg->shwFftLen;
    enFftNumIndex   = pstComCfg->enFftNumIndex;
    pshwOverLap     = &pstFreqOut->ashwOverlapFft[0];
    shwOverlapLen   = shwFftLen - shwFrameLen;

    /* 汉明窗系数 */
    if (CODEC_FRAME_LENGTH_NB == shwFrameLen)
    {
        pshwHamWin      = (VOS_INT16 *)MED_ANR_2MIC_GetWinNb();
    }
    else
    {
        pshwHamWin      = (VOS_INT16 *)MED_ANR_2MIC_GetWinWb();
    }

    /* 准备FFT变换的时域数据 */
    CODEC_OpVecCpy(pshwTransIn, pshwOverLap, shwOverlapLen);
    CODEC_OpVecCpy(&pshwTransIn[shwFrameLen], pshwTimeIn, shwFrameLen);

    /* 加汉明窗 */
    CODEC_OpVvMultR(pshwTransIn, pshwHamWin, shwOverlapLen, pshwTransIn);
    CODEC_OpVvMultRD(pshwTransIn + shwOverlapLen,
                   pshwHamWin + (shwOverlapLen - 1),
                   shwFrameLen,
                   pshwTransIn + shwOverlapLen);

    /* 归一化 */
    if (CODEC_SWITCH_ON == enNormEnable)
    {
        shwInMax       = CODEC_OpVecMaxAbs(pshwTransIn,
                                         shwFftLen,
                                         pswMaxPos);

        shwNormShift   = CODEC_OpNorm_s(shwInMax);

        CODEC_OpVecShl(pshwTransIn,
                     shwFftLen,
                     shwNormShift,
                     pshwTransIn);
    }
    else
    {
        shwNormShift = 0;
    }

    MED_FFT_Fft(enFftNumIndex, pshwTransIn, &pstFreqOut->ashwFreq[0]);

    /* 取当前帧作为下一帧的overlap */
    CODEC_OpVecCpy(pshwOverLap, pshwTimeIn, shwOverlapLen);

    /* 保存归一化系数 */
    pstFreqOut->shwNormShift = shwNormShift;
}
VOS_VOID MED_AEC_2MIC_Freq2Time(
                MED_2MIC_DATAFLOW_MC_STRU         *pstFreqIn,
                MED_2MIC_COMM_STRU                *pstComCfg,
                VOS_INT16                         *pshwTimeOut)
{
    VOS_INT16                           shwNormShift   = 0;
    VOS_INT16                           shwFrameLen;
    VOS_INT16                           shwFftLen;
    VOS_INT16                          *pshwFreqMc;
    VOS_INT16                          *pshwOverLap;
    VOS_INT16                           shwOverlapLen;
    VOS_INT16                          *pshwBuf        = MED_ANR_2MIC_GetshwVecTmp640Ptr1();

    /* 获取FFT变换相关参数 */
    shwFrameLen     =  MED_PP_GetFrameLength();
    shwFftLen       =  pstComCfg->shwFftLen;
    pshwFreqMc      = &pstFreqIn->stMc.ashwFreq[0];
    shwNormShift    =  pstFreqIn->stMc.shwNormShift;
    pshwOverLap     = &pstFreqIn->ashwOverlapIfft[0];
    shwOverlapLen   =  shwFftLen - shwFrameLen;

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_AEC_2MIC_Freq2Time);

    MED_FFT_Ifft(pstComCfg->enIfftNumIndex, pshwFreqMc, pshwBuf);

    /* 反归一化 */
    CODEC_OpVecShr_r(pshwBuf,
                   shwFftLen,
                   shwNormShift,
                   pshwBuf);

    /* 数据重叠 */
    CODEC_OpVvAdd(pshwBuf,
                pshwOverLap,
                shwOverlapLen,
                pshwBuf);

    /* 更新overlap信息 */
    CODEC_OpVecCpy(pshwOverLap, (pshwBuf + shwFrameLen), shwOverlapLen);

    /* 残差 */
    CODEC_OpVecCpy(pshwTimeOut, pshwBuf, shwFrameLen);

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_AEC_2MIC_Freq2Time);

}
VOS_VOID MED_AEC_Freq2Time(
                MED_AEC_TRANS_STRU          *pstTrans,
                VOS_INT16                   *pshwMcErrBinNorm,
                VOS_INT16                    shwNormShift,
                VOS_INT16                   *pshwErrOut)
{
    MED_AEC_TransPostProcess(&(pstTrans->stErrPostStatus),
                               pshwMcErrBinNorm,
                               shwNormShift,
                               pshwErrOut,
                               pstTrans->shwFftLen,
                               pstTrans->enIfftNumIndex,
                               pstTrans->pshwTrapeWin);



}


VOS_VOID MED_AEC_TransPreProcess(
                MED_AEC_OVERLAP_STRU        *pstPreStatus,
                VOS_INT16                   *pshwStream,
                VOS_INT16                    shwNormEn,
                VOS_INT16                   *pshwSpecBin,
                VOS_INT16                   *pshwNormShift,
                VOS_INT16                    shwFftNumIndex)
{
    VOS_INT16              *pshwBuf;                                            /* size MED_AEC_NLP_MAX_FFT_LEN */
    VOS_INT16               shwFrmLen;
    VOS_INT16               shwOverlapLen;
    VOS_INT16               shwFftLen;
    VOS_INT16              *pshwTrapeWin;
    VOS_INT16              *pswMaxPos = MED_NULL;
    VOS_INT16               shwInMax;

    shwFrmLen           = pstPreStatus->shwFrmLen;
    shwOverlapLen       = pstPreStatus->shwOverlapLen;
    shwFftLen           = shwFrmLen + shwOverlapLen;

    pshwBuf             = MED_AEC_GetshwVecTmp640Ptr1();

    if (MED_AEC_NLP_NB_OVERLAP_LEN == shwOverlapLen)
    {
        pshwTrapeWin        = MED_AEC_GetNlpWinNb();
    }
    else
    {
        pshwTrapeWin        = MED_AEC_GetNlpWinWb();
    }

    /* 前1帧的 overlap */
    CODEC_OpVecCpy(pshwBuf, pstPreStatus->ashwOverlap, shwOverlapLen);

    /* 本帧数据 */
    CODEC_OpVecCpy(pshwBuf + shwOverlapLen, pshwStream, shwFrmLen);

    /* 取当前帧的结尾部分作为下1帧的overlap */
    CODEC_OpVecCpy(pstPreStatus->ashwOverlap, pshwBuf + shwFrmLen, shwOverlapLen);

    /* 加窗 */
    CODEC_OpVvMultR(pshwBuf, pshwTrapeWin, shwFftLen, pshwBuf);

    /* 归一化 */
    if (CODEC_SWITCH_ON == shwNormEn)
    {
        shwInMax       = CODEC_OpVecMaxAbs(pshwBuf,
                                           shwFftLen,
                                           pswMaxPos);

        *pshwNormShift = CODEC_OpNorm_s(shwInMax);

        CODEC_OpVecShl(pshwBuf,
                       shwFftLen,
                      *pshwNormShift,
                       pshwBuf);
    }
    else
    {
        *pshwNormShift = 0;
    }

    MED_FFT_Fft(shwFftNumIndex, pshwBuf, pshwSpecBin);

}


VOS_VOID MED_AEC_TransPostProcess(
                MED_AEC_OVERLAP_STRU        *pstPostStatus,
                VOS_INT16                   *pshwErrBin,
                VOS_INT16                    shwNormShift,
                VOS_INT16                   *pshwErrFrm,
                VOS_INT16                    shwFftLen,
                VOS_INT16                    shwIfftNumIndex,
                VOS_INT16                   *pshwTrapeWin)

{
    VOS_INT16              *pshwBuf;                                            /* size MED_AEC_NLP_MAX_FFT_LEN */
    VOS_INT16               shwFrmLen     = pstPostStatus->shwFrmLen;
    VOS_INT16               shwOverlapLen = pstPostStatus->shwOverlapLen;

    pshwBuf             = MED_AEC_GetshwVecTmp640Ptr1();

    MED_FFT_Ifft(shwIfftNumIndex, pshwErrBin, pshwBuf);

    /* 反归一化 */
    CODEC_OpVecShr_r(pshwBuf,
                     shwOverlapLen + shwFrmLen,
                     shwNormShift,
                     pshwBuf);

    /* 反变换后数据加窗 */
    CODEC_OpVvMultR(pshwBuf, pshwTrapeWin, shwOverlapLen + shwFrmLen, pshwBuf);


    /* 数据重叠 */
    CODEC_OpVvAdd(pshwBuf,
                  pstPostStatus->ashwOverlap,
                  shwOverlapLen,
                  pshwBuf);

    CODEC_OpVecCpy(pstPostStatus->ashwOverlap, (pshwBuf + shwFrmLen), (shwFftLen - shwFrmLen));

    /* 残差 */
    CODEC_OpVecCpy(pshwErrFrm, pshwBuf, shwFrmLen);

}
VOS_VOID MED_AEC_NlpGainModulTab(
                                 CODEC_SAMPLE_RATE_MODE_ENUM_INT32   enPpSampleRate,
                                 MED_PP_MIC_NUM_ENUM_UINT16          enMicNum,
                                 VOS_INT16                          *pshwNlpProbBandLen,
                                 VOS_INT16                         **ppshwGainModulTab)
{
    if((CODEC_SAMPLE_RATE_MODE_BUTT == enPpSampleRate) || (enMicNum > 2))
    {
        return;
    }

    /* 根据采样率获得截止子带长度和NLP子带抑制各频点增益调制系数表数据 */
    if (CODEC_SAMPLE_RATE_MODE_8000 == enPpSampleRate)
    {
        *pshwNlpProbBandLen = MED_AEC_NLP_CAL_SP_PROB_BAND_LEN_NB;

        /* 根据麦克数获得NLP子带抑制各频点增益调制系数表数据 */
        if (MED_PP_MIC_NUM_1 == enMicNum)
        {
            *ppshwGainModulTab = MED_AEC_GetNlpGainModulTabNB();
        }
        else
        {
            *ppshwGainModulTab = MED_AEC_Get2MicNlpGainModulTabNB();
        }
    }
    else
    {
        *pshwNlpProbBandLen = MED_AEC_NLP_CAL_SP_PROB_BAND_LEN_WB;

        if (MED_PP_MIC_NUM_1 == enMicNum)
        {
            *ppshwGainModulTab = MED_AEC_GetNlpGainModulTabWB();
        }
        else
        {
            *ppshwGainModulTab = MED_AEC_Get2MicNlpGainModulTabWB();
        }

    }
}

VOS_VOID MED_1MIC_ExtendBand2FreqBinGain(
                VOS_INT16               *pshwBandGain,
                VOS_INT16                shwFreqBinLen,
                VOS_INT16                shwBandLen,
                VOS_INT16               *pshwFreqBinGain)
{
    VOS_INT16 shwCntI;
    VOS_INT16 shwCntJ;
    VOS_INT16 shwChLo;
    VOS_INT16 shwChHi;

    VOS_INT16 *pshwMedAnr2MicChTbl = (VOS_INT16 *)MED_2MIC_GetAnr2MicChTblPtr();

    UCOM_MemSet(pshwFreqBinGain,
                0,
                (VOS_UINT16)shwFreqBinLen * sizeof(VOS_INT16));

    /* 计算每个子带的增益 */
    for (shwCntI = 0; shwCntI < shwBandLen; shwCntI++)
    {
        /* 子带范围 */
        shwChLo = *((pshwMedAnr2MicChTbl + (shwCntI * 2)) + 0);
        shwChHi = *((pshwMedAnr2MicChTbl + (shwCntI * 2)) + 1);

        for (shwCntJ = (shwChLo - 1); shwCntJ < shwChHi; shwCntJ++)
        {
            pshwFreqBinGain[shwCntJ] = pshwBandGain[shwCntI];
        }
    }
}


#ifdef  __cplusplus
#if  __cplusplus
}
#endif
#endif
