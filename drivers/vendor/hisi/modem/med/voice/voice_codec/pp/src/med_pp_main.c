

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "OmCodecInterface.h"
#include "med_pp_main.h"
#include "om.h"
#include "voice_debug.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
    可维可测信息中包含的C文件编号宏定义
*****************************************************************************/
/*lint -e(767)*/
#define THIS_FILE_ID                    OM_FILE_ID_MED_PP_MAIN_C


/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
MED_PP_STRU                            g_stMedPpObj;                            /* PP全局控制实体 */
AUDIO_PP_STRU                          g_stAudioPpObj;                          /* 音频PP全局控制实体 */

/*****************************************************************************
  3 函数实现
*****************************************************************************/

VOS_INT16 MED_PP_Init()
 {
    VOS_INT16                           shwFlag;
    MED_PP_STRU                        *pstPP = MED_PP_GetObjPtr();

    /* 创建预加重滤波实体 */
    pstPP->pstPreEmpMc   = MED_PREEMP_Create();
    pstPP->pstPreEmpRc   = MED_PREEMP_Create();
    pstPP->pstPreEmpFar  = MED_PREEMP_Create();

    /* 创建去加重滤波实体 */
    pstPP->pstDeEmpMc    = MED_DEEMP_Create();

    /* 创建上行高通滤波实体 */
    pstPP->pstHpfTxMc     = MED_HPF_Create();
    pstPP->pstHpfTxRc     = MED_HPF_Create();
    pstPP->pstHpfAecMc    = MED_HPF_Create();
    pstPP->pstHpfAecRc    = MED_HPF_Create();

    /* 创建下行高通滤波实体 */
    pstPP->pstHpfRx      = MED_HPF_Create();

    /* 创建上行设备增益值实体 */
    pstPP->pstDevGainTx  = MED_DEVGAIN_Create();

    /* 创建下行设备增益值实体 */
    pstPP->pstDevGainRx  = MED_DEVGAIN_Create();

    /* 创建上行补偿滤波器实体 */
    pstPP->pstCompTx     = MED_COMP_Create();

    /* 创建下行补偿滤波器实体 */
    pstPP->pstCompRx     = MED_COMP_Create();

    /* 创建AEC实体 */
    pstPP->pstAec        = MED_AEC_Create();

    /* 创建NS实体 上行 */
    pstPP->pstAnrTx      = MED_EANR_1MIC_Create();
    pstPP->pstAnr2Mic    = MED_ANR_2MIC_Create();

    /* 创建ANR实体 下行 */
    pstPP->pstAnrRx      = MED_EANR_1MIC_Create();

    /* 创建上行AGC实体 */
    pstPP->pstAgcTx      = MED_AGC_Create();

    /* 创建MBDRC实体 */
    pstPP->pstMbdrc      = MED_MBDRC_Create();

    /* 创建上行AIG实体 */
    pstPP->pstAigTx      = MED_AIG_Create();

    /* 创建下行AIG实体 */
    pstPP->pstAigRx      = MED_AIG_Create();

    /* 创建下行AGC实体 */
    pstPP->pstAgcRx      = MED_AGC_Create();

    /* 创建AVC实体 */
    pstPP->pstAvc        = MED_AVC_Create();

    /* 创建上行VAD实体 */
    pstPP->pstVadTx      = MED_VAD_Create();

    /* 创建下行VAD实体 */
    pstPP->pstVadRx      = MED_VAD_Create();

    /* 创建侧音增益实体 */
    pstPP->pstSTGain     = MED_SIDETONE_Create();

    shwFlag =
        (  (MED_NULL == pstPP->pstPreEmpMc)
        || (MED_NULL == pstPP->pstPreEmpRc)
        || (MED_NULL == pstPP->pstPreEmpFar)
        || (MED_NULL == pstPP->pstDeEmpMc)
        || (MED_NULL == pstPP->pstHpfTxMc)
        || (MED_NULL == pstPP->pstHpfTxRc)
        || (MED_NULL == pstPP->pstHpfAecMc)
        || (MED_NULL == pstPP->pstHpfAecRc)
        || (MED_NULL == pstPP->pstHpfRx)
        || (MED_NULL == pstPP->pstDevGainTx)
        || (MED_NULL == pstPP->pstDevGainRx)
        || (MED_NULL == pstPP->pstCompTx)
        || (MED_NULL == pstPP->pstCompRx)
        || (MED_NULL == pstPP->pstAec)
        || (MED_NULL == pstPP->pstAnrTx)
        || (MED_NULL == pstPP->pstAnr2Mic)
        || (MED_NULL == pstPP->pstAnrRx)
        || (MED_NULL == pstPP->pstAgcTx)
        || (MED_NULL == pstPP->pstAgcRx)
        || (MED_NULL == pstPP->pstMbdrc)
        || (MED_NULL == pstPP->pstAigTx)
        || (MED_NULL == pstPP->pstAigRx)
        || (MED_NULL == pstPP->pstAvc)
        || (MED_NULL == pstPP->pstVadTx)
        || (MED_NULL == pstPP->pstVadRx)
        || (MED_NULL == pstPP->pstSTGain) );

    return shwFlag;

}



VOS_UINT32 MED_PpSetPara(
                VOICE_NV_STRU                       *pstNv,
                MED_PP_MIC_NUM_ENUM_UINT16         enPpMicNum )
{
    MED_PP_STRU                        *pstPP           = MED_PP_GetObjPtr();
    VOS_UINT32                          uwRet           = UCOM_RET_SUCC;
    CODEC_SAMPLE_RATE_MODE_ENUM_INT32   enSampleRate    = MED_PP_GetSampleRate();


    /* 入参检查 */
    if (VOS_NULL == pstNv)
    {
        return UCOM_RET_NUL_PTR;
    }

    pstPP->enMicNum   = enPpMicNum;

    pstPP->uhwFrameId = 0;

    /* 预加重滤波参数加载 */
    uwRet += MED_PREEMP_SetPara(  pstPP->pstPreEmpMc,
                                  pstNv->ashwPreEmp,
                                  CODEC_NV_PREEMP_LEN);
    uwRet += MED_PREEMP_SetPara(  pstPP->pstPreEmpRc,
                                  pstNv->ashwPreEmp,
                                  CODEC_NV_PREEMP_LEN);
    uwRet += MED_PREEMP_SetPara(  pstPP->pstPreEmpFar,
                                  pstNv->ashwPreEmp,
                                  CODEC_NV_PREEMP_LEN);

    /* 去加重滤波参数加载 */
    uwRet += MED_DEEMP_SetPara(   pstPP->pstDeEmpMc,
                                  pstNv->ashwDeEmp,
                                  CODEC_NV_DEEMP_LEN);

    /* 上行高通滤波参数加载 */
    uwRet += MED_HPF_SetPara(     pstPP->pstHpfTxMc,
                                  pstNv->ashwHpfTx,
                                  CODEC_NV_HPF_TX_LEN);
    uwRet += MED_HPF_SetPara(     pstPP->pstHpfTxRc,
                                  pstNv->ashwHpfTx,
                                  CODEC_NV_HPF_TX_LEN);
    uwRet += MED_HPF_SetPara(     pstPP->pstHpfAecMc,
                                  pstNv->ashwHpfTx,
                                  CODEC_NV_HPF_TX_LEN);
    uwRet += MED_HPF_SetPara(     pstPP->pstHpfAecRc,
                                  pstNv->ashwHpfTx,
                                  CODEC_NV_HPF_TX_LEN);

    /* 下行高通滤波参数加载 */
    uwRet += MED_HPF_SetPara(     pstPP->pstHpfRx,
                                  pstNv->ashwHpfRx,
                                  CODEC_NV_HPF_RX_LEN);

    /* 更新上行设备匹配增益值 */
    uwRet += MED_DEVGAIN_SetPara( pstPP->pstDevGainTx,
                                  &(pstNv->ashwDevGain[0]),
                                  1);

    /* 更新下行设备匹配增益值 */
    uwRet += MED_DEVGAIN_SetPara( pstPP->pstDevGainRx,
                                  &(pstNv->ashwDevGain[1]),
                                  1);

    /* 更新上行补偿滤波器系数 */
    uwRet += MED_COMP_SetPara((VOS_UINT32)enSampleRate,
                                          pstPP->pstCompRx,
                                          pstNv->ashwCompRx,
                                          CODEC_NV_COMP_RX_LEN);

    /* 更新下行补偿滤波器系数 */
    uwRet += MED_COMP_SetPara((VOS_UINT32)enSampleRate,
                                          pstPP->pstCompTx,
                                          pstNv->ashwCompTx,
                                          CODEC_NV_COMP_TX_LEN);

    /* 更新AEC参数 */
    uwRet += MED_AEC_SetPara(     pstPP->pstAec,
                                  pstNv->ashwAec,
                                  CODEC_NV_AEC_LEN,
                                  enSampleRate,
                                  enPpMicNum);

    /* 更新ANR参数 */
    uwRet += MED_EANR_1MIC_SetPara( pstPP->pstAnrTx,
                                   pstNv->ashwAnr1MicTx,
                                   CODEC_NV_EANR_1MIC_LEN,
                                   enSampleRate);

    uwRet += MED_EANR_1MIC_SetPara( pstPP->pstAnrRx,
                                   pstNv->ashwAnr1MicRx,
                                   CODEC_NV_EANR_1MIC_LEN,
                                   enSampleRate);

    uwRet += MED_ANR_2MIC_SetPara( pstPP->pstAnr2Mic,
                                   pstNv->ashwAnr2Mic,
                                   CODEC_NV_ANR_2MIC_LEN,
                                   enSampleRate);

    /* 更新上行AGC参数*/
    uwRet += MED_AGC_SetPara(     pstPP->pstAgcTx,
                                  pstNv->ashwAgcTx,
                                  CODEC_NV_AGC_TX_LEN);

    /* 更新下行AGC参数*/
    uwRet += MED_AGC_SetPara(     pstPP->pstAgcRx,
                                  pstNv->ashwAgcRx,
                                  CODEC_NV_AGC_RX_LEN);
    /* 更新MBDRC参数*/
    uwRet += MED_MBDRC_SetPara(   pstPP->pstMbdrc,
                                  pstNv->ashwMbdrc,
                                  CODEC_NV_MBDRC_LEN,
                                  enSampleRate);

    /* 更新上行AIG参数*/
    uwRet += MED_AIG_SetPara (    pstPP->pstAigTx,
                                  pstNv->ashwAigTx,
                                  CODEC_NV_AIG_TX_LEN,
                                  enSampleRate);

    /* 更新下行AIG参数*/
    uwRet += MED_AIG_SetPara (    pstPP->pstAigRx,
                                  pstNv->ashwAigRx,
                                  CODEC_NV_AIG_RX_LEN,
                                  enSampleRate);


    /* 更新AVC参数*/
    uwRet += MED_AVC_SetPara(     pstPP->pstAvc,
                                  pstNv->ashwAvc,
                                  CODEC_NV_AVC_LEN,
                                  enPpMicNum);

    /* 更新上行VAD参数*/
    uwRet += MED_VAD_SetPara(     pstPP->pstVadTx,
                                  pstNv->ashwVadTx,
                                  CODEC_NV_VAD_TX_LEN);

    /* 更新下行VAD参数*/
    uwRet += MED_VAD_SetPara(     pstPP->pstVadRx,
                                  pstNv->ashwVadRx,
                                  CODEC_NV_VAD_RX_LEN);

    /* 更新侧音增益参数*/
    uwRet += MED_SIDETONE_SetPara(pstPP->pstSTGain,
                                  pstNv->ashwSTGain,
                                  CODEC_NV_SIDEGAIN_LEN);

    return uwRet;

}


VOS_UINT32 MED_PpGetPara( VOICE_NV_STRU *pstNv)
{
    MED_PP_STRU                        *pstObjs     = MED_PP_GetObjPtr();
    VOS_UINT32                          swRet       = 0;

    /* 注: 上下行音量不再使用PP的NV项控制 */

    /* 读预加重滤波，写到pstNv所指内存中。因为主辅麦克共用预加重滤波NV参数，在此只用get主麦克*/
    swRet += MED_PREEMP_GetPara( pstObjs->pstPreEmpMc,
                                 pstNv->ashwPreEmp,
                                 CODEC_NV_PREEMP_LEN);

    /* 读去加重滤波，写到pstNv所指内存中 */
    swRet += MED_DEEMP_GetPara(  pstObjs->pstDeEmpMc,
                                 pstNv->ashwDeEmp,
                                 CODEC_NV_DEEMP_LEN);

    /* 读上行高通滤波参数，写到pstNv所指内存中 */
    swRet += MED_HPF_GetPara(    pstObjs->pstHpfTxMc,
                                 pstNv->ashwHpfTx,
                                 CODEC_NV_HPF_TX_LEN);

    /* 读下行高通滤波参数，写到pstNv所指内存中*/
    swRet += MED_HPF_GetPara(    pstObjs->pstHpfRx,
                                 pstNv->ashwHpfRx,
                                 CODEC_NV_HPF_RX_LEN);

    /* 读上行设备增益值，写到pstNv所指内存中 */
    swRet += MED_DEVGAIN_GetPara(pstObjs->pstDevGainTx,
                                 &(pstNv->ashwDevGain[0]),
                                 1);

    /* 读下行设备增益值，写到pstNv所指内存中 */
    swRet += MED_DEVGAIN_GetPara(pstObjs->pstDevGainRx,
                                 &(pstNv->ashwDevGain[1]),
                                 1);

    /* 读侧音增益值，写到pstNv所指内存中 */
    swRet += MED_SIDETONE_GetPara(pstObjs->pstSTGain,
                                 pstNv->ashwSTGain,
                                 CODEC_NV_SIDEGAIN_LEN);

    /* 读上行补偿滤波器系数，写到pstNv所指内存中 */
    swRet += MED_COMP_GetPara(   pstObjs->pstCompTx,
                                 pstNv->ashwCompTx,
                                 CODEC_NV_COMP_TX_LEN);

    /* 读下行补偿滤波器系数，写到pstNv所指内存中 */
    swRet += MED_COMP_GetPara(   pstObjs->pstCompRx,
                                 pstNv->ashwCompRx,
                                 CODEC_NV_COMP_RX_LEN);

    /* 读AEC参数，写到pstNv所指内存中 */
    swRet +=  MED_AEC_GetPara(   pstObjs->pstAec,
                                 pstNv->ashwAec,
                                 CODEC_NV_AEC_LEN);

    /* 读ANR参数，写到pstNv所指内存中 */
    swRet +=  MED_EANR_1MIC_GetPara(   pstObjs->pstAnrTx,
                                      pstNv->ashwAnr1MicTx,
                                      CODEC_NV_EANR_1MIC_LEN);

    swRet +=  MED_EANR_1MIC_GetPara(   pstObjs->pstAnrRx,
                                      pstNv->ashwAnr1MicRx,
                                      CODEC_NV_EANR_1MIC_LEN);


    swRet +=  MED_ANR_2MIC_GetPara(   pstObjs->pstAnr2Mic,
                                      pstNv->ashwAnr2Mic,
                                      CODEC_NV_ANR_2MIC_LEN);

    /* 读上行AGC参数，写到pstNv所指内存中*/
    swRet +=  MED_AGC_GetPara(   pstObjs->pstAgcTx,
                                 pstNv->ashwAgcTx,
                                 CODEC_NV_AGC_TX_LEN);

    /* 读MBDRC参数，写到pstNv所指内存中*/
    swRet +=  MED_MBDRC_GetPara( pstObjs->pstMbdrc,
                                 pstNv->ashwMbdrc,
                                 CODEC_NV_MBDRC_LEN);

    /* 读上行AIG参数，写到pstNv所指内存中*/
    swRet +=  MED_AIG_GetPara( pstObjs->pstAigTx,
                               pstNv->ashwAigTx,
                               CODEC_NV_AIG_TX_LEN);

    /* 读下行AIG参数，写到pstNv所指内存中*/
    swRet +=  MED_AIG_GetPara( pstObjs->pstAigRx,
                               pstNv->ashwAigRx,
                               CODEC_NV_AIG_RX_LEN);

    /* 读下行AGC参数，写到pstNv所指内存中*/
    swRet +=  MED_AGC_GetPara(   pstObjs->pstAgcRx,
                                 pstNv->ashwAgcRx,
                                 CODEC_NV_AGC_RX_LEN);

    /* 读AVC参数，写到pstNv所指内存中*/
    swRet +=  MED_AVC_GetPara(   pstObjs->pstAvc,
                                 pstNv->ashwAvc,
                                 CODEC_NV_AVC_LEN);

    /* 读上行VAD参数，写到pstNv所指内存中*/
    swRet +=  MED_VAD_GetPara(   pstObjs->pstVadTx,
                                 pstNv->ashwVadTx,
                                 CODEC_NV_VAD_TX_LEN);

    /* 读下行VAD参数，写到pstNv所指内存中*/
    swRet +=  MED_VAD_GetPara(   pstObjs->pstVadRx,
                                 pstNv->ashwVadRx,
                                 CODEC_NV_VAD_RX_LEN);

    return swRet;

}


VOS_UINT32 MED_TxPp(
                VOS_INT16                         *pshwMcIn,
                VOS_INT16                         *pshwRcIn,
                VOS_INT16                         *pshwLineOut,
                VOS_INT16                         *pshwEcRef)
{
    MED_PP_STRU        *pstObjs     = MED_PP_GetObjPtr();
    VOS_UINT32          uwRet;

    /* 更新远端数据 */
    MED_AEC_UpdateRx(pstObjs->pstAec, pshwEcRef);

    /* 根据单双麦克使能标志分别调用*/
    if (MED_PP_MIC_NUM_1 == pstObjs->enMicNum)
    {
        /*  单麦克上行语音前处理 */
        uwRet = MED_PpTx_1Mic(pshwMcIn, pshwLineOut);

    }
    else
    {
        /* 双麦克上行语音前处理 */
        uwRet = MED_PpTx_2Mic(pshwMcIn, pshwRcIn, pshwLineOut);
    }

    /* 更新滤波器系数 */
    MED_AEC_UpdateTx();

    return uwRet;

}
VOS_UINT32 MED_PpTx_1Mic(
                VOS_INT16                         *pshwMicIn,
                VOS_INT16                         *pshwLineOut)
{
    MED_PP_STRU                        *pstObjs     = MED_PP_GetObjPtr();
    CODEC_VAD_FLAG_ENUM_INT16           enVadFlag;
    VOS_UINT32                          uwRet = 0;
    VOS_INT16                           shwFrameLength =  MED_PP_GetFrameLength();

    /* 入参判断 */
    if (   (MED_NULL == pshwMicIn)
        || (MED_NULL == pshwLineOut))
    {
        return UCOM_RET_NUL_PTR;
    }

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_TxPp_1MIC);

    /* 1. VAD */
    uwRet += MED_VAD_Main(pstObjs->pstVadTx, pshwMicIn, &enVadFlag);

    /* 2. Tx 匹配增益: shwMicIn -> shwMicIn */
    uwRet += MED_DEVGAIN_Main(pstObjs->pstDevGainTx, pshwMicIn, pshwMicIn);

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_HPF_Main_TX);

    /* 3. Tx 高通滤波 */
    uwRet += MED_HPF_Main(pstObjs->pstHpfTxMc, pshwMicIn, pshwMicIn);

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_HPF_Main_TX);

    /* 4. 更新侧音 */
    uwRet += MED_SIDESTONE_UpdateTx(pstObjs->pstSTGain, pshwMicIn);

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_PREEMP_Main_TX);

    /* 5. 近端信号预加重滤波 */
    uwRet += MED_PREEMP_Main(pstObjs->pstPreEmpMc, pshwMicIn, pshwMicIn);

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_PREEMP_Main_TX);

    /* 钩取AEC的输入 */
    VOICE_DbgSendHook(VOICE_HOOK_AECIN_PCM, pshwMicIn, (VOS_UINT16)shwFrameLength);

    /* 6. EC和NS联合处理 */
    uwRet += MED_1MIC_AECANR_Main(pshwMicIn,
                                  pshwLineOut,
                                  enVadFlag,
                                  pstObjs->pstPreEmpFar,
                                  pstObjs->pstHpfAecMc,
                                  pstObjs->pstAvc);

    /* 7. AGC处理 */
    uwRet += MED_AGC_Main(pstObjs->pstAgcTx, pshwLineOut, enVadFlag, pshwLineOut);

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_AIG_Main_TX);

    /* 8. AIG处理 */
    uwRet += MED_AIG_Main(pstObjs->pstAigTx, pshwLineOut, pshwLineOut, pshwLineOut, enVadFlag);

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_AIG_Main_TX);

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_DEEMP_Main_TX);

    /* 9 去加重 */
    uwRet += MED_DEEMP_Main(pstObjs->pstDeEmpMc, pshwLineOut, pshwLineOut);

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_DEEMP_Main_TX);

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_COMP_Main_TX);

    /* 10. Tx 补偿滤波 */
    uwRet += MED_COMP_Main(pstObjs->pstCompTx, pshwLineOut, pshwLineOut);

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_COMP_Main_TX);

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_TxPp_1MIC);

    if (uwRet > 0)
    {
        return UCOM_RET_ERR_STATE;
    }

    return UCOM_RET_SUCC;
}


VOS_UINT32 MED_PpTx_2Mic(
                VOS_INT16                         *pshwMcIn,
                VOS_INT16                         *pshwRcIn,
                VOS_INT16                         *pshwLineOut)
{
    MED_PP_STRU                        *pstObjs     = MED_PP_GetObjPtr();
    CODEC_VAD_FLAG_ENUM_INT16           enVadFlag;
    VOS_UINT32                          uwRet       = 0;
    VOS_INT16                           shwFrameLength =  MED_PP_GetFrameLength();

    /* 入参判断 */
    if (   (MED_NULL == pshwMcIn)
        || (MED_NULL == pshwRcIn)
        || (MED_NULL == pshwLineOut))
    {
        return UCOM_RET_NUL_PTR;
    }

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_TxPp_2MIC);

    /* 1. VAD */
    uwRet += MED_VAD_Main(pstObjs->pstVadTx, pshwMcIn, &enVadFlag);

    /* 2. Tx 主辅麦克匹配增益: shwMcIn -> shwMcIn；shwRcIn -> shwRcIn */
    uwRet += MED_DEVGAIN_Main(pstObjs->pstDevGainTx, pshwMcIn, pshwMcIn);
    uwRet += MED_DEVGAIN_Main(pstObjs->pstDevGainTx, pshwRcIn, pshwRcIn);

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_HPF_Main_TX);

    /* 3. Tx 主辅麦克高通滤波 */
    uwRet += MED_HPF_Main(pstObjs->pstHpfTxMc, pshwMcIn, pshwMcIn);

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_HPF_Main_TX);

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_HPF_Main_TX);

    uwRet += MED_HPF_Main(pstObjs->pstHpfTxRc, pshwRcIn, pshwRcIn);

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_HPF_Main_TX);

    /* 4. 更新侧音 */
    uwRet += MED_SIDESTONE_UpdateTx(pstObjs->pstSTGain, pshwMcIn);

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_PREEMP_Main_TX);

    /* 5. 主辅麦克信号预加重滤波 */
    uwRet += MED_PREEMP_Main(pstObjs->pstPreEmpMc, pshwMcIn, pshwMcIn);

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_PREEMP_Main_TX);

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_PREEMP_Main_TX);

    uwRet += MED_PREEMP_Main(pstObjs->pstPreEmpRc, pshwRcIn, pshwRcIn);

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_PREEMP_Main_TX);

    /* 钩取AEC的输入 */
    VOICE_DbgSendHook(VOICE_HOOK_AECIN_PCM, pshwMcIn, (VOS_UINT16)shwFrameLength);

    /* 6. EC和NS联合处理 */
    uwRet += MED_2MIC_AECANR_Main(pshwMcIn,
                                  pshwRcIn,
                                  pshwLineOut,
                                  enVadFlag,
                                  pstObjs->pstPreEmpFar,
                                  pstObjs->pstHpfAecMc,
                                  pstObjs->pstHpfAecRc,
                                  pstObjs->pstAvc);

    /* 7. AGC处理 */
    uwRet += MED_AGC_Main(pstObjs->pstAgcTx, pshwLineOut, enVadFlag, pshwLineOut);

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_AIG_Main_TX);

    /* 8. AIG处理 */
    uwRet += MED_AIG_Main(pstObjs->pstAigTx, pshwLineOut, pshwLineOut, pshwLineOut, enVadFlag);

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_AIG_Main_TX);

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_DEEMP_Main_TX);

    /* 9 去加重 */
    uwRet += MED_DEEMP_Main(pstObjs->pstDeEmpMc, pshwLineOut, pshwLineOut);

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_DEEMP_Main_TX);

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_COMP_Main_TX);

    /* 10. Tx 补偿滤波 */
    uwRet += MED_COMP_Main(pstObjs->pstCompTx, pshwLineOut, pshwLineOut);

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_COMP_Main_TX);

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_TxPp_2MIC);

    if (uwRet > 0)
    {
        return UCOM_RET_ERR_STATE;
    }

    return UCOM_RET_SUCC;
}


VOS_UINT32 MED_RxPp( VOS_INT16                         *pshwLineIn)
{
    MED_PP_STRU                        *pstObjs        = MED_PP_GetObjPtr();
    MED_EANR_STRU                      *pstMedAnr;
    CODEC_VAD_FLAG_ENUM_INT16           enVadFlag;
    VOS_UINT32                          uwRet          = 0;
    VOS_INT16                          *pshwAigRef;
    VOS_INT16                           shwDeviceGain;
    VOS_INT16                           shwFrameLen    = MED_PP_GetFrameLength();

    /* 入参判断 */
    if (   (MED_NULL == pshwLineIn)
        || (MED_NULL == pstObjs))
    {
        return UCOM_RET_NUL_PTR;
    }

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_RxPp);

    pstMedAnr = (MED_EANR_STRU *)pstObjs->pstAnrRx;

    /* 帧数累加 */
    pstObjs->uhwFrameId = pstObjs->uhwFrameId + 1;

    /* 分配内存 */
    pshwAigRef = (VOS_INT16 *)UCOM_MemAlloc((VOS_UINT16)shwFrameLen * sizeof(VOS_INT16));

    /* 2. VAD */
    uwRet += MED_VAD_Main(pstObjs->pstVadRx, pshwLineIn, &enVadFlag);

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_HPF_Main_RX);

    /* 3. 高通滤波器 */
    uwRet += MED_HPF_Main(pstObjs->pstHpfRx, pshwLineIn, pshwLineIn);

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_HPF_Main_RX);

    /* AIG 数据缓存 */
    CODEC_OpVecCpy(pshwAigRef, pshwLineIn, shwFrameLen);

    /* 4. 下行ANR、MBDRC */
    uwRet += MED_ANR_MBDRC_Rx_Main(pstObjs->pstAnrRx,
                                   pstObjs->pstMbdrc,
                                   pshwLineIn,
                                   pshwLineIn,
                                   enVadFlag,
                                   (VOS_UINT16)(pstMedAnr->shwEnable));

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_AIG_Main_RX);

    /* 5. AIG */
    uwRet += MED_AIG_Main(pstObjs->pstAigRx, pshwLineIn, pshwAigRef, pshwLineIn, enVadFlag);

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_AIG_Main_RX);

    /* 6. AGC前下行底噪声清零 */
    uwRet += MED_AGC_PreClearNoise(pstObjs->pstAgcRx, pshwLineIn, pshwLineIn);

    /* 7. AGC */
    uwRet += MED_AGC_Main(pstObjs->pstAgcRx, pshwLineIn, enVadFlag, pshwLineIn);

    OM_CPULOAD_FuncEnter(OM_CPULOAD_PERFORMANCE_COMP_Main_RX);

    /* 8. Rx 补偿滤波 */
    uwRet += MED_COMP_Main(pstObjs->pstCompRx, pshwLineIn, pshwLineIn);

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_COMP_Main_RX);

    /* 9. AVC */
    (VOS_VOID)MED_DEVGAIN_GetPara(pstObjs->pstDevGainRx, &shwDeviceGain, 1);
    uwRet += MED_AVC_Main(pstObjs->pstAvc, pshwLineIn, shwDeviceGain);

    /* 10. 侧音叠加, 叠加上一帧的上行数据 */
    uwRet += MED_SIDETONE_Main(pstObjs->pstSTGain, pshwLineIn, pshwLineIn);

    /* 11. 匹配增益, shwSpkOut -> shwSpkOut */
    uwRet += MED_DEVGAIN_Main(pstObjs->pstDevGainRx, pshwLineIn, pshwLineIn);

    /* 释放内存 */
    UCOM_MemFree(pshwAigRef);

    OM_CPULOAD_FuncExit(OM_CPULOAD_PERFORMANCE_RxPp);

    if (uwRet > 0)
    {
        return UCOM_RET_ERR_STATE;
    }

    return UCOM_RET_SUCC;
}
VOS_UINT32 MED_PpDestroy(VOS_VOID)
{
    MED_PP_STRU              *pstObjs     = MED_PP_GetObjPtr();
    VOS_UINT32                uwRet = 0;

    /* 释放上行主辅mic信号预加重滤波实体，并更新g_stMedCtrlProcObjs */
    uwRet += MED_PREEMP_Destroy(&(pstObjs->pstPreEmpMc));
    uwRet += MED_PREEMP_Destroy(&(pstObjs->pstPreEmpRc));

    /* 释放上行far_end信号预加重滤波实体，并更新g_stMedCtrlProcObjs */
    uwRet += MED_PREEMP_Destroy(&(pstObjs->pstPreEmpFar));

    /* 释放上行mic信号去加重滤波实体，并更新g_stMedCtrlProcObjs */
    uwRet += MED_DEEMP_Destroy(&(pstObjs->pstDeEmpMc));

    /* 释放上行高通滤波实体，并更新g_stMedCtrlProcObjs */
    uwRet += MED_HPF_Destroy(&(pstObjs->pstHpfTxMc));
    uwRet += MED_HPF_Destroy(&(pstObjs->pstHpfTxRc));
    uwRet += MED_HPF_Destroy(&(pstObjs->pstHpfAecMc));
    uwRet += MED_HPF_Destroy(&(pstObjs->pstHpfAecRc));

    /* 释放下行高通滤波实体，并更新g_stMedCtrlProcObjs */
    uwRet += MED_HPF_Destroy(&(pstObjs->pstHpfRx));

    /* 释放上行增益值实体，并更新g_stMedCtrlProcObjs */
    uwRet += MED_DEVGAIN_Destroy(&(pstObjs->pstDevGainTx));

    /* 释放下行增益值实体，并更新g_stMedCtrlProcObjs */
    uwRet += MED_DEVGAIN_Destroy(&(pstObjs->pstDevGainRx));

    /* 释放侧音增益实体，并更新g_stMedCtrlProcObjs */
    uwRet += MED_SIDETONE_Destroy(&(pstObjs->pstSTGain));

    /* 释放上行补偿滤波器实体，并更新g_stMedCtrlProcObjs */
    uwRet += MED_COMP_Destroy(&(pstObjs->pstCompTx));

    /* 释放下行补偿滤波器实体，并更新g_stMedCtrlProcObjs */
    uwRet += MED_COMP_Destroy(&(pstObjs->pstCompRx));

    /* 释放AEC实体，并更新g_stMedCtrlProcObjs */
    uwRet += MED_AEC_Destroy(&(pstObjs->pstAec));

    /* 释放ANR实体，并更新g_stMedCtrlProcObjs */
    uwRet += MED_EANR_1MIC_Destroy(&(pstObjs->pstAnrTx));
    uwRet += MED_EANR_1MIC_Destroy(&(pstObjs->pstAnrRx));
    uwRet += MED_ANR_2MIC_Destroy(&(pstObjs->pstAnr2Mic));

    /* 释放上行AGC实体，并更新g_stMedCtrlProcObjs */
    uwRet += MED_AGC_Destroy(&(pstObjs->pstAgcTx));

    /* 释放下行AGC实体，并更新g_stMedCtrlProcObjs */
    uwRet += MED_AGC_Destroy(&(pstObjs->pstAgcRx));

    /* 释放MBDRC实体，并更新g_stMedCtrlProcObjs */
    uwRet += MED_MBDRC_Destroy(&(pstObjs->pstMbdrc));

    /* 释放上行AIG实体，并更新g_stMedCtrlProcObjs */
    uwRet += MED_AIG_Destroy(&(pstObjs->pstAigTx));

    /* 释放下行AIG实体，并更新g_stMedCtrlProcObjs */
    uwRet += MED_AIG_Destroy(&(pstObjs->pstAigRx));

    /* 释放AVC实体，并更新g_stMedCtrlProcObjs */
    uwRet += MED_AVC_Destroy(&(pstObjs->pstAvc));

    /* 释放上行VAD实体，并更新g_stMedCtrlProcObjs */
    uwRet += MED_VAD_Destroy(&(pstObjs->pstVadTx));

    /* 释放下行VAD实体，并更新g_stMedCtrlProcObjs */
    uwRet += MED_VAD_Destroy(&(pstObjs->pstVadRx));

    return uwRet;

}


VOS_INT16 AUDIO_PP_Init()
 {
    VOS_INT16                           shwFlag;
    AUDIO_PP_STRU                      *pstPP = AUDIO_PP_GetObjPtr();

    /* 创建上行左声道补偿滤波器实体 */
    pstPP->pstCompTxL    = MED_COMP_Create();

    /* 创建上行右声道补偿滤波器实体 */
    pstPP->pstCompTxR     = MED_COMP_Create();

    /* 创建下行左声道补偿滤波器实体 */
    pstPP->pstCompRxL     = MED_COMP_Create();

    /* 创建下行右声道补偿滤波器实体 */
    pstPP->pstCompRxR     = MED_COMP_Create();

    /* 创建左声道MBDRC实体 */
    pstPP->pstMbdrcL      = MED_MBDRC_Create();

    /* 创建右声道MBDRC实体 */
    pstPP->pstMbdrcR      = MED_MBDRC_Create();

    shwFlag =
        (  (VOS_NULL == pstPP->pstCompTxL)
        || (VOS_NULL == pstPP->pstCompTxR)
        || (VOS_NULL == pstPP->pstCompRxL)
        || (VOS_NULL == pstPP->pstCompRxR)
        || (VOS_NULL == pstPP->pstMbdrcL)
        || (VOS_NULL == pstPP->pstMbdrcR));

    return shwFlag;

}

VOS_UINT32 AUDIO_PpSetPara(
                AUDIO_NV_STRU             *pstNv,
                VOS_UINT32                 uwSampleRate)
{
    VOS_UINT32                uwRet      = UCOM_RET_SUCC;
    AUDIO_PP_STRU            *pstPp      = AUDIO_PP_GetObjPtr();

    /* 入参检查 */
    if (VOS_NULL == pstNv)
    {
        return UCOM_RET_NUL_PTR;
    }

    /* 更新上行左声道补偿滤波器系数 */
    uwRet += MED_COMP_SetPara(  uwSampleRate,
                                pstPp->pstCompTxL,
                                pstNv->ashwCompTx,
                                AUDIO_NV_COMP_TX_LEN);

    /* 更新上行右声道补偿滤波器系数 */
    uwRet += MED_COMP_SetPara(  uwSampleRate,
                                pstPp->pstCompTxR,
                                pstNv->ashwCompTx,
                                AUDIO_NV_COMP_TX_LEN);

    /* 更新下行左声道补偿滤波器系数 */
    uwRet += MED_COMP_SetPara(  uwSampleRate,
                                pstPp->pstCompRxL,
                                pstNv->ashwCompRx,
                                AUDIO_NV_COMP_RX_LEN);

    /* 更新下行右声道补偿滤波器系数 */
    uwRet += MED_COMP_SetPara(  uwSampleRate,
                                pstPp->pstCompRxR,
                                pstNv->ashwCompRx,
                                AUDIO_NV_COMP_RX_LEN);

    /* 设置音频左声道MBDRC系数 */
    uwRet += MED_MBDRC_SetPara( pstPp->pstMbdrcL,
                                pstNv->ashwMbdrc,
                                AUDIO_NV_MBDRC_LEN,
                                (VOS_INT32)uwSampleRate);

    /* 设置音频右声道MBDRC系数 */
    uwRet += MED_MBDRC_SetPara( pstPp->pstMbdrcR,
                                pstNv->ashwMbdrc,
                                AUDIO_NV_MBDRC_LEN,
                                (VOS_INT32)uwSampleRate);

    return uwRet;

}
VOS_UINT32 AUDIO_PpGetPara(AUDIO_NV_STRU   *pstNv)
{
    AUDIO_PP_STRU            *pstPp     = AUDIO_PP_GetObjPtr();
    VOS_UINT32                uwRet     = UCOM_RET_SUCC;

    /* 读上行左声道补偿滤波器系数，写到pstNv所指内存中 */
    uwRet += MED_COMP_GetPara(   pstPp->pstCompTxL,
                                 pstNv->ashwCompTx,
                                 AUDIO_NV_COMP_TX_LEN);

    /* 读上行右声道补偿滤波器系数，写到pstNv所指内存中 */
    uwRet += MED_COMP_GetPara(   pstPp->pstCompTxR,
                                 pstNv->ashwCompTx,
                                 AUDIO_NV_COMP_TX_LEN);

    /* 读下行左声道补偿滤波器系数，写到pstNv所指内存中 */
    uwRet += MED_COMP_GetPara(   pstPp->pstCompRxL,
                                 pstNv->ashwCompRx,
                                 AUDIO_NV_COMP_RX_LEN);

    /* 读下行右声道补偿滤波器系数，写到pstNv所指内存中 */
    uwRet += MED_COMP_GetPara(   pstPp->pstCompRxR,
                                 pstNv->ashwCompRx,
                                 AUDIO_NV_COMP_RX_LEN);

    /* 读左声道MBDRC参数，写到pstNv所指内存中*/
    uwRet += MED_MBDRC_GetPara(  pstPp->pstMbdrcL,
                                 pstNv->ashwMbdrc,
                                 AUDIO_NV_MBDRC_LEN);

    /* 读右声道MBDRC参数，写到pstNv所指内存中*/
    uwRet += MED_MBDRC_GetPara(  pstPp->pstMbdrcR,
                                 pstNv->ashwMbdrc,
                                 AUDIO_NV_MBDRC_LEN);

    return uwRet;

}


VOS_UINT32 AUDIO_ProcPpTx(
                VOS_INT16                         *pshwMicInL,
                VOS_INT16                         *pshwMicInR,
                VOS_INT16                         *pshwLineOutL,
                VOS_INT16                         *pshwLineOutR,
                AUDIO_PP_CHANNEL_ENUM_UINT16       uhwChannel)
{
    AUDIO_PP_STRU                      *pstObjs     = AUDIO_PP_GetObjPtr();
    VOS_UINT32                          uwRet       = UCOM_RET_SUCC;

    /* 入参判断 */
    if (  ((VOS_NULL == pshwMicInL) && (VOS_NULL == pshwMicInR))
        ||((VOS_NULL == pshwLineOutL) && (VOS_NULL == pshwLineOutR))
        ||(VOS_NULL == pstObjs))
    {
        return UCOM_RET_NUL_PTR;
    }

    switch (uhwChannel)
    {
        /* Tx 左声道补偿滤波 */
        case AUDIO_PP_CHANNEL_LEFT  :
        {
            uwRet = MED_COMP_Main(pstObjs->pstCompTxL, pshwMicInL, pshwLineOutL);
            break;
        }

        /* Tx 右声道补偿滤波 */
        case AUDIO_PP_CHANNEL_RIGHT :
        {
            uwRet = MED_COMP_Main(pstObjs->pstCompTxR, pshwMicInR, pshwLineOutR);
            break;
        }

        /* Tx 双声道补偿滤波 */
        case AUDIO_PP_CHANNEL_ALL   :
        {
            uwRet  = MED_COMP_Main(pstObjs->pstCompTxL, pshwMicInL, pshwLineOutL);
            uwRet += MED_COMP_Main(pstObjs->pstCompTxR, pshwMicInR, pshwLineOutR);
            break;
        }

        default                     :
        {
            return UCOM_RET_ERR_STATE;
        }
    }

    return uwRet;
}


VOS_UINT32 AUDIO_ProcPpRx(
                VOS_INT16                         *pshwLineInL,
                VOS_INT16                         *pshwLineInR,
                VOS_INT16                         *pshwLineOutL,
                VOS_INT16                         *pshwLineOutR,
                AUDIO_PP_CHANNEL_ENUM_UINT16       uhwChannel)
{
    AUDIO_PP_STRU                      *pstObjs     = AUDIO_PP_GetObjPtr();
    VOS_UINT32                          uwRet       = UCOM_RET_SUCC;

    /* 入参判断 */
    if (  ((VOS_NULL == pshwLineInL) && (VOS_NULL == pshwLineInR))
        ||((VOS_NULL == pshwLineOutL) && (VOS_NULL == pshwLineOutR))
        ||(VOS_NULL == pstObjs))
    {
        return UCOM_RET_NUL_PTR;
    }

    switch (uhwChannel)
    {
        /* Rx 左声道补偿滤波和MBDRC处理*/
        case AUDIO_PP_CHANNEL_LEFT :
        {
            uwRet += MED_COMP_Main(pstObjs->pstCompRxL, pshwLineInL, pshwLineOutL);

            uwRet += MED_ANR_MBDRC_Rx_Main(VOS_NULL,
                                           pstObjs->pstMbdrcL,
                                           pshwLineOutL,
                                           pshwLineOutL,
                                           CODEC_VAD_SPEECH,
                                           CODEC_SWITCH_OFF);
            break;
        }

        /* Rx 右声道补偿滤波和MBDRC处理*/
        case AUDIO_PP_CHANNEL_RIGHT :
        {
            uwRet += MED_COMP_Main(pstObjs->pstCompRxR, pshwLineInR, pshwLineOutR);

            uwRet += MED_ANR_MBDRC_Rx_Main(VOS_NULL,
                                           pstObjs->pstMbdrcR,
                                           pshwLineOutR,
                                           pshwLineOutR,
                                           CODEC_VAD_SPEECH,
                                           CODEC_SWITCH_OFF);
            break;
        }

        /* Rx 双声道补偿滤波和MBDRC处理*/
        case AUDIO_PP_CHANNEL_ALL :
        {
            uwRet += MED_COMP_Main(pstObjs->pstCompRxL, pshwLineInL, pshwLineOutL);

            uwRet += MED_ANR_MBDRC_Rx_Main(VOS_NULL,
                                           pstObjs->pstMbdrcL,
                                           pshwLineOutL,
                                           pshwLineOutL,
                                           CODEC_VAD_SPEECH,
                                           CODEC_SWITCH_OFF);

            uwRet += MED_COMP_Main(pstObjs->pstCompRxR, pshwLineInR, pshwLineOutR);

            uwRet += MED_ANR_MBDRC_Rx_Main(VOS_NULL,
                                           pstObjs->pstMbdrcR,
                                           pshwLineOutR,
                                           pshwLineOutR,
                                           CODEC_VAD_SPEECH,
                                           CODEC_SWITCH_OFF);
            break;
        }

        default :
        {
            return UCOM_RET_ERR_STATE;
        }
    }
    if (uwRet > 0)
        {
            return UCOM_RET_ERR_STATE;
        }

    return UCOM_RET_SUCC;
}

VOS_UINT32 AUDIO_PpDestroy(VOS_VOID)
{
    AUDIO_PP_STRU              *pstObjs     = AUDIO_PP_GetObjPtr();
    VOS_UINT32                  uwRet       = UCOM_RET_SUCC;

    /* 释放上行左声道补偿滤波器实体 */
    uwRet += MED_COMP_Destroy(&(pstObjs->pstCompTxL));

    /* 释放上行右声道补偿滤波器实体 */
    uwRet += MED_COMP_Destroy(&(pstObjs->pstCompTxR));

    /* 释放下行左声道补偿滤波器实体 */
    uwRet += MED_COMP_Destroy(&(pstObjs->pstCompRxL));

    /* 释放下行右声道补偿滤波器实体 */
    uwRet += MED_COMP_Destroy(&(pstObjs->pstCompRxR));

    /* 释放左声道MBDRC实体 */
    uwRet += MED_MBDRC_Destroy(&(pstObjs->pstMbdrcL));

    /* 释放右声道MBDRC实体 */
    uwRet += MED_MBDRC_Destroy(&(pstObjs->pstMbdrcR));

    return uwRet;

}

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

