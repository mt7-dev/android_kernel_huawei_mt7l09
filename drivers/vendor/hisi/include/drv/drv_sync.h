

#ifndef __DRV_SYNC_H__
#define __DRV_SYNC_H__
#include "drv_comm.h"


/*************************SYNC START**********************************/

/**************************************************************************
  错误码定义
**************************************************************************/
#define BSP_ERR_SYNC_BASE                (BSP_S32)(0x80000000 | (BSP_DEF_ERR(BSP_MODU_SYNC, 0)))
#define BSP_ERR_SYNC_TIMEOUT             (BSP_ERR_SYNC_BASE + 0x0)

/**************************************************************************
  结构定义
**************************************************************************/
typedef enum tagSYNC_MODULE_E
{
    SYNC_MODULE_MEM     = 0,
    SYNC_MODULE_NANDC,
    SYNC_MODULE_PMU,
    SYNC_MODULE_SOCP,
    SYNC_MODULE_DMA,
    SYNC_MODULE_IPF,
    SYNC_MODULE_NV,
    SYNC_MODULE_YAFFS,
    SYNC_MODULE_MSP,
    SYNC_MODULE_GPIO,
    SYNC_MODULE_CSHELL,
    SYNC_MODULE_MNTN,
    SYNC_MODULE_MSPNV,
    SYNC_MODULE_ONLINE,
    SYNC_MODULE_CHG,
    SYNC_MODULE_TEST01,
    SYNC_MODULE_TEST02,
    SYNC_MODULE_BUTT
}SYNC_MODULE_E;

/**************************************************************************
  函数声明
**************************************************************************/
BSP_S32 BSP_SYNC_Lock(SYNC_MODULE_E u32Module, BSP_U32 *pState, BSP_U32 u32TimeOut);
BSP_S32 BSP_SYNC_UnLock(SYNC_MODULE_E u32Module, BSP_U32 u32State);
BSP_S32 BSP_SYNC_Wait(SYNC_MODULE_E u32Module, BSP_U32 u32TimeOut);
BSP_S32 BSP_SYNC_Give(SYNC_MODULE_E u32Module);

/*************************SYNC END************************************/

#endif

