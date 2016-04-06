



/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "codec_op_etsi.h"
#include "codec_op_lib.h"
#include "codec_op_netsi.h"
#include "codec_com_codec.h"
#include "med_pp_comm.h"

#ifndef __MED_EANR_H__
#define __MED_EANR_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#define MED_EANR_GetObjPtr()               (&g_astMedEanrObj[0])                   /* 获取EANR全局变量 */
#define MED_EANR_GetObjInfoPtr()           (&g_stMedEanrObjInfo)                  /* 获取EANR全局对象信息 */

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define MED_EANR_MAX_OBJ_NUM                    (2)                             /*EANR实体个数*/
#define MED_EANR_1MIC_PARAM_LEN                 (3)                             /*EANR的NV有效参数个数*/
#define MED_EANR_MAXWINLEN                      (1024)                          /*STFT加窗长度，可根据FFT大小进行变化*/
#define MED_EANR_MAXBINNUM                      (41)                            /*最大的子带数，窄带33宽带41*/
#define MED_EANR_DATAWINLEN                     (328)                           /*更新功率谱最小值时需要保存的功率谱数据长度(8*41)*/
#define MED_EANR_DATAWINNUM                     (8)                             /*更新功率谱最小值时需要保存的功率谱帧数*/
#define MED_EANR_UPDATELEN                      (21)                            /*更新功率谱最小值时更新功率谱数据的时间间隔*/
#define MED_EANR_SNRMAPCOEFA                    (32603)                         /*(CODEC_OpSub(32767, 0.005))*/
#define MED_EANR_SNRMAPCOEFB                    (32685)                         /*(CODEC_OpAdd(0.005>>1, CODEC_OpSub(32767, 0.005)))*/
#define MED_EANR_ALPHA_COEF                     (27852)                         /*=27852(0.85->27852 Q15)，对应变量的平滑系数*/
#define MED_EANR_SPEECHDEC_THRESHOLDA           (31277)                         /*=31277 (4.6*Bmin = 7.636->31277   Q12)，对应变量的阈值*/
#define MED_EANR_SPEECHDEC_THRESHOLDB           (22710)                         /*=22710 (1.67*Bmin = 2.7722->22710 Q13)，对应变量的阈值*/
#define MED_EANR_SPEECHDEC_MIN_COMPENS          (27197)                         /*= 27197 (1.66->27197 Q14)，最小值补偿*/
#define MED_EANR_SPEECHPROB_STAT                (2)                             /*噪声稳定性参数，对于稳定噪声，参数选取影响不大*/
#define MED_EANR_SPEECHPROB_SNR_SMTH_COEF       (31130)                         /*=31130(0.95->31130 Q15)，先验信噪比的平滑系数*/
#define MED_EANR_SPEECHPROB_THRESHOLDC          (24576)                         /*=24576(3.0->24576 Q13)，变量计算公式中变量的经验值*/
#define MED_EANR_NOISESPECT_ALPHA_COEF          (27853)                         /*=27853(0.85->27853 Q15)，噪声估计中的平滑系数*/
#define MED_EANR_NOISESPECT_ALPHA_COEF_L        (32440)                         /*=32440(0.99->32440 Q15)，噪声长时平滑系数*/
#define MED_EANR_APSSMOOTH_ASNR_COEF            (22937)                         /*=22937(0.7->22937 Q15)，先验信噪比的长时平均*/
#define MED_EANR_PRO_BIN_IDXA                   (13)                            /*起始子带索引*/
#define MED_EANR_PRO_BIN_IDXB                   (31)                            /*终止子带索引*/
#define MED_EANR_PROABSENCE_BIN_IDXC            (2)                             /*先验信噪比均值计算起始子带索引*/
#define MED_EANR_PROABSENCE_SNR_H_LIMIT         (640)                           /*=640(10->640 Q6)，先验信噪比均值对数值的限幅上限*/
#define MED_EANR_PROABSENCE_SNR_L_LIMIT         (0)                             /*=0，先验信噪比均值对数值的限幅下限*/
#define MED_EANR_PROABSENCE_ABSENCE_PRO_MAX     (32702)                         /*=32702(0.998->32702 Q15)，语音不存在概率的极值*/
#define MED_EANR_PSTSNRMAX                      (522240)                        /*后验信噪比最大值限制(255->522240 Q11)*/
#define MED_EANR_MAX16                          (32767)                         /*MED_INT16所能表示的最大正数，多处用来表示1(Q15)*/
#define MED_EANR_MAX11                          (2047)                          /*12bit所能表示最大值，定点化过程中常数*/
#define MED_EANR_Q11                            (2048)                          /*2^11,定点化过程中常数*/
#define MED_EANR_Q7                             (128)                           /*2^7, 定点化过程中常数*/
#define MED_EANR_Q13                            (8192)                          /*2^13,定点化过程中常数*/
#define MED_EANR_Q14                            (16384)                         /*2^14,定点化过程中常数*/
#define MED_EANR_SMTHSTARTFRM                   (14)                            /*最小值搜索起始非零帧序号*/
#define MED_EANR_SNRFACT_MAX                    (32650)                         /*32650信噪比因子上限15.9424->32650 Q11, EXP(15.9424)<<8 = 2^31 */
#define MED_EANR_NOISESPECT_PROBTHD             (32750)                         /*噪声谱平滑时语音存在概率门限，超过此门限噪声不平滑，*/
#define MED_EANR_NOISESPECT_NOISECOMPENS        (24060)                         /*噪声无音补偿因子，1.4685->24060 Q14*/
#define MED_EANR_PRESENCEPROB_TH1               (13)                            /*0.1->13(Q7),Local先验信噪比门限低界，小于此门限Local有音概率设置为最小值*/
#define MED_EANR_PRESENCEPROB_TH2               (40)                            /*0.3162->40(Q7),Local先验信噪比门限高界，大于此门限Local有音概率设置为最大值*/
#define MED_EANR_PRESENCEPROB_FRMTH             (120)                           /*计算有音概率时的时间帧门限*/
#define MED_EANR_PRESENCEPROB_POWTH             (20480)                         /*2.5->20480(Q13)计算有音概率时的功率门限*/
#define MED_EANR_PROB_MIN                       (164)                           /*0.005->164(Q15)有音无音概率最小值*/
#define MED_EANR_ABSENCEPROB_SNRTH              (-640)                          /*计算无音概率时信噪比门限，-10 -> -640(Q6)*/
#define MED_EANR_ABSENCEPROB_SNRPENALTYA        (320)                           /*计算无音概率时信噪比补偿因子,5 ->320(Q6)*/
#define MED_EANR_ABSENCEPROB_SNRPENALTYB        (640)                           /*计算无音概率时信噪比补偿因子,10->640(Q6)*/
#define MED_EANR_ABSENCEPROB_DIVISOR            (5)                             /*计算无音概率时的除数*/
#define MED_EANR_GAIN_ABSENCEPROBTH             (29491)                         /*计算增益时无音概率门限，  0.9->29491(Q15)*/
#define MED_EANR_GAIN_SNRFACTORTH               (10240)                         /*计算增益时信噪比因子门限，5  ->10240(Q11)*/
#define MED_EANR_GAIN_SMOOTHCOEF                (6554)                          /*计算增益时功率谱平滑系数，0.2->6554(Q15)*/
#define MED_EANR_GAIN_UPLIMIT                   (4095)                          /*增益上界，1->4095(Q12)*/
#define MED_EANR_PRISNRMAXLIMIT                 (32640)                         /*先验信噪比上界，255->32640(Q7)*/
#define MED_EANR_MINPOWERTHD                    (16384)                         /*最小能量阈值*/
/*****************************************************************************
  3 枚举定义
*****************************************************************************/


/*****************************************************************************
  4 消息头定义
*****************************************************************************/


/*****************************************************************************
  5 消息定义
*****************************************************************************/


/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/
/*****************************************************************************
 实体名称  : MED_EANR_COMM_STRU
 功能描述  : 定点EANR公有结构体
*****************************************************************************/
typedef struct
{
    MED_INT32 swIdx;                                                            /*时间帧计数指示*/
    MED_INT16 shwZfNum;                                                         /*全零帧计数指示*/
    MED_INT16 shwIdxSwitch;                                                     /*多帧查找最小值存储数据更新指示*/
    MED_INT16 shwNormShift;                                                     /*时频域数据左移bit数*/
    MED_INT16 shwRev;
    MED_INT32 aswNoisePow[MED_EANR_MAXBINNUM];                                  /*噪声能量谱*/
    MED_INT32 aswNoisePowRud[MED_EANR_MAXBINNUM];                               /*未修正的噪声能量谱*/
    MED_INT32 aswPowSpctSmth[MED_EANR_MAXBINNUM];                               /*时频平滑后功率谱*/
    MED_INT32 aswPowSpctCond[MED_EANR_MAXBINNUM];                               /*粗略语音判决指示下平滑后的功率谱估计*/
    MED_INT32 aswPowSpctMin[MED_EANR_MAXBINNUM];                                /*时间窗内平滑后功率谱最小值*/
    MED_INT32 aswPowSpctMinC[MED_EANR_MAXBINNUM];                               /*时间窗内条件平滑后功率谱最小值*/
    MED_INT32 aswPowSpctMinCurt[MED_EANR_MAXBINNUM];                            /*当前平滑后功率谱最小值*/
    MED_INT32 aswPowSpctMinCurtC[MED_EANR_MAXBINNUM];                           /*当前条件平滑后功率谱最小值*/
    MED_INT16 ashwSNRSmth[MED_EANR_MAXBINNUM];                                  /*平滑后信噪比*/
    MED_INT16 ashwPresenIndSmth[MED_EANR_MAXBINNUM];                            /*平滑后语音存在指示*/
    MED_INT16 shwSubBandNum;                                                    /*子带数目*/
    MED_INT16 shwOpertBandNum;                                                  /*处理的子带数*/
}MED_EANR_COMM_STRU;

/*****************************************************************************
 实体名称  : MED_EANR_TFSMOOTH_STRU
 功能描述  : 定点MED_EANR_TFSmooth_FIX函数结构体
*****************************************************************************/
typedef struct
{
    MED_INT32 aswPowSpctSmth[MED_EANR_MAXBINNUM];                               /*时频平滑后功率谱*/
    MED_INT32 aswPowSpct[MED_EANR_MAXBINNUM];                                   /*原始功率谱*/
}MED_EANR_TFSMOOTH_STRU;

/*****************************************************************************
 实体名称  : MED_EANR_SPEECHDEC_STRU
 功能描述  : 定点MED_EANR_SpeechDec_FIX函数结构体
*****************************************************************************/
typedef struct
{
    MED_INT16 ashwSpechIdx[MED_EANR_MAXBINNUM];                                 /*初始的有音(1)和无音(0)指示*/
}MED_EANR_SPEECHDEC_STRU;

/*****************************************************************************
 实体名称  : MED_EANR_SPEECHPROB_STRU
 功能描述  : 定点MED_EANR_SpeechProb_FIX函数结构体
*****************************************************************************/
typedef struct
{
    MED_INT16 shwSNRMin;                                                        /*先验信噪比最小值*/
    MED_INT16 ashwPriSNR[MED_EANR_MAXBINNUM];                                   /*先验信噪比*/
    MED_INT16 ashwSpchProb[MED_EANR_MAXBINNUM];                                 /*有语音的条件概率*/
}MED_EANR_SPEECHPROB_STRU;

/*****************************************************************************
 实体名称  : MED_EANR_MINTRACKUPDATE_STRU
 功能描述  : 定点MED_EANR_MinTrackUpdate_FIX函数结构体
*****************************************************************************/
typedef struct
{
    MED_INT32 aswPowStck[MED_EANR_DATAWINLEN];                                  /*临时存储变量,用于存储功率谱*/
    MED_INT32 aswCondPowStck[MED_EANR_DATAWINLEN];                              /*临时存储变量,用于存储条件功率谱*/
}MED_EANR_MINTRACKUPDATE_STRU;

/*****************************************************************************
 实体名称  : MED_EANR_NOISESPECT_STRU
 功能描述  : 定点MED_EANR_NoiseSpect_FIX函数结构体
*****************************************************************************/
typedef struct
{
    MED_INT32 aswNoisePowLong[MED_EANR_MAXBINNUM];                              /*长时平滑的噪声功率谱*/
}MED_EANR_NOISESPECT_STRU;

/*****************************************************************************
 实体名称  : MED_EANR_SNRSMOOTH_STRU
 功能描述  : 定点MED_EANR_SNRSmooth_FIX函数结构体
*****************************************************************************/
typedef struct
{
    MED_INT16 ashwSNRSmthGlb[MED_EANR_MAXBINNUM];                               /*先验信噪比的Global平滑结果*/
    MED_INT16 ashwSNRSmthLcl[MED_EANR_MAXBINNUM];                               /*先验信噪比的Local平滑结果*/
}MED_EANR_SNRSMOOTH_STRU;

/*****************************************************************************
 实体名称  : MED_EANR_PRESENCEPROB_STRU
 功能描述  : 定点MED_EANR_PresenceProb_FIX函数结构体
*****************************************************************************/
typedef struct
{
    MED_INT16 ashwProbLcl[MED_EANR_MAXBINNUM];                                  /*Local有音概率*/
    MED_INT16 ashwProbGlb[MED_EANR_MAXBINNUM];                                  /*Global有音概率*/
}MED_EANR_PRESENCEPROB_STRU;

/*****************************************************************************
 实体名称  : MED_EANR_ABSENCEPROB_STRU
 功能描述  : 定点MED_EANR_AbsenceProb_FIX函数结构体
*****************************************************************************/
typedef struct
{
    MED_INT16 shwMeanPriSNR;                                                    /*先验信噪比的均值*/
    MED_INT16 shwPriSNR;                                                        /*先验信噪比均值对数值的限幅值*/
    MED_INT16 ashwAbsenProb[MED_EANR_MAXBINNUM];                                /*无音概率*/
}MED_EANR_ABSENCEPROB_STRU;

/*****************************************************************************
 实体名称  : MED_EANR_GAIN_STRU
 功能描述  : 定点MED_EANR_Gain_FIX函数结构体
*****************************************************************************/
typedef struct
{
    MED_INT16 ashwPriSNRMid[MED_EANR_MAXBINNUM];                                /*用于先验信噪比估计中间变量*/
    MED_INT16 shwSNRSqrtMin;                                                    /*最小增益*/
    MED_INT16 ashwGain[MED_EANR_MAXBINNUM];                                     /*增益*/
}MED_EANR_GAIN_STRU;


typedef struct
{
    MED_OBJ_HEADER
    MED_EANR_COMM_STRU                           stCommon;                      /*公用结构体*/
    MED_EANR_TFSMOOTH_STRU                       stTFSmooth;                    /*时频域平滑结构体*/
    MED_EANR_SPEECHDEC_STRU                      stSpeechDec;                   /*语音粗判结构体*/
    MED_EANR_SPEECHPROB_STRU                     stSpeechProb;                  /*粗略的语音存在概率计算结构体*/
    MED_EANR_MINTRACKUPDATE_STRU                 stMinTrackUpdate;              /*功率谱最小值搜索更新结构体*/
    MED_EANR_NOISESPECT_STRU                     stNoiseSpect;                  /*噪声功率计算结构体*/
    MED_EANR_SNRSMOOTH_STRU                      stSnrSmooth;                   /*先验信噪比计算结构体*/
    MED_EANR_PRESENCEPROB_STRU                   stPresenceProb;                /*语音存在概率计算结构体*/
    MED_EANR_ABSENCEPROB_STRU                    stAbsenceProb;                 /*语音不存在概率计算结构体*/
    MED_EANR_GAIN_STRU                           stGain;                        /*增益计算结构体*/
    MED_INT16                                    shwEnable;                                                        /*EANR使能控制，0为关闭，1为打开*/
    MED_INT16                                    shwReduceDB;
    MED_INT16                                    shwZeroFlagTh;
    MED_INT16                                    shwReserve;
}MED_EANR_STRU;


/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/
extern MED_EANR_STRU    g_astMedEanrObj[MED_EANR_MAX_OBJ_NUM];                  /* ANR模块全局控制实体 */
extern MED_OBJ_INFO     g_stMedEanrObjInfo;                                     /* ANR信息 */
extern MED_INT16        MED_EANR_SUBBAND_FL[40];                                /*子带下限索引*/
extern MED_INT16        MED_EANR_SUBBAND_FH[40];                                /*子带上限索引*/
extern MED_INT16        MED_EANR_SNR_WIN_GLB[31];                               /*先验信噪比Global平滑窗系数*/


/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern VOS_VOID* MED_EANR_1MIC_Create( VOS_VOID );
extern VOS_UINT32  MED_EANR_1MIC_Destroy(VOS_VOID  **ppInstance);
extern VOS_UINT32  MED_EANR_1MIC_GetPara(
                       VOS_VOID               *pInstance,
                       VOS_INT16              *pshwPara,
                       VOS_INT16               shwParaLen);
extern VOS_UINT32  MED_EANR_1MIC_Main(
                       VOS_VOID                    *pInstance,
                       VOS_INT16                   *pshwFrmFreq,
                       VOS_INT16                   *pshwFrmFreqRef,
                       VOS_INT16                    shwNormShift);
extern VOS_UINT32 MED_EANR_1MIC_SetPara(
                       VOS_VOID                          *pInstance,
                       VOS_INT16                         *pshwPara,
                       VOS_INT16                          shwParaLen,
                       CODEC_SAMPLE_RATE_MODE_ENUM_INT32  enSampleRate);
extern void MED_EANR_AbsenceProb_FIX(MED_EANR_COMM_STRU *stCommon,
                                     MED_EANR_ABSENCEPROB_STRU *stAbsenceProb,
                                     MED_INT16 *ashwProbLcl,
                                     MED_INT16 *ashwProbGlb);
extern void MED_EANR_CondiSmooth_FIX(MED_EANR_COMM_STRU *stCommon,
                                     MED_INT32 *aswPowSpctSmth,
                                     MED_INT32 *aswPowSpct,
                                     MED_INT16 *ashwSpechIdx);
extern void MED_EANR_ConvSmooth(MED_INT16 *ashwWin,
                                MED_INT16 *pshwIn,
                                MED_INT16 *pshwOut,
                                MED_INT16 shwBinNum);
extern void MED_EANR_DATA_FIX(MED_EANR_COMM_STRU *stCommon,
                              MED_INT16 *ashwGain,
                              MED_INT16 *shwDataBuf);
extern void MED_EANR_FastSmooth_16(MED_INT16 *pshwIn, MED_INT16 *pshwOut, MED_INT16 shwLen);
extern void MED_EANR_FastSmooth_32(MED_INT32 *pswIn, MED_INT32 *pswOut, MED_INT16 shwLen);
extern void MED_EANR_FastSmooth_Shl2(MED_INT16 *pshwIn, MED_INT16 *pshwOut, MED_INT16 shwLen);
extern void MED_EANR_Gain_FIX(MED_EANR_COMM_STRU *stCommon,
                              MED_EANR_GAIN_STRU *stGain,
                              MED_INT16 shwSNRMin,
                              MED_INT32 *aswPowSpct,
                              MED_INT16 *ashwAbsenProb);
extern void MED_EANR_MinTrackUpdate_FIX(MED_EANR_COMM_STRU *stCommon,
                                        MED_EANR_MINTRACKUPDATE_STRU *stMinTrackUpdate,
                                        MED_INT32 *aswPowSpctSmth);
extern void MED_EANR_MinTrack_FIX(MED_EANR_COMM_STRU *stCommon, MED_INT32 *aswPowSpctSmth);
extern void MED_EANR_NoiseSpect_FIX(MED_EANR_COMM_STRU *stCommon,
                                    MED_EANR_NOISESPECT_STRU *stNoiseSpect,
                                    MED_INT16 *ashwSpchProb,
                                    MED_INT32 *aswPowSpct);
extern void MED_EANR_PresenceProb_FIX(MED_EANR_COMM_STRU *stCommon,
                                      MED_EANR_PRESENCEPROB_STRU *stPresenceProb,
                                      MED_INT32 *aswNoisePowLong,
                                      MED_INT16 *ashwSNRSmthLcl,
                                      MED_INT16 *ashwSNRSmthGlb);
extern void MED_EANR_SNRSmooth_FIX(MED_EANR_COMM_STRU *stCommon,
                                   MED_EANR_SNRSMOOTH_STRU *stSnrSmooth,
                                   MED_INT16 *ashwPriSNR);
extern void MED_EANR_SpeechDec_FIX(MED_EANR_COMM_STRU *stCommon,
                                   MED_EANR_SPEECHDEC_STRU *stSpeechDec,
                                   MED_INT32*aswPowSpctSmth,
                                   MED_INT32 *aswPowSpct);
extern void MED_EANR_SpeechProb_FIX(MED_EANR_COMM_STRU *stCommon,
                                    MED_EANR_SPEECHPROB_STRU *stSpeechProb,
                                    MED_INT16 *ashwPriSNRMid,
                                    MED_INT32 *aswPowSpctSmth,
                                    MED_INT32 *aswPowSpct);
extern void MED_EANR_TFSmooth_FIX(MED_EANR_COMM_STRU *stCommon,
                                  MED_EANR_TFSMOOTH_STRU *stTFSmooth,
                                  MED_INT16 *shwDataBuf);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of med_eanr.h */
