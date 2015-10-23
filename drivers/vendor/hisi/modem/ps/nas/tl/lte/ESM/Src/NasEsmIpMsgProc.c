



/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
#include    "NasEsmIpMsgProc.h"
#include    "NasEsmPublic.h"
#include    "NasEsmNwMsgDecode.h"

/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_NASESMIPMSGPROC_C
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
/*****************************************************************************
 Function Name  : NAS_ESM_SndEsmIpPdpReleaseInd
 Description    : 向IP模块发送ID_ESM_IP_PDP_RELEASE_IND消息
 Input          : ulEpsbId-------------------承载号
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.lihong00150010      2010-12-01  Draft Enact

*****************************************************************************/
/*lint -e960*/
/*lint -e961*/
VOS_VOID NAS_ESM_SndEsmIpPdpReleaseInd
(
    VOS_UINT8                           ucEpsbId
)
{
    ESM_IP_PDP_RELEASE_IND_STRU        *pstPdpReleaseInd    = VOS_NULL_PTR;

    /*分配空间并检验分配是否成功*/
    pstPdpReleaseInd = (VOS_VOID*)NAS_ESM_ALLOC_MSG(sizeof(ESM_IP_PDP_RELEASE_IND_STRU));

    if ( VOS_NULL_PTR == pstPdpReleaseInd )
    {
        /*打印异常信息*/
        NAS_ESM_ERR_LOG("NAS_ESM_SndEsmIpConfigParaReq:ERROR:Alloc Msg fail!");
        return ;
    }

    /*清空*/
    NAS_ESM_MEM_SET(    NAS_ESM_GET_MSG_ENTITY(pstPdpReleaseInd),
                        NAS_ESM_NULL,
                        NAS_ESM_GET_MSG_LENGTH(pstPdpReleaseInd));

    /* 填写消息内容 */
    pstPdpReleaseInd->ucEpsbId      = ucEpsbId;

    /*填写消息头*/
    NAS_ESM_WRITE_IP_MSG_HEAD(pstPdpReleaseInd, ID_ESM_IP_PDP_RELEASE_IND);

    /* 调用消息发送函数 */
    NAS_ESM_SND_MSG(pstPdpReleaseInd);
}


/*****************************************************************************
 Function Name  : NAS_ESM_SndEsmIpRelInd
 Description    : 向IP模块发送ID_ESM_IP_REL_IND消息
 Input          : VOS_VOID
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.lihong00150010      2010-12-01  Draft Enact

*****************************************************************************/
VOS_VOID NAS_ESM_SndEsmIpRelInd( VOS_VOID )
{
    ESM_IP_REL_IND_STRU                *pstRelInd           = VOS_NULL_PTR;

    /*分配空间并检验分配是否成功*/
    pstRelInd = (VOS_VOID*)NAS_ESM_ALLOC_MSG(sizeof(ESM_IP_REL_IND_STRU));

    if ( VOS_NULL_PTR == pstRelInd )
    {
        /*打印异常信息*/
        NAS_ESM_ERR_LOG("NAS_ESM_SndEsmIpRelInd:ERROR:Alloc Msg fail!");
        return ;
    }

    /*清空*/
    NAS_ESM_MEM_SET(    NAS_ESM_GET_MSG_ENTITY(pstRelInd),
                        NAS_ESM_NULL,
                        NAS_ESM_GET_MSG_LENGTH(pstRelInd));

    /*填写消息头*/
    NAS_ESM_WRITE_IP_MSG_HEAD(pstRelInd, ID_ESM_IP_REL_IND);

    /* 调用消息发送函数 */
    NAS_ESM_SND_MSG(pstRelInd);
}


/*****************************************************************************
 Function Name  : NAS_ESM_SndEsmIpNwParaIndMsg
 Description    : 向IP模块发送ID_ESM_IP_NW_PARA_IND消息
 Input          : ulEpsbId-------------------承载号
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
      1.lihong00150010      2010-12-11  Draft Enact

*****************************************************************************/
VOS_VOID NAS_ESM_SndEsmIpNwParaIndMsg
(
    VOS_UINT8                           ucEpsbId
)
{
    ESM_IP_NW_PARA_IND_STRU            *pstNwParaInd        = VOS_NULL_PTR;
    NAS_ESM_EPSB_CNTXT_INFO_STRU       *pstEpsbCntxtInfo    = VOS_NULL_PTR;


    pstEpsbCntxtInfo = NAS_ESM_GetEpsbCntxtInfoAddr(ucEpsbId);

    /* 判断IP地址是否有效 */
    if (NAS_ESM_IS_IPV4_ADDR_INVALID(pstEpsbCntxtInfo->stPdnAddrInfo.aucIpV4Addr))
    {
        /*打印异常信息*/
        NAS_ESM_ERR_LOG("NAS_ESM_SndEsmIpNwParaIndMsg:Ipv4 is invaild!");
        return ;
    }

    /*分配空间并检验分配是否成功*/
    pstNwParaInd = (VOS_VOID*)NAS_ESM_ALLOC_MSG(sizeof(ESM_IP_NW_PARA_IND_STRU));

    if ( VOS_NULL_PTR == pstNwParaInd )
    {
        /*打印异常信息*/
        NAS_ESM_ERR_LOG("NAS_ESM_SndEsmIpNwParaIndMsg:ERROR:Alloc Msg fail!");
        return ;
    }

    /*清空*/
    NAS_ESM_MEM_SET(    NAS_ESM_GET_MSG_ENTITY(pstNwParaInd),
                        NAS_ESM_NULL,
                        NAS_ESM_GET_MSG_LENGTH(pstNwParaInd));

    /* 填写消息内容 */
    pstNwParaInd->ucEpsbId              = ucEpsbId;

    pstNwParaInd->bitOpIpAddr           = NAS_ESM_OP_TRUE;
    pstNwParaInd->stIpAddr.ucIpType     = NAS_ESM_PDN_IPV4;
    NAS_ESM_MEM_CPY(    pstNwParaInd->stIpAddr.aucIpV4Addr,
                        pstEpsbCntxtInfo->stPdnAddrInfo.aucIpV4Addr,
                        NAS_ESM_IP_ADDR_LEN_IPV4);

    pstNwParaInd->bitOpSubnetMask       = NAS_ESM_OP_TRUE;
    pstNwParaInd->stSubnetMask.ucIpType = NAS_ESM_PDN_IPV4;
    NAS_ESM_MEM_CPY(    pstNwParaInd->stSubnetMask.aucIpV4Addr,
                        pstEpsbCntxtInfo->stSubnetMask.aucIpV4Addr,
                        NAS_ESM_IP_ADDR_LEN_IPV4);

    pstNwParaInd->bitOpGateWay          = NAS_ESM_OP_TRUE;
    pstNwParaInd->stGateWay.ucIpType    = NAS_ESM_PDN_IPV4;
    NAS_ESM_MEM_CPY(    pstNwParaInd->stGateWay.aucIpV4Addr,
                        pstEpsbCntxtInfo->stGateWayAddrInfo.aucIpV4Addr,
                        NAS_ESM_IP_ADDR_LEN_IPV4);

    if (NAS_ESM_OP_TRUE == pstEpsbCntxtInfo->bitOpPco)
    {
        pstNwParaInd->stDnsServer.ucDnsSerNum = pstEpsbCntxtInfo->stPcoInfo.ucIpv4DnsSerNum;
        NAS_ESM_MEM_CPY(    pstNwParaInd->stDnsServer.astDnsServer,
                            pstEpsbCntxtInfo->stPcoInfo.astIpv4DnsServer,
                            (NAS_ESM_MAX_DNS_SERVER_NUM * sizeof(NAS_ESM_IPV4_ADDR_STRU)));
    }

    /*填写消息头*/
    NAS_ESM_WRITE_IP_MSG_HEAD(pstNwParaInd, ID_ESM_IP_NW_PARA_IND);

    /* 调用消息发送函数 */
    NAS_ESM_SND_MSG(pstNwParaInd);
}
/*lint +e961*/
/*lint +e960*/

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif


/* end of NasEsmIpMsgProc.c */
