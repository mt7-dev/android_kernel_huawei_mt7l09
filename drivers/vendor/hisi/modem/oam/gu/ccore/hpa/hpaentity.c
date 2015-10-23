/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: HPAEntity.c                                                     */
/*                                                                           */
/* Author: Xu cheng                                                          */
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date: 2008-02                                                             */
/*                                                                           */
/* Description: implement HPA subroutine                                     */
/*                                                                           */
/* Others:                                                                   */
/*                                                                           */
/* History:                                                                  */
/* 1. Date: 2008-02                                                          */
/*    Author: Xu cheng                                                       */
/*    Modification: Create this file                                         */
/*                                                                           */
/*****************************************************************************/

#include "hpacomm.h"

#include "bbp_wcdma_interface.h"
#include "DspInterface.h"
#include "apminterface.h"
#include "omprivate.h"
#include "sleepflow.h"
#include "phyoaminterface.h"
#include "rfa.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */


/* Macro of log */
#define    THIS_FILE_ID        PS_FILE_ID_HPA_ENTITY_C

extern VOS_VOID OM_NoSigFilterMsg(MsgBlock *pMsg);

/*****************************************************************************
 Function   : HPA_ReadCfnSfn
 Description: Read SFN & CFN from register
 Input      : void
 Return     : void
 Other      :
 *****************************************************************************/
VOS_VOID HPA_ReadCfnSfn(VOS_VOID)
{
    VOS_UINT16 usHpaCfnRead;

    /* Read SFN */
    g_usHpaSfnRead = (VOS_UINT16)HPA_Read32Reg(WBBP_SFN_ADDR);
    g_usHpaSfnRead &= 0x0FFF; /* SFN Last 12 bit is vaild  */

    /* Read CFN */
    usHpaCfnRead = (VOS_UINT16)HPA_Read32Reg(WBBP_CFN_ADDR);
    /* SFN Last 8 bit is vaild  */
    g_ucHpaCfnRead = (VOS_UCHAR)(usHpaCfnRead & 0xFF);

    /* Tell MAC */
    /*g_stMacFnInfoInd.enMsgName = ID_PHY_MAC_INFO_IND;
    g_stMacFnInfoInd.usCfn = (VOS_UINT16)g_ucHpaCfnRead;
    g_stMacFnInfoInd.ulSfn = (VOS_UINT32)g_usHpaSfnRead;*/

    return;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */


