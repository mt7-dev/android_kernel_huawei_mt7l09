


/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
#include  "NasEmmcPublic.h"
#include  "NasEmmcSendMsg.h"
#include  "NasLmmPubMOm.h"

/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_NASEMMCPUBLIC_C
/*lint +e767*/

/*****************************************************************************
  1.1 Cplusplus Announce
*****************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
/*****************************************************************************
  2 Declare the Global Variable
*****************************************************************************/
/* 全局变量g_stEmmcGlobalCtrl的定义 */
NAS_EMMC_GLOBAL_STRU                    g_stEmmcGlobalCtrl;



/*****************************************************************************
  3 Function
*****************************************************************************/

VOS_VOID NAS_EMMC_FillRrcNasForbTaList
(
    LRRC_LNAS_FORB_TA_LIST_STRU           *pstRrcNasForbTaList
)
{
    VOS_UINT32                          i = 0;
    NAS_MM_TA_LIST_STRU                *pstForbTaListForRoaming = NAS_EMMC_NULL_PTR;

    /*get ForbTaListForRoaming and ForbTaListForRpos*/
    pstForbTaListForRoaming = (VOS_VOID *)NAS_EMM_GetEmmGlobleAddr(NAS_LMM_GLOBLE_FORB_TA_FOR_ROAMING);

    /* Fill ForbTaListForRoaming */
    for ( i=0; i < pstForbTaListForRoaming->ulTaNum; i++)
    {
        pstRrcNasForbTaList->astForbTa[i].stTac.ucTac
            = pstForbTaListForRoaming->astTa[i].stTac.ucTac;
        pstRrcNasForbTaList->astForbTa[i].stTac.ucTacCont
            = pstForbTaListForRoaming->astTa[i].stTac.ucTacCnt;
        pstRrcNasForbTaList->astForbTa[i].stPlmnId.aucPlmnId[0]
            = pstForbTaListForRoaming->astTa[i].stPlmnId.aucPlmnId[0];
        pstRrcNasForbTaList->astForbTa[i].stPlmnId.aucPlmnId[1]
            = pstForbTaListForRoaming->astTa[i].stPlmnId.aucPlmnId[1];
        pstRrcNasForbTaList->astForbTa[i].stPlmnId.aucPlmnId[2]
            = pstForbTaListForRoaming->astTa[i].stPlmnId.aucPlmnId[2];
    }

    /*caculate forbidden TA num*/
    pstRrcNasForbTaList->ulTaNum  = pstForbTaListForRoaming->ulTaNum ;

    return;
}
/*lint -e960*/
/*lint -e961*/
VOS_VOID  NAS_EMMC_FillUserSpecRrcNasForbTaList
(
    const NAS_MM_PLMN_ID_STRU  *pstPlmnId,
    NAS_MM_TA_LIST_STRU  *pstTaList
)
{

    NAS_MM_TA_LIST_STRU                *pstForbTaListForRoaming = NAS_EMMC_NULL_PTR;
    NAS_LMM_NETWORK_INFO_STRU           *pstNetInfo = NAS_EMMC_NULL_PTR;
    VOS_UINT32                          i = 0;
    VOS_UINT32                          j = 0;

    if ((NAS_EMMC_NULL_PTR == pstPlmnId) ||
        (NAS_EMMC_NULL_PTR == pstTaList))
    {
        NAS_EMM_PUBU_LOG_ERR("NAS_EMMC_FillUserSpecRrcNasForbTaList: Input is Null");
        return;
    }

    /*get ForbTaListForRoaming and ForbTaListForRpos*/
    pstForbTaListForRoaming = (VOS_VOID *)NAS_EMM_GetEmmGlobleAddr(NAS_LMM_GLOBLE_FORB_TA_FOR_ROAMING);
    pstNetInfo = (VOS_VOID*)NAS_EMM_GetEmmGlobleAddr(NAS_LMM_GLOBLE_NET_INFO);
    for (i = 0; i < pstForbTaListForRoaming->ulTaNum; i++)
    {
        if (NAS_LMM_MATCH_SUCCESS != NAS_LMM_PlmnMatch(pstPlmnId,&pstForbTaListForRoaming->astTa[i].stPlmnId))
        {
            NAS_LMM_MEM_CPY(&pstTaList->astTa[j],
                            &pstForbTaListForRoaming->astTa[i],
                            sizeof(NAS_MM_TA_STRU));
            j++;

        }

    }
    pstTaList->ulTaNum = j;

    NAS_EMM_PUBU_LOG1_NORM("NAS_EMMC_FillUserSpecRrcNasForbTaList:Rej15 TA NUM =, ",
                                   pstNetInfo->stForbTaForRej15.ulTaNum);

    for(i = 0; i < pstNetInfo->stForbTaForRej15.ulTaNum; i++ )
    {

       NAS_LMM_AddTaInTaList(   &pstNetInfo->stForbTaForRej15.astTa[i],
                                pstTaList,
                                NAS_LMM_MAX_FORBTA_NUM);
    }

    NAS_EMM_PUBU_LOG1_NORM("NAS_EMMC_FillUserSpecRrcNasForbTaList:RRC FORB TA NUM =, ",
                                       pstTaList->ulTaNum);
    return;
}
VOS_VOID NAS_EMMC_GetCurrentTa( NAS_MM_TA_STRU *pstTa )
{
    NAS_MM_NETWORK_ID_STRU              *pstAreaInfo;

    if (NAS_EMMC_NULL_PTR == pstTa)
    {
        NAS_LMM_EMMC_LOG_ERR("NAS_EMMC_GetCurrentTa:input ptr null!");
        return;
    }
    pstAreaInfo = (VOS_VOID *)NAS_EMM_GetEmmGlobleAddr(NAS_LMM_GLOBLE_PRESENT_NET_ID);

    NAS_LMM_PlmnCpy(&(pstTa->stPlmnId), &pstAreaInfo->stPlmnId);

    pstTa->stTac.ucTac                  = pstAreaInfo->stTac.ucTac;

    pstTa->stTac.ucTacCnt               = pstAreaInfo->stTac.ucTacCnt;

    return;
}



VOS_VOID  NAS_EMMC_GetForbPlmnInfo
(
    NAS_MM_PLMN_LIST_STRU               *pstFPlmn,
    NAS_MM_PLMN_LIST_STRU               *pstFPlmnForGprs
)
{
    MMC_LMM_PUB_INFO_STRU               stFPlmnInfo;
    MMC_LMM_PUB_INFO_STRU               stFPlmnForGprsInfo;
    MMC_LMM_RESULT_ID_ENUM_UINT32       ulErrInd;

    if ((NAS_EMMC_NULL_PTR == pstFPlmn) ||
        (NAS_EMMC_NULL_PTR == pstFPlmnForGprs))
    {
        NAS_LMM_EMMC_LOG_ERR("NAS_EMMC_GetForbPlmnInfo:input ptr null!");
        return;
    }

    NAS_LMM_MEM_SET(&stFPlmnInfo,0,sizeof(MMC_LMM_PUB_INFO_STRU));
    NAS_LMM_MEM_SET(&stFPlmnForGprsInfo,0,sizeof(MMC_LMM_PUB_INFO_STRU));
    /*PC REPLAY MODIFY BY LEILI BEGIN*/
    /*获取禁止PLMN*/
    ulErrInd = NAS_EMM_GetGulPubInfo(MMC_LMM_FPLMN,&stFPlmnInfo);

    if (MMC_LMM_SUCC != ulErrInd)
    {
        NAS_LMM_EMMC_LOG_NORM("NAS_EMMC_GetCurrentForbInfo: FPLMN Invalid");
    }

    /*获取禁止PLMN for GPRS*/
    ulErrInd = NAS_EMM_GetGulPubInfo(MMC_LMM_FPLMN_FOR_GPRS,&stFPlmnForGprsInfo);

    if (MMC_LMM_SUCC != ulErrInd)
    {
        NAS_LMM_EMMC_LOG_NORM("NAS_EMMC_GetCurrentForbInfo: FPLMN FOR GRPS Invalid");
    }
    /*PC REPLAY MODIFY BY LEILI END*/
    NAS_LMM_PlmnListCpy(pstFPlmn, (NAS_MM_PLMN_LIST_STRU *)&(stFPlmnInfo.u.stForbiddenPlmnList));
    NAS_LMM_PlmnListCpy(pstFPlmnForGprs, (NAS_MM_PLMN_LIST_STRU *)&(stFPlmnForGprsInfo.u.stForbiddenPlmnListForGPRS));

    return;
}




VOS_VOID NAS_EMMC_GetCurrentForbInfo
(
    EMMC_EMM_FORBIDDEN_INFO_ENUM_UINT32  *pulForbdInfo
)
{
    NAS_MM_NETWORK_ID_STRU              *pstAreaInfo;
    NAS_MM_PLMN_LIST_STRU               stForbPlmn;
    NAS_MM_PLMN_LIST_STRU               stForbPlmnForGprs;
    NAS_MMC_TA_LIST_STRU                *pstForbTaForRoming;
    NAS_MMC_TA_LIST_STRU                *pstForbTaForRegional;
    NAS_MMC_TA_STRU                     stTa;

    EMMC_EMM_FORBIDDEN_INFO_ENUM_UINT32  ulForbdInfo;

    if (NAS_EMMC_NULL_PTR == pulForbdInfo)
    {
        NAS_LMM_EMMC_LOG_ERR("NAS_EMMC_GetCurrentForbInfo:input ptr null!");
        return;
    }

    ulForbdInfo                         = EMMC_EMM_NO_FORBIDDEN;
    pstAreaInfo = (VOS_VOID *)NAS_EMM_GetEmmGlobleAddr(NAS_LMM_GLOBLE_PRESENT_NET_ID);

    NAS_LMM_MEM_SET(&stForbPlmn,0,sizeof(NAS_MM_PLMN_LIST_STRU));
    NAS_LMM_MEM_SET(&stForbPlmnForGprs,0,sizeof(NAS_MM_PLMN_LIST_STRU));

    NAS_EMMC_GetForbPlmnInfo(&stForbPlmn,&stForbPlmnForGprs);

    pstForbTaForRoming = (VOS_VOID *)NAS_EMM_GetEmmGlobleAddr(NAS_LMM_GLOBLE_FORB_TA_FOR_ROAMING);
    pstForbTaForRegional = (VOS_VOID *)NAS_EMM_GetEmmGlobleAddr(NAS_LMM_GLOBLE_FORB_TA_FOR_RPOS);

    NAS_EMMC_GetCurrentTa(&stTa);

    if (NAS_LMM_MATCH_SUCCESS ==
           NAS_LMM_PlmnIDMatchPlmnList(&pstAreaInfo->stPlmnId, &stForbPlmn))
    {
        ulForbdInfo                     = EMMC_EMM_FORBIDDEN_PLMN;
    }
    else if (NAS_LMM_MATCH_SUCCESS ==
            NAS_LMM_PlmnIDMatchPlmnList(&pstAreaInfo->stPlmnId,&stForbPlmnForGprs))
    {
        ulForbdInfo                     = EMMC_EMM_FORBIDDEN_PLMN_FOR_GPRS;
    }
    else if (NAS_LMM_MATCH_SUCCESS == NAS_LMM_TaMatchTaList(&stTa, pstForbTaForRoming))
    {
        ulForbdInfo                     = EMMC_EMM_FORBIDDEN_TA_FOR_ROAMING;
    }
    else if (NAS_LMM_MATCH_SUCCESS == NAS_LMM_TaMatchTaList(&stTa, pstForbTaForRegional))
    {
        ulForbdInfo                     = EMMC_EMM_FORBIDDEN_TA_FOR_RPOS;
    }
    else
    {
    }

    *pulForbdInfo                       = ulForbdInfo;

    return;


}


/* lihong00150010 emergency tau&service begin */

VOS_UINT32 NAS_LMM_IsCellStatusNormal
(
    MMC_LMM_PLMN_ID_STRU               *pstPlmn,
    const   NAS_MM_TA_STRU             *pstTa
)
{
    const NAS_MM_TA_LIST_STRU                *pstForbTaListForRoaming = VOS_NULL_PTR;
    const NAS_MM_TA_LIST_STRU                *pstForbTaListForRpos = VOS_NULL_PTR;

    pstForbTaListForRoaming = (VOS_VOID *)NAS_EMM_GetEmmGlobleAddr(NAS_LMM_GLOBLE_FORB_TA_FOR_ROAMING);
    pstForbTaListForRpos = (VOS_VOID *)NAS_EMM_GetEmmGlobleAddr(NAS_LMM_GLOBLE_FORB_TA_FOR_RPOS);

    NAS_LMM_EMMC_LOG2_NORM("NAS_LMM_IsCellStatusNormal:NAS_LMM_GetSimState = ,NAS_LMM_GetPsSimValidity",
                            NAS_LMM_GetSimState(),NAS_LMM_GetPsSimValidity());
    NAS_LMM_EMMC_LOG2_NORM("NAS_LMM_IsCellStatusNormal:NAS_MML_GetSimType = ,Nas_PlmnIdIsForbid",
                            NAS_MML_GetSimType(),Nas_PlmnIdIsForbid(pstPlmn));
    NAS_LMM_EMMC_LOG2_NORM("NAS_LMM_IsCellStatusNormal:NAS_LMM_TaMatchTaList = ,NAS_LMM_TaMatchTaList",
                            NAS_LMM_TaMatchTaList(pstTa, pstForbTaListForRoaming),NAS_LMM_TaMatchTaList(pstTa, pstForbTaListForRpos));

    if ((NAS_LMM_SIM_STATUS_AVAILABLE == NAS_LMM_GetSimState())
        &&(NAS_LMM_SIM_VALID == NAS_LMM_GetPsSimValidity())
        &&(NAS_MML_SIM_TYPE_USIM == NAS_MML_GetSimType())
        &&(MMC_LMM_PLMN_ALLOW_REG == Nas_PlmnIdIsForbid(pstPlmn))
        &&(NAS_LMM_MATCH_FAIL == NAS_LMM_TaMatchTaList(pstTa, pstForbTaListForRoaming))
        &&(NAS_LMM_MATCH_FAIL == NAS_LMM_TaMatchTaList(pstTa, pstForbTaListForRpos)))
    {
        return VOS_TRUE;
    }

    return VOS_FALSE;
}
/* lihong00150010 emergency tau&service end */

VOS_VOID  NAS_EMMC_ProcessMmcLteSysInfoInd
(
    MMC_LMM_SYS_INFO_STRU        *pstLteInfoIndMsg,
    EMMC_EMM_CHANGE_INFO_ENUM_UINT32   *pulChangeInfo,
    EMMC_EMM_CELL_STATUS_ENUM_UINT32   *penCellStatus,
    EMMC_EMM_FORBIDDEN_INFO_ENUM_UINT32 *penForbdInfo
)
{
    EMMC_EMM_CHANGE_INFO_ENUM_UINT32    ulChangeInfo = EMMC_EMM_NO_CHANGE;
    NAS_MM_NETWORK_ID_STRU             *pstAreaInfo;
    NAS_LMM_NETWORK_INFO_STRU           *pstNetInfo;
    NAS_MM_TA_STRU                      stTa;
    /* lihong00150010 emergency tau&service begin */
    EMMC_EMM_CELL_STATUS_ENUM_UINT32    enCellStatus = EMMC_EMM_CELL_STATUS_NORMAL;
    EMMC_EMM_FORBIDDEN_INFO_ENUM_UINT32 enForbdInfo = EMMC_EMM_NO_FORBIDDEN;
    /* lihong00150010 emergency tau&service end */

    NAS_LMM_EMMC_LOG_INFO("NAS_EMMC_ProcessMmcLteSysInfoInd is entered.");

    pstNetInfo = (VOS_VOID *)NAS_EMM_GetEmmGlobleAddr(NAS_LMM_GLOBLE_NET_INFO);

    pstNetInfo->bitOpPresentNetId = NAS_EMMC_BIT_SLCT;

    pstAreaInfo = (VOS_VOID *)NAS_EMM_GetEmmGlobleAddr(NAS_LMM_GLOBLE_PRESENT_NET_ID);


    /*比较系统消息与UE维护的当前网络信息,获取信息变更类型*/
    if(NAS_LMM_MATCH_FAIL ==
            NAS_LMM_PlmnMatch(&(pstAreaInfo->stPlmnId),
                (NAS_MM_PLMN_ID_STRU *)pstLteInfoIndMsg->stSpecPlmnIdList.astSuitPlmnList))
    {
        ulChangeInfo                    = EMMC_EMM_CHANGE_PLMN;
        NAS_LMM_EMMC_LOG_INFO("NAS_EMMC_ProcessMmcLteSysInfoInd: MMC_EMM_CHANGE_PLMN");
    }
    else if(NAS_LMM_MATCH_FAIL ==
                NAS_LMM_TacMatch(&(pstAreaInfo->stTac),
                    (NAS_MM_TAC_STRU *)&(pstLteInfoIndMsg->stTac)))
    {
        ulChangeInfo                    = EMMC_EMM_CHANGE_TA;
        NAS_LMM_EMMC_LOG_INFO("NAS_EMMC_ProcessMmcLteSysInfoInd: MMC_EMM_CHANGE_TA");
    }
    else if (pstAreaInfo->ulCellId != pstLteInfoIndMsg->ulCellId)
    {
        ulChangeInfo                    = EMMC_EMM_CHANGE_CELL;
    }
    else
    {
        ulChangeInfo                    = EMMC_EMM_NO_CHANGE;
    }

    #if 0
    /*初始化低质量Plmn List*/
    stPlmnList.ulLowPlmnNum = 0;

    /*构造高质量Plmn List*/
    stPlmnList.ulHighPlmnNum = pstRrcInfoIndMsg->stSpecPlmnIdList.ulSuitPlmnNum;
    NAS_LMM_MEM_CPY((VOS_VOID *)stPlmnList.astHighPlmnList,
               (const VOS_VOID *)pstRrcInfoIndMsg->stSpecPlmnIdList.astSuitPlmnList,
               stPlmnList.ulHighPlmnNum * sizeof(NAS_MMC_PLMN_ID_STRU));

    /*将RRC上报系统消息中的PLMN添加至Available Plmn List*/
    NAS_MMC_UpdateAvailablePlmnList(NAS_MMC_GetAutoMode(), &stPlmnList);
    #endif

    NAS_LMM_PlmnCpy(&pstAreaInfo->stPlmnId,
            (NAS_MM_PLMN_ID_STRU *)&pstLteInfoIndMsg->stSpecPlmnIdList.astSuitPlmnList[0]);


    pstAreaInfo->stTac.ucTac            = pstLteInfoIndMsg->stTac.ucTac;
    pstAreaInfo->stTac.ucTacCnt         = pstLteInfoIndMsg->stTac.ucTacCnt;

    pstAreaInfo->ulCellId               = pstLteInfoIndMsg->ulCellId;
    /* lihong00150010 emergency tau&service begin */
    /* 获取禁止信息 */
    if (NAS_EMMC_USER_SPEC_PLMN_YES == NAS_EMMC_GetUserSpecPlmnFlag())
    {
       enForbdInfo =  EMMC_EMM_NO_FORBIDDEN;
    }
    else
    {
        NAS_EMMC_GetCurrentForbInfo(&enForbdInfo);
    }

    pstAreaInfo->ulForbiddenInfo = enForbdInfo;

    /*此修改保证了cell status状态不被重写， 不然可能会导致anycell搜到可用
    的网络的时候，emm 状态还是受限服务态*/
    NAS_EMM_GetCurrentTa(&stTa);

    NAS_LMM_EMMC_LOG1_NORM("NAS_EMMC_ProcessMmcLteSysInfoInd:cellstauts = ",
                            pstLteInfoIndMsg->enCellStatusInd);

    if (LRRC_LNAS_CELL_STATUS_NORMAL == pstLteInfoIndMsg->enCellStatusInd)
    {

        enCellStatus = EMMC_EMM_CELL_STATUS_NORMAL;
    }
    else if (LRRC_LNAS_CELL_STATUS_ANYCELL== pstLteInfoIndMsg->enCellStatusInd)
    {
        /*当前PLMN允许注册（MML API函数），USIM卡存在、USIM卡有效、且SIM卡类型为USIM、TA不被禁*/
        /* lihong00150010 emergency tau&service begin */
        if (VOS_TRUE == NAS_LMM_IsCellStatusNormal( &pstLteInfoIndMsg->stSpecPlmnIdList.astSuitPlmnList[0],
                                                    &stTa))
        {
            NAS_LMM_EMMC_LOG_NORM("NAS_EMMC_ProcessMmcLteSysInfoInd: anycell search find avaiable cell,cellStatus: NORMAL");
            enCellStatus = EMMC_EMM_CELL_STATUS_NORMAL;
        }
        else
        {
            NAS_LMM_EMMC_LOG_NORM("NAS_EMMC_ProcessMmcLteSysInfoInd: anycell search don't find avaiable cell, cellStatus: ANYCELL");
            enCellStatus = EMMC_EMM_CELL_STATUS_ANYCELL;
        }
        /* lihong00150010 emergency tau&service end */
    }
    else
    {
        NAS_LMM_EMMC_LOG_WARN("NAS_LMM_PreProcMmcLteSysInfoInd:cellstauts is err! ");
        enCellStatus = EMMC_EMM_CELL_STATUS_BUTT;
    }
    pstAreaInfo->ulCellStatus = enCellStatus;

	/* lihong00150010 emergency tau&service end */
    NAS_LMM_MEM_CPY(    pstNetInfo->aulLteBand,
                        pstLteInfoIndMsg->stLteBand.aulLteBand,
                        sizeof(LTE_BAND_STRU));

    /*update the bitOp for AreaInfo*/
    pstAreaInfo->bitOpCellId            = NAS_EMMC_BIT_SLCT;
    pstAreaInfo->bitOpPlmnId            = NAS_EMMC_BIT_SLCT;
    pstAreaInfo->bitOpTac               = NAS_EMMC_BIT_SLCT;
    pstNetInfo->bitOpLteBand            = NAS_EMMC_BIT_SLCT;

    /* 当前TA仍在拒绝原因12的TA内，记录标记，便于服务状态上报 */
    if(NAS_LMM_MATCH_SUCCESS==
                NAS_LMM_TaMatchTaList(&stTa,
                NAS_LMM_GetEmmInfoNetInfoForbTaForRposAddr()))
    {
        NAS_LMM_SetNasRejCause12Flag(NAS_LMM_REJ_CAUSE_IS_12);
    }

    *pulChangeInfo                      = ulChangeInfo;
	/* lihong00150010 emergency tau&service begin */
    *penForbdInfo                       = enForbdInfo;
    *penCellStatus                      = enCellStatus;
	/* lihong00150010 emergency tau&service end */
}


VOS_VOID NAS_EMMC_DeleteForbTa
(
    const NAS_MM_TA_STRU               *pstTa,
    NAS_MM_TA_LIST_STRU                *pstTaList
)
{
    NAS_LMM_DeleteTaFromTaList(pstTa, pstTaList, NAS_EMMC_MAX_FORBTA_NUM);

    return;
}



VOS_VOID  NAS_EMMC_UpdateForbTaList( VOS_VOID )
{
    NAS_MM_TA_LIST_STRU               *pstCurTaiList = VOS_NULL_PTR;
    NAS_MM_TA_LIST_STRU               *pstForbTaListForRoaming = VOS_NULL_PTR;
    NAS_MM_TA_LIST_STRU               *pstForbTaListForRpos = VOS_NULL_PTR;
    VOS_UINT32                          ulPriForbTaNum = 0;
    VOS_UINT32                          i = 0;

    /* 如果是紧急注册过程中或者紧急注册，则不需要将TAI LIST中的TA从被禁TA列表
       中删除 */
    if ((NAS_LMM_REG_STATUS_EMC_REGING  == NAS_LMM_GetEmmInfoRegStatus())
        || (NAS_LMM_REG_STATUS_EMC_REGED == NAS_LMM_GetEmmInfoRegStatus()))
    {
        NAS_LMM_EMMC_LOG_INFO("NAS_EMMC_UpdateForbTaList:EMC REGED OR EMC REGING!");
        return ;
    }

    /*获取当前的TAI List,ForTA for Roaming,ForbTA for RPOS*/
    pstCurTaiList = (VOS_VOID *)NAS_EMM_GetEmmGlobleAddr(NAS_LMM_GLOBLE_TA_LIST);
    pstForbTaListForRoaming = (VOS_VOID *)NAS_EMM_GetEmmGlobleAddr(NAS_LMM_GLOBLE_FORB_TA_FOR_ROAMING);
    pstForbTaListForRpos = (VOS_VOID *)NAS_EMM_GetEmmGlobleAddr(NAS_LMM_GLOBLE_FORB_TA_FOR_RPOS);

    /*更新 ForbTA for Roaming,如果变化通知RRC*/
    if(0 != pstForbTaListForRoaming->ulTaNum)
    {
        ulPriForbTaNum = pstForbTaListForRoaming->ulTaNum;
        for(i = 0; i < pstCurTaiList->ulTaNum; ++i)
        {
            if(NAS_LMM_MATCH_SUCCESS == NAS_LMM_TaMatchTaList(&(pstCurTaiList->astTa[i]),
                                                              pstForbTaListForRoaming))
            {
                NAS_EMMC_DeleteForbTa(&(pstCurTaiList->astTa[i]),pstForbTaListForRoaming);
            }
        }
        /*ForbTA个数发生变化，通知RRC*/
        if(ulPriForbTaNum != pstForbTaListForRoaming->ulTaNum)
        {
            NAS_EMMC_SendRrcCellSelectionReq(LRRC_LNAS_FORBTA_CHANGE);

        }
    }

    /*
    NAS_LMM_PUBM_LOG2_INFO("\nNAS_EMMC_UpdateForbTaList:ForbTA for Roaming NUM: old  new \n",
                           ulPriForbTaNum,pstForbTaListForRoaming->ulTaNum);
    */

    /*更新 ForbTA for RPOS,如果变化通知RRC*/
    if(0 != pstForbTaListForRpos->ulTaNum)
    {
        for(i = 0; i < pstCurTaiList->ulTaNum; ++i)
        {
            if(NAS_LMM_MATCH_SUCCESS == NAS_LMM_TaMatchTaList(&(pstCurTaiList->astTa[i]),
                                                              pstForbTaListForRpos))
            {
                NAS_EMMC_DeleteForbTa(&(pstCurTaiList->astTa[i]),pstForbTaListForRpos);
            }
        }
    }

    NAS_LMM_EMMC_LOG1_INFO("NAS_EMMC_UpdateForbTaList:ForbTA for RPOS NUM: New :",
                           pstForbTaListForRpos->ulTaNum);

    return;
}
VOS_VOID  NAS_EMMC_DeleteCurrentAreaCellId( VOS_VOID )
{
    NAS_MM_NETWORK_ID_STRU              *pstAreaInfo;

    pstAreaInfo = (VOS_VOID *)NAS_EMM_GetEmmGlobleAddr(NAS_LMM_GLOBLE_PRESENT_NET_ID);

    /* 小区的有效取值范围为0~504 ,将小区ID设置为无效 */
    pstAreaInfo->ulCellId               = NAS_EMMC_INVALID_CELLID;
}
VOS_VOID NAS_EMMC_ProcessRrcAreaLostInd(VOS_VOID)
{
    NAS_LMM_EMMC_LOG_NORM("NAS_EMMC_ProcessRrcAreaLostInd is entered.");

    /*上报覆盖区丢失关键事件*/
    NAS_LMM_SendOmtKeyEvent(MM_KEY_EVENT_COVERAGE_LOST);

    /* 删除当前小区 */
    NAS_EMMC_DeleteCurrentAreaCellId();

    return;
}


VOS_VOID NAS_EMMC_ProcessRrcNotCampOnInd(VOS_VOID)
{
    NAS_LMM_EMMC_LOG_NORM("NAS_EMMC_ProcessRrcNotCampOnInd is entered.");

    /* 设置Not Camp On标识 */
    NAS_EMMC_SetNotCampOnFlag(VOS_TRUE);

    /*上报覆盖区丢失关键事件*/
    /* NAS_LMM_SendOmtKeyEvent(MM_KEY_EVENT_COVERAGE_LOST); */

    /* 删除当前小区 */
    /* NAS_EMMC_DeleteCurrentAreaCellId(); */

    return;
}
VOS_UINT32 NAS_EMMC_IsBgListSearch(MMC_LMM_PLMN_SRCH_TYPE_ENUM_UINT32 ulSearchType)
{
    NAS_LMM_EMMC_LOG_NORM("NAS_EMMC_IsBgListSearch is entered.");

    /**
     * 只有当:
     *   - 下发搜索类型为LIST搜，且
     *   - 子状态不为NO_CELL，    且
     *   - 没有收到过Not Camp On消息
     * 时发起的搜索才是背景LIST搜
     */
    if ((MMC_LMM_PLMN_SRCH_LIST == ulSearchType) &&
        (EMM_SS_DEREG_NO_CELL_AVAILABLE != NAS_LMM_GetEmmCurFsmSS()) &&
        (EMM_SS_REG_NO_CELL_AVAILABLE   != NAS_LMM_GetEmmCurFsmSS()) &&
        (VOS_FALSE == NAS_EMMC_GetNotCampOnFlag()))
    {
        return VOS_TRUE;
    }
    else
    {
        return VOS_FALSE;
    }
}



VOS_VOID  NAS_EMMC_ProcessMmcLteSysInfoIndTaOrCellID
(
    const MMC_LMM_SYS_INFO_STRU          *pstLteInfoIndMsg,
    VOS_UINT32                           *pulChangeInfo
)
{
    VOS_UINT32                          ulChangeInfo = NAS_EMMC_NULL;
    NAS_MM_NETWORK_ID_STRU             *pstAreaInfo;
    NAS_LMM_NETWORK_INFO_STRU          *pstNetInfo;

    NAS_LMM_EMMC_LOG_INFO("NAS_EMMC_ProcessMmcLteSysInfoIndTaOrCellID is entered.");

    pstNetInfo = (VOS_VOID *)NAS_EMM_GetEmmGlobleAddr(NAS_LMM_GLOBLE_NET_INFO);

    pstNetInfo->bitOpPresentNetId = NAS_EMMC_BIT_SLCT;

    pstAreaInfo = (VOS_VOID *)NAS_EMM_GetEmmGlobleAddr(NAS_LMM_GLOBLE_PRESENT_NET_ID);


    /*比较系统消息与UE维护的当前网络信息,获取信息变更类型*/
    if(NAS_LMM_MATCH_FAIL ==
            NAS_LMM_PlmnMatch(&(pstAreaInfo->stPlmnId),
                (NAS_MM_PLMN_ID_STRU *)pstLteInfoIndMsg->stSpecPlmnIdList.astSuitPlmnList))
    {
        ulChangeInfo = ulChangeInfo | NAS_EMMC_BIT_SLCT;
        NAS_LMM_EMMC_LOG_INFO("NAS_EMMC_ProcessMmcLteSysInfoIndTaOrCellID: MMC_EMM_CHANGE_PLMN");
    }
    if(NAS_LMM_MATCH_FAIL ==
                NAS_LMM_TacMatch(&(pstAreaInfo->stTac),
                    (NAS_MM_TAC_STRU *)&(pstLteInfoIndMsg->stTac)))
    {
        ulChangeInfo = ulChangeInfo | (NAS_EMMC_BIT_SLCT << 1);
        NAS_LMM_EMMC_LOG_INFO("NAS_EMMC_ProcessMmcLteSysInfoIndTaOrCellID: MMC_EMM_CHANGE_TA");
    }
    if (pstAreaInfo->ulCellId != pstLteInfoIndMsg->ulCellId)
    {
        ulChangeInfo  = ulChangeInfo | (NAS_EMMC_BIT_SLCT << 2);
        NAS_LMM_EMMC_LOG_INFO("NAS_EMMC_ProcessMmcLteSysInfoIndTaOrCellID: MMC_EMM_CHANGE_CELLID");
    }

    *pulChangeInfo                      = ulChangeInfo;
}
VOS_UINT32  NAS_EMMC_GetUserSpecPlmnFlag(VOS_VOID)
{
    NAS_EMMC_GLOBAL_STRU               *pstEmmcInfo;
    NAS_EMMC_USER_SPEC_PLMN_FLAG_UINT32  ulUserFlag;

    pstEmmcInfo = NAS_EMMC_GetEmmcInfoAddr();

    ulUserFlag = pstEmmcInfo->enUserSpecPlmnFlag;

    return ulUserFlag;
}


VOS_VOID  NAS_EMMC_SetUserSpecPlmnFlag(NAS_EMMC_USER_SPEC_PLMN_FLAG_UINT32  ulUserFlag)
{
    NAS_EMMC_GLOBAL_STRU               *pstEmmcInfo;

    pstEmmcInfo = NAS_EMMC_GetEmmcInfoAddr();

    pstEmmcInfo->enUserSpecPlmnFlag = ulUserFlag;

    return;
}


VOS_VOID  NAS_EMMC_AddPlmnInRej18PlmnList(const NAS_MM_PLMN_ID_STRU *pstPlmnId)
{
    NAS_EMMC_GLOBAL_STRU                *pstEmmcInfo;

    pstEmmcInfo = NAS_EMMC_GetEmmcInfoAddr();

    NAS_LMM_AddPlmnInPlmnList(  pstPlmnId,
                                &pstEmmcInfo->stRej18PlmnList,
                                NAS_MM_MAX_PLMN_NUM);
    return;
}


VOS_UINT32 NAS_EMMC_PlmnMatchRej18PlmnList
(
    NAS_MM_PLMN_ID_STRU *pstPlmnId
)
{
    NAS_EMM_REJ_FLAG_ENUM_UINT32        ulRslt = NAS_EMM_REJ_NO;
    NAS_EMMC_GLOBAL_STRU                *pstEmmcInfo;

    (void)pstPlmnId;

    pstEmmcInfo = NAS_EMMC_GetEmmcInfoAddr();

    ulRslt = NAS_LMM_PlmnIDMatchPlmnList(pstPlmnId,&pstEmmcInfo->stRej18PlmnList);

    if (NAS_LMM_MATCH_SUCCESS == ulRslt)
    {
        ulRslt = NAS_EMM_REJ_YES;
    }

    return ulRslt;
}


VOS_VOID NAS_EMMC_SetRejCause18Flag
(
    NAS_EMM_REJ_FLAG_ENUM_UINT32 ulRej18Flag
)
{
    NAS_EMMC_GLOBAL_STRU                *pstEmmcInfo;

    pstEmmcInfo = NAS_EMMC_GetEmmcInfoAddr();

    pstEmmcInfo->ulRej18Flag = ulRej18Flag;

    return;
}


VOS_UINT32 NAS_EMMC_GetRejCause18Flag(VOS_VOID)
{
    NAS_EMMC_GLOBAL_STRU                *pstEmmcInfo;
    NAS_EMM_REJ_FLAG_ENUM_UINT32        ulRej18Flag;

    pstEmmcInfo = NAS_EMMC_GetEmmcInfoAddr();

    ulRej18Flag =  pstEmmcInfo->ulRej18Flag;

    NAS_LMM_EMMC_LOG1_INFO("NAS_EMMC_GetRejCause18Flag: ulRej18Flag = ",
                             ulRej18Flag);

    return ulRej18Flag;
}
/*lint +e961*/
/*lint +e960*/

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

