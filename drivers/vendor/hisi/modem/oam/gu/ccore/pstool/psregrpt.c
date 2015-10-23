

#ifdef  __cplusplus
  #if  __cplusplus
  extern "C"{
  #endif
#endif


/******************************************************************************
   1 头文件包含
******************************************************************************/

#include "stddef.h"
#include "pslog.h"
#include "om.h"
#include "apminterface.h"
#include "hpaglobalvar.h"
#include "omprivate.h"
#include "sleepflow.h"
#include "PsCommonDef.h"
#include "TtfOamInterface.h"
#include "psregrpt.h"

/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/

/*lint -e767 修改人：甘兰47350；检视人：徐铖51137；原因简述：寄存器上报功能*/
#define    THIS_FILE_ID        PS_FILE_ID_PS_REG_RPT_C
/*lint +e767 修改人：甘兰47350；检视人：徐铖51137；原因简述：寄存器上报功能*/




/******************************************************************************
   2 外部函数和变量声明
******************************************************************************/

/******************************************************************************
   3 私有定义
******************************************************************************/

/******************************************************************************
   4 全局变量定义
******************************************************************************/
HTIMER                  g_PsRegRptTimer = VOS_NULL_PTR;

PS_REG_RPT_CFG_STRU     g_astPsRegRptCfg[PS_REG_SYS_MODE_BUTT];
PS_REG_RPT_VALUE_STRU   g_astPsRegRptBuf[PS_REG_RPT_RBUF_NUM];

TTF_RING_Q_ST           g_stPsRegRptRingQ   = {0};
PS_REG_SIMPLE_RPT_MSG   g_stPsRegSimpleRptMsg;

extern VOS_UINT32 OM_TraceMsgHook(VOS_VOID* pMsg);

VOS_UINT32              g_ulPsReg = VOS_YES;      /* */

VOS_UINT32              g_aulRegCaptureNum[PS_REG_SYS_MODE_BUTT] = {0};

VOS_UINT32              g_aulRegFullNum[PS_REG_SYS_MODE_BUTT] = {0};

/******************************************************************************
   5 函数实现
******************************************************************************/

VOS_VOID PsRegCapture(VOS_UINT8 ucRptMode, VOS_UINT16 usSfn,
                            VOS_UINT32 ulFN, VOS_UINT32 ulSysMode)
{
    PS_REG_SYS_MODE_ENUM_UINT32 enSysMode;
    PS_REG_RPT_VALUE_STRU       *pstPsRegRptValue;
    PS_REG_RPT_CFG_STRU         *pstPsRegRptCfg;
    VOS_UINT32                  ulRegRptCntLoop;
    VOS_UINT32                  ulCnt;

    /*定义不同需要进行转换*/
    enSysMode       = ulSysMode;
    pstPsRegRptCfg  = &g_astPsRegRptCfg[enSysMode];

    /*判断当前捕获寄存器的个数*/
    if (0 == pstPsRegRptCfg->ulCnt)
    {
        return;
    }

    if(VOS_NO == g_ulPsReg)
    {
        return;
    }

    ulCnt = pstPsRegRptCfg->ulCnt;

    pstPsRegRptValue = (PS_REG_RPT_VALUE_STRU*)TTF_RING_Q_Rear(WUEPS_PID_REG, &g_stPsRegRptRingQ);/* 得到队列尾指针所指向的节点指针 */

    /* 环形队列已经装满,等待定时器超时，将元素取走再入队 */
    if (VOS_NULL_PTR == pstPsRegRptValue)
    {
        g_aulRegFullNum[enSysMode]++;

        return;
    }

    g_aulRegCaptureNum[enSysMode]++;

    /*读取寄存器*/
    for (ulRegRptCntLoop = 0; ulRegRptCntLoop < ulCnt; ulRegRptCntLoop++)
    {
        pstPsRegRptValue->aulRegValue[ulRegRptCntLoop] = *((volatile VOS_UINT32*)(pstPsRegRptCfg->aulRegAddr[ulRegRptCntLoop]));
    }

    pstPsRegRptValue->ulCnt     = ulCnt;
    pstPsRegRptValue->enSysMode = enSysMode;

    TTF_RING_Q_In(WUEPS_PID_REG, &g_stPsRegRptRingQ);

    return;
}
VOS_VOID PsRegRpt(VOS_VOID)
{
    PS_REG_RPT_VALUE_STRU       *pstPsRegRptMsg;
    VOS_UINT32                  ulRegRingQBufNumLoop;
    VOS_UINT32                  ulRingQCnt;
    VOS_UINT32                  ulTotalLen;

    ulRingQCnt = TTF_RING_Q_GetCnt(WUEPS_PID_REG, &g_stPsRegRptRingQ);

    if ( PS_REG_RPT_RBUF_NUM < ulRingQCnt )
    {
        DRV_SYSTEM_ERROR(OAM_REG_REPORT_ERROR_END, (VOS_INT)ulRingQCnt, 0, (VOS_CHAR *)&g_stPsRegRptRingQ, sizeof(g_stPsRegRptRingQ));

        return;
    }

    for (ulRegRingQBufNumLoop = 0; ulRegRingQBufNumLoop < ulRingQCnt; ulRegRingQBufNumLoop++)
    {
        pstPsRegRptMsg = (PS_REG_RPT_VALUE_STRU*)TTF_RING_Q_Out(WUEPS_PID_REG, &g_stPsRegRptRingQ);

        if (VOS_NULL_PTR == pstPsRegRptMsg)
        {
            continue;
        }

        /*根据不同模式填充发送和接收PID*/
        if (PS_REG_SYS_MODE_WCDMA == pstPsRegRptMsg->enSysMode)
        {
            g_stPsRegSimpleRptMsg.usPrimId      = ID_OM_APP_W_REG_IND;
            g_stPsRegSimpleRptMsg.ulSenderPid   = WUEPS_PID_HPA;
            g_stPsRegSimpleRptMsg.ulReceiverPid = WUEPS_PID_HPA;

            g_stPsRegSimpleRptMsg.stAppHeader.ucFuncType    = OM_TRANS_FUNC;
        }
#if  ( FEATURE_MULTI_MODEM == FEATURE_ON )
        else if (PS_REG_SYS_MODE_GSM1 == pstPsRegRptMsg->enSysMode)
        {
            g_stPsRegSimpleRptMsg.usPrimId      = ID_OM_APP_G_REG_IND;
            g_stPsRegSimpleRptMsg.ulSenderPid   = UEPS_PID_GHPA_PP;
            g_stPsRegSimpleRptMsg.ulReceiverPid = UEPS_PID_GHPA_PP;

            g_stPsRegSimpleRptMsg.stAppHeader.ucFuncType
                = OM_ADD_FUNCID_MODEMINFO(OM_TRANS_FUNC, MODEM_ID_1);
        }
#endif
        else
        {
            g_stPsRegSimpleRptMsg.usPrimId      = ID_OM_APP_G_REG_IND;
            g_stPsRegSimpleRptMsg.ulSenderPid   = UEPS_PID_GHPA_PP;
            g_stPsRegSimpleRptMsg.ulReceiverPid = UEPS_PID_GHPA_PP;

            g_stPsRegSimpleRptMsg.stAppHeader.ucFuncType    = OM_TRANS_FUNC;
        }

        /*寄存器个数*/
        ulTotalLen = pstPsRegRptMsg->ulCnt;
        g_stPsRegSimpleRptMsg.usCnt = (VOS_UINT16)ulTotalLen;

        /*寄存器内容总长度*/
        ulTotalLen *= sizeof(VOS_UINT32);
        VOS_MemCpy(g_stPsRegSimpleRptMsg.aulValue, pstPsRegRptMsg->aulRegValue, ulTotalLen);

        /*发送数据总长度*/
        ulTotalLen += offsetof(PS_REG_SIMPLE_RPT_MSG, aulValue);

        g_stPsRegSimpleRptMsg.stAppHeader.usLength = (VOS_UINT16)(ulTotalLen - VOS_OM_HEADER_LEN);
        g_stPsRegSimpleRptMsg.ulLength = (ulTotalLen - OM_APP_TRANS_LEN)
                                            - VOS_MSG_HEAD_LENGTH;

        OM_AddSNTime(&(g_stPsRegSimpleRptMsg.stAppHeader.ulSn),
                        &(g_stPsRegSimpleRptMsg.stAppHeader.ulTimeStamp));

        OM_SendData((OM_RSP_PACKET_STRU*)&g_stPsRegSimpleRptMsg, (VOS_UINT16)ulTotalLen);
    }

    return;
}


VOS_VOID PsRegRptStop(PS_REG_SYS_MODE_ENUM_UINT32 enSysMode)
{
    VOS_INT32                           lLockLevel;

    lLockLevel = VOS_SplIMP();

    PS_MEM_SET(&g_astPsRegRptCfg[enSysMode], 0, sizeof(PS_REG_RPT_CFG_STRU));

    /*如果W、G模寄存器个数为0，则将上报寄存器关闭*/
#if  ( FEATURE_MULTI_MODEM == FEATURE_ON )
    if ((0 == g_astPsRegRptCfg[PS_REG_SYS_MODE_WCDMA].ulCnt)
        &&(0 == g_astPsRegRptCfg[PS_REG_SYS_MODE_GSM].ulCnt)
        &&(0 == g_astPsRegRptCfg[PS_REG_SYS_MODE_GSM1].ulCnt)
        &&(VOS_NULL_PTR != g_PsRegRptTimer))
#else
    if ((0 == g_astPsRegRptCfg[PS_REG_SYS_MODE_WCDMA].ulCnt)
        &&(0 == g_astPsRegRptCfg[PS_REG_SYS_MODE_GSM].ulCnt)
        &&(VOS_NULL_PTR != g_PsRegRptTimer))
#endif
    {
        VOS_StopRelTimer(&g_PsRegRptTimer);
        g_PsRegRptTimer = VOS_NULL_PTR;

        PS_MEM_SET(g_astPsRegRptBuf, 0, sizeof(g_astPsRegRptBuf));
        TTF_RING_Q_Flush( WUEPS_PID_REG, &g_stPsRegRptRingQ );
    }

    VOS_Splx(lLockLevel);

    return;
}
VOS_VOID PsRegAdd(PS_REG_SYS_MODE_ENUM_UINT32 enSysMode, VOS_UINT32 ulRegAddr)
{
    PS_REG_RPT_CFG_STRU     *pstPsRegRptMsg;
    VOS_INT32               lLockLevel;

    lLockLevel = VOS_SplIMP();

    pstPsRegRptMsg = &g_astPsRegRptCfg[enSysMode];
    pstPsRegRptMsg->aulRegAddr[pstPsRegRptMsg->ulCnt++] = ulRegAddr;

    VOS_Splx(lLockLevel);

    return;
}


VOS_VOID PsRegDel(PS_REG_SYS_MODE_ENUM_UINT32 enSysMode, VOS_UINT32 ulRegAddr)
{
    PS_REG_RPT_CFG_STRU     *pstPsRegRptMsg;
    VOS_UINT32              ulRegNumLoop;
    VOS_UINT32              ulMoveNumLoop;
    VOS_INT32               lLockLevel;

    pstPsRegRptMsg = &g_astPsRegRptCfg[enSysMode];

    for (ulRegNumLoop = 0; ulRegNumLoop < pstPsRegRptMsg->ulCnt; ulRegNumLoop++)
    {
        if (ulRegAddr == pstPsRegRptMsg->aulRegAddr[ulRegNumLoop])
        {
            lLockLevel = VOS_SplIMP();

            for(ulMoveNumLoop = ulRegNumLoop; ulMoveNumLoop < (VOS_UINT32)(pstPsRegRptMsg->ulCnt - 1); ulMoveNumLoop++)
            {
                pstPsRegRptMsg->aulRegAddr[ulMoveNumLoop] = pstPsRegRptMsg->aulRegAddr[ulMoveNumLoop + 1];
            }

            pstPsRegRptMsg->ulCnt--;

            VOS_Splx(lLockLevel);

            break;
        }
    }

    return;
}


PS_BOOL_ENUM_UINT8 PsRegFind(PS_REG_SYS_MODE_ENUM_UINT32 enSysMode,
                                    VOS_UINT32 ulRegAddr)
{
    PS_REG_RPT_CFG_STRU     *pstPsRegRptMsg;
    VOS_UINT32              ulRegNumLoop;

    pstPsRegRptMsg = &g_astPsRegRptCfg[enSysMode];

    for(ulRegNumLoop = 0; ulRegNumLoop < pstPsRegRptMsg->ulCnt; ulRegNumLoop++)
    {
        if(ulRegAddr == pstPsRegRptMsg->aulRegAddr[ulRegNumLoop])
        {
            return PS_TRUE;
        }
    }

    return PS_FALSE;
}


VOS_UINT32 PsRegNumberGet(PS_REG_SYS_MODE_ENUM_UINT32 enSysMode)
{
    return g_astPsRegRptCfg[enSysMode].ulCnt;
}


VOS_VOID* PsRegsInfoAddrGet(PS_REG_SYS_MODE_ENUM_UINT32 enSysMode)
{
    return (VOS_VOID*)(g_astPsRegRptCfg[enSysMode].aulRegAddr);
}


VOS_VOID PsRegInit(VOS_VOID)
{
    VOS_VOID*   apNodeAddr[PS_REG_RPT_RBUF_NUM];
    VOS_UINT32  ulResult;
    VOS_UINT32  ulRingQCnt;

    VOS_MemSet(&g_stPsRegSimpleRptMsg,  0, sizeof(g_stPsRegSimpleRptMsg));
    VOS_MemSet(g_astPsRegRptCfg,        0, sizeof(g_astPsRegRptCfg));
    VOS_MemSet(g_astPsRegRptBuf,        0, sizeof(g_astPsRegRptBuf));

    /*寄存器上报给PC的数据初始化*/
    g_stPsRegSimpleRptMsg.stAppHeader.ucFuncType    = OM_TRANS_FUNC;
    g_stPsRegSimpleRptMsg.ulSenderCpuId             = VOS_LOCAL_CPUID;
    g_stPsRegSimpleRptMsg.ulReceiverCpuId           = VOS_LOCAL_CPUID;

    ulResult = TTF_RING_Q_Init(WUEPS_PID_REG, &g_stPsRegRptRingQ, PS_REG_RPT_RBUF_NUM);

    if (PS_SUCC != ulResult)
    {
        PS_LOG(WUEPS_PID_REG, 0, LOG_LEVEL_ERROR, "PsRegRingQInit: TTF_RING_Q_Init Fail !");
        return;
    }

    for (ulRingQCnt = 0; ulRingQCnt < PS_REG_RPT_RBUF_NUM; ulRingQCnt++)
    {
        apNodeAddr[ulRingQCnt] = &(g_astPsRegRptBuf[ulRingQCnt]);
    }

    TTF_RING_Q_NodeAddrSet(WUEPS_PID_REG, &g_stPsRegRptRingQ, apNodeAddr, PS_REG_RPT_RBUF_NUM);

    return;
}
VOS_VOID PsRegSndOmAppRegModCnf(VOS_UINT16 usTransPrimId,
                            VOS_UINT32 ulResult, OM_APP_REG_MODIFY_CNF_STRU* pPara)
{
    OM_APP_REG_MODIFY_CNF_STRU  *pOmAppRegModCnf;
    OM_APP_TRANS_CNF_STRU       *pstAppTransCnf;
    VOS_UINT32                   ulDataLen;

    ulDataLen   = offsetof(OM_APP_REG_MODIFY_CNF_STRU, aulRegAddr) + (pPara->ulRegNum*sizeof(VOS_UINT32));

    pstAppTransCnf = (OM_APP_TRANS_CNF_STRU *)VOS_MemAlloc(WUEPS_PID_OM, DYNAMIC_MEM_PT, 
                                       offsetof(OM_APP_TRANS_CNF_STRU, aucPara[0]) + ulDataLen);
    if (VOS_NULL_PTR == pstAppTransCnf)
    {
        PS_LOG(WUEPS_PID_OM, 0, LOG_LEVEL_ERROR, "Error: Alloc msg ID_OM_APP_REG_ADD_CNF fail!");
        return;
    }

    VOS_MemSet(pstAppTransCnf, 0, offsetof(OM_APP_TRANS_CNF_STRU, aucPara[0]) + ulDataLen);

    pstAppTransCnf->stAppHeader.ucFuncType     = OM_TRANS_FUNC;
    pstAppTransCnf->usPrimId                   = usTransPrimId;

    OM_AddSNTime(&(pstAppTransCnf->stAppHeader.ulSn), &(pstAppTransCnf->stAppHeader.ulTimeStamp));

    /* 下一字节至包尾的长度，单位为Byte */
    pstAppTransCnf->stAppHeader.usLength = (VOS_UINT16)((offsetof(OM_APP_TRANS_CNF_STRU, aucPara[0])
                                           - offsetof(OM_APP_TRANS_CNF_STRU, stAppHeader.ulSn))
                                           + ulDataLen);

    pOmAppRegModCnf             = (OM_APP_REG_MODIFY_CNF_STRU*)(pstAppTransCnf->aucPara);

    pOmAppRegModCnf->enSysMode  = pPara->enSysMode;
    pOmAppRegModCnf->ulErrCode  = ulResult;
    pOmAppRegModCnf->ulRegNum   = pPara->ulRegNum;

    /*将当前捕获的寄存器地址拷贝到发送数据中*/
    if ((PS_SUCC == ulResult) && (0 != pOmAppRegModCnf->ulRegNum))
    {
        PS_MEM_CPY(pOmAppRegModCnf->aulRegAddr, PsRegsInfoAddrGet(pPara->enSysMode),
                    sizeof(VOS_UINT32)*pOmAppRegModCnf->ulRegNum);
    }
 
    if (VOS_OK != OM_SendData((OM_RSP_PACKET_STRU*)pstAppTransCnf,
                              (VOS_UINT16)(pstAppTransCnf->stAppHeader.usLength + VOS_OM_HEADER_LEN)))
    {
        PS_LOG(WUEPS_PID_OM, 0, LOG_LEVEL_ERROR, "Error: Send msg ID_OM_APP_REG_ADD_CNF fail!");
    }
    else
    {
        PS_LOG(WUEPS_PID_OM, 0, LOG_LEVEL_INFO, "Info: Send msg ID_OM_APP_REG_ADD_CNF success!");
    }

    VOS_MemFree(WUEPS_PID_OM, pstAppTransCnf);

    return ;
}


VOS_VOID PsRegRcvAppOmRegStopReq(VOS_VOID* pRcvMsg)
{
    OM_APP_REG_MODIFY_CNF_STRU  stOmAppRegCnf;

    OM_REG_RPT_STOP_STRU *pMsg = (OM_REG_RPT_STOP_STRU*)pRcvMsg;

    stOmAppRegCnf.enSysMode = pMsg->enSysMode;
    stOmAppRegCnf.ulRegNum  = 0;

    if (PS_REG_SYS_MODE_BUTT <= pMsg->enSysMode)
    {
        PS_LOG1(WUEPS_PID_REG, 0, LOG_LEVEL_WARNING, "Config system mode is wrong!", (VOS_INT32)pMsg->enSysMode);
        PsRegSndOmAppRegModCnf(ID_APP_OM_REG_STOP_CNF, PS_REG_MODE_ERR, &stOmAppRegCnf);
        return;
    }

    /*寄存器上报功能已经处于关闭状态，则打印信息*/
    if(0 == PsRegNumberGet(pMsg->enSysMode))
    {
        PS_LOG(WUEPS_PID_REG, 0, LOG_LEVEL_WARNING, "RegRptStart Function has been stoped!");
        PsRegSndOmAppRegModCnf(ID_APP_OM_REG_STOP_CNF, PS_REG_STOP_ERR, &stOmAppRegCnf);
        return;
    }

    PsRegRptStop(pMsg->enSysMode);

    PsRegSndOmAppRegModCnf(ID_APP_OM_REG_STOP_CNF, PS_REG_OK, &stOmAppRegCnf);

    return;
}
VOS_VOID PsRegRcvAppOmRegAddReq(VOS_VOID* pMsg)
{
    APP_OM_REG_MODIFY_REQ_STRU  *pRcvMsg = (APP_OM_REG_MODIFY_REQ_STRU*)pMsg;
    OM_APP_REG_MODIFY_CNF_STRU  stOmAppRegCnf;
    VOS_UINT32                  ulRegNumLoop;
    VOS_UINT32                  ulOldRegNum;
    VOS_UINT32                  ulValidRegNum = 0;
    PS_REG_SYS_MODE_ENUM_UINT32 enSysMode;

    enSysMode               = pRcvMsg->enSysMode;
    stOmAppRegCnf.enSysMode = enSysMode;
    stOmAppRegCnf.ulRegNum  = 0;
    /*配置模式检查*/
    if (PS_REG_SYS_MODE_BUTT <= enSysMode)
    {
        PS_LOG1(WUEPS_PID_REG, 0, LOG_LEVEL_WARNING, "Config system mode is wrong!", (VOS_INT32)enSysMode);
        PsRegSndOmAppRegModCnf(ID_OM_APP_REG_ADD_CNF, PS_REG_MODE_ERR, &stOmAppRegCnf);
        return;
    }

    /*寄存器个数检查*/
    if (PS_REG_RPT_MAX_NUM < pRcvMsg->ulRegNum)
    {
        PS_LOG1(WUEPS_PID_REG, 0, LOG_LEVEL_WARNING, "Only Support 1024 Regist Number At Most!", (VOS_INT32)pRcvMsg->ulRegNum);
        PsRegSndOmAppRegModCnf(ID_OM_APP_REG_ADD_CNF, PS_REG_NUM_ERR, &stOmAppRegCnf);
        return;
    }

    /* 计算新配置里新增的寄存器个数 */
    for (ulRegNumLoop = 0; ulRegNumLoop < pRcvMsg->ulRegNum; ulRegNumLoop++)
    {
        if (0 != (pRcvMsg->aulRegAddr[ulRegNumLoop]&HPA_ARM_ALIGNMENT))/* 非四字节对齐的地址不能上报 */
        {
            PS_LOG(WUEPS_PID_REG, 0, LOG_LEVEL_WARNING, "Invalid Address!");
            PsRegSndOmAppRegModCnf(ID_OM_APP_REG_ADD_CNF, PS_REG_ALIGN_ERR, &stOmAppRegCnf);
            return;
        }

        if (PS_FALSE == PsRegFind(enSysMode, pRcvMsg->aulRegAddr[ulRegNumLoop]))
        {
            ulValidRegNum++;
        }
    }

    /* 新增寄存器个数为0，直接返回成功 */
    if (0 == ulValidRegNum)
    {
        stOmAppRegCnf.ulRegNum = PsRegNumberGet(enSysMode);
        PsRegSndOmAppRegModCnf(ID_OM_APP_REG_ADD_CNF, PS_REG_OK, &stOmAppRegCnf);
        return;
    }

    ulOldRegNum = PsRegNumberGet(enSysMode);

    /*判断寄存器个数是否超过最大值*/
    if ((PS_REG_SYS_MODE_WCDMA == enSysMode)
        &&(PS_REG_RPT_MAX_NUM < (ulOldRegNum + ulValidRegNum)))
    {
        stOmAppRegCnf.ulRegNum  = (ulOldRegNum + ulValidRegNum) - PS_REG_RPT_MAX_NUM;
        PsRegSndOmAppRegModCnf(ID_OM_APP_REG_ADD_CNF, PS_REG_NUM_OVERFLOW, &stOmAppRegCnf);
        return;
    }

#if  ( FEATURE_MULTI_MODEM == FEATURE_ON )  /*工具对于两个Modem的G模寄存器总数进行限制*/
    if (((PS_REG_SYS_MODE_GSM == enSysMode)||(PS_REG_SYS_MODE_GSM1 == enSysMode))
        &&(g_stGhpaRegCnt.usRegCnt < (ulOldRegNum + ulValidRegNum)))
#else
    if ((PS_REG_SYS_MODE_GSM == enSysMode)
        &&(g_stGhpaRegCnt.usRegCnt < (ulOldRegNum + ulValidRegNum)))
#endif
    {
        stOmAppRegCnf.ulRegNum  = (ulOldRegNum + ulValidRegNum) - g_stGhpaRegCnt.usRegCnt;
        PsRegSndOmAppRegModCnf(ID_OM_APP_REG_ADD_CNF, PS_REG_NUM_OVERFLOW, &stOmAppRegCnf);
        return;
    }

    /* 将新增的寄存器全部保存到配置里 */
    for (ulRegNumLoop = 0; ulRegNumLoop < pRcvMsg->ulRegNum; ulRegNumLoop++)
    {
        if (PS_FALSE == PsRegFind(enSysMode, pRcvMsg->aulRegAddr[ulRegNumLoop]))
        {
            PsRegAdd(enSysMode, pRcvMsg->aulRegAddr[ulRegNumLoop]);
        }
    }

    /*如果定时器没有开启，则不需要再次启动*/
    if (VOS_NULL_PTR == g_PsRegRptTimer)
    {
        if (VOS_OK != VOS_StartCallBackRelTimer(&g_PsRegRptTimer,
                                    WUEPS_PID_HPA,
                                    PS_REG_RPT_FREQ_LEN,
                                    0,
                                    0,
                                    VOS_RELTIMER_LOOP,
                                    (REL_TIMER_FUNC)PsRegRpt,
                                    VOS_TIMER_NO_PRECISION))
        {
            PS_LOG(WUEPS_PID_REG, 0, LOG_LEVEL_WARNING, "VOS_StartCallBackRelTimer Fail!");
        }
    }

    stOmAppRegCnf.ulRegNum = PsRegNumberGet(enSysMode);
    PsRegSndOmAppRegModCnf(ID_OM_APP_REG_ADD_CNF, PS_REG_OK, &stOmAppRegCnf);

    return;
}
VOS_VOID PsRegRcvAppOmRegDelReq(VOS_VOID* pMsg)
{
    PS_REG_SYS_MODE_ENUM_UINT32 enSysMode;
    APP_OM_REG_MODIFY_REQ_STRU  *pRcvMsg = (APP_OM_REG_MODIFY_REQ_STRU*)pMsg;
    OM_APP_REG_MODIFY_CNF_STRU  stOmAppRegCnf;
    VOS_UINT32                  ulRegCnt;
    VOS_UINT32                  ulRegNumLoop;

    enSysMode               = pRcvMsg->enSysMode;
    stOmAppRegCnf.enSysMode = enSysMode;
    stOmAppRegCnf.ulRegNum  = 0;

    /*配置模式检查*/
    if (PS_REG_SYS_MODE_BUTT <= enSysMode)
    {
        PS_LOG1(WUEPS_PID_REG, 0, LOG_LEVEL_WARNING, "Config system mode is wrong!", (VOS_INT32)enSysMode);
        PsRegSndOmAppRegModCnf(ID_OM_APP_REG_DEL_CNF, PS_REG_MODE_ERR, &stOmAppRegCnf);
        return;
    }

    /*寄存器个数检查*/
    if (PS_REG_RPT_MAX_NUM < pRcvMsg->ulRegNum)
    {
        PS_LOG(WUEPS_PID_REG, 0, LOG_LEVEL_WARNING, "RegRpt number is not in[1,1024]!");
        PsRegSndOmAppRegModCnf(ID_OM_APP_REG_DEL_CNF, PS_REG_NUM_ERR, &stOmAppRegCnf);
        return;
    }

    for (ulRegNumLoop = 0; ulRegNumLoop < pRcvMsg->ulRegNum; ulRegNumLoop++)
    {
        /*非四字节对齐的地址不能上报*/
        if (0 != (pRcvMsg->aulRegAddr[ulRegNumLoop]&HPA_ARM_ALIGNMENT))
        {
            PS_LOG(WUEPS_PID_REG, 0, LOG_LEVEL_WARNING, "nInvalid Address!");
            PsRegSndOmAppRegModCnf(ID_OM_APP_REG_DEL_CNF, PS_REG_ALIGN_ERR, &stOmAppRegCnf);
            return;
        }

        PsRegDel(enSysMode, pRcvMsg->aulRegAddr[ulRegNumLoop]);
    }

    ulRegCnt = PsRegNumberGet(enSysMode);

    /*如果寄存器全部清除，停止寄存器上报*/
    if (0 == ulRegCnt)
    {
        PsRegRptStop(enSysMode);
    }

    stOmAppRegCnf.ulRegNum = ulRegCnt;
    PsRegSndOmAppRegModCnf(ID_OM_APP_REG_DEL_CNF, PS_REG_OK, &stOmAppRegCnf);

    return;
}


VOS_VOID  Reg_MsgProc(MsgBlock *pRcvMsg)
{
    OM_RecordInfoStart(VOS_EXC_DUMP_MEM_NUM_2, pRcvMsg->ulSenderPid, WUEPS_PID_REG, *((VOS_UINT32*)pRcvMsg->aucValue));

    switch ( *((VOS_UINT16 *)((VOS_UINT8 *)(pRcvMsg) + VOS_MSG_HEAD_LENGTH)) )
    {
        case ID_APP_OM_REG_STOP_REQ:
            PsRegRcvAppOmRegStopReq(pRcvMsg);
            break;

        case ID_APP_OM_REG_ADD_REQ:
            PsRegRcvAppOmRegAddReq(pRcvMsg);
            break;

        case ID_APP_OM_REG_DEL_REQ:
            PsRegRcvAppOmRegDelReq(pRcvMsg);
            break;

        default:                                                                /* 无效的消息ID */
            PS_LOG2(WUEPS_PID_REG, 0, LOG_LEVEL_WARNING, "not expected msg! ulSenderPid<1>,enMsgName<2>",(VOS_INT32)(pRcvMsg->ulSenderPid),(VOS_INT32)(*((VOS_UINT16 *)((VOS_UINT8 *)(pRcvMsg) + VOS_MSG_HEAD_LENGTH))));
            break;
    }

    OM_RecordInfoEnd(VOS_EXC_DUMP_MEM_NUM_2);
}
VOS_UINT32 WuepsRegPidInit(enum VOS_INIT_PHASE_DEFINE ip)
{
    switch( ip )
    {
        case VOS_IP_LOAD_CONFIG:
            PsRegInit();
            break;

        default:
            break;
    }

    return VOS_OK;
}

#ifdef  __cplusplus
  #if  __cplusplus
  }
  #endif
#endif

