

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "mlib_typedef.h"
#include "ucom_comm.h"
#include "ucom_mem_dyn.h"
#include "ucom_nv.h"
#include "mlib_utility.h"
#include "mlib_interface.h"
#include "voice_debug.h"
#include "med_drv_timer_hifi.h"
#include "om_comm.h"
#include "ucom_stub.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 全局变量定义
*****************************************************************************/


/*****************************************************************************
  3 函数实现
*****************************************************************************/


MLIB_VOID MUTIL_MemCpy(
                MLIB_VOID              *pvDes,
                MLIB_VOID              *pvSrc,
                MLIB_UINT32             uwCount)
{
    UCOM_MemCpy(pvDes, pvSrc, uwCount);
}


MLIB_VOID MUTIL_MemSet(
                MLIB_VOID               *pvDes,
                MLIB_UINT8               ucData,
                MLIB_UINT32              uwCount)
{
    UCOM_MemSet(pvDes, ucData, uwCount);
}


MLIB_VOID*  MUTIL_Malloc( MLIB_UINT32  uwSize )
{
    return UCOM_MemAlloc(uwSize);
}


MLIB_VOID  MUTIL_Free( MLIB_VOID *pMem )
{
    UCOM_MemFree(pMem);
}


MLIB_UINT32 MUTIL_ReadNV(
                MLIB_UINT16             uhwId,
                MLIB_VOID               *pItem,
                MLIB_UINT32              uwLength)
{
    return UCOM_NV_Read(uhwId, pItem, uwLength);
}


MLIB_VOID MUTIL_LogReport(
                MLIB_LOG_LEVEL_ENUM_UINT32           enLevel,
                MLIB_UINT16                          uhwFileID,
                MLIB_UINT16                          uhwLineID,
                MLIB_INT32                           swValue1,
                MLIB_INT32                           swValue2,
                MLIB_INT32                           swValue3)
{
    OM_LOG_LogReport(
        (OM_LOG_RPT_LEVEL_ENUM_UINT16)enLevel,
        uhwFileID,
        uhwLineID,
        OM_LOG_ID_MLIB_EXT,
        1,
        swValue1,
        swValue2,
        swValue3);
}



MLIB_VOID  MLIB_LogApr(MLIB_APR_LOG_ID_ENUM_UINT32 enLogId)
{
    OM_LOG_AprLogReport(OM_APR_MODULE_EXT_PP, enLogId);
}



MLIB_VOID MUTIL_DataHook(
                MLIB_UINT16              uhwPos,
                MLIB_VOID               *pvData,
                MLIB_UINT16              uhwLen)
{
    MLIB_UINT16                    uhwHookLen  = 0;
    MSG_VOICE_OM_HOOK_STRU         stHookHead;
    UCOM_DATA_BLK_STRU             astBlk[2];
    VOICE_HOOK_STRU               *pstHookObj = VOICE_DbgGetHookPtr();

    uhwHookLen = (uhwLen > VOICE_MAX_HOOK_LEN) ? VOICE_MAX_HOOK_LEN : uhwLen;

    /* 填充原语内容 */
    stHookHead.usMsgId      = (VOS_UINT16)ID_VOICE_OM_HOOK_IND;
    stHookHead.usHookTarget = uhwPos;
    stHookHead.ulFrameTick  = pstHookObj->ulFrameTick;
    stHookHead.ulTimeStamp  = DRV_TIMER_ReadSysTimeStamp();
    stHookHead.usHookLength = (VOS_UINT16)(uhwHookLen * 2);

    /* 填充地址和长度 */
    astBlk[0].pucData       = (VOS_UCHAR *)&stHookHead;
    astBlk[0].uwSize        = sizeof(stHookHead);

    /* 填充地址和长度 */
    astBlk[1].pucData       = (VOS_UCHAR *)pvData;
    astBlk[1].uwSize        = uhwHookLen * 2;


    /* 上报HOOK信息 */
    OM_COMM_SendTrans(astBlk, 2);
}


MLIB_VOID  MLIB_UtilitySetSource(
                MLIB_SOURCE_STRU       *pstSource,
                MLIB_UINT32             uwChnID,
                MLIB_VOID              *pvData,
                MLIB_UINT32             uwSize,
                MLIB_UINT32             uwSampleRate)
{
    pstSource->astChn[uwChnID].pucData       = (MLIB_UINT8*)pvData;
    pstSource->astChn[uwChnID].uwSize        = uwSize;
    pstSource->astChn[uwChnID].uwSampleRate  = uwSampleRate;
    pstSource->astChn[uwChnID].uwResolution  = 16;
}

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

