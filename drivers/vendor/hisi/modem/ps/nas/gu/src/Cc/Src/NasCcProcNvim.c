
/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "NasCcCtx.h"
#include "NasCcProcNvim.h"
#include "NVIM_Interface.h"
#include "NasCcCommon.h"

#include "NasNvInterface.h"
#include "TafNvInterface.h"

#include "NasUsimmApi.h"
#include "NasCcTimer.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define    THIS_FILE_ID        PS_FILE_ID_NAS_CC_PROC_NIVM_C

/*****************************************************************************
  2 全局变量
*****************************************************************************/

/*****************************************************************************
  3 宏定义
*****************************************************************************/

/*****************************************************************************
   4 函数实现
*****************************************************************************/


VOS_VOID  NAS_CC_ReadCcTimerLenNvim( VOS_VOID )
{
    NAS_CC_NVIM_TIMER_LEN_STRU          stNasCcTimerLen;
    VOS_UINT32                          ulLength;
    VOS_UINT32                          ulTimerLength;

    ulLength = 0;

    ulTimerLength = 0;
    
    PS_MEM_SET(&stNasCcTimerLen, 0, sizeof(NAS_CC_NVIM_TIMER_LEN_STRU));
    
    NV_GetLength(en_NV_Item_CC_TimerLen, &ulLength);	
    if (ulLength > sizeof(NAS_CC_NVIM_TIMER_LEN_STRU))
    {
        return;
    }
    
    if (VOS_OK != NV_Read(en_NV_Item_CC_TimerLen,
                         &stNasCcTimerLen, ulLength))
    {
        NAS_CC_WARN_LOG("NAS_CC_ReadCcTimerLenNvim():WARNING: en_NV_Item_CC_TimerLen Error");

        return;
    }
    
    /* 如果NV中设置的值为0或者大于等于30s，则按照协议默认值30s处理，否则视为有效值，需要进行设置 */
    if ((stNasCcTimerLen.ucT305Len > 0)
      &&(stNasCcTimerLen.ucT305Len < 30))
    {
        /* 更新T305的时长 */
        ulTimerLength = NAS_CC_ONE_THOUSAND_MILLISECOND * ((VOS_UINT32)stNasCcTimerLen.ucT305Len);
        NAS_CC_SetNvTimerLen(TI_NAS_CC_T305, ulTimerLength);
    }
    else
    {
        /* 更新T305的时长 */
        ulTimerLength = 30000;
        NAS_CC_SetNvTimerLen(TI_NAS_CC_T305, ulTimerLength);
    }

    if ((stNasCcTimerLen.ucT308Len > 0)
      &&(stNasCcTimerLen.ucT308Len < 30))
    {
        /* 更新T308的时长 */
        ulTimerLength = NAS_CC_ONE_THOUSAND_MILLISECOND * ((VOS_UINT32)stNasCcTimerLen.ucT308Len);
        NAS_CC_SetNvTimerLen(TI_NAS_CC_T308, ulTimerLength);
    }
    else
    {
        /* 更新T308的时长 */
        ulTimerLength = 30000;
        NAS_CC_SetNvTimerLen(TI_NAS_CC_T308, ulTimerLength);
    }

    return;
}
VOS_VOID  NAS_CC_ReadCcbsNvim( VOS_VOID )
{
    NAS_CC_NVIM_CCBS_SUPPORT_FLG_STRU   stCcbsSupportFlg;
    NAS_CC_CUSTOM_CFG_INFO_STRU        *pstCcbsSupportFlg = VOS_NULL_PTR;
    VOS_UINT32                          ulLength;

    ulLength = 0;
    

    stCcbsSupportFlg.ucCcbsSupportFlg = NAS_CC_NV_ITEM_DEACTIVE;

    pstCcbsSupportFlg = NAS_CC_GetCustomCfgInfo();
    
    NV_GetLength(en_NV_Item_NVIM_CCBS_SUPPORT_FLG, &ulLength);	
    if (ulLength > sizeof(NAS_NVIM_CCBS_SUPPORT_FLG_STRU))
    {
        return;
    }
    
    if (VOS_OK != NV_Read(en_NV_Item_NVIM_CCBS_SUPPORT_FLG,
                         &stCcbsSupportFlg, ulLength))
    {
        NAS_CC_WARN_LOG("NAS_CC_ReadCcbsNvim():WARNING: en_NV_Item_NVIM_CCBS_SUPPORT_FLG Error");

        return;
    }


    if ((NAS_CC_NV_ITEM_ACTIVE != stCcbsSupportFlg.ucCcbsSupportFlg)
     && (NAS_CC_NV_ITEM_DEACTIVE != stCcbsSupportFlg.ucCcbsSupportFlg))
    {
        pstCcbsSupportFlg->ucCcbsSupportFlg = NAS_CC_NV_ITEM_DEACTIVE;
        NAS_CC_WARN_LOG("NAS_CC_ReadCcbsNvim():WARNING: NV parameter Error");
        return;
    }

    pstCcbsSupportFlg->ucCcbsSupportFlg = stCcbsSupportFlg.ucCcbsSupportFlg;

    return;
}



VOS_VOID NAS_CC_ReadNvimInfo(VOS_VOID)
{
    /* en_NV_Item_NVIM_CCBS_SUPPORT_FLG */
    NAS_CC_ReadCcbsNvim();

    /* en_NV_Item_CC_TimerLen */
    NAS_CC_ReadCcTimerLenNvim();

    return;
}



#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif



