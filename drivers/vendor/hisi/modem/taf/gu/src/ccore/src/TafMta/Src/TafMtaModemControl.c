

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "TafMtaMain.h"
#include "TafMtaComm.h"
#include "TafMtaCtx.h"
#include "TafSdcCtx.h"
#include "TafSdcLib.h"
#include "MtaMtcInterface.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define    THIS_FILE_ID        PS_FILE_ID_TAF_MTA_MODEM_CONTROL_C

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/


/*****************************************************************************
  3 函数实现
*****************************************************************************/
#if (FEATURE_ON == FEATURE_MULTI_MODEM)

VOS_VOID TAF_MTA_RcvMtcPsTransferInd(
    VOS_VOID                           *pMsg
)
{
    MTC_MTA_PS_TRANSFER_IND_STRU       *pstPsTransferIndMsg;                    /* 接收消息指针 */
    AT_APPCTRL_STRU                     stAppCtrl;
    MTA_AT_PS_TRANSFER_IND_STRU         stPsTransferInd;

    /* 初始化局部变量 */
    pstPsTransferIndMsg     = (MTC_MTA_PS_TRANSFER_IND_STRU*)pMsg;
    PS_MEM_SET(&stAppCtrl, 0, sizeof(AT_APPCTRL_STRU));
    PS_MEM_SET(&stPsTransferInd, 0, sizeof(MTA_AT_PS_TRANSFER_IND_STRU));

    /* 填写消息结构 */
    stPsTransferInd.ucCause = pstPsTransferIndMsg->ucCause;

    /* 填写事件客户端类型为广播事件 */
    stAppCtrl.usClientId    = MTA_CLIENTID_BROADCAST;

    /* 给AT模块发送ID_MTA_AT_PS_TRANSFER_IND消息 */
    TAF_MTA_SndAtMsg( &stAppCtrl,
                      ID_MTA_AT_PS_TRANSFER_IND,
                      sizeof(MTA_AT_PS_TRANSFER_IND_STRU),
                      (VOS_UINT8*)&stPsTransferInd );

    return;
}
#endif
VOS_VOID TAF_MTA_SndMipiClkInfoToAt(VOS_UINT16 usMipiClk)
{
    AT_APPCTRL_STRU                     stAppCtrl;
    MTA_AT_RF_LCD_MIPICLK_IND_STRU      stMipClkInd;
    TAF_MTA_RF_LCD_CTX_STRU            *pstMtaRfLcdCtxAddr;

    /* 初始化局部变量 */
    PS_MEM_SET(&stAppCtrl, 0, sizeof(AT_APPCTRL_STRU));
    PS_MEM_SET(&stMipClkInd, 0, sizeof(MTA_AT_RF_LCD_MIPICLK_IND_STRU));

    /* 获取MTA上下文中保存Rf&Lcd干扰规避上下文地址 */
    pstMtaRfLcdCtxAddr = TAF_MTA_GetRfLcdCtxAddr();

    /* 填写消息结构 */
    stMipClkInd.usMipiClk = usMipiClk;

    /* 填写事件客户端类型为广播事件 */
    stAppCtrl.usClientId    = MTA_CLIENTID_BROADCAST;

    /* 给AT模块发送ID_MTA_AT_PS_TRANSFER_IND消息 */
    TAF_MTA_SndAtMsg( &stAppCtrl,
                      ID_MTA_AT_MIPICLK_INFO_IND,
                      sizeof(MTA_AT_RF_LCD_MIPICLK_IND_STRU),
                      (VOS_UINT8*)&stMipClkInd );

    /* 保存上报的MipiClk值 */
    pstMtaRfLcdCtxAddr->usReportMipiClk= usMipiClk;

    if (0 != pstMtaRfLcdCtxAddr->usReportMipiClk)
    {
        /* 在上报的为有效值时，启动定时器 */
        TAF_MTA_StartTimer(TI_TAF_MTA_RF_LCD_MIPI_CLK_REPORT, pstMtaRfLcdCtxAddr->ulTimerInterVal);
    }

    return;
}
VOS_UINT32 TAF_MTA_GetMipiClkRptCtl(VOS_VOID)
{
    TAF_SDC_CURC_RPT_CTRL_STRU         *pstCurcRptCtrl      = VOS_NULL_PTR;
    VOS_UINT32                          ulChkCurcRptFlg;

    /* 判断当前主动上报是否关闭 */
    pstCurcRptCtrl  = TAF_SDC_GetCurcRptCtrl();
    ulChkCurcRptFlg = TAF_SDC_CheckRptCmdStatus(pstCurcRptCtrl->aucRptCfg,
                                                TAF_SDC_CMD_RPT_CTRL_BY_CURC,
                                                TAF_SDC_RPT_CMD_MIPICLK);

    return ulChkCurcRptFlg;

}
VOS_VOID TAF_MTA_RcvTiMipiClkExpired(VOS_VOID *pMsg)
{
    TAF_MTA_RF_LCD_CTX_STRU            *pstMtaRfLcdCtxAddr  = VOS_NULL_PTR;
    VOS_UINT32                          ulChkCurcRptFlg;

    /* 获取MTA上下文中保存Rf&Lcd干扰规避上下文地址 */
    pstMtaRfLcdCtxAddr = TAF_MTA_GetRfLcdCtxAddr();

    if(pstMtaRfLcdCtxAddr->usNewMipiClk == pstMtaRfLcdCtxAddr->usReportMipiClk)
    {
        return;
    }

    ulChkCurcRptFlg = TAF_MTA_GetMipiClkRptCtl();
    if (VOS_FALSE == ulChkCurcRptFlg)
    {
        return;
    }

    /* 上报mipiclk */
    TAF_MTA_SndMipiClkInfoToAt(pstMtaRfLcdCtxAddr->usNewMipiClk);

    return;

}


VOS_VOID TAF_MTA_RcvMtcMipiClkInfoInd(VOS_VOID *pMsg)
{
    MTC_MTA_MIPICLK_INFO_IND_STRU      *pstMipiClkIndMsg    = VOS_NULL_PTR;
    TAF_MTA_RF_LCD_CTX_STRU            *pstMtaRfLcdCtxAddr  = VOS_NULL_PTR;
    VOS_UINT32                          ulChkCurcRptFlg;
    TAF_MTA_TIMER_STATUS_ENUM_UINT8     enStatus;

    /* 获取MTA上下文中保存Rf&Lcd干扰规避上下文地址 */
    pstMtaRfLcdCtxAddr                  = TAF_MTA_GetRfLcdCtxAddr();

    /* 初始化局部变量 */
    pstMipiClkIndMsg                    = (MTC_MTA_MIPICLK_INFO_IND_STRU *)pMsg;

    /* 保存最新上报的mipiclk值 */
    pstMtaRfLcdCtxAddr->usNewMipiClk    = pstMipiClkIndMsg->usMipiCLk;

    /* 定时器是否在运行 */
    enStatus = TAF_MTA_GetTimerStatus(TI_TAF_MTA_RF_LCD_MIPI_CLK_REPORT);
    if (TAF_MTA_TIMER_STATUS_RUNING == enStatus)
    {
        return;
    }

    /* 新上报的MipiClk与之前上报的 mipiclk值比较 */
    if (pstMtaRfLcdCtxAddr->usNewMipiClk == pstMtaRfLcdCtxAddr->usReportMipiClk)
    {
        return;
    }

    /* 判断当前主动上报是否关闭 */
    ulChkCurcRptFlg = TAF_MTA_GetMipiClkRptCtl();
    if (VOS_FALSE == ulChkCurcRptFlg)
    {
        return;
    }

    /* 给AT模块发送消息 */
    TAF_MTA_SndMipiClkInfoToAt(pstMtaRfLcdCtxAddr->usNewMipiClk);

    return;
}
VOS_VOID TAF_MTA_RcvAtQryMipiClkReq(VOS_VOID *pMsg)
{
    AT_MTA_MSG_STRU                    *pstAtMtaMsg         = VOS_NULL_PTR;
    TAF_MTA_RF_LCD_CTX_STRU            *pstMtaRfLcdCtx      = VOS_NULL_PTR;
    MTA_AT_RF_LCD_MIPICLK_CNF_STRU      stQryMipiCnf;
    TAF_MTA_TIMER_STATUS_ENUM_UINT8     enStatus;

    pstAtMtaMsg             = (AT_MTA_MSG_STRU *)pMsg;

    /* 获取MTA里Rf&Lcd上下文 */
    pstMtaRfLcdCtx          = TAF_MTA_GetRfLcdCtxAddr();

    /* 判断当前定时器状态 */
    enStatus = TAF_MTA_GetTimerStatus(TI_TAF_MTA_RF_LCD_MIPI_CLK_REPORT);
    if (TAF_MTA_TIMER_STATUS_RUNING == enStatus)
    {
        TAF_MTA_StopTimer(TI_TAF_MTA_RF_LCD_MIPI_CLK_REPORT);
    }

    /* 填充消息内容 */
    PS_MEM_SET(&stQryMipiCnf, 0x00, sizeof(stQryMipiCnf));
    stQryMipiCnf.usMipiClk      = pstMtaRfLcdCtx->usNewMipiClk;
    stQryMipiCnf.usResult       = VOS_OK;

    /* 如果该特性未开启，则给上层回复失败 */
    if (0 == pstMtaRfLcdCtx->usEnableBitMap)
    {
        stQryMipiCnf.usResult   = VOS_ERR;
    }
    else
    {
        /* 开启定时器 */
        TAF_MTA_StartTimer(TI_TAF_MTA_RF_LCD_MIPI_CLK_REPORT, pstMtaRfLcdCtx->ulTimerInterVal);

        /* 保存上报MIPICLK值 */
        pstMtaRfLcdCtx->usReportMipiClk = pstMtaRfLcdCtx->usNewMipiClk;
    }

    /* 给AT发送查询MIPICLK回复 */
    TAF_MTA_SndAtMsg((AT_APPCTRL_STRU *)&(pstAtMtaMsg->stAppCtrl),
                     ID_MTA_AT_MIPICLK_QRY_CNF,
                     sizeof(stQryMipiCnf),
                     (VOS_UINT8*)&stQryMipiCnf);

    return;
}




#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif


