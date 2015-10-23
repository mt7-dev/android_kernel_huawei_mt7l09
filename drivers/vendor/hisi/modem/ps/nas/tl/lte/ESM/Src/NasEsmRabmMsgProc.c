


/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
#include    "NasEsmInclude.h"

/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_NASESMRABMMSGPROC_C
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



/*****************************************************************************
  3 Function
*****************************************************************************/

VOS_VOID NAS_ESM_RabmMsgDistr( VOS_VOID *pRcvMsg )
{
    PS_MSG_HEADER_STRU         *pRabmMsg = VOS_NULL_PTR;

    /*打印进入该函数*/
    NAS_ESM_INFO_LOG("NAS_ESM_RabmMsgDistr is entered.");

    pRabmMsg = (PS_MSG_HEADER_STRU*)pRcvMsg;

    /*根据消息名，调用相应的消息处理函数*/
    switch(pRabmMsg->ulMsgName)
    {
        /*如果收到的是ID_ESM_ERABM_REL_REQ消息*/
        case ID_ESM_ERABM_REL_REQ:

            /* 如果ESM处于非注册态，则直接返回 */
            if (NAS_ESM_PS_REGISTER_STATUS_DETACHED == NAS_ESM_GetEmmStatus())
            {
                NAS_ESM_WARN_LOG("NAS_ESM_RabmMsgDistr:WARNING:Msg is discard cause ESM state is detached!");
                return ;
            }


            /*清资源*/
            NAS_ESM_ClearEsmResource();
            break;

        /*RB未建立，本地去激活相应承载上下文*/
        case ID_ESM_ERABM_BEARER_STATUS_REQ:

            /*本地去激活承载上下文*/
            NAS_ESM_RcvEsmRabmBearerStatusReq((ESM_ERABM_BEARER_STATUS_REQ_STRU *) pRcvMsg);
            break;

        default:
            /*打印异常信息*/
            NAS_ESM_WARN_LOG("NAS_ESM_RabmMsgDistr:WARNING:RABM->SM Message name non-existent!");
            break;
    }
}


/*lint -e960*/
/*lint -e961*/
VOS_VOID  NAS_ESM_GetPdnAddr
(
    NAS_ESM_CONTEXT_IP_ADDR_STRU *pstPdnAddr,
    VOS_UINT32 ulEpsbId
)
{
    NAS_ESM_EPSB_CNTXT_INFO_STRU       *pstEpsbCntxtInfo   = VOS_NULL_PTR;

    if(VOS_NULL_PTR == pstPdnAddr)
    {
        NAS_ESM_INFO_LOG("NAS_ESM_GetPdnAddr:Error:Input parameter is null!");
        return;
    }

    /*获取对应EpsbCntxtTbl表地址*/
    pstEpsbCntxtInfo = NAS_ESM_GetEpsbCntxtInfoAddr(ulEpsbId);

    /* 若是专有承载，则获取关联缺省承载的EpsbCntxtTbl表地址 */
    if(NAS_ESM_BEARER_TYPE_DEDICATED == NAS_ESM_GetBearCntxtType(ulEpsbId))
    {
        pstEpsbCntxtInfo = NAS_ESM_GetEpsbCntxtInfoAddr(pstEpsbCntxtInfo->ulLinkedEpsbId);
    }

    /* 获取PDN信息 */
    if(NAS_ESM_OP_TRUE == pstEpsbCntxtInfo->bitOpPdnAddr)
    {
        NAS_ESM_MEM_CPY(    pstPdnAddr,
                            &pstEpsbCntxtInfo->stPdnAddrInfo,
                            sizeof(NAS_ESM_CONTEXT_IP_ADDR_STRU));
    }
    else
    {
        NAS_ESM_INFO_LOG("NAS_ESM_GetPdnAddr:Error:No Pdn Address!");
    }
}
/*lint -specific(-e433)*/
VOS_VOID NAS_ESM_SndEsmRabmActIndMsg( VOS_UINT32  ulEpsbId )
{
    VOS_UINT32                          ulTftPfNum         = NAS_ESM_NULL;
    VOS_UINT32                          ulCnt              = NAS_ESM_NULL;
    VOS_UINT32                          ulTmpCid           = NAS_ESM_NULL;
    ESM_ERABM_ACT_IND_STRU              *pEsmRabmActIndMsg  = VOS_NULL_PTR;
    NAS_ESM_SDF_CNTXT_INFO_STRU        *pstSdfCntxtInfo    = VOS_NULL_PTR;
    NAS_ESM_EPSB_CNTXT_INFO_STRU       *pstEpsbCntxtInfo   = VOS_NULL_PTR;

    /*分配空间并检验分配是否成功*/
    /*lint -e433 -e826*/
    pEsmRabmActIndMsg = (VOS_VOID*)NAS_ESM_ALLOC_MSG(sizeof(ESM_ERABM_ACT_IND_STRU));
    /*lint +e433 +e826*/

    if ( VOS_NULL_PTR == pEsmRabmActIndMsg )
    {
        /*打印异常信息*/
        NAS_ESM_ERR_LOG("NAS_ESM_SndEsmRabmActIndMsg:ERROR:Alloc Msg fail!");
        return ;
    }

    /*清空*/
    NAS_ESM_MEM_SET(NAS_ESM_GET_MSG_ENTITY(pEsmRabmActIndMsg), 0, NAS_ESM_GET_MSG_LENGTH(pEsmRabmActIndMsg));

    /*获取对应EpsbCntxtTbl表地址*/
    pstEpsbCntxtInfo = NAS_ESM_GetEpsbCntxtInfoAddr(ulEpsbId);
    ulTmpCid = pstEpsbCntxtInfo->ulBitCId;

    /* 拷贝承载的所有PF信息 */
    for(ulCnt = NAS_ESM_MIN_CID; ulCnt <= NAS_ESM_MAX_CID; ulCnt++)
    {
        if(NAS_ESM_OP_TRUE ==((ulTmpCid >> ulCnt )& NAS_ESM_BIT_0))
        {
            pstSdfCntxtInfo = NAS_ESM_GetSdfCntxtInfo(ulCnt);

            /*拷贝SDF的PF信息*/
            NAS_ESM_CopyRabmTftInfo(pEsmRabmActIndMsg->astTftPf + ulTftPfNum,\
                               pstSdfCntxtInfo->astSdfPfInfo,\
                               pstSdfCntxtInfo->ulSdfPfNum);

            ulTftPfNum += pstSdfCntxtInfo->ulSdfPfNum;
        }
    }

    /*填写消息内容*/
    pEsmRabmActIndMsg->ulEpsId = ulEpsbId;
    pEsmRabmActIndMsg->ulTftPfNum = ulTftPfNum;

    /* 填写PDN信息 */
    NAS_ESM_GetPdnAddr(&pEsmRabmActIndMsg->stPdnAddr, ulEpsbId);

    pEsmRabmActIndMsg->enBearerCntxtType = pstEpsbCntxtInfo->enBearerCntxtType;
    pEsmRabmActIndMsg->ulLinkedEpsbId = pstEpsbCntxtInfo->ulLinkedEpsbId;
    pEsmRabmActIndMsg->ucQCI = pstEpsbCntxtInfo->stEpsQoSInfo.ucNwQCI;
    /*填写消息头*/
    NAS_ESM_WRITE_RABM_MSG_HEAD(pEsmRabmActIndMsg,ID_ESM_ERABM_ACT_IND);

    /* 调用消息发送函数 */
    NAS_ESM_SND_MSG(pEsmRabmActIndMsg);

}
/*lint -specific(+e433)*/



/*lint -specific(-e433)*/
VOS_VOID NAS_ESM_SndEsmRabmMdfIndMsg( VOS_UINT32       ulEpsbId )
{
    VOS_UINT32                          ulTftPfNum         = NAS_ESM_NULL;
    VOS_UINT32                          ulCnt              = NAS_ESM_NULL;
    VOS_UINT32                          ulTmpCid           = NAS_ESM_NULL;
    ESM_ERABM_MDF_IND_STRU              *pEsmRabmMdfIndMsg  = VOS_NULL_PTR;
    NAS_ESM_SDF_CNTXT_INFO_STRU        *pstSdfCntxtInfo    = VOS_NULL_PTR;
    NAS_ESM_EPSB_CNTXT_INFO_STRU       *pstEpsbCntxtInfo   = VOS_NULL_PTR;


    /*分配空间并检验分配是否成功*/
    /*lint -e433 -e826*/
    pEsmRabmMdfIndMsg = (VOS_VOID*)NAS_ESM_ALLOC_MSG(sizeof(ESM_ERABM_MDF_IND_STRU));
    /*lint +e433 +e826*/

    if ( VOS_NULL_PTR == pEsmRabmMdfIndMsg )
    {
        /*打印异常信息*/
        NAS_ESM_ERR_LOG("NAS_ESM_SndEsmRabmMdfIndMsg:ERROR:Alloc Msg fail!");
        return ;
    }

    /*清空*/
    NAS_ESM_MEM_SET(NAS_ESM_GET_MSG_ENTITY(pEsmRabmMdfIndMsg), 0, NAS_ESM_GET_MSG_LENGTH(pEsmRabmMdfIndMsg));

    /*获取对应EpsbCntxtTbl表地址*/
    pstEpsbCntxtInfo = NAS_ESM_GetEpsbCntxtInfoAddr(ulEpsbId);
    ulTmpCid = pstEpsbCntxtInfo->ulBitCId;

    /* 拷贝承载的所有PF信息 */
    for(ulCnt = NAS_ESM_MIN_CID; ulCnt <= NAS_ESM_MAX_CID; ulCnt++)
    {
        if(NAS_ESM_OP_TRUE ==((ulTmpCid >> ulCnt )& NAS_ESM_BIT_0))
        {
            pstSdfCntxtInfo = NAS_ESM_GetSdfCntxtInfo(ulCnt);

            /*拷贝SDF的PF信息*/
            NAS_ESM_CopyRabmTftInfo(pEsmRabmMdfIndMsg->astTftPf + ulTftPfNum,\
                               pstSdfCntxtInfo->astSdfPfInfo,\
                               pstSdfCntxtInfo->ulSdfPfNum);

            ulTftPfNum += pstSdfCntxtInfo->ulSdfPfNum;
        }
    }

    /*填写消息内容*/
    pEsmRabmMdfIndMsg->ulEpsId = ulEpsbId;
    pEsmRabmMdfIndMsg->ulTftPfNum = ulTftPfNum;

    /* 填写PDN信息 */
    NAS_ESM_GetPdnAddr(&pEsmRabmMdfIndMsg->stPdnAddr, ulEpsbId);

    pEsmRabmMdfIndMsg->enBearerCntxtType = pstEpsbCntxtInfo->enBearerCntxtType;
    pEsmRabmMdfIndMsg->ulLinkedEpsbId = pstEpsbCntxtInfo->ulLinkedEpsbId;
    pEsmRabmMdfIndMsg->ucQCI= pstEpsbCntxtInfo->stEpsQoSInfo.ucNwQCI;

    /*填写消息头*/
    NAS_ESM_WRITE_RABM_MSG_HEAD(pEsmRabmMdfIndMsg,ID_ESM_ERABM_MDF_IND);


    /* 调用消息发送函数 */
    NAS_ESM_SND_MSG(pEsmRabmMdfIndMsg);

}
/*lint -specific(+e433)*/



VOS_VOID NAS_ESM_SndEsmRabmDeactIndMsg(VOS_UINT32          ulEpsbIdNum,
                                                  const VOS_UINT32         *pulEpsbId)
{
    ESM_ERABM_DEACT_IND_STRU       *pEsmRabmDeactIndMsg   = VOS_NULL_PTR;

    /*检查入口参数*/
    if( (ulEpsbIdNum == 0) || (ulEpsbIdNum > NAS_ESM_MAX_EPSB_NUM) \
            || (VOS_NULL_PTR == pulEpsbId) )
    {
        /*打印异常信息*/
        NAS_ESM_WARN_LOG("NAS_ESM_SndEsmRabmDeactIndMsg:WARNING:Input Para error!");
        return ;
    }

    /*分配空间并检验分配是否成功*/
    pEsmRabmDeactIndMsg = (VOS_VOID*)NAS_ESM_ALLOC_MSG(sizeof(ESM_ERABM_DEACT_IND_STRU));

    if ( VOS_NULL_PTR == pEsmRabmDeactIndMsg )
    {
        /*打印异常信息*/
        NAS_ESM_ERR_LOG("NAS_ESM_SndEsmRabmDeactIndMsg:ERROR:Alloc Msg fail!");
        return ;
    }

    /*清空*/
    NAS_ESM_MEM_SET(NAS_ESM_GET_MSG_ENTITY(pEsmRabmDeactIndMsg), 0, NAS_ESM_GET_MSG_LENGTH(pEsmRabmDeactIndMsg));

    /*将ulEpsbIdNum、pulEpsbId填入pEsmRabmDeactIndMsg*/
    pEsmRabmDeactIndMsg->ulEpsIdNum = ulEpsbIdNum;
    NAS_ESM_MEM_CPY((VOS_UINT8*)pEsmRabmDeactIndMsg->aulEpsId, (VOS_UINT8*)pulEpsbId,\
              (pEsmRabmDeactIndMsg->ulEpsIdNum)*(sizeof(VOS_UINT32)/sizeof(VOS_UINT8)));

    /*填写消息头*/
    NAS_ESM_WRITE_RABM_MSG_HEAD(pEsmRabmDeactIndMsg,ID_ESM_ERABM_DEACT_IND);

    /* 调用消息发送函数 */
    NAS_ESM_SND_MSG(pEsmRabmDeactIndMsg);
}


VOS_VOID NAS_ESM_SndEsmRabmRelIndMsg( VOS_VOID )
{
    ESM_ERABM_REL_IND_STRU       *pEsmRabmRelIndMsg   = VOS_NULL_PTR;

    /*分配空间并检验分配是否成功*/
    pEsmRabmRelIndMsg = (VOS_VOID*)NAS_ESM_ALLOC_MSG(sizeof(ESM_ERABM_REL_IND_STRU));

    if ( VOS_NULL_PTR == pEsmRabmRelIndMsg )
    {
        /*打印异常信息*/
        NAS_ESM_ERR_LOG("NAS_ESM_SndEsmRabmRelIndMsg:ERROR:Alloc Msg fail!");
        return ;
    }

    /*清空*/
    NAS_ESM_MEM_SET(NAS_ESM_GET_MSG_ENTITY(pEsmRabmRelIndMsg), 0, NAS_ESM_GET_MSG_LENGTH(pEsmRabmRelIndMsg));

    /*填写消息头*/
    NAS_ESM_WRITE_RABM_MSG_HEAD(pEsmRabmRelIndMsg,ID_ESM_ERABM_REL_IND);

    /* 调用消息发送函数 */
    NAS_ESM_SND_MSG(pEsmRabmRelIndMsg);

}
VOS_VOID NAS_ESM_SndEsmRabmDeactAllMsg( VOS_VOID )
{
    VOS_UINT32                  ulEpsbId                    = NAS_ESM_NULL;
    VOS_UINT32                  ulActiveFlg                 = NAS_ESM_FAILURE;
    VOS_UINT32                  ulEpsbIdNum                 = NAS_ESM_NULL;
    VOS_UINT32                  aulEpsbId[NAS_ESM_MAX_EPSB_NUM]  = {NAS_ESM_NULL};

    /*打印进入该函数*/
    NAS_ESM_INFO_LOG("NAS_ESM_SndEsmRabmDeactAllMsg is entered.");

    /*找出已激活承载，并记录其ID*/
    for( ulEpsbId = NAS_ESM_MIN_EPSB_ID; ulEpsbId <= NAS_ESM_MAX_EPSB_ID; ulEpsbId++ )
    {
        /*此承载已激活*/
        if( NAS_ESM_BEARER_STATE_ACTIVE == NAS_ESM_GetBearCntxtState(ulEpsbId) )
        {
            /*保存已激活承载ID,准备发送给RABM*/
            aulEpsbId[ulEpsbIdNum++] = ulEpsbId;

            /*设置标识位*/
            ulActiveFlg = NAS_ESM_SUCCESS;
        }
    }

    /*如果所有的承载均未激活*/
    if( NAS_ESM_FAILURE == ulActiveFlg )
    {
        /*打印信息*/
        NAS_ESM_NORM_LOG("NAS_ESM_SndEsmRabmDeactAllMsg:NORMAL:no bearer active!");
        return ;
    }

    NAS_ESM_SndEsmRabmDeactIndMsg(ulEpsbIdNum, aulEpsbId);

}
VOS_VOID NAS_ESM_CopyRabmTftInfo( ESM_ERABM_TFT_PF_STRU* pstRabmTftInfo,
                                         const NAS_ESM_CONTEXT_TFT_STRU* pstCntxtTftInfo,
                                            VOS_UINT32 ulSdfPfNum)
{
    VOS_UINT32                          ulPfNum = NAS_ESM_NULL;

    ulPfNum = ulSdfPfNum;

    /*清空*/
    NAS_ESM_MEM_SET((VOS_VOID*)pstRabmTftInfo, 0, ulPfNum * sizeof(ESM_ERABM_TFT_PF_STRU));

    /* 拷贝*/
    NAS_ESM_MEM_CPY(pstRabmTftInfo, pstCntxtTftInfo, ulPfNum * sizeof(ESM_ERABM_TFT_PF_STRU));
}



VOS_VOID  NAS_ESM_RcvEsmRabmBearerStatusReq(const ESM_ERABM_BEARER_STATUS_REQ_STRU *pRcvMsg )
{
    VOS_UINT32                  ulCnt                           = NAS_ESM_NULL;
    VOS_UINT32                  ulQueryRslt                     = NAS_ESM_FAILURE;
    VOS_UINT32                  ulEpsbId                        = NAS_ESM_NULL;
    VOS_UINT32                  ulEpsbIdNum                     = NAS_ESM_NULL;
    VOS_UINT32                  aulEpsbId[NAS_ESM_MAX_EPSB_NUM] = {NAS_ESM_NULL};
    VOS_UINT32                  ulStateTblIndex                 =  NAS_ESM_NULL;
    NAS_ESM_EPSB_CNTXT_INFO_STRU *pstEpsbCntxtInfo              = VOS_NULL_PTR;

    /*打印进入该函数*/
    NAS_ESM_INFO_LOG("NAS_ESM_RcvEsmRabmBearerStatusReq is entered.");

    /* 如果ESM处于非注册态，则直接返回 */
    if (NAS_ESM_PS_REGISTER_STATUS_DETACHED == NAS_ESM_GetEmmStatus())
    {
        NAS_ESM_WARN_LOG("NAS_ESM_RcvEsmRabmBearerStatusReq:WARNING:Msg is discard cause ESM state is detached!");
        return ;
    }

    /*取出消息中的EPSID信息*/
    ulEpsbIdNum = pRcvMsg->ulEpsIdNum;
    NAS_ESM_MEM_CPY(aulEpsbId, pRcvMsg->aulEpsId, ulEpsbIdNum*(sizeof(VOS_UINT32)/sizeof(VOS_UINT8)));

    /*轮询每一个承载信息*/
    for( ulEpsbId = NAS_ESM_MIN_EPSB_ID; ulEpsbId <= NAS_ESM_MAX_EPSB_ID; ulEpsbId++ )
    {
        /*取出承载信息*/
        pstEpsbCntxtInfo = NAS_ESM_GetEpsbCntxtInfoAddr(ulEpsbId);

        /*如果此承载未激活*/
        if( NAS_ESM_BEARER_STATE_INACTIVE == pstEpsbCntxtInfo->enBearerCntxtState )
        {
            continue;
        }

        ulQueryRslt = NAS_ESM_FAILURE;

        /*查询ID_ESM_ERABM_BEARER_STATUS_IND消息的EPSID信息中是否包含此承载ID*/
        for( ulCnt = 0; ulCnt < ulEpsbIdNum; ulCnt++ )
        {
            if( ulEpsbId == aulEpsbId[ulCnt] )
            {
                ulQueryRslt = NAS_ESM_SUCCESS;
                break;
            }
        }

        /*如果不包含*/
        if( NAS_ESM_FAILURE == ulQueryRslt )
        {
            /*通知APP释放承载，并释放相关承载和资源*/
            NAS_ESM_DeactBearerAndInformApp(ulEpsbId);
            /*清除和中止与ulEpsbId关联的流程信息*/
            if(NAS_ESM_SUCCESS == NAS_ESM_QueryStateTblIndexByEpsbId(ulEpsbId, &ulStateTblIndex))
            {
                NAS_ESM_RelStateTblResource(ulStateTblIndex);
            }
        }
    }

    /*通知EMM当前承载状态信息，发送ID_EMM_ESM_BEARER_STATUS_REQ*/
    NAS_ESM_SndEsmEmmBearerStatusReqMsg(EMM_ESM_BEARER_CNTXT_MOD_MUTUAL);

}
/*lint +e961*/
/*lint +e960*/
#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
/* end of NasEsmRabmMsgProc.c */
