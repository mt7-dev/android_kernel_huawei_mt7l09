
#ifndef    _BSP_UDI_H_
#define    _BSP_UDI_H_


#include  <osl_types.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**************************************************************************
  宏定义
**************************************************************************/
#define UDI_INVALID_HANDLE     (-1)

#define UDI_CAPA_BLOCK_READ	((BSP_U32)(1<<0)) /* 该设备的read接口为阻塞接口 */
#define UDI_CAPA_BLOCK_WRITE	((BSP_U32)(1<<1)) /* 该设备的write接口为阻塞接口 */
#define UDI_CAPA_READ_CB		((BSP_U32)(1<<2)) /* 该设备支持read 回调函数 */
#define UDI_CAPA_WRITE_CB		((BSP_U32)(1<<3)) /* 该设备支持write 回调函数 */
#define UDI_CAPA_BUFFER_LIST	((BSP_U32)(1<<4)) /* 该设备的读写buffer为内存链表结构(默认为普通内存块) */
#define UDI_CAPA_CTRL_OPT		((BSP_U32)(1<<5)) /* 该设备支持控制操作 */


/**************************************************************************
  结构定义
**************************************************************************/
#define UDI_BUILD_DEV_ID(dev, type) (((BSP_U32)(dev) << 8) | ((BSP_U32)(type) & 0x00ff))
#define UDI_BUILD_CMD_ID(dev, cmd) (((BSP_U32)(dev) << 8) | ((BSP_U32)(cmd) & 0xffff))


typedef s32 UDI_HANDLE;

/* 主设备定义 */
typedef enum tagUDI_DEVICE_MAIN_ID
{
	UDI_DEV_USB_ACM = 0,
	UDI_DEV_USB_NCM,
	UDI_DEV_ICC,
	UDI_DEV_UART,

	UDI_DEV_MAX                 /* 必须在最后, 用于边界值 */
}UDI_DEVICE_MAIN_ID;

/* 各设备类型定义(要和 usb 多设备形态统一起来) */
/* ACM */
typedef enum tagUDI_ACM_DEV_TYPE
{
	UDI_USB_ACM_CTRL,
	UDI_USB_ACM_AT,
	UDI_USB_ACM_SHELL,
	UDI_USB_ACM_LTE_DIAG,
	UDI_USB_ACM_3G_DIAG,
	UDI_USB_ACM_MODEM,
	UDI_USB_ACM_GPS,
	UDI_USB_ACM_MAX                /* 必须在最后, 用于边界值 */
}UDI_ACM_DEV_TYPE;

/* NCM */
typedef enum tagUDI_NCM_DEV_TYPE
{
	UDI_USB_NCM_NDIS,
	UDI_USB_NCM_CTRL,
	UDI_USB_NCM_NDIS1,
	UDI_USB_NCM_CTRL1,
	UDI_USB_NCM_NDIS2,
	UDI_USB_NCM_CTRL2,
	UDI_USB_NCM_NDIS3,
	UDI_USB_NCM_CTRL3,
    UDI_USB_NCM_BOTTOM,

	UDI_USB_NCM_MAX                /* 必须在最后, 用于边界值 */
}UDI_NCM_DEV_TYPE;

/* 设备ID号定义 */
typedef enum tagUDI_DEVICE_ID
{
	/* USB ACM */
	UDI_ACM_CTRL_ID =			UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_ACM_CTRL),
	UDI_ACM_AT_ID =			UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_ACM_AT),
	UDI_ACM_SHELL_ID =			UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_ACM_SHELL),
	UDI_ACM_LTE_DIAG_ID =		UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_ACM_LTE_DIAG),
	UDI_ACM_3G_DIAG_ID =		UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_ACM_3G_DIAG),
	UDI_ACM_MODEM_ID =		UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_ACM_MODEM),
	UDI_ACM_GPS_ID=			UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_ACM_GPS),

	/* USB NCM */
	UDI_NCM_NDIS_ID =			UDI_BUILD_DEV_ID(UDI_DEV_USB_NCM, UDI_USB_NCM_NDIS),
	UDI_NCM_CTRL_ID =			UDI_BUILD_DEV_ID(UDI_DEV_USB_NCM, UDI_USB_NCM_CTRL),

	/* UART */
	UDI_UART_0_ID =				UDI_BUILD_DEV_ID(UDI_DEV_UART, 0),
	UDI_UART_1_ID =				UDI_BUILD_DEV_ID(UDI_DEV_UART, 1),

	/* ICC */
	UDI_ICC_IFC_ID  =			UDI_BUILD_DEV_ID(UDI_DEV_ICC, 0),
	UDI_ICC_IPM_ID  =			UDI_BUILD_DEV_ID(UDI_DEV_ICC, 1),

	/* MUST BE LAST */
	UDI_INVAL_DEV_ID = 0xFFFF
} UDI_DEVICE_ID;

/* 设备的打开参数 */
typedef struct tagUDI_OPEN_PARAM
{
	UDI_DEVICE_ID devid;			/* 设备ID */
	void   *pPrivate;				/* 模块特有的数据 */
} UDI_OPEN_PARAM;

/* IOCTL 命令码,需要的命令码在此添加 */
typedef enum tagUDI_IOCTL_CMD_TYPE
{
	UDI_IOCTL_SET_WRITE_CB = 0xF001,            /* 设置一个起始码值防止与系统定义冲突 */
	UDI_IOCTL_SET_READ_CB,

	UDI_IOCTL_INVAL_CMD = 0xFFFFFFFF
} UDI_IOCTL_CMD_TYPE;

/**************************************************************************
  函数声明
**************************************************************************/
/*****************************************************************************
* 函 数 名  : udi_get_capability
*
* 功能描述  : 根据设备ID获取当前设备支持的特性
*
* 输入参数  : devId: 设备ID
* 输出参数  : 无
* 返 回 值  : 支持的特性值
*****************************************************************************/
s32 udi_get_capability(UDI_DEVICE_ID devId);

/*****************************************************************************
* 函 数 名  : udi_open
*
* 功能描述  : 打开设备(数据通道)
*
* 输入参数  : pParam: 设备的打开配置参数
* 输出参数  : 无
* 返 回 值  : -1:失败 / 其他:成功
*****************************************************************************/
UDI_HANDLE udi_open(UDI_OPEN_PARAM *pParam);

/*****************************************************************************
* 函 数 名  : udi_close
*
* 功能描述  : 关闭设备(数据通道)
*
* 输入参数  : handle: 设备的handle
* 输出参数  : 无
* 返 回 值  : 成功/失败
*****************************************************************************/
s32 udi_close(UDI_HANDLE handle);

/*****************************************************************************
* 函 数 名  : udi_write
*
* 功能描述  : 数据写
*
* 输入参数  : handle:  设备的handle
*             pMemObj: buffer内存 或 内存链表对象
*             u32Size: 数据写尺寸 或 内存链表对象可不设置
* 输出参数  :
*
* 返 回 值  : 完成字节数 或 成功/失败
*****************************************************************************/
s32 udi_write(UDI_HANDLE handle, void* pMemObj, BSP_U32 u32Size);

/*****************************************************************************
* 函 数 名  : udi_read
*
* 功能描述  : 数据读
*
* 输入参数  : handle:  设备的handle
*             pMemObj: buffer内存 或 内存链表对象
*             u32Size: 数据读尺寸 或 内存链表对象可不设置
* 输出参数  :
*
* 返 回 值  : 完成字节数 或 成功/失败
*****************************************************************************/
s32 udi_read(UDI_HANDLE handle, void* pMemObj, BSP_U32 u32Size);

/*****************************************************************************
* 函 数 名  : udi_ioctl
*
* 功能描述  : 数据通道属性配置
*
* 输入参数  : handle: 设备的handle
*             u32Cmd: IOCTL命令码
*             pParam: 操作参数
* 输出参数  :
*
* 返 回 值  : 成功/失败
*****************************************************************************/
s32 udi_ioctl(UDI_HANDLE handle, BSP_U32 u32Cmd, VOID* pParam);

s32 BSP_UDI_Init(VOID);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _BSP_UDI_H_ */

