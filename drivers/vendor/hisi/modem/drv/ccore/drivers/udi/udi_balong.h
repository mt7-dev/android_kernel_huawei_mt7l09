
#ifndef    _BSP_UDI_DRV_H_
#define    _BSP_UDI_DRV_H_

#include "product_config.h"
#include "bsp_om.h"
#include <drv_udi.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#define UDI_GET_MAIN_DEV_ID(id) (((id) & 0xff00) >> 8 )
#define UDI_GET_DEV_TYPE(id) (((id) & 0x00ff))
#define UDI_UNUSED_PARAM(param) //(param=param)

#define BSP_ASSERT(expr) \
{\
    if (!(expr)) { \
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_UDI,\
			"Memory Malloc Error, line:%d\n", __LINE__, 0, 0, 0, 0, 0);\
        return BSP_ERROR;\
    } \
}


UDI_HANDLE BSP_UDI_ICC_ChnToHandle(BSP_U32 u32Chn);

typedef BSP_S32 (*UDI_ADP_INIT_CB_T) (VOID);
typedef BSP_U32 (*UDI_GET_CAPABILITY_CB_T)(UDI_DEVICE_ID devId);
typedef BSP_S32 (*UDI_OPEN_CB_T)(UDI_OPEN_PARAM *param, UDI_HANDLE handle);
typedef BSP_S32 (*UDI_CLOSE_CB_T)(VOID* pPrivate);
typedef BSP_S32 (*UDI_WRITE_CB_T)(VOID* pPrivate, void* pMemObj, BSP_U32 u32Size);
typedef BSP_S32 (*UDI_READ_CB_T)(VOID* pPrivate, void* pMemObj, BSP_U32 u32Size);
typedef BSP_S32 (*UDI_IOCTL_CB_T) (VOID* pPrivate, BSP_U32 u32Cmd, VOID* pParam);


/* 驱动适配函数指针表 */
typedef struct tagUDI_DRV_INTEFACE_TABLE
{
    /* capability */
    UDI_ADP_INIT_CB_T       udi_get_capability_cb;

    /* data interfaces */
    UDI_OPEN_CB_T           udi_open_cb;
    UDI_CLOSE_CB_T          udi_close_cb;
    UDI_WRITE_CB_T          udi_write_cb;
    UDI_READ_CB_T           udi_read_cb;
    UDI_IOCTL_CB_T          udi_ioctl_cb;
}UDI_DRV_INTEFACE_TABLE;

BSP_S32 BSP_UDI_Init(VOID);
BSP_S32 BSP_UDI_SetPrivate(UDI_DEVICE_ID devId, VOID* pPrivate);
BSP_S32 BSP_UDI_SetCapability(UDI_DEVICE_ID devId, BSP_U32 u32Capability);
BSP_S32 BSP_UDI_SetInterfaceTable(UDI_DEVICE_ID devId, UDI_DRV_INTEFACE_TABLE *pDrvInterface);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _BSP_UDI_DRV_H_ */


