

#include <msp_errno.h>
#include <osm.h>
#include <gen_msg.h>
#include <ftm.h>
#include "LMspLPhyInterface.h"
#include "ftm_ct.h"
/*lint -save -e537*/
#include <DrvInterface.h>
/*lint -restore*/

/*lint -e767*/
#define    THIS_FILE_ID        MSP_FILE_ID_FTM_LTE_QUICK_CT_C
/*lint -e767*/

/* 从地软查到的PA值 */
VOS_UINT32 g_ulFtmCmTms = 0;

VOS_UINT32 g_ulFtmBspFail=0;
VOS_UINT32 g_ulFtmDspFail=0;

/*****************************************************************************
 函 数 名  : at_ftm_fblk_store
 功能描述  : 保存参数
 输入参数  : ucStoreType 保存类型
               FTM_F_CT_STORE_REQ_TEMP 收到装备参数设置指令，未收到LRTT确认原语，只缓存参数
               FTM_F_CT_STORE_REQ_GLOBAL 收到LRTT确认原语，保存参数，供装备查询
             pstReq  参数
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
/*lint -save -e655 -e732 -e958*/
static VOID at_ftm_fblk_store(VOS_UINT8 ucStoreType, FTM_SET_F_FBLK_REQ_STRU* pstReq)
{
    static FTM_SET_F_FBLK_REQ_STRU stReq = {0};
    FTM_CT_F_MANAGE_INFO_STRU* pstFtmFastCtInfo = ftm_GetFastCTMainInfo();

    /* 收到装备参数设置指令，未收到LRTT确认原语，只缓存参数*/
    if((FTM_F_CT_STORE_REQ_TEMP==ucStoreType) && (NULL != pstReq))
    {
        MSP_MEMCPY(&stReq, pstReq, sizeof(FTM_SET_F_FBLK_REQ_STRU));
    }
    /* 收到LRTT确认原语，保存参数，供装备查询 */
    else
    {
        pstFtmFastCtInfo->ulCurCmdStepsFlag |= CT_F_STEPS_DONE_FBLK;
        pstFtmFastCtInfo->ulBlocking= stReq.ulBlocking;
    }

    return ;
}

/*****************************************************************************
 函 数 名  : at_ftm_fblk_setcnf
 功能描述  : 保存参数
 输入参数  : ulErrCode 错误码
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
static VOS_UINT32 at_ftm_fblk_setcnf(VOS_UINT32 ulErrCode)
{
    FTM_SET_F_FBLK_CNF_STRU stCnf = { 0 };

    stCnf.ulErrCode = ulErrCode;

    return ftm_comm_send(ID_MSG_FTM_F_SET_FBLK_CNF,
        (VOS_UINT32)&stCnf, sizeof(FTM_SET_F_FBLK_CNF_STRU));
}

/*****************************************************************************
 函 数 名  : at_ftm_fblk_read
 功能描述  : ID_MSG_FTM_F_RD_FBLK_REQ 处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fblk_read(VOS_VOID* pParam)
{
    FTM_RD_F_FBLK_CNF_STRU stRdCnf = { 0 };

    stRdCnf.ulErrCode = ERR_MSP_SUCCESS;
    stRdCnf.ulBlocking= ftm_GetFastCTMainInfo()->ulBlocking;

     return ftm_comm_send(ID_MSG_FTM_F_RD_FBLK_CNF, (VOS_UINT32)&stRdCnf, sizeof(FTM_RD_F_FBLK_CNF_STRU));
}

/*****************************************************************************
 函 数 名  : at_ftm_fblk_set
 功能描述  : ID_MSG_FTM_F_SET_FBLK_REQ 处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fblk_set(VOS_VOID* pParam)
{
    FTM_SET_F_FBLK_REQ_STRU* pstFtmReq      = (FTM_SET_F_FBLK_REQ_STRU*)pParam;
    OM_PHY_CT_F_BLK_SET_REQ_STRU stReqToDsp = { 0 };

    /* 判断是否处于非信令模式下 */
    if(ftm_GetNoneSig() != FTM_NONESIG_RTT_OPEN)
    {
        return at_ftm_fblk_setcnf(ERR_MSP_UE_MODE_ERR);
    }

    /* 发送原语到DSP */
    stReqToDsp.ulMsgId     = OM_PHY_CT_F_FBLK_SET_REQ;
    stReqToDsp.usIsBLKMode = (VOS_UINT16)pstFtmReq->ulBlocking;
    if(ERR_MSP_SUCCESS != ftm_mailbox_ltect_write(&stReqToDsp, sizeof(OM_PHY_CT_F_BLK_SET_REQ_STRU)))
    {
        return at_ftm_fblk_setcnf(ERR_MSP_UNKNOWN);
    }

    at_ftm_fblk_store(FTM_F_CT_STORE_REQ_TEMP, pstFtmReq);
    return ERR_MSP_WAIT_ASYNC;
}

/*****************************************************************************
 函 数 名  : at_ftm_fblk_timeout
 功能描述  : 超时处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fblk_timeout()
{
    return at_ftm_fblk_setcnf(ERR_MSP_TIME_OUT);
}

/*****************************************************************************
 函 数 名  : at_ftm_fblk_dspcnf
 功能描述  : DSP原语PHY_OM_CT_F_FBLK_SET_CNF处理函数
 输入参数  : pParam PHY_OM_CT_F_BLK_SET_CNF_STRU指针
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fblk_dspcnf(VOS_VOID* pParam)
{

    at_ftm_fblk_store(FTM_F_CT_STORE_REQ_GLOBAL, NULL);

    return at_ftm_fblk_setcnf(ERR_MSP_SUCCESS);
}

/*****************************************************************************
 函 数 名  : at_ftm_fchans_store
 功能描述  : 保存参数
 输入参数  : ucStoreType 保存类型
               FTM_F_CT_STORE_REQ_TEMP 收到装备参数设置指令，未收到LRTT确认原语，只缓存参数
               FTM_F_CT_STORE_REQ_GLOBAL 收到LRTT确认原语，保存参数，供装备查询
             pstReq  参数
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
static VOID at_ftm_fchans_store(VOS_UINT8 ucStoreType, FTM_SET_F_FCHANS_REQ_STRU* pstReq)
{
    static FTM_SET_F_FCHANS_REQ_STRU stReq = {0};
    FTM_CT_F_MANAGE_INFO_STRU* pstFtmFastCtInfo = ftm_GetFastCTMainInfo();

    /* 收到装备参数设置指令，未收到LRTT确认原语，只缓存参数*/
    if((FTM_F_CT_STORE_REQ_TEMP==ucStoreType) && (NULL != pstReq))
    {
        MSP_MEMCPY(&stReq, pstReq, sizeof(FTM_SET_F_FCHANS_REQ_STRU));
    }
    /* 收到LRTT确认原语，保存参数，供装备查询 */
    else
    {
        pstFtmFastCtInfo->ulCurCmdStepsFlag |= CT_F_STEPS_DONE_FCHANS;
        pstFtmFastCtInfo->usCurUlListNum = stReq.usULFreqNum;
        pstFtmFastCtInfo->usCurDlListNum = stReq.usDLFreqNum;
        MSP_MEMCPY(pstFtmFastCtInfo->ausCurUlFreqInfo, stReq.usULFreqList, sizeof(pstFtmFastCtInfo->ausCurUlFreqInfo));
        MSP_MEMCPY(pstFtmFastCtInfo->ausCurDlFreqInfo, stReq.usDLFreqList, sizeof(pstFtmFastCtInfo->ausCurDlFreqInfo));
    }

    return ;
}

/*****************************************************************************
 函 数 名  : at_ftm_fchans_setcnf
 功能描述  : 保存参数
 输入参数  : ulErrCode 错误码
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
static VOS_UINT32 at_ftm_fchans_setcnf(VOS_UINT32 ulErrCode)
{
    FTM_SET_F_FCHANS_CNF_STRU stCnf = { 0 };

    stCnf.ulErrCode = ulErrCode;

    return ftm_comm_send(ID_MSG_FTM_F_SET_FCHANS_CNF,
        (VOS_UINT32)&stCnf, sizeof(FTM_SET_F_FCHANS_CNF_STRU));
}

/*****************************************************************************
 函 数 名  : at_ftm_fchans_read
 功能描述  : ID_MSG_FTM_F_RD_FCHANS_REQ 处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fchans_read(VOS_VOID* pParam)
{
    FTM_RD_F_FCHANS_CNF_STRU stRdCnf = { 0 };
    FTM_CT_F_MANAGE_INFO_STRU* pstFtmFastCtInfo = ftm_GetFastCTMainInfo();

    stRdCnf.ulErrCode   = ERR_MSP_SUCCESS;
    stRdCnf.usULFreqNum = pstFtmFastCtInfo->usCurUlListNum;
    stRdCnf.usDLFreqNum = pstFtmFastCtInfo->usCurDlListNum;

    MSP_MEMCPY(stRdCnf.usULFreqList, pstFtmFastCtInfo->ausCurUlFreqInfo, sizeof(stRdCnf.usULFreqList));
    MSP_MEMCPY(stRdCnf.usDLFreqList, pstFtmFastCtInfo->ausCurDlFreqInfo, sizeof(stRdCnf.usDLFreqList));

    return ftm_comm_send(ID_MSG_FTM_F_RD_FCHANS_CNF, (VOS_UINT32)&stRdCnf, sizeof(FTM_RD_F_FCHANS_CNF_STRU));
}

/*****************************************************************************
 函 数 名  : at_ftm_fchans_set
 功能描述  : ID_MSG_FTM_F_SET_FCHANS_REQ 处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fchans_set(VOS_VOID* pParam)
{
    VOS_UINT16 usTmp                              = 0;
    FTM_SET_F_FCHANS_REQ_STRU* pstFtmReq     = (FTM_SET_F_FCHANS_REQ_STRU*)pParam;
    OM_PHY_CT_F_FREQ_SET_REQ_STRU stReqToDsp = { 0 };
    FTM_CT_F_MANAGE_INFO_STRU* pstFtmFastCtInfo = ftm_GetFastCTMainInfo();
    FCHAN_MODE_ENUM fchanMode = pstFtmFastCtInfo->enCurMode;
    /* 判断是否处于非信令模式下 */
    if(ftm_GetNoneSig() != FTM_NONESIG_RTT_OPEN)
    {
        return at_ftm_fchans_setcnf(ERR_MSP_UE_MODE_ERR);
    }

    /* 判断参数是否正确 */
    if ((pstFtmReq->usULFreqNum > CT_F_FREQ_UL_LIST_MAX_NUM)
        || (pstFtmReq->usDLFreqNum > CT_F_FREQ_DL_LIST_MAX_NUM))
    {
        return at_ftm_fchans_setcnf(ERR_MSP_INVALID_PARAMETER);
    }

    if((pstFtmReq->usULFreqNum>0) && (pstFtmReq->usDLFreqNum>0)
       && (pstFtmReq->usULFreqNum != pstFtmReq->usDLFreqNum ))
    {
        return at_ftm_fchans_setcnf(ERR_MSP_INVALID_PARAMETER);
    }

    /* 发送原语到DSP */
    stReqToDsp.ulMsgId     = OM_PHY_CT_F_FREQ_SET_REQ;
    stReqToDsp.ulMode      = fchanMode;
    stReqToDsp.usUlListNum = pstFtmReq->usULFreqNum;
    stReqToDsp.usDlListNum = pstFtmReq->usDLFreqNum;

    /* 信道号转换成频率信息 */
    for(usTmp=0; usTmp<stReqToDsp.usUlListNum; usTmp++)
    {
        stReqToDsp.ausUlFreqInfo[usTmp] = at_ftm_get_freq(fchanMode, pstFtmReq->usULFreqList[usTmp]);
    }

    for(usTmp=0; usTmp<stReqToDsp.usDlListNum; usTmp++)
    {
        stReqToDsp.ausDlFreqInfo[usTmp] = at_ftm_get_freq(fchanMode, pstFtmReq->usDLFreqList[usTmp]);
    }
    
    if(ERR_MSP_SUCCESS != ftm_mailbox_ct_write(fchanMode, &stReqToDsp, sizeof(OM_PHY_CT_F_FREQ_SET_REQ_STRU)))
    {
        return at_ftm_fchans_setcnf(ERR_MSP_UNKNOWN);
    }
    at_ftm_fchans_store(FTM_F_CT_STORE_REQ_TEMP, pstFtmReq);
    return ERR_MSP_WAIT_ASYNC;
}

/*****************************************************************************
 函 数 名  : at_ftm_fchans_timeout
 功能描述  : 超时处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fchans_timeout()
{
    return at_ftm_fchans_setcnf(ERR_MSP_TIME_OUT);
}

/*****************************************************************************
 函 数 名  : at_ftm_fchans_dspcnf
 功能描述  : DSP原语PHY_OM_CT_F_FREQ_SET_CNF处理函数
 输入参数  : pParam PHY_OM_CT_F_FREQ_SET_CNF_STRU指针
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fchans_dspcnf(VOS_VOID* pParam)
{

    at_ftm_fchans_store(FTM_F_CT_STORE_REQ_GLOBAL, NULL);

    return at_ftm_fchans_setcnf(ERR_MSP_SUCCESS);
}

/*****************************************************************************
 函 数 名  : at_ftm_fsegment_store
 功能描述  : 保存参数
 输入参数  : ucStoreType 保存类型
               FTM_F_CT_STORE_REQ_TEMP 收到装备参数设置指令，未收到LRTT确认原语，只缓存参数
               FTM_F_CT_STORE_REQ_GLOBAL 收到LRTT确认原语，保存参数，供装备查询
             pstReq  参数
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
static VOID at_ftm_fsegment_store(VOS_UINT8 ucStoreType, FTM_SET_F_FSEGMENT_REQ_STRU* pstReq)
{
    static FTM_SET_F_FSEGMENT_REQ_STRU stReq    = {0};
    FTM_CT_F_MANAGE_INFO_STRU* pstFtmFastCtInfo = ftm_GetFastCTMainInfo();

    /* 收到装备参数设置指令，未收到LRTT确认原语，只缓存参数*/
    if((FTM_F_CT_STORE_REQ_TEMP==ucStoreType) && (NULL != pstReq))
    {
        MSP_MEMCPY(&stReq, pstReq, sizeof(FTM_SET_F_FSEGMENT_REQ_STRU));
    }
    /* 收到LRTT确认原语，保存参数，供装备查询 */
    else
    {
        pstFtmFastCtInfo->ulCurCmdStepsFlag |= CT_F_STEPS_DONE_FSEGMENT;
        pstFtmFastCtInfo->usCurSegmentLen    = stReq.usSegmentLen;
        pstFtmFastCtInfo->usCurMeasOffset    = stReq.usMeasOffset;
        pstFtmFastCtInfo->usCurMeasLen       = stReq.usMeasLen;
    }

    return ;
}

/*****************************************************************************
 函 数 名  : at_ftm_fsegment_setcnf
 功能描述  : 保存参数
 输入参数  : ulErrCode 错误码
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
static VOS_UINT32 at_ftm_fsegment_setcnf(VOS_UINT32 ulErrCode)
{
    FTM_SET_F_FSEGMENT_CNF_STRU stCnf = { 0 };

    stCnf.ulErrCode = ulErrCode;

    return ftm_comm_send(ID_MSG_FTM_F_SET_FSEGMENT_CNF,
        (VOS_UINT32)&stCnf, sizeof(FTM_SET_F_FSEGMENT_CNF_STRU));
}

/*****************************************************************************
 函 数 名  : at_ftm_fsegment_read
 功能描述  : ID_MSG_FTM_F_RD_FSEGMENT_REQ 处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fsegment_read(VOS_VOID* pParam)
{
    FTM_RD_F_FSEGMENT_CNF_STRU stRdCnf = { 0 };
    FTM_CT_F_MANAGE_INFO_STRU* pstFtmFastCtInfo = ftm_GetFastCTMainInfo();

    stRdCnf.ulErrCode    = ERR_MSP_SUCCESS;
    stRdCnf.usSegmentLen = pstFtmFastCtInfo->usCurSegmentLen;
    stRdCnf.usMeasOffset = pstFtmFastCtInfo->usCurMeasOffset;
    stRdCnf.usMeasLen    = pstFtmFastCtInfo->usCurMeasLen;

    return ftm_comm_send(ID_MSG_FTM_F_RD_FSEGMENT_CNF, (VOS_UINT32)&stRdCnf, sizeof(FTM_RD_F_FSEGMENT_CNF_STRU));
}

/*****************************************************************************
 函 数 名  : at_ftm_fsegment_set
 功能描述  : ID_MSG_FTM_F_SET_FSEGMENT_REQ 处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fsegment_set(VOS_VOID* pParam)
{
    FTM_SET_F_FSEGMENT_REQ_STRU* pstFtmReq      = (FTM_SET_F_FSEGMENT_REQ_STRU*)pParam;
    OM_PHY_CT_F_SEGMENT_SET_REQ_STRU stReqToDsp = { 0 };
    FCHAN_MODE_ENUM fchanMode = ftm_GetFastCTMainInfo()->enCurMode;
    /* 判断是否处于非信令模式下 */
    if(ftm_GetNoneSig() != FTM_NONESIG_RTT_OPEN)
    {
        return at_ftm_fsegment_setcnf(ERR_MSP_UE_MODE_ERR);
    }

    /* 判断参数是否正确 */

    /* 发送原语到DSP */
    stReqToDsp.ulMsgId   = OM_PHY_CT_F_SEGMENT_SET_REQ;
    stReqToDsp.usSegment = pstFtmReq->usSegmentLen;
    stReqToDsp.measOffset= pstFtmReq->usMeasOffset;
    stReqToDsp.measLen   = pstFtmReq->usMeasLen;
    if(ERR_MSP_SUCCESS != ftm_mailbox_ct_write(fchanMode, &stReqToDsp, sizeof(OM_PHY_CT_F_SEGMENT_SET_REQ_STRU)))
    {
        return at_ftm_fsegment_setcnf(ERR_MSP_UNKNOWN);
    }
    at_ftm_fsegment_store(FTM_F_CT_STORE_REQ_TEMP, pstFtmReq);
    return ERR_MSP_WAIT_ASYNC;
}

/*****************************************************************************
 函 数 名  : at_ftm_fsegment_timeout
 功能描述  : 超时处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fsegment_timeout()
{
    return at_ftm_fsegment_setcnf(ERR_MSP_TIME_OUT);
}

/*****************************************************************************
 函 数 名  : at_ftm_fsegment_dspcnf
 功能描述  : DSP原语PHY_OM_CT_F_SEGMENT_SET_CNF处理函数
 输入参数  : pParam PHY_OM_CT_F_SEGMENT_SET_CNF_STRU指针
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fsegment_dspcnf(VOS_VOID* pParam)
{

    at_ftm_fsegment_store(FTM_F_CT_STORE_REQ_GLOBAL, NULL);

    return at_ftm_fsegment_setcnf(ERR_MSP_SUCCESS);
}

/*****************************************************************************
 函 数 名  : at_ftm_fpows_store
 功能描述  : 保存参数
 输入参数  : ucStoreType 保存类型
               FTM_F_CT_STORE_REQ_TEMP 收到装备参数设置指令，未收到LRTT确认原语，只缓存参数
               FTM_F_CT_STORE_REQ_GLOBAL 收到LRTT确认原语，保存参数，供装备查询
             pstReq  参数
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
static VOID at_ftm_fpows_store(VOS_UINT8 ucStoreType, FTM_SET_F_FPOWS_REQ_STRU* pstReq)
{
    static FTM_SET_F_FPOWS_REQ_STRU stReq       = {0};
    FTM_CT_F_MANAGE_INFO_STRU* pstFtmFastCtInfo = ftm_GetFastCTMainInfo();

    /* 收到装备参数设置指令，未收到LRTT确认原语，只缓存参数*/
    if((FTM_F_CT_STORE_REQ_TEMP==ucStoreType) && (NULL != pstReq))
    {
        MSP_MEMCPY(&stReq, pstReq, sizeof(FTM_SET_F_FPOWS_REQ_STRU));
    }
    /* 收到LRTT确认原语，保存参数，供装备查询 */
    else
    {
        pstFtmFastCtInfo->ulCurCmdStepsFlag |= CT_F_STEPS_DONE_FPOWS;
        pstFtmFastCtInfo->usCurTxPowerNum    = stReq.usTxPowerNum;
        MSP_MEMCPY(pstFtmFastCtInfo->asCurTxPowerList, stReq.asTxPower,sizeof(pstFtmFastCtInfo->asCurTxPowerList));
    }

    return ;
}

/*****************************************************************************
 函 数 名  : at_ftm_fpows_setcnf
 功能描述  : 保存参数
 输入参数  : ulErrCode 错误码
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
static VOS_UINT32 at_ftm_fpows_setcnf(VOS_UINT32 ulErrCode)
{
    FTM_SET_F_FPOWS_CNF_STRU stCnf = { 0 };

    stCnf.ulErrCode = ulErrCode;

    return ftm_comm_send(ID_MSG_FTM_F_SET_FPOWS_CNF,
        (VOS_UINT32)&stCnf, sizeof(FTM_SET_F_FPOWS_CNF_STRU));
}

/*****************************************************************************
 函 数 名  : at_ftm_fpows_read
 功能描述  : ID_MSG_FTM_F_RD_FPOWS_REQ 处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fpows_read(VOS_VOID* pParam)
{
    FTM_RD_F_FPOWS_CNF_STRU stRdCnf             = { 0 };
    FTM_CT_F_MANAGE_INFO_STRU* pstFtmFastCtInfo = ftm_GetFastCTMainInfo();

    stRdCnf.ulErrCode    = ERR_MSP_SUCCESS;
    stRdCnf.usTxPowerNum = pstFtmFastCtInfo->usCurTxPowerNum;
    MSP_MEMCPY(stRdCnf.asTxPower, pstFtmFastCtInfo->asCurTxPowerList, sizeof(stRdCnf.asTxPower));

    return ftm_comm_send(ID_MSG_FTM_F_RD_FPOWS_CNF, (VOS_UINT32)&stRdCnf, sizeof(FTM_RD_F_FPOWS_CNF_STRU));
}

/*****************************************************************************
 函 数 名  : at_ftm_fpows_set
 功能描述  : ID_MSG_FTM_F_SET_FPOWS_REQ 处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fpows_set(VOS_VOID* pParam)
{
    FTM_SET_F_FPOWS_REQ_STRU* pstFtmReq         = (FTM_SET_F_FPOWS_REQ_STRU*)pParam;
    OM_PHY_CT_F_TXPOW_SET_REQ_STRU stReqToDsp   = { 0 };

    /* 判断是否处于非信令模式下 */
    if(ftm_GetNoneSig() != FTM_NONESIG_RTT_OPEN)
    {
        return at_ftm_fpows_setcnf(ERR_MSP_UE_MODE_ERR);
    }

    /* 判断参数是否正确 */

    /* 发送原语到DSP */
    stReqToDsp.ulMsgId      = OM_PHY_CT_F_TXPOW_SET_REQ;
    stReqToDsp.usTxPowerNum = pstFtmReq->usTxPowerNum;
    MSP_MEMCPY(stReqToDsp.ausTxPower, pstFtmReq->asTxPower, sizeof(stReqToDsp.ausTxPower));
    if(ERR_MSP_SUCCESS != ftm_mailbox_ltect_write(&stReqToDsp, sizeof(OM_PHY_CT_F_TXPOW_SET_REQ_STRU)))
    {
        return at_ftm_fpows_setcnf(ERR_MSP_UNKNOWN);
    }

    at_ftm_fpows_store(FTM_F_CT_STORE_REQ_TEMP, pstFtmReq);
    return ERR_MSP_WAIT_ASYNC;
}

/*****************************************************************************
 函 数 名  : at_ftm_fpows_timeout
 功能描述  : 超时处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fpows_timeout()
{
    return at_ftm_fpows_setcnf(ERR_MSP_TIME_OUT);
}

/*****************************************************************************
 函 数 名  : at_ftm_fpows_dspcnf
 功能描述  : DSP原语PHY_OM_CT_F_TXPOW_SET_CNF处理函数
 输入参数  : pParam PHY_OM_CT_F_TXPOW_SET_CNF_STRU指针
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fpows_dspcnf(VOS_VOID* pParam)
{

    at_ftm_fpows_store(FTM_F_CT_STORE_REQ_GLOBAL, NULL);

    return at_ftm_fpows_setcnf(ERR_MSP_SUCCESS);
}

/*****************************************************************************
 函 数 名  : at_ftm_fpas_store
 功能描述  : 保存参数
 输入参数  : ucStoreType 保存类型
               FTM_F_CT_STORE_REQ_TEMP 收到装备参数设置指令，未收到LRTT确认原语，只缓存参数
               FTM_F_CT_STORE_REQ_GLOBAL 收到LRTT确认原语，保存参数，供装备查询
             pstReq  参数
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
static VOID at_ftm_fpas_store(VOS_UINT8 ucStoreType, FTM_SET_F_FPAS_REQ_STRU* pstReq)
{
    static FTM_SET_F_FPAS_REQ_STRU stReq        = {0};
    FTM_CT_F_MANAGE_INFO_STRU* pstFtmFastCtInfo = ftm_GetFastCTMainInfo();

    /* 收到装备参数设置指令，未收到LRTT确认原语，只缓存参数*/
    if((FTM_F_CT_STORE_REQ_TEMP==ucStoreType) && (NULL != pstReq))
    {
        MSP_MEMCPY(&stReq, pstReq, sizeof(FTM_SET_F_FPAS_REQ_STRU));
    }
    /* 收到LRTT确认原语，保存参数，供装备查询 */
    else
    {
        pstFtmFastCtInfo->ulCurCmdStepsFlag |= CT_F_STEPS_DONE_FPAS;
        pstFtmFastCtInfo->usCurPaLevelNum    = stReq.usPaLevelNum;
        MSP_MEMCPY(pstFtmFastCtInfo->aucCurPaLevelList, stReq.aucPaLevelList, sizeof(pstFtmFastCtInfo->aucCurPaLevelList));
    }

    return ;
}

/*****************************************************************************
 函 数 名  : at_ftm_fpas_setcnf
 功能描述  : 保存参数
 输入参数  : ulErrCode 错误码
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
static VOS_UINT32 at_ftm_fpas_setcnf(VOS_UINT32 ulErrCode)
{
    FTM_SET_F_FPAS_CNF_STRU stCnf = { 0 };

    stCnf.ulErrCode = ulErrCode;

    return ftm_comm_send(ID_MSG_FTM_F_SET_FPAS_CNF,
        (VOS_UINT32)&stCnf, sizeof(FTM_SET_F_FPAS_CNF_STRU));
}

/*****************************************************************************
 函 数 名  : at_ftm_fpas_read
 功能描述  : ID_MSG_FTM_F_RD_FPAS_REQ 处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fpas_read(VOS_VOID* pParam)
{
    FTM_RD_F_FPAS_CNF_STRU stRdCnf = { 0 };
    FTM_CT_F_MANAGE_INFO_STRU* pstFtmFastCtInfo = ftm_GetFastCTMainInfo();

    stRdCnf.ulErrCode    = ERR_MSP_SUCCESS;
    stRdCnf.usPaLevelNum = pstFtmFastCtInfo->usCurPaLevelNum;
    MSP_MEMCPY(stRdCnf.aucPaLevelList,  pstFtmFastCtInfo->aucCurPaLevelList, sizeof(stRdCnf.aucPaLevelList));

    return ftm_comm_send(ID_MSG_FTM_F_RD_FPAS_CNF, (VOS_UINT32)&stRdCnf, sizeof(FTM_RD_F_FPAS_CNF_STRU));
}

/*****************************************************************************
 函 数 名  : at_ftm_fpas_set
 功能描述  : ID_MSG_FTM_F_SET_FPAS_REQ 处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fpas_set(VOS_VOID* pParam)
{
    FTM_SET_F_FPAS_REQ_STRU* pstFtmReq          = (FTM_SET_F_FPAS_REQ_STRU*)pParam;
    OM_PHY_CT_F_PA_SET_REQ_STRU stReqToDsp      = { 0 };
    FCHAN_MODE_ENUM fchanMode = ftm_GetFastCTMainInfo()->enCurMode;
    /* 判断是否处于非信令模式下 */
    if(ftm_GetNoneSig() != FTM_NONESIG_RTT_OPEN)
    {
        return at_ftm_fpas_setcnf(ERR_MSP_UE_MODE_ERR);
    }

    /* 判断参数是否正确 */

    /* 发送原语到DSP */
    stReqToDsp.ulMsgId    = OM_PHY_CT_F_PA_SET_REQ;
    stReqToDsp.usLevelNum = pstFtmReq->usPaLevelNum;
    MSP_MEMCPY(stReqToDsp.aucLvlList, pstFtmReq->aucPaLevelList, sizeof(stReqToDsp.aucLvlList));
    if(ERR_MSP_SUCCESS != ftm_mailbox_ct_write(fchanMode, &stReqToDsp, sizeof(OM_PHY_CT_F_PA_SET_REQ_STRU)))
    {
        return at_ftm_fpas_setcnf(ERR_MSP_UNKNOWN);
    }
    at_ftm_fpas_store(FTM_F_CT_STORE_REQ_TEMP, pstFtmReq);
    return ERR_MSP_WAIT_ASYNC;
}

/*****************************************************************************
 函 数 名  : at_ftm_fpas_timeout
 功能描述  : 超时处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fpas_timeout()
{
    return at_ftm_fpas_setcnf(ERR_MSP_TIME_OUT);
}

/*****************************************************************************
 函 数 名  : at_ftm_fpas_dspcnf
 功能描述  : DSP原语PHY_OM_CT_F_PA_SET_CNF处理函数
 输入参数  : pParam PHY_OM_CT_F_PA_SET_CNF_STRU指针
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fpas_dspcnf(VOS_VOID* pParam)
{

    at_ftm_fpas_store(FTM_F_CT_STORE_REQ_GLOBAL, NULL);

    return at_ftm_fpas_setcnf(ERR_MSP_SUCCESS);
}

/*****************************************************************************
 函 数 名  : at_ftm_flnas_store
 功能描述  : 保存参数
 输入参数  : ucStoreType 保存类型
               FTM_F_CT_STORE_REQ_TEMP 收到装备参数设置指令，未收到LRTT确认原语，只缓存参数
               FTM_F_CT_STORE_REQ_GLOBAL 收到LRTT确认原语，保存参数，供装备查询
             pstReq  参数
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
static VOID at_ftm_flnas_store(VOS_UINT8 ucStoreType, FTM_SET_F_AAGC_REQ_STRU* pstReq)
{
    static FTM_SET_F_AAGC_REQ_STRU stReq        = {0};
    FTM_CT_F_MANAGE_INFO_STRU* pstFtmFastCtInfo = ftm_GetFastCTMainInfo();

    /* 收到装备参数设置指令，未收到LRTT确认原语，只缓存参数*/
    if((FTM_F_CT_STORE_REQ_TEMP==ucStoreType) && (NULL != pstReq))
    {
        MSP_MEMCPY(&stReq, pstReq, sizeof(FTM_SET_F_AAGC_REQ_STRU));
    }
    /* 收到LRTT确认原语，保存参数，供装备查询 */
    else
    {
        pstFtmFastCtInfo->ulCurCmdStepsFlag |= CT_F_STEPS_DONE_FLNAS;
        pstFtmFastCtInfo->usCurAagcNum       = stReq.usAagcNum;
        MSP_MEMCPY(pstFtmFastCtInfo->aucCurAagcList, stReq.aucAAGCValue, sizeof(pstFtmFastCtInfo->aucCurAagcList));
    }

    return ;
}

/*****************************************************************************
 函 数 名  : at_ftm_flnas_setcnf
 功能描述  : 保存参数
 输入参数  : ulErrCode 错误码
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
static VOS_UINT32 at_ftm_flnas_setcnf(VOS_UINT32 ulErrCode)
{
    FTM_SET_F_AAGC_CNF_STRU stCnf = { 0 };

    stCnf.ulErrCode = ulErrCode;

    return ftm_comm_send(ID_MSG_FTM_F_SET_FLNAS_CNF,
        (VOS_UINT32)&stCnf, sizeof(FTM_SET_F_AAGC_CNF_STRU));
}

/*****************************************************************************
 函 数 名  : at_ftm_flnas_read
 功能描述  : ID_MSG_FTM_F_RD_FLNAS_REQ 处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_flnas_read(VOS_VOID* pParam)
{
    FTM_RD_F_AAGC_CNF_STRU stRdCnf = { 0 };
    FTM_CT_F_MANAGE_INFO_STRU* pstFtmFastCtInfo = ftm_GetFastCTMainInfo();

    stRdCnf.ulErrCode = ERR_MSP_SUCCESS;
    stRdCnf.usAagcNum = pstFtmFastCtInfo->usCurAagcNum;
    MSP_MEMCPY(stRdCnf.aucAAGCValue, pstFtmFastCtInfo->aucCurAagcList, sizeof(stRdCnf.aucAAGCValue));

    return ftm_comm_send(ID_MSG_FTM_F_RD_FLNAS_CNF, (VOS_UINT32)&stRdCnf, sizeof(FTM_RD_F_AAGC_CNF_STRU));
}

/*****************************************************************************
 函 数 名  : at_ftm_flnas_set
 功能描述  : ID_MSG_FTM_F_SET_FLNAS_REQ 处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_flnas_set(VOS_VOID* pParam)
{
    FTM_SET_F_AAGC_REQ_STRU* pstFtmReq          = (FTM_SET_F_AAGC_REQ_STRU*)pParam;
    OM_PHY_CT_F_AAGC_SET_REQ_STRU stReqToDsp    = { 0 };
    FCHAN_MODE_ENUM fchanMode = ftm_GetFastCTMainInfo()->enCurMode;
    /* 判断是否处于非信令模式下 */
    if(ftm_GetNoneSig() != FTM_NONESIG_RTT_OPEN)
    {
        return at_ftm_flnas_setcnf(ERR_MSP_UE_MODE_ERR);
    }

    /* 判断参数是否正确 */

    /* 发送原语到DSP */
    stReqToDsp.ulMsgId   = OM_PHY_CT_F_AAGC_SET_REQ;
    stReqToDsp.usAagcNum = pstFtmReq->usAagcNum;
    MSP_MEMCPY(stReqToDsp.aucAAGCValue, pstFtmReq->aucAAGCValue, sizeof(stReqToDsp.aucAAGCValue));
    if(ERR_MSP_SUCCESS != ftm_mailbox_ct_write(fchanMode, &stReqToDsp, sizeof(OM_PHY_CT_F_AAGC_SET_REQ_STRU)))
    {
        return at_ftm_flnas_setcnf(ERR_MSP_UNKNOWN);
    }
    at_ftm_flnas_store(FTM_F_CT_STORE_REQ_TEMP, pstFtmReq);
    return ERR_MSP_WAIT_ASYNC;
}

/*****************************************************************************
 函 数 名  : at_ftm_flnas_timeout
 功能描述  : 超时处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_flnas_timeout()
{
    return at_ftm_flnas_setcnf(ERR_MSP_TIME_OUT);
}

/*****************************************************************************
 函 数 名  : at_ftm_flnas_dspcnf
 功能描述  : DSP原语PHY_OM_CT_F_AAGC_SET_CNF处理函数
 输入参数  : pParam PHY_OM_CT_F_AAGC_SET_CNF_STRU指针
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_flnas_dspcnf(VOS_VOID* pParam)
{

    at_ftm_flnas_store(FTM_F_CT_STORE_REQ_GLOBAL, NULL);

    return at_ftm_flnas_setcnf(ERR_MSP_SUCCESS);
}

/*****************************************************************************
 函 数 名  : at_ftm_fpdms_store
 功能描述  : 保存参数
 输入参数  : ucStoreType 保存类型
               FTM_F_CT_STORE_REQ_TEMP 收到装备参数设置指令，未收到LRTT确认原语，只缓存参数
               FTM_F_CT_STORE_REQ_GLOBAL 收到LRTT确认原语，保存参数，供装备查询
             pstReq  参数
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
static VOID at_ftm_fpdms_store(VOS_UINT8 ucStoreType, FTM_SET_F_FPDMS_REQ_STRU* pstReq)
{
    static FTM_SET_F_FPDMS_REQ_STRU stReq    = {0};
    FTM_CT_F_MANAGE_INFO_STRU *pstFCtMainInfo = ftm_GetFastCTMainInfo();

    /* 收到装备参数设置指令，未收到LRTT确认原语，只缓存参数*/
    if ((FTM_F_CT_STORE_REQ_TEMP==ucStoreType) && (NULL != pstReq))
    {
        MSP_MEMCPY(&stReq, pstReq, sizeof(FTM_SET_F_FPDMS_REQ_STRU));
    }
    /* 收到LRTT确认原语，保存参数，供装备查询 */
    else
    {
        pstFCtMainInfo->ulCurCmdStepsFlag |= CT_F_STEPS_DONE_FPDMS;
        pstFCtMainInfo->ulFpdmsSw          = stReq.ulSwitch;
    }

    return ;
}

/*****************************************************************************
 函 数 名  : at_ftm_fpdms_setcnf
 功能描述  : 保存参数
 输入参数  : ulErrCode 错误码
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
static VOS_UINT32 at_ftm_fpdms_setcnf(VOS_UINT32 ulErrCode)
{
    FTM_SET_F_FPDMS_CNF_STRU stCnf = { 0 };

    stCnf.ulErrCode = ulErrCode;

    return ftm_comm_send(ID_MSG_FTM_SET_FPDMS_CNF,
        (VOS_UINT32)&stCnf, sizeof(FTM_SET_F_FPDMS_CNF_STRU));
}

/*****************************************************************************
 函 数 名  : at_ftm_fpdms_read
 功能描述  : ID_MSG_FTM_RD_FPDMS_RD 处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fpdms_read(VOS_VOID* pParam)
{
    FTM_RD_F_FPDMS_CNF_STRU stRdCnf          = { 0 };
    FTM_CT_F_MANAGE_INFO_STRU *stFCtMainInfo = ftm_GetFastCTMainInfo();

    stRdCnf.ulErrCode = ERR_MSP_SUCCESS;
    stRdCnf.ulSwitch  = stFCtMainInfo->ulFpdmsSw;

    return ftm_comm_send(ID_MSG_FTM_RD_FPDMS_CNF, (VOS_UINT32)&stRdCnf, sizeof(FTM_RD_F_FPDMS_CNF_STRU));
}

/*****************************************************************************
 函 数 名  : at_ftm_fpdms_set
 功能描述  : ID_MSG_FTM_F_SET_FPDMS_REQ 处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fpdms_set(VOS_VOID* pParam)
{
    FTM_SET_F_FPDMS_REQ_STRU* pstFtmReq       = (FTM_SET_F_FPDMS_REQ_STRU*)pParam;
    OM_PHY_CT_F_FPDMS_SET_REQ_STRU stReqToDsp = { 0 };

    /* 判断是否处于非信令模式下 */
    if (ftm_GetNoneSig() != FTM_NONESIG_RTT_OPEN)
    {
        return at_ftm_fpdms_setcnf(ERR_MSP_UE_MODE_ERR);
    }

    /* 发送原语到DSP */
    stReqToDsp.ulMsgId    = OM_PHY_CT_F_FPDMS_REQ;
    stReqToDsp.ulIsEnable = pstFtmReq->ulSwitch;
    if(ERR_MSP_SUCCESS != ftm_mailbox_ltect_write(&stReqToDsp, sizeof(OM_PHY_CT_F_FPDMS_SET_REQ_STRU)))
    {
        return at_ftm_fpdms_setcnf(ERR_MSP_UNKNOWN);
    }

    at_ftm_fpdms_store(FTM_F_CT_STORE_REQ_TEMP, pstFtmReq);
    return ERR_MSP_WAIT_ASYNC;
}

/*****************************************************************************
 函 数 名  : at_ftm_fpdms_timeout
 功能描述  : 超时处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fpdms_timeout()
{
    return at_ftm_fpdms_setcnf(ERR_MSP_TIME_OUT);
}

/*****************************************************************************
 函 数 名  : at_ftm_fpdms_dspcnf
 功能描述  : DSP原语PHY_OM_CT_F_FPDMS_CNF处理函数
 输入参数  : pParam PHY_OM_CT_F_FPDMS_SET_CNF_STRU指针
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fpdms_dspcnf(VOS_VOID* pParam)
{

    at_ftm_fpdms_store(FTM_F_CT_STORE_REQ_GLOBAL, NULL);

    return at_ftm_fpdms_setcnf(ERR_MSP_SUCCESS);
}

/*****************************************************************************
 函 数 名  : at_ftm_ftxwave_store
 功能描述  : 保存参数
 输入参数  : ucStoreType 保存类型
               FTM_F_CT_STORE_REQ_TEMP 收到装备参数设置指令，未收到LRTT确认原语，只缓存参数
               FTM_F_CT_STORE_REQ_GLOBAL 收到LRTT确认原语，保存参数，供装备查询
             pstReq  参数
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
static VOID at_ftm_ftxwave_store(VOS_UINT8 ucStoreType, FTM_SET_F_FTXWAVE_REQ_STRU* pstReq)
{
    static FTM_SET_F_FTXWAVE_REQ_STRU stReq  = {0};
    FTM_CT_F_MANAGE_INFO_STRU *pstFCtMainInfo = ftm_GetFastCTMainInfo();

    /* 收到装备参数设置指令，未收到LRTT确认原语，只缓存参数*/
    if ((FTM_F_CT_STORE_REQ_TEMP==ucStoreType) && (NULL != pstReq))
    {
        MSP_MEMCPY(&stReq, pstReq, sizeof(FTM_SET_F_FTXWAVE_REQ_STRU));
    }
    /* 收到LRTT确认原语，保存参数，供装备查询 */
    else
    {
        pstFCtMainInfo->ulCurCmdStepsFlag |= CT_F_STEPS_DONE_FTXWAVE;
        pstFCtMainInfo->ucCurWaveType      = (VOS_UINT8)stReq.usWaveType;
    }

    return ;
}

/*****************************************************************************
 函 数 名  : at_ftm_ftxwave_setcnf
 功能描述  : 保存参数
 输入参数  : ulErrCode 错误码
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
static VOS_UINT32 at_ftm_ftxwave_setcnf(VOS_UINT32 ulErrCode)
{
    FTM_SET_F_FTXWAVE_CNF_STRU stCnf = { 0 };

    stCnf.ulErrCode = ulErrCode;

    return ftm_comm_send(ID_MSG_FTM_F_SET_FTXWAVE_CNF,
        (VOS_UINT32)&stCnf, sizeof(FTM_SET_F_FTXWAVE_CNF_STRU));
}

/*****************************************************************************
 函 数 名  : at_ftm_ftxwave_read
 功能描述  : ID_MSG_FTM_F_RD_FTXWAVE_REQ 处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_ftxwave_read(VOS_VOID* pParam)
{
    FTM_RD_F_FTXWAVE_CNF_STRU stRdCnf         = { 0 };
    FTM_CT_F_MANAGE_INFO_STRU *pstFCtMainInfo = ftm_GetFastCTMainInfo();

    stRdCnf.ulErrCode  = ERR_MSP_SUCCESS;
    stRdCnf.usWaveType = pstFCtMainInfo->ucCurWaveType;

    return ftm_comm_send(ID_MSG_FTM_F_RD_FTXWAVE_CNF, (VOS_UINT32)&stRdCnf, sizeof(FTM_RD_F_FTXWAVE_CNF_STRU));
}
static VOS_UINT8 ftxwave_is_para_correct(VOS_INT16 wave)
{
    /*lint -e685*/
	/*lint -e568*/
	if (((wave >= 0) && (wave <= 6))||(wave == 11))
	/*lint +e568*/
	/*lint +e685*/	
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*****************************************************************************
 函 数 名  : at_ftm_ftxwave_set
 功能描述  : ID_MSG_FTM_F_SET_FTXWAVE_REQ 处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_ftxwave_set(VOS_VOID* pParam)
{
    FTM_SET_F_FTXWAVE_REQ_STRU* pstFtmReq      = (FTM_SET_F_FTXWAVE_REQ_STRU*)pParam;
    OM_PHY_CT_F_TXWAVE_SET_REQ_STRU stReqToDsp = { 0 };
    FCHAN_MODE_ENUM fchanMode = ftm_GetFastCTMainInfo()->enCurMode;
    /* 判断是否处于非信令模式下 */
    if (ftm_GetNoneSig() != FTM_NONESIG_RTT_OPEN)
    {
        return at_ftm_ftxwave_setcnf(ERR_MSP_UE_MODE_ERR);
    }

    /* 发送原语到DSP */
    stReqToDsp.ulMsgId    = OM_PHY_CT_F_TXWAVE_SET_REQ;
    stReqToDsp.usWaveType = pstFtmReq->usWaveType;
	/*lint -save -e713*/
    if(FALSE == ftxwave_is_para_correct(stReqToDsp.usWaveType))
	/*lint -restore*/	
    {
        return at_ftm_ftxwave_setcnf(ERR_MSP_INVALID_PARAMETER);
    }
    if(ERR_MSP_SUCCESS != ftm_mailbox_ct_write(fchanMode, &stReqToDsp, sizeof(OM_PHY_CT_F_TXWAVE_SET_REQ_STRU)))
    {
        return at_ftm_ftxwave_setcnf(ERR_MSP_UNKNOWN);
    }

    at_ftm_ftxwave_store(FTM_F_CT_STORE_REQ_TEMP, pstFtmReq);
    return ERR_MSP_WAIT_ASYNC;
}

/*****************************************************************************
 函 数 名  : at_ftm_ftxwave_timeout
 功能描述  : 超时处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_ftxwave_timeout()
{
    return at_ftm_ftxwave_setcnf(ERR_MSP_TIME_OUT);
}

/*****************************************************************************
 函 数 名  : at_ftm_ftxwave_dspcnf
 功能描述  : DSP原语PHY_OM_CT_F_TXWAVE_SET_CNF处理函数
 输入参数  : pParam PHY_OM_CT_F_TXWAVE_SET_CNF_STRU指针
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_ftxwave_dspcnf(VOS_VOID* pParam)
{

    at_ftm_ftxwave_store(FTM_F_CT_STORE_REQ_GLOBAL, NULL);

    return at_ftm_ftxwave_setcnf(ERR_MSP_SUCCESS);
}

/*****************************************************************************
 函 数 名  : at_ftm_fstart_store
 功能描述  : 保存参数
 输入参数  : ucStoreType 保存类型
               FTM_F_CT_STORE_REQ_TEMP 收到装备参数设置指令，未收到LRTT确认原语，只缓存参数
               FTM_F_CT_STORE_REQ_GLOBAL 收到LRTT确认原语，保存参数，供装备查询
             pstReq  参数
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
static VOID at_ftm_fstart_store(VOS_UINT8 ucStoreType, FTM_SET_F_TRIGGER_REQ_STRU* pstReq)
{
    static FTM_SET_F_TRIGGER_REQ_STRU stReq     = {0};
    FTM_CT_F_MANAGE_INFO_STRU* pstFtmFastCtInfo = ftm_GetFastCTMainInfo();

    /* 收到装备参数设置指令，未收到LRTT确认原语，只缓存参数*/
    if((FTM_F_CT_STORE_REQ_TEMP==ucStoreType) && (NULL != pstReq))
    {
        MSP_MEMCPY(&stReq, pstReq, sizeof(FTM_SET_F_TRIGGER_REQ_STRU));
    }
    /* 收到LRTT确认原语，保存参数，供装备查询 */
    else
    {
         pstFtmFastCtInfo->ulCurCmdStepsFlag |= CT_F_STEPS_DONE_FSTART;
         pstFtmFastCtInfo->ucCurTriggerType   = stReq.ucType;
    }

    return ;
}

/*****************************************************************************
 函 数 名  : at_ftm_fstart_setcnf
 功能描述  : 保存参数
 输入参数  : ulErrCode 错误码
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
static VOS_UINT32 at_ftm_fstart_setcnf(VOS_UINT32 ulErrCode)
{
    FTM_SET_F_TRIGGER_CNF_STRU stCnf = { 0 };

    stCnf.ulErrCode = ulErrCode;

    return ftm_comm_send(ID_MSG_FTM_F_SET_FSTART_CNF,
        (VOS_UINT32)&stCnf, sizeof(FTM_SET_F_TRIGGER_CNF_STRU));
}

/**********************************************************************************

处理依据1:

    该命令使用在AT^FCHANS下发成功之后，
    发射通道的校准在AT^FPOWS、AT^FPAS、AT^FSEGMENT命令下发成功之后，
    接收通道的校准在AT^FLNAS、AT^FSEGMENT命令下发成功之后。

处理依据2:

    MSP对AT^FCHANS的判断条件应该如下：

    a)、如果AT^FCHANS设置了上行信道，没有设置下行信道；校准类型我们只能是0，是1或2时返回错误；
    b)、如果AT^FCHANS设置了下行信道，没有设置上行信道；则校准类型0、1或2时均返回错误；
    c)、如果AT^FCHANS同时设置了上行/下行信道;校准类型我们可以是0、1、2；

校准类型:

    0   发送通道校准
    1   接收通道校准（接收默认为双通道）
    2   发射/接收通道同时校准（接收默认为双通道）

***********************************************************************************/
static VOS_BOOL at_ftm_fstart_setparaIsCorrect(FTM_SET_F_TRIGGER_REQ_STRU* pstFtmReq)
{
    VOS_UINT8 bRet = TRUE;
	FTM_CT_F_CMD_STEPS_FLAG_ENUM_UINT32 ulStepFlag = 0;

    if ( START_TRIGGER_TYPE_TX == pstFtmReq->ucType)
    {
        /* 0x1E 等于 CT_F_STEPS_DONE_FCHANS | CT_F_STEPS_DONE_FSEGMENT |CT_F_STEPS_DONE_FPOWS |CT_F_STEPS_DONE_FPAS */
        ulStepFlag = CT_F_STEPS_DONE_FSEGMENT |CT_F_STEPS_DONE_FCHANS|CT_F_STEPS_DONE_FPAS;
        if (ftm_CheckCmdSteps(ulStepFlag) == FALSE)
        {
            bRet = FALSE;
        }

    }
    else if( START_TRIGGER_TYPE_RX == pstFtmReq->ucType)
    {
        /* 0x3E 等于 CT_F_STEPS_DONE_FCHANS | CT_F_STEPS_DONE_FSEGMENT |CT_F_STEPS_DONE_FPOWS |CT_F_STEPS_DONE_FPAS|CT_F_STEPS_DONE_FLNAS */
        /* Added by daizhicheng without CT_F_STEPS_DONE_FPOWS*/
        ulStepFlag = CT_F_STEPS_DONE_FSEGMENT |CT_F_STEPS_DONE_FCHANS|CT_F_STEPS_DONE_FPAS | CT_F_STEPS_DONE_FLNAS;
        if (ftm_CheckCmdSteps(ulStepFlag) == FALSE)
        {
            bRet = FALSE;
        }
    }
    else if( START_TRIGGER_TYPE_TXRX == pstFtmReq->ucType)
    {
        /* 0x3E 等于 CT_F_STEPS_DONE_FCHANS | CT_F_STEPS_DONE_FSEGMENT |CT_F_STEPS_DONE_FPOWS |CT_F_STEPS_DONE_FPAS|CT_F_STEPS_DONE_FLNAS */
        /* Added by daizhicheng without CT_F_STEPS_DONE_FPOWS*/
        ulStepFlag = CT_F_STEPS_DONE_FSEGMENT |CT_F_STEPS_DONE_FCHANS|CT_F_STEPS_DONE_FPAS | CT_F_STEPS_DONE_FLNAS;
        if (ftm_CheckCmdSteps(ulStepFlag) == FALSE)
        {
            bRet = FALSE;
        }
    }
    else if(START_TRIGGER_TYPE_MAXTX == pstFtmReq->ucType)
	{
	    ulStepFlag = CT_F_STEPS_DONE_FTXWAVE|CT_F_STEPS_DONE_FSEGMENT
			        |CT_F_STEPS_DONE_FCHANS  |CT_F_STEPS_DONE_FPAS;
	    if(ftm_CheckCmdSteps(ulStepFlag) == FALSE)
	    {
	        bRet = FALSE;
	    }

	}
    else
    {
        /* 非法校准类型 */
        bRet = FALSE;
    }

    return bRet;
}

/*****************************************************************************
 函 数 名  : at_ftm_fstart_set
 功能描述  : ID_MSG_FTM_F_SET_FSTART_REQ 处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fstart_set(VOS_VOID* pParam)
{
    FTM_SET_F_TRIGGER_REQ_STRU* pstFtmReq       = (FTM_SET_F_TRIGGER_REQ_STRU*)pParam;
    OM_PHY_CT_F_TRIGGER_SET_REQ_STRU stReqToDsp = { 0 };
    FCHAN_MODE_ENUM fchanMode = ftm_GetFastCTMainInfo()->enCurMode;
    /* 判断是否处于非信令模式下 */
    if(ftm_GetNoneSig() != FTM_NONESIG_RTT_OPEN)
    {
        return at_ftm_fstart_setcnf(ERR_MSP_UE_MODE_ERR);
    }
#if (1)
    /* 判断参数是否正确 */
    if(at_ftm_fstart_setparaIsCorrect(pstFtmReq) == FALSE)
    {
        return at_ftm_fstart_setcnf(ERR_MSP_INVALID_PARAMETER);
    }
#endif
    /* 发送原语到DSP */
    stReqToDsp.ulMsgId = OM_PHY_CT_F_TRIGGER_SET_REQ;
    stReqToDsp.ucType  = pstFtmReq->ucType;
    if(ERR_MSP_SUCCESS != ftm_mailbox_ct_write(fchanMode, &stReqToDsp, sizeof(OM_PHY_CT_F_TRIGGER_SET_REQ_STRU)))
    {
        return at_ftm_fstart_setcnf(ERR_MSP_UNKNOWN);
    }
    at_ftm_fstart_store(FTM_F_CT_STORE_REQ_TEMP, pstFtmReq);
    return ERR_MSP_WAIT_ASYNC;
}

/*****************************************************************************
 函 数 名  : at_ftm_fstart_timeout
 功能描述  : 超时处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fstart_timeout()
{
    return at_ftm_fstart_setcnf(ERR_MSP_TIME_OUT);
}

/*****************************************************************************
 函 数 名  : at_ftm_fstart_dspcnf
 功能描述  : DSP原语PHY_OM_CT_F_TRIGGER_SET_CNF处理函数
 输入参数  : pParam PHY_OM_CT_F_TRIGGER_SET_CNF_STRU指针
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fstart_dspcnf(VOS_VOID* pParam)
{

    at_ftm_fstart_store(FTM_F_CT_STORE_REQ_GLOBAL, NULL);

    return at_ftm_fstart_setcnf(ERR_MSP_SUCCESS);
}

/*****************************************************************************
 函 数 名  : at_ftm_fstart_readcnf
 功能描述  : 保存参数
 输入参数  : ulErrCode 错误码
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
static VOS_UINT32 at_ftm_fstart_readcnf(VOS_UINT8 ucType, VOS_UINT8 ucStatus, VOS_UINT32 ulErrCode)
{
    FTM_RD_F_TRIGGER_CNF_STRU stCnf = { 0 };

    stCnf.ulErrCode = ulErrCode;
    stCnf.ucType    = ucType;
    stCnf.ucStatus  = ucStatus;

    return ftm_comm_send(ID_MSG_FTM_F_RD_FSTART_CNF,
        (VOS_UINT32)&stCnf, sizeof(FTM_RD_F_TRIGGER_CNF_STRU));
}

/*****************************************************************************
 函 数 名  : at_ftm_fstart_readtimeout
 功能描述  : 超时处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fstart_read_timeout()
{
    return at_ftm_fstart_readcnf(0, 0, ERR_MSP_TIME_OUT);
}

/*****************************************************************************
 函 数 名  : at_ftm_fstart_read
 功能描述  : ID_MSG_FTM_F_RD_FSTART_REQ 处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fstart_read(VOS_VOID* pParam)
{
    OM_PHY_CT_F_TRIGGER_RD_REQ_STRU stReqToDsp  = { 0 };
    FCHAN_MODE_ENUM fchanMode = ftm_GetFastCTMainInfo()->enCurMode;
    /* 判断是否处于非信令模式下 */
    if(ftm_GetNoneSig() != FTM_NONESIG_RTT_OPEN)
    {
        return at_ftm_fstart_readcnf(0, 0, ERR_MSP_UE_MODE_ERR);
    }
    /* 发送原语到DSP */
    stReqToDsp.ulMsgId = OM_PHY_CT_F_TRIGGER_RD_REQ;
    if(ERR_MSP_SUCCESS != ftm_mailbox_ct_write(fchanMode, &stReqToDsp, sizeof(OM_PHY_CT_F_TRIGGER_RD_REQ_STRU)))
    {
        return at_ftm_fstart_readcnf(0, 0, ERR_MSP_UNKNOWN);
    }
    return ERR_MSP_WAIT_ASYNC;
}

/*****************************************************************************
 函 数 名  : at_ftm_fstart_readdspcnf
 功能描述  : DSP原语PHY_OM_CT_F_TRIGGER_RD_CNF处理函数
 输入参数  : pParam PHY_OM_CT_F_TRIGGER_RD_CNF_STRU指针
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fstart_read_dspcnf(VOS_VOID* pParam)
{
    PHY_OM_CT_F_TRIGGER_RD_CNF_STRU* pDspCnf = (PHY_OM_CT_F_TRIGGER_RD_CNF_STRU*)pParam;

    return at_ftm_fstart_readcnf(pDspCnf->ucType, pDspCnf->ucStatus, ERR_MSP_SUCCESS);
}

/*****************************************************************************
 函 数 名  : at_ftm_frssis_set
 功能描述  : ID_MSG_FTM_F_SET_FPAS_REQ 处理函数
             判断DSP是否主动上报测量结果，如果上报则将拷贝全局缓存并将结果返回，未上报则返回错误信息
 输入参数  : pParam FTM_SET_F_FRSSIS_REQ_STRU指针
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_frssis_set(VOS_VOID* pParam)
{
    VOS_BOOL bFreqExist = FALSE;
    VOS_UINT16 usTmp       = 0;

    FTM_SET_F_FRSSIS_CNF_STRU stCnf             = { 0 };
    FTM_SET_F_FRSSIS_REQ_STRU* pstFtmReq        = (FTM_SET_F_FRSSIS_REQ_STRU*)(pParam);
    FTM_CT_F_MANAGE_INFO_STRU* pstFtmFastCtInfo = ftm_GetFastCTMainInfo();

    /* 判断是否处于非信令模式下 */
    if(ftm_GetNoneSig() != FTM_NONESIG_RTT_OPEN)
    {
        stCnf.ulErrCode = ERR_MSP_UE_MODE_ERR;

        return ftm_comm_send(ID_MSG_FTM_F_SET_FRSSIS_CNF,
            (VOS_UINT32)&stCnf, sizeof(FTM_SET_F_FRSSIS_CNF_STRU));
    }

    /* 判断参数是否正确 */
    if(ftm_CheckCmdSteps(CT_F_STEPS_DONE_FRSSIS_IND) != TRUE )
    {
        stCnf.ulErrCode = ERR_MSP_INVALID_PARAMETER;

        return ftm_comm_send(ID_MSG_FTM_F_SET_FRSSIS_CNF,
            (VOS_UINT32)&stCnf, sizeof(FTM_SET_F_FRSSIS_CNF_STRU));
    }

    /* 判断下行行道是否存在于用户已配置的下行信道数组中 */
    for(usTmp = 0 ; usTmp < pstFtmFastCtInfo->usCurDlListNum ; usTmp ++)
    {
        if (pstFtmFastCtInfo->ausCurDlFreqInfo[usTmp] == pstFtmReq->usDLFreq)
        {
            bFreqExist = TRUE;
            break; /* 找到第一个匹配的下行信道号就返回 */
        }
    }

    if(bFreqExist == FALSE)
    {
       stCnf.ulErrCode = ERR_MSP_NOT_SET_CHAN;

       return ftm_comm_send(ID_MSG_FTM_F_SET_FRSSIS_CNF,
        (VOS_UINT32)&stCnf, sizeof(FTM_SET_F_FRSSIS_CNF_STRU));
    }

    /* 拷贝全局变量中的结果，用于填充响应结构，并发出 */
    stCnf.ulErrCode    = ERR_MSP_SUCCESS;
    /*Modify by daizhicheng for using at^flnas channel number */
    //stCnf.usRSSIPowNum = pstFtmFastCtInfo->usCurTxPowerNum;
    stCnf.usRSSIPowNum = pstFtmFastCtInfo->usCurAagcNum;
    MSP_MEMCPY(stCnf.ausRxANT1RSSI, pstFtmFastCtInfo->ausRxANT1RSSI[usTmp], sizeof(stCnf.ausRxANT1RSSI));
    MSP_MEMCPY(stCnf.ausRxANT2RSSI, pstFtmFastCtInfo->ausRxANT2RSSI[usTmp], sizeof(stCnf.ausRxANT2RSSI));

    return ftm_comm_send(ID_MSG_FTM_F_SET_FRSSIS_CNF,
        (VOS_UINT32)&stCnf, sizeof(FTM_SET_F_FRSSIS_CNF_STRU));
}

/*****************************************************************************
 函 数 名  : at_ftm_frssis_dspind
 功能描述  : DSP原语PHY_OM_CT_F_MEAS_RSSI_IND处理函数
             处理DSP主动上报的RSSI测量值:将测量结果保存在全局缓存中
             测量结束后，该消息主动上报一次,所有频点测量结果同时上报
 输入参数  : pParam PHY_OM_CT_F_MEAS_RSSI_IND_STRU指针
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_frssis_dspind(VOS_VOID* pParam)
{
    VOS_UINT16 usTmpi = 0;
    VOS_UINT16 usTmpj = 0;
    CT_F_ANT_RSSI_STRU* pstRSSI = NULL;

    PHY_OM_CT_F_MEAS_RSSI_IND_STRU* pstRttRssi  = (PHY_OM_CT_F_MEAS_RSSI_IND_STRU*)pParam;
    FTM_CT_F_MANAGE_INFO_STRU* pstFtmFastCtInfo = ftm_GetFastCTMainInfo();

    HAL_SDMLOG("\n ENTER at_ftm_frssis_dspind, Addr: 0x%x, ulMsgId: 0x%x \n", (VOS_UINT32)pstRttRssi, pstRttRssi->ulMsgId);

    pstFtmFastCtInfo->ulCurCmdStepsFlag |= CT_F_STEPS_DONE_FRSSIS_IND;

    for(usTmpi = 0 ; usTmpi < FREQ_MAX_NUM ; usTmpi ++)
    {
        for(usTmpj = 0 ; usTmpj < POW_MAX_NUM ; usTmpj ++)
        {
            pstRSSI = &(pstRttRssi->astRxANTRSSI[usTmpi][usTmpj]);

            /* 强制覆盖之前保存在全局结构中的RSSI测量值 */
            pstFtmFastCtInfo->ausRxANT1RSSI[usTmpi][usTmpj] = (VOS_UINT16)(pstRSSI->sRxANT1RSSI);
            pstFtmFastCtInfo->ausRxANT2RSSI[usTmpi][usTmpj] = (VOS_UINT16)(pstRSSI->sRxANT2RSSI);
        }
    }

    return ERR_MSP_SUCCESS;
}

/*****************************************************************************
 函 数 名  : at_ftm_fcmtms_read
 功能描述  : ID_MSG_FTM_F_RD_FCMTMS_REQ 处理函数
             从全局缓存中读取温度值列表，并发送
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fcmtms_read(VOS_VOID* pParam)
{
    FTM_RD_F_FCMTMS_CNF_STRU stRdCnf            = { 0 };
    FTM_CT_F_MANAGE_INFO_STRU* pstFtmFastCtInfo = ftm_GetFastCTMainInfo();

    stRdCnf.ulErrCode  = ERR_MSP_SUCCESS;
    stRdCnf.usCmtmsNum = pstFtmFastCtInfo->usCmtmsNum;
    MSP_MEMCPY(stRdCnf.ausCmtms, pstFtmFastCtInfo->ausCmtms, sizeof(stRdCnf.ausCmtms));

    return ftm_comm_send(ID_MSG_FTM_F_RD_FCMTMS_CNF, (VOS_UINT32)&stRdCnf, sizeof(FTM_RD_F_FCMTMS_CNF_STRU));
}

/*****************************************************************************
 函 数 名  : at_ftm_fcmtms_dspind
 功能描述  : DSP原语PHY_OM_CT_F_MEAS_CMTMS_IND处理函数
             将温度值列表保存到全局缓存中
 输入参数  : pParam PHY_OM_CT_F_MEAS_CMTMS_IND_STRU指针
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fcmtms_dspind(VOS_VOID* pParam)
{
    PHY_OM_CT_F_MEAS_CMTMS_IND_STRU* pDspInd  = (PHY_OM_CT_F_MEAS_CMTMS_IND_STRU*)pParam;
    FTM_CT_F_MANAGE_INFO_STRU *pstFCtMainInfo = ftm_GetFastCTMainInfo();

    pstFCtMainInfo->ulCurCmdStepsFlag |= CT_F_STEPS_DONE_FCMTMS_IND; /* 保存已成功上报标志 */
	pstFCtMainInfo->usCmtmsNum         = pDspInd->usCmtmsNum;
	MSP_MEMCPY(pstFCtMainInfo->ausCmtms, pDspInd->ausCmtms, sizeof(pstFCtMainInfo->ausCmtms));

    return ERR_MSP_SUCCESS;
}

/*****************************************************************************
 函 数 名  : at_ftm_fmaxpower_store
 功能描述  : 保存参数
 输入参数  : ucStoreType 保存类型
               FTM_F_CT_STORE_REQ_TEMP 收到装备参数设置指令，未收到LRTT确认原语，只缓存参数
               FTM_F_CT_STORE_REQ_GLOBAL 收到LRTT确认原语，保存参数，供装备查询
             pstReq  参数
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
static VOID at_ftm_fmaxpower_store(VOS_UINT8 ucStoreType, FTM_SET_F_FMAXPOWER_REQ_STRU* pstReq)
{
    static FTM_SET_F_FMAXPOWER_REQ_STRU stReq   = {0};
    FTM_CT_F_MANAGE_INFO_STRU* pstFtmFastCtInfo = ftm_GetFastCTMainInfo();

    /* 收到装备参数设置指令，未收到LRTT确认原语，只缓存参数*/
    if((FTM_F_CT_STORE_REQ_TEMP==ucStoreType) && (NULL != pstReq))
    {
        MSP_MEMCPY(&stReq, pstReq, sizeof(FTM_SET_F_FMAXPOWER_REQ_STRU));
    }
    /* 收到LRTT确认原语，保存参数，供装备查询 */
    else
    {
        pstFtmFastCtInfo->ulCurCmdStepsFlag         |= CT_F_STEPS_DONE_FMAXPOWER;
        pstFtmFastCtInfo->ulFmaxpower_usPaLevelNum   = stReq.usPaLevelNum;
		pstFtmFastCtInfo->ulFmaxpower_usPaReduceGain = stReq.usPaReduceGain;
    }

    return ;
}

/*****************************************************************************
 函 数 名  : at_ftm_fmaxpower_setcnf
 功能描述  : 返回ID_MSG_FTM_F_SET_FMAXPOWER_CNF
 输入参数  : ulErrCode 错误码
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
static VOS_UINT32 at_ftm_fmaxpower_setcnf(VOS_UINT32 ulErrCode)
{
    FTM_SET_F_FMAXPOWER_CNF_STRU stCnf = { 0 };

    stCnf.ulErrCode = ulErrCode;

    return ftm_comm_send(ID_MSG_FTM_F_SET_FMAXPOWER_CNF,
        (VOS_UINT32)&stCnf, sizeof(FTM_SET_F_FMAXPOWER_CNF_STRU));
}

/*****************************************************************************
 函 数 名  : at_ftm_fmaxpower_read
 功能描述  : ID_MSG_FTM_F_RD_FMAXPOWER_REQ 处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fmaxpower_read(VOS_VOID* pParam)
{
    FTM_RD_F_FMAXPOWER_CNF_STRU stRdCnf         = { 0 };
    FTM_CT_F_MANAGE_INFO_STRU* pstFtmFastCtInfo = ftm_GetFastCTMainInfo();

    stRdCnf.ulErrCode      = ERR_MSP_SUCCESS;
    stRdCnf.usPaLevelNum   = (VOS_UINT16)pstFtmFastCtInfo->ulFmaxpower_usPaLevelNum;
	stRdCnf.usPaReduceGain = (VOS_UINT16)pstFtmFastCtInfo->ulFmaxpower_usPaReduceGain;

    return ftm_comm_send(ID_MSG_FTM_F_RD_FMAXPOWER_CNF, (VOS_UINT32)&stRdCnf, sizeof(FTM_RD_F_FMAXPOWER_CNF_STRU));
}

/*****************************************************************************
 函 数 名  : at_ftm_fmaxpower_set
 功能描述  : ID_MSG_FTM_F_SET_FPAPOWER_REQ 处理函数
             向DSP发送OM_PHY_CT_F_MAX_POWER_SET_REQ原语
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fmaxpower_set(VOS_VOID* pParam)
{
    FTM_SET_F_FMAXPOWER_REQ_STRU* pstFtmReq       = (FTM_SET_F_FMAXPOWER_REQ_STRU*)pParam;
    OM_PHY_CT_F_MAX_POWER_SET_REQ_STRU stReqToDsp = { 0 };

    /* 判断是否处于非信令模式下 */
    if(ftm_GetNoneSig() != FTM_NONESIG_RTT_OPEN)
    {
        return at_ftm_fmaxpower_setcnf(ERR_MSP_UE_MODE_ERR);
    }

    /* 发送原语到DSP */
    stReqToDsp.ulMsgId        = OM_PHY_CT_F_MAX_POWER_SET_REQ;
    stReqToDsp.usPaLevelNum   = pstFtmReq->usPaLevelNum;
    stReqToDsp.usPaReduceGain = pstFtmReq->usPaReduceGain;
    if(ERR_MSP_SUCCESS != ftm_mailbox_ltect_write(&stReqToDsp, sizeof(OM_PHY_CT_F_MAX_POWER_SET_REQ_STRU)))
    {
        return at_ftm_fmaxpower_setcnf(ERR_MSP_UNKNOWN);
    }

    at_ftm_fmaxpower_store(FTM_F_CT_STORE_REQ_TEMP, pstFtmReq);
    return ERR_MSP_WAIT_ASYNC;
}

/*****************************************************************************
 函 数 名  : at_ftm_fmaxpower_timeout
 功能描述  : 超时处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fmaxpower_timeout()
{
    return at_ftm_fmaxpower_setcnf(ERR_MSP_TIME_OUT);
}

/*****************************************************************************
 函 数 名  : at_ftm_fmaxpower_dspcnf
 功能描述  : DSP原语PHY_OM_CT_F_MAX_POWER_SET_CNF处理函数
 输入参数  : pParam PHY_OM_CT_F_MAX_POWER_SET_CNF_STRU指针
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fmaxpower_dspcnf(VOS_VOID* pParam)
{

    at_ftm_fmaxpower_store(FTM_F_CT_STORE_REQ_GLOBAL, NULL);

    return at_ftm_fmaxpower_setcnf(ERR_MSP_SUCCESS);
}

/*****************************************************************************
 函 数 名  : at_ftm_fpapower_store
 功能描述  : 保存参数
 输入参数  : ucStoreType 保存类型
               FTM_F_CT_STORE_REQ_TEMP 收到装备参数设置指令，未收到LRTT确认原语，只缓存参数
               FTM_F_CT_STORE_REQ_GLOBAL 收到LRTT确认原语，保存参数，供装备查询
             pstReq  参数
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
static VOID at_ftm_fpapower_store(VOS_UINT8 ucStoreType, FTM_SET_F_FPAPOWER_REQ_STRU* pstReq)
{
    static FTM_SET_F_FPAPOWER_REQ_STRU stReq    = {0};
    FTM_CT_F_MANAGE_INFO_STRU* pstFtmFastCtInfo = ftm_GetFastCTMainInfo();

    /* 收到装备参数设置指令，未收到LRTT确认原语，只缓存参数*/
    if((FTM_F_CT_STORE_REQ_TEMP==ucStoreType) && (NULL != pstReq))
    {
        MSP_MEMCPY(&stReq, pstReq, sizeof(FTM_SET_F_FPAPOWER_REQ_STRU));
    }
    /* 收到LRTT确认原语，保存参数，供装备查询 */
    else
    {
        pstFtmFastCtInfo->ulCurCmdStepsFlag      |= CT_F_STEPS_DONE_FPAPOWER;
        pstFtmFastCtInfo->ulFpapower_usPaLeverNum = stReq.usPaLeverNum;
        MSP_MEMCPY(pstFtmFastCtInfo->usFpapower_list, stReq.usMaxPowerList, sizeof(pstFtmFastCtInfo->usFpapower_list));
    }

    return ;
}

/*****************************************************************************
 函 数 名  : at_ftm_fpapower_setcnf
 功能描述  : 返回ID_MSG_FTM_F_SET_FPAPOWER_CNF
 输入参数  : ulErrCode 错误码
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
static VOS_UINT32 at_ftm_fpapower_setcnf(VOS_UINT32 ulErrCode)
{
    FTM_SET_F_FPAPOWER_CNF_STRU stCnf = { 0 };

    stCnf.ulErrCode = ulErrCode;

    return ftm_comm_send(ID_MSG_FTM_F_SET_FPAPOWER_CNF,
        (VOS_UINT32)&stCnf, sizeof(FTM_SET_F_FPAPOWER_CNF_STRU));
}

/*****************************************************************************
 函 数 名  : at_ftm_fpapower_read
 功能描述  : ID_MSG_FTM_F_RD_FPAPOWER_REQ 处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fpapower_read(VOS_VOID* pParam)
{
    FTM_RD_F_FPAPOWER_CNF_STRU stRdCnf          = { 0 };
    FTM_CT_F_MANAGE_INFO_STRU* pstFtmFastCtInfo = ftm_GetFastCTMainInfo();

    stRdCnf.ulErrCode    = ERR_MSP_SUCCESS;
    stRdCnf.usPaLeverNum = (VOS_UINT16)(pstFtmFastCtInfo->ulFpapower_usPaLeverNum);
    MSP_MEMCPY(stRdCnf.usMaxPowerList, pstFtmFastCtInfo->usFpapower_list, sizeof(stRdCnf.usMaxPowerList));

    return ftm_comm_send(ID_MSG_FTM_F_RD_FPAPOWER_CNF, (VOS_UINT32)&stRdCnf, sizeof(FTM_RD_F_FPAPOWER_CNF_STRU));
}

/*****************************************************************************
 函 数 名  : at_ftm_fpapower_set
 功能描述  : ID_MSG_FTM_F_SET_FPAPOWER_REQ 处理函数
             向DSP发送OM_PHY_CT_F_PA_POWER_SET_REQ原语
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fpapower_set(VOS_VOID* pParam)
{
    FTM_SET_F_FPAPOWER_REQ_STRU* pstFtmReq       = (FTM_SET_F_FPAPOWER_REQ_STRU*)pParam;
    OM_PHY_CT_F_PA_POWER_SET_REQ_STRU stReqToDsp = { 0 };

    /* 判断是否处于非信令模式下 */
    if(ftm_GetNoneSig() != FTM_NONESIG_RTT_OPEN)
    {
        return at_ftm_fpapower_setcnf(ERR_MSP_UE_MODE_ERR);
    }

    /* 发送原语到DSP */
    stReqToDsp.ulMsgId      = OM_PHY_CT_F_PA_POWER_SET_REQ;
    stReqToDsp.usPaLevelNum = pstFtmReq->usPaLeverNum;
    MSP_MEMCPY(stReqToDsp.asMaxPwrList, pstFtmReq->usMaxPowerList, sizeof(stReqToDsp.asMaxPwrList));
    if(ERR_MSP_SUCCESS != ftm_mailbox_ltect_write(&stReqToDsp, sizeof(OM_PHY_CT_F_PA_POWER_SET_REQ_STRU)))
    {
        return at_ftm_fpapower_setcnf(ERR_MSP_UNKNOWN);
    }

    at_ftm_fpapower_store(FTM_F_CT_STORE_REQ_TEMP, pstFtmReq);
    return ERR_MSP_WAIT_ASYNC;
}

/*****************************************************************************
 函 数 名  : at_ftm_fpapower_timeout
 功能描述  : 超时处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fpapower_timeout()
{
    return at_ftm_fpapower_setcnf(ERR_MSP_TIME_OUT);
}

/*****************************************************************************
 函 数 名  : at_ftm_fpapower_dspcnf
 功能描述  : DSP原语PHY_OM_CT_F_PA_POWER_SET_CNF处理函数
 输入参数  : pParam PHY_OM_CT_F_PA_POWER_SET_CNF_STRU指针
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fpapower_dspcnf(VOS_VOID* pParam)
{

    at_ftm_fpapower_store(FTM_F_CT_STORE_REQ_GLOBAL, NULL);

    return at_ftm_fpapower_setcnf(ERR_MSP_SUCCESS);
}

/*****************************************************************************
 函 数 名  : at_ftm_fcaliip2s_store
 功能描述  : 保存参数
 输入参数  : ucStoreType 保存类型
               FTM_F_CT_STORE_REQ_TEMP 收到装备参数设置指令，未收到LRTT确认原语，只缓存参数
               FTM_F_CT_STORE_REQ_GLOBAL 收到LRTT确认原语，保存参数，供装备查询
             pstReq  参数
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
static VOID at_ftm_fcaliip2s_store(VOS_UINT8 ucStoreType, FTM_SET_FCALIIP2S_REQ_STRU* pstReq)
{
    static FTM_SET_FCALIIP2S_REQ_STRU stReq     = { 0 };
    FTM_CT_F_MANAGE_INFO_STRU* pstFtmFastCtInfo = ftm_GetFastCTMainInfo();

    /* 收到装备参数设置指令，未收到LRTT确认原语，只缓存参数*/
    if((FTM_F_CT_STORE_REQ_TEMP==ucStoreType) && (NULL != pstReq))
    {
        MSP_MEMCPY(&stReq, pstReq, sizeof(FTM_SET_FCALIIP2S_REQ_STRU));
    }
    /* 收到LRTT确认原语，保存参数，供装备查询 */
    else
    {
        MSP_MEMCPY(&pstFtmFastCtInfo->stFCaliIp2sSetReq, &stReq, sizeof(FTM_SET_FCALIIP2S_REQ_STRU));

        /* 设置查询信息  */
        MSP_MEMSET(&pstFtmFastCtInfo->stFCaliIp2sRdCnf, 0, sizeof(pstFtmFastCtInfo->stFCaliIp2sRdCnf));
        pstFtmFastCtInfo->stFCaliIp2sRdCnf.ulErrCode = ERR_MSP_SUCCESS;
        pstFtmFastCtInfo->stFCaliIp2sRdCnf.usStatus  = FTM_STATUS_EXECUTING;
        pstFtmFastCtInfo->stFCaliIp2sRdCnf.usChanNum = pstFtmFastCtInfo->stFCaliIp2sSetReq.usChanNum;
        MSP_MEMCPY(pstFtmFastCtInfo->stFCaliIp2sRdCnf.usChan, pstFtmFastCtInfo->stFCaliIp2sSetReq.usChan,
            sizeof(pstFtmFastCtInfo->stFCaliIp2sSetReq.usChan));
    }

    return ;
}

/*****************************************************************************
 函 数 名  : at_ftm_fcaldip2s_setcnf
 功能描述  : 返回ID_MSG_FTM_SET_FCALIIP2S_CNF
 输入参数  : ulErrCode 错误码
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
static VOS_UINT32 at_ftm_fcaliip2s_setcnf(VOS_UINT32 ulErrCode)
{
    FTM_SET_FCALIIP2S_CNF_STRU stCnf = { 0 };

    stCnf.ulErrCode = ulErrCode;

    return ftm_comm_send(ID_MSG_FTM_SET_FCALIIP2S_CNF,
        (VOS_UINT32)&stCnf, sizeof(FTM_SET_FCALIIP2S_CNF_STRU));
}

/*****************************************************************************
 函 数 名  : at_ftm_fcaliip2s_read
 功能描述  : ID_MSG_FTM_RD_FCALIIP2S_REQ 处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fcaliip2s_read(VOS_VOID* pParam)
{
    FTM_CT_F_MANAGE_INFO_STRU* pstFtmFastCtInfo = ftm_GetFastCTMainInfo();

    return ftm_comm_send(ID_MSG_FTM_RD_FCALIIP2S_CNF, (VOS_UINT32)&pstFtmFastCtInfo->stFCaliIp2sRdCnf,
        sizeof(FTM_RD_FCALIIP2S_CNF_STRU));
}

/*****************************************************************************
 函 数 名  : at_ftm_fcaliip2s_set
 功能描述  : ID_MSG_FTM_SET_FCALIIP2S_REQ 处理函数
             向DSP发送OM_PHY_CT_F_CALIIP2_SET_REQ原语
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fcaliip2s_set(VOS_VOID* pParam)
{
    VOS_UINT32 i = 0;
    FTM_SET_FCALIIP2S_REQ_STRU* pstFtmReq       = (FTM_SET_FCALIIP2S_REQ_STRU*)pParam;
    OM_PHY_CT_F_CALIIP2_SET_REQ_STRU stReqToDsp = { 0 };

    FCHAN_MODE_ENUM fchanMode = ftm_GetFastCTMainInfo()->enCurMode;

    /* 判断是否处于非信令模式下 */
    if(ftm_GetNoneSig() != FTM_NONESIG_RTT_OPEN)
    {
        return at_ftm_fcaliip2s_setcnf(ERR_MSP_UE_MODE_ERR);
    }

    /* 判断参数是否有效 */
    if(pstFtmReq->usChanNum > FTM_CALIIP2_MAX_CHAN_NUM)
    {
        return at_ftm_fcaliip2s_setcnf(ERR_MSP_INVALID_PARAMETER);
    }

    /* 发送原语到DSP */
    stReqToDsp.ulMsgId   = OM_PHY_CT_F_CALIIP2_SET_REQ;
    stReqToDsp.usChanNum = pstFtmReq->usChanNum;
    for(i=0; i<pstFtmReq->usChanNum; i++)
    {
        stReqToDsp.usChan[i] = at_ftm_get_freq(fchanMode, pstFtmReq->usChan[i]);
    }

     /* 此原语只发送到LTE邮箱，不管fchanMode */
    if(ERR_MSP_SUCCESS != ftm_mailbox_ltect_write(&stReqToDsp, sizeof(OM_PHY_CT_F_CALIIP2_SET_REQ_STRU)))
    {
        return at_ftm_fcaliip2s_setcnf(ERR_MSP_UNKNOWN);
    }

    at_ftm_fcaliip2s_store(FTM_F_CT_STORE_REQ_TEMP, pstFtmReq);
    return ERR_MSP_WAIT_ASYNC;
}

/*****************************************************************************
 函 数 名  : at_ftm_fcaliip2s_timeout
 功能描述  : 超时处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fcaliip2s_timeout()
{
    return at_ftm_fcaliip2s_setcnf(ERR_MSP_TIME_OUT);
}

/*****************************************************************************
 函 数 名  : at_ftm_fcaliip2s_dspcnf
 功能描述  : DSP原语PHY_OM_CT_F_CALIIP2_SET_CNF处理函数
 输入参数  : pParam PHY_OM_CT_F_CALIIP2_SET_CNF_STRU指针
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fcaliip2s_dspcnf(VOS_VOID* pParam)
{

    at_ftm_fcaliip2s_store(FTM_F_CT_STORE_REQ_GLOBAL, NULL);

    return at_ftm_fcaliip2s_setcnf(ERR_MSP_SUCCESS);
}

/*****************************************************************************
 函 数 名  : at_ftm_fcaliip2s_dspind
 功能描述  : DSP原语PHY_OM_CT_F_CALIIP2_IND处理函数
             将DSP主动上报的信息存入全局缓存中
 输入参数  : pParam PHY_OM_CT_F_CALIIP2_IND_STRU指针
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fcaliip2s_dspind(VOS_VOID* pParam)
{
    PHY_OM_CT_F_CALIIP2_IND_STRU* pstCaliIp2Ind = (PHY_OM_CT_F_CALIIP2_IND_STRU*)pParam;
    FTM_CT_F_MANAGE_INFO_STRU* pstFtmFastCtInfo = ftm_GetFastCTMainInfo();

    pstFtmFastCtInfo->stFCaliIp2sRdCnf.ulErrCode = ERR_MSP_SUCCESS;
    pstFtmFastCtInfo->stFCaliIp2sRdCnf.usStatus  = FTM_STATUS_EXECUTED;
    pstFtmFastCtInfo->stFCaliIp2sRdCnf.usChanNum = pstCaliIp2Ind->usChanNum;
    MSP_MEMCPY(pstFtmFastCtInfo->stFCaliIp2sRdCnf.usMain_I_DivOff, pstCaliIp2Ind->usMain_I_DivOff, sizeof(pstCaliIp2Ind->usMain_I_DivOff));
    MSP_MEMCPY(pstFtmFastCtInfo->stFCaliIp2sRdCnf.usMain_Q_DivOff, pstCaliIp2Ind->usMain_Q_DivOff, sizeof(pstCaliIp2Ind->usMain_Q_DivOff));
    MSP_MEMCPY(pstFtmFastCtInfo->stFCaliIp2sRdCnf.usMain_I_DivOn,  pstCaliIp2Ind->usMain_I_DivOn,  sizeof(pstCaliIp2Ind->usMain_I_DivOn));
    MSP_MEMCPY(pstFtmFastCtInfo->stFCaliIp2sRdCnf.usMain_Q_DivOn,  pstCaliIp2Ind->usMain_Q_DivOn,  sizeof(pstCaliIp2Ind->usMain_Q_DivOn));
    MSP_MEMCPY(pstFtmFastCtInfo->stFCaliIp2sRdCnf.usDiv_I,         pstCaliIp2Ind->usDiv_I,         sizeof(pstCaliIp2Ind->usDiv_I));
    MSP_MEMCPY(pstFtmFastCtInfo->stFCaliIp2sRdCnf.usDiv_Q,         pstCaliIp2Ind->usDiv_Q,         sizeof(pstCaliIp2Ind->usDiv_Q));

    return ERR_MSP_SUCCESS;
}

/*****************************************************************************
 函 数 名  : at_ftm_fcalidcocs_store
 功能描述  : 保存参数
 输入参数  : ucStoreType 保存类型
               FTM_F_CT_STORE_REQ_TEMP 收到装备参数设置指令，未收到LRTT确认原语，只缓存参数
               FTM_F_CT_STORE_REQ_GLOBAL 收到LRTT确认原语，保存参数，供装备查询
             pstReq  参数
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
static VOID at_ftm_fcalidcocs_store(VOS_UINT8 ucStoreType, FTM_SET_FCALIDCOCS_REQ_STRU* pstReq)
{
    static FTM_SET_FCALIDCOCS_REQ_STRU stReq    = {0};
    FTM_CT_F_MANAGE_INFO_STRU* pstFtmFastCtInfo = ftm_GetFastCTMainInfo();

    /* 收到装备参数设置指令，未收到LRTT确认原语，只缓存参数*/
    if((FTM_F_CT_STORE_REQ_TEMP==ucStoreType) && (NULL != pstReq))
    {
        MSP_MEMCPY(&stReq, pstReq, sizeof(FTM_SET_FCALIDCOCS_REQ_STRU));
    }
    /* 收到LRTT确认原语，保存参数，供装备查询 */
    else
    {
        MSP_MEMCPY(&pstFtmFastCtInfo->stFCaliDcocsSetReq, &stReq, sizeof(FTM_SET_FCALIDCOCS_REQ_STRU));

        /* 设置查询信息  */
        MSP_MEMSET(&pstFtmFastCtInfo->stFCaliDcocsRdCnf, 0, sizeof(pstFtmFastCtInfo->stFCaliDcocsRdCnf));
        pstFtmFastCtInfo->stFCaliDcocsRdCnf.ulErrCode = ERR_MSP_SUCCESS;
        pstFtmFastCtInfo->stFCaliDcocsRdCnf.usStatus  = FTM_STATUS_EXECUTING;
        pstFtmFastCtInfo->stFCaliDcocsRdCnf.usChannel = pstFtmFastCtInfo->stFCaliDcocsSetReq.usChannel;
    }

    return ;
}

/*****************************************************************************
 函 数 名  : at_ftm_fcalidcocs_setcnf
 功能描述  : 返回ID_MSG_FTM_SET_FCALIDCOCS_CNF
 输入参数  : ulErrCode 错误码
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
static VOS_UINT32 at_ftm_fcalidcocs_setcnf(VOS_UINT32 ulErrCode)
{
    FTM_SET_FCALIDCOCS_CNF_STRU stCnf = { 0 };

    stCnf.ulErrCode = ulErrCode;

    return ftm_comm_send(ID_MSG_FTM_SET_FCALIDCOCS_CNF,
        (VOS_UINT32)&stCnf, sizeof(FTM_SET_FCALIDCOCS_CNF_STRU));
}

/*****************************************************************************
 函 数 名  : at_ftm_fcalidcocs_read
 功能描述  : ID_MSG_FTM_RD_FCALIDCOCS_REQ 处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fcalidcocs_read(VOS_VOID* pParam)
{
    FTM_CT_F_MANAGE_INFO_STRU* pstFtmFastCtInfo = ftm_GetFastCTMainInfo();

    return ftm_comm_send(ID_MSG_FTM_RD_FCALIDCOCS_CNF, (VOS_UINT32)&pstFtmFastCtInfo->stFCaliDcocsRdCnf,
        sizeof(FTM_RD_FCALIDCOCS_CNF_STRU));
}

/*****************************************************************************
 函 数 名  : at_ftm_fcalidcocs_set
 功能描述  : ID_MSG_FTM_SET_FCALIDCOCS_REQ 处理函数
             向DSP发送OM_PHY_CT_F_CALIDCOCS_SET_REQ原语
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fcalidcocs_set(VOS_VOID* pParam)
{
    FTM_SET_FCALIDCOCS_REQ_STRU* pstFtmReq       = (FTM_SET_FCALIDCOCS_REQ_STRU*)pParam;
    OM_PHY_CT_F_CALIDCOCS_SET_REQ_STRU stReqToDsp= { 0 };

    FCHAN_MODE_ENUM fchanMode = ftm_GetFastCTMainInfo()->enCurMode;

    /* 判断是否处于非信令模式下 */
    if(ftm_GetNoneSig() != FTM_NONESIG_RTT_OPEN)
    {
        return at_ftm_fcalidcocs_setcnf(ERR_MSP_UE_MODE_ERR);
    }

    /* 发送原语到DSP */
    stReqToDsp.ulMsgId   = OM_PHY_CT_F_CALIDCOCS_SET_REQ;
    stReqToDsp.usChannel = at_ftm_get_freq(fchanMode,pstFtmReq->usChannel);

    /* 此原语只发送到LTE邮箱，不管fchanMode */
    if(ERR_MSP_SUCCESS != ftm_mailbox_ltect_write(&stReqToDsp, sizeof(OM_PHY_CT_F_CALIDCOCS_SET_REQ_STRU)))
    {
        return at_ftm_fcalidcocs_setcnf(ERR_MSP_UNKNOWN);
    }

    at_ftm_fcalidcocs_store(FTM_F_CT_STORE_REQ_TEMP, pstFtmReq);
    return ERR_MSP_WAIT_ASYNC;
}

/*****************************************************************************
 函 数 名  : at_ftm_fcalidcocs_timeout
 功能描述  : 超时处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fcalidcocs_timeout()
{
    return at_ftm_fcalidcocs_setcnf(ERR_MSP_TIME_OUT);
}

/*****************************************************************************
 函 数 名  : at_ftm_fcalidcocs_dspcnf
 功能描述  : DSP原语PHY_OM_CT_F_CALIDCOCS_SET_CNF处理函数
 输入参数  : pParam PHY_OM_CT_F_CALIDCOCS_SET_CNF_STRU指针
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fcalidcocs_dspcnf(VOS_VOID* pParam)
{

    at_ftm_fcalidcocs_store(FTM_F_CT_STORE_REQ_GLOBAL, NULL);

    return at_ftm_fcalidcocs_setcnf(ERR_MSP_SUCCESS);
}

/*****************************************************************************
 函 数 名  : at_ftm_fcalidcocs_dspind
 功能描述  : DSP原语PHY_OM_CT_F_CALIDCOCS_IND处理函数
             将DSP主动上报的信息存入全局缓存中
 输入参数  : pParam PHY_OM_CT_F_CALIDCOCS_IND_STRU指针
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fcalidcocs_dspind(VOS_VOID* pParam)
{
    PHY_OM_CT_F_CALIDCOCS_IND_STRU* pstCaliDcocsInd = (PHY_OM_CT_F_CALIDCOCS_IND_STRU*)pParam;
    FTM_CT_F_MANAGE_INFO_STRU* pstFtmFastCtInfo     = ftm_GetFastCTMainInfo();

    pstFtmFastCtInfo->stFCaliDcocsRdCnf.ulErrCode = ERR_MSP_SUCCESS;
    pstFtmFastCtInfo->stFCaliDcocsRdCnf.usStatus  = FTM_STATUS_EXECUTED;

    MSP_MEMCPY(pstFtmFastCtInfo->stFCaliDcocsRdCnf.usBLK_ANT1_I, pstCaliDcocsInd->usBLK_ANT1_I, sizeof(pstCaliDcocsInd->usBLK_ANT1_I));
    MSP_MEMCPY(pstFtmFastCtInfo->stFCaliDcocsRdCnf.usBLK_ANT1_Q, pstCaliDcocsInd->usBLK_ANT1_Q, sizeof(pstCaliDcocsInd->usBLK_ANT1_Q));
    MSP_MEMCPY(pstFtmFastCtInfo->stFCaliDcocsRdCnf.usNOBLK_ANT1_I, pstCaliDcocsInd->usNOBLK_ANT1_I, sizeof(pstCaliDcocsInd->usNOBLK_ANT1_I));
    MSP_MEMCPY(pstFtmFastCtInfo->stFCaliDcocsRdCnf.usNOBLK_ANT1_Q, pstCaliDcocsInd->usNOBLK_ANT1_Q, sizeof(pstCaliDcocsInd->usNOBLK_ANT1_Q));

    MSP_MEMCPY(pstFtmFastCtInfo->stFCaliDcocsRdCnf.usBLK_ANT2_I, pstCaliDcocsInd->usBLK_ANT2_I, sizeof(pstCaliDcocsInd->usBLK_ANT2_I));
    MSP_MEMCPY(pstFtmFastCtInfo->stFCaliDcocsRdCnf.usBLK_ANT2_Q, pstCaliDcocsInd->usBLK_ANT2_Q, sizeof(pstCaliDcocsInd->usBLK_ANT2_Q));
    MSP_MEMCPY(pstFtmFastCtInfo->stFCaliDcocsRdCnf.usNOBLK_ANT2_I, pstCaliDcocsInd->usNOBLK_ANT2_I, sizeof(pstCaliDcocsInd->usNOBLK_ANT2_I));
    MSP_MEMCPY(pstFtmFastCtInfo->stFCaliDcocsRdCnf.usNOBLK_ANT2_Q, pstCaliDcocsInd->usNOBLK_ANT2_Q, sizeof(pstCaliDcocsInd->usNOBLK_ANT2_Q));

    return ERR_MSP_SUCCESS;
}

/*****************************************************************************
 函 数 名  : at_ftm_fcaliip2smrf_store
 功能描述  : 保存参数
 输入参数  : ucStoreType 保存类型
               FTM_F_CT_STORE_REQ_TEMP 收到装备参数设置指令，未收到LRTT确认原语，只缓存参数
               FTM_F_CT_STORE_REQ_GLOBAL 收到LRTT确认原语，保存参数，供装备查询
             pstReq  参数
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
static VOID at_ftm_fcaliip2sMrf_store(VOS_UINT8 ucStoreType, FTM_SET_FCALIIP2SMRF_REQ_STRU* pstReq)
{
    VOS_UINT32 i =0;
    static FTM_SET_FCALIIP2SMRF_REQ_STRU stReq     = { 0 };
    FTM_CT_F_MANAGE_INFO_STRU* pstFtmFastCtInfo = ftm_GetFastCTMainInfo();

    /* 收到装备参数设置指令，未收到LRTT确认原语，只缓存参数*/
    if((FTM_F_CT_STORE_REQ_TEMP==ucStoreType) && (NULL != pstReq))
    {
        MSP_MEMCPY(&stReq, pstReq, sizeof(FTM_SET_FCALIIP2SMRF_REQ_STRU));
    }
    /* 收到LRTT确认原语，保存参数，供装备查询 */
    else
    {
        MSP_MEMCPY(&pstFtmFastCtInfo->stFCaliIp2sMrfSetReq, &stReq, sizeof(FTM_SET_FCALIIP2SMRF_REQ_STRU));

        /* 设置查询信息  */
        MSP_MEMSET(&pstFtmFastCtInfo->stFCaliIp2sMrfRdCnf, 0, sizeof(pstFtmFastCtInfo->stFCaliIp2sMrfRdCnf));
        pstFtmFastCtInfo->stFCaliIp2sMrfRdCnf.ulErrCode = ERR_MSP_SUCCESS;
        pstFtmFastCtInfo->stFCaliIp2sMrfRdCnf.usStatus  = FTM_STATUS_EXECUTING;
        pstFtmFastCtInfo->stFCaliIp2sMrfRdCnf.usChanNum = pstFtmFastCtInfo->stFCaliIp2sMrfSetReq.usChanNum;
        for(i=0; i<pstFtmFastCtInfo->stFCaliIp2sMrfRdCnf.usChanNum; i++)
        {
            pstFtmFastCtInfo->stFCaliIp2sMrfRdCnf.usChan[i] = pstFtmFastCtInfo->stFCaliIp2sMrfSetReq.usChan[i];
        }
    }

    return ;
}

/*****************************************************************************
 函 数 名  : at_ftm_fcaldip2sMrf_setcnf
 功能描述  : 返回ID_MSG_FTM_SET_FCALIIP2SMRF_CNF
 输入参数  : ulErrCode 错误码
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
static VOS_UINT32 at_ftm_fcaliip2sMrf_setcnf(VOS_UINT32 ulErrCode)
{
    FTM_SET_FCALIIP2SMRF_CNF_STRU stCnf = { 0 };

    stCnf.ulErrCode = ulErrCode;

    return ftm_comm_send(ID_MSG_FTM_SET_FCALIIP2SMRF_CNF,
        (VOS_UINT32)&stCnf, sizeof(FTM_SET_FCALIIP2SMRF_CNF_STRU));
}

/*****************************************************************************
 函 数 名  : at_ftm_fcaliip2sMrf_read
 功能描述  : ID_MSG_FTM_RD_FCALIIP2SMRF_REQ 处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fcaliip2sMrf_read(VOS_VOID* pParam)
{
    FTM_CT_F_MANAGE_INFO_STRU* pstFtmFastCtInfo = ftm_GetFastCTMainInfo();

    return ftm_comm_send(ID_MSG_FTM_RD_FCALIIP2SMRF_CNF, (VOS_UINT32)&pstFtmFastCtInfo->stFCaliIp2sMrfRdCnf,
        sizeof(FTM_RD_FCALIIP2SMRF_CNF_STRU));
}

/*****************************************************************************
 函 数 名  : at_ftm_fcaliip2sMrf_set
 功能描述  : ID_MSG_FTM_SET_FCALIIP2SMRF_REQ 处理函数
             向DSP发送OM_PHY_CT_F_CALIIP2_MULTIF_SET_REQ原语
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fcaliip2sMrf_set(VOS_VOID* pParam)
{
    VOS_UINT32 i = 0;
    FTM_SET_FCALIIP2SMRF_REQ_STRU* pstFtmReq       = (FTM_SET_FCALIIP2SMRF_REQ_STRU*)pParam;
    OM_PHY_CT_F_CALIIP2_SET_MUTIRF_REQ_STRU stReqToDsp = { 0 };
    

    /* 判断是否处于非信令模式下 */
    if(ftm_GetNoneSig() != FTM_NONESIG_RTT_OPEN)
    {
        return at_ftm_fcaliip2sMrf_setcnf(ERR_MSP_UE_MODE_ERR);
    }

    /* 发送原语到DSP */
    stReqToDsp.ulMsgId    = OM_PHY_CT_F_CALIIP2_SET_MUTIRF_REQ;
    stReqToDsp.usChanNum  = pstFtmReq->usChanNum;
    for(i=0; i<stReqToDsp.usChanNum; i++)
    {
        stReqToDsp.usChan[i] = at_ftm_get_freq(EN_FCHAN_MODE_FDD_LTE,pstFtmReq->usChan[i]);
    }

    if(ERR_MSP_SUCCESS != ftm_mailbox_ltect_write(&stReqToDsp, sizeof(OM_PHY_CT_F_CALIIP2_SET_MUTIRF_REQ_STRU)))
    {
        return at_ftm_fcaliip2sMrf_setcnf(ERR_MSP_UNKNOWN);
    }

    at_ftm_fcaliip2sMrf_store(FTM_F_CT_STORE_REQ_TEMP, pstFtmReq);
    return ERR_MSP_WAIT_ASYNC;
}

/*****************************************************************************
 函 数 名  : at_ftm_fcaliip2sMrf_timeout
 功能描述  : 超时处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fcaliip2sMrf_timeout()
{
    return at_ftm_fcaliip2sMrf_setcnf(ERR_MSP_TIME_OUT);
}

/*****************************************************************************
 函 数 名  : at_ftm_fcaliip2sMrf_dspcnf
 功能描述  : DSP原语PHY_OM_CT_F_CALIIP2_SET_MUTIRF_CNF处理函数
 输入参数  : pParam PHY_OM_CT_F_CALIIP2_SET_MUTIRF_CNF_STRU指针
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fcaliip2sMrf_dspcnf(VOS_VOID* pParam)
{

    at_ftm_fcaliip2sMrf_store(FTM_F_CT_STORE_REQ_GLOBAL, NULL);

    return at_ftm_fcaliip2sMrf_setcnf(ERR_MSP_SUCCESS);
}

/*****************************************************************************
 函 数 名  : at_ftm_fcaliip2sMrf_dspind
 功能描述  : DSP原语PHY_OM_CT_F_CALIIP2_MUTIRF_IND处理函数
             将DSP主动上报的信息存入全局缓存中
 输入参数  : pParam PHY_OM_CT_F_CALIIP2_MUTIRF_IND_STRU指针
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fcaliip2sMrf_dspind(VOS_VOID* pParam)
{
    PHY_OM_CT_F_CALIIP2_MUTIRF_IND_STRU* pstCaliIp2Ind = (PHY_OM_CT_F_CALIIP2_MUTIRF_IND_STRU*)pParam;
    FTM_CT_F_MANAGE_INFO_STRU* pstFtmFastCtInfo = ftm_GetFastCTMainInfo();

    pstFtmFastCtInfo->stFCaliIp2sMrfRdCnf.ulErrCode = ERR_MSP_SUCCESS;
    pstFtmFastCtInfo->stFCaliIp2sMrfRdCnf.usStatus  = FTM_STATUS_EXECUTED;
    pstFtmFastCtInfo->stFCaliIp2sMrfRdCnf.usChanNum = pstCaliIp2Ind->usChanNum;
    MSP_MEMCPY(pstFtmFastCtInfo->stFCaliIp2sMrfRdCnf.astIp2CalRst, pstCaliIp2Ind->astIp2CalRst, sizeof(pstCaliIp2Ind->astIp2CalRst));

    return ERR_MSP_SUCCESS;
}

/*****************************************************************************
 函 数 名  : at_ftm_fcalidcocsMrf_store
 功能描述  : 保存参数
 输入参数  : ucStoreType 保存类型
               FTM_F_CT_STORE_REQ_TEMP 收到装备参数设置指令，未收到LRTT确认原语，只缓存参数
               FTM_F_CT_STORE_REQ_GLOBAL 收到LRTT确认原语，保存参数，供装备查询
             pstReq  参数
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
static VOID at_ftm_fcalidcocsMrf_store(VOS_UINT8 ucStoreType, FTM_SET_FCALIDCOCSMRF_REQ_STRU* pstReq)
{
    static FTM_SET_FCALIDCOCSMRF_REQ_STRU stReq    = {0};
    FTM_CT_F_MANAGE_INFO_STRU* pstFtmFastCtInfo = ftm_GetFastCTMainInfo();

    /* 收到装备参数设置指令，未收到LRTT确认原语，只缓存参数*/
    if((FTM_F_CT_STORE_REQ_TEMP==ucStoreType) && (NULL != pstReq))
    {
        MSP_MEMCPY(&stReq, pstReq, sizeof(FTM_SET_FCALIDCOCSMRF_REQ_STRU));
    }
    /* 收到LRTT确认原语，保存参数，供装备查询 */
    else
    {
        MSP_MEMCPY(&pstFtmFastCtInfo->stFCaliDcocsMrfSetReq, &stReq, sizeof(FTM_SET_FCALIDCOCSMRF_REQ_STRU));

        /* 设置查询信息  */
        MSP_MEMSET(&pstFtmFastCtInfo->stFCaliDcocsMrfRdCnf, 0, sizeof(pstFtmFastCtInfo->stFCaliDcocsRdCnf));
        pstFtmFastCtInfo->stFCaliDcocsMrfRdCnf.ulErrCode = ERR_MSP_SUCCESS;
        pstFtmFastCtInfo->stFCaliDcocsMrfRdCnf.usStatus  = FTM_STATUS_EXECUTING;
        pstFtmFastCtInfo->stFCaliDcocsMrfRdCnf.usChannel = stReq.usChannel;
    }

    return ;
}

/*****************************************************************************
 函 数 名  : at_ftm_fcalidcocsMrf_setcnf
 功能描述  : 返回ID_MSG_FTM_SET_FCALIDCOCSMRF_CNF
 输入参数  : ulErrCode 错误码
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
static VOS_UINT32 at_ftm_fcalidcocsMrf_setcnf(VOS_UINT32 ulErrCode)
{
    FTM_SET_FCALIDCOCSMRF_CNF_STRU stCnf = { 0 };

    stCnf.ulErrCode = ulErrCode;

    return ftm_comm_send(ID_MSG_FTM_SET_FCALIDCOCSMRF_CNF,
        (VOS_UINT32)&stCnf, sizeof(FTM_SET_FCALIDCOCSMRF_CNF_STRU));
}

/*****************************************************************************
 函 数 名  : at_ftm_fcalidcocsMrf_read
 功能描述  : ID_MSG_FTM_RD_FCALIDCOCSMRF_REQ 处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fcalidcocsMrf_read(VOS_VOID* pParam)
{
    FTM_CT_F_MANAGE_INFO_STRU* pstFtmFastCtInfo = ftm_GetFastCTMainInfo();

    return ftm_comm_send(ID_MSG_FTM_RD_FCALIDCOCSMRF_CNF, (VOS_UINT32)&pstFtmFastCtInfo->stFCaliDcocsMrfRdCnf,
        sizeof(FTM_RD_FCALIDCOCSMRF_CNF_STRU));
}

/*****************************************************************************
 函 数 名  : at_ftm_fcalidcocsMrf_set
 功能描述  : ID_MSG_FTM_SET_FCALIDCOCSMRF_REQ 处理函数
             向DSP发送OM_PHY_CT_F_CALIDCOCS_SET_MUTIRF_REQ原语
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fcalidcocsMrf_set(VOS_VOID* pParam)
{
    FTM_SET_FCALIDCOCSMRF_REQ_STRU* pstFtmReq       = (FTM_SET_FCALIDCOCSMRF_REQ_STRU*)pParam;
    OM_PHY_CT_F_CALIDCOCS_SET_MUTIRF_REQ_STRU stReqToDsp= { 0 };

    /* 判断是否处于非信令模式下 */
    if(ftm_GetNoneSig() != FTM_NONESIG_RTT_OPEN)
    {
        return at_ftm_fcalidcocsMrf_setcnf(ERR_MSP_UE_MODE_ERR);
    }

    /* 发送原语到DSP */
    stReqToDsp.ulMsgId   = OM_PHY_CT_F_CALIDCOCS_SET_MUTIRF_REQ;
    stReqToDsp.usChannel = at_ftm_get_freq(EN_FCHAN_MODE_FDD_LTE, pstFtmReq->usChannel);
    if(ERR_MSP_SUCCESS != ftm_mailbox_ltect_write(&stReqToDsp, sizeof(OM_PHY_CT_F_CALIDCOCS_SET_MUTIRF_REQ_STRU)))
    {
        return at_ftm_fcalidcocsMrf_setcnf(ERR_MSP_UNKNOWN);
    }

    at_ftm_fcalidcocsMrf_store(FTM_F_CT_STORE_REQ_TEMP, pstFtmReq);
    return ERR_MSP_WAIT_ASYNC;
}

/*****************************************************************************
 函 数 名  : at_ftm_fcalidcocsMrf_timeout
 功能描述  : 超时处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fcalidcocsMrf_timeout()
{
    return at_ftm_fcalidcocsMrf_setcnf(ERR_MSP_TIME_OUT);
}

/*****************************************************************************
 函 数 名  : at_ftm_fcalidcocsMrf_dspcnf
 功能描述  : DSP原语PHY_OM_CT_F_CALIDCOCS_SET_MUTIRF_CNF处理函数
 输入参数  : pParam PHY_OM_CT_F_CALIDCOCS_SET_MUTIRF_CNF_STRU指针
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fcalidcocsMrf_dspcnf(VOS_VOID* pParam)
{

    at_ftm_fcalidcocsMrf_store(FTM_F_CT_STORE_REQ_GLOBAL, NULL);

    return at_ftm_fcalidcocsMrf_setcnf(ERR_MSP_SUCCESS);
}

/*****************************************************************************
 函 数 名  : at_ftm_fcalidcocsMrf_dspind
 功能描述  : DSP原语PHY_OM_CT_F_CALIDCOCS_MUTIRF_IND处理函数
             将DSP主动上报的信息存入全局缓存中
 输入参数  : pParam PHY_OM_CT_F_CALIDCOCS_MUTIRF_IND_STRU指针
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fcalidcocsMrf_dspind(VOS_VOID* pParam)
{
    PHY_OM_CT_F_CALIDCOCS_MUTIRF_IND_STRU* pstCaliDcocsInd = (PHY_OM_CT_F_CALIDCOCS_MUTIRF_IND_STRU*)pParam;
    FTM_CT_F_MANAGE_INFO_STRU* pstFtmFastCtInfo     = ftm_GetFastCTMainInfo();

    pstFtmFastCtInfo->stFCaliDcocsMrfRdCnf.ulErrCode = ERR_MSP_SUCCESS;
    pstFtmFastCtInfo->stFCaliDcocsMrfRdCnf.usStatus  = FTM_STATUS_EXECUTED;
    pstFtmFastCtInfo->stFCaliDcocsMrfRdCnf.usChannel = pstCaliDcocsInd->usChanNum;
    MSP_MEMCPY(pstFtmFastCtInfo->stFCaliDcocsMrfRdCnf.astDcocCalRst, pstCaliDcocsInd->astDcocCalRst, sizeof(pstCaliDcocsInd->astDcocCalRst));

    return ERR_MSP_SUCCESS;
}

/*****************************************************************************
 函 数 名  : at_ftm_fgainstates_store
 功能描述  : 保存参数
 输入参数  : ucStoreType 保存类型
               FTM_F_CT_STORE_REQ_TEMP 收到装备参数设置指令，未收到LRTT确认原语，只缓存参数
               FTM_F_CT_STORE_REQ_GLOBAL 收到LRTT确认原语，保存参数，供装备查询
             pstReq  参数
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
static VOID at_ftm_fgainstates_store(VOS_UINT8 ucStoreType, FTM_SET_FGAINSTATES_REQ_STRU* pstReq)
{
    static FTM_SET_FGAINSTATES_REQ_STRU stReq   = {0};
    FTM_CT_F_MANAGE_INFO_STRU* pstFtmFastCtInfo = ftm_GetFastCTMainInfo();

    /* 收到装备参数设置指令，未收到LRTT确认原语，只缓存参数*/
    if((FTM_F_CT_STORE_REQ_TEMP==ucStoreType) && (NULL != pstReq))
    {
        MSP_MEMCPY(&stReq, pstReq, sizeof(FTM_SET_FGAINSTATES_REQ_STRU));
    }
    /* 收到LRTT确认原语，保存参数，供装备查询 */
    else
    {
        MSP_MEMCPY(&pstFtmFastCtInfo->stFGainStatesSetReq, &stReq, sizeof(FTM_SET_FGAINSTATES_REQ_STRU));
    }

    return ;
}

/*****************************************************************************
 函 数 名  : at_ftm_fgainstates_setcnf
 功能描述  : 返回ID_MSG_FTM_SET_FGAINSTATES_CNF
 输入参数  : ulErrCode 错误码
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
static VOS_UINT32 at_ftm_fgainstates_setcnf(VOS_UINT32 ulErrCode)
{
    FTM_SET_FGAINSTATES_CNF_STRU stCnf = { 0 };

    stCnf.ulErrCode = ulErrCode;

    return ftm_comm_send(ID_MSG_FTM_SET_FGAINSTATES_CNF,
        (VOS_UINT32)&stCnf, sizeof(FTM_SET_FGAINSTATES_CNF_STRU));
}

/*****************************************************************************
 函 数 名  : at_ftm_fgainstates_read
 功能描述  : ID_MSG_FTM_RD_FGAINSTATES_REQ 处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fgainstates_read(VOS_VOID* pParam)
{
    FTM_RD_FGAINSTATES_CNF_STRU stRdCnf         = { 0 };
    FTM_CT_F_MANAGE_INFO_STRU* pstFtmFastCtInfo = ftm_GetFastCTMainInfo();

    stRdCnf.ulErrCode      = ERR_MSP_SUCCESS;
    stRdCnf.usGainStateNum = pstFtmFastCtInfo->stFGainStatesSetReq.usGainStateNum;
    MSP_MEMCPY(stRdCnf.usGainState, pstFtmFastCtInfo->stFGainStatesSetReq.usGainState, sizeof(stRdCnf.usGainState));

    return ftm_comm_send(ID_MSG_FTM_RD_FGAINSTATES_CNF, (VOS_UINT32)&stRdCnf, sizeof(FTM_RD_FGAINSTATES_CNF_STRU));
}

/*****************************************************************************
 函 数 名  : at_ftm_fgainstates_set
 功能描述  : ID_MSG_FTM_SET_FGAINSTATES_REQ 处理函数
             向DSP发送OM_PHY_CT_F_GAINSTATE_SET_REQ原语
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fgainstates_set(VOS_VOID* pParam)
{
    FTM_SET_FGAINSTATES_REQ_STRU* pstFtmReq       = (FTM_SET_FGAINSTATES_REQ_STRU*)pParam;
    OM_PHY_CT_F_GAINSTATE_SET_REQ_STRU stReqToDsp = { 0 };
    FCHAN_MODE_ENUM fchanMode = ftm_GetFastCTMainInfo()->enCurMode;
    /* 判断是否处于非信令模式下 */
    if(ftm_GetNoneSig() != FTM_NONESIG_RTT_OPEN)
    {
        return at_ftm_fgainstates_setcnf(ERR_MSP_UE_MODE_ERR);
    }

    /* 判断参数是否有效 */
    if(pstFtmReq->usGainStateNum > FTM_GAINSTATE_MAX_NUM)
    {
         return at_ftm_fgainstates_setcnf(ERR_MSP_INVALID_PARAMETER);
    }

    /* 发送原语到DSP */
    stReqToDsp.ulMsgId        = OM_PHY_CT_F_GAINSTATE_SET_REQ;
    stReqToDsp.usGainStateNum = pstFtmReq->usGainStateNum;
    MSP_MEMCPY(stReqToDsp.usGainState, pstFtmReq->usGainState, sizeof(stReqToDsp.usGainState));
    if(ERR_MSP_SUCCESS != ftm_mailbox_ct_write(fchanMode, &stReqToDsp, sizeof(OM_PHY_CT_F_GAINSTATE_SET_REQ_STRU)))
    {
        return at_ftm_fgainstates_setcnf(ERR_MSP_UNKNOWN);
    }
    at_ftm_fgainstates_store(FTM_F_CT_STORE_REQ_TEMP, pstFtmReq);
    return ERR_MSP_WAIT_ASYNC;
}

/*****************************************************************************
 函 数 名  : at_ftm_fgainstates_timeout
 功能描述  : 超时处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fgainstates_timeout()
{
    return at_ftm_fgainstates_setcnf(ERR_MSP_TIME_OUT);
}

/*****************************************************************************
 函 数 名  : at_ftm_fgainstates_dspcnf
 功能描述  : DSP原语PHY_OM_CT_F_GAINSTATE_SET_CNF处理函数
 输入参数  : pParam PHY_OM_CT_F_GAINSTATE_SET_CNF_STRU指针
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fgainstates_dspcnf(VOS_VOID* pParam)
{

    at_ftm_fgainstates_store(FTM_F_CT_STORE_REQ_GLOBAL, NULL);

    return at_ftm_fgainstates_setcnf(ERR_MSP_SUCCESS);
}

/*****************************************************************************
 函 数 名  : at_ftm_fdbbatts_store
 功能描述  : 保存参数
 输入参数  : ucStoreType 保存类型
               FTM_F_CT_STORE_REQ_TEMP 收到装备参数设置指令，未收到LRTT确认原语，只缓存参数
               FTM_F_CT_STORE_REQ_GLOBAL 收到LRTT确认原语，保存参数，供装备查询
             pstReq  参数
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
static VOID at_ftm_fdbbatts_store(VOS_UINT8 ucStoreType, FTM_SET_FDBBATTS_REQ_STRU* pstReq)
{
    static FTM_SET_FDBBATTS_REQ_STRU stReq      = { 0 };
    FTM_CT_F_MANAGE_INFO_STRU* pstFtmFastCtInfo = ftm_GetFastCTMainInfo();

    /* 收到装备参数设置指令，未收到LRTT确认原语，只缓存参数*/
    if((FTM_F_CT_STORE_REQ_TEMP==ucStoreType) && (NULL != pstReq))
    {
        MSP_MEMCPY(&stReq, pstReq, sizeof(FTM_SET_FDBBATTS_REQ_STRU));
    }
    /* 收到LRTT确认原语，保存参数，供装备查询 */
    else
    {
        MSP_MEMCPY(&pstFtmFastCtInfo->stFDbbAttSetReq, &stReq, sizeof(FTM_SET_FDBBATTS_REQ_STRU));
    }

    return ;
}

/*****************************************************************************
 函 数 名  : at_ftm_fdbbatts_setcnf
 功能描述  : 返回ID_MSG_FTM_SET_FDBBATTS_CNF
 输入参数  : ulErrCode 错误码
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
static VOS_UINT32 at_ftm_fdbbatts_setcnf(VOS_UINT32 ulErrCode)
{
    FTM_SET_FDBBATTS_CNF_STRU stCnf = { 0 };

    stCnf.ulErrCode = ulErrCode;

    return ftm_comm_send(ID_MSG_FTM_SET_FDBBATTS_CNF,
        (VOS_UINT32)&stCnf, sizeof(FTM_SET_FDBBATTS_CNF_STRU));
}

/*****************************************************************************
 函 数 名  : at_ftm_fdbbatts_read
 功能描述  : ID_MSG_FTM_RD_FDBBATTS_REQ 处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fdbbatts_read(VOS_VOID* pParam)
{
    FTM_RD_FDBBATTS_CNF_STRU stRdCnf            = { 0 };
    FTM_CT_F_MANAGE_INFO_STRU* pstFtmFastCtInfo = ftm_GetFastCTMainInfo();

    stRdCnf.ulErrCode   = ERR_MSP_SUCCESS;
    stRdCnf.usDbbAttNum = pstFtmFastCtInfo->stFDbbAttSetReq.usDbbAttNum;
    MSP_MEMCPY(stRdCnf.usDbbAtt, pstFtmFastCtInfo->stFDbbAttSetReq.usDbbAtt, sizeof(stRdCnf.usDbbAtt));

    return ftm_comm_send(ID_MSG_FTM_RD_FDBBATTS_CNF, (VOS_UINT32)&stRdCnf, sizeof(FTM_RD_FDBBATTS_CNF_STRU));
}

/*****************************************************************************
 函 数 名  : at_ftm_fdbbatts_set
 功能描述  : ID_MSG_FTM_SET_FDBBATTS_REQ 处理函数
             向DSP发送OM_PHY_CT_F_DBBATT_SET_REQ原语
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fdbbatts_set(VOS_VOID* pParam)
{
    FTM_SET_FDBBATTS_REQ_STRU* pstFtmReq         = (FTM_SET_FDBBATTS_REQ_STRU*)pParam;
    OM_PHY_CT_F_DBBATT_SET_REQ_STRU stReqToDsp   = { 0 };
    FCHAN_MODE_ENUM fchanMode = ftm_GetFastCTMainInfo()->enCurMode;
    /* 判断是否处于非信令模式下 */
    if(ftm_GetNoneSig() != FTM_NONESIG_RTT_OPEN)
    {
        return at_ftm_fdbbatts_setcnf(ERR_MSP_UE_MODE_ERR);
    }

    /* 发送原语到DSP */
    stReqToDsp.ulMsgId     = OM_PHY_CT_F_DBBATT_SET_REQ;
    stReqToDsp.usDbbAttNum = pstFtmReq->usDbbAttNum;
    MSP_MEMCPY(stReqToDsp.usDbbAtt, pstFtmReq->usDbbAtt, sizeof(stReqToDsp.usDbbAtt));
    if(ERR_MSP_SUCCESS != ftm_mailbox_ct_write(fchanMode, &stReqToDsp, sizeof(OM_PHY_CT_F_DBBATT_SET_REQ_STRU)))
    {
        return at_ftm_fdbbatts_setcnf(ERR_MSP_UNKNOWN);
    }
    at_ftm_fdbbatts_store(FTM_F_CT_STORE_REQ_TEMP, pstFtmReq);
    return ERR_MSP_WAIT_ASYNC;
}

/*****************************************************************************
 函 数 名  : at_ftm_fdbbatts_timeout
 功能描述  : 超时处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fdbbatts_timeout()
{
    return at_ftm_fdbbatts_setcnf(ERR_MSP_TIME_OUT);
}

/*****************************************************************************
 函 数 名  : at_ftm_fdbbatts_dspcnf
 功能描述  : DSP原语PHY_OM_CT_F_DBBATT_SET_CNF处理函数
 输入参数  : pParam PHY_OM_CT_F_DBBATT_SET_CNF_STRU指针
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fdbbatts_dspcnf(VOS_VOID* pParam)
{

    at_ftm_fdbbatts_store(FTM_F_CT_STORE_REQ_GLOBAL, NULL);

    return at_ftm_fdbbatts_setcnf(ERR_MSP_SUCCESS);
}

/*****************************************************************************
 函 数 名  : at_ftm_fbbatts_store
 功能描述  : 保存参数
 输入参数  : ucStoreType 保存类型
               FTM_F_CT_STORE_REQ_TEMP 收到装备参数设置指令，未收到LRTT确认原语，只缓存参数
               FTM_F_CT_STORE_REQ_GLOBAL 收到LRTT确认原语，保存参数，供装备查询
             pstReq  参数
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
static VOID at_ftm_fbbatts_store(VOS_UINT8 ucStoreType, FTM_SET_FBBATTS_REQ_STRU* pstReq)
{
    static FTM_SET_FBBATTS_REQ_STRU stReq    = {0};
    FTM_CT_F_MANAGE_INFO_STRU* pstFtmFastCtInfo = ftm_GetFastCTMainInfo();

    /* 收到装备参数设置指令，未收到LRTT确认原语，只缓存参数*/
    if((FTM_F_CT_STORE_REQ_TEMP==ucStoreType) && (NULL != pstReq))
    {
        MSP_MEMCPY(&stReq, pstReq, sizeof(FTM_SET_FBBATTS_REQ_STRU));
    }
    /* 收到LRTT确认原语，保存参数，供装备查询 */
    else
    {
        MSP_MEMCPY(&pstFtmFastCtInfo->stFBbAttSetReq, &stReq, sizeof(FTM_SET_FBBATTS_REQ_STRU));
    }

    return ;
}

/*****************************************************************************
 函 数 名  : at_ftm_fbbatts_setcnf
 功能描述  : 返回ID_MSG_FTM_SET_FBBATTS_CNF
 输入参数  : ulErrCode 错误码
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
static VOS_UINT32 at_ftm_fbbatts_setcnf(VOS_UINT32 ulErrCode)
{
    FTM_SET_FBBATTS_CNF_STRU stCnf = { 0 };

    stCnf.ulErrCode = ulErrCode;

    return ftm_comm_send(ID_MSG_FTM_SET_FBBATTS_CNF,
        (VOS_UINT32)&stCnf, sizeof(FTM_SET_FBBATTS_CNF_STRU));
}

/*****************************************************************************
 函 数 名  : at_ftm_fbbatts_read
 功能描述  : ID_MSG_FTM_RD_FBBATTS_REQ 处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fbbatts_read(VOS_VOID* pParam)
{
    FTM_RD_FBBATTS_CNF_STRU stRdCnf             = { 0 };
    FTM_CT_F_MANAGE_INFO_STRU* pstFtmFastCtInfo = ftm_GetFastCTMainInfo();

    stRdCnf.ulErrCode  = ERR_MSP_SUCCESS;
    stRdCnf.usBbAttNum = pstFtmFastCtInfo->stFBbAttSetReq.usBbAttNum;
    MSP_MEMCPY(stRdCnf.usBbAtt, pstFtmFastCtInfo->stFBbAttSetReq.usBbAtt, sizeof(stRdCnf.usBbAtt));

    return ftm_comm_send(ID_MSG_FTM_RD_FBBATTS_CNF, (VOS_UINT32)&stRdCnf, sizeof(FTM_RD_FBBATTS_CNF_STRU));
}

/*****************************************************************************
 函 数 名  : at_ftm_fbbatts_set
 功能描述  : ID_MSG_FTM_SET_FBBATTS_REQ 处理函数
             向DSP发送OM_PHY_CT_F_BBATT_SET_REQ原语
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fbbatts_set(VOS_VOID* pParam)
{
    FTM_SET_FBBATTS_REQ_STRU* pstFtmReq          = (FTM_SET_FBBATTS_REQ_STRU*)pParam;
    OM_PHY_CT_F_BBATT_SET_REQ_STRU stReqToDsp    = { 0 };
    FCHAN_MODE_ENUM fchanMode = ftm_GetFastCTMainInfo()->enCurMode;
    /* 判断是否处于非信令模式下 */
    if(ftm_GetNoneSig() != FTM_NONESIG_RTT_OPEN)
    {
        return at_ftm_fbbatts_setcnf(ERR_MSP_UE_MODE_ERR);
    }

    /* 发送原语到DSP */
    stReqToDsp.ulMsgId    = OM_PHY_CT_F_BBATT_SET_REQ;
    stReqToDsp.usBbAttNum = pstFtmReq->usBbAttNum;
    MSP_MEMCPY(stReqToDsp.usBbAtt, pstFtmReq->usBbAtt, sizeof(stReqToDsp.usBbAtt));
    if(ERR_MSP_SUCCESS != ftm_mailbox_ct_write(fchanMode, &stReqToDsp, sizeof(OM_PHY_CT_F_BBATT_SET_REQ_STRU)))
    {
        return at_ftm_fbbatts_setcnf(ERR_MSP_UNKNOWN);
    }
    at_ftm_fbbatts_store(FTM_F_CT_STORE_REQ_TEMP, pstFtmReq);
    return ERR_MSP_WAIT_ASYNC;
}

/*****************************************************************************
 函 数 名  : at_ftm_fbbatts_timeout
 功能描述  : 超时处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fbbatts_timeout()
{
    return at_ftm_fbbatts_setcnf(ERR_MSP_TIME_OUT);
}

/*****************************************************************************
 函 数 名  : at_ftm_fbbatts_dspcnf
 功能描述  : DSP原语PHY_OM_CT_F_BBATT_SET_CNF处理函数
 输入参数  : pParam PHY_OM_CT_F_BBATT_SET_CNF_STRU指针
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fbbatts_dspcnf(VOS_VOID* pParam)
{

    at_ftm_fbbatts_store(FTM_F_CT_STORE_REQ_GLOBAL, NULL);

    return at_ftm_fbbatts_setcnf(ERR_MSP_SUCCESS);
}

/*****************************************************************************
 函 数 名  : at_ftm_fcalitxiqs_store
 功能描述  : 保存参数
 输入参数  : ucStoreType 保存类型
               FTM_F_CT_STORE_REQ_TEMP 收到装备参数设置指令，未收到LRTT确认原语，只缓存参数
               FTM_F_CT_STORE_REQ_GLOBAL 收到LRTT确认原语，保存参数，供装备查询
             pstReq  参数
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
static VOID at_ftm_fcalitxiqs_store(VOS_UINT8 ucStoreType, FTM_SET_FCALITXIQS_REQ_STRU* pstReq)
{
    static FTM_SET_FCALITXIQS_REQ_STRU stReq    = { 0 };
    FTM_CT_F_MANAGE_INFO_STRU* pstFtmFastCtInfo = ftm_GetFastCTMainInfo();

    /* 收到装备参数设置指令，未收到LRTT确认原语，只缓存参数*/
    if((FTM_F_CT_STORE_REQ_TEMP==ucStoreType) && (NULL != pstReq))
    {
        MSP_MEMCPY(&stReq, pstReq, sizeof(FTM_SET_FCALITXIQS_REQ_STRU));
    }
    /* 收到LRTT确认原语，保存参数，供装备查询 */
    else
    {
         MSP_MEMCPY(&pstFtmFastCtInfo->stFCaliTxiqSetReq, &stReq, sizeof(FTM_SET_FCALITXIQS_REQ_STRU));

         /* 设置查询信息  */
         MSP_MEMSET(&pstFtmFastCtInfo->stFCaliTxiqRdCnf, 0, sizeof(pstFtmFastCtInfo->stFCaliTxiqRdCnf));
         pstFtmFastCtInfo->stFCaliTxiqRdCnf.ulErrCode = ERR_MSP_SUCCESS;
         pstFtmFastCtInfo->stFCaliTxiqRdCnf.usStatus  = FTM_STATUS_EXECUTING;
    }

    return ;
}

/*****************************************************************************
 函 数 名  : at_ftm_fcalitxiqs_setcnf
 功能描述  : 返回ID_MSG_FTM_SET_FCALITXIQS_CNF
 输入参数  : ulErrCode 错误码
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
static VOS_UINT32 at_ftm_fcalitxiqs_setcnf(VOS_UINT32 ulErrCode)
{
    FTM_SET_FCALITXIQS_CNF_STRU stCnf = { 0 };

    stCnf.ulErrCode = ulErrCode;

    return ftm_comm_send(ID_MSG_FTM_SET_FCALITXIQS_CNF,
        (VOS_UINT32)&stCnf, sizeof(FTM_SET_FCALITXIQS_CNF_STRU));
}

/*****************************************************************************
 函 数 名  : at_ftm_fcalitxiqs_read
 功能描述  : ID_MSG_FTM_RD_FCALITXIQS_REQ 处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fcalitxiqs_read(VOS_VOID* pParam)
{
    FTM_CT_F_MANAGE_INFO_STRU* pstFtmFastCtInfo = ftm_GetFastCTMainInfo();

    return ftm_comm_send(ID_MSG_FTM_RD_FCALITXIQS_CNF, (VOS_UINT32)&pstFtmFastCtInfo->stFCaliTxiqRdCnf,
        sizeof(FTM_RD_FCALITXIQS_CNF_STRU));
}

/*****************************************************************************
 函 数 名  : at_ftm_fcalitxiqs_set
 功能描述  : ID_MSG_FTM_SET_FCALITXIQS_REQ 处理函数
             向DSP发送OM_PHY_CT_F_CALITXIQ_SET_REQ原语
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fcalitxiqs_set(VOS_VOID* pParam)
{
    FTM_SET_FCALITXIQS_REQ_STRU* pstFtmReq      = (FTM_SET_FCALITXIQS_REQ_STRU*)pParam;
	OM_PHY_CT_F_TXIQ_SET_REQ_STRU stReqToDsp	= { 0 };	
	FCHAN_MODE_ENUM fchanMode = ftm_GetFastCTMainInfo()->enCurMode;

    /* 判断是否处于非信令模式下 */
    if(ftm_GetNoneSig() != FTM_NONESIG_RTT_OPEN)
    {
        return at_ftm_fcalitxiqs_setcnf(ERR_MSP_UE_MODE_ERR);
    }

    /* 发送原语到DSP */
    stReqToDsp.ulMsgId   = OM_PHY_CT_F_CALITXIQ_SET_REQ;
    stReqToDsp.usChannel = at_ftm_get_freq(fchanMode, pstFtmReq->usChannel);

    /*AT^FCALITXIQS只发送到LTE邮箱 */
    if(ERR_MSP_SUCCESS != ftm_mailbox_ltect_write( &stReqToDsp, sizeof(OM_PHY_CT_F_TXIQ_SET_REQ_STRU)))
    {
        return at_ftm_fcalitxiqs_setcnf(ERR_MSP_UNKNOWN);
    }

    at_ftm_fcalitxiqs_store(FTM_F_CT_STORE_REQ_TEMP, pstFtmReq);
    return ERR_MSP_WAIT_ASYNC;
}

/*****************************************************************************
 函 数 名  : at_ftm_fcalitxiqs_timeout
 功能描述  : 超时处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fcalitxiqs_timeout()
{
    return at_ftm_fcalitxiqs_setcnf(ERR_MSP_TIME_OUT);
}

/*****************************************************************************
 函 数 名  : at_ftm_fcalitxiqs_dspcnf
 功能描述  : DSP原语PHY_OM_CT_F_CALITXIQ_SET_CNF处理函数
 输入参数  : pParam PHY_OM_CT_F_TXIQ_SET_CNF_STRU指针
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fcalitxiqs_dspcnf(VOS_VOID* pParam)
{

    at_ftm_fcalitxiqs_store(FTM_F_CT_STORE_REQ_GLOBAL, NULL);

    return at_ftm_fcalitxiqs_setcnf(ERR_MSP_SUCCESS);
}

/*****************************************************************************
 函 数 名  : at_ftm_fcalitxiqs_dspind
 功能描述  : DSP原语PHY_OM_CT_F_CALITXIQ_IND处理函数
             将DSP主动上报的信息存入全局缓存中
 输入参数  : pParam PHY_OM_CT_F_TXIQ_IND_STRU指针
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fcalitxiqs_dspind(VOS_VOID* pParam)
{
    PHY_OM_CT_F_TXIQ_IND_STRU* pstCaliTxiqInd   = (PHY_OM_CT_F_TXIQ_IND_STRU*)pParam;
    FTM_CT_F_MANAGE_INFO_STRU* pstFtmFastCtInfo = ftm_GetFastCTMainInfo();

    pstFtmFastCtInfo->stFCaliTxiqRdCnf.ulErrCode   = ERR_MSP_SUCCESS;
    pstFtmFastCtInfo->stFCaliTxiqRdCnf.usStatus    = FTM_STATUS_EXECUTED;
    pstFtmFastCtInfo->stFCaliTxiqRdCnf.usAmplitude = pstCaliTxiqInd->usAmplitude;
    pstFtmFastCtInfo->stFCaliTxiqRdCnf.usPhase     = pstCaliTxiqInd->usPhase;
    pstFtmFastCtInfo->stFCaliTxiqRdCnf.usDCI       = pstCaliTxiqInd->usDCI;
    pstFtmFastCtInfo->stFCaliTxiqRdCnf.usDCQ       = pstCaliTxiqInd->usDCQ;

    return ERR_MSP_SUCCESS;
}

/*****************************************************************************
 函 数 名  : at_ftm_fipstart_store
 功能描述  : 保存参数
 输入参数  : ucStoreType 保存类型
               FTM_F_CT_STORE_REQ_TEMP 收到装备参数设置指令，未收到LRTT确认原语，只缓存参数
               FTM_F_CT_STORE_REQ_GLOBAL 收到LRTT确认原语，保存参数，供装备查询
             pstReq  参数
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
static VOID at_ftm_fipstart_store(VOS_UINT8 ucStoreType, FTM_SET_F_FIPSTART_REQ_STRU * pstReq)
{
    static FTM_SET_F_FIPSTART_REQ_STRU stReq    = { 0 };
    FTM_CT_F_MANAGE_INFO_STRU* pstFtmFastCtInfo = ftm_GetFastCTMainInfo();

    /* 收到装备参数设置指令，未收到LRTT确认原语，只缓存参数*/
    if((FTM_F_CT_STORE_REQ_TEMP==ucStoreType) && (NULL != pstReq))
    {
        MSP_MEMCPY(&stReq, pstReq, sizeof(FTM_SET_F_FIPSTART_REQ_STRU));
    }
    /* 收到LRTT确认原语，保存参数，供装备查询 */
    else
    {
        pstFtmFastCtInfo->ulCurCmdStepsFlag |= CT_F_STEPS_DONE_FIPSTART;
        pstFtmFastCtInfo->ulChannel          = stReq.ulChannel;
    }

    return ;
}

/*****************************************************************************
 函 数 名  : at_ftm_fipstart_setcnf
 功能描述  : 返回ID_MSG_FTM_F_SET_FIPSTART_CNF
 输入参数  : ulErrCode 错误码
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
static VOS_UINT32 at_ftm_fipstart_setcnf(VOS_UINT32 ulIp2Value1, VOS_UINT32 ulIp2Value2,  VOS_UINT32 ulErrCode)
{
    FTM_SET_F_FIPSTART_CNF_STRU stCnf = { 0 };

    stCnf.ulErrCode   = ulErrCode;
    stCnf.ulIp2Value1 = ulIp2Value1;
    stCnf.ulIp2Value2 = ulIp2Value2;

    return ftm_comm_send(ID_MSG_FTM_F_SET_FIPSTART_CNF,
        (VOS_UINT32)&stCnf, sizeof(FTM_SET_F_FIPSTART_CNF_STRU));
}

/*****************************************************************************
 函 数 名  : at_ftm_fipstart_set
 功能描述  : ID_MSG_FTM_F_SET_FIPSTART_REQ 处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fipstart_set(VOS_VOID* pParam)
{
    FTM_SET_F_FIPSTART_REQ_STRU* pstFtmReq      = (FTM_SET_F_FIPSTART_REQ_STRU*)pParam;
    OM_PHY_CT_F_IP2START_REQ_STRU stReqToDsp    = { 0 };

    /* 判断是否处于非信令模式下 */
    if(ftm_GetNoneSig() != FTM_NONESIG_RTT_OPEN)
    {
        return at_ftm_fipstart_setcnf(0, 0, ERR_MSP_UE_MODE_ERR);
    }

    /* 发送原语到DSP */
	/*lint -save -e734*/
    stReqToDsp.ulMsgId = OM_PHY_CT_F_FIPSTART_SET_REQ;
    stReqToDsp.usBand  = pstFtmReq->ulChannel;
	/*lint -restore*/
    if(ERR_MSP_SUCCESS != ftm_mailbox_ltect_write(&stReqToDsp, sizeof(OM_PHY_CT_F_IP2START_REQ_STRU)))
    {
        return at_ftm_fipstart_setcnf(0, 0, ERR_MSP_UNKNOWN);
    }

    at_ftm_fipstart_store(FTM_F_CT_STORE_REQ_TEMP, pstFtmReq);
    return ERR_MSP_WAIT_ASYNC;
}

/*****************************************************************************
 函 数 名  : at_ftm_fipstart_timeout
 功能描述  : 超时处理函数
 输入参数  : 无
 输出参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fipstart_timeout()
{
    return at_ftm_fipstart_setcnf(0, 0, ERR_MSP_TIME_OUT);
}

/*****************************************************************************
 函 数 名  : at_ftm_fipstart_dspind
 功能描述  : DSP原语PHY_OM_CT_F_MEAS_IP2_IND处理函数
 输入参数  : pParam PHY_OM_CT_F_MEAS_IP2_IND_STRU指针
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fipstart_dspind(VOS_VOID* pParam)
{
    PHY_OM_CT_F_MEAS_IP2_IND_STRU* pDspInd = (PHY_OM_CT_F_MEAS_IP2_IND_STRU*)pParam;

    at_ftm_fipstart_store(FTM_F_CT_STORE_REQ_GLOBAL, NULL);

    return at_ftm_fipstart_setcnf(pDspInd->ulIP2Value1 , pDspInd->ulIP2Value1,  ERR_MSP_SUCCESS);
}

/*****************************************************************************
 函 数 名  : at_ftm_fcalpddcs_setcnf
 功能描述  : 保存参数
 输入参数  : ulErrCode 错误码
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
PRIVATE VOS_UINT32 at_ftm_fcalpddcs_setcnf(VOS_UINT32 ulErrCode)
{
    FTM_SET_F_FCALPDDCS_CNF_STRU stCnf = { 0 };

    stCnf.ulErrCode = ulErrCode;

    return ftm_comm_send(ID_MSG_FTM_F_SET_FCALPDDCS_CNF,
        (VOS_UINT32)&stCnf, sizeof(FTM_SET_F_FCALPDDCS_CNF_STRU));
}

/*****************************************************************************
 函 数 名  : at_ftm_fcalpddcs_store
 功能描述  : 保存参数
 输入参数  : ucStoreType 保存类型
               FTM_F_CT_STORE_REQ_TEMP 收到装备参数设置指令，未收到LRTT确认原语，只缓存参数
               FTM_F_CT_STORE_REQ_GLOBAL 收到LRTT确认原语，保存参数，供装备查询
             pstReq  参数
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
PRIVATE VOID at_ftm_fcalpddcs_store(VOS_UINT8 ucStoreType, FTM_SET_F_FCALPDDCS_REQ_STRU* pstReq)
{
    static FTM_SET_F_FCALPDDCS_REQ_STRU stReq     = { 0 };
	FTM_CT_F_MANAGE_INFO_STRU* pstFtmFastCtInfo  = ftm_GetFastCTMainInfo();

    /* 收到装备参数设置指令，未收到LRTT确认原语，只缓存参数*/
    if((FTM_F_CT_STORE_REQ_TEMP==ucStoreType) && (NULL != pstReq))
    {
        MSP_MEMCPY(&stReq, pstReq, sizeof(FTM_SET_F_FCALPDDCS_REQ_STRU));
    }
    /* 收到LRTT确认原语，保存参数，供装备查询 */
    else
    {
        MSP_MEMCPY(&pstFtmFastCtInfo->stFCalPDDCSSetReq, &stReq, sizeof(FTM_SET_F_FCALPDDCS_REQ_STRU));

        /* 设置查询信息  */
        MSP_MEMSET(&pstFtmFastCtInfo->stFCalPDDCSRdCnf, 0, sizeof(pstFtmFastCtInfo->stFCalPDDCSRdCnf));
        pstFtmFastCtInfo->stFCalPDDCSRdCnf.ulErrCode = ERR_MSP_SUCCESS;
	    pstFtmFastCtInfo->stFCalPDDCSRdCnf.usStatus  = FTM_CALPDDCS_EXECUTING;///??
    }

    return ;
}

/*****************************************************************************
 函 数 名  : at_ftm_fcalpddcs_set
 功能描述  : ID_MSG_FTM_F_SET_FCALPDDCS_REQ 处理函数
                          向DSP发送OM_PHY_CT_F_BBATT_SET_REQ原语
 输入参数  : pParam FTM_SET_F_FCALPDDCS_REQ_STRU指针
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fcalpddcs_set(VOID* pParam)
{
    FTM_SET_F_FCALPDDCS_REQ_STRU* pstFtmReq      = (FTM_SET_F_FCALPDDCS_REQ_STRU*)pParam;
    OM_PHY_CT_F_PD_AUTOCAL_REQ_STRU stReqToDsp    = { 0 };
	FCHAN_MODE_ENUM fchanMode = ftm_GetFastCTMainInfo()->enCurMode;

    /* 判断是否处于非信令模式下 */
    if(ftm_GetNoneSig() != FTM_NONESIG_RTT_OPEN)
    {
        return at_ftm_fcalpddcs_setcnf(ERR_MSP_UE_MODE_ERR);
    }

    /* 发送原语到DSP */
    stReqToDsp.ulMsgId = OM_PHY_CT_F_PD_AUTO_CAL_REQ;/* ??  原语待确认*/
    stReqToDsp.ulPdAutoFlg = pstFtmReq->ulPdAutoFlg;
    if(ERR_MSP_SUCCESS != ftm_mailbox_ct_write(fchanMode, &stReqToDsp, sizeof(OM_PHY_CT_F_PD_AUTOCAL_REQ_STRU)))
    {
        HAL_SDMLOG("[%s]:send data to dsp fail \n", __FUNCTION__);
        return at_ftm_fcalpddcs_setcnf(ERR_MSP_UNKNOWN);
    }

    at_ftm_fcalpddcs_store(FTM_F_CT_STORE_REQ_TEMP, pstFtmReq);
    return ERR_MSP_WAIT_ASYNC;
}

/*****************************************************************************
 函 数 名  : at_ftm_fcalpddcs_dspind
 功能描述  : DSP原语PHY_OM_CT_F_MEAS_PDDCS_IND处理函数
             处理DSP主动上报的PDDC测量值:将测量结果保存在全局缓存中
 输入参数  : pParam PHY_OM_CT_F_MEAS_PDDCS_IND_STRU指针
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fcalpddcs_dspind(VOID* pParam)
{
    PHY_OM_CT_F_PD_AUTO_IND_STRU* pstCalPDDCInd = (PHY_OM_CT_F_PD_AUTO_IND_STRU*)pParam;
    FTM_CT_F_MANAGE_INFO_STRU* pstFtmFastCtInfo = ftm_GetFastCTMainInfo();

    HAL_SDMLOG("[%s]:msgID =%d \n", __FUNCTION__,pstCalPDDCInd->ulMsgId);
    pstFtmFastCtInfo->stFCalPDDCSRdCnf.ulErrCode = ERR_MSP_SUCCESS;
    pstFtmFastCtInfo->stFCalPDDCSRdCnf.usStatus  = FTM_CALPDDCS_EXECUTED;
    pstFtmFastCtInfo->stFCalPDDCSRdCnf.usDacCfg = pstCalPDDCInd->usDacCfg;
    pstFtmFastCtInfo->stFCalPDDCSRdCnf.usVgaCfg = pstCalPDDCInd->usVgaCfg;
    return ERR_MSP_SUCCESS;
}

/*****************************************************************************
 函 数 名  : at_ftm_fcalpddcs_read
 功能描述  : ID_MSG_FTM_F_RD_FCALPDDCS_REQ 处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fcalpddcs_read()
{
    FTM_CT_F_MANAGE_INFO_STRU* pstFtmFastCtInfo = ftm_GetFastCTMainInfo();

    return ftm_comm_send(ID_MSG_FTM_F_RD_FCALPDDCS_CNF, (VOS_UINT32)&pstFtmFastCtInfo->stFCalPDDCSRdCnf,
        sizeof(FTM_RD_F_FCALPDDCS_CNF_STRU));
}

/*****************************************************************************
 函 数 名  : at_ftm_fcalpddcs_dspcnf
 功能描述  : DSP原语PHY_OM_CT_F_FCALPDDCS_SET_CNF处理函数
 输入参数  : NULL
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fcalpddcs_dspcnf(VOID* pParam)
{
    at_ftm_fcalpddcs_store(FTM_F_CT_STORE_REQ_GLOBAL, NULL);

    return at_ftm_fcalpddcs_setcnf(ERR_MSP_SUCCESS);
}

/*****************************************************************************
 函 数 名  : at_ftm_fcalpddcs_timeout
 功能描述  : 超时处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fcalpddcs_timeout()
{
    return at_ftm_fcalpddcs_setcnf(ERR_MSP_TIME_OUT);
}


/*****************************************************************************
 函 数 名  : at_ftm_fpdpows_store
 功能描述  : 保存参数
 输入参数  : ucStoreType 保存类型
               FTM_F_CT_STORE_REQ_TEMP 收到装备参数设置指令，未收到LRTT确认原语，只缓存参数
               FTM_F_CT_STORE_REQ_GLOBAL 收到LRTT确认原语，保存参数，供装备查询
             pstReq  参数
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
PRIVATE VOID at_ftm_fpdpows_store(VOS_UINT8 ucStoreType, FTM_SET_F_FPDPOWS_REQ_STRU* pstReq)
{
    static FTM_SET_F_FPDPOWS_REQ_STRU stReq    = {0};
    FTM_CT_F_MANAGE_INFO_STRU *pstFCtMainInfo = ftm_GetFastCTMainInfo();

    /* 收到装备参数设置指令，未收到LRTT确认原语，只缓存参数*/
    if ((FTM_F_CT_STORE_REQ_TEMP==ucStoreType) && (NULL != pstReq))
    {
        MSP_MEMCPY(&stReq, pstReq, sizeof(FTM_SET_F_FPDPOWS_REQ_STRU));
    }
    /* 收到LRTT确认原语，保存参数，供装备查询 */
    else
    {
        pstFCtMainInfo->stFpdpowsSetReq  = stReq;
		pstFCtMainInfo->ulCurCmdStepsFlag |= CT_F_STEPS_DONE_FPDPOWS; /*记标记*/
    }

    return ;
}

/*****************************************************************************
 函 数 名  : at_ftm_fpdpows_setcnf
 功能描述  : 保存参数
 输入参数  : ulErrCode 错误码
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
PRIVATE VOS_UINT32 at_ftm_fpdpows_setcnf(VOS_UINT32 ulErrCode)
{
    FTM_SET_F_FPDPOWS_CNF_STRU stCnf = { 0 };

    stCnf.ulErrCode = ulErrCode;

    return ftm_comm_send(ID_MSG_FTM_SET_FPDPOWS_CNF,
        (VOS_UINT32)&stCnf, sizeof(FTM_SET_F_FPDPOWS_CNF_STRU));
}

/*****************************************************************************
 函 数 名  : at_ftm_fpdpows_read
 功能描述  : ID_MSG_FTM_RD_FPDPOWS_RD 处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
#if 0
EXTERN U32_T at_ftm_fpdpows_read()
{
    FTM_RD_F_FPDPOWS_CNF_STRU stRdCnf          = { 0 };
    FTM_CT_F_MANAGE_INFO_STRU *stFCtMainInfo = ftm_GetFastCTMainInfo();

    stRdCnf.ulErrCode = ERR_MSP_SUCCESS;
    stRdCnf.ulSwitch  = stFCtMainInfo->ulFpdpowsSw;

    return ftm_comm_send(ID_MSG_FTM_RD_FPDPOWS_CNF, (U32_T)&stRdCnf, sizeof(FTM_RD_F_FPDPOWS_CNF_STRU));
}
#endif
/*****************************************************************************
 函 数 名  : at_ftm_fpdpows_set
 功能描述  : ID_MSG_FTM_F_SET_FPDPOWS_REQ 处理函数
 输入参数  : FTM_SET_F_FPDPOWS_REQ_STRU指针，保存开关
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fpdpows_set(VOID* pParam)
{
    FTM_SET_F_FPDPOWS_REQ_STRU*  stFtmReq        = (FTM_SET_F_FPDPOWS_REQ_STRU*)pParam;
    OM_PHY_CT_F_PD_REQ_STRU stReqToDsp = { 0 };
	FCHAN_MODE_ENUM fchanMode = ftm_GetFastCTMainInfo()->enCurMode;//需要是LTE

    /* 判断是否处于非信令模式下 */
    if (ftm_GetNoneSig() != FTM_NONESIG_RTT_OPEN)
    {
        return at_ftm_fpdpows_setcnf(ERR_MSP_UE_MODE_ERR);
    }

    /* 发送原语到DSP */
    stReqToDsp.ulMsgId    = OM_PHY_CT_F_PD_CAL_REQ;
    stReqToDsp.ulPdEnbFlg = stFtmReq->ulPdEnbFlg;
	stReqToDsp.usDacCfg = stFtmReq->usDacCfg;
	stReqToDsp.usVgaCfg = stFtmReq->usVgaCfg;

	HAL_SDMLOG("[%s]:ready send data to dsp  \n", __FUNCTION__);
	if(ERR_MSP_SUCCESS != ftm_mailbox_ct_write(fchanMode, &stReqToDsp, sizeof(OM_PHY_CT_F_PD_REQ_STRU)))
		{
			HAL_SDMLOG("[%s]:send data to dsp fail \n", __FUNCTION__);
			return at_ftm_fpdpows_setcnf(ERR_MSP_UNKNOWN);
		}

    at_ftm_fpdpows_store(FTM_F_CT_STORE_REQ_TEMP, stFtmReq);
    return ERR_MSP_WAIT_ASYNC;
}

/*****************************************************************************
 函 数 名  : at_ftm_fpdpows_timeout
 功能描述  : 超时处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fpdpows_timeout()
{
    HAL_SDMLOG("[%s]: FPDDPOWS set to dsp timeout\n", __FUNCTION__);
    return at_ftm_fpdpows_setcnf(ERR_MSP_TIME_OUT);
}

/*****************************************************************************
 函 数 名  : at_ftm_fpdpows_dspcnf
 功能描述  : DSP原语PHY_OM_CT_F_FPDPOWS_CNF处理函数
 输入参数  : pParam PHY_OM_CT_F_FPDPOWS_SET_CNF_STRU指针
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fpdpows_dspcnf(VOID* pParam)
{

    at_ftm_fpdpows_store(FTM_F_CT_STORE_REQ_GLOBAL, NULL);
    HAL_SDMLOG("[%s]: FPDDPOWS set to dsp CNF\n", __FUNCTION__);
    return at_ftm_fpdpows_setcnf(ERR_MSP_SUCCESS);
}

VOS_UINT32 at_ftm_fpdpows_dspind(VOID* pParam)
{
    PHY_OM_CT_F_PD_IND_STRU* pstfpdpowsInd = (PHY_OM_CT_F_PD_IND_STRU*)pParam;
    FTM_CT_F_MANAGE_INFO_STRU* pstFtmFastCtInfo = ftm_GetFastCTMainInfo();
	VOS_UINT16 usTmpi = 0;
    VOS_UINT16 usTmpj = 0;
    VOS_UINT16 usTmpValue = 0;

    HAL_SDMLOG("ENTER at_ftm_fpdpows_dspind \n");
    vos_printf("xxxxxxxxxxxxxxat_ftm_fpdpows_dspind");

    pstFtmFastCtInfo->ulCurCmdStepsFlag |= CT_F_STEPS_DONE_FPDPOWS;

	for(usTmpi = 0 ; usTmpi < FREQ_MAX_NUM ; usTmpi ++)
    {
        for(usTmpj = 0 ; usTmpj < POW_MAX_NUM ; usTmpj ++)
        {
            usTmpValue = pstfpdpowsInd->ausFreqVota[usTmpi][usTmpj];

            /* 强制覆盖之前保存在全局结构中的PDDC测量值 */
            pstFtmFastCtInfo->stFQPDDCRESRdCnf.ausPDDCValue[usTmpi][usTmpj] = usTmpValue;

            vos_printf("%d ",usTmpValue);
        }

        vos_printf("\n ");
    }

 //   pstFtmFastCtInfo->ulErrCode = ERR_MSP_SUCCESS;
    return ERR_MSP_SUCCESS;
}


/*****************************************************************************
 函 数 名  : at_ftm_fqpddcres_set
 功能描述  : ID_MSG_FTM_F_SET_FQPDDCRES_REQ 处理函数
             拷贝全局缓存并将结果返回，上报结果不判断是否成功，由装备判断状态
 输入参数  : pParam FTM_SET_F_FQPDDCRES_REQ_STRU指针
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_fqpddcres_set(VOID* pParam)
{
    VOS_BOOL bFreqExist = FALSE;
    VOS_UINT16 usTmp       = 0;

    FTM_SET_FQPDDCRES_CNF_STRU stCnf             = { {0}, };
    FTM_SET_FQPDDCRES_REQ_STRU* pstFtmReq        = (FTM_SET_FQPDDCRES_REQ_STRU*)(pParam);
    FTM_CT_F_MANAGE_INFO_STRU* pstFtmFastCtInfo = ftm_GetFastCTMainInfo();

    /* 判断是否处于非信令模式下 */
    #if 0
    if((ftm_GetNoneSig() != FTM_NONESIG_RTT_OPEN)
		||(!ftm_CheckSynCmdSteps(CT_F_STEPS_DONE_FPDPOWS|CT_F_STEPS_DONE_FSTART)))
    {
        stCnf.ulErrCode = ERR_MSP_UE_MODE_ERR;
        HAL_SDMLOG("%s:NONSIG is not ok OR FPDPOWS and FSTART is not ok\n",__FUNCTION__);
        return ftm_comm_send(ID_MSG_FTM_F_SET_FQPDDCRES_CNF,
            (VOS_UINT32)&stCnf, sizeof(FTM_SET_FQPDDCRES_CNF_STRU));
    }
    #endif

    HAL_SDMLOG("ENTER at_ftm_fqpddcres_set \n");
    /* 判断上行行道是否存在于用户已配置的上行信道数组中 */
    for(usTmp = 0 ; usTmp < pstFtmFastCtInfo->usCurUlListNum ; usTmp ++)
    {
        if (pstFtmFastCtInfo->ausCurUlFreqInfo[usTmp] == pstFtmReq->ulChannel)
        {
            bFreqExist = TRUE;
            break; /* 找到第一个匹配的下行信道号就返回 */
        }
    }

    if(bFreqExist == FALSE)
    {
       stCnf.ulErrCode = ERR_MSP_NOT_SET_CHAN;

       return ftm_comm_send(ID_MSG_FTM_F_SET_FQPDDCRES_CNF,
        (VOS_UINT32)&stCnf, sizeof(FTM_SET_FQPDDCRES_CNF_STRU));
    }

    /* 拷贝全局变量中的结果，用于填充响应结构，并发出 */
    stCnf.ulErrCode    = ERR_MSP_SUCCESS;
    MSP_MEMCPY(stCnf.ausPDDCValue, pstFtmFastCtInfo->stFQPDDCRESRdCnf.ausPDDCValue[usTmp], sizeof(stCnf.ausPDDCValue));

    return ftm_comm_send(ID_MSG_FTM_F_SET_FQPDDCRES_CNF,
        (VOS_UINT32)&stCnf, sizeof(FTM_SET_FQPDDCRES_CNF_STRU));
}
/*lint -restore*/






