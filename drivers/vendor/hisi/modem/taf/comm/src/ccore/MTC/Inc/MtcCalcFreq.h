

#ifndef __MTC_CALC_FREQ_H__
#define __MTC_CALC_FREQ_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include "PsTypeDef.h"

#if (FEATURE_ON == FEATURE_LTE)
#include "LNvCommon.h"
#endif

#include "MtcRrcInterface.h"
#include "MtcRfLcdIntrusion.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define DC_MIMO_REMOVEINTERFREQOFF                  ( 0xbfff )                  /* 去除偏移值 */
#define FREQ_BAND_NUM                               ( 10 )                               /* 目前支持的频带数目 */

#if (FEATURE_ON == FEATURE_LTE)
/* 检查是否为FDD的band ind */
#define MTC_COMM_CHECK_IS_FDD_BAND_IND(ucBandInd)\
    ((((MTC_FREQ_BAND1 <= ucBandInd) && (MTC_FREQ_BAND32 > ucBandInd)))? VOS_TRUE:VOS_FALSE)

/* 检查是否为TDD的band ind */
#define MTC_COMM_CHECK_IS_PROCOL_TDD_BAND_IND(ucBandInd)\
    (((MTC_FREQ_BAND33 <= ucBandInd) && (MTC_FREQ_BAND43 >= ucBandInd))? VOS_TRUE:VOS_FALSE)

/* 非标准频段 begin */
#define MTC_COMM_CHECK_IS_NONSTANDARD_BAND_IND(ucBandInd)\
    ((((LTE_COMM_NONSTANDARD_BAND_BEGIN  <= ucBandInd) && (LTE_COMM_NONSTANDARD_BAND_END >= ucBandInd)) )? VOS_TRUE:VOS_FALSE)
#endif

#define MTC_GAS_BAND_TYPE_UNKNOWN                      (0)                      /* 未知频段 */

#define MTC_GSM_BAND_TYPE_450                          (0x0001)                /* ARFCN所代表的频段:GSM 450 */
#define MTC_GSM_BAND_TYPE_480                          (0x0002)                /* ARFCN所代表的频段:GSM 480 */
#define MTC_GSM_BAND_TYPE_850                          (0x0004)                /* ARFCN所代表的频段:GSM 850 */
#define MTC_GSM_BAND_TYPE_P900                         (0x0008)                /* ARFCN所代表的频段:P-GSM 900 */
#define MTC_GSM_BAND_TYPE_R900                         (0x0010)                /* ARFCN所代表的频段:R-GSM 900 */
#define MTC_GSM_BAND_TYPE_E900                         (0x0020)                /* ARFCN所代表的频段:E-GSM 900 */
#define MTC_GSM_BAND_TYPE_1800                         (0x0040)                /* ARFCN所代表的频段:DCS 1800 */
#define MTC_GSM_BAND_TYPE_1900                         (0x0080)                /* ARFCN所代表的频段:PCS 1900 */
#define MTC_GSM_BAND_TYPE_700                          (0x0100)                /* ARFCN所代表的频段:GSM 700 */



#define MTC_WAS_FREQ_BAND2_ADD_MAX_FREQ_NUM             (12)                    /*频段2的最大附加频率数*/
#define MTC_WAS_FREQ_BAND4_ADD_MAX_FREQ_NUM             (9)                     /*频段4的最大附加频率数*/
#define MTC_WAS_FREQ_BAND5_ADD_MAX_FREQ_NUM             (6)                     /*频段5的最大附加频率数*/
#define MTC_WAS_FREQ_BAND6_ADD_MAX_FREQ_NUM             (2)                     /*频段6的最大附加频率数*/
#define MTC_WAS_FREQ_BAND7_ADD_MAX_FREQ_NUM             (14)                    /*频段7的最大附加频率数*/
#define MTC_WAS_FREQ_BAND19_ADD_MAX_FREQ_NUM            (3)                     /*频段19的最大附加频率数*/

#define MTC_WAS_FREQ_BAND1_DL_LOW_FREQ                  (10562)
#define MTC_WAS_FREQ_BAND1_DL_HIGH_FREQ                 (10838)
#define MTC_WAS_FREQ_BAND1_TXRXSEPERATION               (950)

#define MTC_WAS_FREQ_BAND2_DL_LOW_FREQ                  (9662)
#define MTC_WAS_FREQ_BAND2_DL_HIGH_FREQ                 (9938)
#define MTC_WAS_FREQ_BAND2_TXRXSEPERATION               (400)

#define MTC_WAS_FREQ_BAND3_DL_LOW_FREQ                  (1162)
#define MTC_WAS_FREQ_BAND3_DL_HIGH_FREQ                 (1513)
#define MTC_WAS_FREQ_BAND3_TXRXSEPERATION               (225)

#define MTC_WAS_FREQ_BAND4_DL_LOW_FREQ                  (1537)
#define MTC_WAS_FREQ_BAND4_DL_HIGH_FREQ                 (1738)
#define MTC_WAS_FREQ_BAND4_TXRXSEPERATION               (225)

#define MTC_WAS_FREQ_BAND5_DL_LOW_FREQ                  (4357)
#define MTC_WAS_FREQ_BAND5_DL_HIGH_FREQ                 (4458)
#define MTC_WAS_FREQ_BAND5_TXRXSEPERATION               (225)

#define MTC_WAS_FREQ_BAND6_DL_LOW_FREQ                  (4387)
#define MTC_WAS_FREQ_BAND6_DL_HIGH_FREQ                 (4413)
#define MTC_WAS_FREQ_BAND6_TXRXSEPERATION               (225)

#define MTC_WAS_FREQ_BAND7_DL_LOW_FREQ                  (2237)
#define MTC_WAS_FREQ_BAND7_DL_HIGH_FREQ                 (2563)
#define MTC_WAS_FREQ_BAND7_TXRXSEPERATION               (225)

#define MTC_WAS_FREQ_BAND8_DL_LOW_FREQ                  (2937)
#define MTC_WAS_FREQ_BAND8_DL_HIGH_FREQ                 (3088)
#define MTC_WAS_FREQ_BAND8_TXRXSEPERATION               (225)

#define MTC_WAS_FREQ_BAND9_DL_LOW_FREQ                  (9237)
#define MTC_WAS_FREQ_BAND9_DL_HIGH_FREQ                 (9387)
#define MTC_WAS_FREQ_BAND9_TXRXSEPERATION               (475)

#define MTC_WAS_FREQ_BAND10_DL_LOW_FREQ                 (3112)
#define MTC_WAS_FREQ_BAND10_DL_HIGH_FREQ                (3388)
#define MTC_WAS_FREQ_BAND10_TXRXSEPERATION              (225)

#define MTC_WAS_FREQ_BAND11_DL_LOW_FREQ                 (3712)
#define MTC_WAS_FREQ_BAND11_DL_HIGH_FREQ                (3787)
#define MTC_WAS_FREQ_BAND11_TXRXSEPERATION              (225)

#define MTC_WAS_FREQ_BAND19_DL_LOW_FREQ                 (712)
#define MTC_WAS_FREQ_BAND19_DL_HIGH_FREQ                (763)
#define MTC_WAS_FREQ_BAND19_TXRXSEPERATION              (400)


#define MTC_TDS_FRQ_UNIN                                (2) /* 单位为100KHZ */

/*****************************************************************************
  3 枚举定义
*****************************************************************************/
/*****************************************************************************
 枚举名    : MTC_BAND_INFO_ENUM
 协议表格  :
 枚举说明  : 频段枚举
*****************************************************************************/
enum MTC_BAND_INFO_ENUM
{
    MTC_FREQ_BAND_NONE = 0,                                                     /*  没有Band */
    MTC_FREQ_BAND1 ,                                                            /*  Band1 */
    MTC_FREQ_BAND2 ,
    MTC_FREQ_BAND3 ,
    MTC_FREQ_BAND4 ,
    MTC_FREQ_BAND5 ,
    MTC_FREQ_BAND6 ,
    MTC_FREQ_BAND7 ,
    MTC_FREQ_BAND8 ,
    MTC_FREQ_BAND9 ,
    MTC_FREQ_BAND10,
    MTC_FREQ_BAND11,
    MTC_FREQ_BAND12,
    MTC_FREQ_BAND13,
    MTC_FREQ_BAND14,
    MTC_FREQ_BAND15,
    MTC_FREQ_BAND16,
    MTC_FREQ_BAND17,
    MTC_FREQ_BAND18,
    MTC_FREQ_BAND19,
    MTC_FREQ_BAND20,

    MTC_FREQ_BAND21,
    MTC_FREQ_BAND22,
    MTC_FREQ_BAND23,
    MTC_FREQ_BAND24,
    MTC_FREQ_BAND25,
    MTC_FREQ_BAND26,
    MTC_FREQ_BAND27,
    MTC_FREQ_BAND28,
    MTC_FREQ_BAND29,
    MTC_FREQ_BAND30,
    MTC_FREQ_BAND31,
    MTC_FREQ_BAND32,
    MTC_FREQ_BAND33 = 33,
    MTC_FREQ_BAND34,
    MTC_FREQ_BAND35,
    MTC_FREQ_BAND36,
    MTC_FREQ_BAND37,
    MTC_FREQ_BAND38,
    MTC_FREQ_BAND39,
    MTC_FREQ_BAND40,
    MTC_FREQ_BAND41,
    MTC_FREQ_BAND42,
    MTC_FREQ_BAND43,
    MTC_FREQ_BAND64 = 64,

    /* 非标频段begin */
    MTC_FREQ_BAND101 = 101,
    MTC_FREQ_BAND102 = 102,
    MTC_FREQ_BAND103 = 103,
    MTC_FREQ_BAND104 = 104,
    MTC_FREQ_BAND105 = 105,
    MTC_FREQ_BAND106 = 106,
    MTC_FREQ_BAND107 = 107,
    MTC_FREQ_BAND108 = 108,
    MTC_FREQ_BAND109 = 109,
    MTC_FREQ_BAND110 = 110,
    MTC_FREQ_BAND111 = 111,
    MTC_FREQ_BAND112 = 112,
    MTC_FREQ_BAND113 = 113,
    MTC_FREQ_BAND114 = 114,
    MTC_FREQ_BAND115 = 115,
    MTC_FREQ_BAND116 = 116,

    /* 非标频段end */
    MTC_FREQ_BAND_BUTT = 0xFFFF
};
typedef VOS_UINT16 MTC_BAND_INFO_ENUM_UINT16;



/*****************************************************************************
  4 全局变量声明
*****************************************************************************/


/*****************************************************************************
  5 消息头定义
*****************************************************************************/


/*****************************************************************************
  6 消息定义
*****************************************************************************/


/*****************************************************************************
  7 STRUCT定义
*****************************************************************************/
/*****************************************************************************
 结构名    : MTC_RF_WCDMA_FREQ_BAND_INFO_STRU
 协议表格  : 25.101协议
 结构说明  : 频段的相关信息
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          uhwFreqBandId;                          /* BAND ID*/
    VOS_UINT16                          uhwTxRxSeparation;                      /* 协议典型的上下行收发频差*/
    VOS_UINT16                          uhwDlGeneralFreqNum_High;               /* 一般频点的下行频点上限 */
    VOS_UINT16                          uhwDlGeneralFreqNum_Low;                /* 一般频点的下行频点下限 */
    VOS_UINT16                          uhwDlGeneralOffset;                     /* 一般频点的下行偏置 */
    VOS_UINT16                          uhwDlAddOffset;                         /* 离散频点的下行偏置 */
    VOS_UINT16                          uhwUlGeneralFreqNum_High;               /* 一般频点的上行频点上限 */
    VOS_UINT16                          uhwUlGeneralFreqNum_Low;                /* 一般频点的上行频点下限 */
    VOS_UINT16                          uhwUlGeneralOffset;                     /* 一般频点的上行偏置 */
    VOS_UINT16                          uhwUlAddOffset;                         /* 离散频点的上行偏置 */
    VOS_UINT16                          uhwBandWidth;                           /* 频段带宽 */
    VOS_UINT16                          uhwStartFreq;                           /* 频段起始频率 100kHZ */
}MTC_RF_WCDMA_FREQ_BAND_INFO_STRU;

/*****************************************************************************
  8 UNION定义
*****************************************************************************/

/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/

/*****************************************************************************
  10 函数声明
*****************************************************************************/
VOS_UINT16 MTC_CalcGsmDlFreq(
    VOS_UINT16                          ushwBand,
    VOS_UINT16                          ushwFreqIndex
);

VOS_UINT16  MTC_CalcWcdmaDlFreq(
    VOS_UINT16                          ushwFreqBandId,
    VOS_UINT16                          ushwDlFreqNum
);

VOS_UINT32 MTC_WCDMAFindFreqBandNoByDlFreq(VOS_UINT16 usDlFreq);


#if (FEATURE_ON == FEATURE_LTE)
VOS_UINT16  MTC_CalcLteDlFreq(
    VOS_UINT16                          ucFreqBandIndicator,
    VOS_UINT16                          usDlEarfcn
);
#endif

#if (FEATURE_ON == FEATURE_UE_MODE_TDS)
VOS_UINT16  MTC_CalcTdsDlFreq(
    VOS_UINT16                          usDlEarfcn
);

#endif

#if 0
VOS_VOID  MTC_FillGsmNcellFreq(
    MTC_RF_USING_FREQ_LIST_STRU        *pstRfUsingFreqList,
    MTC_RF_FREQ_LIST_STRU              *pstRfFreqList
);

VOS_VOID  MTC_FillWcdmaNcellFreq(
    MTC_RF_USING_FREQ_LIST_STRU        *pstRfUsingFreqList,
    MTC_RF_FREQ_LIST_STRU              *pstRfFreqList
);

#if (FEATURE_ON == FEATURE_LTE)
VOS_VOID  MTC_FillLteNcellFreq(
    MTC_RF_USING_FREQ_LIST_STRU        *pstRfUsingFreqList,
    MTC_RF_FREQ_LIST_STRU              *pstRfFreqList
);
#endif

#if (FEATURE_ON == FEATURE_UE_MODE_TDS)
VOS_VOID  MTC_FillTdsNcellFreq(
    MTC_RF_USING_FREQ_LIST_STRU        *pstRfUsingFreqList,
    MTC_RF_FREQ_LIST_STRU              *pstRfFreqList
);
#endif
#endif

VOS_VOID  MTC_CalcHoppingFreq(
    MTC_RF_FREQ_LIST_STRU              *pstGsmFreqInfo,
    MTC_RF_FREQ_LIST_STRU              *pstRfFreqList
);

VOS_VOID  MTC_ChangeRfArfcnToFreq(
    RRC_MTC_USING_FREQ_IND_STRU        *pstUsingFreq,
    MTC_RF_USING_FREQ_LIST_STRU        *pstRfFreqList
);

VOS_VOID MTC_CalcRfDlFreq(
    RRC_MTC_USING_FREQ_IND_STRU        *pstUsingFreq,
    MTC_MODEM_FREQ_LIST_STRU           *pstModemFreq
);

#if (VOS_OS_VER == VOS_WIN32)
#pragma pack()
#else
#pragma pack(0)
#endif




#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of MtcCalcFreq.h */
