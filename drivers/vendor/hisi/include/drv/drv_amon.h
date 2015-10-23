

#ifndef _DRV_AMON_H
#define _DRV_AMON_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "drv_comm.h"


/* Monitor监控、采集ID最大数目 */
#define AXI_MAX_CONFIG_ID	8
#define AXI_MAX_PORT_CNT    8

/* 寄存器配置 */
typedef struct
{
	BSP_U32 	ulRegOffset;		/* 寄存器偏移 */
	BSP_U32 	ulRegValue;			/* 寄存器值 */
} AXI_REG_CONFIG_STRU;

/* 采集配置回复 */
typedef struct 
{
	BSP_U32 			ulRegCnt;	/* 寄存器配置数目 */
	AXI_REG_CONFIG_STRU regConfig[0];/* 寄存器配置 */	
} AXI_DATA_CONFIG_CNF_STRU;

/* 监控配置恢复 */
typedef AXI_DATA_CONFIG_CNF_STRU AXI_MON_CONFIG_CNF_STRU;

/* 监控ID统计数据 */
typedef struct
{
	BSP_U32	    ulIdEn;				/* ID使能 */
	BSP_U32	    ulIncr1Cnt;			/* INCR1操作统计 */
	BSP_U32	    ulIncr2Cnt;			/* INCR2操作统计 */
	BSP_U32	    ulIncr4Cnt;			/* INCR4操作统计 */
	BSP_U32	    ulIncr8Cnt;			/* INCR8操作统计 */
	BSP_U32	    ulIncr16Cnt;		/* INCR16操作统计 */
	BSP_U32	    ulWrapCnt;			/* Wrap操作统计 */
	BSP_U32	    ulBurstSendCnt;		/* Burst发出操作统计 */
	BSP_U32	    ulBurstFinCnt;		/* Burst完成操作统计 */
	BSP_U32	    ulRdDelayLow;		/* 读操作延迟统计，低位 */
	BSP_U32     ulRdDelayHigh;		/* 读操作延迟统计，高位 */
	BSP_U32	    ulWrDelayLow;		/* 写操作延迟统计，低位 */
	BSP_U32	    ulWrDelayHigh;		/* 写操作延迟统计，高位 */
	BSP_U32	    ulWrRespDelayLow;	/* 写响应操作延迟统计，低位 */
	BSP_U32	    ulWrRespDelayHigh;	/* 写响应操作延迟统计，高位 */
	BSP_U32	    ulRdMaxWait;		/* 读最大等待 */
	BSP_U32	    ulWrMaxWait;		/* 写最大等待 */
	BSP_U32	    ulWrRespMaxWait;	/* 写响应最大等待 */
	BSP_U32	    ulRdTotalCntLow;	/* 读数据量等待，byte，低位 */
	BSP_U32	    ulRdTotalCntHigh;	/* 读数据量等待，byte，高位 */
	BSP_U32	    ulWrTotalCntLow;	/* 写数据量等待，byte，低位 */
	BSP_U32	    ulWrTotalCntHigh;	/* 写数据量等待，byte，高位 */	
} AXI_MON_ID_STATISTIC_DATA_STRU;

/* 监控终止回复：停止、导出操作 */
typedef struct
{
	BSP_U32						    ulIdelCnt;					    /* 空闲计数 */
	BSP_U32	                        ulRdWaitCycle[AXI_MAX_PORT_CNT];/* 统计打开监控功能到精确定位读等待统计开始时所消耗的周期数 */
	BSP_U32	                        ulWrWaitCycle[AXI_MAX_PORT_CNT];/* 统计打开监控功能到精确定位写等待统计开始时所消耗的周期数 */    
	AXI_MON_ID_STATISTIC_DATA_STRU	idStat[AXI_MAX_CONFIG_ID];	    /* 监控ID统计数据 */
} AXI_MON_TERMINATE_CNF_STRU;

/*****************************************************************************
 函 数 名  : DRV_AXI_DATA_CONFIG
 功能描述  : AXI Monitor采集配置接口
 输入参数  : pData  : 采集配置数据结构指针         
 输出参数  : outLen : 采集寄存器配置状态数据结构长度
 注意事项  : 返回指针指向的内存由驱动分配，调用者释放
 返 回 值  : 配置成功: AXI_DATA_CONFIG_CNF_STRU *  配置失败: NULL
*****************************************************************************/
AXI_DATA_CONFIG_CNF_STRU * DRV_AXI_DATA_CONFIG(BSP_U8 * pData, BSP_U32 * outLen);

/*****************************************************************************
 函 数 名  : DRV_AXI_REG_CONFIG
 功能描述  : AXI Monitor采集、监控寄存器配置接口
 输入参数  : pData: 采集、监控寄存器配置数据结构指针
 输出参数  : 无。
 返 回 值  : 配置成功:0 配置失败:非0
*****************************************************************************/
BSP_U32 DRV_AXI_REG_CONFIG(BSP_U8 * pData);

/*****************************************************************************
 函 数 名  : DRV_AXI_DATA_CTRL
 功能描述  : AXI Monitor采集控制命令接口:启动、停止、导出
 输入参数  : pData: 导出数据结构首地址
 输出参数  : 无。
 返 回 值  : 成功:0 失败:非0
*****************************************************************************/
BSP_U32 DRV_AXI_DATA_CTRL(BSP_U8 * pData);

/*****************************************************************************
 函 数 名  : DRV_AXI_DATA_CONFIG
 功能描述  : AXI Monitor监控配置接口
 输入参数  : pData  : 监控配置数据结构指针        
 输出参数  : outLen : 监控寄存器配置状态数据结构长度
 注意事项  : 返回指针指向的内存由驱动分配，调用者释放
 返 回 值  : 配置成功: AXI_MON_CONFIG_CNF_STRU *  配置失败: NULL
*****************************************************************************/
AXI_MON_CONFIG_CNF_STRU * DRV_AXI_MON_CONFIG(BSP_U8 * pData, BSP_U32 * outLen);

/*****************************************************************************
 函 数 名  : DRV_AXI_MON_START
 功能描述  : AXI Monitor监控启动接口
 输入参数  : pData: 监控启动数据结构首地址
 输出参数  : 无。
 返 回 值  : 成功:0 失败:非0
*****************************************************************************/
BSP_U32 DRV_AXI_MON_START(BSP_U8 * pData);

/*****************************************************************************
 函 数 名  : DRV_AXI_MON_TERMINATE
 功能描述  : AXI Monitor监控终止接口
 输入参数  : pData  : 监控终止数据结构指针          
 输出参数  : outLen : 监控终止统计结果数据结构长度
 注意事项  : 返回指针指向的内存由驱动分配，调用者释放
 返 回 值  : 配置成功: AXI_MON_TERMINATE_CNF_STRU *  配置失败: NULL
*****************************************************************************/
AXI_MON_TERMINATE_CNF_STRU * DRV_AXI_MON_TERMINATE(BSP_U8 * pData, BSP_U32 * outLen);

#ifdef __cplusplus
}
#endif

#endif /* end of _DRV_AMON_H*/


