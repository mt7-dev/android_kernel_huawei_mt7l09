/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  drv_lowpower_mntn.h
*
*   作    者 :  xujingcui
*
*   描    述 : 本文件用于低功耗可谓可测信息的记录和上报
*
*   修改记录 :  2013年6月10日  v1.00 xujingcui创建
*************************************************************************/
#ifndef _BSP_LOWPOWER_MNTN_H_
#define _BSP_LOWPOWER_MNTN_H_
#include <product_config.h>
#include <bsp_hardtimer.h>
#include <bsp_om.h>
#include <bsp_nvim.h>
#include <drv_nv_def.h>
#include <drv_nv_id.h>
#include <osl_irq.h>
#include <hi_syscrg_interface.h>
#include <hi_pwrctrl.h>
#include <bsp_dump.h>
#include <osl_spinlock.h>
#include <bsp_memmap.h>
#include <soc_interrupts_comm.h>

/*each core has lock numbers --MAX_LOCK_NUM_CORE */
#define MAX_LOCK_NUM_CORE    32

/*max lock name*/
#define MAX_LOCK_LEN   32

/*This structure is used for saving Msg from all wakelocks and opposing sleep IP*/
struct lock_status{
	char  name[MAX_LOCK_LEN];
	unsigned int status;                                 /*1: means oppose sleep*/
};

struct lock_info{
	struct lock_status lock_lookup[MAX_LOCK_NUM_CORE];
};

/* 记录申请的DUMP内存信息的结构体 */
struct lp_dump_mem_info
{
    unsigned int lp_dump_base_addr;
    unsigned int lp_dump_base_addr_legth;
    unsigned int lp_current_legth;
#if defined(__KERNEL__)||defined(__VXWORKS__)
    spinlock_t	spinlock;
#endif
};

/*
*此结构体用于HSO显示信息使用
*[0]for earlysuspend
*[1]for dpm,the others for wakelock
*/
extern struct lock_info hi6930_lockinfo;
/*******************************************************
*记录低功耗NV全局变量
********************************************************/
extern ST_PWC_SWITCH_STRU g_nv_pwc_switch;
#ifdef __KERNEL__
/*记录低功耗共享地址*/
extern void* g_lowpower_shared_addr;
#else
extern unsigned int g_lowpower_shared_addr;
#endif

/*******************************************************
*此函数提供用于获取NV控制项
********************************************************/
void bsp_lowpower_mntn_init(void);

/*******************************************************
*此函数提供用于adp_dpm.c 获取NV控制信息
********************************************************/
void  bsp_adp_dpm_debug_init(void);

/*******************************************************
*此函数提供用于adp_dpm.c 显示debug信息
********************************************************/
void debug_adp_dpm(void);
void debug_pastar_dpm(void);

/*******************************************************
*此函数提供用于初始化和更新醒着的时间戳
********************************************************/
void update_awake_time_stamp(void);

/*******************************************************
*此函数用于判断醒来的时间是否超时，并上报
********************************************************/
void check_awake_time_limit(void);

/***********************************************************************
*功能:申请以页为单位的空间
*入参:
*bytes: 需要的字节数,
*refcount:存放引用计数
*返回:成功为首地址，失败为NULL
************************************************************************/
void* dmem_malloc(unsigned int bytes, unsigned int *refcount);

/***********************************************************************
*功能:释放空间
*入参:空间地址指针
*返回:无
************************************************************************/
void dmem_free(void* p);

/***********************************************************************
*功能:申请以页为单位的空间，并修改页表属性为NO CACHE
*入参:
*bytes: 需要的字节数,
*refcount:存放引用计数
*返回:成功为首地址，失败为NULL
************************************************************************/
void* dmem_cache_dma_malloc(unsigned int bytes, unsigned int *refcount);

/***********************************************************************
*功能:释放空间，并修改页表属性
*入参:
*返回:修改属性成功为0  失败为1
************************************************************************/
STATUS dmem_cache_dma_free(void* p);

/***********************************************************************
*功能:修改页表属性为只读
*入参:
*p:空间地址指针
*refcount:存放引用计数
*返回:成功为0 失败为-1
************************************************************************/
STATUS dmem_lock(void* p, unsigned int *refcount);

/***********************************************************************
*功能:修改页表属性为读写
*入参:
*p:空间地址指针
*refcount:存放引用计数
*返回:成功为0 失败为-1
************************************************************************/
STATUS dmem_unlock(void* p, unsigned int *refcount);

/***********************************************************************
*功能:DRV内部函数用于打开BBP PLL(for cipher K3V3)
*入参:其他模块请不要引用此函数
*返回:成功为0 失败为1
************************************************************************/
int bsp_bbp_pll_enable(void);

/***********************************************************************
*功能:DRV内部函数用于关闭BBP PLL(for cipher K3V3)
*入参:其他模块请不要引用此函数
*返回:成功为0 失败为1
************************************************************************/
int bsp_bbp_pll_disable(void);

/***********************************************************************
*功能:用于保存SOC的低功耗寄存器和PMU\PASTAR的寄存器值
*到dump内存中
*入参:无
*返回:无
************************************************************************/
void bsp_save_lowpower_status(void);

/***********************************************************************
*功能:用于导出最后一次保存的SOC的低功耗寄存器和
*PMU\PASTAR的寄存器值
*入参:无
*返回:无
************************************************************************/
void bsp_dump_lowpower_status(void);

/***********************************************************************
*功能:用于导出实时的SOC的低功耗寄存器和
*PMU\PASTAR的寄存器值
*入参:无
*返回:无
************************************************************************/
void bsp_show_lowpower_status(void);

/***********************************************************************
*功能:导出记录的最后一次保存的挂死信息
*入参:无
*返回:无
************************************************************************/
void bsp_dump_bus_error_status(void);
#endif

