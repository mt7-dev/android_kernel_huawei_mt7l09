/*******************************************************************************
  Copyright    : 2005-2007, Huawei Tech. Co., Ltd.
  File name    : SsRcvTaf.c
  Description  : 接收TAF消息的处理
  Function List:
      1.  Ss_CheckTafId                  判断TAF ID是否是ACMP指定的TAF ID
      2.  Ss_RcvMnssBeginReq             SS收到MNSS_BEGIN_REQ的处理
      3.  Ss_RcvMnssFacilityReq          SS收到MNSS_FACILITY_REQ的处理
      4.  Ss_RcvMnssEndReq               SS收到MNSS_END_REQ的处理
      5.  Ss_RcvTafMsg                   分发TAF消息
  History:
      1.  张志勇 2004.03.08   新版作成
      2.  丁  庆 2006.11.07   问题单A32D07063
      3.  丁  庆 2006.11.23   问题单A32D07452
*******************************************************************************/

#include "SsInclude.h"


#ifdef  __cplusplus
  #if  __cplusplus
  extern "C"{
  #endif
#endif


/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
#define    THIS_FILE_ID        PS_FILE_ID_SS_RCVTAF_C


/***********************************************************************
*  MODULE   : Ss_RcvMnssBeginReq
*  FUNCTION : SS收到MNSS_BEGIN_REQ的处理
*  INPUT    : ST_SSP_MSG        *pSspMsg    当前处理的消息
*             VOS_UINT8             ucTafId     消息中的TAF ID
*  OUTPUT   : VOS_VOID
*  RETURN   : VOS_VOID
*  NOTE     : 无
*  HISTORY  :
*     1.  张志勇 04-03-08  新版作成
************************************************************************/

VOS_VOID Ss_RcvMnssBeginReq( ST_SSP_MSG *pSspMsg )
{
    VOS_UINT8   ucTi;                                                               /* 定义TI                                   */
    VOS_UINT8   ucCr;                                                               /* 定义CR                                   */
  
   ucCr = pSspMsg->ucCr;                                                       /* 获取CR                                   */
    if( ucCr <= SS_MO_CR_NUM )
    {
        if( SS_NONE_TI != ( ucTi = Ss_EntityBuildTi( ucCr )))
        {                                                                       /* 选择空闲TI                               */
            Ss_EntityCreat( ucTi, ucCr );                              /* 建立SS实体                               */
            Ss_SaveRegister( pSspMsg, ucTi );                                   /* 缓存REGISTER消息                         */
            Ss_SndMmssEstReq( ucTi );                                           /* 发送MMSS_EST_REQ原语                     */
            g_SsEntity[ucTi].ucState = SS_S_CONNECT_PEND;                       /* 状态迁移                                 */
            
            SS_NORM_LOG("\n SS state: SS_S_NULL ==> SS_S_CONNECT_PEND \r" );
        }
    }
}

/***********************************************************************
*  MODULE   : Ss_RcvMnssFacilityReq
*  FUNCTION : SS收到MNSS_FACILITY_REQ的处理
*  INPUT    : ST_SSP_MSG        *pSspMsg    当前处理的消息
*             VOS_UINT8             ucTafId     消息中的TAF ID
*  OUTPUT   : VOS_VOID
*  RETURN   : VOS_VOID
*  NOTE     : 无
*  HISTORY  :
*     1.  张志勇 04-03-08  新版作成
************************************************************************/

VOS_VOID Ss_RcvMnssFacilityReq( ST_SSP_MSG *pSspMsg )
{
    VOS_UINT8   ucTi;                                                               /* 定义TI                                   */

    if( SS_NONE_TI != ( ucTi = Ss_EntitySelectTi( pSspMsg->ucCr )))
    {                                                                           /* 根据CR和TAF ID找TI                       */
        Ss_SndFacility( pSspMsg, ucTi );                                        /* 发送DR(FACILITY)原语                     */
    }
}
VOS_VOID Ss_RcvMnssEndReq( ST_SSP_MSG *pSspMsg )
{
    VOS_UINT8   i;
    VOS_UINT8   ucTi = SS_NONE_TI;                                                            /* 定义TI                                   */

    for( i = 0 ; i < SS_MAX_ENTITY_NUM ; i++ )
    {
        if( pSspMsg->ucCr == g_SsEntity[i].ucCr )
        {
            ucTi = i;
            break;
        }
    }

    if( SS_NONE_TI != ucTi)
    {
        if (SS_S_CONNECTED == g_SsEntity[ucTi].ucState)
        {
            Ss_SndRelCmpl( pSspMsg, ucTi, 0 );                                  /* 发送DR(RELCMPL)原语                      */
        }
        
        if ( ( VOS_TRUE == pSspMsg->SspmsgCore.u.EndReq.OP_SspTaCause )
          && ( EN_STC_NORMAL == pSspMsg->SspmsgCore.u.EndReq.enSspTaCause )
          && (SS_S_CONNECT_PEND == g_SsEntity[ucTi].ucState))  
        {
            Ss_SndMmssAbortReq(ucTi);
        }
        else
        {
            Ss_SndMmssRelReq( ucTi );                                               /* 释放对应的MM连接                         */
        }
        Ss_EntityClear( ucTi );                                                 /* 释放对应的Ss实体                         */
    }
}

/***********************************************************************
*  MODULE   : Ss_RcvTafMsg
*  FUNCTION : 分发TAF消息
*  INPUT    : VOS_VOID      *pMsg   当前处理的消息
*  OUTPUT   : VOS_VOID
*  RETURN   : VOS_VOID
*  NOTE     : 无
*  HISTORY  :
*     1.  张志勇 04-03-08  新版作成
************************************************************************/

VOS_VOID Ss_RcvSsaMsg(
                  VOS_VOID *pMsg
                  )
{
    ST_SSP_MSG                      *pstSsMsg;                                   /* CCP消息结构                              */

    pstSsMsg = (ST_SSP_MSG*)pMsg;                               /* 获取ASN解码后SSP消息                     */

    switch( pstSsMsg->SspmsgCore.ucChoice )
        {                                                                       /* 根据消息名分发                           */
        case MNSS_BEGIN_REQ:                                                    /* MNSS_BEGIN_REQ                           */
            Ss_RcvMnssBeginReq( pstSsMsg );                             /* 调用 MNSS_BEGIN_REQ 消息处理             */
            break;
        case MNSS_FACILITY_REQ:                                                 /* MNSS_FACILITY_REQ                        */
            Ss_RcvMnssFacilityReq( pstSsMsg );                          /* 调用 MNSS_FACILITY_REQ 消息处理          */
            break;
        case MNSS_END_REQ:                                                      /* MNSS_END_REQ                             */
            Ss_RcvMnssEndReq( pstSsMsg );                               /* 调用 MNSS_END_REQ 消息处理               */
            break;
        default:                                                                /* 其他TAF消息                              */
            SS_WARN_LOG( "\n [Error]APP->SS MsgType Error!\r" );
            break;
        }

}

#ifdef  __cplusplus
  #if  __cplusplus
  }
  #endif
#endif

