

#ifndef __MTCDEBUG_H__
#define __MTCDEBUG_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include "PsCommonDef.h"
#include "MtcCtx.h"
#include "MtcIntrusion.h"
#include "MtcPsTransfer.h"
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
/* 封装可维可测事件消息头 */
#define MTC_DEBUG_CFG_MSG_HDR(pstMsg, SenderPid, ReceiverPid, ulLen)\
                (pstMsg)->ulSenderCpuId   = VOS_LOCAL_CPUID;\
                (pstMsg)->ulSenderPid     = SenderPid;\
                (pstMsg)->ulReceiverCpuId = VOS_LOCAL_CPUID;\
                (pstMsg)->ulReceiverPid   = ReceiverPid;\
                (pstMsg)->ulLength        = (ulLen);



#define MTC_DEBUG_RF_LCD_ALLOC_MSG_ERR()        g_stMtcDebugInfo.stRfLcdDebugInfo.ulAllocMsgErr++

#define MTC_DEBUG_RF_LCD_SND_MIPICLK_IND_OK()   g_stMtcDebugInfo.stRfLcdDebugInfo.ulSndMipiClkInfoIndOK++

#define MTC_DEBUG_RF_LCD_SND_MIPICLK_IND_ERR()  g_stMtcDebugInfo.stRfLcdDebugInfo.ulSndMipiClkInfoIndErr++

#define MTC_DEBUG_SAVE_AS_RF_USING_INFO_NUM(ASFreq)      g_stRfUsingDebugInfo.ASFreq.ulFreqNum++

#define MTC_DEBUG_SAVE_RF_USING_INFO(ASFreq, info, Value, Num)  g_stRfUsingDebugInfo.ASFreq.astArfcnInfo[Num].info = (Value)

#define MTC_DEBUG_SAVE_RF_USING_CS_PS_INFO(csFlag, psFlag) \
                g_stRfUsingDebugInfo.enCsExistFlag = csFlag;\
                g_stRfUsingDebugInfo.enPsExistFlag = psFlag

#define MTC_DEBUG_COPY_MODEM_MIPICLK(ModemMipiClk)  PS_MEM_CPY(&g_stRfUsingDebugInfo.stModemMipiClk, &(ModemMipiClk), sizeof(MTC_MODEM_MIPI_CLK_PRI_STRU))





/*****************************************************************************
  3 枚举定义
*****************************************************************************/

enum MTC_DEBUG_BAND_ENUM
{
    MTC_DEBUG_BAND_NONE                   = 0x00,
    MTC_DEBUG_BAND_1,
    MTC_DEBUG_BAND_3,
    MTC_DEBUG_BAND_8,
    MTC_DEBUG_BAND_3_8,
    MTC_DEBUG_BAND_31,
    MTC_DEBUG_BAND_31_8,
    MTC_DEBUG_BAND_32,
    MTC_DEBUG_BAND_32_8,
    MTC_DEBUG_BAND_39,

    MTC_DEBUG_BAND_BUTT
};
typedef VOS_UINT8  MTC_DEBUG_BAND_ENUM_UINT8;


enum MTC_DEBUG_MSG_ID_ENUM
{
    ID_DEBUG_MTC_CTX_INFO                           = 0xF000,                  /* _H2ASN_MsgChoice MTC_CONTEXT_DEBUG_STRU */
    ID_DEBUG_MTC_RF_USING_FREQ_LIST_INFO            = 0xF001,                  /* _H2ASN_MsgChoice MTC_DEBUG_RF_USING_FREQ_LIST_STRU */

    ID_DEBUG_MTC_BUTT
};
typedef VOS_UINT32 MTC_DEBUG_MSG_ID_ENUM_UINT32;



/*****************************************************************************
  5 消息头定义
*****************************************************************************/

/*****************************************************************************
  6 消息定义
*****************************************************************************/

/*****************************************************************************
  7 STRUCT定义
*****************************************************************************/

typedef struct
{
    VOS_UINT16                          ausRcvPowerOnNum[MODEM_ID_BUTT];
    VOS_UINT16                          ausRcvPowerOffNum[MODEM_ID_BUTT];
    VOS_UINT16                          ausRcvCsSrvNum[MODEM_ID_BUTT];
    VOS_UINT16                          usSndAction2TDSRrcReqNum;
    VOS_UINT16                          usSndAction2LTERrcReqNum;
    VOS_UINT16                          usSndAction2GASReqNum;
    VOS_UINT16                          usRcvTDSRrcActionCnfNum;
    VOS_UINT16                          usRcvLTERrcActionCnfNum;
    VOS_UINT16                          usRcvGASBandSetCnfNum;
    VOS_UINT16                          usRcvLTEBandSetCnfNum;
    VOS_UINT16                          usRcvTDSBandSetCnfNum;
    VOS_UINT16                          usRcvGASActionCnfNum;
    VOS_UINT16                          usSndGasSetBandReqNum;
    VOS_UINT16                          usSndLteSetBandReqNum;
    VOS_UINT16                          usSndTdsSetBandReqNum;
}MTC_INTRUSION_DEBUG_INFO;


typedef struct
{
    VOS_UINT16                          usRcvTDSAreaLostIndNum;
    VOS_UINT16                          usRcvLTEAreaLostIndNum;
    VOS_UINT16                          usRcvAreaAvaliableIndNum;
    VOS_UINT16                          ausSndPsTransferIndNum[MTC_PS_TRANSFER_CAUSE_BUTT];
    VOS_UINT16                          ausReserved[2];
}MTC_PS_TRANSFER_DEBUG_INFO;


typedef struct
{
    VOS_UINT32                          ulAllocMsgErr;
    VOS_UINT32                          ulSndMipiClkInfoIndOK;
    VOS_UINT32                          ulSndMipiClkInfoIndErr;
}MTC_DEBUG_RF_LCD_INFO_STRU;



typedef struct
{
    MTC_INTRUSION_DEBUG_INFO            stIntrusionDebugInfo;
    MTC_PS_TRANSFER_DEBUG_INFO          stPsTransferDebugInfo;
    MTC_DEBUG_RF_LCD_INFO_STRU          stRfLcdDebugInfo;
}MTC_DEBUG_INFO_STRU;


typedef struct
{
    VOS_MSG_HEADER
    MTC_DEBUG_MSG_ID_ENUM_UINT32        enMsgType;
    MTC_CONTEXT_STRU                    stMtcCtx;
}MTC_CONTEXT_DEBUG_STRU;


typedef struct
{
    VOS_UINT16                          usArfcn;
    VOS_UINT16                          usBandId;
    VOS_UINT16                          usFreq;
    VOS_UINT8                           aucRsv[2];
}MTC_DEBUG_ARFCN_INFO_STRU;


typedef struct
{
    VOS_UINT32                          ulFreqNum;
    MTC_DEBUG_ARFCN_INFO_STRU           astArfcnInfo[MTC_RF_FREQ_MAX_NUM];
}MTC_DEBUG_RF_FREQ_LIST_STRU;


typedef struct
{
    MSG_HEADER_STRU                     stMsgHeader;                            /*_H2ASN_Skip*/
    PS_BOOL_ENUM_UINT8                  enCsExistFlag;                          /* PS_FALSE 表示不存在CS连接 */
    PS_BOOL_ENUM_UINT8                  enPsExistFlag;                          /* PS_FALSE 表示不存在PS连接 */
    VOS_UINT8                           aucRsv[2];
    MTC_DEBUG_RF_FREQ_LIST_STRU         stGsmFreq;
    MTC_DEBUG_RF_FREQ_LIST_STRU         stGsmHoppingFreq;
    MTC_DEBUG_RF_FREQ_LIST_STRU         stWcdmaFreq;
    MTC_DEBUG_RF_FREQ_LIST_STRU         stLteFreq;
    MTC_DEBUG_RF_FREQ_LIST_STRU         stTdsFreq;
    MTC_MODEM_MIPI_CLK_PRI_STRU         stModemMipiClk;
}MTC_DEBUG_RF_USING_FREQ_LIST_STRU;


/*****************************************************************************
  H2ASN顶级消息结构定义
*****************************************************************************/
typedef struct
{
    MTC_DEBUG_MSG_ID_ENUM_UINT32        enMsgId;                                /*_H2ASN_MsgChoice_Export MTC_DEBUG_MSG_ID_ENUM_UINT32*/
    VOS_UINT8                           aucMsgBlock[4];
    /***************************************************************************
        _H2ASN_MsgChoice_When_Comment          MTC_DEBUG_MSG_ID_ENUM_UINT32
    ****************************************************************************/
}MTC_DEBUG_MSG_DATA;
/*_H2ASN_Length UINT32*/

typedef struct
{
    VOS_MSG_HEADER
    MTC_DEBUG_MSG_DATA                  stMsgData;
}MtcDebugInterface_MSG;

/*****************************************************************************
  9 全局变量声明
*****************************************************************************/
extern MTC_DEBUG_INFO_STRU                     g_stMtcDebugInfo;
extern MTC_DEBUG_RF_USING_FREQ_LIST_STRU       g_stRfUsingDebugInfo;
/*****************************************************************************
  10 UNION定义
*****************************************************************************/

/*****************************************************************************
  11 OTHERS定义
*****************************************************************************/

/*****************************************************************************
  12 函数声明
*****************************************************************************/
VOS_VOID MTC_InitDebugCtx(VOS_VOID);

MTC_INTRUSION_DEBUG_INFO* MTC_GetIntrusionDebugCtxAddr(VOS_VOID);

MTC_PS_TRANSFER_DEBUG_INFO* MTC_GetPsTransferDebugCtxAddr(VOS_VOID);

VOS_VOID MTC_DEBUG_IntrusionRcvSetBandCnf(VOS_UINT32 ulPid);

VOS_VOID MTC_DEBUG_IntrusionSendActionReq(VOS_UINT32 ulPid);

VOS_VOID MTC_DEBUG_IntrusionRcvActionCnf(VOS_UINT32 ulPid);

VOS_VOID MTC_DEBUG_RcvPowerStateInd(
    MODEM_ID_ENUM_UINT16                enModemId,
    MTC_MODEM_POWER_STATE_ENUM_UINT8    enPowerState
);

VOS_VOID MTC_DEBUG_IntrusionSendSetBandReq(VOS_UINT32 ulPid);

VOS_VOID MTC_DEBUG_RcvCsSrvInfoInd(
    MODEM_ID_ENUM_UINT16                enModemId
);

VOS_VOID MTC_DEBUG_RcvRrcAreaLostInd(
    VOS_UINT32                          ulSenderPid
);

VOS_VOID MTC_DEBUG_SndPsTransferInd(
    MTC_PS_TRANSFER_CAUSE_ENUM_UINT8    enPsTransferCause
);

VOS_VOID MTC_DEBUG_RcvRrcAreaAvaliableInd(VOS_VOID);

MTC_DEBUG_RF_LCD_INFO_STRU* MTC_DEBUG_GetRfLcdDebugAddr(VOS_VOID);

VOS_VOID MTC_DEBUG_TraceCtxInfo(VOS_VOID);
VOS_VOID MTC_DEBUG_InitRfUsingFreqListInfo(VOS_VOID);
VOS_VOID MTC_DEBUG_TraceRfUsingFreqListInfo(VOS_VOID);



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

#endif /* end of MtcDebug.h */


