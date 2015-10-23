

#ifdef __cplusplus
extern "C"
{
#endif
#include "amon_balong.h"

#ifndef ENABLE_BUILD_AMON
/*****************************************************************************
 函 数 名  : DRV_AXI_DATA_CONFIG
 功能描述  : AXI Monitor采集配置接口
 输入参数  : pData  : 采集配置数据结构指针         
 输出参数  : outLen : 采集寄存器配置状态数据结构长度
 注意事项  : 返回指针指向的内存由驱动分配，调用者释放
 返 回 值  : 配置成功: AXI_DATA_CONFIG_CNF_STRU *  配置失败: NULL
*****************************************************************************/
AXI_DATA_CONFIG_CNF_STRU * DRV_AXI_DATA_CONFIG(BSP_U8 * pData, BSP_U32 * outLen)
{
    return NULL;
}

/*****************************************************************************
 函 数 名  : DRV_AXI_DATA_REG_CONFIG
 功能描述  : AXI Monitor采集寄存器配置接口
 输入参数  : pData: 采集寄存器配置数据结构指针
 输出参数  : 无。
 返 回 值  : 配置成功:0 配置失败:非0
*****************************************************************************/
BSP_U32 DRV_AXI_REG_CONFIG(BSP_U8 * pData)
{
    return BSP_ERROR;
}

/*****************************************************************************
 函 数 名  : DRV_AXI_DATA_CTRL
 功能描述  : AXI Monitor采集控制命令接口:启动、停止、导出
 输入参数  : pData: 导出数据结构首地址
 输出参数  : 无。
 返 回 值  : 成功:0 失败:非0
*****************************************************************************/
BSP_U32 DRV_AXI_DATA_CTRL(BSP_U8 * pData)
{
    return BSP_ERROR;
}

/*****************************************************************************
 函 数 名  : DRV_AXI_DATA_CONFIG
 功能描述  : AXI Monitor监控配置接口
 输入参数  : pData  : 监控配置数据结构指针          
 输出参数  : outLen : 监控寄存器配置状态数据结构长度
 注意事项  : 返回指针指向的内存由驱动分配，调用者释放
 返 回 值  : 配置成功: AXI_MON_CONFIG_CNF_STRU *  配置失败: NULL
*****************************************************************************/
AXI_MON_CONFIG_CNF_STRU * DRV_AXI_MON_CONFIG(BSP_U8 * pData, BSP_U32 * outLen)
{
    return NULL;
}

/*****************************************************************************
 函 数 名  : DRV_AXI_MON_START
 功能描述  : AXI Monitor监控启动接口
 输入参数  : pData: 监控启动数据结构首地址
 输出参数  : 无。
 返 回 值  : 成功:0 失败:非0
*****************************************************************************/
BSP_U32 DRV_AXI_MON_START(BSP_U8 * pData)
{
    return BSP_ERROR;
}

/*****************************************************************************
 函 数 名  : DRV_AXI_MON_TERMINATE
 功能描述  : AXI Monitor监控终止接口
 输入参数  : pData  : 监控终止数据结构指针         
 输出参数  : outLen : 监控终止统计结果数据结构长度
 注意事项  : 返回指针指向的内存由驱动分配，调用者释放
 返 回 值  : 配置成功: AXI_MON_TERMINATE_CNF_STRU *  配置失败: NULL
*****************************************************************************/
AXI_MON_TERMINATE_CNF_STRU * DRV_AXI_MON_TERMINATE(BSP_U8 * pData, BSP_U32 * outLen)
{
    return NULL;
}

#else
/*****************************************************************************
 函 数 名  : DRV_AXI_DATA_CONFIG
 功能描述  : AXI Monitor采集配置接口
 输入参数  : pData  : 采集配置数据结构指针         
 输出参数  : outLen : 采集寄存器配置状态数据结构长度
 注意事项  : 返回指针指向的内存由驱动分配，调用者释放
 返 回 值  : 配置成功: AXI_DATA_CONFIG_CNF_STRU *  配置失败: NULL
*****************************************************************************/
AXI_DATA_CONFIG_CNF_STRU * DRV_AXI_DATA_CONFIG(BSP_U8 * pData, BSP_U32 * outLen)
{
    return bsp_axi_capt_config(pData, outLen);
}

/*****************************************************************************
 函 数 名  : DRV_AXI_DATA_REG_CONFIG
 功能描述  : AXI Monitor采集寄存器配置接口
 输入参数  : pData: 采集寄存器配置数据结构指针
 输出参数  : 无。
 返 回 值  : 配置成功:0 配置失败:非0
*****************************************************************************/
BSP_U32 DRV_AXI_REG_CONFIG(BSP_U8 * pData)
{
    return (BSP_U32)bsp_axi_reg_config(pData);
}

/*****************************************************************************
 函 数 名  : DRV_AXI_DATA_CTRL
 功能描述  : AXI Monitor采集控制命令接口:启动、停止、导出
 输入参数  : pData: 导出数据结构首地址
 输出参数  : 无。
 返 回 值  : 成功:0 失败:非0
*****************************************************************************/
BSP_U32 DRV_AXI_DATA_CTRL(BSP_U8 * pData)
{
    return (BSP_U32)bsp_axi_capt_ctrl(pData);
}

/*****************************************************************************
 函 数 名  : DRV_AXI_DATA_CONFIG
 功能描述  : AXI Monitor监控配置接口
 输入参数  : pData  : 监控配置数据结构指针          
 输出参数  : outLen : 监控寄存器配置状态数据结构长度
 注意事项  : 返回指针指向的内存由驱动分配，调用者释放
 返 回 值  : 配置成功: AXI_MON_CONFIG_CNF_STRU *  配置失败: NULL
*****************************************************************************/
AXI_MON_CONFIG_CNF_STRU * DRV_AXI_MON_CONFIG(BSP_U8 * pData, BSP_U32 * outLen)
{
    return bsp_axi_mon_config(pData, outLen);
}

/*****************************************************************************
 函 数 名  : DRV_AXI_MON_START
 功能描述  : AXI Monitor监控启动接口
 输入参数  : pData: 监控启动数据结构首地址
 输出参数  : 无。
 返 回 值  : 成功:0 失败:非0
*****************************************************************************/
BSP_U32 DRV_AXI_MON_START(BSP_U8 * pData)
{
    return (BSP_U32)bsp_axi_mon_start(pData);
}

/*****************************************************************************
 函 数 名  : DRV_AXI_MON_TERMINATE
 功能描述  : AXI Monitor监控终止接口
 输入参数  : pData  : 监控终止数据结构指针         
 输出参数  : outLen : 监控终止统计结果数据结构长度
 注意事项  : 返回指针指向的内存由驱动分配，调用者释放
 返 回 值  : 配置成功: AXI_MON_TERMINATE_CNF_STRU *  配置失败: NULL
*****************************************************************************/
AXI_MON_TERMINATE_CNF_STRU * DRV_AXI_MON_TERMINATE(BSP_U8 * pData, BSP_U32 * outLen)
{
    return bsp_axi_mon_terminate(pData, outLen);
}
#endif


#ifdef __cplusplus
}
#endif


