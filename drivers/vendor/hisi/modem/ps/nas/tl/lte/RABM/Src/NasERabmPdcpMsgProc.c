


/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
#include  "NasERabmPdcpMsgProc.h"
#include  "NasERabmAppMsgProc.h"
#include  "NasERabmEsmMsgProc.h"

/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_NASRABMPDCPMSGPROC_C
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
VOS_VOID APP_DATA_IND_FUNC( VOS_UINT32  ulRabId, VOS_UINT32  ulDataLen, const VOS_UINT8 *srcMem)
{
    (VOS_VOID)ulRabId;
    (VOS_VOID)ulDataLen;
    (VOS_VOID)srcMem;
}

VOS_VOID APP_ERABM_DATA_TRANS_FUNC( const VOS_UINT8 *pucData)
{
    (VOS_VOID)pucData;
}








#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

