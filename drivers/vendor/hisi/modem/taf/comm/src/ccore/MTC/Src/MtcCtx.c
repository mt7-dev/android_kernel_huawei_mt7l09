

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "MtcCtx.h"
#include "MtcDebug.h"
#include "TafNvInterface.h"
#include "NasNvInterface.h"
#include "NVIM_Interface.h"
#include "MtcRfLcdIntrusion.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define    THIS_FILE_ID        PS_FILE_ID_MTC_CTX_C

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
MTC_CONTEXT_STRU                        g_stMtcCtx;


/*****************************************************************************
  3 函数定义
*****************************************************************************/

MTC_CONTEXT_STRU* MTC_GetCtxAddr(VOS_VOID)
{
    return &g_stMtcCtx;
}


MTC_INTRUSION_CTX_STRU* MTC_GetIntrusionCtxAddr(VOS_VOID)
{
    return &(MTC_GetCtxAddr()->stIntrusionCtx);
}


MTC_MODEM_INFO_STRU* MTC_GetModemCtxAddr(MODEM_ID_ENUM_UINT16 enModemId)
{
    return &(MTC_GetCtxAddr()->astModemInfo[enModemId]);
}


VOS_VOID MTC_InitIntrusionCtx(VOS_VOID)
{
    VOS_UINT32                          ulRet;
    MTC_INTRUSION_CTX_STRU             *pstMtcIntrusionCtx = VOS_NULL_PTR;

    pstMtcIntrusionCtx = MTC_GetIntrusionCtxAddr();

    /* 初始化上下文 */
    PS_MEM_SET(pstMtcIntrusionCtx, 0, sizeof(MTC_INTRUSION_CTX_STRU));

    /* 读取NV项 */
    ulRet = NV_Read(en_NV_Item_RF_INTRUSION_CFG,
                    &pstMtcIntrusionCtx->stIntrusionCfg,
                    sizeof(MTC_NV_RF_INTRUSION_CFG_STRU));
    if (VOS_OK != ulRet)
    {
        MTC_WARNING_LOG("MTC_InitIntrusionCtx: Read Nv Fail.");
    }

    MTC_SetNotchEnalbeFlag(PS_FALSE);

    MTC_SetNarrowDcsEnalbeFlag(PS_FALSE);

    MTC_SetTlRfCtrlEnalbeFlag(PS_FALSE);

    MTC_SetRseCfgValue(PS_FALSE);

    return;
}


VOS_VOID MTC_InitPsTransferCtx(VOS_VOID)
{
    VOS_UINT32                          ulRet;
    MTC_PS_TRANSFER_CTX_STRU           *pstPsTransferCtx = VOS_NULL_PTR;

    pstPsTransferCtx = MTC_GetPsTransferCtxAddr();

    /* 初始化上下文 */
    PS_MEM_SET(pstPsTransferCtx, 0, sizeof(MTC_PS_TRANSFER_CTX_STRU));

    /* 初始化PSTRANSFER的上报状态值 */
    pstPsTransferCtx->enReportCause     = MTC_PS_TRANSFER_CAUSE_BUTT;
    pstPsTransferCtx->enRrcCause        = MTC_PS_TRANSFER_CAUSE_BUTT;

    /* 读取NV项 */
    ulRet = NV_Read(en_NV_Item_PS_TRANSFER_CFG,
                    &pstPsTransferCtx->stPsTransferCfg,
                    sizeof(MTC_NV_PS_TRANSFER_CFG_STRU));
    if (VOS_OK != ulRet)
    {
        MTC_WARNING_LOG("MTC_InitPsTransferCtx: Read Nv Fail.");
    }

    return;
}
VOS_VOID MTC_InitOperatorCustSolution(VOS_VOID)
{
    SVLTE_SUPPORT_FLAG_STRU             stSvlteSupportFlag;
    TAF_NV_LC_CTRL_PARA_STRU            stLcCtrlPara;

    PS_MEM_SET(&stSvlteSupportFlag, 0x00, sizeof(stSvlteSupportFlag));
    PS_MEM_SET(&stLcCtrlPara, 0x00, sizeof(stLcCtrlPara));

    /* 读SVLTE特性NV */
    if (NV_OK != NV_Read(en_NV_Item_SVLTE_FLAG,
                         &stSvlteSupportFlag,
                         sizeof(SVLTE_SUPPORT_FLAG_STRU)))
    {
        MTC_ERROR_LOG("MTC_InitOperatorCustSolution(): en_NV_Item_SVLTE_FLAG error");
        stSvlteSupportFlag.ucSvlteSupportFlag = VOS_FALSE;
    }

    /* 读LC特性NV */
    if (NV_OK != NV_Read(en_NV_Item_LC_Ctrl_PARA,
                         &stLcCtrlPara,
                         sizeof(TAF_NV_LC_CTRL_PARA_STRU)))
    {
        MTC_ERROR_LOG("MTC_InitOperatorCustSolution(): en_NV_Item_LC_Ctrl_PARA error");
        stLcCtrlPara.ucLCEnableFlg = VOS_FALSE;
    }

    if (VOS_TRUE == stSvlteSupportFlag.ucSvlteSupportFlag)
    {
        MTC_SetOperatorCustSolution(MTC_OPERATOR_CUST_CMCC_SVLTE);
    }
    else if (VOS_TRUE == stLcCtrlPara.ucLCEnableFlg)
    {
        MTC_SetOperatorCustSolution(MTC_OPERATOR_CUST_CT_LC);
    }
    else
    {
        MTC_SetOperatorCustSolution(MTC_OPERATOR_CUST_NONE);
    }

    return;
}


VOS_VOID MTC_ParsePlatFormRat(
    VOS_UINT8                          *pucRat,
    PLATAFORM_RAT_CAPABILITY_STRU      *pstPlatFormRat
)
{
    VOS_UINT32                          ulIndex;
    VOS_UINT16                          usRatNum;

    usRatNum = pstPlatFormRat->usRatNum;
    *pucRat  = 0;

    /* 将对应bit设置为1 */
    for (ulIndex = 0; ulIndex < usRatNum; ulIndex++)
    {
        if (pstPlatFormRat->aenRatList[ulIndex] >= PLATFORM_RAT_BUTT)
        {
            return;
        }

        *pucRat |= (VOS_UINT8)MTC_SET_BIT(pstPlatFormRat->aenRatList[ulIndex]);
    }

    return;
}
VOS_UINT32 MTC_IsSupportRatType(
    MODEM_ID_ENUM_UINT16                enModemId,
    MTC_RATMODE_ENUM_UINT8              enRatMode
)
{
    if (enRatMode >= MTC_RATMODE_BUTT)
    {
        return VOS_FALSE;
    }

    /* 判断modemId是否有效 */
    if (enModemId >= MODEM_ID_BUTT)
    {
        return VOS_FALSE;
    }

    if (0 != (MTC_GetModemCtxAddr(enModemId)->ucRatCap & MTC_SET_BIT(enRatMode)))
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;

}
VOS_VOID MTC_ResetModemInfo(MODEM_ID_ENUM_UINT16 enModemId)
{
    MTC_MODEM_INFO_STRU                *pstModemInfo = VOS_NULL_PTR;

    pstModemInfo = MTC_GetModemCtxAddr(enModemId);

    PS_MEM_SET(&pstModemInfo->stSerCellBandInfo, 0, sizeof(RRC_MTC_MS_BAND_INFO_STRU));
    PS_MEM_SET(&pstModemInfo->stCurBandInfo, 0, sizeof(RRC_MTC_MS_BAND_INFO_STRU));
    PS_MEM_SET(&pstModemInfo->stSrvInfo, 0, sizeof(MTC_MODEM_SERVICE_INFO_STRU));
    pstModemInfo->enRatMode = MTC_RATMODE_BUTT;

    return;
}


VOS_VOID MTC_InitModemCtx(VOS_VOID)
{
    VOS_UINT32                          ulRet;
    VOS_UINT16                          usIndex;

    PLATAFORM_RAT_CAPABILITY_STRU       stModemPlatFormRat;
    MTC_MODEM_INFO_STRU                *pastModemCtxAddr = VOS_NULL_PTR;

    for (usIndex = 0; usIndex < MODEM_ID_BUTT; usIndex++)
    {
        PS_MEM_SET(&stModemPlatFormRat, 0, sizeof(stModemPlatFormRat));

        /* 获取Modem上下文 地址 */
        pastModemCtxAddr = MTC_GetModemCtxAddr(usIndex);

        PS_MEM_SET(pastModemCtxAddr, 0, sizeof(MTC_MODEM_INFO_STRU));

        /* 读取Modem当前接入技术nv项 */
        ulRet = NV_ReadEx(usIndex,
                          en_NV_Item_Platform_RAT_CAP,
                          &stModemPlatFormRat,
                          sizeof(PLATAFORM_RAT_CAPABILITY_STRU));
        if (VOS_OK != ulRet)
        {
            MTC_WARNING_LOG("MTC_InitIntrusionCtx: Read Nv Fail.");
            continue;
        }

        MTC_ParsePlatFormRat(&(pastModemCtxAddr->ucRatCap), &stModemPlatFormRat);

        pastModemCtxAddr->enRatMode = MTC_RATMODE_BUTT;
    }

    return;
}


VOS_VOID MTC_InitCtx(VOS_VOID)
{
    /* 初始化MTC调试信息上下文 */
    MTC_InitDebugCtx();

    /* 初始化频段冲突相关上下文 */
    MTC_InitIntrusionCtx();

    /* 初始化MTC模块PS域迁移相关上下文 */
    MTC_InitPsTransferCtx();

    /* 初始化MTC模块Modem信息上下文 */
    MTC_InitModemCtx();

    /* 初始化MTC模块运营商定制特性 */
    MTC_InitOperatorCustSolution();

    /* 初始化RF&LCD干扰规避上下文 */
    MTC_InitRfLcdIntrusionCtx();

    return ;
}


MTC_CFG_ENUM_UINT8 MTC_GetIntrusionCfg(VOS_VOID)
{
    MTC_INTRUSION_CTX_STRU             *pstMtcIntrusionCtx = VOS_NULL_PTR;

    pstMtcIntrusionCtx = MTC_GetIntrusionCtxAddr();

    if (0 != (pstMtcIntrusionCtx->stIntrusionCfg.ucSolutionMask & MTC_INTRUSION_CFG_BIT))
    {
        return MTC_CFG_ENABLE;
    }
    else
    {
        return MTC_CFG_DISABLE;
    }
}


VOS_VOID MTC_SetModemPowerState(
    MODEM_ID_ENUM_UINT16                enModemId,
    MTC_MODEM_POWER_STATE_ENUM_UINT8    enState
)
{
    MTC_GetCtxAddr()->astModemInfo[enModemId].enPowerState = enState;

    return;
}


MTC_MODEM_POWER_STATE_ENUM_UINT8 MTC_GetModemPowerState(MODEM_ID_ENUM_UINT16 enModemId)
{
    return MTC_GetModemCtxAddr(enModemId)->enPowerState;
}


VOS_VOID MTC_SetModemUsimValidFlag(
    MODEM_ID_ENUM_UINT16                enModemId,
    VOS_UINT8                           ucUsimValidStatus
)
{
    MTC_GetCtxAddr()->astModemInfo[enModemId].ucUsimValidFlag = ucUsimValidStatus;

    return;
}


VOS_UINT8 MTC_GetModemUsimValidFlag(MODEM_ID_ENUM_UINT16 enModemId)
{
    return MTC_GetModemCtxAddr(enModemId)->ucUsimValidFlag;
}



VOS_VOID MTC_SetModemRatMode(
    MODEM_ID_ENUM_UINT16                enModemId,
    MTC_RATMODE_ENUM_UINT8              enRatMode
)
{
    MTC_GetModemCtxAddr(enModemId)->enRatMode = enRatMode;

    return;
}


MTC_RATMODE_ENUM_UINT8 MTC_GetModemRatMode(MODEM_ID_ENUM_UINT16 enModemId)
{
    return MTC_GetCtxAddr()->astModemInfo[enModemId].enRatMode;
}


VOS_VOID MTC_SetModemCallSrvExistFlg(
    MODEM_ID_ENUM_UINT16                enModemId,
    VOS_UINT8                           ucFlg
)
{
    MTC_GetCtxAddr()->astModemInfo[enModemId].stSrvInfo.ucCallSrvExistFlg = ucFlg;

    return;
}


VOS_VOID MTC_SetModemSmsSrvExistFlg(
    MODEM_ID_ENUM_UINT16                enModemId,
    VOS_UINT8                           ucFlg
)
{
    MTC_GetCtxAddr()->astModemInfo[enModemId].stSrvInfo.ucSmsSrvExistFlg = ucFlg;

    return;
}


VOS_VOID MTC_SetModemSsSrvExistFlg(
    MODEM_ID_ENUM_UINT16                enModemId,
    VOS_UINT8                           ucFlg
)
{
    MTC_GetCtxAddr()->astModemInfo[enModemId].stSrvInfo.ucSsSrvExistFlg = ucFlg;

    return;
}


MTC_CS_SERVICE_STATE_ENUM_UINT8 MTC_GetModemCsSrvExistFlg(
    MODEM_ID_ENUM_UINT16                enModemId
)
{
    MTC_MODEM_SERVICE_INFO_STRU        *pstSrvInfo = VOS_NULL_PTR;

    pstSrvInfo = &(MTC_GetCtxAddr()->astModemInfo[enModemId].stSrvInfo);

    /* 仅在当前呼叫、短信、补充业务都不存在时认为CS域服务不存在 */
    if ( (VOS_FALSE == pstSrvInfo->ucCallSrvExistFlg)
      && (VOS_FALSE == pstSrvInfo->ucSmsSrvExistFlg)
      && (VOS_FALSE == pstSrvInfo->ucSsSrvExistFlg) )
    {
        return MTC_CS_NO_SERVICE;
    }

    return MTC_CS_IN_SERVICE;
}
RRC_MTC_MS_BAND_INFO_STRU* MTC_GetModemCurBandInfo(
    MODEM_ID_ENUM_UINT16                enModemId
)
{
    return &(MTC_GetCtxAddr()->astModemInfo[enModemId].stCurBandInfo);
}


RRC_MTC_MS_BAND_INFO_STRU* MTC_GetModemSerCellBandInfo(
    MODEM_ID_ENUM_UINT16                enModemId
)
{
    return &(MTC_GetCtxAddr()->astModemInfo[enModemId].stSerCellBandInfo);
}


VOS_VOID MTC_SetNotchEnalbeFlag(PS_BOOL_ENUM_UINT8 enFlag)
{
    MTC_INTRUSION_CTX_STRU             *pstMtcIntrusionCtx = VOS_NULL_PTR;

    pstMtcIntrusionCtx = MTC_GetIntrusionCtxAddr();

    pstMtcIntrusionCtx->enNotchEnableFlag = enFlag;

    return;
}


PS_BOOL_ENUM_UINT8 MTC_GetNotchEnalbeFlag(VOS_VOID)
{
    MTC_INTRUSION_CTX_STRU             *pstMtcIntrusionCtx = VOS_NULL_PTR;

    pstMtcIntrusionCtx = MTC_GetIntrusionCtxAddr();

    return pstMtcIntrusionCtx->enNotchEnableFlag;
}


MTC_CFG_ENUM_UINT8 MTC_GetNotchCfg(VOS_VOID)
{
    MTC_INTRUSION_CTX_STRU             *pstMtcIntrusionCtx = VOS_NULL_PTR;

    pstMtcIntrusionCtx = MTC_GetIntrusionCtxAddr();

    if (0 != (pstMtcIntrusionCtx->stIntrusionCfg.ucSolutionMask & MTC_NOTCH_CFG_BIT))
    {
        return MTC_CFG_ENABLE;
    }
    else
    {
        return MTC_CFG_DISABLE;
    }
}


VOS_VOID MTC_SetNarrowDcsEnalbeFlag(PS_BOOL_ENUM_UINT8 enFlag)
{
    MTC_INTRUSION_CTX_STRU             *pstMtcIntrusionCtx = VOS_NULL_PTR;

    pstMtcIntrusionCtx = MTC_GetIntrusionCtxAddr();

    pstMtcIntrusionCtx->enNarrowDcsEnableFlag = enFlag;

    return;
}


PS_BOOL_ENUM_UINT8 MTC_GetNarrowDcsEnalbeFlag(VOS_VOID)
{
    MTC_INTRUSION_CTX_STRU             *pstMtcIntrusionCtx = VOS_NULL_PTR;

    pstMtcIntrusionCtx = MTC_GetIntrusionCtxAddr();

    return pstMtcIntrusionCtx->enNarrowDcsEnableFlag;
}


MTC_CFG_ENUM_UINT8 MTC_GetNarrowBandDcsCfg(VOS_VOID)
{
    MTC_INTRUSION_CTX_STRU             *pstMtcIntrusionCtx = VOS_NULL_PTR;

    pstMtcIntrusionCtx = MTC_GetIntrusionCtxAddr();

    if (0 != (pstMtcIntrusionCtx->stIntrusionCfg.ucSolutionMask & MTC_NARROW_BAND_DCS_CFG_BIT))
    {
        return MTC_CFG_ENABLE;
    }
    else
    {
        return MTC_CFG_DISABLE;
    }
}




MTC_CFG_ENUM_UINT8 MTC_GetTlRfCtrlCfg(VOS_VOID)
{
    MTC_RF_INTRUSION_CFG_STRU          *pstRfIntrusionCfg = VOS_NULL_PTR;
    MTC_CFG_ENUM_UINT8                  enMtcCfg;

    pstRfIntrusionCfg = (MTC_RF_INTRUSION_CFG_STRU *)&(MTC_GetIntrusionCtxAddr()->stIntrusionCfg);
    enMtcCfg          = (VOS_TRUE == pstRfIntrusionCfg->ucTlRfCtrlCfg) ? MTC_CFG_ENABLE : MTC_CFG_DISABLE;

    return enMtcCfg;
}


VOS_VOID MTC_SetTlRfCtrlEnalbeFlag(PS_BOOL_ENUM_UINT8 enFlag)
{
    MTC_GetIntrusionCtxAddr()->enTlRfCtrlEnableFlag = enFlag;
    return;
}


PS_BOOL_ENUM_UINT8 MTC_GetTlRfCtrlEnalbeFlag(VOS_VOID)
{
    return MTC_GetIntrusionCtxAddr()->enTlRfCtrlEnableFlag;
}


MTC_PS_TRANSFER_ENUM_UINT8 MTC_GetPsTransferCfg(VOS_VOID)
{
    return MTC_GetCtxAddr()->stPsTransferCtx.stPsTransferCfg.enSolutionCfg;
}


MTC_PS_TRANSFER_CTX_STRU* MTC_GetPsTransferCtxAddr(VOS_VOID)
{
    return &(MTC_GetCtxAddr()->stPsTransferCtx);
}


MTC_MODEM_NETWORK_INFO_STRU* MTC_GetModemNetworkInfoAddr(
    MODEM_ID_ENUM_UINT16                enModemId
)
{
    return &(MTC_GetCtxAddr()->astModemInfo[enModemId].stNetworkInfo);
}


MTC_OPERATOR_CUST_SOLUTION_ENUM_UINT8 MTC_GetOperatorCustSolution(VOS_VOID)
{
    return MTC_GetCtxAddr()->enOperatorCustSolution;
}


VOS_VOID MTC_SetOperatorCustSolution(
    MTC_OPERATOR_CUST_SOLUTION_ENUM_UINT8   enOperatorCustSolution
)
{
    MTC_GetCtxAddr()->enOperatorCustSolution = enOperatorCustSolution;
    return;
}



MTC_RF_LCD_INTRUSION_CTX_STRU* MTC_GetRfLcdIntrusionCtxAddr(VOS_VOID)
{
    return &(MTC_GetCtxAddr()->stRfLcdIntrusionCtx);
}


VOS_UINT32 MTC_GetMipiClkRcvPid(VOS_VOID)
{
    return MTC_GetRfLcdIntrusionCtxAddr()->ulMipiClkReportModemId;
}


VOS_VOID MTC_ReadRfLcdCfgNv(VOS_VOID)
{
    VOS_UINT16                          j;
    VOS_UINT16                          i;
    VOS_UINT32                          ulRet;
    VOS_UINT32                          ulFlag;
    MTC_NVIM_RF_LCD_CFG_STRU            stRfLcdCfg;
    MTC_RF_LCD_INTRUSION_CTX_STRU      *pstRfLcdIntrusionCtx = VOS_NULL_PTR;

    /* 清零 */
    PS_MEM_SET(&stRfLcdCfg, 0x0, sizeof(MTC_NVIM_RF_LCD_CFG_STRU));

    pstRfLcdIntrusionCtx = MTC_GetRfLcdIntrusionCtxAddr();

    /* 读取en_NV_Item_MTC_RF_LCD_CFG NV项 */
    ulRet = NV_Read(en_NV_Item_MTC_RF_LCD_CFG,
                    &stRfLcdCfg,
                    sizeof(MTC_NVIM_RF_LCD_CFG_STRU));
    if (VOS_OK != ulRet)
    {
        pstRfLcdIntrusionCtx->usEnableBitMap = 0x0;
        MTC_WARNING1_LOG("MTC_ReadRfLcdCfgNv: Read Nv Fail.", en_NV_Item_MTC_RF_LCD_CFG);
    }
    else
    {
        pstRfLcdIntrusionCtx->usFreqWidth  = stRfLcdCfg.usFreqWidth;
        pstRfLcdIntrusionCtx->usEnableBitMap = stRfLcdCfg.usEnableBitMap;
    }

    if (0x0 == pstRfLcdIntrusionCtx->usEnableBitMap)
    {
        return;
    }

    /* 判断频率信息的正确性 */
    for (i = 0; i < MTC_RF_LCD_MIPICLK_MAX_NUM; i++)
    {
        ulFlag = VOS_FALSE;
        pstRfLcdIntrusionCtx->astRfLcdFreqList[i].ulMipiClk     = stRfLcdCfg.astRfMipiClkFreqList[i].ulMipiClk;
        pstRfLcdIntrusionCtx->astRfLcdFreqList[i].ulAvailNum    = 0;

        for(j = 0; j < MTC_RF_LCD_MIPICLK_FREQ_MAX_NUM; j++)
        {
            /* 频率值需要大于带宽，否则对应MIPICLK的后续频率都视为无效值 */
            if (stRfLcdCfg.astRfMipiClkFreqList[i].aulFreq[j] >= pstRfLcdIntrusionCtx->usFreqWidth)
            {
                pstRfLcdIntrusionCtx->astRfLcdFreqList[i].aulFreq[j] = stRfLcdCfg.astRfMipiClkFreqList[i].aulFreq[j];
                pstRfLcdIntrusionCtx->astRfLcdFreqList[i].ulAvailNum++;
                ulFlag = VOS_TRUE;
            }
            else
            {
                break;
            }
        }

        if (VOS_TRUE == ulFlag)
        {
            pstRfLcdIntrusionCtx->ucAvailFreqListNum++;
        }
    }

    return;
}


MTC_MODEM_MIPICLK_BITMAP_STRU* MTC_GetRfCellInfoAddr(
    MODEM_ID_ENUM_UINT16                enModemId
)
{
    return &(MTC_GetModemCtxAddr(enModemId)->stRfCellInfo);
}


VOS_UINT16 MTC_GetMipiClkBitMap(VOS_VOID)
{
    return MTC_GetRfLcdIntrusionCtxAddr()->usMipiClkBitMap;
}


VOS_VOID MTC_InitRfCellInfo(MODEM_ID_ENUM_UINT16 enModemId)
{
    MTC_MODEM_MIPICLK_BITMAP_STRU      *pstRfCellInfo = VOS_NULL_PTR;

    pstRfCellInfo = MTC_GetRfCellInfoAddr(enModemId);

    pstRfCellInfo->enCsExistFlag     = PS_FALSE;
    pstRfCellInfo->enPsExistFlag     = PS_FALSE;
    pstRfCellInfo->usScellBitMap     = MTC_GetMipiClkBitMap();
    pstRfCellInfo->usHoppingBitMap   = MTC_GetMipiClkBitMap();
    pstRfCellInfo->usNcellBitMap     = MTC_GetMipiClkBitMap();
}


VOS_VOID MTC_InitRfLcdIntrusionCtx(VOS_VOID)
{
    MTC_RF_LCD_INTRUSION_CTX_STRU      *pstRfLcdIntrusionCtx = VOS_NULL_PTR;
    VOS_UINT8                           ucIndex;
    VOS_UINT16                          usModemIndex;

    pstRfLcdIntrusionCtx = MTC_GetRfLcdIntrusionCtxAddr();

    /* 初始化上下文 */
    PS_MEM_SET(pstRfLcdIntrusionCtx, 0x0, sizeof(MTC_RF_LCD_INTRUSION_CTX_STRU));

    /* 初始化为0xffff,为后继移位作准备 */
    pstRfLcdIntrusionCtx->usMipiClkBitMap = 0xFFFF;

    /* 读取NV项 */
    MTC_ReadRfLcdCfgNv();

    /* 根据LCD MIPICLK有效个数设置位图 */
    if (pstRfLcdIntrusionCtx->ucAvailFreqListNum <= MTC_RF_LCD_MIPICLK_MAX_NUM)
    {
        ucIndex = 16 - pstRfLcdIntrusionCtx->ucAvailFreqListNum;
        pstRfLcdIntrusionCtx->usMipiClkBitMap >>= ucIndex;
    }

    /* 设置^mipiclk主动上报接收的MTA Pid */
    pstRfLcdIntrusionCtx->ulMipiClkReportModemId = I0_UEPS_PID_MTA;             /* 当前方案默认从Modem0上报 */

    for (usModemIndex = 0; usModemIndex < MODEM_ID_BUTT; usModemIndex++)
    {
        /* 初始化RF&LCD筛选模块上下文 */
        MTC_InitRfCellInfo(usModemIndex);
    }

    return;
}
MTC_CFG_ENUM_UINT8 MTC_GetRfLcdIntrusionCfg(VOS_VOID)
{
    MTC_RF_LCD_INTRUSION_CTX_STRU      *pstRfLcdIntrusionCtx = VOS_NULL_PTR;

    pstRfLcdIntrusionCtx = MTC_GetRfLcdIntrusionCtxAddr();

    if (0 != pstRfLcdIntrusionCtx->usEnableBitMap)
    {
        return MTC_CFG_ENABLE;
    }
    else
    {
        return MTC_CFG_DISABLE;
    }
}


MTC_CFG_ENUM_UINT8 MTC_GetRseSupportCfg(VOS_VOID)
{
    MTC_INTRUSION_CTX_STRU             *pstMtcIntrusionCtx = VOS_NULL_PTR;

    pstMtcIntrusionCtx = MTC_GetIntrusionCtxAddr();

    if (0 != (pstMtcIntrusionCtx->stIntrusionCfg.ucSolutionMask & MTC_RSE_CFG_BIT))
    {
        return MTC_CFG_ENABLE;
    }
    else
    {
        return MTC_CFG_DISABLE;
    }
}


PS_BOOL_ENUM_UINT8 MTC_GetRseCfgValue(VOS_VOID)
{
    MTC_INTRUSION_CTX_STRU             *pstMtcIntrusionCtx = VOS_NULL_PTR;

    pstMtcIntrusionCtx = MTC_GetIntrusionCtxAddr();

    return pstMtcIntrusionCtx->enRseCfg;
}


VOS_VOID MTC_SetRseCfgValue(PS_BOOL_ENUM_UINT8 enValue)
{
    MTC_INTRUSION_CTX_STRU             *pstMtcIntrusionCtx = VOS_NULL_PTR;

    pstMtcIntrusionCtx = MTC_GetIntrusionCtxAddr();

    pstMtcIntrusionCtx->enRseCfg = enValue;

    return;
}





#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

