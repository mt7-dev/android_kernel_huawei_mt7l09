
#include "vos.h"
#include "Taf_Tafm_Remote.h"

#include "MnCommApi.h"

#include "TafClientApi.h"

#include "MnClient.h"
#include "TafAppMma.h"


#ifdef  __cplusplus
  #if  __cplusplus
  extern "C"{
  #endif
#endif


/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
#define    THIS_FILE_ID PS_FILE_ID_TAF_TAFM_APPREMOTE_C

/****************************************************/
/*    远端API使用的全局变量全部在定义时进行初始化   */
/****************************************************/
/*记录APP回调函数信息结果*/


/*记录TAF远端是否注册过MUX回调函数*/
VOS_UINT8 g_ucTafRetRegMuxCallbackFlag = TAF_REG_MUX_CALLBACK_NO;
VOS_UINT32  Taf_GetAppMsgLen(VOS_UINT32 ulParaLen, VOS_UINT32 *pulMsgLen)
{
    VOS_UINT32  ulMsgLen;

    if(VOS_NULL_PTR == pulMsgLen)
    {
        return VOS_ERR;
    }

    if ( ulParaLen <= 4 )
    {
        ulMsgLen = sizeof(MN_APP_REQ_MSG_STRU);
    }
    else
    {
        ulMsgLen = (sizeof(MN_APP_REQ_MSG_STRU) + ulParaLen) -4;
    }

    *pulMsgLen = ulMsgLen;

    return VOS_OK;

}

/**********************************************************/
/*                  通信参数设置查询API                   */
/**********************************************************/

VOS_UINT32  Taf_ParaQuery  (
    VOS_UINT16                     ClientId,
    VOS_UINT8                      OpId,
    TAF_PARA_TYPE                  ParaType,
    VOS_VOID                      *pPara
)
{
    VOS_UINT8                           *pucPara;
    VOS_UINT16                           usMsgName;
    VOS_UINT32                           ulParaLen;
    VOS_UINT32                           ulRst;
    VOS_UINT32                           ulReceiverPid;

    if ((ParaType >= TAF_PH_ACCESS_MODE_PARA)
        && (ParaType < TAF_TELE_PARA_BUTT))
    {
        ulParaLen = sizeof(TAF_PARA_TYPE);
        ulReceiverPid = I0_WUEPS_PID_MMA;
        usMsgName = TAF_MSG_PARA_READ;
    }
    else
    {
        return TAF_FAILURE;
    }

    pucPara = (VOS_UINT8 *)PS_MEM_ALLOC( WUEPS_PID_AT, ulParaLen );

    if (VOS_NULL_PTR == pucPara)
    {
        return TAF_FAILURE;
    }

    PS_MEM_SET( pucPara, 0 , ulParaLen );

    PS_MEM_CPY( pucPara, &ParaType, sizeof(TAF_PARA_TYPE) );

    ulRst = MN_FillAndSndAppReqMsg( ClientId,
                                  OpId,
                                  usMsgName,
                                  pucPara,
                                  ulParaLen,
                                  ulReceiverPid);
    PS_MEM_FREE( WUEPS_PID_AT, pucPara );

    return ulRst;
}


TAF_UINT32 Taf_PrefPlmnHandle (
    MN_CLIENT_ID_T                      ClientId,
    MN_OPERATION_ID_T                   OpId,
    TAF_PH_PREF_PLMN_OPERATE_STRU  *pstPrefPlmnOperate
)

{
    /* 检查入口参数合法性 */
    if ( (pstPrefPlmnOperate->enPrefPLMNType > MN_PH_PREF_PLMN_HPLMN)
      || (pstPrefPlmnOperate->enPrefPlmnOperType > MN_PH_PrefPLMN_MODIFY_ONE))
    {
        return TAF_FAILURE;
    }

    return MN_FillAndSndAppReqMsg( ClientId,
                                   OpId,
                                   TAF_MSG_MMA_HANDLE_PREF_PLMN_INFO,
                                   pstPrefPlmnOperate,
                                   sizeof(TAF_PH_PREF_PLMN_OPERATE_STRU),
                                   I0_WUEPS_PID_MMA);


}

#ifdef  __cplusplus
  #if  __cplusplus
  }
  #endif
#endif

