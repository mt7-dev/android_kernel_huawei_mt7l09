

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "codec_typedefine.h"
#include "med_aec_comm.h"
#include "med_aec_af.h"
#include "med_aec_hf_af.h"
#include "med_aec_dtd.h"
#include "med_aec_nlp.h"
#include "med_aec_hf_nlp.h"

#ifndef _MED_AEC_MAIN_H_
#define _MED_AEC_MAIN_H_


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* AEC内部状态结构 */
typedef struct
{
    MED_OBJ_HEADER
    CODEC_SWITCH_ENUM_UINT16            shwIsEnable;
    CODEC_SWITCH_ENUM_UINT16            shwIsEAecEnable;
    MED_AEC_OFFSET_OBJ_STRU             stDelay;
    MED_AEC_AF_STRU                     stMcAf;
    MED_AEC_AF_STRU                     stRcAf;
    MED_AEC_DTD_STRU                    stDtd;
    MED_AEC_NLP_STRU                    stNlp;
    MED_AEC_TRANS_STRU                  stTrans;
    MED_AEC_HF_AF_STRU                  stEAecHfAf;
    MED_AEC_HF_NLP_STRU                 stEAecHfNlp;

} MED_AEC_OBJ_STRU;

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define MED_AEC_GetObjPtr()              (&g_astMedAecObjPool[0])
#define MED_AEC_GetMcBinPtr()            (&g_shwAecMcBin[0])
#define MED_AEC_GetMcErrNLPBinNormPtr()  (&g_shwAecMcErrNLPBinNorm[0])
#define MED_AEC_GetMcErrAFBinNormPtr()   (&g_shwAecMcErrAFBinNorm[0])
#define MED_AEC_GetObjInfoPtr()          (&g_stMedAecObjInfo)
#define MED_AEC_MAX_OBJ_NUM              (1)

#define MED_AEC_NV_LEN                   (41)                                   /*AEC NV项元素个数 */
/*****************************************************************************
  3 枚举定义
*****************************************************************************/

/*****************************************************************************
  4 全局变量声明
*****************************************************************************/
extern MED_AEC_OBJ_STRU                 g_astMedAecObjPool[1];
extern VOS_INT16                        g_shwAecMcBin[MED_AEC_NLP_MAX_FFT_LEN];
extern VOS_INT16                        g_shwAecMcErrBinNorm[MED_AEC_NLP_MAX_FFT_LEN];

/*****************************************************************************
  5 STRUCT定义
*****************************************************************************/


/*****************************************************************************
  6 UNION定义
*****************************************************************************/

/*****************************************************************************
  7 OTHERS定义
*****************************************************************************/

/*****************************************************************************
  8 函数声明
*****************************************************************************/
extern VOS_UINT32  MED_1MIC_AECANR_Main(
                       VOS_INT16              *pshwMicIn,
                       VOS_INT16              *pshwLineOut,
                       VOS_INT16               enVadFlag,
                       VOS_VOID               *pstPreEmpFar,
                       VOS_VOID               *pstHpfAecMc,
                       VOS_VOID               *pstAvc);
extern VOS_UINT32 MED_2MIC_AECANR_Main(
                       VOS_INT16               *pshwMcIn,
                       VOS_INT16               *pshwRcIn,
                       VOS_INT16               *pshwLineOut,
                       VOS_INT16                enVadFlag,
                       VOS_VOID                *pstPreEmpFar,
                       VOS_VOID                *pstHpfAecMc,
                       VOS_VOID                *pstHpfAecRc,
                       VOS_VOID                *pstAvc );
extern VOS_VOID* MED_AEC_Create(VOS_VOID);
extern VOS_UINT32  MED_AEC_Destroy(VOS_VOID  **ppAecObj);
extern VOS_UINT32 MED_AEC_GetPara(
                       VOS_VOID         *pAecInstance,
                       VOS_INT16        *pshwAecParam,
                       VOS_INT16         shwParaLen);
extern VOS_UINT32 MED_AEC_SetPara (
                       VOS_VOID                        *pAecInstance,
                       VOS_INT16                       *pshwAecParam,
                       VOS_INT16                        shwParaLen,
                       CODEC_SAMPLE_RATE_MODE_ENUM_INT32  enSampleRate,
                       MED_PP_MIC_NUM_ENUM_UINT16       enPpMicNum);
extern VOS_UINT32  MED_AEC_UpdateRx(
                       VOS_VOID               *pvAecInstance,
                       VOS_INT16              *pshwLineIn);
extern VOS_VOID MED_AEC_UpdateTx(VOS_VOID);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of med_aec_main.h*/

