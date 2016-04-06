#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#include "drv_ncm.h"
#include "bsp_ncm.h"
#include "drv_udi.h"  

/*****************************************************************************
* 函 数 名  : bsp_ncm_open
*
* 功能描述  : 用来获取可以使用NCM设备ID,数据通道类型，PS使用;控制通道类型，
*              MSP传输AT命令使用
*
* 输入参数  : NCM_DEV_TYPE_E enDevType  设备类型
* 输出参数  : pu32NcmDevId              NCM 设备ID
*
* 返 回 值  : BSP_ERR_NET_NOIDLEDEV
*             OK
*
* 修改记录  : 2010年3月17日   liumengcun  creat
*****************************************************************************/
void* bsp_ncm_open(NCM_DEV_TYPE_E enDevType, BSP_U32 pu32NcmDevId)
{
    return NULL;
}

/*****************************************************************************
* 函 数 名  : bsp_ncm_write
*
* 功能描述  : 发送数据
*
* 输入参数  : BSP_U32 u32NcmDevId  NCM设备ID
                             void *     net_priv tcp/ip传来的参数，不和tcp/ip对接时该参数设置为NULL
* 输出参数  : BSP_VOID *pPktEncap       包封装首地址
*
* 返 回 值  : BSP_OK
*             BSP_ERR_NET_INVALID_PARA
*             BSP_ERR_NET_BUF_ALLOC_FAILED
*
* 修改记录  : 2010年3月17日   liumengcun  creat
*****************************************************************************/
BSP_U32 bsp_ncm_write(void* handle, BSP_VOID *pPktEncap, void *net_priv)
{
    return 0;
}

/*****************************************************************************
* 函 数 名  : bsp_ncm_ioctl
*
* 功能描述  : 配置NCM设备属性
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 
*
* 修改记录  : 2010年3月17日   liumengcun  creat
*****************************************************************************/
BSP_U32 bsp_ncm_ioctl(void *handle, NCM_IOCTL_CMD_TYPE_E enNcmCmd, BSP_VOID *param)
{
    return 0;
}

/*****************************************************************************
* 函 数 名  : bsp_ncm_close
*
* 功能描述  : 关闭NCM设备
*
* 输入参数  : NCM_DEV_TYPE_E enDevType   设备ID类型
*             BSP_U32 u32NcmDevId        NCM设备ID
* 输出参数  : 无
*
* 返 回 值  : OK
*
* 修改记录  : 2010年3月17日   liumengcun  creat
*****************************************************************************/
BSP_U32 bsp_ncm_close(NCM_DEV_TYPE_E enDevType, void *handle)
{
    return 0;
}
