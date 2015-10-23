

/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
#include    "NasEmmSecuSoftUsim.h"
#include    "NasLmmPubMPrint.h"
#include    "NasEmmSecuOm.h"
#include    "NasLmmPubMNvim.h"

#if(VOS_WIN32 == VOS_OS_VER)
#include    "msp.h"
#endif

/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_NASEMMSECUSOFTUSIM_C
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
#if 0
VOS_UINT32                              g_ulSoftUsimSqn  = NAS_EMM_SOFT_USIM_NOSUPPORT_SQN;
NAS_EMM_SOFT_USIM_AUTH_PARAM_STRU       g_stSoftUsimAuthParam;
NAS_EMM_USIM_CONTROL_STRU               g_stUsimInfo;

#endif
/*****************************************************************************
  3 Function
*****************************************************************************/

#if(VOS_WIN32 == VOS_OS_VER)
VOS_UINT32 USIMM_SetFileReq(VOS_UINT32 ulSenderPid, VOS_UINT32 ulSendPara,USIMM_SET_FILE_INFO_STRU *pstSetFileInfo)
{
    /*testFileReqEfId = usEfId;
    ptestFileSenducEf = pucEf;
    testFileSenducEfLen = ucEfLen;*/
    return VOS_OK;
}
VOS_UINT32 USIMM_GetFileReq(VOS_UINT32 ulSenderPid, VOS_UINT32 ulSendPara, USIMM_GET_FILE_INFO_STRU *pstGetFileInfo)
{
    /*testFileReqEfId = usEfId;*/
    return VOS_OK;
}
VOS_UINT32 USIMM_AuthReq(VOS_UINT32 ulSenderPid, USIMM_APP_TYPE_ENUM_UINT32 ulAppType, USIMM_AUTH_DATA_STRU *pstAuth)
{
    return VOS_OK;
}
#endif
VOS_VOID PS_GetLeastSignBitsByTrunc(const VOS_UINT8 aucSrcData[MAX_HMAC_HASH_SIZE],
                                              VOS_UINT8 aucDestData[MAX_HMAC_HASH_SIZE/2])
{
    VOS_UINT32                          ulLoop              = 0;
    VOS_UINT32                          ulTmpLoop           = 0;

    for ( ulLoop = 0 ; ulLoop< (MAX_HMAC_HASH_SIZE/2); ulLoop++ )
    {
        /*修改:应该取低位，而不是高位*/
        ulTmpLoop = ulLoop + (MAX_HMAC_HASH_SIZE/2);
        aucDestData[ulLoop] = aucSrcData[ulTmpLoop];
    }

    return;
}


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

