
#include <osm.h>
#include <gen_msg.h>
/*lint -save -e537*/
#include <DrvInterface.h>
/*lint -restore*/
#include <ftm.h>
#include <ftm_ct.h>
/*lint -save -e537*/
#include <msp_errno.h>
/*lint -restore*/

/***********************************************************************************
  Prototype       : TDS_PsAppSendMsg
 Description     : PS调用APP的接口，用于从DSP接收TDS维修原语(AT实现, PS调用)
 Input           : ulLen 消息长度 (长度数据不大于48字节)
 Output          : None.
 Return Value    : 成功:0, 失败:其他值
 History         :
************************************************************************************/

VOS_UINT32 TDS_PsAtSendMsg(VOS_UINT32 ulLen, TDS_AT_PS_MSG_INFO_STRU *pstTdsAppPsMsg)
{
    if((0 == ulLen) || (NULL == pstTdsAppPsMsg))
    {
        return ERR_MSP_INVALID_PARAMETER;
    }
/*  return l1a_CtCnf(&pstTdsAppPsMsg->ulMsgId, 12); */
    return ERR_MSP_UNKNOWN;
}

