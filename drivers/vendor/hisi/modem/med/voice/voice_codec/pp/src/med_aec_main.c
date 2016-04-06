
#include "med_aec_main.h"
#include "med_filt.h"
#include "med_gain.h"
#include "med_vad.h"
#include "med_eanr.h"
#include "med_anr_2mic_interface.h"
#include "med_avc.h"
#include "med_pp_comm.h"
#include "med_pp_main.h"

#include "med_mbdrc.h"
#include "voice_debug.h"
#include "om.h"

#ifdef  __cplusplus
#if  __cplusplus
extern "C"{
#endif
#endif

/*****************************************************************************
   2 全局变量定义
*****************************************************************************/
MED_AEC_OBJ_STRU                 g_astMedAecObjPool[1];                          /* AEC内部数据实体 */
VOS_INT16                        g_shwAecMcBin[MED_AEC_NLP_MAX_FFT_LEN];
VOS_INT16                        g_shwAecMcErrNLPBinNorm[MED_AEC_NLP_MAX_FFT_LEN];
VOS_INT16                        g_shwAecMcErrAFBinNorm[MED_AEC_NLP_MAX_FFT_LEN];
MED_OBJ_INFO                     g_stMedAecObjInfo;

/*****************************************************************************
   3 函数实现
*****************************************************************************/

VOS_VOID* MED_AEC_Create(VOS_VOID)
{
    return (MED_PP_Create(
                    MED_AEC_GetObjInfoPtr(),
                    MED_AEC_GetObjPtr(),
                    MED_AEC_MAX_OBJ_NUM,
                    sizeof(MED_AEC_OBJ_STRU)));
}


VOS_UINT32  MED_AEC_Destroy(VOS_VOID  **ppAecObj)
{
    VOS_UINT32              uwRet;
    MED_AEC_OBJ_STRU       *pstObj;
    pstObj      = (MED_AEC_OBJ_STRU *)(*ppAecObj);

    /* 判断入参是否合法 */
    uwRet       = MED_PP_CheckPtrValid(MED_AEC_GetObjInfoPtr(), pstObj);

    /* 入参合法，则将指针对象释放，并将指针赋空 */
    if (UCOM_RET_SUCC == uwRet)
    {
        UCOM_MemSet(pstObj, 0, sizeof(MED_AEC_OBJ_STRU));
        pstObj->enIsUsed  = CODEC_OBJ_UNUSED;
        *ppAecObj         = MED_NULL;
    }

    return uwRet;
}


VOS_UINT32 MED_AEC_SetPara (
                VOS_VOID                        *pAecInstance,
                VOS_INT16                       *pshwAecParam,
                VOS_INT16                        shwParaLen,
                CODEC_SAMPLE_RATE_MODE_ENUM_INT32  enSampleRate,
                MED_PP_MIC_NUM_ENUM_UINT16       enPpMicNum)
{
    VOS_INT16                           shwFlag;
    MED_AEC_NV_STRU                    *pstNv;
    MED_AEC_OBJ_STRU                   *pstObj;

    pstObj = (MED_AEC_OBJ_STRU*)pAecInstance;
    pstNv  = (MED_AEC_NV_STRU*)pshwAecParam;

    /* 长度参数校验 */
    if (shwParaLen < MED_AEC_NV_LEN)
    {
        return UCOM_RET_ERR_PARA;
    }

    /* 实体参数校验 */
    if (MED_PP_CheckPtrValid(MED_AEC_GetObjInfoPtr(), pstObj) != UCOM_RET_SUCC)
    {
        return UCOM_RET_ERR_PARA;
    }

    /* NV项参数校验 */
    shwFlag =
           (pstNv->shwOffsetLen        > MED_AEC_MAX_OFFSET             )       /* 最大补偿长度，单位采样点 Q0*/
        || (pstNv->shwAfTailLen        > MED_AEC_MAX_TAIL_LEN           )       /* 最大尾端长度，单位ms，最大支持60ms尾端延迟 Q0*/
        || (pstNv->shwDtdStModiThd     > MED_AEC_MAX_DTD_ST_MODI_THD    )       /* DTD单讲修正级别最大值 Q0*/
        || (pstNv->shwDtdDtModiThd     > MED_AEC_MAX_DTD_DT_MODI_THD    )       /* DTD双讲修正级别最大值 Q15*/
        || (pstNv->shwDtdMutePowerThd  > MED_AEC_MAX_DTD_POWER_THD      )       /* DTD能量级别最大值 Q0*/
        || (pstNv->shwNlpDt2StLvl      > MED_AEC_MAX_NLP_DT2ST_THD      )       /* NLP双讲切换单讲级别最大值 Q0*/
        || (pstNv->shwNlpSt2DtLvl      > MED_AEC_MAX_NLP_ST2DT_THD      )       /* NLP单讲切换双讲级别最大值 Q0*/
        || (pstNv->shwNlpCngInsertLvl  > MED_AEC_MAX_NLP_CNG_THD        )       /* NLP插入舒适噪声级别最大值 Q0*/
        || (pstNv->shwNlpNoiseFloorThd > MED_AEC_MAX_NLP_NOISE_FLOOR_THD)       /* NLP本底噪声级别最大值 Q0*/
        || (pstNv->shwNlpNonlinearThd  > MED_AEC_MAX_NLP_NON_LINEAR_THD )       /* NLP高频非线性抑制门限最大值 Q0*/
        || (pstNv->enEstFlag           > CODEC_SWITCH_BUTT                )     /* DTD增强单讲(EST)使能开关*/
        || (pstNv->shwEstPowerThd      > MED_AEC_MAX_DTD_EST_POWER_THD  )       /* DTD增强单讲(EST)能量阈值最大值*/
        || (pstNv->shwEstAmplThd       > MED_AEC_MAX_DTD_EST_AMPL_THD   )       /* DTD增强单讲(EST)幅度阈值最大值*/
        || (pstNv->enNearFarRatioEnable> CODEC_SWITCH_BUTT )                    /* 非线性DTD近端与远端功率谱密度比值使能 */
        || (pstNv->shwNearFarRatioGain > MED_AEC_MAX_DTD_NLINE_NEAR_FAR_RATIO_GAIN ) /* 非线性DTD远端功率谱密度的增益最大值 Q11 */
        || (pstNv->shwSpSerThd         > MED_AEC_MAX_DTD_NLINE_SP_SER_THD )     /* 非线性DTD语音存在概率为1的SER门限最大值 Q11 */
        || (pstNv->shwEchoSerThd       > MED_AEC_MAX_DTD_NLINE_ECHO_SER_THD )   /* 非线性DTD回声存在概率为0的SER门限最大值 Q11 */
        || (pstNv->shwBandPsdMuteThd   > MED_AEC_MAX_DTD_NLINE_BAND_PSD_MUTE_THD ) /* 非线性DTD近端功率谱密度判为静音的门限最大值 Q0 */
        || (pstNv->shwSpThdInit        > MED_AEC_MAX_DTD_NLINE_SP_THD_INIT  )   /* 非线性DTD双讲判定语音存在概率阈值初始值的最大值 Q15 */
        || (pstNv->shwSpThdMax         > MED_AEC_MAX_DTD_NLINE_SP_THD_MAX   )   /* 非线性DTD双讲判定语音存在概率阈值最大值的最大值 Q15 */
        || (pstNv->shwSpThdMin         > MED_AEC_MAX_DTD_NLINE_SP_THD_MIN   )   /* 非线性DTD双讲判定语音存在概率阈值最小值的最大值 Q15 */
        || (pstNv->shwSumPsdThd        > MED_AEC_MAX_DTD_NLINE_SUM_PSD_THD  )   /* 非线性DTD双讲判定剩余能量阈值最大值的最大值 Q15 */
        || (pstNv->shwNlpRefCnt        > MED_AEC_MAX_NLP_OVERDRIVE_FAR_CNT )    /* NLP: 远端信号计数 */
        || (pstNv->shwNlpRefAmp1       > MED_AEC_MAX_NLP_OVERDRIVE_FAR_THD )    /* NLP: 远端信号能量门限1，用于抑制第一声回声 */
        || (pstNv->shwNlpExOverdrive   > MED_AEC_MAX_NLP_OVERDRIVE_MAX )        /* NLP: 加强的Overdrive */
        || (pstNv->shwNlpResdPowAlph   > MED_AEC_MAX_NLP_STSUPPRESS_ALPH )      /* NLP: 残差信号能量滤波系数 */
        || (pstNv->shwNlpResdPowThd    > MED_AEC_MAX_NLP_STSUPPRESS_POWTHD )    /* NLP: 残差信号能量门限 */
        || (pstNv->shwNlpSmoothGainDod >= MED_AEC_MAX_NLP_SMOOTH_GAIN_DOD )     /* NLP:增益因子幂指数的最大门限*/
        || (pstNv->shwNlpSmoothGainDod < -1 )                                   /* NLP:增益因子幂指数的最小门限*/
        || (pstNv->shwNlpBandSortIdx   > MED_AEC_MAX_NLP_BANDSORT_IDX )         /* NLP:排序后IDX的最大门限*/
        || (pstNv->shwNlpBandSortIdxLow < 0 );                                  /* NLP:排序后IDX的最小门限*/


    if (shwFlag)
    {
        return UCOM_RET_ERR_PARA;
    }

    /* AEC实体状态清零 */
    UCOM_MemSet(pstObj, 0, sizeof(MED_AEC_OBJ_STRU));

    pstObj->enIsUsed = CODEC_OBJ_USED;

    pstObj->stDelay.shwOffsetLen        = pstNv->shwOffsetLen;
    pstObj->shwIsEnable                 = pstNv->enAecEnable;
    pstObj->shwIsEAecEnable             = pstNv->enEAecEnable;
    pstObj->stMcAf.stConfig.enAfEnable  = pstNv->enAecEnable;
    pstObj->stRcAf.stConfig.enAfEnable  = pstNv->enAecEnable;
    pstObj->stNlp.stConfig.enNlpEnable  = pstNv->enAecEnable;

    /* AF初始化 */
    MED_AEC_AfInit(pstNv, enSampleRate, &pstObj->stMcAf);
    MED_AEC_AfInit(pstNv, enSampleRate, &pstObj->stRcAf);

    MED_AEC_HF_AfInit(enSampleRate, &pstObj->stEAecHfAf);

    /* DTD初始化 */
    MED_AEC_DtdInit(pstNv, enSampleRate, &pstObj->stDtd);

    /* NLP初始化 */
    MED_AEC_NlpInit(pstNv, enSampleRate, &pstObj->stNlp, enPpMicNum);

    MED_AEC_HF_NlpInit(pstNv, enSampleRate, &pstObj->stEAecHfNlp);

    /* TRANS初始化 */
    MED_AEC_TransInit(pstNv, enSampleRate, &pstObj->stTrans);

    return UCOM_RET_SUCC;

}
VOS_UINT32 MED_AEC_GetPara(
                VOS_VOID         *pAecInstance,
                VOS_INT16        *pshwAecParam,
                VOS_INT16         shwParaLen)
{
    MED_AEC_NV_STRU              *pstNv;
    MED_AEC_OBJ_STRU             *pstObj = (MED_AEC_OBJ_STRU*)pAecInstance;

    /* NV项校验 */
    if (shwParaLen < (VOS_INT16)(sizeof(MED_AEC_NV_STRU)/sizeof(VOS_INT16)))

    {
        return UCOM_RET_ERR_PARA;
    }

    /* AEC实体状态清零 */
    CODEC_OpVecSet(pshwAecParam, shwParaLen, 0);

    /* 从pAecInstance中提取NV参数到stNv中 */
    pstNv = (MED_AEC_NV_STRU*)pshwAecParam;

    pstNv->enAecEnable         =  pstObj->shwIsEnable;
    pstNv->enEAecEnable        =  pstObj->shwIsEAecEnable;
    pstNv->shwOffsetLen        =  pstObj->stDelay.shwOffsetLen;
    pstNv->shwAfTailLen        =  pstObj->stMcAf.stConfig.shwTailLen;

    pstNv->shwDtdDtModiThd     =  pstObj->stDtd.stDtdLine.shwDtModiThd;
    pstNv->shwDtdStModiThd     =  pstObj->stDtd.stDtdLine.shwStModiThd;
    pstNv->shwDtdMutePowerThd  =  pstObj->stDtd.shwMutePowerThd;

    pstNv->shwNlpDt2StLvl      =  pstObj->stNlp.stConfig.shwDt2StLvl;
    pstNv->shwNlpSt2DtLvl      =  pstObj->stNlp.stConfig.shwSt2DtLvl;
    pstNv->shwNlpCngInsertLvl  =  pstObj->stNlp.stConfig.shwCngInsertLvl;
    pstNv->shwNlpMaxSuppressLvl=  pstObj->stNlp.stConfig.shwMaxSuppress;
    pstNv->shwNlpNonlinearThd  =  pstObj->stNlp.stConfig.shwNonlinearThd;
    pstNv->shwNlpBandProbSupFastAlpha = pstObj->stNlp.stConfig.shwNlpBandProbSupFastAlpha;
    pstNv->shwNlpBandProbSupSlowAlpha = pstObj->stNlp.stConfig.shwNlpBandProbSupSlowAlpha;

    pstNv->enEstFlag           =  pstObj->stDtd.enEstFlag;
    pstNv->shwEstPowerThd      =  pstObj->stDtd.shwEstPowerThd;
    pstNv->shwEstAmplThd       =  pstObj->stDtd.shwEstAmplThd;

    pstNv->enNearFarRatioEnable= pstObj->stDtd.stDtdNline.enNearFarRatioEnable;
    pstNv->shwNearFarRatioActiveFrmNum = pstObj->stDtd.stDtdNline.shwNearFarRatioActiveFrmNum;
    pstNv->shwNearFarRatioGain = pstObj->stDtd.stDtdNline.shwNearFarRatioGain;
    pstNv->shwSpSerThd         = pstObj->stDtd.stDtdNline.shwSpSerThd;
    pstNv->shwEchoSerThd       = pstObj->stDtd.stDtdNline.shwEchoSerThd;
    pstNv->shwBandPsdMuteThd   = pstObj->stDtd.stDtdNline.shwBandPsdMuteThd;

    pstNv->shwSpThdInit        =  pstObj->stDtd.stDtdNline.shwSpThdInit;
    pstNv->shwSpThdMax         =  pstObj->stDtd.stDtdNline.shwSpThdMax;
    pstNv->shwSpThdMin         =  pstObj->stDtd.stDtdNline.shwSpThdMin;

    pstNv->shwSumPsdThd        =  CODEC_OpSaturate(pstObj->stDtd.stDtdNline.swSumPsdThd   \
                                  / (VOS_INT32)MED_AEC_DTD_NLINE_SUM_PSD_THD_BASE);

    pstNv->shwNlpNoiseFloorThd =  shl(1, pstObj->stNlp.stConfig.shwNoiseFloorValue);

    pstNv->shwNlpRefCnt        =  pstObj->stEAecHfNlp.stSmoothGain.shwRefCntThd;
    pstNv->shwNlpExOverdrive   =  pstObj->stEAecHfNlp.stSmoothGain.shwExOverdrive;
    pstNv->shwNlpRefAmp1       =  pstObj->stEAecHfNlp.stSmoothGain.shwRefAmp1;

    pstNv->shwNlpResdPowAlph   =  pstObj->stEAecHfNlp.stSTSuppress.shwResdPowAlph;
    pstNv->shwNlpResdPowThd    =  pstObj->stEAecHfNlp.stSTSuppress.shwResdPowThd;

    pstNv->shwNlpSmoothGainDod =  pstObj->stEAecHfNlp.stSmoothGain.shwOvrdod;

    pstNv->shwNlpBandSortIdx   =  pstObj->stEAecHfNlp.stBandSort.shwHnlIdx;
    pstNv->shwNlpBandSortIdxLow=  pstObj->stEAecHfNlp.stBandSort.shwHnlIdxLow;

    return UCOM_RET_SUCC;
}
VOS_UINT32  MED_1MIC_AECANR_Main(
                VOS_INT16              *pshwMicIn,
                VOS_INT16              *pshwLineOut,
                VOS_INT16               enVadFlag,
                VOS_VOID               *pstPreEmpFar,
                VOS_VOID               *pstHpfAecMc,
                VOS_VOID               *pstAvc)
{
    VOS_INT16                           shwOffsetLen;
    VOS_INT16                          *pshwOffsetBuf;
    VOS_INT16                           shwFreqLen;
    VOS_INT16                           shwSubFramIndex;
    MED_PP_STRU                        *pstpp      = MED_PP_GetObjPtr();
    MED_AEC_OBJ_STRU                   *pstAECObj;
    MED_EANR_STRU                      *pstANRObj;
    MED_AEC_OFFSET_OBJ_STRU            *pstCompObj;
    MED_AEC_AF_STRU                    *pstAfObj;
    MED_AEC_DTD_STRU                   *pstDtdObj    = MED_NULL;
    MED_AEC_NLP_STRU                   *pstNlpObj    = MED_NULL;
    MED_AEC_TRANS_STRU                 *pstTransObj  = MED_NULL;
    MED_AEC_HF_AF_STRU                 *pstEAecHfAf  = MED_NULL;
    MED_AEC_HF_NLP_STRU                *pstEAecHfNlp = MED_NULL;
    CODEC_SWITCH_ENUM_UINT16            shwEAecEnable;
    VOS_INT32                           aswErr[CODEC_FRAME_LENGTH_WB/2];
    VOS_INT16                          *pshwErr = (VOS_INT16*)aswErr;
    VOS_INT16                          *pshwErrAF;
    VOS_INT16                          *pshwErrNlpTmp;
    VOS_INT16                          *pshwErrAFTmp;
    VOS_INT16                          *pshwMicInTmp;
    VOS_INT16                          *pshwMcBin        = MED_AEC_GetMcBinPtr();
    VOS_INT16                          *pshwMcErrNLPBinNorm
                                                         = MED_AEC_GetMcErrNLPBinNormPtr();
    VOS_INT16                          *pshwMcErrAFBinNorm
                                                         = MED_AEC_GetMcErrAFBinNormPtr();
    VOS_INT16                          *pshwAnrBinRef;
    VOS_INT16                          *pshwDelayFar;
    VOS_INT16                          *pshwDelayNear;
    VOS_INT16                           shwNormShiftAF;
    VOS_INT16                           shwNormShiftNLP;
    VOS_INT16                           ashwTxBinTmp[MED_AEC_NLP_MAX_FFT_LEN];
    VOS_INT16                           shwFrameLength = MED_PP_GetFrameLength();

    pstAECObj   = (MED_AEC_OBJ_STRU *)pstpp->pstAec;
    pstANRObj   = (MED_EANR_STRU *)pstpp->pstAnrTx;

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_1MIC_AECANR_Main);

    if (CODEC_OBJ_UNUSED == pstAECObj->enIsUsed)
    {
        return UCOM_RET_FAIL;
    }

    pstCompObj    = &pstAECObj->stDelay;
    pstAfObj      = &pstAECObj->stMcAf;
    pstDtdObj     = &pstAECObj->stDtd;
    pstNlpObj     = &pstAECObj->stNlp;
    pstTransObj   = &pstAECObj->stTrans;
    pstEAecHfAf   = &pstAECObj->stEAecHfAf;
    pstEAecHfNlp  = &pstAECObj->stEAecHfNlp;
    shwEAecEnable = pstAECObj->shwIsEAecEnable;

    pshwOffsetBuf = pstCompObj->ashwOffsetBuf;
    shwOffsetLen  = pstCompObj->shwOffsetLen;
    shwFreqLen    = pstNlpObj->shwFftLen;


    if (   (CODEC_SWITCH_OFF == pstANRObj->shwEnable)
        && (CODEC_SWITCH_OFF == pstAECObj->shwIsEnable))
    {
        CODEC_OpVecCpy(pshwLineOut, pshwMicIn, (VOS_INT32)shwFrameLength);
        return UCOM_RET_SUCC;
    }

    /* 分配内存 */
    pshwAnrBinRef = (VOS_INT16 *)UCOM_MemAlloc((VOS_UINT16)shwFreqLen * sizeof(VOS_INT16));
    pshwDelayFar  = (VOS_INT16 *)UCOM_MemAlloc((VOS_UINT16)(MED_AEC_MAX_OFFSET + CODEC_FRAME_LENGTH_WB) * sizeof(VOS_INT16));
    pshwDelayNear = (VOS_INT16 *)UCOM_MemAlloc((VOS_UINT16)(MED_AEC_MAX_OFFSET + CODEC_FRAME_LENGTH_WB) * sizeof(VOS_INT16));
    pshwErrAF     = (VOS_INT16 *)UCOM_MemAlloc((VOS_UINT16)CODEC_FRAME_LENGTH_WB * sizeof(VOS_INT16));

    /* AEC参考数据钩取 */
    VOICE_DbgSendHook(VOICE_HOOK_AECREF_PCM, &pstAfObj->ashwFar[0], (VOS_UINT16)shwFrameLength);

    /* AEC远端信号预加重滤波 */
    MED_PREEMP_Main(pstPreEmpFar, &pstAfObj->ashwFar[0], &pstAfObj->ashwFar[0]);

    if (shwOffsetLen > MED_AEC_OFFSET_THD)
    {
        /* 延迟补偿 */
        CODEC_OpVecCpy(&pshwOffsetBuf[shwOffsetLen], &pstAfObj->ashwFar[0], (VOS_INT32)shwFrameLength);

        CODEC_OpVecCpy(&pshwDelayFar[0], &pshwOffsetBuf[0], (VOS_INT32)shwFrameLength);

        CODEC_OpVecCpy(&pshwDelayNear[0], &pshwMicIn[0], (VOS_INT32)shwFrameLength);
    }
    else
    {
        shwOffsetLen = CODEC_OpAbs_s(shwOffsetLen - MED_AEC_OFFSET_THD);
        /* 延迟补偿 */
        CODEC_OpVecCpy(&pshwOffsetBuf[shwOffsetLen], &pshwMicIn[0], (VOS_INT32)shwFrameLength);

        CODEC_OpVecCpy(&pshwDelayNear[0], &pshwOffsetBuf[0], (VOS_INT32)shwFrameLength);

        CODEC_OpVecCpy(&pshwDelayFar[0], &pstAfObj->ashwFar[0], (VOS_INT32)shwFrameLength);

    }

    /* 采用AEC */
    if (CODEC_SWITCH_OFF == shwEAecEnable)
    {
        /* 将远端信号除2，避免溢出失真 */
        CODEC_OpVecShr_r(&pshwDelayFar[0],
                       (VOS_INT32)shwFrameLength,
                       1,
                       &pshwDelayFar[0]);

        /* 更新远端信号 */
        CODEC_OpVecCpy(&pstAfObj->ashwFar[0],
                      pshwDelayFar,
                      pstAfObj->shwFrameLen);

        /* 静音检测 */
        MED_AEC_DtdMuteDetect(pstDtdObj, &pstAfObj->ashwFar[0]);

        /* 执行自适应滤波AF */
        MED_AEC_AfMain(pstAfObj, pshwDelayNear, pstDtdObj->enDtdFlag, pshwErr);

        /* 执行双讲检测DTD */
        MED_AEC_Dtd(pstDtdObj,
                    pstAfObj->ashwErrFreq,
                    pstAfObj->ashwEchoFreq,
                    pstAfObj->ashwFarFreq[0],
                    pshwErr,
                    pshwMicIn);

        /* 高通滤波 */
        MED_HPF_Main(pstHpfAecMc, pshwErr, pshwErr);

        /* NLP和ANR 注:20ms = 10ms+10ms 两个子帧依次处理 */
        for (shwSubFramIndex = 0; shwSubFramIndex < MED_AEC_NLP_SUB_FRAME_NUM; shwSubFramIndex++)
        {
            pshwErrNlpTmp   = pshwErr   + (shwSubFramIndex * pstNlpObj->shwFrameLen);
            pshwMicInTmp = pshwMicIn + (shwSubFramIndex * pstNlpObj->shwFrameLen);

            /* 时频域转换 */
            MED_AEC_Time2Freq(
                        pstTransObj,
                        pshwMicInTmp,
                        pshwErrNlpTmp,
                        pshwMcBin,
                        pshwMcErrNLPBinNorm,
                        &shwNormShiftNLP);

            /* ANR参考输入赋值 */
            CODEC_OpVecCpy(pshwAnrBinRef, pshwMcErrNLPBinNorm, shwFreqLen);

            /* 采用AEC，不采用EAEC */
            /* 执行非线性抑制NLP */
            MED_AEC_1MIC_NlpMain(
                        pstNlpObj,
                        &(pstDtdObj->stDtdNline),
                        pshwErrNlpTmp,
                        pstDtdObj->enDtdFlag,
                        pshwMcBin,
                        pshwMcErrNLPBinNorm,
                        shwNormShiftNLP);

            /* 残差信号反归一化 */
            CODEC_OpVecShr_r(pshwMcErrNLPBinNorm,
                        (VOS_INT32)pstNlpObj->shwFftLen,
                        shwNormShiftNLP,
                        ashwTxBinTmp);

            /* 噪声抑制ANR */
            (VOS_VOID)MED_EANR_1MIC_Main(pstANRObj,
                                        pshwMcErrNLPBinNorm,
                                        pshwAnrBinRef,
                                        shwNormShiftNLP);

            /* 更新AVC数据 */
            MED_AVC_UpdateTx((MED_AVC_OBJ_STRU *)pstAvc, ashwTxBinTmp, enVadFlag);

            /* 频时域转换 */
            MED_AEC_Freq2Time(
                        pstTransObj,
                        pshwMcErrNLPBinNorm,
                        shwNormShiftNLP,
                        pshwErrNlpTmp);
        }
    }
    else /* 采用EAEC */
    {
        if (CODEC_SWITCH_ON == pstAECObj->shwIsEnable)
        {
		    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_AEC_HF_AfMain);
            /* 自适应滤波 */
            MED_AEC_HF_AfMain (
                    pstEAecHfAf,
                    pshwDelayNear,
                    pshwDelayFar,
                    pshwErr);

            OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_AEC_HF_AfMain);

            OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_AEC_HF_NlpMain);

            /* AF后的残差信号 */
            CODEC_OpVecCpy(pshwErrAF, pshwErr, CODEC_FRAME_LENGTH_WB);
            /* 非线性抑制 */

	        MED_AEC_HF_NlpMain(
	                pstEAecHfNlp,
	                pstEAecHfAf->stCoefUpdt.aswWeight[0],
	                pstEAecHfAf->ashwNear,
	                pstEAecHfAf->ashwFar,
	                pshwErr,
	                pshwErr);

            OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_AEC_HF_NlpMain);
        }
        else
        {
            /* AEC未处理的信号 */
            CODEC_OpVecCpy(pshwErrAF, pshwDelayNear, CODEC_FRAME_LENGTH_WB);
            CODEC_OpVecCpy(pshwErr, pshwDelayNear, CODEC_FRAME_LENGTH_WB);
        }

        /* NLP和ANR 注:20ms = 10ms+10ms 两个子帧依次处理 */
        for (shwSubFramIndex = 0; shwSubFramIndex < MED_AEC_NLP_SUB_FRAME_NUM; shwSubFramIndex++)
        {
            pshwErrNlpTmp = pshwErr   + (shwSubFramIndex * pstNlpObj->shwFrameLen);
            pshwErrAFTmp  = pshwErrAF + (shwSubFramIndex * pstNlpObj->shwFrameLen);

            /* 时频域转换 */
            MED_EAEC_Time2Freq(
                        pstTransObj,
                        pshwErrAFTmp,
                        pshwErrNlpTmp,
                        pshwMcErrAFBinNorm,
                        pshwMcErrNLPBinNorm,
                        &shwNormShiftAF,
                        &shwNormShiftNLP);

            /* ANR参考输入赋值 */
            CODEC_OpVecCpy(pshwAnrBinRef, pshwMcErrAFBinNorm, shwFreqLen);

            /* 残差信号反归一化 */
            CODEC_OpVecShr_r(pshwMcErrNLPBinNorm,
                        (VOS_INT32)pstNlpObj->shwFftLen,
                        shwNormShiftNLP,
                        ashwTxBinTmp);

            OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_EANR_1MIC_Main_TX);
            /* 噪声抑制ANR */
            (VOS_VOID)MED_EANR_1MIC_Main(pstANRObj,
                                        pshwMcErrNLPBinNorm,
                                        pshwAnrBinRef,
                                        shwNormShiftAF);
            OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_EANR_1MIC_Main_TX);

            /* 更新AVC数据 */
            MED_AVC_UpdateTx((MED_AVC_OBJ_STRU *)pstAvc, ashwTxBinTmp, enVadFlag);

            /* 频时域转换 */
            MED_AEC_Freq2Time(
                        pstTransObj,
                        pshwMcErrNLPBinNorm,
                        shwNormShiftNLP,
                        pshwErrNlpTmp);

        }
    }

    /* 更新延迟补偿缓冲buffer */
    CODEC_OpVecCpy(pshwOffsetBuf, pshwOffsetBuf + shwFrameLength, shwOffsetLen);

    /* 将AEC和ANR处理后数据拷入输出数组pshwLineOut[] */
    CODEC_OpVecCpy(pshwLineOut, pshwErr, shwFrameLength);

    /* 释放内存 */
    UCOM_MemFree(pshwAnrBinRef);
    UCOM_MemFree(pshwDelayFar);
    UCOM_MemFree(pshwDelayNear);
    UCOM_MemFree(pshwErrAF);
    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_1MIC_AECANR_Main);

    return UCOM_RET_SUCC;

}


VOS_UINT32 MED_2MIC_AECANR_Main(
                VOS_INT16               *pshwMcIn,
                VOS_INT16               *pshwRcIn,
                VOS_INT16               *pshwLineOut,
                VOS_INT16                enVadFlag,
                VOS_VOID                *pstPreEmpFar,
                VOS_VOID                *pstHpfAecMc,
                VOS_VOID                *pstHpfAecRc,
                VOS_VOID                *pstAvc )
{
    VOS_INT16                           shwOffsetLen;
    VOS_INT16                          *pshwOffsetBuf;
    MED_AEC_OBJ_STRU                   *pstAECObj      = MED_AEC_GetObjPtr();
    MED_ANR_2MIC_STRU                  *pstANR2MicObj  = MED_ANR_2MIC_GetObjPtr();
    MED_AEC_OFFSET_OBJ_STRU            *pstCompObj;
    MED_AEC_AF_STRU                    *pstMcAfObj;
    MED_AEC_AF_STRU                    *pstRcAfObj;
    MED_AEC_DTD_STRU                   *pstDtdObj   = MED_NULL;
    MED_AEC_NLP_STRU                   *pstNlpObj   = MED_NULL;
    VOS_INT16                          *pshwMcErr   = MED_NULL;
    VOS_INT16                          *pshwRcErr   = MED_NULL;
    VOS_INT16                          *pshwMcTd    = MED_NULL;
    VOS_INT16                          *pshwRcTd    = MED_NULL;
    VOS_INT16                          *pshwRcCalib = MED_NULL;
    VOS_UINT32                          uswMemSize;
    VOS_INT16                          *pshwTxBinTmp = MED_NULL;
    VOS_INT16                           shwFrameLength = MED_PP_GetFrameLength();
    MED_2MIC_DATAFLOW_MC_STRU          *pstFreqMc;
    MED_2MIC_DATAFLOW_RC_STRU          *pstFreqRc;

    VOS_INT16                           shwPosMax;
    VOS_INT16                           shwMaxFar;

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_2MIC_AECANR_Main);


    if (CODEC_OBJ_UNUSED == pstAECObj->enIsUsed)
    {
        return UCOM_RET_FAIL;
    }

    /* 申请内存 */
    uswMemSize  = (VOS_UINT16)shwFrameLength * sizeof(VOS_INT16);
    pshwMcErr   = (VOS_INT16 *)UCOM_MemAlloc(uswMemSize);
    pshwRcErr   = (VOS_INT16 *)UCOM_MemAlloc(uswMemSize);
    pshwMcTd    = (VOS_INT16 *)UCOM_MemAlloc(uswMemSize);
    pshwRcTd    = (VOS_INT16 *)UCOM_MemAlloc(uswMemSize);
    pshwRcCalib = (VOS_INT16 *)UCOM_MemAlloc(uswMemSize);

    /* 获取各模块指针 */
    pstCompObj    = &pstAECObj->stDelay;
    pstMcAfObj    = &pstAECObj->stMcAf;
    pstRcAfObj    = &pstAECObj->stRcAf;
    pstDtdObj     = &pstAECObj->stDtd;
    pstNlpObj     = &pstAECObj->stNlp;

    pshwOffsetBuf = pstCompObj->ashwOffsetBuf;
    shwOffsetLen  = pstCompObj->shwOffsetLen;

    if (   (CODEC_SWITCH_OFF == pstANR2MicObj->enEnable)
        && (CODEC_SWITCH_OFF == pstAECObj->shwIsEnable))
    {
        CODEC_OpVecCpy(pshwLineOut, pshwMcIn, (VOS_INT32)shwFrameLength);
        UCOM_MemFree(pshwMcErr);
        UCOM_MemFree(pshwRcErr);
        UCOM_MemFree(pshwMcTd);
        UCOM_MemFree(pshwRcTd);
        UCOM_MemFree(pshwRcCalib);

        return UCOM_RET_SUCC;
    }

    /* AEC参考数据钩取 */
    VOICE_DbgSendHook(VOICE_HOOK_AECREF_PCM, &pstMcAfObj->ashwFar[0], (VOS_UINT16)shwFrameLength);

    /* AEC远端信号预加重滤波 */
    MED_PREEMP_Main(pstPreEmpFar, &pstMcAfObj->ashwFar[0], &pstMcAfObj->ashwFar[0]);

    /* 延迟补偿 */
    CODEC_OpVecCpy(&pshwOffsetBuf[shwOffsetLen], &pstMcAfObj->ashwFar[0], (VOS_INT32)shwFrameLength);

    /* 将远端信号除2，避免溢出失真 */
    CODEC_OpVecShr_r(&pshwOffsetBuf[shwOffsetLen],
                   (VOS_INT32)shwFrameLength,
                   1,
                   &pshwOffsetBuf[shwOffsetLen]);

    /* 更新主辅麦克中存储的远端信号 */
    CODEC_OpVecCpy(&pstMcAfObj->ashwFar[0],
                  pshwOffsetBuf,
                  pstMcAfObj->shwFrameLen);

    CODEC_OpVecCpy(&pstRcAfObj->ashwFar[0],
                  pshwOffsetBuf,
                  pstRcAfObj->shwFrameLen);

    /* 求参考信号最大值 */
    shwMaxFar   = CODEC_OpVecMaxAbs(&pstMcAfObj->ashwFar[0], pstRcAfObj->shwFrameLen, &shwPosMax);
    pstANR2MicObj->pstTdProcess->stAnc.stLaf.shwMaxFar = shwMaxFar;

    /* 静音检测 */
    MED_AEC_DtdMuteDetect(pstDtdObj, &pstMcAfObj->ashwFar[0]);

    /* 主辅麦克分别执行自适应滤波AF */
    MED_AEC_AfMain(pstMcAfObj, pshwMcIn, pstDtdObj->enDtdFlag, pshwMcErr);

    MED_AEC_AfMain(pstRcAfObj, pshwRcIn, pstDtdObj->enDtdFlag, pshwRcErr);

    /* 执行双讲检测DTD */
    MED_AEC_Dtd(pstDtdObj,
                pstMcAfObj->ashwErrFreq,
                pstMcAfObj->ashwEchoFreq,
                pstMcAfObj->ashwFarFreq[0],
                pshwMcErr,
                pshwMcIn);


    /* 主辅麦克分别高通滤波 */
    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_HPF_Main_TX);
    MED_HPF_Main(pstHpfAecMc, pshwMcErr, pshwMcErr);
    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_HPF_Main_TX);

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_HPF_Main_TX);

    MED_HPF_Main(pstHpfAecRc, pshwRcErr, pshwRcErr);
    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_HPF_Main_TX);

    /* 双麦克降噪时域处理 */
    MED_ANR_2MIC_TdProcess(  pstANR2MicObj->enEnable,
                             enVadFlag,
                             pshwMcErr,
                             pshwRcErr,
                             pshwMcTd,
                             pshwRcTd,
                             pshwRcCalib);

    /* 主辅麦克频谱信号数据结构体 */
    pstFreqMc = &(pstANR2MicObj->stDataFlowMc);
    pstFreqRc = &(pstANR2MicObj->stDataFlowRc);

    /* 时频域转换 */
    MED_AEC_2MIC_Time2Freq(pstFreqMc,
                           pstFreqRc,
                           pstANR2MicObj->stComCfg,
                           pshwMcTd,
                           pshwRcTd,
                           pshwMcIn,
                           pshwRcCalib);

    /* 主辅麦克信号进一步回声抑制，在频域处理 */
    MED_AEC_2MIC_NlpMain(pstFreqMc,
                         pstFreqRc,
                         pstNlpObj,
                         &(pstDtdObj->stDtdNline),
                         pshwMcTd,
                         pstDtdObj->enDtdFlag);

    /* 残差信号反归一化 */
    uswMemSize    = (VOS_UINT16)pstNlpObj->shwFftLen * sizeof(VOS_INT16);
    pshwTxBinTmp  = (VOS_INT16 *)UCOM_MemAlloc(uswMemSize);
    CODEC_OpVecShr_r(pstFreqMc->stMc.ashwFreq,
                 (VOS_INT32)pstNlpObj->shwFftLen,
                 pstFreqMc->stMc.shwNormShift,
                 pshwTxBinTmp);


    /* 双麦克频域后处理 */
    MED_ANR_2MIC_FdProcess(pstANR2MicObj->enEnable,
                           pstFreqMc,
                           pstFreqRc,
                           shwMaxFar);

    /* 更新AVC数据 */
    MED_AVC_UpdateTx((MED_AVC_OBJ_STRU *)pstAvc, pshwTxBinTmp, enVadFlag);

    /* 频时转化*/
    MED_AEC_2MIC_Freq2Time(pstFreqMc, &(pstANR2MicObj->stComCfg),pshwMcTd);

    /* 更新延迟补偿缓冲buffer */
    CODEC_OpVecCpy(pshwOffsetBuf, pshwOffsetBuf + shwFrameLength, shwOffsetLen);

    /* 将AEC和ANR处理后数据拷入输出数组pshwLineOut[] */
    CODEC_OpVecCpy(pshwLineOut, pshwMcTd, shwFrameLength);

    /* 释放内存 */
    UCOM_MemFree(pshwMcErr);
    UCOM_MemFree(pshwRcErr);
    UCOM_MemFree(pshwMcTd);
    UCOM_MemFree(pshwRcTd);
    UCOM_MemFree(pshwRcCalib);
    UCOM_MemFree(pshwTxBinTmp);

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_2MIC_AECANR_Main);
    return UCOM_RET_SUCC;
}
VOS_UINT32  MED_AEC_UpdateRx(
                VOS_VOID               *pvAecInstance,
                VOS_INT16              *pshwLineIn)
{
    MED_AEC_OBJ_STRU                   *pstAECObj = (MED_AEC_OBJ_STRU*)pvAecInstance;
    VOS_INT16                          *pshwFar;
    VOS_INT16                           shwFrameLength = MED_PP_GetFrameLength();

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_AEC_UpdateRx);

    /* 如果AEC实体未使能，则异常退出 */
    if (CODEC_OBJ_UNUSED == pstAECObj->enIsUsed)
    {
        return UCOM_RET_FAIL;
    }

    /* 获取远端信号数据，并保存在主麦克AF结构体中，辅麦克也调用此远端信号处理 */
    pshwFar        = &(pstAECObj->stMcAf).ashwFar[0];
    CODEC_OpVecCpy(pshwFar, pshwLineIn, shwFrameLength);

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_AEC_UpdateRx);

    return UCOM_RET_SUCC;
}


VOS_VOID MED_AEC_UpdateTx(VOS_VOID)
{
    MED_AEC_OBJ_STRU                   *pstAECObj  = MED_AEC_GetObjPtr();
    MED_AEC_AF_STRU                    *pstMcAfObj;
    MED_AEC_AF_STRU                    *pstRcAfObj;
    MED_AEC_DTD_STRU                   *pstDtdObj  = MED_NULL;
    CODEC_SWITCH_ENUM_UINT16            shwEAecEnable;

    pstMcAfObj                          = &(pstAECObj->stMcAf);
    pstRcAfObj                          = &(pstAECObj->stRcAf);
    pstDtdObj                           = &(pstAECObj->stDtd);
    shwEAecEnable                       = pstAECObj->shwIsEAecEnable;

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_AEC_UpdateTx);
    /* 采用AEC */
    if (CODEC_SWITCH_OFF == shwEAecEnable)
    {
        MED_AEC_AfMainUpdate(pstMcAfObj, pstDtdObj->enDtdFlag);

        MED_AEC_AfMainUpdate(pstRcAfObj, pstDtdObj->enDtdFlag);
    }

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_AEC_UpdateTx);

    return;
}

#ifdef  __cplusplus
#if  __cplusplus
}
#endif
#endif
