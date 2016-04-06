
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <linux/slab.h>
#include <linux/err.h>
#include <bsp_acm.h>
#include <bsp_ncm.h>
#include <drv_icc.h>
#include "udi_balong.h"



static BSP_S32 udiAdpNcmInit(VOID);
static BSP_S32 udiAdpIccInit(VOID);
static BSP_S32 udiAdpAcmInit(VOID);
static BSP_S32 udiAdpIccInit(VOID);



/* 各模块特性值定义 */
#define UDI_USB_ACM_CAPA		(UDI_CAPA_BLOCK_READ | UDI_CAPA_BLOCK_WRITE | UDI_CAPA_READ_CB | UDI_CAPA_WRITE_CB)
#define UDI_USB_NCM_NDIS_CAPA	(UDI_CAPA_READ_CB | UDI_CAPA_BUFFER_LIST)
#define UDI_USB_NCM_CTRL_CAPA	(UDI_CAPA_READ_CB | UDI_CAPA_CTRL_OPT)
#define UDI_ICC_CAPA				(UDI_CAPA_BLOCK_READ | UDI_CAPA_BLOCK_WRITE | UDI_CAPA_READ_CB | UDI_CAPA_WRITE_CB)
#define UDI_UART_CAPA			(UDI_CAPA_BLOCK_READ | UDI_CAPA_BLOCK_WRITE)


/* 各模块初始化函数定义 */
VOID* g_udiInitFuncTable[(BSP_U32)UDI_DEV_MAX+1] =
{
	/* ACM Init */
	udiAdpAcmInit,

	/* NCM Init */
	udiAdpNcmInit,

	/* ICC Init */
	udiAdpIccInit,

	/* Must Be END */
	NULL
};

/*** eric **/

static BSP_S32 udiAcmAdpOpen(UDI_OPEN_PARAM *param, UDI_HANDLE handle)
{
	//BSP_U8* pstrName;
	BSP_U32 u32Type;
	BSP_PVOID filep;

	UDI_UNUSED_PARAM(handle);

	u32Type = UDI_GET_DEV_TYPE(param->devid);
	filep = bsp_acm_open(u32Type);

	if (filep == NULL)
	{
		return ERROR;
	}

	(BSP_VOID)BSP_UDI_SetPrivate(param->devid, filep);
	return OK;
}

static BSP_S32 udiAdpAcmInit(VOID)
{
	UDI_DRV_INTEFACE_TABLE* pDrvInterface = NULL;
	BSP_U32 u32Cnt;

	/* 构造回调函数指针列表 */
	pDrvInterface = kmalloc(sizeof(UDI_DRV_INTEFACE_TABLE), GFP_KERNEL);
	if (NULL == pDrvInterface)
	{
		printk(KERN_ERR "BSP_MODU_UDI NO Mem, line:%d\n", __LINE__);
		return ERROR;
	}
	memset((VOID*)pDrvInterface, 0, sizeof(UDI_DRV_INTEFACE_TABLE));

	/* 只设置需要实现的几个接口 */
	pDrvInterface->udi_open_cb = udiAcmAdpOpen;
	pDrvInterface->udi_close_cb = (UDI_CLOSE_CB_T)bsp_acm_close;
	pDrvInterface->udi_write_cb = (UDI_WRITE_CB_T)bsp_acm_write;
	pDrvInterface->udi_read_cb = (UDI_READ_CB_T)bsp_acm_read;
	pDrvInterface->udi_ioctl_cb = (UDI_IOCTL_CB_T)bsp_acm_ioctl;

	/*  ACM 都使用同一套驱动函数指针 */
	for (u32Cnt = UDI_USB_ACM_CTRL; u32Cnt < UDI_USB_ACM_MAX; u32Cnt++)
	{
		(BSP_VOID)BSP_UDI_SetCapability((UDI_DEVICE_ID)UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, u32Cnt), UDI_USB_ACM_CAPA);
		(BSP_VOID)BSP_UDI_SetInterfaceTable((UDI_DEVICE_ID)UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, u32Cnt), pDrvInterface);
	}

	return OK;
}

/**************************************************************************
  NCM 适配实现
**************************************************************************/
#define BSP_NCM_MAX_DEV_NUM  4 /* 最大NCM设备数量，用作数组定义*/
static BSP_U32 sg_chnNcmDataHandleTbl[BSP_NCM_MAX_DEV_NUM] = {0};
#define UDI2NCM_ID(udi_id) ((udi_id) / 2)

UDI_HANDLE BSP_UDI_NCM_DataChnToHandle(BSP_U32 u32Chn)
{
    return sg_chnNcmDataHandleTbl[u32Chn];
}

static BSP_S32 udiNcmAdpOpen(UDI_OPEN_PARAM *param, UDI_HANDLE handle)
{
	BSP_U32 u32NcmDevId = 0;
	BSP_U32 u32ChanId;
	void* pCtrl;

	UDI_UNUSED_PARAM(handle);

	u32ChanId = UDI_GET_DEV_TYPE(param->devid);
	u32NcmDevId = UDI2NCM_ID(u32ChanId);

	pCtrl = bsp_ncm_open(NCM_DEV_DATA_TYPE, u32NcmDevId);
	if (!pCtrl) {
		BSP_UDI_SetPrivate(param->devid, pCtrl);
		sg_chnNcmDataHandleTbl[u32NcmDevId] = handle;    /*lint !e732*/
		return 0;
	}

	return -EINVAL;
}

static BSP_S32 udiNcmAdpCtrlOpen(UDI_OPEN_PARAM *param, UDI_HANDLE handle)
{
	BSP_U32 u32NcmDevId;
	/*modify bsp_ncm_open ,add by wangweichao*/
	BSP_U32 u32ChanId;
	void* pCtrl;

	UDI_UNUSED_PARAM(handle);

	u32ChanId = UDI_GET_DEV_TYPE(param->devid);
	u32NcmDevId = UDI2NCM_ID(u32ChanId);

	pCtrl = bsp_ncm_open(NCM_DEV_CTRL_TYPE, u32NcmDevId);
	if (!pCtrl) {
		(BSP_VOID)BSP_UDI_SetPrivate(param->devid, pCtrl);
		return 0;
	}

	return -EINVAL;
}

static BSP_S32 udiNcmAdpClose(VOID* pPrivate)
{
	return (BSP_S32)bsp_ncm_close(NCM_DEV_DATA_TYPE, pPrivate);
}

static BSP_S32 udiNcmAdpCtrlClose(VOID* pPrivate)
{
	return (BSP_S32)bsp_ncm_close(NCM_DEV_CTRL_TYPE, pPrivate);
}

static BSP_S32 udiNcmAdpWrite(VOID* pPrivate, VOID* pMemObj, BSP_S32 s32Size)
{
	UDI_UNUSED_PARAM(s32Size);
	return (BSP_S32)bsp_ncm_write(pPrivate, pMemObj, NULL);
}

static BSP_S32 udiAdpNcmInit(VOID)
{
	BSP_U32 i;
	UDI_DRV_INTEFACE_TABLE* pDrvDataInterface = NULL;
	UDI_DRV_INTEFACE_TABLE* pDrvCtrlInterface = NULL;

	/* 构造回调函数指针列表(数据通道) */
	pDrvDataInterface = kmalloc(sizeof(UDI_DRV_INTEFACE_TABLE), GFP_KERNEL);
	if (NULL == pDrvDataInterface)
	{
		printk(KERN_ERR "BSP_MODU_UDI NO Mem, line:%d\n", __LINE__);
		goto ADP_NCM_INIT_ERR;
	}
	memset((VOID*)pDrvDataInterface, 0, sizeof(UDI_DRV_INTEFACE_TABLE));

	/* 只设置需要实现的几个接口(数据通道) */
	pDrvDataInterface->udi_open_cb = (UDI_OPEN_CB_T)udiNcmAdpOpen;
	pDrvDataInterface->udi_close_cb = (UDI_CLOSE_CB_T)udiNcmAdpClose;
	pDrvDataInterface->udi_write_cb = (UDI_WRITE_CB_T)udiNcmAdpWrite;
	pDrvDataInterface->udi_ioctl_cb = (UDI_IOCTL_CB_T)bsp_ncm_ioctl;

	/* 设置函数指针(数据通道) */
	for(i=0; i<UDI_USB_NCM_BOTTOM/2; i++)
	{
		(BSP_VOID)BSP_UDI_SetCapability(UDI_NCM_NDIS_ID, UDI_USB_NCM_NDIS_CAPA);
		(BSP_VOID)BSP_UDI_SetInterfaceTable(UDI_NCM_NDIS_ID, pDrvDataInterface);
	}

	/* 构造回调函数指针列表(控制通道) */
	pDrvCtrlInterface = kmalloc(sizeof(UDI_DRV_INTEFACE_TABLE), GFP_KERNEL);
	if (NULL == pDrvCtrlInterface)
	{
		printk(KERN_ERR "BSP_MODU_UDI NO Mem, line:%d\n", __LINE__);
		goto ADP_NCM_INIT_ERR;
	}
	memset((VOID*)pDrvCtrlInterface, 0, sizeof(UDI_DRV_INTEFACE_TABLE));

	/* 构造回调函数指针列表(控制通道) */
	pDrvCtrlInterface->udi_open_cb = (UDI_OPEN_CB_T)udiNcmAdpCtrlOpen;
	pDrvCtrlInterface->udi_close_cb = (UDI_CLOSE_CB_T)udiNcmAdpCtrlClose;
	pDrvCtrlInterface->udi_write_cb = (UDI_WRITE_CB_T)udiNcmAdpWrite;
	pDrvCtrlInterface->udi_ioctl_cb = (UDI_IOCTL_CB_T)bsp_ncm_ioctl;

	/* 设置函数指针(数据通道) */
	for(i=0; i<UDI_USB_NCM_BOTTOM/2; i++)
	{
		(BSP_VOID)BSP_UDI_SetCapability(UDI_NCM_CTRL_ID, UDI_USB_NCM_CTRL_CAPA);
		(BSP_VOID)BSP_UDI_SetInterfaceTable(UDI_NCM_CTRL_ID, pDrvCtrlInterface);
	}

	return OK;

ADP_NCM_INIT_ERR:
	if (NULL != pDrvDataInterface)
	{
		/* coverity[dead_error_condition] */
		kfree(pDrvDataInterface);
	}
	/* coverity[dead_error_condition] */
	if (NULL != pDrvCtrlInterface)
	{
		/* coverity[dead_error_line] */
		kfree(pDrvCtrlInterface);
	}

	return ERROR;
}


/**************************************************************************
  ICC 适配实现
**************************************************************************/
static BSP_U32 sg_chnHandleTbl[ICC_CHAN_NUM_MAX] = {0};

UDI_HANDLE BSP_UDI_ICC_ChnToHandle(BSP_U32 u32Chn)
{
	return sg_chnHandleTbl[u32Chn];
}

static BSP_S32 udiIccAdpOpen(UDI_OPEN_PARAM *param, UDI_HANDLE handle)
{
	unsigned long u32ChanId;
	ICC_CHAN_ATTR_S *pstOpenParam;

	u32ChanId = UDI_GET_DEV_TYPE(param->devid);
	(BSP_VOID)BSP_UDI_SetPrivate(param->devid, (void*)u32ChanId);

	/* 从param 中解析出各个参数 */
	pstOpenParam = (ICC_CHAN_ATTR_S *)param->pPrivate;
	sg_chnHandleTbl[u32ChanId] = handle;     /*lint !e732*/

	return BSP_ICC_Open(u32ChanId, pstOpenParam);
}

static BSP_S32 udiAdpIccInit(VOID)
{
	UDI_DRV_INTEFACE_TABLE* pDrvInterface = NULL;
	BSP_U32 u32Cnt;

	/* 构造回调函数指针列表 */
	pDrvInterface = kmalloc(sizeof(UDI_DRV_INTEFACE_TABLE), GFP_KERNEL);
	if (NULL == pDrvInterface)
	{
		printk(KERN_ERR "BSP_MODU_UDI NO Mem, line:%d\n", __LINE__);
		return ERROR;
	}
	memset((VOID*)pDrvInterface, 0, sizeof(UDI_DRV_INTEFACE_TABLE));

	/* 只设置需要实现的几个接口 */
	pDrvInterface->udi_open_cb = (UDI_OPEN_CB_T)udiIccAdpOpen;
	pDrvInterface->udi_close_cb = (UDI_CLOSE_CB_T)BSP_ICC_Close;
	pDrvInterface->udi_write_cb = (UDI_WRITE_CB_T)BSP_ICC_Write;
	pDrvInterface->udi_read_cb = (UDI_READ_CB_T)BSP_ICC_Read;
	pDrvInterface->udi_ioctl_cb = (UDI_IOCTL_CB_T)BSP_ICC_Ioctl;

	/* 几个 ICC 都使用同一套驱动函数指针 */
	for (u32Cnt = 0; u32Cnt < ICC_CHAN_NUM_MAX; u32Cnt++)
	{
		(BSP_VOID)BSP_UDI_SetCapability((UDI_DEVICE_ID)UDI_BUILD_DEV_ID(UDI_DEV_ICC, u32Cnt), UDI_ICC_CAPA);
		(BSP_VOID)BSP_UDI_SetInterfaceTable((UDI_DEVICE_ID)UDI_BUILD_DEV_ID(UDI_DEV_ICC, u32Cnt), pDrvInterface);
	}

	return OK;
}


#ifdef __cplusplus
}
#endif /* __cplusplus */




