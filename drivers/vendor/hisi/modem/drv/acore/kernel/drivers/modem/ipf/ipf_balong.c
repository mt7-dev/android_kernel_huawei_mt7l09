/*************************************************************************
*   版权所有(C) 1987-2012, 深圳华为技术有限公司.
*
*   文 件 名 :  ipf_balong.c
*
*   作    者 :  luting chendongyue
*
*   描    述 :  本文主要完成IP过滤接口函数，实现上行和下行的IP过滤功能
*
*   修改记录 :  2011年1月11日  v1.00  luting  创建
*                            2012年11月23日v2.00 chendongyue 修改
*                            2013年3月29日v2.10 chendongyue 修改
*
*************************************************************************/
/*lint -save -e429 -e529 -e534 -e550 -e650 -e661 -e715 -e537  -e737 -e539 -e574 -e239 -e438 -e701 -e740 -e958 -e451
-e64 -e732 -e740
*/

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __VXWORKS__
#include <vxWorks.h>
#include <cacheLib.h>
#include <intLib.h>
#include <logLib.h>
#include <string.h>
#include <taskLib.h>
#include <memLib.h>
#include <config.h>
#include <stdio.h>
#include <stdlib.h>

#include "arm_pbxa9.h"
#include "product_config.h"

#include "TtfDrvInterface.h"
#include "bsp_dpm.h"
#include "bsp_hardtimer.h"
#endif

#ifdef __KERNEL__ 
#include <linux/amba/bus.h>
#include <linux/io.h> 
#include <linux/gfp.h>
#include <linux/clkdev.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/mm.h>
#include <linux/mman.h>

#include <asm/system.h>
#include <asm/irq.h>
#include <asm/mach-types.h>
#include <asm/pgtable.h>
#include <asm/mach/arch.h>
#include <asm/mach/irq.h>
#include <asm/mach/map.h>
#include <asm/mach/time.h>
#include <asm/io.h>
#include <asm/system.h>

#endif
#include <bsp_clk.h>
#include <bsp_hardtimer.h>

#if (defined(BSP_CONFIG_HI3630))
#include <bsp_ipc.h>
#endif
#include <bsp_ipf.h>

IPF_UL_S g_stIpfUl = {0};
IPF_DL_S g_stIpfDl = {0};
/* 调试信息结构体 */
IPF_DEBUG_INFO_S* g_stIPFDebugInfo = NULL;
BSP_U32 g_IPFInit;

#if (defined(BSP_CONFIG_HI3630))
/*k3低功耗恢复标记*/
u32* ipf_acore_init_status = NULL;
u32* ipf_ccore_init_status = NULL;
#endif

#ifdef CONFIG_BALONG_MODEM_RESET
/*C核启动(数传允许)标示*/
IPF_FORREST_CONTROL_E* modem_reset_flag = NULL;
#endif

#ifdef __VXWORKS__

IPF_MATCH_INFO_S* g_pstExFilterAddr;
IPF_ID_S* g_stIPFExtFreeList = NULL;
IPF_ID_S* g_stIPFBasicFreeList = NULL;
IPF_ID_S* g_stIPFFreeList = NULL;
IPF_ID_S* g_stIPFHeadList = NULL;
IPF_FILTER_INFO_S g_stIPFFilterInfo[IPF_FILTER_CHAIN_MAX_NUM];
IPF_PWRCTL_FILTER_INFO_S g_stIPFPwrCtlFilterInfo[IPF_FILTER_CHAIN_MAX_NUM];
BSP_U32 g_u32IPFPwrCtlStart = 0;
BSP_U32 g_u32IpfSwitchInt = 0;
spinlock_t ipf_filter_spinlock;
u32 g_filter_delay_time = 10;

BSP_S32 BSP_IPF_BDInfo(IPF_CHANNEL_TYPE_E eChnType, BSP_U32 u32BdqPtr);
BSP_S32 BSP_IPF_RDInfo(IPF_CHANNEL_TYPE_E eChnType, BSP_U32 u32RdqPtr);
BSP_S32 BSP_IPF_ADInfo(IPF_CHANNEL_TYPE_E eChnType, BSP_U32 u32AdqPtr, BSP_U32 u32AdType);
BSP_VOID IPF_AddToBasicFreeList(IPF_ID_S* stNode);
BSP_VOID IPF_AddToExtFreeList(IPF_ID_S* stNode);
IPF_ID_S* IPF_MallocOneBasicFilter(BSP_VOID);
IPF_ID_S* IPF_MallocOneExtFilter(BSP_VOID);
BSP_S32 IPF_ConfigCD(TTF_MEM_ST *pstTtf, BSP_U16* pu16TotalDataLen, BSP_U32* pu32BdInPtr);
BSP_U32 BSP_IPF_GetDlRdNum(BSP_VOID);
BSP_U32 BSP_IPF_GetUlRdNum(BSP_VOID);
BSP_VOID BSP_IPF_UseFilterInfo(IPF_FILTER_CHAIN_TYPE_E eFilterChainhead);
BSP_VOID BSP_IPF_BasicFreeFilterInfo(BSP_VOID);
BSP_VOID BSP_IPF_ExtFreeFilterInfo(BSP_VOID);
BSP_VOID BSP_IPF_FilterInfoHWID(BSP_U32 u32Num);
BSP_VOID BSP_IPF_FilterInfoPSID(IPF_FILTER_CHAIN_TYPE_E eFilterChainhead, BSP_U32 u32Num);
BSP_VOID BSP_IPF_BackupFilter(BSP_U32 u32FilterID);
BSP_S32 BSP_IPF_Shared_DDR_Info(BSP_VOID);


#ifdef CONFIG_CCORE_PM
static int ipf_ul_dpm_prepare(struct dpm_device *dev);
static int ipf_ul_suspend(struct dpm_device *dev);
static int ipf_ul_resume(struct dpm_device *dev);
static int ipf_ul_dpm_complete(struct dpm_device *dev);

struct dpm_device ipf_dpm_device={
	.device_name = "ipf_dpm",
	.prepare = ipf_ul_dpm_prepare,
	.suspend_late = NULL,
	.suspend = ipf_ul_suspend,
	.resume = ipf_ul_resume,
	.resume_early = NULL,
	.complete = ipf_ul_dpm_complete,
};
#endif

#endif

	
#ifdef __KERNEL__
static int ipf_probe(struct platform_device *pdev);
static int ipf_remove(struct platform_device *pdev);

#ifdef CONFIG_BALONG_MODEM_RESET
int bsp_ipf_reset_ccore_cb(DRV_RESET_CALLCBFUN_MOMENT eparam, int userdata);
#endif

#ifdef CONFIG_PM
static int ipf_dl_dpm_prepare(struct device *pdev);
static void ipf_dl_dpm_complete(struct device *pdev);
static int ipf_dl_suspend(struct device *dev);
static int ipf_dl_resume(struct device *dev);


static const struct dev_pm_ops balong_ipf_dev_pm_ops ={
	.prepare = ipf_dl_dpm_prepare,
	.suspend_noirq = ipf_dl_suspend,
	.resume_noirq = ipf_dl_resume,
	.complete = ipf_dl_dpm_complete,
};
#define BALONG_DEV_PM_OPS (&balong_ipf_dev_pm_ops)

#if (defined(BSP_CONFIG_HI3630))
spinlock_t ipf_filter_spinlock;
#endif

#endif

/*C核启动(数传允许)标示，该标志用于Acore决定是否进行上行数传*/
IPF_FORREST_CONTROL_E g_eCcoreResetFlag = IPF_FORRESET_CONTROL_ALLOW;

static struct platform_driver balong_driver_ipf = {
	.probe		= ipf_probe,
	.remove		= ipf_remove,

	.driver		= {
		.name	= "balong_ipf_v700r200",
		.owner	= THIS_MODULE,
#ifdef CONFIG_PM
		.pm		= BALONG_DEV_PM_OPS
#endif
	},
};

struct platform_device balong_device_ipf = {
	.name		  = "balong_ipf_v700r200",
	.id                	  =0,
};



#endif


/* IPF transfer time recorder start */
#define IPF_MAX_STAMP_ORDER             32
#define IPF_MAX_TIME_LIMIT              (19200000*10)
#define IPF_FLS_MASK                    (31)


typedef struct tagIPF_TIMESTAMP_INFO_S
{
    unsigned int diff_order_cnt[IPF_MAX_STAMP_ORDER];
    unsigned int cnt_sum;
    unsigned int diff_sum;
    unsigned int diff_max;
    unsigned int overflow;
}IPF_TIMESTAMP_INFO_S;

#ifdef __KERNEL__
IPF_TIMESTAMP_INFO_S g_ipf_dl_timestamp_info;
unsigned int g_ipf_ul_start_enable;
#define IPF_GET_TIMESTAMP_INFO() (&g_ipf_dl_timestamp_info)
#define IPF_START_STAMP_ENABLE (g_ipf_ul_start_enable)
#endif

#ifdef __VXWORKS__
IPF_TIMESTAMP_INFO_S g_ipf_ul_timestamp_info;
unsigned int g_ipf_dl_start_enable;
#define IPF_GET_TIMESTAMP_INFO() (&g_ipf_ul_timestamp_info)
#define IPF_START_STAMP_ENABLE (g_ipf_dl_start_enable)
#endif

/*
 * On ARMv5 and above those functions can be implemented around
 * the clz instruction.
 * refer to kernel/arch/arm/include/asm/bitops.h
 * put the code here for both vxWorks and linux version.
 */
static inline int ipf_fls(int x)
{
    int ret;

    asm("clz\t%0, %1" : "=r" (ret) : "r" (x));
    ret = 32 - ret;
    return ret;
}

static inline void ipf_record_start_time_stamp(BSP_U32 en, BSP_U32* rec_point)
{
    if (!en) {
        return;
    }
    *rec_point = bsp_get_slice_value_hrt();
    return;
}

static inline void
ipf_record_end_time_stamp(BSP_U32 en, BSP_U32 beg_time)
{
    unsigned int diff_time;
    IPF_TIMESTAMP_INFO_S* stamp_info = IPF_GET_TIMESTAMP_INFO();
    int idx;

    if (!en) {
        return;
    }

    diff_time = bsp_get_slice_value_hrt() - beg_time;

    /* avoid to record the overflowed value */
    if (diff_time > IPF_MAX_TIME_LIMIT) {
        stamp_info->overflow++;
    }
    else {
        if (diff_time > stamp_info->diff_max)
            stamp_info->diff_max = diff_time;

        stamp_info->diff_sum += diff_time;
        stamp_info->cnt_sum++;

        /* find the first bit not zero */
        idx = ((ipf_fls(diff_time)-1) & IPF_FLS_MASK);
        stamp_info->diff_order_cnt[idx]++;
    }
}

static inline
unsigned int ipf_calc_percent(unsigned int value, unsigned int sum)
{
    if (0 == sum) {
        return 0;
    }
    return (value * 100 / sum);
}

void ipf_enable_ul_time_stamp(int en)
{
    g_stIPFDebugInfo->ipf_timestamp_ul_en = en;
    return;
}

void ipf_enable_dl_time_stamp(int en)
{
    g_stIPFDebugInfo->ipf_timestamp_dl_en = en;
    return;
}

void ipf_clear_time_stamp(void)
{
    IPF_TIMESTAMP_INFO_S* stamp_info = IPF_GET_TIMESTAMP_INFO();

    memset(stamp_info, 0, sizeof(IPF_TIMESTAMP_INFO_S));
    return;
}

void ipf_dump_time_stamp(void)
{
    IPF_TIMESTAMP_INFO_S* stamp_info = IPF_GET_TIMESTAMP_INFO();
    unsigned int tmp = 0;
    int i;

    IPF_PRINT(" max diff:%u(%uus)\n",
              stamp_info->diff_max, stamp_info->diff_max*10/192);
    IPF_PRINT(" sum diff:%u(%uus)\n",
              stamp_info->diff_sum, stamp_info->diff_max/19);

    if (stamp_info->cnt_sum) {
        tmp = stamp_info->diff_sum / stamp_info->cnt_sum;
    }

    IPF_PRINT(" avg diff:%u(%uus)\n", tmp, tmp*10/192);

    for (i = 0; i < IPF_MAX_STAMP_ORDER; i++) {
        tmp = ipf_calc_percent(stamp_info->diff_order_cnt[i], stamp_info->cnt_sum);
        IPF_PRINT(" diff time (%u~%u) (%uus~%uus) count:%u (%u %%)\n",
            (0x80000000 >> (31-i)),
            (0xFFFFFFFF >> (31-i)),
            (0x80000000 >> (31-i))/19,
            (0xFFFFFFFF >> (31-i))/19,
            stamp_info->diff_order_cnt[i], tmp);
    }
    return;
}


/* IPF transfer time recorder end */


void ipf_write_basic_filter(u32 filter_hw_id, IPF_MATCH_INFO_S* match_infos)
{
    u32 j;
    u32 match_info;
    u32* match_info_addr = (u32*)match_infos;
    ipf_writel(filter_hw_id, HI_IPF_REGBASE_ADDR_VIRT + HI_BFLT_INDEX_OFFSET);
    for(j=0; j<(sizeof(IPF_MATCH_INFO_S)/4); j++)
    {
        match_info = *(match_info_addr+j);
        ipf_writel((match_info), (HI_IPF_REGBASE_ADDR_VIRT+HI_FLT_LOCAL_ADDR0_OFFSET+j*4)); 
    }
}

/*****************************************************************************
* 函 数 名      : ipf_init
*
* 功能描述  : IPF初始化     内部使用，不作为接口函数
*
* 输入参数  : BSP_VOID
* 输出参数  : 无
* 返 回 值     : IPF_SUCCESS    初始化成功
*                           IPF_ERROR      初始化失败
*
* 修改记录  :2011年1月21日   鲁婷  创建
				 2013年4月30日    陈东岳修改，将寄存器配置分配到两核 
*****************************************************************************/
#ifdef __KERNEL__
BSP_S32 __init ipf_init(BSP_VOID)
#else
BSP_S32 ipf_init(BSP_VOID)
#endif
{
    BSP_U32 u32BDSize[IPF_CHANNEL_MAX] = {IPF_ULBD_DESC_SIZE, IPF_DLBD_DESC_SIZE};
    BSP_U32 u32RDSize[IPF_CHANNEL_MAX] = {IPF_ULRD_DESC_SIZE, IPF_DLRD_DESC_SIZE};
    BSP_U32 u32ADCtrl[IPF_CHANNEL_MAX] = {IPF_ADQ_DEFAULT_SEETING,IPF_ADQ_DEFAULT_SEETING};

#ifdef __VXWORKS__
    BSP_U32 u32IntMask0 = 0;
    BSP_U32 u32IntMask1 = 0;
    BSP_U32 u32ChanCtrl[IPF_CHANNEL_MAX] = {0,0};
    BSP_U32 u32IntStatus = 0;
    BSP_U32 u32Timeout = 0;
    BSP_U32 u32IntGate = 0;
    BSP_U32 u32DMAOutstanding = 3;	
#ifdef CONFIG_CCORE_PM
    BSP_U32 u32Result = 0;
#endif

#else
    BSP_S32 s32Ret = 0;
#endif

#if (defined(BSP_CONFIG_HI3630))
    unsigned long ipf_flags = 0;
#endif

#if !(defined(BSP_CONFIG_HI3630) && defined(__KERNEL__))
    /*ipf enable clock*/
    struct clk *c_IpfClk; 

    c_IpfClk = clk_get(NULL, "ipf_clk");
    if(IS_ERR(c_IpfClk))
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"ipf_init: ipf clk get failed.\n");
        return BSP_ERROR;
    }

    if(BSP_OK !=clk_enable(c_IpfClk))
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"ipf_init: ipf clk open failed.\n");
        return BSP_ERROR;
    }
#endif

#ifdef __VXWORKS__
    /* 初始化全局结构体 */
    memset((BSP_VOID*)IPF_ULBD_MEM_ADDR, 0x0, SHM_MEM_IPF_SIZE - IPF_DLDESC_SIZE);/* [false alarm]:fortify disable */
    
    g_stIpfUl.pstIpfBDQ = (IPF_BD_DESC_S*)IPF_ULBD_MEM_ADDR;
    g_stIpfUl.pstIpfRDQ = (IPF_RD_DESC_S*)IPF_ULRD_MEM_ADDR;
    g_stIpfUl.pstIpfADQ0 = (IPF_AD_DESC_S*)IPF_ULAD0_MEM_ADDR;
    g_stIpfUl.pstIpfADQ1 = (IPF_AD_DESC_S*)IPF_ULAD1_MEM_ADDR;
    g_stIpfUl.pu32IdleBd = (BSP_U32*)IPF_ULBD_IDLENUM_ADDR;
    *(BSP_U32*)IPF_ULBD_IDLENUM_ADDR = IPF_ULBD_DESC_SIZE;
    
    g_stIpfDl.pstIpfBDQ = (IPF_BD_DESC_S*)IPF_DLBD_MEM_ADDR;
    g_stIpfDl.pstIpfRDQ = (IPF_RD_DESC_S*)IPF_DLRD_MEM_ADDR;
    g_stIpfDl.pstIpfADQ0 = (IPF_AD_DESC_S*)IPF_DLAD0_MEM_ADDR;
    g_stIpfDl.pstIpfADQ1 = (IPF_AD_DESC_S*)IPF_DLAD1_MEM_ADDR;
    g_stIpfDl.pstIpfCDQ = (IPF_CD_DESC_S*)IPF_DLCD_MEM_ADDR;
    g_stIpfDl.u32IpfCdRptr = (BSP_U32*) IPF_DLCDRPTR_MEM_ADDR;
    *(g_stIpfDl.u32IpfCdRptr) = 0;
    #ifdef __BSP_IPF_DEBUG__
    g_stIpfDl.pstIpfDebugCDQ = (IPF_CD_DESC_S*)IPF_DEBUG_DLCD_ADDR;
    #endif
    g_stIpfDl.u32IdleBd = IPF_DLBD_DESC_SIZE;
	
    g_stIPFDebugInfo = (IPF_DEBUG_INFO_S*)IPF_DEBUG_INFO_ADDR;

    /* 申请扩展过滤器的内存, 配置扩展过滤器的基址 */
    g_pstExFilterAddr = (IPF_MATCH_INFO_S*)IPF_EXT_FILTER_ADDR;
    memset((BSP_VOID*)g_pstExFilterAddr, 0x0, EXFLITER_NUM*sizeof(IPF_MATCH_INFO_S));
    
    /* 配置扩展过滤器的起始地址 */	
    ipf_writel((BSP_U32)g_pstExFilterAddr, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_EF_BADDR_OFFSET);

    /* 初始化过滤器链表 */
    if(IPF_FilterList_Init() != IPF_SUCCESS)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF, "\r ipf_init malloc list ERROR! \n");
        goto FREE_ERROR;
    }
    
    /* 配置超时配置寄存器，使能超时中断，设置超时时间 */
    u32Timeout = TIME_OUT_CFG | (TIME_OUT_ENABLE);
    ipf_writel(u32Timeout, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_TIME_OUT_OFFSET);

    /* 配置时钟门控配置寄存器，设置为自动门控*/
    u32IntGate = GATE_CFG;
#if (defined(BSP_CONFIG_HI3630))
    ipf_writel(u32IntGate, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_GATE_OFFSET);/* [false alarm]:fortify disable */
#endif
    /* 清除上下行队列 */
    u32ChanCtrl[IPF_CHANNEL_UP] = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_CTRL_OFFSET);
    u32ChanCtrl[IPF_CHANNEL_UP] |= 0x30;
    ipf_writel(u32ChanCtrl[IPF_CHANNEL_UP], HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_CTRL_OFFSET);

    u32ChanCtrl[IPF_CHANNEL_DOWN] = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_CTRL_OFFSET);
    u32ChanCtrl[IPF_CHANNEL_DOWN] |= 0x30;
    ipf_writel(u32ChanCtrl[IPF_CHANNEL_DOWN], HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_CTRL_OFFSET);

    /* 配置上行通道的AD、BD和RD深度 */
    ipf_writel(u32BDSize[IPF_CHANNEL_UP]-1, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_BDQ_SIZE_OFFSET);
    ipf_writel(u32RDSize[IPF_CHANNEL_UP]-1, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_RDQ_SIZE_OFFSET);
    ipf_writel(u32ADCtrl[IPF_CHANNEL_UP], HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_ADQ_CTRL_OFFSET);

    /* 上行通道的BD和RD起始地址*/    
    ipf_writel((BSP_U32)g_stIpfUl.pstIpfBDQ, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_BDQ_BADDR_OFFSET);
    ipf_writel((BSP_U32)g_stIpfUl.pstIpfRDQ, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_RDQ_BADDR_OFFSET);
    ipf_writel((BSP_U32)g_stIpfUl.pstIpfADQ0, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_ADQ0_BASE_OFFSET);
    ipf_writel((BSP_U32)g_stIpfUl.pstIpfADQ1, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_ADQ1_BASE_OFFSET);
	
    /* 配置中断屏蔽,只开5个中断，结果上报，结果超时，RD下溢，AD0、AD1空 */
    u32IntMask0 = IPF_INT_OPEN0;  /* 0号为Modem CPU */
    u32IntMask1 = IPF_INT_OPEN1;
    ipf_writel(u32IntMask0, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_INT_MASK0_OFFSET);
    ipf_writel(u32IntMask1, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_INT_MASK1_OFFSET);

    /* 清中断 */
    u32IntStatus = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_INT_STATE_OFFSET);
    ipf_writel(u32IntStatus, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_INT_STATE_OFFSET);
    /*开DMA通道的Outstanding读写*/
    ipf_writel(u32DMAOutstanding, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_DMA_CTRL1_OFFSET);

    /*初始化ipf过滤器配置自旋锁*/
    spin_lock_init(&ipf_filter_spinlock);
	
    IPF_Int_Connect();
	
#ifdef CONFIG_CCORE_PM

    u32Result = bsp_device_pm_add(&ipf_dpm_device);
    if(OK!= u32Result)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r ipf_dpm_device add error\n");
        return IPF_ERROR;
    }
#if (defined(BSP_CONFIG_HI3630))
    ipf_acore_init_status = (u32*)IPF_INIT_ADDR;
    ipf_ccore_init_status = (u32*)(IPF_INIT_ADDR+4);
	
    spin_lock_irqsave(&ipf_filter_spinlock, ipf_flags);
    bsp_ipc_spin_lock(IPC_SEM_IPF_PWCTRL);
    *ipf_ccore_init_status = IPF_MCORE_INIT_SUCCESS;
    bsp_ipc_spin_unlock(IPC_SEM_IPF_PWCTRL);
    spin_unlock_irqrestore(&ipf_filter_spinlock, ipf_flags);
#endif

#ifdef CONFIG_BALONG_MODEM_RESET
    modem_reset_flag = (IPF_FORREST_CONTROL_E*)(IPF_INIT_ADDR+8);
#endif

#endif

    g_IPFInit = IPF_MCORE_INIT_SUCCESS;

    bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"[IPF] ipf init success\n");
	
    return IPF_SUCCESS;
    
    FREE_ERROR:
    bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r ipf_init malloc ERROR! \n");
    return IPF_ERROR;
	
#else

    memset((BSP_VOID*)IPF_DLBD_MEM_ADDR, 0x0, IPF_DLDESC_SIZE);
	
    g_stIpfUl.pstIpfBDQ = (IPF_BD_DESC_S*)IPF_ULBD_MEM_ADDR;
    g_stIpfUl.pstIpfRDQ = (IPF_RD_DESC_S*)IPF_ULRD_MEM_ADDR;
    g_stIpfUl.pstIpfADQ0 = (IPF_AD_DESC_S*)IPF_ULAD0_MEM_ADDR;
    g_stIpfUl.pstIpfADQ1 = (IPF_AD_DESC_S*)IPF_ULAD1_MEM_ADDR;
    g_stIpfUl.pu32IdleBd = (BSP_U32*)IPF_ULBD_IDLENUM_ADDR;

    g_stIpfDl.pstIpfBDQ = (IPF_BD_DESC_S*)IPF_DLBD_MEM_ADDR;
    g_stIpfDl.pstIpfRDQ = (IPF_RD_DESC_S*)IPF_DLRD_MEM_ADDR;
    g_stIpfDl.pstIpfADQ0 = (IPF_AD_DESC_S*)IPF_DLAD0_MEM_ADDR;
    g_stIpfDl.pstIpfADQ1 = (IPF_AD_DESC_S*)IPF_DLAD1_MEM_ADDR;
    g_stIpfDl.pstIpfCDQ = (IPF_CD_DESC_S*)IPF_DLCD_MEM_ADDR;

    g_stIpfDl.pstIpfPhyBDQ = (IPF_BD_DESC_S*)(IPF_IO_ADDRESS_PHY(IPF_DLBD_MEM_ADDR));
    g_stIpfDl.pstIpfPhyRDQ = (IPF_RD_DESC_S*)(IPF_IO_ADDRESS_PHY(IPF_DLRD_MEM_ADDR));
    g_stIpfDl.pstIpfPhyADQ0 = (IPF_AD_DESC_S*)(IPF_IO_ADDRESS_PHY(IPF_DLAD0_MEM_ADDR));
    g_stIpfDl.pstIpfPhyADQ1 = (IPF_AD_DESC_S*)(IPF_IO_ADDRESS_PHY(IPF_DLAD1_MEM_ADDR));
	
    g_stIpfDl.u32IpfCdRptr = (BSP_U32*) IPF_DLCDRPTR_MEM_ADDR;
    *(g_stIpfDl.u32IpfCdRptr) = 0;

    #if (defined(BSP_CONFIG_HI3630))
    spin_lock_init(&ipf_filter_spinlock);
    #endif
	
    #ifdef __BSP_IPF_DEBUG__
    g_stIpfDl.pstIpfDebugCDQ = (IPF_CD_DESC_S*)IPF_DEBUG_DLCD_ADDR;
    #endif

    g_stIPFDebugInfo = (IPF_DEBUG_INFO_S*)IPF_DEBUG_INFO_ADDR;
	
    /* 配置下行通道的AD、BD和RD深度 */
    ipf_writel(u32BDSize[IPF_CHANNEL_DOWN]-1, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_BDQ_SIZE_OFFSET);
    ipf_writel(u32RDSize[IPF_CHANNEL_DOWN]-1, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_RDQ_SIZE_OFFSET);
    ipf_writel(u32ADCtrl[IPF_CHANNEL_DOWN], HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_ADQ_CTRL_OFFSET);

    /*下行通道的BD和RD起始地址*/    
    ipf_writel((BSP_U32)g_stIpfDl.pstIpfPhyBDQ, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_BDQ_BADDR_OFFSET);
    ipf_writel((BSP_U32)g_stIpfDl.pstIpfPhyRDQ, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_RDQ_BADDR_OFFSET);
    ipf_writel((BSP_U32)g_stIpfDl.pstIpfPhyADQ0, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_ADQ0_BASE_OFFSET);
    ipf_writel((BSP_U32)g_stIpfDl.pstIpfPhyADQ1, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_ADQ1_BASE_OFFSET);

    IPF_Int_Connect();

    s32Ret = platform_device_register(&balong_device_ipf);
    if(s32Ret)
    {

        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,
                        "Platform device register failed! \n");

        return s32Ret;
    }
    s32Ret = platform_driver_register(&balong_driver_ipf);
    if(s32Ret)
    {

        platform_device_unregister(&balong_device_ipf);

        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,
                        "Platform driver register failed! \n");
        return s32Ret;
    }
#if (defined(BSP_CONFIG_HI3630))
    ipf_acore_init_status = (u32*)IPF_INIT_ADDR;
    ipf_ccore_init_status = (u32*)(IPF_INIT_ADDR+4);

    spin_lock_irqsave(&ipf_filter_spinlock, ipf_flags);
    bsp_ipc_spin_lock(IPC_SEM_IPF_PWCTRL);
    *ipf_acore_init_status = IPF_ACORE_INIT_SUCCESS;
    cache_sync();
    bsp_ipc_spin_unlock(IPC_SEM_IPF_PWCTRL);
    spin_unlock_irqrestore(&ipf_filter_spinlock, ipf_flags);
#endif

#ifdef CONFIG_BALONG_MODEM_RESET
    modem_reset_flag = (IPF_FORREST_CONTROL_E*)(IPF_INIT_ADDR+8);
    *modem_reset_flag = IPF_FORRESET_CONTROL_ALLOW;

    if(0 != DRV_CCORERESET_REGCBFUNC("IPF_BALONG",bsp_ipf_reset_ccore_cb, 0, DRV_RESET_CB_PIOR_IPF))
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"ipf set modem reset call back func fail!\n");
    }
#endif

    g_IPFInit = IPF_ACORE_INIT_SUCCESS;

    bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"[IPF]  ipf init success\n");

    return s32Ret;
#endif
}

/*****************************************************************************
* 函 数 名     : IPF_Int_Connect
*
* 功能描述  : 挂IPF中断处理函数(两核都提供)
*
* 输入参数  : BSP_VOID
* 输出参数  : 无
* 返 回 值      : 无
*
* 修改记录  :2011年12月2日   鲁婷  创建
*****************************************************************************/
BSP_VOID IPF_Int_Connect(BSP_VOID)
{
	BSP_S32 s32Result;
#ifdef CONFIG_MODULE_VIC
	bsp_vic_connect((int)INT_LVL_IPF, (vicfuncptr)IPF_IntHandler, (s32)0);    
	bsp_vic_enable(INT_LVL_IPF);

#else 
	s32Result = request_irq(INT_LVL_IPF, (irq_handler_t)IPF_IntHandler, IRQF_NO_SUSPEND, "balong_ipf_v700r200", NULL);
	if(0 != s32Result)
	{
            bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r IPF_Int_Connect Failed! \n",0,0,0,0,0,0);
	}
#endif
}

/*****************************************************************************
* 函 数 名  : IPF_IntHandler
*
* 功能描述  : IPF中断处理函数
*
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : 无
*
* 修改记录  :2011年1月24日   鲁婷  创建


需要改动，增加ADQ空中段的处理函数。未完
*****************************************************************************/
#ifdef __KERNEL__
irqreturn_t  IPF_IntHandler (int irq, void* dev)
#else
BSP_VOID IPF_IntHandler(BSP_VOID)
#endif
{
    BSP_U32 u32IpfInt = 0;

    /* 读取中断状态 */
#ifdef __VXWORKS__ 

    u32IpfInt = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_INT0_OFFSET); 

    /* 上行结果上报和结果超时上报 */
    if(u32IpfInt & (IPF_UL_RPT_INT0|IPF_UL_TIMEOUT_INT0))
    {
        ipf_writel((IPF_UL_RPT_INT0|IPF_UL_TIMEOUT_INT0), HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_INT_STATE_OFFSET);
	
        g_stIPFDebugInfo->ipf_ulbd_done_count++;

        /* 唤醒ps上行任务 */
        if(g_stIpfUl.pFnUlIntCb != NULL)
        {
            (BSP_VOID)g_stIpfUl.pFnUlIntCb();
        }
        else
        {
            bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,
                        "\r IPF_IntHandler ULTASK NULL! \n",0,0,0,0,0,0);
        }
    }

    /* 上行ADQ0、ADQ1都空指示 */	
    if((IPF_UL_ADQ0_EPTY_INT0 | IPF_UL_ADQ1_EPTY_INT0) == (u32IpfInt & (IPF_UL_ADQ0_EPTY_INT0 | IPF_UL_ADQ1_EPTY_INT0)))
    {
        g_stIPFDebugInfo->u32UlAdq0Overflow++;
        g_stIPFDebugInfo->u32UlAdq1Overflow++;

        ipf_writel((IPF_UL_ADQ0_EPTY_INT0 | IPF_UL_ADQ1_EPTY_INT0), HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_INT_STATE_OFFSET);
        if(g_stIpfUl.pAdqEmptyUlCb != NULL)
        {
            (BSP_VOID)g_stIpfUl.pAdqEmptyUlCb(IPF_EMPTY_ADQ);  
            return;
        }
        else
        {
            bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r IPF_IntHandler ULADQEMPTY NULL! \n",0,0,0,0,0,0);
        }
    }

	
    /* 上行ADQ0空指示 */
    if(u32IpfInt & IPF_UL_ADQ0_EPTY_INT0)
    {
        g_stIPFDebugInfo->u32UlAdq0Overflow++;
        ipf_writel(IPF_UL_ADQ0_EPTY_INT0, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_INT_STATE_OFFSET);

        if(g_stIpfUl.pAdqEmptyUlCb != NULL)
        {
            (BSP_VOID)g_stIpfUl.pAdqEmptyUlCb(IPF_EMPTY_ADQ0);  
        }
        else
        {
            bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r IPF_IntHandler ULADQEMPTY NULL! \n",0,0,0,0,0,0);
        }
    }
    /* 上行ADQ1空指示 */	
    if(u32IpfInt & IPF_UL_ADQ1_EPTY_INT0)
    {
       g_stIPFDebugInfo->u32UlAdq1Overflow++;
       ipf_writel(IPF_UL_ADQ1_EPTY_INT0, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_INT_STATE_OFFSET);
       if(g_stIpfUl.pAdqEmptyUlCb != NULL)
       {
           (BSP_VOID)g_stIpfUl.pAdqEmptyUlCb(IPF_EMPTY_ADQ1);  
       }
       else
       {
           bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r IPF_IntHandler ULADQEMPTY NULL! \n",0,0,0,0,0,0);
       }
    }

#else

    u32IpfInt = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_INT1_OFFSET); 

    /* 下行结果上报和结果超时上报 */
    if(u32IpfInt&(IPF_DL_RPT_INT1|IPF_DL_TIMEOUT_INT1))
    {
        ipf_writel((IPF_DL_RPT_INT1|IPF_DL_TIMEOUT_INT1), HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_INT_STATE_OFFSET);

        g_stIPFDebugInfo->ipf_dlbd_done_count++;

        /* 唤醒ps下行任务 */
        if(g_stIpfDl.pFnDlIntCb != NULL)
        {
            (BSP_VOID)g_stIpfDl.pFnDlIntCb();  
        }
        else
        {
            bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r IPF_IntHandler DLTASK NULL! \n",0,0,0,0,0,0);
	 }
    } 
     /* 下行ADQ0、ADQ1都空指示 */	
    if((IPF_DL_ADQ0_EPTY_INT1 | IPF_DL_ADQ1_EPTY_INT1) == (u32IpfInt & (IPF_DL_ADQ0_EPTY_INT1 | IPF_DL_ADQ1_EPTY_INT1)))
    {
        g_stIPFDebugInfo->u32DlAdq0Overflow++;
        g_stIPFDebugInfo->u32DlAdq1Overflow++;
        ipf_writel((IPF_DL_ADQ0_EPTY_INT1 | IPF_DL_ADQ1_EPTY_INT1), HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_INT_STATE_OFFSET);
        if(g_stIpfDl.pAdqEmptyDlCb != NULL)
        {
            (BSP_VOID)g_stIpfDl.pAdqEmptyDlCb(IPF_EMPTY_ADQ);  
        }
        else
        {
            bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r IPF_IntHandler DLADQEMPTY NULL! \n",0,0,0,0,0,0);
        }
        return IRQ_HANDLED;
    }

    /* 下行ADQ0空指示 */
    if(u32IpfInt & IPF_DL_ADQ0_EPTY_INT1)
    {
        g_stIPFDebugInfo->u32DlAdq0Overflow++;
        ipf_writel(IPF_DL_ADQ0_EPTY_INT1, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_INT_STATE_OFFSET);
        if(g_stIpfDl.pAdqEmptyDlCb != NULL)
        {
            (BSP_VOID)g_stIpfDl.pAdqEmptyDlCb(IPF_EMPTY_ADQ0);  
        }
        else
        {
            bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r IPF_IntHandler DLADQEMPTY NULL! \n",0,0,0,0,0,0);
        }
    }
	
    /* 下行ADQ1空指示 */	
    if(u32IpfInt & IPF_DL_ADQ1_EPTY_INT1)
    {
        g_stIPFDebugInfo->u32DlAdq1Overflow++;
        ipf_writel(IPF_DL_ADQ1_EPTY_INT1, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_INT_STATE_OFFSET);
        if(g_stIpfDl.pAdqEmptyDlCb != NULL)
        {
            (BSP_VOID)g_stIpfDl.pAdqEmptyDlCb(IPF_EMPTY_ADQ1);  
        }
        else
        {
            bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r IPF_IntHandler DLADQEMPTY NULL! \n",0,0,0,0,0,0);
        }
    }
    return IRQ_HANDLED;
#endif
}/*lint !e550*/


#if (defined(BSP_CONFIG_HI3630))
/*由于K3V3总线设计问题，ipf无法在M3上恢复ipf寄存器，故移动到acore和ccore*/

/*****************************************************************************
* 函 数 名       : ipf_drx_restore_filter
*
* 功能描述  : 提供的低功耗接口，用于IPF上电恢复
*
* 输入参数  : 无
*             
* 输出参数  : 无
* 返 回 值     : 无
*
* 注意:该函数需要在锁中断的情况下调用
* 修改记录  : 2013年4月23日v1.00 chendongyue 创建
			2014年1月23日v1.01 chendongyue 移植到A9core
*****************************************************************************/
s32 ipf_drx_restore_filter(void)
{
    u32 i,j,k,filter_serial;
    IPF_MATCH_INFO_S* pstMatchInfo;
    u32 match_info;
    u32* match_info_addr = (u32 *)(IPF_PWRCTL_BASIC_FILTER_ADDR);

    for(i=0 ; i < IPF_MODEM_MAX; i++)
    {
        match_info_addr = (u32 *)(IPF_PWRCTL_BASIC_FILTER_ADDR + i*sizeof(IPF_MATCH_INFO_S));
        pstMatchInfo = (IPF_MATCH_INFO_S*)match_info_addr;
        filter_serial = i;
        k = 0;
        do
        {
            if(filter_serial < IPF_BF_NUM)
            {
                ipf_writel(filter_serial, HI_IPF_REGBASE_ADDR_VIRT + HI_BFLT_INDEX_OFFSET); 
                for(j=0; j<(sizeof(IPF_MATCH_INFO_S)/4); j++)
                {
                        match_info = *(match_info_addr+j);
                        ipf_writel((match_info), (HI_IPF_REGBASE_ADDR_VIRT+HI_FLT_LOCAL_ADDR0_OFFSET+j*4)); 
                }
                k++;
            }
            filter_serial = pstMatchInfo->unFltChain.Bits.u16NextIndex;
            pstMatchInfo = (IPF_MATCH_INFO_S*)(IPF_PWRCTL_BASIC_FILTER_ADDR + filter_serial*sizeof(IPF_MATCH_INFO_S));
            match_info_addr = (u32 *)pstMatchInfo;
            if(k >= IPF_BF_NUM)
            {
                break;
            }
        }while(filter_serial != IPF_TAIL_INDEX);
    }
    return IPF_SUCCESS;
}

/*****************************************************************************
* 函 数 名       : ipf_check_filter_restore
*
* 功能描述  : 提供的低功耗接口，用于IPF上电恢复
*
* 输入参数  : 无
*             
* 输出参数  : 无
* 返 回 值     : 无
*
* 
* 修改记录  : 2014年1月23日v1.00 chendongyue创建
*****************************************************************************/
void ipf_check_filter_restore(void)
{
	unsigned long ipf_flags = 0;
	u32 ipf_init_status = 0;
	spin_lock_irqsave(&ipf_filter_spinlock, ipf_flags);
	bsp_ipc_spin_lock(IPC_SEM_IPF_PWCTRL);

	ipf_init_status = (*ipf_acore_init_status)|(*ipf_ccore_init_status);
#ifdef __VXWORKS__
	*ipf_ccore_init_status = IPF_MCORE_INIT_SUCCESS;
#endif

#ifdef __KERNEL__
	*ipf_acore_init_status = IPF_ACORE_INIT_SUCCESS;
#endif
	cache_sync();

	if(IPF_PWC_DOWN != ipf_init_status)
	{
		bsp_ipc_spin_unlock(IPC_SEM_IPF_PWCTRL);
		spin_unlock_irqrestore(&ipf_filter_spinlock, ipf_flags);
		return;
	}
	else
	{
		ipf_drx_restore_filter();
		bsp_ipc_spin_unlock(IPC_SEM_IPF_PWCTRL);
		spin_unlock_irqrestore(&ipf_filter_spinlock, ipf_flags);
		return;
	}
}

#endif

/*****************************************************************************
* 函 数 名      : BSP_IPF_ConfigTimeout
*
* 功能描述  : 调试使用，配置超时时间接口
*
* 输入参数  : BSP_U32 u32Timeout 配置的超时时间
* 输出参数  : 无
* 返 回 值     : IPF_SUCCESS    成功
*                           BSP_ERR_IPF_INVALID_PARA      参数无效
*
* 说明              : 1代表256个时钟周期
*
* 修改记录   : 2011年11月30日   鲁婷  创建
*****************************************************************************/
BSP_S32 BSP_IPF_ConfigTimeout(BSP_U32 u32Timeout)
{
    if((u32Timeout == 0) || (u32Timeout > 0xFFFF))
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r BSP_IPF_ConfigTimeout u32Timeout ERROR! \n");
        return BSP_ERR_IPF_INVALID_PARA;
    }

    u32Timeout |= TIME_OUT_ENABLE;
    ipf_writel(u32Timeout, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_TIME_OUT_OFFSET);
    
    return IPF_SUCCESS;
}

/*****************************************************************************
* 函 数 名      : BSP_IPF_PktLen
*
* 功能描述  : 该接口用来配置过滤器的最大和最小包长
*
* 输入参数  : BSP_U32 MaxLen   最大包长
*                           BSP_U32 MinLen   最小包长
*
* 输出参数   : 无
* 返 回 值      : IPF_SUCCESS                成功
*                           BSP_ERR_IPF_INVALID_PARA   参数错误(最大包长比最小包长小)
*
* 修改记录  :2011年2月17日   鲁婷  创建
*****************************************************************************/
BSP_S32 BSP_IPF_SetPktLen(BSP_U32 u32MaxLen, BSP_U32 u32MinLen)
{
    BSP_U32 u32PktLen = 0;

    /* 参数检查 */
    if(u32MaxLen < u32MinLen)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF, "\r BSP_IPF_PktLen input error! \n");
        return BSP_ERR_IPF_INVALID_PARA;
    }
    
    u32PktLen = ((u32MaxLen&0x3FFF)<<16) | (u32MinLen&0x3FFF);
    
    ipf_writel(u32PktLen, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_PKT_LEN_OFFSET);
    return IPF_SUCCESS;
}/*lint !e550*/

/**********************************可维可测接口函数************************************/
BSP_VOID BSP_IPF_Help(BSP_VOID)
{
    IPF_PRINT("===============================================\n");
    IPF_PRINT("BSP_IPF_Info    参数1:通道类型  0为上行，1为下行\n");
    IPF_PRINT("BSP_IPF_Shared_DDR_Info \n");
    IPF_PRINT("BSP_IPF_BDInfo  参数1:通道类型  参数2:BD指针\n");
    IPF_PRINT("BSP_IPF_RDInfo  参数1:通道类型  参数2:RD指针\n"); 
    IPF_PRINT("BSP_IPF_ADInfo  参数1:通道类型  参数2:AD指针参数3:AD 队列类型0为短,1为长\n"); 
    IPF_PRINT("===============================================\n");
    IPF_PRINT("BSP_IPF_Dump_BDInfo  参数1:通道类型\n");
    IPF_PRINT("BSP_IPF_Dump_RDInfo  参数1:通道类型\n"); 
    IPF_PRINT("BSP_IPF_Dump_ADInfo  参数1:通道类型\n"); 
    IPF_PRINT("===============================================\n");
    IPF_PRINT("ipf_enable_ul_time_stamp  参数1:0-disable, 1-enable\n");
    IPF_PRINT("ipf_enable_dl_time_stamp  参数1:0-disable, 1-enable\n");
    IPF_PRINT("ipf_clear_time_stamp  清除实际戳记录\n");
    IPF_PRINT("ipf_dump_time_stamp  Linux:下行时间差, vxWorks:上行时间差\n");

#ifdef __VXWORKS__    
    IPF_PRINT("===============================================\n");
    IPF_PRINT("BSP_IPF_UseFilterInfo  参数1:通道类型\n"); 
    IPF_PRINT("BSP_IPF_FreeFilterInfo\n"); 
    IPF_PRINT("BSP_IPF_FilterInfoHWID  参数1:硬件Filter ID\n");
    IPF_PRINT("BSP_IPF_FilterInfoPSID  参数1 :PS Filter ID\n");
#endif
    IPF_PRINT("===============================================\n");
    IPF_PRINT("上行配置BD,BD不够用次数(仅当调用GetDesc时有效):            %d\n",g_stIPFDebugInfo->u32UlBdNotEnough);
    IPF_PRINT("上行配置BD,AD0不够用次数(仅当调用GetDesc时有效):            %d\n",g_stIPFDebugInfo->u32UlAd0NotEnough);
    IPF_PRINT("上行配置BD,AD1不够用次数(仅当调用GetDesc时有效):            %d\n",g_stIPFDebugInfo->u32UlAd1NotEnough);
    IPF_PRINT("下行配置BD,BD不够用次数(仅当调用GetDesc时有效):            %d\n",g_stIPFDebugInfo->u32DlBdNotEnough);
    IPF_PRINT("下行配置BD,AD0不够用次数(仅当调用GetDesc时有效):            %d\n",g_stIPFDebugInfo->u32DlAd0NotEnough);
    IPF_PRINT("下行配置BD,AD1不够用次数(仅当调用GetDesc时有效):            %d\n",g_stIPFDebugInfo->u32DlAd1NotEnough);
    IPF_PRINT("下行配置CD,CD不够用次数:            %d\n",g_stIPFDebugInfo->u32DlCdNotEnough);
    IPF_PRINT("中断上报上行BD队列溢出次数:         %d\n",g_stIPFDebugInfo->u32UlBdqOverflow);
    IPF_PRINT("中断上报下行BD队列溢出次数:         %d\n",g_stIPFDebugInfo->u32DlBdqOverflow);
    IPF_PRINT("===============================================\n");
    IPF_PRINT("上行方向给BD长度配置为0次数:            %u\n",g_stIPFDebugInfo->ipf_ulbd_len_zero_count);
    IPF_PRINT("上行方向给AD0配置错误次数:            %u\n",g_stIPFDebugInfo->ipf_ulbd_len_zero_count);
    IPF_PRINT("上行方向给AD1配置错误次数:            %u\n",g_stIPFDebugInfo->ipf_ulbd_len_zero_count);
    IPF_PRINT("下行方向给BD长度配置为0次数:            %u\n",g_stIPFDebugInfo->ipf_dlbd_len_zero_count);
    IPF_PRINT("下行方向给AD0配置错误次数:            %u\n",g_stIPFDebugInfo->ipf_dlbd_len_zero_count);
    IPF_PRINT("下行方向给AD1配置错误次数:            %u\n",g_stIPFDebugInfo->ipf_dlbd_len_zero_count);
    IPF_PRINT("acore尝试在ccore复位时数传次数:            %u\n",g_stIPFDebugInfo->ipf_acore_not_init_count);
    IPF_PRINT("ccore尝试在ccore复位时数传次数:            %u\n",g_stIPFDebugInfo->ipf_ccore_not_init_count);
    IPF_PRINT("===============================================\n");
    IPF_PRINT("上行时间戳功能使能:                  %u\n",g_stIPFDebugInfo->ipf_timestamp_ul_en);
    IPF_PRINT("上行BD配置次数:                      %u\n",g_stIPFDebugInfo->ipf_cfg_ulbd_count);
    IPF_PRINT("上行BD完成中断次数:                  %u\n",g_stIPFDebugInfo->ipf_ulbd_done_count);
    IPF_PRINT("上行RD获取次数:                      %u\n",g_stIPFDebugInfo->ipf_get_ulrd_count);
    IPF_PRINT("上行AD0配置次数:                     %u\n",g_stIPFDebugInfo->ipf_cfg_ulad0_count);
    IPF_PRINT("上行AD1配置次数:                     %u\n",g_stIPFDebugInfo->ipf_cfg_ulad1_count);
    IPF_PRINT("CCore suspend次数:                   %u\n",g_stIPFDebugInfo->ipf_ccore_suspend_count);
    IPF_PRINT("CCore resume次数:                    %u\n",g_stIPFDebugInfo->ipf_ccore_resume_count);
    IPF_PRINT("===============================================\n");
    IPF_PRINT("下行时间戳功能使能:                  %u\n",g_stIPFDebugInfo->ipf_timestamp_dl_en);
    IPF_PRINT("下行BD配置次数:                      %u\n",g_stIPFDebugInfo->ipf_cfg_dlbd_count);
    IPF_PRINT("下行BD完成中断次数:                  %u\n",g_stIPFDebugInfo->ipf_dlbd_done_count);
    IPF_PRINT("下行RD获取次数:                      %u\n",g_stIPFDebugInfo->ipf_get_dlrd_count);
    IPF_PRINT("下行AD0配置次数:                     %u\n",g_stIPFDebugInfo->ipf_cfg_dlad0_count);
    IPF_PRINT("下行AD1配置次数:                     %u\n",g_stIPFDebugInfo->ipf_cfg_dlad1_count);
    IPF_PRINT("ACore suspend次数:                   %u\n",g_stIPFDebugInfo->ipf_acore_suspend_count);
    IPF_PRINT("ACore resume次数:                    %u\n",g_stIPFDebugInfo->ipf_acore_resume_count);
}

BSP_S32 BSP_IPF_Shared_DDR_Info(BSP_VOID)
{
/*	BSP_U32* pIPFInit = (BSP_U32*)IPF_INIT_ADDR;*/
	BSP_U32 ipf_Shared_ddr_start = SHM_MEM_IPF_ADDR;
	
	BSP_U32 ipf_Shared_ddr_ul_start = IPF_ULBD_MEM_ADDR;

	BSP_U32 ipf_Shared_ddr_filter_pwc_start = IPF_PWRCTL_BASIC_FILTER_ADDR;

	BSP_U32 ipf_Shared_ddr_pwc_info_start = IPF_PWRCTL_INFO_ADDR;

	BSP_U32 ipf_Shared_ddr_dlcdrptr = IPF_DLCDRPTR_MEM_ADDR;

	BSP_U32 ipf_Shared_ddr_debug_dlcd_start = IPF_DEBUG_DLCD_ADDR;
	BSP_U32 ipf_Shared_ddr_debug_dlcd_size = IPF_DEBUG_DLCD_SIZE;
	BSP_U32 ipf_Shared_ddr_end = IPF_DEBUG_INFO_END_ADDR;
	BSP_U32 ipf_Shared_ddr_len = IPF_DEBUG_INFO_END_ADDR - SHM_MEM_IPF_ADDR;
	
	IPF_PRINT("ipf_Shared_ddr_start                    value is 0x%x \n", ipf_Shared_ddr_start);
	IPF_PRINT("ipf_Shared_ddr_ul_start                value is 0x%x \n", ipf_Shared_ddr_ul_start);
	IPF_PRINT("ipf_Shared_ddr_filter_pwc_start     value is 0x%x \n", ipf_Shared_ddr_filter_pwc_start);
	IPF_PRINT("ipf_Shared_ddr_pwc_info_start      value is 0x%x \n", ipf_Shared_ddr_pwc_info_start);
	IPF_PRINT("ipf_Shared_ddr_dlcdrptr                value is 0x%x \n", ipf_Shared_ddr_dlcdrptr);
	IPF_PRINT("ipf_Shared_ddr_debug_dlcd_start   value is 0x%x \n", ipf_Shared_ddr_debug_dlcd_start);
	IPF_PRINT("ipf_Shared_ddr_debug_dlcd_size    value is 0x%x \n", ipf_Shared_ddr_debug_dlcd_size);
	IPF_PRINT("ipf_Shared_ddr_end                     value is 0x%x \n", ipf_Shared_ddr_end);
	IPF_PRINT("ipf_Shared_ddr_len                     value is 0x%x (Max len is 0x10000)\n", ipf_Shared_ddr_len);
/*	if(IPF_INIT_SUCCESS != (*pIPFInit))
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r IPF Not Init! \n");
		IPF_PRINT("g_pIPFInit value is 0x%x \n", (*pIPFInit));
		return BSP_ERR_IPF_NOT_INIT;
	}
*/
	return IPF_SUCCESS;
}

BSP_S32 BSP_IPF_BDInfo(IPF_CHANNEL_TYPE_E eChnType, BSP_U32 u32BdqPtr)
{
#ifdef __BSP_IPF_CD_DEBUG__
    BSP_U32 u32CdPtr;
#endif
      
    switch(eChnType)
    {
        case IPF_CHANNEL_UP:
                if(u32BdqPtr >= IPF_ULBD_DESC_SIZE)
                {
                    return IPF_ERROR;
                }
                IPF_PRINT("==========BD Info=========\n");
                IPF_PRINT("BD位置:         %d\n",u32BdqPtr);
                IPF_PRINT("u16Attribute:   %d\n",g_stIpfUl.pstIpfBDQ[u32BdqPtr].u16Attribute);
                IPF_PRINT("u16PktLen:      %d\n",g_stIpfUl.pstIpfBDQ[u32BdqPtr].u16PktLen);
                IPF_PRINT("u32InPtr:       0x%x\n",g_stIpfUl.pstIpfBDQ[u32BdqPtr].u32InPtr);
                IPF_PRINT("u32OutPtr:      0x%x\n",g_stIpfUl.pstIpfBDQ[u32BdqPtr].u32OutPtr);
                IPF_PRINT("u16Resv:        %d\n",g_stIpfUl.pstIpfBDQ[u32BdqPtr].u16Resv);
                IPF_PRINT("u16UsrField1:   %d\n",g_stIpfUl.pstIpfBDQ[u32BdqPtr].u16UsrField1);
                IPF_PRINT("u32UsrField2:   0x%x\n",g_stIpfUl.pstIpfBDQ[u32BdqPtr].u32UsrField2);
                IPF_PRINT("u32UsrField3:   0x%x\n",g_stIpfUl.pstIpfBDQ[u32BdqPtr].u32UsrField3);
            break;
       case IPF_CHANNEL_DOWN:
                if(u32BdqPtr >= IPF_DLBD_DESC_SIZE)
                {
                    return IPF_ERROR;
                }
                IPF_PRINT("==========BD Info=========\n");
                IPF_PRINT("BD位置:         %d\n",u32BdqPtr);
                IPF_PRINT("u16Attribute:   %d\n",g_stIpfDl.pstIpfBDQ[u32BdqPtr].u16Attribute);
                IPF_PRINT("u16PktLen:      %d\n",g_stIpfDl.pstIpfBDQ[u32BdqPtr].u16PktLen);
                IPF_PRINT("u32InPtr:       0x%x\n",g_stIpfDl.pstIpfBDQ[u32BdqPtr].u32InPtr);
                IPF_PRINT("u32OutPtr:      0x%x\n",g_stIpfDl.pstIpfBDQ[u32BdqPtr].u32OutPtr);
                IPF_PRINT("u16Resv:        %d\n",g_stIpfDl.pstIpfBDQ[u32BdqPtr].u16Resv);
                IPF_PRINT("u16UsrField1:   %d\n",g_stIpfDl.pstIpfBDQ[u32BdqPtr].u16UsrField1);
                IPF_PRINT("u32UsrField2:   0x%x\n",g_stIpfDl.pstIpfBDQ[u32BdqPtr].u32UsrField2);
                IPF_PRINT("u32UsrField3:   0x%x\n",g_stIpfDl.pstIpfBDQ[u32BdqPtr].u32UsrField3);
                #ifdef __BSP_IPF_CD_DEBUG__
                #ifdef __VXWORKS__
                u32CdPtr = g_stIpfDl.pstIpfBDQ[u32BdqPtr].u32InPtr-(BSP_U32)g_stIpfDl.pstIpfCDQ;
                #else
                u32CdPtr = IO_ADDRESS(g_stIpfDl.pstIpfBDQ[u32BdqPtr].u32InPtr)-(BSP_U32)g_stIpfDl.pstIpfCDQ;
                #endif
                u32CdPtr = u32CdPtr/sizeof(IPF_CD_DESC_S);
                while(g_stIpfDl.pstIpfDebugCDQ[u32CdPtr].u16Attribute != 1)
                {
                    IPF_PRINT("==========CD Info=========\n");
                    IPF_PRINT("CD位置:             %d\n",u32CdPtr);
                    IPF_PRINT("u16Attribute:       %d\n",g_stIpfDl.pstIpfDebugCDQ[u32CdPtr].u16Attribute);
                    IPF_PRINT("u16PktLen:          %d\n",g_stIpfDl.pstIpfDebugCDQ[u32CdPtr].u16PktLen);
                    IPF_PRINT("u32Ptr:             0x%x\n",g_stIpfDl.pstIpfDebugCDQ[u32CdPtr].u32Ptr);                
                    u32CdPtr = ((u32CdPtr+1) < IPF_DLCD_DESC_SIZE)?(u32CdPtr+1):0;
                };
                IPF_PRINT("==========CD Info=========\n");
                IPF_PRINT("CD位置:             %d\n",u32CdPtr);
                IPF_PRINT("u16Attribute:       %d\n",g_stIpfDl.pstIpfDebugCDQ[u32CdPtr].u16Attribute);
                IPF_PRINT("u16PktLen:          %d\n",g_stIpfDl.pstIpfDebugCDQ[u32CdPtr].u16PktLen);
                IPF_PRINT("u32Ptr:             0x%x\n",g_stIpfDl.pstIpfDebugCDQ[u32CdPtr].u32Ptr);  
                #endif
            break; 
        default:
            break;
    }
    IPF_PRINT("************************\n");
    return 0;
}

BSP_S32 BSP_IPF_Dump_BDInfo(IPF_CHANNEL_TYPE_E eChnType)
{
    BSP_U32 i;
    switch(eChnType)
    {
        case IPF_CHANNEL_UP:
			
            for(i = 0;i < IPF_ULBD_DESC_SIZE;i++)
            {
                BSP_IPF_BDInfo(IPF_CHANNEL_UP,i);
            }
        break;
		
        case IPF_CHANNEL_DOWN:

            for(i = 0;i < IPF_DLBD_DESC_SIZE;i++)
            {
                BSP_IPF_BDInfo(IPF_CHANNEL_DOWN,i);
            }
        break;
				
        default:
        IPF_PRINT("Input param invalid ! \n");
        break;

    }
    return 0;
}

BSP_S32 BSP_IPF_RDInfo(IPF_CHANNEL_TYPE_E eChnType, BSP_U32 u32RdqPtr)
{     
    switch(eChnType)
    {
        case IPF_CHANNEL_UP:
            if(u32RdqPtr >= IPF_ULRD_DESC_SIZE)
            {
                return IPF_ERROR;
            }
            IPF_PRINT("===========RD Info==========\n");
            IPF_PRINT("RD位置:             %d\n",u32RdqPtr);
            IPF_PRINT("u16Attribute:       %d\n",g_stIpfUl.pstIpfRDQ[u32RdqPtr].u16Attribute);
            IPF_PRINT("u16PktLen:          %d\n",g_stIpfUl.pstIpfRDQ[u32RdqPtr].u16PktLen);
            IPF_PRINT("u32InPtr:           0x%x\n",g_stIpfUl.pstIpfRDQ[u32RdqPtr].u32InPtr);
            IPF_PRINT("u32OutPtr:          0x%x\n",g_stIpfUl.pstIpfRDQ[u32RdqPtr].u32OutPtr);
            IPF_PRINT("u16Result:          0x%x\n",g_stIpfUl.pstIpfRDQ[u32RdqPtr].u16Result);
            IPF_PRINT("u16UsrField1:       0x%x\n",g_stIpfUl.pstIpfRDQ[u32RdqPtr].u16UsrField1);
            IPF_PRINT("u32UsrField2:       0x%x\n",g_stIpfUl.pstIpfRDQ[u32RdqPtr].u32UsrField2);
            IPF_PRINT("u32UsrField3:       0x%x\n",g_stIpfUl.pstIpfRDQ[u32RdqPtr].u32UsrField3);
            break;
       case IPF_CHANNEL_DOWN:
            if(u32RdqPtr >= IPF_DLRD_DESC_SIZE)
            {
                return IPF_ERROR;
            }
            IPF_PRINT("============RD Info===========\n");
            IPF_PRINT("RD位置:             %d\n",u32RdqPtr);
            IPF_PRINT("u16Attribute:       %d\n",g_stIpfDl.pstIpfRDQ[u32RdqPtr].u16Attribute);
            IPF_PRINT("u16PktLen:          %d\n",g_stIpfDl.pstIpfRDQ[u32RdqPtr].u16PktLen);
            IPF_PRINT("u32InPtr:           0x%x\n",g_stIpfDl.pstIpfRDQ[u32RdqPtr].u32InPtr);
            IPF_PRINT("u32OutPtr:          0x%x\n",g_stIpfDl.pstIpfRDQ[u32RdqPtr].u32OutPtr);
            IPF_PRINT("u16Result:          0x%x\n",g_stIpfDl.pstIpfRDQ[u32RdqPtr].u16Result);
            IPF_PRINT("u16UsrField1:       0x%x\n",g_stIpfDl.pstIpfRDQ[u32RdqPtr].u16UsrField1);
            IPF_PRINT("u32UsrField2:       0x%x\n",g_stIpfDl.pstIpfRDQ[u32RdqPtr].u32UsrField2);
            IPF_PRINT("u32UsrField3:       0x%x\n",g_stIpfDl.pstIpfRDQ[u32RdqPtr].u32UsrField3);
            break; 
        default:
            break;
    }
    IPF_PRINT("************************\n");
    return 0;
}


BSP_S32 BSP_IPF_Dump_RDInfo(IPF_CHANNEL_TYPE_E eChnType)
{
    int i;

    switch(eChnType)
    {
        case IPF_CHANNEL_UP:

            for(i = 0;i < IPF_ULBD_DESC_SIZE;i++)
            {
                BSP_IPF_RDInfo(IPF_CHANNEL_UP,i);
            }
        break;
		
        case IPF_CHANNEL_DOWN:

            for(i = 0;i < IPF_DLBD_DESC_SIZE;i++)
            {
                BSP_IPF_RDInfo(IPF_CHANNEL_DOWN,i);
            }
        break;
        default:
        IPF_PRINT("Input param invalid ! \n");
        break;
    }
    return 0;
}
BSP_S32 BSP_IPF_ADInfo(IPF_CHANNEL_TYPE_E eChnType, BSP_U32 u32AdqPtr, BSP_U32 u32AdType)
{

    switch(eChnType)
    {
        case IPF_CHANNEL_UP:
            if(u32AdqPtr >= IPF_ULAD0_DESC_SIZE)
            {
                return IPF_ERROR;
            }
            if(0 == u32AdType)
            {
                 IPF_PRINT("===========UL AD0 Info==========\n");
                 IPF_PRINT("AD位置:             %d\n",u32AdqPtr);
                 IPF_PRINT("u32OutPtr0(phy_addr, use by hardware):       0x%x\n",g_stIpfUl.pstIpfADQ0[u32AdqPtr].u32OutPtr0);
                 IPF_PRINT("u32OutPtr1(usrfield skb_addr default):          0x%x\n",g_stIpfUl.pstIpfADQ0[u32AdqPtr].u32OutPtr1);
            }
            else
            {
                 IPF_PRINT("===========UL AD1 Info==========\n");
                 IPF_PRINT("AD位置:             %d\n",u32AdqPtr);
                 IPF_PRINT("u32OutPtr0(phy_addr, use by hardware):       0x%x\n",g_stIpfUl.pstIpfADQ1[u32AdqPtr].u32OutPtr0);
                 IPF_PRINT("u32OutPtr1(usrfield skb_addr default):          0x%x\n",g_stIpfUl.pstIpfADQ1[u32AdqPtr].u32OutPtr1);
            }
            break;
       case IPF_CHANNEL_DOWN:
            if(u32AdqPtr >= IPF_ULAD1_DESC_SIZE)
            {
                return IPF_ERROR;
            }
            if(0 == u32AdType)
	      	{
                 IPF_PRINT("===========DL AD0 Info==========\n");
                 IPF_PRINT("AD位置:             %d\n",u32AdqPtr);
                 IPF_PRINT("u32OutPtr0(phy_addr, use by hardware):       0x%x\n",g_stIpfDl.pstIpfADQ0[u32AdqPtr].u32OutPtr0);
                 IPF_PRINT("u32OutPtr1(usrfield skb_addr default):          0x%x\n",g_stIpfDl.pstIpfADQ0[u32AdqPtr].u32OutPtr1);
            }
            else
            {
                 IPF_PRINT("===========DL AD1 Info==========\n");
                 IPF_PRINT("AD位置:             %d\n",u32AdqPtr);
                 IPF_PRINT("u32OutPtr0(phy_addr, use by hardware):       0x%x\n",g_stIpfDl.pstIpfADQ1[u32AdqPtr].u32OutPtr0);
                 IPF_PRINT("u32OutPtr1(usrfield skb_addr default):          0x%x\n",g_stIpfDl.pstIpfADQ1[u32AdqPtr].u32OutPtr1);
            }
            break;
        default:
            break;
    }
    IPF_PRINT("************************\n");
    return 0;
}


BSP_S32 BSP_IPF_Dump_ADInfo(IPF_CHANNEL_TYPE_E eChnType, BSP_U32 u32AdType)
{
    int i;

    switch(eChnType)
    {
        case IPF_CHANNEL_UP:
            for(i = 0;i < IPF_ULAD0_DESC_SIZE;i++)
            {
                BSP_IPF_ADInfo(IPF_CHANNEL_UP, i, u32AdType);
            }
        break;
		
        case IPF_CHANNEL_DOWN:

            for(i = 0;i < IPF_DLAD0_DESC_SIZE;i++)
            {
                BSP_IPF_ADInfo(IPF_CHANNEL_DOWN, i, u32AdType);
            }
        break;
		
        default:
        IPF_PRINT("Input param invalid ! \n");
        break;
    }
    return 0;
}

BSP_S32 BSP_IPF_Info(IPF_CHANNEL_TYPE_E eChnType)
{
    BSP_U32 u32BdqDepth = 0;
    BSP_U32 u32BdqWptr = 0;
    BSP_U32 u32BdqRptr = 0;
    BSP_U32 u32BdqWaddr = 0;
    BSP_U32 u32BdqRaddr = 0;
    BSP_U32 u32RdqDepth = 0;
    BSP_U32 u32RdqRptr = 0;
    BSP_U32 u32RdqWptr = 0;
    BSP_U32 u32RdqWaddr = 0;
    BSP_U32 u32RdqRaddr = 0;
    BSP_U32 u32Depth = 0;
    BSP_U32 u32status = 0;
	
    BSP_U32 u32Adq0Rptr = 0;
    BSP_U32 u32Adq0Wptr = 0;

    BSP_U32 u32Adq1Rptr = 0;
    BSP_U32 u32Adq1Wptr = 0;

    if(IPF_CHANNEL_UP == eChnType)
    {
        u32Depth = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_DQ_DEPTH_OFFSET);
        u32RdqDepth = (u32Depth>>16)&IPF_DQ_DEPTH_MASK;
        u32BdqDepth = u32Depth&IPF_DQ_DEPTH_MASK;

        u32status = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_STATE_OFFSET);

        u32BdqWptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_BDQ_WPTR_OFFSET);
        u32BdqRptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_BDQ_RPTR_OFFSET); 
        u32BdqWaddr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_BDQ_WADDR_OFFSET); 
        u32BdqRaddr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_BDQ_RADDR_OFFSET); 
    
        u32RdqWptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_RDQ_WPTR_OFFSET);
        u32RdqRptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_RDQ_RPTR_OFFSET);
        u32RdqWaddr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_RDQ_WADDR_OFFSET); 
        u32RdqRaddr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_RDQ_RADDR_OFFSET); 
		
        u32Adq0Rptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_ADQ0_RPTR_OFFSET);
        u32Adq0Wptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_ADQ0_WPTR_OFFSET);

        u32Adq1Rptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_ADQ1_RPTR_OFFSET);
        u32Adq1Wptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_ADQ1_WPTR_OFFSET);

    }
    else if(IPF_CHANNEL_DOWN == eChnType)
    {
        u32Depth = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_DQ_DEPTH_OFFSET);
        u32RdqDepth = (u32Depth>>16)&IPF_DQ_DEPTH_MASK;
        u32BdqDepth = u32Depth&IPF_DQ_DEPTH_MASK;

        u32status = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_STATE_OFFSET);
		
        u32BdqWptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_BDQ_WPTR_OFFSET);
        u32BdqRptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_BDQ_RPTR_OFFSET); 
        u32BdqWaddr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_BDQ_WADDR_OFFSET); 
        u32BdqRaddr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_BDQ_RADDR_OFFSET); 
    
        u32RdqWptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_RDQ_WPTR_OFFSET);
        u32RdqRptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_RDQ_RPTR_OFFSET);
        u32RdqWaddr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_RDQ_WADDR_OFFSET); 
        u32RdqRaddr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_RDQ_RADDR_OFFSET); 

        u32Adq0Rptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_ADQ0_RPTR_OFFSET);
        u32Adq0Wptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_ADQ0_WPTR_OFFSET);

        u32Adq1Rptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_ADQ1_RPTR_OFFSET);
        u32Adq1Wptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_ADQ1_WPTR_OFFSET);

    }
    else
    {
        return 1;
    }
    IPF_PRINT("============================\n");
    IPF_PRINT("通道 状态:            0x%x\n", u32status);
    IPF_PRINT("BD 深度:            %d\n", u32BdqDepth);
    IPF_PRINT("BD 写指针:          %d\n", u32BdqWptr);
    IPF_PRINT("BD 读指针:          %d\n", u32BdqRptr);
    IPF_PRINT("BD 写地址:          0x%x\n", u32BdqWaddr);
    IPF_PRINT("BD 读地址:          0x%x\n", u32BdqRaddr);
    IPF_PRINT("RD 深度:            %d\n", u32RdqDepth);
    IPF_PRINT("RD 读指针:          %d\n", u32RdqRptr);
    IPF_PRINT("RD 写指针:          %d\n", u32RdqWptr);
    IPF_PRINT("RD 读地址:          0x%x\n", u32RdqRaddr);
    IPF_PRINT("RD 写地址:          0x%x\n", u32RdqWaddr);

    IPF_PRINT("AD0 读指针:          %d\n", u32Adq0Rptr);
    IPF_PRINT("AD0 写指针:          %d\n", u32Adq0Wptr);
	
    IPF_PRINT("AD1 读指针:          %d\n", u32Adq1Rptr);
    IPF_PRINT("AD1 写指针:          %d\n", u32Adq1Wptr);
    IPF_PRINT("============================\n");   
    return 0;
}

BSP_VOID BSP_IPF_MEM(BSP_VOID)
{
	BSP_U32 ipf_Shared_ddr_start = SHM_MEM_IPF_ADDR;
	
	BSP_U32 ipf_Shared_ddr_ul_start = IPF_ULBD_MEM_ADDR;

	BSP_U32 ipf_Shared_ddr_filter_pwc_start = IPF_PWRCTL_BASIC_FILTER_ADDR;

	BSP_U32 ipf_Shared_ddr_pwc_info_start = IPF_PWRCTL_INFO_ADDR;

	BSP_U32 ipf_Shared_ddr_dlcdrptr = IPF_DLCDRPTR_MEM_ADDR;

	BSP_U32 ipf_Shared_ddr_debug_dlcd_start = IPF_DEBUG_DLCD_ADDR;
	BSP_U32 ipf_Shared_ddr_debug_dlcd_size = IPF_DEBUG_DLCD_SIZE;
	BSP_U32 ipf_Shared_ddr_end = IPF_DEBUG_INFO_END_ADDR;

	
	IPF_PRINT("ipf_Shared_ddr_start                    value is 0x%x \n", ipf_Shared_ddr_start);
	IPF_PRINT("ipf_Shared_ddr_ul_start                value is 0x%x \n", ipf_Shared_ddr_ul_start);
	IPF_PRINT("ipf_Shared_ddr_filter_pwc_start     value is 0x%x \n", ipf_Shared_ddr_filter_pwc_start);
	IPF_PRINT("ipf_Shared_ddr_pwc_info_start      value is 0x%x \n", ipf_Shared_ddr_pwc_info_start);
	IPF_PRINT("ipf_Shared_ddr_dlcdrptr                value is 0x%x \n", ipf_Shared_ddr_dlcdrptr);
	IPF_PRINT("ipf_Shared_ddr_debug_dlcd_start   value is 0x%x \n", ipf_Shared_ddr_debug_dlcd_start);
	IPF_PRINT("ipf_Shared_ddr_debug_dlcd_size    value is 0x%x \n", ipf_Shared_ddr_debug_dlcd_size);
	IPF_PRINT("ipf_Shared_ddr_end                     value is 0x%x \n", ipf_Shared_ddr_end);
	IPF_PRINT("ipf_Shared_ddr_total_size             value is 0x%x \n", (ipf_Shared_ddr_end-ipf_Shared_ddr_start));


    IPF_PRINT("=======================================\n");
    IPF_PRINT("   BSP_IPF_MEM          ADDR            SIZE\n");
    IPF_PRINT("%s%#x\t\t%#x\n", "IPF_ULBD_MEM_ADDR    ", IPF_ULBD_MEM_ADDR, IPF_ULBD_MEM_SIZE);
    IPF_PRINT("%s%#x\t\t%#x\n", "IPF_ULRD_MEM_ADDR    ", IPF_ULRD_MEM_ADDR, IPF_ULRD_MEM_SIZE);
    IPF_PRINT("%s%#x\t\t%#x\n", "IPF_DLBD_MEM_ADDR    ", IPF_DLBD_MEM_ADDR, IPF_DLBD_MEM_SIZE);
    IPF_PRINT("%s%#x\t\t%#x\n", "IPF_DLRD_MEM_ADDR    ", IPF_DLRD_MEM_ADDR, IPF_DLRD_MEM_SIZE);
    IPF_PRINT("%s%#x\t\t%#x\n", "IPF_DLCD_MEM_ADDR    ", IPF_DLCD_MEM_ADDR, IPF_DLCD_MEM_SIZE);
    IPF_PRINT("%s%#x\t\t%#x\n", "IPF_INIT_ADDR        ", IPF_INIT_ADDR, IPF_INIT_SIZE);
    IPF_PRINT("%20s%#x\t\t%#x\n", "IPF_DEBUG_INFO_ADDR  ", IPF_DEBUG_INFO_ADDR, IPF_DEBUG_INFO_SIZE);
}

#ifdef __VXWORKS__
#ifdef CONFIG_BALONG_MODEM_RESET
/*****************************************************************************
* 函 数 名     : BSP_IPF_GetControlFLagForCcoreReset
*
* 功能描述  : modem单独复位ipf适配函数，用于在复位时阻止下行数传
*
* 输入参数  : 无
*   
* 输出参数  : 无
*
* 返 回 值     : 成功
*
* 修改记录  :2014年2月14日   陈东岳  创建
*****************************************************************************/
static IPF_FORREST_CONTROL_E BSP_IPF_GetControlFLagForCcoreReset(BSP_VOID)
{
    return *modem_reset_flag;
}
#endif


/*****************************************************************************
* 函 数 名  : IPF_FilterList_Init
*
* 功能描述  : IPF过滤器链表初始化     内部使用，不作为接口函数
*
* 输入参数  : BSP_VOID
* 输出参数  : 无
* 返 回 值  : IPF_SUCCESS    过滤器链表初始化成功
*                       IPF_ERROR      过滤器链表初始化失败
*
* 修改记录  :2011年11月17日   鲁婷  创建
*****************************************************************************/
BSP_S32 IPF_FilterList_Init(BSP_VOID)
{
    BSP_U32 i = 0;
    IPF_ID_S* current = NULL;
    IPF_ID_S* prev = NULL;
    IPF_ID_S* tmp = NULL;
	
    /* 初始化可作头节的的过滤器和uselist，从0-7*/	
    for(i =  0; i < IPF_FILTER_CHAIN_MAX_NUM; i++)
    {
        g_stIPFFilterInfo[i].u32FilterNum = 0;
        g_stIPFFilterInfo[i].pstUseList = NULL;
        IPF_DeleteAll(i);
    }

    /* 初始化basiclist  从8号开始，0-7是链表头，单列*/
    for(i =  IPF_FILTER_CHAIN_MAX_NUM; i < IPF_BF_NUM; i++)
    {
        /* coverity[alloc_fn] */
        current = (IPF_ID_S*)malloc(sizeof(IPF_ID_S));
        if(NULL == current)
        {
            bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF, "\r IPF_FilterList_Init malloc ERROR! \n");
            goto FREE_ALL;
        }
        if(g_stIPFBasicFreeList != NULL)
        {
            if(NULL != prev)
            {
                prev->pstNext = current;/*lint !e613*/
            }
            else
            {
                bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF, "\r IPF_FilterList_Init prev == null! \n");
                goto FREE_ALL;
            }
        }
        else /* basicfreelist头结点 */
        {
            g_stIPFBasicFreeList = current;
        }
        current->u32FilterID = i;
        current->u32PsID = 0;
        current->pstNext = NULL;

        prev = current;
    }
	
    prev = NULL;
    /* 初始化extfreelist  从64号开始*/
    for(i =  IPF_BF_NUM; i < IPF_TOTAL_FILTER_NUM; i++)
    {
        current = (IPF_ID_S*)malloc(sizeof(IPF_ID_S));
        if(NULL == current)
        {
            bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF, "\r IPF_FilterList_Init malloc ERROR! \n");
            goto FREE_ALL;
        }
        if(g_stIPFExtFreeList != NULL)
        {
            if(NULL != prev)
            {
                prev->pstNext = current;/*lint !e613*/
            }
            else
            {
                bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF, "\r IPF_FilterList_Init prev == null! \n");
                goto FREE_ALL;
            }
        }
        else /* freelist头结点 */
        {
            g_stIPFExtFreeList = current;
        }
        current->u32FilterID = i;
        current->u32PsID = 0;
        current->pstNext = NULL;
        prev = current;
    }
    	
    return IPF_SUCCESS;
    
FREE_ALL:
    i=0;
	
    /* coverity[overrun-local] */
    while((g_stIPFFilterInfo[i].pstUseList != NULL)&&(i <IPF_FILTER_CHAIN_MAX_NUM))
    {
        free(g_stIPFFilterInfo[i].pstUseList );
        /* coverity[incr] */
        i++;
    }

    while(g_stIPFBasicFreeList!= NULL)
    {
    	tmp = g_stIPFBasicFreeList->pstNext;
        free(g_stIPFBasicFreeList);
        g_stIPFBasicFreeList = tmp;
    }

    while(g_stIPFExtFreeList!= NULL)
    {
    	tmp = g_stIPFExtFreeList->pstNext;
        free(g_stIPFExtFreeList);
        g_stIPFExtFreeList = tmp;
    }

    return IPF_ERROR;
}

/*****************************************************************************
* 函 数 名  : IPF_AddToFreeList
*
* 功能描述  : 将结点加到freelist的合适位置
*
* 输入参数  : IPF_ID_S* stNode             待插入的结点指针
*             
* 输出参数  : 无

* 返 回 值  : 无
*
* 修改记录  : 2011年3月30日   鲁婷  创建
*****************************************************************************/
BSP_VOID IPF_AddToBasicFreeList(IPF_ID_S* stNode)
{
    IPF_ID_S* current = NULL;    
    IPF_ID_S* prev = NULL;

    current = g_stIPFBasicFreeList;

    /* 如果freelist为空 */
    if(NULL == current)
    {
        g_stIPFBasicFreeList = stNode;
        g_stIPFBasicFreeList->pstNext = NULL;
        return;
    }

    /* 如果比第一个结点的ID还小 */
    if(stNode->u32FilterID < current->u32FilterID)
    {
        stNode->pstNext = current;
        g_stIPFBasicFreeList = stNode;       
        return;
    }

    prev = g_stIPFBasicFreeList;
    current = g_stIPFBasicFreeList->pstNext;

    while(current != NULL)
    {
        if(stNode->u32FilterID < current->u32FilterID)
        {
            prev->pstNext = stNode;
            stNode->pstNext = current;
            return;
        }
        prev = current;
        current = current->pstNext;
    }
    /* 放在最后 */
    prev->pstNext = stNode;
    stNode->pstNext = NULL;

}

BSP_VOID IPF_AddToExtFreeList(IPF_ID_S* stNode)
{
    IPF_ID_S* current = NULL;    
    IPF_ID_S* prev = NULL;

    current = g_stIPFExtFreeList;

    /* 如果freelist为空 */
    if(NULL == current)
    {
        g_stIPFExtFreeList = stNode;
        g_stIPFExtFreeList->pstNext = NULL;
        return;
    }

    /* 如果比第一个结点的ID还小 */
    if(stNode->u32FilterID < current->u32FilterID)
    {
        stNode->pstNext = current;
        g_stIPFExtFreeList = stNode;       
        return;
    }

    prev = g_stIPFExtFreeList;
    current = g_stIPFExtFreeList->pstNext;

    while(current != NULL)
    {
        if(stNode->u32FilterID < current->u32FilterID)
        {
            prev->pstNext = stNode;
            stNode->pstNext = current;
            return;
        }
        prev = current;
        current = current->pstNext;
    }
    /* 放在最后 */
    prev->pstNext = stNode;
    stNode->pstNext = NULL;

}

/*****************************************************************************
* 函 数 名  : IPF_DeleteAll
*
* 功能描述  : 删除链表中的所有结点
*
* 输入参数  : IPF_FILTER_CHAIN_TYPE_E eFilterChainhead过滤器链首地址           
*             
* 输出参数  : 无

* 返 回 值  : 无
*
* 修改记录  :2011年3月30日   鲁婷  创建
*****************************************************************************/
BSP_VOID IPF_DeleteAll(IPF_FILTER_CHAIN_TYPE_E eFilterChainhead)
{
    IPF_ID_S* pstMove = NULL;
    /* coverity[var_decl] */
    while(IPF_FILTERHEAD < g_stIPFFilterInfo[eFilterChainhead].u32FilterNum)
    {
        pstMove = g_stIPFFilterInfo[eFilterChainhead].pstUseList;
        /* 将结点从uselist删除 */
        g_stIPFFilterInfo[eFilterChainhead].pstUseList = pstMove->pstNext;
	
        /* 将删除的结点添加到freelist中 */ 
        if(IPF_BF_NUM > (pstMove->u32FilterID))
        {
            IPF_AddToBasicFreeList(pstMove); 
        }
        else
        {	
            IPF_AddToExtFreeList(pstMove); 
        }
        g_stIPFFilterInfo[eFilterChainhead].u32FilterNum--;
    }

    /* 将首过滤器索引设置为511，使能配置为0 */
    ipf_writel(eFilterChainhead, HI_IPF_REGBASE_ADDR_VIRT + HI_BFLT_INDEX_OFFSET);
    ipf_writel(IPF_TAIL_INDEX, HI_IPF_REGBASE_ADDR_VIRT+HI_FLT_CHAIN_OFFSET); 
    ipf_writel(IPF_DISABLE_FILTER, HI_IPF_REGBASE_ADDR_VIRT+HI_FLT_RULE_CTRL_OFFSET); 
    BSP_IPF_BackupFilter(eFilterChainhead);
}

/*****************************************************************************
* 函 数 名  : IPF_MallocOneBasicFilter
*
* 功能描述  : 从freelist中分配一个结点(取出第一个结点)
*
* 输入参数  : 无       
*             
* 输出参数  : 无

* 返 回 值  : 结点指针
*
* 修改记录  : 2011年3月30日   鲁婷  创建
*
* 说明      : 由调用函数来保证一定能分配到结点
*****************************************************************************/
IPF_ID_S* IPF_MallocOneBasicFilter(BSP_VOID)
{
    IPF_ID_S* current = NULL;
    	current = g_stIPFBasicFreeList ;

    if(NULL == current)
    {
        bsp_trace(BSP_LOG_LEVEL_WARNING, BSP_MODU_IPF, "IPF_MallocOneBasicFilter Basic run out\n");
        return NULL;
    }
    g_stIPFBasicFreeList  = g_stIPFBasicFreeList ->pstNext;  

    return current;
}
/*****************************************************************************
* 函 数 名  : IPF_MallocOneExtFilter
*
* 功能描述  : 从freelist中分配一个结点(取出第一个结点)
*
* 输入参数  : 无       
*             
* 输出参数  : 无

* 返 回 值  : 结点指针
*
* 修改记录  : 2011年3月30日   鲁婷  创建
*
* 说明      : 由调用函数来保证一定能分配到结点
*****************************************************************************/
IPF_ID_S* IPF_MallocOneExtFilter(BSP_VOID)
{
    IPF_ID_S* current = NULL;
    	current = g_stIPFExtFreeList ;

    if(NULL == current)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF, "IPF_MallocOneExtFilter Ext run out\n");
        return NULL;
    }
    g_stIPFExtFreeList  = g_stIPFExtFreeList ->pstNext;  

    return current;
}

/*****************************************************************************
* 函 数 名  : IPF_AddTailUsedFilter
*
* 功能描述  : 将结点加到uselist的结尾
*
* 输入参数  : BSP_U8 eFilterChainhead 通道类型   
*             IPF_ID_S* stNode             待插入的结点指针
*             
* 输出参数  : 无

* 返 回 值  : 无
*
* 修改记录  : 2011年3月30日   鲁婷  创建
                             2012年11月25日陈东岳修改
*****************************************************************************/
BSP_VOID IPF_AddTailUsedFilter(IPF_FILTER_CHAIN_TYPE_E eFilterChainhead,IPF_ID_S* stNode)
{
    IPF_ID_S* current = NULL;

    current = g_stIPFFilterInfo[eFilterChainhead].pstUseList;
    if(NULL == current)
    {
        g_stIPFFilterInfo[eFilterChainhead].pstUseList = stNode;
        g_stIPFFilterInfo[eFilterChainhead].pstUseList->pstNext = NULL;
        g_stIPFFilterInfo[eFilterChainhead].u32FilterNum = 1;
        return;
    }
	
    /* 将结点加到链表结尾 */
    while(current->pstNext != NULL)
    {
        current = current->pstNext;
    }
    current->pstNext = stNode;
    stNode->pstNext = NULL;
    
    g_stIPFFilterInfo[eFilterChainhead].u32FilterNum++;

}

/*****************************************************************************
* 函 数 名  : IPF_AddTailFilterChain
*
* 功能描述  :将过滤器配置到硬件中
*
* 输入参数  : BSP_U32 u32LastFilterID, BSP_U32 u32FilterID, IPF_MATCH_INFO_S* pstMatchInfo
*             
* 输出参数  : 无

* 返 回 值  : 无
*
* 修改记录  : 2011年3月30日   鲁婷  创建
*****************************************************************************/
BSP_VOID IPF_AddTailFilterChain(BSP_U32 u32LastFilterID, BSP_U32 u32FilterID, IPF_MATCH_INFO_S* pstMatchInfo)
{
    BSP_U32 u32Value = 0;
    IPF_MATCH_INFO_S* pstLastMatchInfo = NULL;
    
    /* 基本过滤器--将新的过滤器规则配置到过滤器 */
    if(u32FilterID < IPF_BF_NUM)
    {
        ipf_write_basic_filter(u32FilterID, pstMatchInfo);
		
        /*对基本过滤器进行备份*/
        BSP_IPF_BackupFilter(u32FilterID);
    }
    /* 扩展过滤器 */
    else
    {
        memcpy((BSP_VOID*)(g_pstExFilterAddr + u32FilterID - IPF_BF_NUM), pstMatchInfo, sizeof(IPF_MATCH_INFO_S));
    }

    /* 不是第一个filter才需要配置上一个filter的nextindex域 */
    if(u32LastFilterID != IPF_TAIL_INDEX)
    {
        /* 将上次配置最后一个过滤器的nextIndex域重新配置 */
        if(u32LastFilterID < IPF_BF_NUM)
        {
            ipf_writel(u32LastFilterID, HI_IPF_REGBASE_ADDR_VIRT + HI_BFLT_INDEX_OFFSET); 
            u32Value = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_FLT_CHAIN_OFFSET);
            u32Value = (u32Value&0x1FF0000) + u32FilterID;
            ipf_writel(u32Value, HI_IPF_REGBASE_ADDR_VIRT + HI_FLT_CHAIN_OFFSET);
			
            /*对修改next index的基本过滤器进行重新备份*/
            BSP_IPF_BackupFilter(u32LastFilterID);
        }
        else
        {
            pstLastMatchInfo = g_pstExFilterAddr + u32LastFilterID - IPF_BF_NUM;
            pstLastMatchInfo->unFltChain.Bits.u16NextIndex = u32FilterID;        
        }
    }    
}

/*****************************************************************************
* 函 数 名     : IPF_FindFilterID
*
* 功能描述  : 在上下行链表中寻找与PS ID 匹配的Filter ID
*
* 输入参数  : IPF_FILTER_CHAIN_TYPE_E eFilterChainhead    通道类型
*                           BSP_U32 u32PsID                PS ID            
*             
* 输出参数  : BSP_U32* u32FilterID   查询到的Filter ID

* 返 回 值     : IPF_SUCCESS                查询成功
*                          IPF_ERROR                  查询失败
*
* 修改记录  :2011年1月11日   鲁婷  创建
*****************************************************************************/
BSP_S32 IPF_FindFilterID(IPF_FILTER_CHAIN_TYPE_E eFilterChainhead, BSP_U32 u32PsID, BSP_U32* u32FilterID)
{
    IPF_ID_S* current = NULL;

    current = g_stIPFFilterInfo[eFilterChainhead].pstUseList;
    while(current != NULL)
    {
        if(current->u32PsID != u32PsID)
        {
            current = current->pstNext;
        }
        else
        {
            *u32FilterID = current->u32FilterID;
            return IPF_SUCCESS;
        }
    }
    return IPF_ERROR;        
}

/*****************************************************************************
* 函 数 名      : BSP_IPF_Init
*
* 功能描述  : IPF公共参数初始化
*
* 输入参数  : IPF_COMMON_PARA_S *pstCommPara
* 输出参数  : 无
* 返 回 值      : IPF_SUCCESS    配置成功
*                           BSP_ERR_IPF_NOT_INIT     未初始化
*                           BSP_ERR_IPF_INVALID_PARA  参数错误
*
* 说明             : 配置IPF全局控制配置寄存器 建议选择按优先级排序配置
*
* 修改记录  : 2011年11月29日   鲁婷  创建
*****************************************************************************/
BSP_S32 BSP_IPF_Init(IPF_COMMON_PARA_S *pstCommPara)
{
    BSP_U32 u32IPFCtrl = 0;
    BSP_BOOL chn_schedule_strategy = SCH_BD_ONLY;

#ifndef IPF_SCHEDULER_PATCH
    chn_schedule_strategy = SCH_ALL_DESC;
#else
    chn_schedule_strategy = SCH_BD_ONLY;
#endif
    /* 参数检查 */
    if(NULL == pstCommPara)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r BSP_IPF_Init pstCommPara NULL! \n");
        return BSP_ERR_IPF_INVALID_PARA;
    }
    /*初始化检查*/
    if(IPF_MCORE_INIT_SUCCESS != g_IPFInit)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF, "\r IPF NOT Init! \n");
        return BSP_ERR_IPF_NOT_INIT;
    }

    /* 配置IPF全局控制配置寄存器 */
    u32IPFCtrl = pstCommPara->bFilterSeq | (pstCommPara->bFltAddrReverse<<1) | (pstCommPara->bSpPriSel<<2)/*lint !e701*/ 
                 | (pstCommPara->bSpWrrModeSel<<3) | (pstCommPara->eMaxBurst<<4)/*lint !e701*/
                 | (pstCommPara->bIpv6NextHdSel<<6) | (pstCommPara->bEspAhSel<<7)/*lint !e701*/
                 | (pstCommPara->bAhSpiDisable<<8) | (pstCommPara->bEspSpiDisable<<9)/*lint !e701*/
                 | (pstCommPara->bMultiFilterChainEn<<10) | (pstCommPara->bMultiModeEn<<11)
                 | (pstCommPara->bAdReport<<12) | (chn_schedule_strategy<<13);
	
    bsp_trace(BSP_LOG_LEVEL_DEBUG, BSP_MODU_IPF, "\r u32IPFCtrl = 0x%x \n", u32IPFCtrl);
    ipf_writel(u32IPFCtrl, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CTRL_OFFSET); 

    return  IPF_SUCCESS; 
}

/*****************************************************************************
* 函 数 名      : BSP_IPF_ConfigUlChannel
*
* 功能描述  : 配置上行通道控制参数
*
* 输入参数  : IPF_CHL_CTRL_S *pstCtrl 
* 输出参数  : 无
* 返 回 值      : 无
*
* 说明             : 配置上行IPF通道控制寄存器
*
* 修改记录  : 2011年11月29日   鲁婷  创建
			 2014年1月23日v1.01 陈东岳 修改 由于K3V3总线设计问题，
			 无法在m3上进行低功耗恢复，移动到A9上进行。
*****************************************************************************/
BSP_S32 BSP_IPF_ConfigUlChannel(IPF_CHL_CTRL_S *pstCtrl)
{
    BSP_U32 u32ChanCtrl = 0; 
	
#if (defined(BSP_CONFIG_HI3630))
    /*由于K3V3总线设计问题，使用ipf 前需要检测是否需要恢复过滤器*/
    ipf_check_filter_restore();
#endif

    /* 参数检查 */
    if(NULL == pstCtrl)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r BSP_IPF_ConfigUlChannel pstCtrl NULL! \n");
        return BSP_ERR_IPF_INVALID_PARA;
    }

    /*配置上行通道控制寄存器 */
    u32ChanCtrl = pstCtrl->eIpfMode | (pstCtrl->bEndian<<2) | 
                            (pstCtrl->bDataChain<<3) | (pstCtrl->u32WrrValue<<16);
    bsp_trace(BSP_LOG_LEVEL_DEBUG, BSP_MODU_IPF,"\r u32ChanCtrl = 0x%x \n", u32ChanCtrl);
    ipf_writel(u32ChanCtrl, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_CTRL_OFFSET);  

    return IPF_SUCCESS;
}

/*****************************************************************************
* 函 数 名      : BSP_IPF_ConfigDlChannel
*
* 功能描述  : 配置上行通道控制参数
*
* 输入参数  : IPF_CHL_CTRL_S *pstCtrl 
* 输出参数  : 无
* 返 回 值      : 无
*
* 说明             : 配置下行IPF通道控制寄存器
*
* 修改记录  : 2011年11月29日   鲁婷  创建
			 2014年1月23日v1.01 陈东岳 修改 由于K3V3总线设计问题，
			 无法在m3上进行低功耗恢复，移动到A9上进行。
*****************************************************************************/
BSP_S32 BSP_IPF_ConfigDlChannel(IPF_CHL_CTRL_S *pstCtrl)
{
    BSP_U32 u32ChanCtrl = 0; 

#if (defined(BSP_CONFIG_HI3630))
    /*由于K3V3总线设计问题，使用ipf 前需要检测是否需要恢复过滤器*/
    ipf_check_filter_restore();
#endif

    /* 参数检查 */
    if(NULL == pstCtrl)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r BSP_IPF_ConfigDlChannel pstCtrl NULL! \n");
        return BSP_ERR_IPF_INVALID_PARA;
    }
    
    /*配置上行通道控制寄存器 */
    u32ChanCtrl = pstCtrl->eIpfMode | (pstCtrl->bEndian<<2) | 
                            (pstCtrl->bDataChain<<3) | (pstCtrl->u32WrrValue<<16);
    bsp_trace(BSP_LOG_LEVEL_DEBUG, BSP_MODU_IPF,"\r u32ChanCtrl = 0x%x \n", u32ChanCtrl);
    ipf_writel(u32ChanCtrl, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_CTRL_OFFSET);

    return IPF_SUCCESS;
}

/*****************************************************************************
* 函 数 名      : BSP_IPF_ConfigADThr
*
* 功能描述  : 配置上下行通道ADq长短包阀值
*
* 输入参数  : BSP_U32 u32UlADThr,BSP_U32 u32DlADThr
* 输出参数  : 无
* 返 回 值      : 无
*
* 说明             : 配置上下行通道ADq长短包阀值
*
* 修改记录  : 2011年12月13日   陈东岳创建
			 2014年1月23日v1.01 陈东岳 修改 由于K3V3总线设计问题，
			 无法在m3上进行低功耗恢复，移动到A9上进行。
*****************************************************************************/
BSP_S32 BSP_IPF_ConfigADThr(BSP_U32 u32UlADThr,BSP_U32 u32DlADThr)
{
    BSP_U32 u32ADCtrl[IPF_CHANNEL_MAX] = {0,0};

    /* 检查模块是否初始化 */
    if(IPF_MCORE_INIT_SUCCESS != g_IPFInit)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r IPF Not Init! \n");
        return BSP_ERR_IPF_NOT_INIT;
    }
	
#if (defined(BSP_CONFIG_HI3630))
    /*由于K3V3总线设计问题，使用ipf 前需要检测是否需要恢复过滤器*/
    ipf_check_filter_restore();
#endif

    if((u32UlADThr == 0)  || (u32UlADThr > 0xFFFF) || (u32DlADThr == 0)  || (u32DlADThr > 0xFFFF))
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r BSP_IPF_ConfigADThr INPUT ERROR! \n");
        return BSP_ERR_IPF_INVALID_PARA;
    }

    u32ADCtrl[IPF_CHANNEL_UP] = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_ADQ_CTRL_OFFSET);
    u32ADCtrl[IPF_CHANNEL_DOWN] = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_ADQ_CTRL_OFFSET);
    u32ADCtrl[IPF_CHANNEL_UP] &= IPF_ADQ_THR_MASK;
    u32ADCtrl[IPF_CHANNEL_DOWN] &= IPF_ADQ_THR_MASK;
    u32ADCtrl[IPF_CHANNEL_UP] |= (u32UlADThr<<16);
    u32ADCtrl[IPF_CHANNEL_DOWN] |= (u32DlADThr<<16);
    /* 配置上下行通道的AD阀值*/
    ipf_writel(u32ADCtrl[IPF_CHANNEL_UP], HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_ADQ_CTRL_OFFSET);
    ipf_writel(u32ADCtrl[IPF_CHANNEL_DOWN], HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_ADQ_CTRL_OFFSET);
    return IPF_SUCCESS;
}
BSP_S32 BSP_IPF_SetFilter(IPF_FILTER_CHAIN_TYPE_E eFilterChainhead, IPF_FILTER_CONFIG_S *pstFilterInfo, BSP_U32 u32FilterNum)
{
	BSP_U32 i = 0;
	IPF_ID_S* current = NULL;
	BSP_U32 u32LastFilterID = IPF_TAIL_INDEX;
	BSP_U32 u32FirstFilterID = IPF_TAIL_INDEX;
	unsigned long flags = 0;

#if (defined(BSP_CONFIG_HI3630))
    /*由于K3V3总线设计问题，使用ipf 前需要检测是否需要恢复过滤器*/
    ipf_check_filter_restore();
#endif

	/* 参数检查 */
	if((eFilterChainhead >= IPF_FILTER_CHAIN_MAX_NUM) || (NULL == pstFilterInfo))
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r BSP_IPF_SetFilter input para ERROR! \n");
		return BSP_ERR_IPF_INVALID_PARA;
	}

	/* 检查模块是否初始化 */
	if(IPF_MCORE_INIT_SUCCESS != g_IPFInit)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r IPF Not Init! \n");
		return BSP_ERR_IPF_NOT_INIT;
	}

	spin_lock_irqsave(&ipf_filter_spinlock, flags);
#if (defined(BSP_CONFIG_HI3630))
	bsp_ipc_spin_lock(IPC_SEM_IPF_PWCTRL);
#endif
	/* 清除该寄存器链首地址以外的所有结点 */
	IPF_DeleteAll(eFilterChainhead);
	udelay(g_filter_delay_time);
		
	/*分配滤器*/
	for(i = 0; i < u32FilterNum; i++)
	{
        /* 从freelist中取出未配置的过滤器 */
		if(!(IPF_SET_EXT_FILTER & (pstFilterInfo->u32FilterID)))
		{    
			current = IPF_MallocOneBasicFilter();
			if(NULL == current)
			{		
				current = IPF_MallocOneExtFilter();
				if(NULL == current)
				{		
				#if (defined(BSP_CONFIG_HI3630))
					bsp_ipc_spin_unlock(IPC_SEM_IPF_PWCTRL);
				#endif
					spin_unlock_irqrestore(&ipf_filter_spinlock, flags);
					bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r IPF_MallocOneFilter error! \n");
					return BSP_ERR_IPF_FILTER_NOT_ENOUGH;
				}
			}		
		}
		else
		{
			current = IPF_MallocOneExtFilter();
			if(NULL == current)
			{
			#if (defined(BSP_CONFIG_HI3630))
				bsp_ipc_spin_unlock(IPC_SEM_IPF_PWCTRL);
			#endif
				spin_unlock_irqrestore(&ipf_filter_spinlock, flags);
				bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r IPF_MallocOneFilter error! \n");
				return BSP_ERR_IPF_FILTER_NOT_ENOUGH;
			}
		}
		if(0 == i)
		{
			u32FirstFilterID = current->u32FilterID;
		}
		current->u32PsID = (pstFilterInfo->u32FilterID)&IPF_FILTERTYPE_MASK;
		/* 先将NextIndex配置为511 */
		pstFilterInfo->stMatchInfo.unFltChain.Bits.u16NextIndex = IPF_TAIL_INDEX;
		IPF_AddTailFilterChain(u32LastFilterID, current->u32FilterID, &pstFilterInfo->stMatchInfo);
		IPF_AddTailUsedFilter(eFilterChainhead, current);
		u32LastFilterID = current->u32FilterID;
		pstFilterInfo++; 
	}
	
	/*将配置好的过滤器链挂到链首上*/
	ipf_writel(eFilterChainhead, HI_IPF_REGBASE_ADDR_VIRT + HI_BFLT_INDEX_OFFSET);
	ipf_writel(u32FirstFilterID, HI_IPF_REGBASE_ADDR_VIRT+HI_FLT_CHAIN_OFFSET); 
	
	/*备份更改后的首过滤器*/
	BSP_IPF_BackupFilter(eFilterChainhead);
	cache_sync();

	#if (defined(BSP_CONFIG_HI3630))
		bsp_ipc_spin_unlock(IPC_SEM_IPF_PWCTRL);
	#endif
	spin_unlock_irqrestore(&ipf_filter_spinlock, flags);

	return IPF_SUCCESS;
}
/*****************************************************************************
* 函 数 名      : BSP_IPF_GetFilter
*
* 功能描述  : 提供给PS查询某个Filter配置信息
*
* 输入参数  : BSP_U32 u32FilterID   Filter ID号注意这个是psid
*             
* 输出参数  : IPF_FILTER_CONFIG_S * pFilterInfo  查询到的Filter信息

* 返 回 值     : IPF_SUCCESS                查询成功
*                           IPF_ERROR                  查询失败
*                           BSP_ERR_IPF_NOT_INIT       模块未初始化
*                           BSP_ERR_IPF_INVALID_PARA   参数错误
*
* 修改记录  : 2011年1月11日   鲁婷  创建
                             2011年3月30日   鲁婷  修改
                             2011    11月30日   鲁婷  修改
                             2012    11月26日   陈东岳修改
				 			 2014年1月23日v1.01 陈东岳 修改 由于K3V3总线设计问题，
							 无法在m3上进行低功耗恢复，移动到A9上进行。
*****************************************************************************/
BSP_S32 BSP_IPF_GetFilter(IPF_FILTER_CHAIN_TYPE_E eFilterChainhead, BSP_U32 u32FilterID, IPF_FILTER_CONFIG_S *pstFilterInfo)
{
    BSP_U32 u32FindID = 0;
    
    
    /* 检查模块是否初始化 */
    if(IPF_MCORE_INIT_SUCCESS != g_IPFInit)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r IPF Not Init! \n");
        return BSP_ERR_IPF_NOT_INIT;
    }
	
#if (defined(BSP_CONFIG_HI3630))
    /*由于K3V3总线设计问题，使用ipf 前需要检测是否需要恢复过滤器*/
    ipf_check_filter_restore();
#endif

    /* 参数检查 */
    if(NULL == pstFilterInfo)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF, "\r BSP_IPF_GetFilter input para ERROR! \n");
        return BSP_ERR_IPF_INVALID_PARA;
    }

    /* 查找PS ID 对应的FilterID*/
    if(IPF_FindFilterID(eFilterChainhead, u32FilterID, &u32FindID) != IPF_SUCCESS)
    {
        return IPF_ERROR;
    }
    pstFilterInfo->u32FilterID = u32FilterID;    
    
    /* 如果是基本过滤器 */
    if(u32FindID < IPF_BF_NUM)
    {
        /* 写过滤表操作地址 */    
        ipf_writel(u32FindID, HI_IPF_REGBASE_ADDR_VIRT + HI_BFLT_INDEX_OFFSET); 
         
        /* 读出过滤器的配置 */
        memcpy(&pstFilterInfo->stMatchInfo, (BSP_VOID*)(HI_IPF_REGBASE_ADDR_VIRT+HI_FLT_LOCAL_ADDR0_OFFSET), sizeof(IPF_MATCH_INFO_S));/* [false alarm]:fortify disable */
    }
    /* 扩展过滤器 */
    else
    {
        /* 读出过滤器的配置 */
        memcpy(&pstFilterInfo->stMatchInfo, (BSP_VOID*)(g_pstExFilterAddr + u32FindID - IPF_BF_NUM), sizeof(IPF_MATCH_INFO_S));
    }
    return IPF_SUCCESS;
}

/*****************************************************************************
* 函 数 名      : IPF_ConfigCD
*
* 功能描述  : 配置CD
*
* 输入参数  : IPF_TTF_MEM_ST *pstTtf, BSP_U16* pu16TotalDataLen, BSP_U32* pu32BdInPtr
* 输出参数  : 无
* 返 回 值      :  无
*
* 修改记录  :2011年1月24日   鲁婷  创建
*****************************************************************************/
BSP_S32 IPF_ConfigCD(TTF_MEM_ST *pstTtf, BSP_U16* pu16TotalDataLen, BSP_U32* pu32BdInPtr)
{
    BSP_U32 u32DlCDWptr = 0;
    BSP_U32 u32DlCDRptr = 0;
    BSP_U16 u16TotalDataLen = 0;
    TTF_MEM_ST* p = NULL;

    /* 为防止BD 满或者后续申请目的地址不成功，设置临时cd的位置 */
    u32DlCDWptr = g_stIpfDl.u32IpfCdWptr;
    u32DlCDRptr = *(g_stIpfDl.u32IpfCdRptr);
    *pu32BdInPtr = (BSP_U32)(g_stIpfDl.pstIpfCDQ + (u32DlCDWptr));

    /* 配置CD */
    p = pstTtf;
    do
    {
        /* 判断是否有空闲CD */
        if(u32DlCDRptr != (u32DlCDWptr+1)%IPF_DLCD_DESC_SIZE)
        {
            g_stIpfDl.pstIpfCDQ[u32DlCDWptr].u32Ptr = (BSP_U32)p->pData;
            g_stIpfDl.pstIpfCDQ[u32DlCDWptr].u16PktLen = p->usUsed;
            #ifdef __BSP_IPF_CD_DEBUG__
            g_stIpfDl.pstIpfDebugCDQ[u32DlCDWptr].u32Ptr = (BSP_U32)p->pData;
            g_stIpfDl.pstIpfDebugCDQ[u32DlCDWptr].u16PktLen = p->usUsed;
            if(g_stIPFDebugInfo->u32IpfDebug)
            {
                IPF_PRINT("usUsed = %d  pData = 0x%x\n", p->usUsed, p->pData);
            }
            #endif
            u16TotalDataLen += p->usUsed;
            
            /* 标识是否结束 */
            if(p->pNext != NULL)
            {
                g_stIpfDl.pstIpfCDQ[u32DlCDWptr].u16Attribute = 0;
                #ifdef __BSP_IPF_CD_DEBUG__
                g_stIpfDl.pstIpfDebugCDQ[u32DlCDWptr].u16Attribute = 0;
                #endif
                if(u32DlCDWptr+1 < IPF_DLCD_DESC_SIZE)
                {
                    u32DlCDWptr += 1;
                    p = p->pNext;
                }
                /* 未结束时，CD不能翻转，这是记录CD的尾部，同时将从0开始配置CD */
                else
                {
                    u32DlCDWptr = 0;
                    u16TotalDataLen = 0;
                    p = pstTtf;
                    *pu32BdInPtr = (BSP_U32)g_stIpfDl.pstIpfCDQ;
                    #ifdef __BSP_IPF_CD_DEBUG__
                    if(g_stIPFDebugInfo->u32IpfDebug)
                    {
                        IPF_PRINT("CD 翻转 \n");
                    }
                    #endif
                }
            }
            else
            {
                g_stIpfDl.pstIpfCDQ[u32DlCDWptr].u16Attribute = 1;
                #ifdef __BSP_IPF_CD_DEBUG__
                g_stIpfDl.pstIpfDebugCDQ[u32DlCDWptr].u16Attribute = 1;
                #endif
                
                /* 结束时CD可以翻转 */
                u32DlCDWptr = (u32DlCDWptr+1 < IPF_DLCD_DESC_SIZE)?(u32DlCDWptr+1):0;
                #ifdef __BSP_IPF_CD_DEBUG__
                if(g_stIPFDebugInfo->u32IpfDebug)
                {
                    IPF_PRINT("u32DlCDWptr  = %d\n", u32DlCDWptr);
                }
                #endif
                break;
            }
        }
        else
        {
            bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,
                    "\r DownFilter CD FULL ! u32DlCDRptr=%d u32DlCDWptr=%d\n",u32DlCDRptr,u32DlCDWptr);
            return BSP_ERR_IPF_CDQ_NOT_ENOUGH;
        }        
    }while(p != NULL);
    
    g_stIpfDl.u32IpfCdWptr = u32DlCDWptr;
    *pu16TotalDataLen = u16TotalDataLen;    
    return IPF_SUCCESS;
}

/*****************************************************************************
* 函 数 名      : BSP_IPF_ConfigDownFilter
*
* 功能描述  : IPF下行BD配置函数 
*
* 输入参数  : BSP_U32 u32Num, IPF_CONFIG_ULPARAM_S* pstUlPara
* 输出参数  : 无
* 返 回 值      : IPF_SUCCESS    配置成功
*                           IPF_ERROR      配置失败
*                           BSP_ERR_IPF_NOT_INIT         模块未初始化
*                           BSP_ERR_IPF_INVALID_PARA     参数错误
*
* 修改记录  :2011年11月30日   鲁婷  创建
			2012年11月30日 陈东岳修改添加多过滤器链和
									动态业务模式配置的支持
			2014年1月23日v1.01 陈东岳 修改 由于K3V3总线设计问题，
			无法在m3上进行低功耗恢复，移动到A9上进行。

*****************************************************************************/
BSP_S32 BSP_IPF_ConfigDownFilter(BSP_U32 u32Num, IPF_CONFIG_DLPARAM_S* pstDlPara)
{
    BSP_U32 u32BdqWptr = 0;
    BSP_U32 i;
    BSP_U32 u32BdInPtr = 0;
    BSP_U32 u32BD = 0;
    BSP_U16 u16TotalDataLen = 0;
    BSP_U32 u32TimeStampEn;

#ifdef CONFIG_BALONG_MODEM_RESET	
    /* 检查Ccore是否上电*/
    if(IPF_FORRESET_CONTROL_FORBID <= BSP_IPF_GetControlFLagForCcoreReset())
    {
        g_stIPFDebugInfo->ipf_ccore_not_init_count++;
        return BSP_ERR_IPF_CCORE_RESETTING;
    }
#endif

#if (defined(BSP_CONFIG_HI3630))
    /*由于K3V3总线设计问题，使用ipf 前需要检测是否需要恢复过滤器*/
    ipf_check_filter_restore();
#endif

    /*入参检查*/
    if((NULL == pstDlPara)||(0 == u32Num))
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r BSP_IPF_ConfigDownFilter input para ERROR! \n");
        return BSP_ERR_IPF_INVALID_PARA;
    }
	
#ifdef __BSP_IPF_DEBUG__
    if(u32Num > g_stIpfDl.u32IdleBd)
    {
        g_stIPFDebugInfo->u32DlBdNotEnough++;
	 IPF_PRINT("u32Num %u  <= g_stIpfDl.u32IdleBd %u \n",u32Num,g_stIpfDl.u32IdleBd);
        return IPF_ERROR;
    }
#endif
    for(i = 0; i < u32Num; i++)
    {
        if(0 == pstDlPara[i].u16Len)
        {
            bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r INPUT BD_Len =0 !Drop pkt \n");
            g_stIPFDebugInfo->ipf_dlbd_len_zero_count++;
            return BSP_ERR_IPF_INVALID_PARA;
        }
    }

	
    /* 读出BD写指针 */
    u32BdqWptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_BDQ_WPTR_OFFSET);

    u32BD = u32BdqWptr&IPF_DQ_PTR_MASK;
    u32TimeStampEn = g_stIPFDebugInfo->ipf_timestamp_dl_en;

    for(i = 0; i < u32Num; i++)
    {
        /* 配置CD */
        if(IPF_ConfigCD((void*)pstDlPara[i].u32Data, &u16TotalDataLen, &u32BdInPtr) != IPF_SUCCESS)
        {
            g_stIPFDebugInfo->u32DlCdNotEnough++;
            return BSP_ERR_IPF_CDQ_NOT_ENOUGH;
        }
        g_stIpfDl.pstIpfBDQ[u32BD].u16Attribute = pstDlPara[i].u16Attribute; 
        g_stIpfDl.pstIpfBDQ[u32BD].u32InPtr =  u32BdInPtr;
        g_stIpfDl.pstIpfBDQ[u32BD].u16PktLen = u16TotalDataLen;
        g_stIpfDl.pstIpfBDQ[u32BD].u16UsrField1 = pstDlPara[i].u16UsrField1;
        g_stIpfDl.pstIpfBDQ[u32BD].u32UsrField2 = pstDlPara[i].u32UsrField2;
        g_stIpfDl.pstIpfBDQ[u32BD].u32UsrField3 = pstDlPara[i].u32UsrField3;
        ipf_record_start_time_stamp(u32TimeStampEn, &g_stIpfDl.pstIpfBDQ[u32BD].u32UsrField3);
        u32BD = ((u32BD + 1) < IPF_DLBD_DESC_SIZE)? (u32BD + 1) : 0;
    }

#ifdef CONFIG_BALONG_MODEM_RESET	
    /* 检查Ccore是否上电*/
    if(IPF_FORRESET_CONTROL_FORBID <= BSP_IPF_GetControlFLagForCcoreReset())
    {
        g_stIPFDebugInfo->ipf_ccore_not_init_count++;
        return BSP_ERR_IPF_CCORE_RESETTING;
    }
#endif

    g_stIPFDebugInfo->ipf_cfg_dlbd_count += u32Num;

    /* 更新BD写指针 */
    ipf_writel(u32BD, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_BDQ_WPTR_OFFSET);  

    return IPF_SUCCESS;
}

/*****************************************************************************
* 函 数 名     : BSP_IPF_ConfigUlAd
*
* 功能描述  : 该接口仅在C核提供，用于移动ADQ写指针，
				给空闲的AD分配新的内存缓冲区，一次可以处理多个AD。
				数传前要调用这个函数分配缓冲区。
*                           
* 输入参数  : BSP_U32 u32Num0; 
				 BSP_U32 u32Num1; 
				 BSP_VOID* psk0; 
				 BSP_VOID* psk1
*
* 输出参数  : 无
* 返 回 值      : 无
* 修改记录  :2012年11月24日   陈东岳  创建
			2014年1月23日v1.01 陈东岳 修改 由于K3V3总线设计问题，
			无法在m3上进行低功耗恢复，移动到A9上进行。
*****************************************************************************/
BSP_S32 BSP_IPF_ConfigUlAd(BSP_U32 u32AdType, BSP_U32  u32AdNum, IPF_AD_DESC_S * pstAdDesc)
{
	BSP_U32 u32ADQwptr = 0;
	struct tagIPF_AD_DESC_S * pstADDesc = pstAdDesc;
	BSP_U32 i;

	/* 检查模块是否初始化 */
	if(IPF_MCORE_INIT_SUCCESS != g_IPFInit)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r IPF Not Init! \n");
		return BSP_ERR_IPF_NOT_INIT;
	}
	
#if (defined(BSP_CONFIG_HI3630))
	/*由于K3V3总线设计问题，使用ipf 前需要检测是否需要恢复过滤器*/
	ipf_check_filter_restore();
#endif


	if(NULL == pstAdDesc)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r BSP_IPF_ConfigUlAd input para ERROR! NULL == pstAdDesc\n");
		return BSP_ERR_IPF_INVALID_PARA;
	}	

	if(u32AdType >= IPF_AD_MAX)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r BSP_IPF_ConfigUlAd input para ERROR! u32AdType >= IPF_AD_MAX\n");
		return BSP_ERR_IPF_INVALID_PARA;
	}
	
	if(IPF_AD_0 == u32AdType)
	{
		if(u32AdNum >= IPF_ULAD0_DESC_SIZE)
		{
			bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r BSP_IPF_ConfigUlAd input para ERROR! u32AdNum >=IPF_ULAD0_DESC_SIZE\n");
			g_stIPFDebugInfo->ipf_ulad0_error_count++;
			return BSP_ERR_IPF_INVALID_PARA;
		}
		
		/*读出写指针*/
		u32ADQwptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_ADQ0_WPTR_OFFSET);
		for(i=0; i < u32AdNum; i++)
		{
			if(NULL == (BSP_VOID*)(pstADDesc->u32OutPtr1))
			{
				bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r BSP_IPF_ConfigDlAd input u32OutPtr1 NULL! \n");
				return BSP_ERR_IPF_INVALID_PARA;
			}
			g_stIpfUl.pstIpfADQ0[u32ADQwptr].u32OutPtr1 = pstADDesc->u32OutPtr1;
			g_stIpfUl.pstIpfADQ0[u32ADQwptr].u32OutPtr0 = pstADDesc->u32OutPtr0;
			u32ADQwptr = ((u32ADQwptr + 1) < IPF_ULAD0_DESC_SIZE)? (u32ADQwptr + 1) : 0;	
			pstADDesc++;
		}
		g_stIPFDebugInfo->ipf_cfg_ulad0_count += u32AdNum;

		/* 更新AD0写指针*/
		ipf_writel(u32ADQwptr, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_ADQ0_WPTR_OFFSET);  
	}
	
	else if(IPF_AD_1 == u32AdType)
	{
		if(u32AdNum >= IPF_ULAD1_DESC_SIZE)
		{
			bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r BSP_IPF_ConfigUlAd input para ERROR! u32AdNum >=IPF_ULAD1_DESC_SIZE\n");
			g_stIPFDebugInfo->ipf_ulad1_error_count++;
			return BSP_ERR_IPF_INVALID_PARA;
		}
		
		/*读出写指针*/
		u32ADQwptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_ADQ1_WPTR_OFFSET);
		for(i=0; i < u32AdNum; i++)
		{
			if(NULL == (BSP_VOID*)(pstADDesc->u32OutPtr1))
			{
				bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r BSP_IPF_ConfigDlAd input u32OutPtr1 NULL! \n");
				return BSP_ERR_IPF_INVALID_PARA;
			}
			g_stIpfUl.pstIpfADQ1[u32ADQwptr].u32OutPtr1 = pstADDesc->u32OutPtr1;
			g_stIpfUl.pstIpfADQ1[u32ADQwptr].u32OutPtr0 = pstADDesc->u32OutPtr0;
			u32ADQwptr = ((u32ADQwptr + 1) < IPF_ULAD1_DESC_SIZE)? (u32ADQwptr + 1) : 0;		
			pstADDesc++;
		}
		g_stIPFDebugInfo->ipf_cfg_ulad1_count += u32AdNum;

		/* 更新AD0写指针*/
		ipf_writel(u32ADQwptr, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_ADQ1_WPTR_OFFSET);  

	}
	return IPF_SUCCESS;
}

/*****************************************************************************
* 函 数 名     : BSP_IPF_ChannelEnable
*
* 功能描述  : 该接口用于使能或复位IP过滤器上下行通道
*
* 输入参数  : IPF_CHANNEL_TYPE_E eChanType      上下行通道标识
*                           BSP_BOOL bFlag   使能复位标识 
*
* 输出参数  : 无
* 返 回 值      : IPF_SUCCESS                使能复位成功
*                            IPF_ERROR                  使能复位失败
*                            BSP_ERR_IPF_INVALID_PARA   参数错误
*
* 修改记录  :2011年1月11日   鲁婷  创建
			2014年1月23日v1.01 陈东岳 修改 由于K3V3总线设计问题，
			无法在m3上进行低功耗恢复，移动到A9上进行。
*****************************************************************************/
BSP_S32 BSP_IPF_ChannelEnable(IPF_CHANNEL_TYPE_E eChanType, BSP_BOOL bFlag)
{
    BSP_U32 u32ChanEnable = 0;
    BSP_U32 u32ChanState = 0;
    BSP_U32 u32Times = 0;
    BSP_U32 u32ChCtrl = 0;

#if (defined(BSP_CONFIG_HI3630))
    /*由于K3V3总线设计问题，使用ipf 前需要检测是否需要恢复过滤器*/
    ipf_check_filter_restore();
#endif

    /* 参数检查 */
    if(eChanType >= IPF_CHANNEL_MAX)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,
                     "\r BSP_IPF_ChannelEnable eChanType = %d ! \n",eChanType,0,0,0,0,0);
        return BSP_ERR_IPF_INVALID_PARA;
    }

    /* 通道使能 */
    if(bFlag)
    {
        u32ChanEnable = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH_EN_OFFSET);
        u32ChanEnable |= 0x1<<eChanType;
        ipf_writel(u32ChanEnable, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH_EN_OFFSET);                        
    }
    /* 通道去使能 */
    else
    {
        u32ChanEnable = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH_EN_OFFSET);
        u32ChanEnable &= 0xFFFFFFFF ^ (0x1<<eChanType);
        ipf_writel(u32ChanEnable, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH_EN_OFFSET); 
        
        /* 遍历直到通道状态为0 */      
        do
        {
            (BSP_VOID)taskDelay(1);
            u32ChanState = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_EN_STATE_OFFSET);
            u32ChanState &= 0x1<<eChanType;
            u32Times++;
        }while((u32ChanState) && (u32Times < 100)); 
        
        if(100 == u32Times)
        {
            bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,
                       "\r BSP_IPF_ChannelEnable disable error u32Times = %d ! \n",u32Times,0,0,0,0,0);
            return IPF_ERROR;
        }
		
        /* 复位输入队列和输出队列的读写指针 */
        if(IPF_CHANNEL_UP == eChanType)
        {
            u32ChCtrl = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_CTRL_OFFSET);
            u32ChCtrl |= 0x30; /* 将4，5bit设置为1 */
            ipf_writel(u32ChCtrl, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_CTRL_OFFSET); 
        }
        else if(IPF_CHANNEL_DOWN == eChanType)
        {
            u32ChCtrl = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_CTRL_OFFSET);
            u32ChCtrl |= 0x30; /* 将4，5bit设置为1 */
            ipf_writel(u32ChCtrl, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_CTRL_OFFSET); 
        }
        else
        {
            return BSP_ERR_IPF_INVALID_PARA;
        }
    }
    return IPF_SUCCESS;
}

/*****************************************************************************
* 函 数 名     : BSP_IPF_GetDlBDNum
*
* 功能描述  : 该接口用于获取下行空闲BD和CD 数目
*                           BD 范围: 0~63      CD范围: 0~1023
* 输入参数  : 无
*
* 输出参数  : 空闲CD数目
* 返 回 值      : 空闲BD数目
*  
* 修改记录  :2011年11月30日   鲁婷  创建
			2014年1月23日v1.01 陈东岳 修改 由于K3V3总线设计问题，
			无法在m3上进行低功耗恢复，移动到A9上进行。

*****************************************************************************/
BSP_U32 BSP_IPF_GetDlBDNum(BSP_U32* pu32CDNum)
{
    BSP_U32 u32BdqDepth = 0;
    BSP_U32 u32IdleBd = 0;
    BSP_U32 u32IdleCd = 0;
    BSP_U32 u32IdleCdDown = 0;
    BSP_U32 u32IdleCdUp = 0;
	
#if (defined(BSP_CONFIG_HI3630))
    /*由于K3V3总线设计问题，使用ipf 前需要检测是否需要恢复过滤器*/
    ipf_check_filter_restore();
#endif

    /* 计算空闲BD数量 */
    u32BdqDepth = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_DQ_DEPTH_OFFSET);
    u32BdqDepth = u32BdqDepth & IPF_DQ_DEPTH_MASK;
    u32IdleBd = IPF_DLBD_DESC_SIZE - u32BdqDepth;
    /* 计算空闲CD 数量 */
    if(*(g_stIpfDl.u32IpfCdRptr) > g_stIpfDl.u32IpfCdWptr)
    {
        u32IdleCd = *(g_stIpfDl.u32IpfCdRptr) - g_stIpfDl.u32IpfCdWptr - 1;
    }
    else
    {
        u32IdleCdUp = (*(g_stIpfDl.u32IpfCdRptr) > 1)?(*(g_stIpfDl.u32IpfCdRptr) - 1):0;
        u32IdleCdDown = IPF_DLCD_DESC_SIZE -  g_stIpfDl.u32IpfCdWptr - 1;
        u32IdleCd = (u32IdleCdUp > u32IdleCdDown)? u32IdleCdUp:u32IdleCdDown;
    }
#if 0
    /*保留BDQ_RESERVE_NUM个BD用于减少AD队列空中断*/	
    u32IdleBd = (u32IdleBd > BDQ_RESERVE_NUM)? (u32IdleBd - BDQ_RESERVE_NUM):0;
#endif
    g_stIpfDl.u32IdleBd = u32IdleBd;
    *pu32CDNum = u32IdleCd;

    if(0 == u32IdleBd)
	{
		g_stIPFDebugInfo->u32DlBdNotEnough++;
	}
    return u32IdleBd;
}

/*****************************************************************************
* 函 数 名     : BSP_IPF_GetDlRdNum
*
* 功能描述  : 该接口用于读取下行RD数目
*
* 输入参数  : 无
*   
* 输出参数  : 无
*
* 返 回 值     : 下行RD数目
*
* 修改记录  :2013年8月1日   chendongyue  创建
*****************************************************************************/
BSP_U32 BSP_IPF_GetDlRdNum(BSP_VOID)
{
    BSP_U32 u32RdqDepth = 0;
  
    /* 读取RD深度 */
    u32RdqDepth = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_DQ_DEPTH_OFFSET);
    u32RdqDepth = (u32RdqDepth>>16)&IPF_DQ_DEPTH_MASK;
    return u32RdqDepth;
}


/*****************************************************************************
* 函 数 名     : BSP_IPF_GetDlDescNum
*
* 功能描述  : 该接口可读取下行可传输包数
			用于规避ipf硬件对头阻塞问题
*
* 输入参数  : 无
*   
* 输出参数  : 无
*
* 返 回 值     : 下行可传输包数
*
* 修改记录  :2013年8月1日   chendongyue  创建
			2014年1月23日v1.01 陈东岳 修改 由于K3V3总线设计问题，
			无法在m3上进行低功耗恢复，移动到A9上进行。

*****************************************************************************/

BSP_U32 BSP_IPF_GetDlDescNum(BSP_VOID)
{
	BSP_U32 u32DlAd0Num = 0;
	BSP_U32 u32DlAd1Num = 0;
	BSP_U32 u32DlBdNum = 0;
/*	BSP_U32 u32DlRdNum = 0;*/
	BSP_U32 u32DlAdwptr = 0;
	BSP_U32 u32DlAdrptr = 0;
	BSP_U32 u32DlBdDepth = 0;
	BSP_U32 u32DlCdNum = 0;
	
#if (defined(BSP_CONFIG_HI3630))
	/*由于K3V3总线设计问题，使用ipf 前需要检测是否需要恢复过滤器*/
	ipf_check_filter_restore();
#endif
				
	/* 计算空闲AD0数量 */
	u32DlBdDepth = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_DQ_DEPTH_OFFSET);
	u32DlAdwptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_ADQ0_WPTR_OFFSET);
	u32DlAdrptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_ADQ0_RPTR_OFFSET);
	u32DlBdDepth = u32DlBdDepth&IPF_DQ_DEPTH_MASK;
	
	if (u32DlAdwptr >= u32DlAdrptr)/*写指针在前，正常顺序*/
	{
		u32DlAd0Num = u32DlAdwptr - u32DlAdrptr;
	}
	else
	{
		u32DlAd0Num = IPF_ULAD0_DESC_SIZE - (u32DlAdrptr -u32DlAdwptr);
	}
	if(u32DlAd0Num > u32DlBdDepth)
	{
		u32DlAd0Num -= u32DlBdDepth;
	}
	else
	{
		u32DlAd0Num = 0;
		g_stIPFDebugInfo->u32DlAd0NotEnough++;
	}


	/* 计算空闲AD1数量 */
	u32DlBdDepth = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_DQ_DEPTH_OFFSET);
	u32DlAdwptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_ADQ1_WPTR_OFFSET);
	u32DlAdrptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_ADQ1_RPTR_OFFSET);
	u32DlBdDepth = u32DlBdDepth&IPF_DQ_DEPTH_MASK;

	if (u32DlAdwptr >= u32DlAdrptr)/*写指针在前，正常顺序*/
	{
		u32DlAd1Num = u32DlAdwptr - u32DlAdrptr;
	}
	else
	{
		u32DlAd1Num =  IPF_ULAD1_DESC_SIZE - (u32DlAdrptr -u32DlAdwptr);
	}
	
	if(u32DlAd1Num > u32DlBdDepth)
	{
		u32DlAd1Num -= u32DlBdDepth;
	}
	else
	{
		u32DlAd1Num = 0;
		g_stIPFDebugInfo->u32DlAd1NotEnough++;

	}

	u32DlBdNum = BSP_IPF_GetDlBDNum(&u32DlCdNum);
#if 0
	if(0 == u32DlBdNum)
	{
		g_stIPFDebugInfo->u32DlBdNotEnough++;
	}
#endif
	if(u32DlBdNum > u32DlAd0Num)
	{
		u32DlBdNum = u32DlAd0Num;
	}
	if(u32DlBdNum > u32DlAd1Num)
	{
		u32DlBdNum = u32DlAd1Num;
	}
	/*
	u32DlRdNum = IPF_ULRD_DESC_SIZE - BSP_IPF_GetDlRdNum();
	if(u32DlRdNum > 1)
	{
		u32DlRdNum -= 1;
	}
	else
	{
		u32DlRdNum = 0;
	}



	
	if(u32DlBdNum > u32DlRdNum)
	{
		u32DlBdNum = u32DlRdNum;
	}
	*/
       return u32DlBdNum;

}


/*****************************************************************************
* 函 数 名     : BSP_IPF_DlStateIdle
*
* 功能描述  : 该接口用于获取下行通道是否为空闲
*                            
* 输入参数  : 无
*
* 输出参数  : 无
* 返 回 值      : IPF_SUCCESS  表示下行空闲，
*                            IPF_ERROR      表示下行非空闲，
*  
* 修改记录  :2011年12月9日   鲁婷  创建
*****************************************************************************/
BSP_S32 BSP_IPF_DlStateIdle(BSP_VOID)
{
    BSP_U32 u32DlState = 0;
    BSP_U32 u32BdqWptr = 0;
    
    u32DlState = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_STATE_OFFSET);

    if(u32DlState != IPF_CHANNEL_STATE_IDLE)
    {
        return IPF_ERROR;
    }

    u32BdqWptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_BDQ_WPTR_OFFSET);/* [false alarm]:fortify disable */

    return IPF_SUCCESS;
}

/*****************************************************************************
* 函 数 名     : BSP_IPF_RegisterWakeupUlCb
*
* 功能描述  : 该接口用于注册上行PS任务回调函数
*                           
* 输入参数  : BSP_IPF_WakeupUlCb *pFnWakeupUl
*
* 输出参数  : 无
* 返 回 值      : 无
*  
* 修改记录  :2011年11月30日   鲁婷  创建
*****************************************************************************/
BSP_S32 BSP_IPF_RegisterWakeupUlCb(BSP_IPF_WakeupUlCb pFnWakeupUl)
{
    /* 参数检查 */
    if(NULL == pFnWakeupUl)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r BSP_IPF_RegisterWakeupUlCb inputPara ERROR! \n");
        return BSP_ERR_IPF_INVALID_PARA;
    }
    g_stIpfUl.pFnUlIntCb = pFnWakeupUl;
    return IPF_SUCCESS;
}

/*****************************************************************************
* 函 数 名     : BSP_IPF_RegisterAdqEmptyUlCb
*
* 功能描述  : 此接口只在C核提供，用于注册唤醒上行PS的
                             ADQ队列空回调函数
*                           
* 输入参数  : BSP_IPF_AdqEmptyCb pFnWakeupUl
*
* 输出参数  : 无
* 返 回 值      : IPF_SUCCESS 注册成功
*                            IPF_ERROR	注册失败
* 修改记录  :2012年11月24日   陈东岳  创建
*****************************************************************************/
BSP_S32 BSP_IPF_RegisterAdqEmptyUlCb(BSP_IPF_AdqEmptyUlCb pAdqEmptyUl)
{
    /* 参数检查 */
    if(NULL == pAdqEmptyUl)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r BSP_IPF_RegisterAdqEmptyUlCb inputPara ERROR! \n");
        return IPF_ERROR;
    }
    g_stIpfUl.pAdqEmptyUlCb = pAdqEmptyUl;
    return IPF_SUCCESS;
}


/*****************************************************************************
* 函 数 名     : BSP_IPF_GetUlRd
*
* 功能描述  : 该接口用于读取上行BD, 支持一次读取多个BD
*
* 输入参数  : BSP_U32* pu32Num    
*                           IPF_RD_DESC_S *pstRd
*   
* 输出参数  : BSP_U32* pu32Num    实际读取的RD数目
*
* 返 回 值     : IPF_SUCCESS               操作成功
*                           IPF_ERROR                   操作失败
*
* 修改记录  :2011年11月30日   鲁婷  创建
*****************************************************************************/
BSP_VOID BSP_IPF_GetUlRd(BSP_U32* pu32Num, IPF_RD_DESC_S *pstRd)
{
    BSP_U32 u32RdqRptr = 0;
    BSP_U32 u32RdqDepth = 0;
    BSP_U32 u32Num = 0;
    BSP_U32 i = 0;
    BSP_U32 u32TimeStampEn;
	
    /* 读取RD深度 */
    u32RdqDepth = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_DQ_DEPTH_OFFSET);
    u32RdqDepth = (u32RdqDepth>>16)&IPF_DQ_DEPTH_MASK;
    u32Num = (u32RdqDepth < *pu32Num)?u32RdqDepth:*pu32Num;

    if(0 == u32Num)
    {
        *pu32Num = 0;
        return;
    }

    u32TimeStampEn = g_stIPFDebugInfo->ipf_timestamp_ul_en;

    /* 读取RD读指针 */
    u32RdqRptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_RDQ_RPTR_OFFSET);
    for(i = 0; i < u32Num; i++)
    {        
        /* 获取RD */
        pstRd[i].u16Attribute = g_stIpfUl.pstIpfRDQ[u32RdqRptr].u16Attribute;
        pstRd[i].u16PktLen = g_stIpfUl.pstIpfRDQ[u32RdqRptr].u16PktLen;
        pstRd[i].u16Result = g_stIpfUl.pstIpfRDQ[u32RdqRptr].u16Result;
        pstRd[i].u32InPtr = g_stIpfUl.pstIpfRDQ[u32RdqRptr].u32InPtr;
        pstRd[i].u32OutPtr = g_stIpfUl.pstIpfRDQ[u32RdqRptr].u32OutPtr;
        pstRd[i].u16UsrField1 = g_stIpfUl.pstIpfRDQ[u32RdqRptr].u16UsrField1;
        pstRd[i].u32UsrField2 = g_stIpfUl.pstIpfRDQ[u32RdqRptr].u32UsrField2;
        pstRd[i].u32UsrField3 = g_stIpfUl.pstIpfRDQ[u32RdqRptr].u32UsrField3;
        ipf_record_end_time_stamp(u32TimeStampEn, g_stIpfUl.pstIpfRDQ[u32RdqRptr].u32UsrField3);
				
        /* 更新RD读指针 */
        u32RdqRptr = ((u32RdqRptr+1) < IPF_ULRD_DESC_SIZE)?(u32RdqRptr+1):0;        
    }
    ipf_writel(u32RdqRptr, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_RDQ_RPTR_OFFSET);
    #ifdef __BSP_IPF_DEBUG__
    if(g_stIPFDebugInfo->u32IpfDebug)
    {
        IPF_PRINT("out u32RdqRptr = %d\n", u32RdqRptr);
    }
    #endif
    *pu32Num = u32Num;
    g_stIPFDebugInfo->ipf_get_ulrd_count += u32Num;
}


/*****************************************************************************
* 函 数 名       : BSP_IPF_GetUlAdNum
*
* 功能描述  : 该接口只在C核提供，获取上行（C核）空闲AD数目
*
* 输入参数  :BSP_OK：正常返回
                            BSP_ERROR：出错
*             
* 输出参数  : 无
* 返 回 值     : 无
* 修改记录  : 2011年11月24日   陈东岳  创建
			2014年1月23日v1.01 陈东岳 修改 由于K3V3总线设计问题，
			无法在m3上进行低功耗恢复，移动到A9上进行。
*****************************************************************************/
BSP_S32 BSP_IPF_GetUlAdNum(BSP_U32* pu32AD0Num,BSP_U32* pu32AD1Num)
{
	BSP_U32 u32UlAdDepth = 0;
	BSP_U32 u32UlAdwptr = 0;
	BSP_U32 u32UlAdrptr = 0;

#if (defined(BSP_CONFIG_HI3630))
    /*由于K3V3总线设计问题，使用ipf 前需要检测是否需要恢复过滤器*/
    ipf_check_filter_restore();
#endif

	/*入参检测*/
	if((NULL == pu32AD0Num)||(NULL == pu32AD1Num))
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r BSP_IPF_GetDlAdNum pstCtrl NULL! \n");
		return BSP_ERR_IPF_INVALID_PARA;
	}

	/* 计算空闲AD数量 */
	u32UlAdwptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_ADQ0_WPTR_OFFSET);
	u32UlAdrptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_ADQ0_RPTR_OFFSET);
	if (u32UlAdwptr >= u32UlAdrptr)/*写指针在前，正常顺序*/
	{
		u32UlAdDepth = IPF_ULAD0_DESC_SIZE - (u32UlAdwptr - u32UlAdrptr);
	}
	else
	{
		u32UlAdDepth =  u32UlAdrptr -u32UlAdwptr;
	}
	*pu32AD0Num = u32UlAdDepth;

	/*扣除reserve ad，用于防止硬件将ad队列满识别成空和低功耗引发内存泄露*/	
	if(u32UlAdDepth > IPF_ADQ_RESERVE_NUM)
	{
		*pu32AD0Num = u32UlAdDepth - IPF_ADQ_RESERVE_NUM;
	}
	else
	{
		*pu32AD0Num = 0;
	}

	u32UlAdwptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_ADQ1_WPTR_OFFSET);
	u32UlAdrptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_ADQ1_RPTR_OFFSET);
	if (u32UlAdwptr >= u32UlAdrptr)/*写指针在前，正常顺序*/
	{
		u32UlAdDepth = IPF_ULAD1_DESC_SIZE - (u32UlAdwptr - u32UlAdrptr);
	}
	else
	{
		u32UlAdDepth =  u32UlAdrptr - u32UlAdwptr;
	}
	
	/*扣除reserve ad，用于防止硬件将ad队列满识别成空和低功耗引发内存泄露*/
	if(u32UlAdDepth > IPF_ADQ_RESERVE_NUM)
	{
		*pu32AD1Num = u32UlAdDepth - IPF_ADQ_RESERVE_NUM;
	}
	else
	{
		*pu32AD1Num = 0;
	}

	return IPF_SUCCESS;
}


/*****************************************************************************
* 函 数 名     : BSP_IPF_GetUlRdNum
*
* 功能描述  : 该接口用于读取上行RD数目
*
* 输入参数  : 无
*   
* 输出参数  : 无
*
* 返 回 值     : 上行RD数目
*
* 修改记录  :2012年2月16日   鲁婷  创建
*****************************************************************************/
BSP_U32 BSP_IPF_GetUlRdNum(BSP_VOID)
{
    BSP_U32 u32RdqDepth = 0;
  
    /* 读取RD深度 */
    u32RdqDepth = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_DQ_DEPTH_OFFSET);
    u32RdqDepth = (u32RdqDepth>>16)&IPF_DQ_DEPTH_MASK;
    return u32RdqDepth;
}

/*****************************************************************************
* 函 数 名       : BSP_IPF_SwitchInt
*
* 功能描述  : 提供的低功耗接口， 将上报给A核的中断同时上报给C 核
*
* 输入参数  : BSP_TRUE   上报给C核
*                           BSP_FALSE  不上报给C核
*             
* 输出参数  : 无
* 返 回 值     : 无
* 修改记录  : 2011年2月14日   鲁婷  创建
*****************************************************************************/
BSP_VOID BSP_IPF_SwitchInt(BSP_BOOL bFlag)
{
	BSP_U32 u32IntMask0 = 0;

	if(bFlag == BSP_TRUE)
	{
    	u32IntMask0 = IPF_INT_OPEN0 | IPF_INT_OPEN1;
		g_u32IpfSwitchInt = 1;
	}
	else
	{
		u32IntMask0 = IPF_INT_OPEN0;
		g_u32IpfSwitchInt = 0;
	}
    ipf_writel(u32IntMask0, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_INT_MASK0_OFFSET);
}

/*****************************************************************************
* 函 数 名  : BSP_IPF_UseFilterInfo
*
* 功能描述  : 提供的debug接口，获取已经使用的filter号
*
* 输入参数  : IPF_CHANNEL_TYPE_E eChnType  通道类型
*             
* 输出参数  : 无
* 返 回 值  : 无
*
* 修改记录  : 2011年1月11日   鲁婷  创建
                             2011年3月30日   鲁婷  修改
*****************************************************************************/
BSP_VOID BSP_IPF_UseFilterInfo(IPF_FILTER_CHAIN_TYPE_E eFilterChainhead)
{
    IPF_ID_S* current = NULL;
    BSP_U32 u32FilterNum = 0;
/*    BSP_S32 s32GetChar = 0;*/
    
    u32FilterNum = g_stIPFFilterInfo[eFilterChainhead].u32FilterNum;
    current = g_stIPFFilterInfo[eFilterChainhead].pstUseList;
	
    IPF_PRINT("Total Filter Num = %u\n",u32FilterNum);            
    IPF_PRINT("=============================\n");

    while(u32FilterNum)
    { 
        while(u32FilterNum)
        {
            if(1 == u32FilterNum)
            {
                IPF_PRINT("Filter ID = %d,    PS ID = %d\n    ,Next HWID = %d\n",current->u32FilterID, current->u32PsID, current->pstNext->u32FilterID);            
                u32FilterNum--;
            }
            else
            {
                IPF_PRINT("Filter ID = %d,    PS ID = %d\n    ,Next HWID = %d\n",current->u32FilterID, current->u32PsID, current->pstNext->u32FilterID);            
                current = current->pstNext;
                u32FilterNum--;
            }
        }
    }
    IPF_PRINT("*****************************\n");

}

/*****************************************************************************
* 函 数 名  : BSP_IPF_FreeFilterInfo
*
* 功能描述  : 提供的debug接口，获取未使用的filter号
*
* 输入参数  : IPF_CHANNEL_TYPE_E eChnType  通道类型
*             
* 输出参数  : 无
* 返 回 值  : 无
*
* 修改记录  : 2011年1月11日   鲁婷  创建
*
*****************************************************************************/
BSP_VOID BSP_IPF_BasicFreeFilterInfo(BSP_VOID)
{
    IPF_ID_S* current = NULL;
/*    BSP_S32 s32GetChar = 0;*/

    current = g_stIPFBasicFreeList;

    IPF_PRINT("============================\n");

    while(current != NULL)
    {
        while(current != NULL)
        {
            IPF_PRINT("Filter ID:      %d\n",current->u32FilterID);
            current = current->pstNext; 
        }
    }
    IPF_PRINT("*****************************\n");    
}
BSP_VOID BSP_IPF_ExtFreeFilterInfo(BSP_VOID)
{
    IPF_ID_S* current = NULL;
/*    BSP_S32 s32GetChar = 0;*/

    current = g_stIPFExtFreeList;

    IPF_PRINT("============================\n");

    while(current != NULL)
    {
        while(current != NULL)
        {
            IPF_PRINT("Filter ID:      %d\n",current->u32FilterID);
            current = current->pstNext; 
        }
    }
    IPF_PRINT("*****************************\n");    
}

BSP_VOID BSP_IPF_FilterInfoHWID(BSP_U32 u32Num)
{
    IPF_MATCH_INFO_S stMatchInfo; 
    
    if(u32Num < IPF_BF_NUM)
    {
         /* 写过滤表操作地址 */    
        ipf_writel(u32Num, HI_IPF_REGBASE_ADDR_VIRT + HI_BFLT_INDEX_OFFSET); 
         
        /* 读出过滤器的配置 */
        memcpy(&stMatchInfo, (BSP_VOID*)(HI_IPF_REGBASE_ADDR_VIRT+HI_FLT_LOCAL_ADDR0_OFFSET), sizeof(IPF_MATCH_INFO_S));/* [false alarm]:fortify disable */
    }
    else
    {
        memcpy(&stMatchInfo, (BSP_VOID*)(g_pstExFilterAddr + u32Num - IPF_BF_NUM), sizeof(IPF_MATCH_INFO_S));
    }

    IPF_PRINT("============================\n");
    IPF_PRINT("src ADDR0 : %x.%x.%x.%x\n", stMatchInfo.u8SrcAddr[0], stMatchInfo.u8SrcAddr[1], stMatchInfo.u8SrcAddr[2], stMatchInfo.u8SrcAddr[3]);
    IPF_PRINT("src ADDR1 : %x.%x.%x.%x\n", stMatchInfo.u8SrcAddr[4], stMatchInfo.u8SrcAddr[5], stMatchInfo.u8SrcAddr[6], stMatchInfo.u8SrcAddr[7]);
    IPF_PRINT("src ADDR2 : %x.%x.%x.%x\n", stMatchInfo.u8SrcAddr[8], stMatchInfo.u8SrcAddr[9], stMatchInfo.u8SrcAddr[10], stMatchInfo.u8SrcAddr[11]);
    IPF_PRINT("src ADDR3 : %x.%x.%x.%x\n", stMatchInfo.u8SrcAddr[12], stMatchInfo.u8SrcAddr[13], stMatchInfo.u8SrcAddr[14], stMatchInfo.u8SrcAddr[15]);
    IPF_PRINT("dst ADDR0 : %x.%x.%x.%x\n", stMatchInfo.u8DstAddr[0], stMatchInfo.u8DstAddr[1], stMatchInfo.u8DstAddr[2], stMatchInfo.u8DstAddr[3]);
    IPF_PRINT("dst ADDR1 : %x.%x.%x.%x\n", stMatchInfo.u8DstAddr[4], stMatchInfo.u8DstAddr[5], stMatchInfo.u8DstAddr[6], stMatchInfo.u8DstAddr[7]);
    IPF_PRINT("dst ADDR2 : %x.%x.%x.%x\n", stMatchInfo.u8DstAddr[8], stMatchInfo.u8DstAddr[9], stMatchInfo.u8DstAddr[10], stMatchInfo.u8DstAddr[11]);
    IPF_PRINT("dst ADDR3 : %x.%x.%x.%x\n", stMatchInfo.u8DstAddr[12], stMatchInfo.u8DstAddr[13], stMatchInfo.u8DstAddr[14], stMatchInfo.u8DstAddr[15]);
    IPF_PRINT("SrcPortLo : %d  SrcPortHi: %d\n", stMatchInfo.unSrcPort.Bits.u16SrcPortLo, stMatchInfo.unSrcPort.Bits.u16SrcPortHi);
    IPF_PRINT("DstPortLo : %d  DstPortHi: %d\n", stMatchInfo.unDstPort.Bits.u16DstPortLo, stMatchInfo.unDstPort.Bits.u16DstPortHi);
    IPF_PRINT("TrafficClass :      %d\n", stMatchInfo.unTrafficClass.u32TrafficClass);
    IPF_PRINT("TrafficClassMsk :  %d\n", stMatchInfo.u32LocalAddressMsk);
    IPF_PRINT("Protocol :          %d\n", stMatchInfo.unNextHeader.u32Protocol);
    IPF_PRINT("FlowLable :         %d\n", stMatchInfo.u32FlowLable);
    IPF_PRINT("Type :        %d  Code: %d\n", stMatchInfo.unFltCodeType.Bits.u16Type, stMatchInfo.unFltCodeType.Bits.u16Code);
    IPF_PRINT("NextIndex : %d  FltPri: %d\n", stMatchInfo.unFltChain.Bits.u16NextIndex, stMatchInfo.unFltChain.Bits.u16FltPri);
    IPF_PRINT("FltSpi :             %d\n", stMatchInfo.u32FltSpi);
    IPF_PRINT("FltRuleCtrl :        %x\n", stMatchInfo.unFltRuleCtrl.u32FltRuleCtrl);    
    IPF_PRINT("============================\n");
}

BSP_VOID BSP_IPF_FilterInfoPSID(IPF_FILTER_CHAIN_TYPE_E eFilterChainhead, BSP_U32 u32Num)
{
    BSP_U32 u32FindID = 0;
    
    /* 查找PS ID 对应的FilterID*/
    if(IPF_FindFilterID(eFilterChainhead, u32Num, &u32FindID) != IPF_SUCCESS)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF, "\r cannot find filter ID! \n");
        return ;
    }
    IPF_PRINT("HWID is %d",u32FindID);
    BSP_IPF_FilterInfoHWID(u32FindID);
}

/*****************************************************************************
* 函 数 名     : BSP_IPF_BackupFilter
*
* 功能描述  : 备份基本过滤器到DDR
*
* 输入参数  : 过滤器ID
*   
* 输出参数  : 无
*
* 返 回 值     : 无
*
* 修改记录  :2013年5月16日   陈东岳  创建
*****************************************************************************/
BSP_VOID BSP_IPF_BackupFilter(BSP_U32 u32FilterID)
{
#if 0
    IPF_MATCH_INFO_S * pstFilterMatchInfo = (IPF_MATCH_INFO_S *)IPF_PWRCTL_BASIC_FILTER_ADDR;
    pstFilterMatchInfo += u32FilterID;
    memcpy((BSP_VOID*)pstFilterMatchInfo, (BSP_VOID*)(HI_IPF_REGBASE_ADDR_VIRT+HI_FLT_LOCAL_ADDR0_OFFSET), sizeof(IPF_MATCH_INFO_S));
#else
    BSP_U32 j;	
    BSP_U32* match_info_addr = (u32 *)(IPF_PWRCTL_BASIC_FILTER_ADDR + u32FilterID*sizeof(IPF_MATCH_INFO_S));

    for(j=0; j<(sizeof(IPF_MATCH_INFO_S)/4); j++)
    {
        *match_info_addr = ipf_readl((HI_IPF_REGBASE_ADDR_VIRT+HI_FLT_LOCAL_ADDR0_OFFSET+j*4)); 
        match_info_addr++;

    }
#endif
}

#ifdef CONFIG_CCORE_PM

/*****************************************************************************
* 函 数 名     : ipf_ul_dpm_prepare
*
* 功能描述  : dpm进入准备函数
*
* 输入参数  : 设备指针
*   
* 输出参数  : 无
*
* 返 回 值     : IPF_ERROR 失败
                            IPF_SUCCESS 成功
*
* 修改记录  :2013年6月16日   陈东岳  创建
*****************************************************************************/
static int ipf_ul_dpm_prepare(struct dpm_device *dev)
{
    u32 u32_ul_state;
#if 0
    u32 u32_adq0_wptr;
    u32 u32_adq0_rptr;
    u32 u32_adq1_wptr;
    u32 u32_adq1_rptr;
#endif
    /* 判断上行IPF是否空闲 */
    u32_ul_state = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_STATE_OFFSET);

    if(u32_ul_state != IPF_CHANNEL_STATE_IDLE)
    {
        /*bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r IPF CHANNEL NOT IDLE! \n");*/
        return IPF_ERROR;
    }
    #if 0
    /*等待ad空中断处理完毕*/
    u32_adq0_wptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_ADQ0_WPTR_OFFSET);
    u32_adq0_rptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_ADQ0_RPTR_OFFSET);
    u32_adq1_wptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_ADQ1_WPTR_OFFSET);
    u32_adq1_rptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_ADQ1_RPTR_OFFSET);
    if((u32_adq0_rptr == u32_adq0_wptr)||(u32_adq1_rptr == u32_adq1_wptr))
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r IPF UL AD EMPTY INTRRUPT PROCESSING! \n");
        return IPF_ERROR;
    }
    #endif

    return IPF_SUCCESS;
}

/*****************************************************************************
* 函 数 名     : ipf_ul_suspend
*
* 功能描述  : dpm桩函数
*
* 输入参数  : 设备指针
*   
* 输出参数  : 无
*
* 返 回 值     : 成功
*
* 修改记录  :2013年6月16日   陈东岳  创建
			2014年1月23日v1.01 陈东岳 修改 由于K3V3总线设计问题，
			无法在m3上进行低功耗恢复，移动到A9上进行。
*****************************************************************************/
static int ipf_ul_suspend(struct dpm_device *dev)
{
#if (defined(BSP_CONFIG_HI3630))

    unsigned long ipf_flags = 0;
	
    spin_lock_irqsave(&ipf_filter_spinlock, ipf_flags);
    bsp_ipc_spin_lock(IPC_SEM_IPF_PWCTRL);
    *ipf_ccore_init_status = IPF_PWC_DOWN;
    cache_sync();		
    bsp_ipc_spin_unlock(IPC_SEM_IPF_PWCTRL);
    spin_unlock_irqrestore(&ipf_filter_spinlock, ipf_flags);

#endif
    g_stIPFDebugInfo->ipf_ccore_suspend_count++;
	return IPF_SUCCESS;
}

/*****************************************************************************
* 函 数 名     : ipf_ul_resume
*
* 功能描述  : dpm桩函数
*
* 输入参数  : 设备指针
*   
* 输出参数  : 无
*
* 返 回 值     : 成功
*
* 修改记录  :2013年6月16日   陈东岳  创建
*****************************************************************************/
static int ipf_ul_resume(struct dpm_device *dev)
{
    g_stIPFDebugInfo->ipf_ccore_resume_count++;
	return IPF_SUCCESS;
}

/*****************************************************************************
* 函 数 名     : ipf_ul_dpm_complete
*
* 功能描述  : dpm桩函数
*
* 输入参数  : 设备指针
*   
* 输出参数  : 无
*
* 返 回 值     : 成功
*
* 修改记录  :2013年6月16日   陈东岳  创建
*****************************************************************************/
static int ipf_ul_dpm_complete(struct dpm_device *dev)
{
	return IPF_SUCCESS;
}
#endif
#endif

#ifdef __KERNEL__


/*****************************************************************************
* 函 数 名     : BSP_IPF_DlRegReInit
*
* 功能描述  : Ccore复位时,IPF会随之复位,其寄存器信息会全部消失。
				  该函数在ccore复位、解复位并启动成功后,
				  在ADS回调处理中首先调用,用于重新配置IPF下行通道相关寄存器

*
* 输入参数  : 无
*   
* 输出参数  : 无
*
* 返 回 值     : 无
*
* 修改记录  :2013年9月1日   陈东岳创建
*****************************************************************************/
BSP_VOID BSP_IPF_DlRegReInit(BSP_VOID)
{
	/*配置IPF下行通道寄存器*/
    BSP_U32 u32BDSize[IPF_CHANNEL_MAX] = {IPF_ULBD_DESC_SIZE, IPF_DLBD_DESC_SIZE};
    BSP_U32 u32RDSize[IPF_CHANNEL_MAX] = {IPF_ULRD_DESC_SIZE, IPF_DLRD_DESC_SIZE};
    BSP_U32 u32ADCtrl[IPF_CHANNEL_MAX] = {IPF_ADQ_DEFAULT_SEETING,IPF_ADQ_DEFAULT_SEETING};
    memset((BSP_VOID*)IPF_DLBD_MEM_ADDR, 0x0, IPF_DLDESC_SIZE);
	
    g_stIpfUl.pstIpfBDQ = (IPF_BD_DESC_S*)IPF_ULBD_MEM_ADDR;
    g_stIpfUl.pstIpfRDQ = (IPF_RD_DESC_S*)IPF_ULRD_MEM_ADDR;
    g_stIpfUl.pstIpfADQ0 = (IPF_AD_DESC_S*)IPF_ULAD0_MEM_ADDR;
    g_stIpfUl.pstIpfADQ1 = (IPF_AD_DESC_S*)IPF_ULAD1_MEM_ADDR;
    g_stIpfUl.pu32IdleBd = (BSP_U32*)IPF_ULBD_IDLENUM_ADDR;

    g_stIpfDl.pstIpfBDQ = (IPF_BD_DESC_S*)IPF_DLBD_MEM_ADDR;
    g_stIpfDl.pstIpfRDQ = (IPF_RD_DESC_S*)IPF_DLRD_MEM_ADDR;
    g_stIpfDl.pstIpfADQ0 = (IPF_AD_DESC_S*)IPF_DLAD0_MEM_ADDR;
    g_stIpfDl.pstIpfADQ1 = (IPF_AD_DESC_S*)IPF_DLAD1_MEM_ADDR;
    g_stIpfDl.pstIpfCDQ = (IPF_CD_DESC_S*)IPF_DLCD_MEM_ADDR;

    g_stIpfDl.pstIpfPhyBDQ = (IPF_BD_DESC_S*)(IPF_DLBD_MEM_ADDR - DDR_SHARED_MEM_VIRT_ADDR + DDR_SHARED_MEM_ADDR);
    g_stIpfDl.pstIpfPhyRDQ = (IPF_RD_DESC_S*)(IPF_DLRD_MEM_ADDR - DDR_SHARED_MEM_VIRT_ADDR + DDR_SHARED_MEM_ADDR);
    g_stIpfDl.pstIpfPhyADQ0 = (IPF_AD_DESC_S*)(IPF_DLAD0_MEM_ADDR - DDR_SHARED_MEM_VIRT_ADDR + DDR_SHARED_MEM_ADDR);
    g_stIpfDl.pstIpfPhyADQ1 = (IPF_AD_DESC_S*)(IPF_DLAD1_MEM_ADDR - DDR_SHARED_MEM_VIRT_ADDR + DDR_SHARED_MEM_ADDR);
	
    g_stIpfDl.u32IpfCdRptr = (BSP_U32*) IPF_DLCDRPTR_MEM_ADDR;
    *(g_stIpfDl.u32IpfCdRptr) = 0;

    g_stIPFDebugInfo = (IPF_DEBUG_INFO_S*)IPF_DEBUG_INFO_ADDR;
	
    /* 配置下行通道的AD、BD和RD深度 */
    ipf_writel(u32BDSize[IPF_CHANNEL_DOWN]-1, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_BDQ_SIZE_OFFSET);
    ipf_writel(u32RDSize[IPF_CHANNEL_DOWN]-1, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_RDQ_SIZE_OFFSET);
    ipf_writel(u32ADCtrl[IPF_CHANNEL_DOWN], HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_ADQ_CTRL_OFFSET);

    /*下行通道的BD和RD起始地址*/    
    ipf_writel((BSP_U32)g_stIpfDl.pstIpfPhyBDQ, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_BDQ_BADDR_OFFSET);
    ipf_writel((BSP_U32)g_stIpfDl.pstIpfPhyRDQ, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_RDQ_BADDR_OFFSET);
    ipf_writel((BSP_U32)g_stIpfDl.pstIpfPhyADQ0, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_ADQ0_BASE_OFFSET);
    ipf_writel((BSP_U32)g_stIpfDl.pstIpfPhyADQ1, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_ADQ1_BASE_OFFSET);

    g_IPFInit = IPF_ACORE_INIT_SUCCESS;

    bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"[IPF]  ipf dl register reinit success\n");

    return;

}

/*****************************************************************************
* 函 数 名     : BSP_IPF_GetUsedDlAd
*
* 功能描述  : 功能同V9R1的同名函数，用于获取配置给AD队列的，
				  且尚未被硬件使用的AD信息，调用者释放获取的AD
				  中对应的skb(IMM_Zc)，以防止Ccore reset时内存泄露
				  调用该函数期间，应禁止配置BD。
				  该函数运行时会关闭通道

* 输入参数  : eAdType: AD队列类型
*   
* 输出参数  : pu32AdNum: 需要释放的AD数目
				  pstAdDesc: 需要释放的AD数组头指针
*
* 返 回 值     : BSP_ERR_IPF_INVALID_PARA 入参非法
				  IPF_ERROR 失败
*				  IPF_SUCCESS 成功
* 修改记录  :2013年9月1日   陈东岳创建
*****************************************************************************/
BSP_S32 BSP_IPF_GetUsedDlAd(IPF_AD_TYPE_E eAdType, BSP_U32 * pu32AdNum, IPF_AD_DESC_S * pstAdDesc)
{
	BSP_U32 u32Timeout = 10;
	BSP_U32 u32DlStateValue;
	BSP_U32 u32ChanEnable;
	BSP_U32 u32AdStateValue;
	BSP_U32 u32FreeAdNum = 0;
	BSP_U32 u32ADQwptr;
	BSP_U32 u32ADQrptr;
	BSP_U32 u32ADCtrl;
#ifndef CONFIG_IPF_AD_RPRT_FIX		
	BSP_U32 u32RptrOffsetValue = 0;
#endif
	/*关闭下行AD配置接口*/
	g_IPFInit = 0;

	/*入参检测*/
	if((NULL == pu32AdNum)||(NULL == pstAdDesc))
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r BSP_IPF_ConfigDlAd input para ERROR!NULL == pu32AdNum or NULL == pstAdDesc\n");
		return BSP_ERR_IPF_INVALID_PARA;
	}	
		
	/*等待通道idle ,200ms超时*/
	do
	{
		msleep(20);
		u32DlStateValue = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_STATE_OFFSET);

		if(u32DlStateValue == IPF_CHANNEL_STATE_IDLE)
		{
			break;
		}
		
	}while(--u32Timeout);
	if (!u32Timeout) 
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r After 20ms IPF dl channel still on, unable to free AD \n");
		return IPF_ERROR;
	}
	/*尝试关闭下行通道*/
	u32ChanEnable = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH_EN_OFFSET);
	u32ChanEnable &= 0xFFFFFFFF ^ (0x1<<IPF_CHANNEL_DOWN);
	ipf_writel(u32ChanEnable, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH_EN_OFFSET); 

	u32ADCtrl = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_ADQ_CTRL_OFFSET);
	u32ADCtrl &= IPF_ADQ_EN_MASK;
	u32ADCtrl |= (IPF_NO_ADQ);

	/*关闭AD，用于防止产生ADQ预取*/
	ipf_writel(u32ADCtrl, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_ADQ_CTRL_OFFSET);
	if(IPF_AD_0 == eAdType)
	{
		u32AdStateValue = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_ADQ0_STAT_OFFSET);
		/*回退AD读指针*/
		u32ADQwptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_ADQ0_WPTR_OFFSET);
		u32ADQrptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_ADQ0_RPTR_OFFSET);
#ifndef CONFIG_IPF_AD_RPRT_FIX		
		if(u32AdStateValue & IPF_ADQ_BUF_EPT_MASK)
		{
			u32RptrOffsetValue = 0; 
		}
		else if(u32AdStateValue & IPF_ADQ_BUF_FULL_MASK)
		{
			u32RptrOffsetValue = 2; 
		}
		else
		{
			u32RptrOffsetValue = 1;    
		}
		if(u32ADQrptr >= u32RptrOffsetValue)
		{
			u32ADQrptr = u32ADQrptr - u32RptrOffsetValue;
		}
		else
		{
			u32ADQrptr = IPF_DLAD0_DESC_SIZE + u32ADQrptr - u32RptrOffsetValue;
		}
#endif
		while(u32ADQrptr != u32ADQwptr)
		{
			pstAdDesc->u32OutPtr1 = g_stIpfDl.pstIpfADQ0[u32ADQrptr].u32OutPtr1;
			pstAdDesc->u32OutPtr0 = g_stIpfDl.pstIpfADQ0[u32ADQrptr].u32OutPtr0;
//			printk("AD0[%u]OutPtr1 = 0x%x \n", u32ADQrptr, pstAdDesc->u32OutPtr1);
			u32ADQrptr = ((u32ADQrptr + 1) < IPF_DLAD0_DESC_SIZE)? (u32ADQrptr + 1) : 0;	
			pstAdDesc++;
			u32FreeAdNum++;
		}
	}
	else if(IPF_AD_1 == eAdType)
	{
		u32AdStateValue = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_ADQ1_STAT_OFFSET);
		/*回退AD读指针*/
		u32ADQwptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_ADQ1_WPTR_OFFSET);
		u32ADQrptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_ADQ1_RPTR_OFFSET);
#ifndef CONFIG_IPF_AD_RPRT_FIX		
		if(u32AdStateValue & IPF_ADQ_BUF_EPT_MASK)
		{
			u32RptrOffsetValue = 0; 
		}
		else if(u32AdStateValue & IPF_ADQ_BUF_FULL_MASK)
		{
			u32RptrOffsetValue = 2; 
		}
		else
		{
			u32RptrOffsetValue = 1;    
		}
		if(u32ADQrptr >= u32RptrOffsetValue)
		{
			u32ADQrptr = u32ADQrptr - u32RptrOffsetValue;
		}
		else
		{
			u32ADQrptr = IPF_DLAD1_DESC_SIZE + u32ADQrptr - u32RptrOffsetValue;
		}
#endif

		while(u32ADQrptr != u32ADQwptr)
		{
			pstAdDesc->u32OutPtr1 = g_stIpfDl.pstIpfADQ1[u32ADQrptr].u32OutPtr1;
			pstAdDesc->u32OutPtr0 = g_stIpfDl.pstIpfADQ1[u32ADQrptr].u32OutPtr0;
//			printk("AD1[%u]OutPtr1 = 0x%x", u32ADQrptr, pstAdDesc->u32OutPtr1);
			u32ADQrptr = ((u32ADQrptr + 1) < IPF_DLAD1_DESC_SIZE)? (u32ADQrptr + 1) : 0;	
			pstAdDesc++;
			u32FreeAdNum++;
		}
	}
	else
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r BSP_IPF_ConfigDlAd input para ERROR! u32AdType >= IPF_AD_MAX\n");
		return BSP_ERR_IPF_INVALID_PARA;
	}
	/*返回AD*/
	*pu32AdNum = u32FreeAdNum;
	return IPF_SUCCESS;
	
}



/*****************************************************************************
* 函 数 名     : BSP_IPF_SetControlFLagForCcoreReset
*
* 功能描述  : modem单独复位ipf适配函数，用于在复位时阻止下行数传
*
* 输入参数  : 无
*   
* 输出参数  : 无
*
* 返 回 值     : 成功
*
* 修改记录  :	2013年4月19日   卢彦胜创建
					2013年6月16日   陈东岳适配到V7R2
*****************************************************************************/
BSP_VOID BSP_IPF_SetControlFLagForCcoreReset(IPF_FORREST_CONTROL_E eResetFlag)
{
    if(eResetFlag >= IPF_FORRESET_CONTROL_MAX)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r BSP_IPF_SetControlFLagForCcoreReset eResetFlag overflow! \n");
        return;
    }
    /*设置标志，终止上行数传*/
    g_eCcoreResetFlag = eResetFlag;
#if (defined(BSP_CONFIG_HI3630))
#ifdef CONFIG_BALONG_MODEM_RESET	
    /*设置标志，终止下行数传*/
    *modem_reset_flag = eResetFlag;
#endif
#endif
    cache_sync();			
}

/*****************************************************************************
* 函 数 名     : BSP_IPF_GetControlFLagForCcoreReset
*
* 功能描述  : modem单独复位ipf适配函数，用于在复位时阻止下行数传
*
* 输入参数  : 无
*   
* 输出参数  : 无
*
* 返 回 值     : 成功
*
* 修改记录  :2013年6月16日   陈东岳  创建
*****************************************************************************/
static IPF_FORREST_CONTROL_E BSP_IPF_GetControlFLagForCcoreReset(BSP_VOID)
{
    return g_eCcoreResetFlag;
}

#ifdef CONFIG_BALONG_MODEM_RESET	
int bsp_ipf_reset_ccore_cb(DRV_RESET_CALLCBFUN_MOMENT eparam, int userdata)
{
	BSP_U32 u32Timeout = 2;
	BSP_U32 u32DlStateValue;
	if(DRV_RESET_CALLCBFUN_RESET_BEFORE == eparam)
	{
		BSP_IPF_SetControlFLagForCcoreReset(IPF_FORRESET_CONTROL_FORBID);

		do
		{
			msleep(150);
			u32DlStateValue = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_STATE_OFFSET);

			if(u32DlStateValue == IPF_CHANNEL_STATE_IDLE)
			{
				break;
			}
			
		}while(--u32Timeout);
		if (!u32Timeout) 
		{
			bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,
				"\r IPF dl channel on after bsp_ipf_reset_ccore_cb called \n");
		}
	}
	else if(DRV_RESET_CALLCBFUN_RESET_AFTER == eparam)
	{
		BSP_IPF_SetControlFLagForCcoreReset(IPF_FORRESET_CONTROL_ALLOW);
		return IPF_SUCCESS;
	}
	else
	{
		/*under the requeset of yaoguocai*/
		return IPF_SUCCESS;
	}
	return IPF_SUCCESS;
}
#endif

/******************************************************************************
* 函 数 名     : BSP_IPF_GetUlBDNum
*
* 功能描述  : 该接口用于获取上行空闲BD 数目
*                            范围: 0~64
* 输入参数  : 无
*
* 输出参数  : 无
* 返 回 值      : 空闲BD数目
*  
* 修改记录  :2011年11月30日   鲁婷  创建
*****************************************************************************/
BSP_U32 BSP_IPF_GetUlBDNum(BSP_VOID)
{
    BSP_U32 u32UlBdDepth = 0;
    BSP_U32 u32IdleBd = 0;

    /* 计算空闲BD数量 */
    u32UlBdDepth = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_DQ_DEPTH_OFFSET);

    u32IdleBd = IPF_ULBD_DESC_SIZE - (u32UlBdDepth & IPF_DQ_DEPTH_MASK);
#if 0
    /*保留BDQ_RESERVE_NUM个BD用于减少AD队列空中断*/
    u32IdleBd = (u32IdleBd > BDQ_RESERVE_NUM)? (u32IdleBd - BDQ_RESERVE_NUM):0;
#endif
    *(g_stIpfUl.pu32IdleBd) = u32IdleBd;
	
    if(0 == u32IdleBd)
	{
		g_stIPFDebugInfo->u32UlBdNotEnough++;
	}
    return u32IdleBd;
}

/*****************************************************************************
* 函 数 名     : BSP_IPF_GetUlRdNum
*
* 功能描述  : 该接口用于读取上行RD数目
*
* 输入参数  : 无
*   
* 输出参数  : 无
*
* 返 回 值     : 上行RD数目
*
* 修改记录  :2013年8月1日   chendongyue  创建
*****************************************************************************/
BSP_U32 BSP_IPF_GetUlRdNum(BSP_VOID)
{
    BSP_U32 u32RdqDepth = 0;
  
    /* 读取RD深度 */
    u32RdqDepth = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_DQ_DEPTH_OFFSET);
    u32RdqDepth = (u32RdqDepth>>16)&IPF_DQ_DEPTH_MASK;
    return u32RdqDepth;
}


/*****************************************************************************
* 函 数 名     : BSP_IPF_GetUlDescNum
*
* 功能描述  : 该接口可读取上行可传输包数
			用于规避ipf硬件对头阻塞问题
*
* 输入参数  : 无
*   
* 输出参数  : 无
*
* 返 回 值     : 上行可发送包数
*
* 修改记录  :2013年8月1日   chendongyue  创建
*****************************************************************************/

BSP_U32 BSP_IPF_GetUlDescNum(BSP_VOID)
{
	BSP_U32 u32UlAd0Num = 0;
	BSP_U32 u32UlAd1Num = 0;
	BSP_U32 u32UlBdNum = 0;
/*	BSP_U32 u32UlRdNum = 0;*/
	BSP_U32 u32UlAdwptr = 0;
	BSP_U32 u32UlAdrptr = 0;
	BSP_U32 u32UlBdDepth = 0;

	/* 计算空闲AD0数量 */
	u32UlBdDepth = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_DQ_DEPTH_OFFSET);
	u32UlAdwptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_ADQ0_WPTR_OFFSET);
	u32UlAdrptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_ADQ0_RPTR_OFFSET);
	u32UlBdDepth = u32UlBdDepth&IPF_DQ_DEPTH_MASK;
	
	if (u32UlAdwptr >= u32UlAdrptr)/*写指针在前，正常顺序*/
	{
		u32UlAd0Num = u32UlAdwptr - u32UlAdrptr;
	}
	else
	{
		u32UlAd0Num = IPF_ULAD0_DESC_SIZE - (u32UlAdrptr -u32UlAdwptr);
	}
	if(u32UlAd0Num > u32UlBdDepth)
	{
		u32UlAd0Num -= u32UlBdDepth;
	}
	else
	{
		u32UlAd0Num = 0;
		g_stIPFDebugInfo->u32UlAd0NotEnough++;
	}


	/* 计算空闲AD1数量 */
	u32UlBdDepth = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_DQ_DEPTH_OFFSET);
	u32UlAdwptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_ADQ1_WPTR_OFFSET);
	u32UlAdrptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_ADQ1_RPTR_OFFSET);
	u32UlBdDepth = u32UlBdDepth&IPF_DQ_DEPTH_MASK;
	
	if (u32UlAdwptr >= u32UlAdrptr)/*写指针在前，正常顺序*/
	{
		u32UlAd1Num = u32UlAdwptr - u32UlAdrptr;
	}
	else
	{
		u32UlAd1Num =  IPF_ULAD1_DESC_SIZE - (u32UlAdrptr -u32UlAdwptr);
	}
	
	if(u32UlAd1Num > u32UlBdDepth)
	{
		u32UlAd1Num -= u32UlBdDepth;
	}
	else
	{
		u32UlAd1Num = 0;
		g_stIPFDebugInfo->u32UlAd1NotEnough++;
	}


	u32UlBdNum = BSP_IPF_GetUlBDNum();
#if 0
	if(0 == u32UlBdNum)
	{
		g_stIPFDebugInfo->u32UlBdNotEnough++;
	}
#endif

	if(u32UlBdNum > u32UlAd0Num)
	{
		u32UlBdNum = u32UlAd0Num;
	}

	if(u32UlBdNum > u32UlAd1Num)
	{
		u32UlBdNum = u32UlAd1Num;
	}
	/*
	u32UlRdNum = IPF_ULRD_DESC_SIZE - BSP_IPF_GetUlRdNum();
	if(u32UlRdNum > 1)
	{
		u32UlRdNum -= 1;
	}
	else
	{
		u32UlRdNum = 0;
	}


	if(u32UlBdNum > u32UlRdNum)
	{
		u32UlBdNum = u32UlRdNum;
	*/
       return u32UlBdNum;

}


/*****************************************************************************
* 函 数 名      : BSP_IPF_ConfigUpFilter
*
* 功能描述  : IPF上行BD配置函数 
*
* 输入参数  : BSP_U32 u32Num, IPF_CONFIG_ULPARAM_S* pstUlPara
* 输出参数  : 无
* 返 回 值      : IPF_SUCCESS    配置成功
*                           IPF_ERROR      配置失败
*                           BSP_ERR_IPF_NOT_INIT         模块未初始化
*                           BSP_ERR_IPF_INVALID_PARA     参数错误
*
* 修改记录  :2011年11月30日   鲁婷  创建
				2012年11月30日	陈东岳修改添加多过滤器链和
									动态业务模式配置的支持
*****************************************************************************/
BSP_S32 BSP_IPF_ConfigUpFilter(BSP_U32 u32Num, IPF_CONFIG_ULPARAM_S* pstUlPara)
{
    BSP_U32 u32BdqWptr = 0;
    IPF_CONFIG_ULPARAM_S* pstUlParam = pstUlPara;
    BSP_U32 u32BD = 0;
    BSP_U32 i = 0;
    BSP_U32 u32TimeStampEn;

    /* 参数检查 */
    if((NULL == pstUlPara)||(0 == u32Num))
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,
                         "\r BSP_IPF_ConfigUpFilter pInfoNode NULL! \n",0,0,0,0,0,0);
        return BSP_ERR_IPF_INVALID_PARA;
    }

    /* 检查模块是否初始化 */
    if(IPF_ACORE_INIT_SUCCESS != g_IPFInit)
    {
        g_stIPFDebugInfo->ipf_acore_not_init_count++;
        return BSP_ERR_IPF_NOT_INIT;
    }

    /* 检查Ccore是否上电*/
    if(IPF_FORRESET_CONTROL_FORBID <= BSP_IPF_GetControlFLagForCcoreReset())
    {
        g_stIPFDebugInfo->ipf_acore_not_init_count++;
        return BSP_ERR_IPF_CCORE_RESETTING;
    }

    #ifdef __BSP_IPF_DEBUG__
    if(u32Num > *(g_stIpfUl.pu32IdleBd))
    {
        g_stIPFDebugInfo->u32UlBdNotEnough++;
        return IPF_ERROR;
    }
    #endif
    for(i = 0; i < u32Num; i++)
    {
        if(0 == pstUlParam[i].u16Len)
        {
            bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r INPUT BD_Len =0 !Drop pkt \n");
            g_stIPFDebugInfo->ipf_ulbd_len_zero_count++;
            return BSP_ERR_IPF_INVALID_PARA;
        }
    }
	
    u32TimeStampEn = g_stIPFDebugInfo->ipf_timestamp_ul_en;

    /* 读出BD写指针,将u32BdqWptr作为临时写指针使用 */
    u32BdqWptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_BDQ_WPTR_OFFSET); 
    u32BD = (u32BdqWptr&IPF_DQ_PTR_MASK);
    for(i = 0; i < u32Num; i++)
    {
        g_stIpfUl.pstIpfBDQ[u32BD].u16Attribute = pstUlParam[i].u16Attribute;
        g_stIpfUl.pstIpfBDQ[u32BD].u32InPtr = pstUlParam[i].u32Data;
        g_stIpfUl.pstIpfBDQ[u32BD].u16PktLen = pstUlParam[i].u16Len;
        g_stIpfUl.pstIpfBDQ[u32BD].u16UsrField1 = pstUlParam[i].u16UsrField1;
        g_stIpfUl.pstIpfBDQ[u32BD].u32UsrField2= pstUlParam[i].u32UsrField2;
        g_stIpfUl.pstIpfBDQ[u32BD].u32UsrField3 = pstUlParam[i].u32UsrField3;
        ipf_record_start_time_stamp(u32TimeStampEn, &g_stIpfUl.pstIpfBDQ[u32BD].u32UsrField3);

        #ifdef __BSP_IPF_DEBUG__
        if(g_stIPFDebugInfo->u32IpfDebug)
        {
            IPF_PRINT(" func: %d  %x, %d\n", u32BD, g_stIpfUl.pstIpfBDQ[u32BD].u32UsrField2,g_stIpfUl.pstIpfBDQ[u32BD].u32UsrField3);
        }
        #endif
        u32BD = ((u32BD + 1) < IPF_ULBD_DESC_SIZE)? (u32BD + 1) : 0;
    }
    
    /* 检查Ccore是否上电*/
    if(IPF_FORRESET_CONTROL_FORBID <= BSP_IPF_GetControlFLagForCcoreReset())
    {
        g_stIPFDebugInfo->ipf_acore_not_init_count++;
        return BSP_ERR_IPF_CCORE_RESETTING;
    }
	
    g_stIPFDebugInfo->ipf_cfg_ulbd_count += u32Num;

    /* 更新BD写指针*/
    ipf_writel(u32BD, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH0_BDQ_WPTR_OFFSET);  
    return IPF_SUCCESS;
}

/*****************************************************************************
* 函 数 名     : BSP_IPF_ConfigDlAd
*
* 功能描述  : 该接口仅在A核提供，用于移动ADQ写指针，
				给空闲的AD分配新的内存缓冲区，一次可以处理多个AD。
				数传前要调用这个函数分配缓冲区。
*                           
* 输入参数  : BSP_U32 u32ADNum0; 
				 BSP_U32 u32ADNum1; 
				 BSP_VOID* psk0; 
				 BSP_VOID* psk1
*
* 输出参数  : 无
* 返 回 值      : 无
* 修改记录  :2012年11月24日   陈东岳  创建
*****************************************************************************/
BSP_S32 BSP_IPF_ConfigDlAd(BSP_U32 u32AdType, BSP_U32  u32AdNum, IPF_AD_DESC_S * pstAdDesc)
{
	BSP_U32 u32ADQwptr = 0;
	struct tagIPF_AD_DESC_S * pstADDesc = pstAdDesc;
	BSP_U32 i;
	if(NULL == pstAdDesc)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r BSP_IPF_ConfigDlAd input para ERROR! NULL == pstAdDesc\n");
		return BSP_ERR_IPF_INVALID_PARA;
	}	

	if(u32AdType >= IPF_AD_MAX)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r BSP_IPF_ConfigDlAd input para ERROR! u32AdType >= IPF_AD_MAX\n");
		return BSP_ERR_IPF_INVALID_PARA;
	}

	/* 检查模块是否初始化 */
	if(IPF_ACORE_INIT_SUCCESS != g_IPFInit)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r BSP_IPF_ConfigDlAd IPF Not Init! \n");
		return BSP_ERR_IPF_NOT_INIT;
	}


	if(IPF_AD_0 == u32AdType)
	{
		if(u32AdNum >= IPF_DLAD0_DESC_SIZE)
		{
			bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r BSP_IPF_ConfigDlAd input para ERROR! u32AdNum >=IPF_ULAD0_DESC_SIZE\n");
			return BSP_ERR_IPF_INVALID_PARA;
		}

		/*读出写指针*/
		u32ADQwptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_ADQ0_WPTR_OFFSET);
		for(i=0; i < u32AdNum; i++)
		{
			if(NULL == (BSP_VOID*)(pstADDesc->u32OutPtr1))
			{
				bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r BSP_IPF_ConfigDlAd input u32OutPtr1 NULL! \n");
				g_stIPFDebugInfo->ipf_dlad0_error_count++;
				return BSP_ERR_IPF_INVALID_PARA;
			}
			g_stIpfDl.pstIpfADQ0[u32ADQwptr].u32OutPtr1 = pstADDesc->u32OutPtr1;
			g_stIpfDl.pstIpfADQ0[u32ADQwptr].u32OutPtr0 = pstADDesc->u32OutPtr0;
			u32ADQwptr = ((u32ADQwptr + 1) < IPF_DLAD0_DESC_SIZE)? (u32ADQwptr + 1) : 0;		
			pstADDesc++;
		}
		g_stIPFDebugInfo->ipf_cfg_dlad0_count += u32AdNum;

		/* 更新AD0写指针*/
		ipf_writel(u32ADQwptr, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_ADQ0_WPTR_OFFSET);  
	}
	else if(IPF_AD_1 == u32AdType)
	{
		if(u32AdNum >= IPF_DLAD1_DESC_SIZE)
		{
			bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r BSP_IPF_ConfigDlAd input para ERROR! u32AdNum >=IPF_ULAD1_DESC_SIZE \n");
			return BSP_ERR_IPF_INVALID_PARA;
		}

		/*读出写指针*/
		u32ADQwptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_ADQ1_WPTR_OFFSET);
		for(i=0; i < u32AdNum; i++)
		{
			if(NULL == (BSP_VOID*)(pstADDesc->u32OutPtr1))
			{
				bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r BSP_IPF_ConfigDlAd input u32OutPtr1 NULL! \n");
				g_stIPFDebugInfo->ipf_dlad1_error_count++;
				return BSP_ERR_IPF_INVALID_PARA;
			}

			g_stIpfDl.pstIpfADQ1[u32ADQwptr].u32OutPtr1 = pstADDesc->u32OutPtr1;
			g_stIpfDl.pstIpfADQ1[u32ADQwptr].u32OutPtr0 = pstADDesc->u32OutPtr0;
			u32ADQwptr = ((u32ADQwptr + 1) < IPF_DLAD1_DESC_SIZE)? (u32ADQwptr + 1) : 0;		
			pstADDesc++;
		}
		g_stIPFDebugInfo->ipf_cfg_dlad1_count += u32AdNum;

		/* 更新AD1写指针*/
		ipf_writel(u32ADQwptr, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_ADQ1_WPTR_OFFSET);  
	}
	return IPF_SUCCESS;
}

/*****************************************************************************
* 函 数 名     : BSP_IPF_RegisterWakeupDlCb
*
* 功能描述  : 该接口用于注册下行PS任务回调函数
*                           
* 输入参数  : BSP_IPF_WakeupDlkCb *pFnWakeupDl
*
* 输出参数  : 无
* 返 回 值      : 无
*  
* 修改记录  :2011年11月30日   鲁婷  创建
*****************************************************************************/
BSP_S32 BSP_IPF_RegisterWakeupDlCb(BSP_IPF_WakeupDlCb pFnWakeupDl)
{
    /* 参数检查 */
    if(NULL == pFnWakeupDl)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r BSP_IPF_RegisterWakeupDlCb inputPara ERROR! \n");
        return BSP_ERR_IPF_INVALID_PARA;
    }
    g_stIpfDl.pFnDlIntCb = pFnWakeupDl;
    return IPF_SUCCESS;
}

/*****************************************************************************
* 函 数 名     : BSP_IPF_RegisterAdqEmptyDlCb
*
* 功能描述  : 此接口只在A核提供，用于注册唤醒上行PS的
                             ADQ队列空回调函数
*                           
* 输入参数  : BSP_IPF_AdqEmptyCb pAdqEmptyDl
*
* 输出参数  : 无
* 返 回 值      : IPF_SUCCESS 注册成功
*                            IPF_ERROR	注册失败
* 修改记录  :2012年11月24日   陈东岳  创建
*****************************************************************************/
BSP_S32 BSP_IPF_RegisterAdqEmptyDlCb(BSP_IPF_AdqEmptyDlCb pAdqEmptyDl)
{
    /* 参数检查 */
    if(NULL == pAdqEmptyDl)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r BSP_IPF_RegisterAdqEmptyDlCb inputPara ERROR! \n");
        return IPF_ERROR;
    }
    g_stIpfDl.pAdqEmptyDlCb = pAdqEmptyDl;
    return IPF_SUCCESS;
}

/*****************************************************************************
* 函 数 名     : BSP_IPF_GetDlRd
*
* 功能描述  : 该接口用于读取下行BD, 支持一次读取多个BD
*
* 输入参数  : BSP_U32* pu32Num    
*                           IPF_RD_DESC_S *pstRd
*   
* 输出参数  : BSP_U32* pu32Num    实际读取的RD数目
*
* 返 回 值     : IPF_SUCCESS               操作成功
*                           IPF_ERROR                   操作失败
*
* 修改记录  :2011年11月30日   鲁婷  创建
*****************************************************************************/
BSP_VOID BSP_IPF_GetDlRd(BSP_U32* pu32Num, IPF_RD_DESC_S *pstRd)
{
    BSP_U32 u32RdqRptr = 0;
    BSP_U32 u32RdqDepth = 0;
    BSP_U32 u32Num = 0;
    BSP_U32 i = 0;
    BSP_U32 u32CdqRptr;
    BSP_U32 u32TimeStampEn;
	
    /* 读取RD深度 */
    u32RdqDepth = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_DQ_DEPTH_OFFSET);
    u32RdqDepth = (u32RdqDepth>>16)&IPF_DQ_DEPTH_MASK;
	
    u32Num = (u32RdqDepth < *pu32Num)?u32RdqDepth:*pu32Num;
    if(0 == u32Num)
    {
        *pu32Num = 0;
        return;
    }
    u32TimeStampEn = g_stIPFDebugInfo->ipf_timestamp_dl_en;

    /* 读取RD读指针 */
    u32RdqRptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_RDQ_RPTR_OFFSET);
    for(i = 0; i < u32Num; i++)
    {
        
        /* 获取RD */
        pstRd[i].u16Attribute = g_stIpfDl.pstIpfRDQ[u32RdqRptr].u16Attribute;
        pstRd[i].u16PktLen = g_stIpfDl.pstIpfRDQ[u32RdqRptr].u16PktLen;
        pstRd[i].u16Result = g_stIpfDl.pstIpfRDQ[u32RdqRptr].u16Result;
        pstRd[i].u32InPtr = g_stIpfDl.pstIpfRDQ[u32RdqRptr].u32InPtr;
        pstRd[i].u32OutPtr = g_stIpfDl.pstIpfRDQ[u32RdqRptr].u32OutPtr;
        pstRd[i].u16UsrField1 = g_stIpfDl.pstIpfRDQ[u32RdqRptr].u16UsrField1;
        pstRd[i].u32UsrField2 = g_stIpfDl.pstIpfRDQ[u32RdqRptr].u32UsrField2;
        pstRd[i].u32UsrField3 = g_stIpfDl.pstIpfRDQ[u32RdqRptr].u32UsrField3;
        ipf_record_end_time_stamp(u32TimeStampEn, g_stIpfDl.pstIpfRDQ[u32RdqRptr].u32UsrField3);

       /* 更新CD读指针 */
        u32CdqRptr = (IPF_IO_ADDRESS(pstRd[i].u32InPtr) - (BSP_U32)g_stIpfDl.pstIpfCDQ)/sizeof(IPF_CD_DESC_S);

        while(g_stIpfDl.pstIpfCDQ[u32CdqRptr].u16Attribute != 1)
        {
            /* 将释放的CD  清0 */
            g_stIpfDl.pstIpfCDQ[u32CdqRptr].u16Attribute = 0;
            g_stIpfDl.pstIpfCDQ[u32CdqRptr].u16PktLen = 0;
            g_stIpfDl.pstIpfCDQ[u32CdqRptr].u32Ptr = 0;
            u32CdqRptr = ((u32CdqRptr+1) < IPF_DLCD_DESC_SIZE)?(u32CdqRptr+1):0;
        }
        g_stIpfDl.pstIpfCDQ[u32CdqRptr].u16Attribute = 0;
        g_stIpfDl.pstIpfCDQ[u32CdqRptr].u16PktLen = 0;
        g_stIpfDl.pstIpfCDQ[u32CdqRptr].u32Ptr = 0;
		
        u32CdqRptr = ((u32CdqRptr+1) < IPF_DLCD_DESC_SIZE)?(u32CdqRptr+1):0;
        *(g_stIpfDl.u32IpfCdRptr) = u32CdqRptr;
        /* 更新RD读指针 */
        u32RdqRptr = ((u32RdqRptr+1) < IPF_DLRD_DESC_SIZE)?(u32RdqRptr+1):0;        
    }
    ipf_writel(u32RdqRptr, HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_RDQ_RPTR_OFFSET);         
    *pu32Num = u32Num;
    g_stIPFDebugInfo->ipf_get_dlrd_count += u32Num;
}
/*****************************************************************************
* 函 数 名       : BSP_IPF_GetDlAdNum
*
* 功能描述  : 该接口只在A核提供，获取下行（A核）
					空闲(即填入该AD的缓冲区已经被占用)AD数目
*
* 输入参数  :BSP_OK：正常返回
                            BSP_ERROR：出错
*             
* 输出参数  : 无
* 返 回 值     : 无
* 修改记录  : 2011年11月24日   陈东岳  创建
*****************************************************************************/
BSP_S32 BSP_IPF_GetDlAdNum(BSP_U32* pu32AD0Num,BSP_U32* pu32AD1Num)
{
	BSP_U32 u32DlAdDepth = 0;
	BSP_U32 u32DlAdwptr = 0;
	BSP_U32 u32DlAdrptr = 0;

	/* 检查模块是否初始化 */
	if(IPF_ACORE_INIT_SUCCESS != g_IPFInit)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r BSP_IPF_GetDlAdNum IPF Not Init! \n");
		return BSP_ERR_IPF_NOT_INIT;
	}

	/*入参检测*/
	if((NULL == pu32AD0Num)||(NULL == pu32AD1Num))
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r BSP_IPF_GetDlAdNum pstCtrl NULL! \n");
		return BSP_ERR_IPF_INVALID_PARA;
	}
	
	/* 计算空闲AD0数量 */
	u32DlAdwptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_ADQ0_WPTR_OFFSET);
	u32DlAdrptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_ADQ0_RPTR_OFFSET);
	if (u32DlAdwptr >= u32DlAdrptr)/*写指针在前，正常顺序，两指针相等的情况深度为0*/
	{
		u32DlAdDepth = IPF_DLAD0_DESC_SIZE - (u32DlAdwptr - u32DlAdrptr);
	}
	else
	{
		u32DlAdDepth = u32DlAdrptr -u32DlAdwptr;
	}

	/*扣除reserve ad，用于防止硬件将ad队列满识别成空和低功耗引发内存泄露*/
	if(u32DlAdDepth > IPF_ADQ_RESERVE_NUM)
	{
		*pu32AD0Num = u32DlAdDepth - IPF_ADQ_RESERVE_NUM;
	}
	else
	{
		*pu32AD0Num = 0;
	}
	
	/* 计算空闲AD1数量 */
	u32DlAdwptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_ADQ1_WPTR_OFFSET);
	u32DlAdrptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_ADQ1_RPTR_OFFSET);
	if (u32DlAdwptr >= u32DlAdrptr)/*写指针在前，正常顺序*/
	{
		u32DlAdDepth = IPF_DLAD1_DESC_SIZE - (u32DlAdwptr - u32DlAdrptr);
	}
	else
	{
		u32DlAdDepth =  u32DlAdrptr - u32DlAdwptr;
	}
	*pu32AD1Num = u32DlAdDepth;

	/*扣除reserve ad，用于防止硬件将ad队列满识别成空和低功耗引发内存泄露*/	
	if(u32DlAdDepth > IPF_ADQ_RESERVE_NUM)
	{
		*pu32AD1Num = u32DlAdDepth - IPF_ADQ_RESERVE_NUM;
	}
	else
	{
		*pu32AD1Num = 0;
	}

	return IPF_SUCCESS;
}

/*****************************************************************************
* 函 数 名     : BSP_IPF_GetDlRdNum
*
* 功能描述  : 该接口用于读取下行RD数目
*
* 输入参数  : 无
*   
* 输出参数  : 无
*
* 返 回 值     : 上行RD数目
*
* 修改记录  :2012年7月16日   鲁婷  创建
*****************************************************************************/
BSP_U32 BSP_IPF_GetDlRdNum(BSP_VOID)
{
    BSP_U32 u32RdqDepth = 0;
  
    /* 读取RD深度 */
    u32RdqDepth = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_DQ_DEPTH_OFFSET);
    u32RdqDepth = (u32RdqDepth>>16)&IPF_DQ_DEPTH_MASK;
    return u32RdqDepth;
}

/*****************************************************************************
* 函 数 名     : ipf_probe
*
* 功能描述  : 平台设备桩函数
*
* 输入参数  : 无
*   
* 输出参数  : 无
*
* 返 回 值     : 成功
*
* 修改记录  :2013年6月16日   陈东岳  创建
*****************************************************************************/
static int ipf_probe(struct platform_device *pdev)
{

    return IPF_SUCCESS;
}

/*****************************************************************************
* 函 数 名     : ipf_remove
*
* 功能描述  : 平台设备桩函数
*
* 输入参数  : 无
*   
* 输出参数  : 无
*
* 返 回 值     : 成功
*
* 修改记录  :2013年6月16日   陈东岳  创建
*****************************************************************************/
static int ipf_remove(struct platform_device *pdev)
{
    return IPF_SUCCESS;
}

#ifdef CONFIG_PM

/*****************************************************************************
* 函 数 名     : ipf_dl_dpm_prepare
*
* 功能描述  : dpm进入准备函数
*
* 输入参数  : 设备指针
*   
* 输出参数  : 无
*
* 返 回 值     : IPF_ERROR 失败
                            IPF_SUCCESS 成功
*
* 修改记录  :2013年6月16日   陈东岳  创建
*****************************************************************************/
static int ipf_dl_dpm_prepare(struct device *pdev)
{
    u32 u32_dl_state;
	
    #if 0
    u32 u32_adq0_wptr;
    u32 u32_adq0_rptr;
    u32 u32_adq1_wptr;
    u32 u32_adq1_rptr;
    #endif

#ifdef CONFIG_BALONG_MODEM_RESET	
    /* 检查Ccore是否上电，如果已经下电，跳过低功耗保存恢复流程*/
    if(IPF_FORRESET_CONTROL_FORBID <= BSP_IPF_GetControlFLagForCcoreReset())
    {
        bsp_trace(BSP_LOG_LEVEL_WARNING, BSP_MODU_IPF,"\r CCore power down!IPF  Skip dpm process\n");
        return IPF_SUCCESS;
    }
#endif

    /* 判断下行IPF是否空闲 */
    u32_dl_state = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_STATE_OFFSET);

    if(u32_dl_state != IPF_CHANNEL_STATE_IDLE)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r IPF CHANNEL NOT IDLE! \n");
        return IPF_ERROR;
    }
    #if 0
	/*等待ad空中断处理完毕*/
    u32_adq0_wptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_ADQ0_WPTR_OFFSET);
    u32_adq0_rptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_ADQ0_RPTR_OFFSET);
    u32_adq1_wptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_ADQ1_WPTR_OFFSET);
    u32_adq1_rptr = ipf_readl(HI_IPF_REGBASE_ADDR_VIRT + HI_IPF_CH1_ADQ1_RPTR_OFFSET);
    if((u32_adq0_rptr == u32_adq0_wptr)||(u32_adq1_rptr == u32_adq1_wptr))
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r IPF DL AD EMPTY INTRRUPT PROCESSING! \n");
        return IPF_ERROR;
    }
    #endif
    return IPF_SUCCESS;
}

/*****************************************************************************
* 函 数 名     : ipf_dl_dpm_complete
*
* 功能描述  : dpm桩函数
*
* 输入参数  : 设备指针
*   
* 输出参数  : 无
*
* 返 回 值     : 无
*
* 修改记录  :2013年6月16日   陈东岳  创建
*****************************************************************************/
static void ipf_dl_dpm_complete(struct device *pdev)
{
    return ;
}

/*****************************************************************************
* 函 数 名     : ipf_dl_suspend
*
* 功能描述  : dpm桩函数
*
* 输入参数  : 设备指针
*   
* 输出参数  : 无
*
* 返 回 值     : 成功
*
* 修改记录  :2013年6月16日   陈东岳  创建
			2014年1月23日v1.01 陈东岳 修改 由于K3V3总线设计问题，
			无法在m3上进行低功耗恢复，移动到A9上进行。
*****************************************************************************/
static int ipf_dl_suspend(struct device *dev)
{
#if (defined(BSP_CONFIG_HI3630))

    unsigned long ipf_flags = 0;

#ifdef CONFIG_BALONG_MODEM_RESET	
    /* 检查Ccore是否上电如果已经下电，跳过低功耗保存恢复流程*/
    if(IPF_FORRESET_CONTROL_FORBID <= BSP_IPF_GetControlFLagForCcoreReset())
    {
        bsp_trace(BSP_LOG_LEVEL_WARNING, BSP_MODU_IPF,"\r CCore power down!IPF  Skip dpm process\n");
        return IPF_SUCCESS;
    }
#endif

    spin_lock_irqsave(&ipf_filter_spinlock, ipf_flags);
    bsp_ipc_spin_lock(IPC_SEM_IPF_PWCTRL);
    *ipf_acore_init_status = IPF_PWC_DOWN;
    cache_sync();		
    bsp_ipc_spin_unlock(IPC_SEM_IPF_PWCTRL);
    spin_unlock_irqrestore(&ipf_filter_spinlock, ipf_flags);

#endif
    g_stIPFDebugInfo->ipf_acore_suspend_count++;
    return IPF_SUCCESS;
}


/*****************************************************************************
* 函 数 名     : ipf_dl_resume
*
* 功能描述  : dpm桩函数
*
* 输入参数  : 设备指针
*   
* 输出参数  : 无
*
* 返 回 值     : 成功
*
* 修改记录  :2013年6月16日   陈东岳  创建
			2014年1月23日v1.01 陈东岳 修改 由于K3V3总线设计问题，
			无法在m3上进行低功耗恢复，移动到A9上进行。
*****************************************************************************/
static int ipf_dl_resume(struct device *dev)
{
#ifdef CONFIG_BALONG_MODEM_RESET	
    /* 检查Ccore是否上电如果已经下电，跳过低功耗保存恢复流程*/
    if(IPF_FORRESET_CONTROL_FORBID <= BSP_IPF_GetControlFLagForCcoreReset())
    {
        bsp_trace(BSP_LOG_LEVEL_WARNING, BSP_MODU_IPF,"\r CCore power down!IPF Skip dpm process\n");
        return IPF_SUCCESS;
    }
#endif

#if (defined(BSP_CONFIG_HI3630))
    /*由于K3V3总线设计问题，恢复过滤器移动到此处*/
    ipf_check_filter_restore();
#endif
    g_stIPFDebugInfo->ipf_acore_resume_count++;
    return IPF_SUCCESS;
}
#endif

#endif

#ifdef __KERNEL__
module_init(ipf_init); /*lint !e528*/
EXPORT_SYMBOL(ipf_init);
EXPORT_SYMBOL(BSP_IPF_ConfigTimeout);
EXPORT_SYMBOL(BSP_IPF_ConfigUpFilter);
EXPORT_SYMBOL(BSP_IPF_GetUlBDNum);
EXPORT_SYMBOL(BSP_IPF_SetPktLen);
EXPORT_SYMBOL(BSP_IPF_RegisterWakeupDlCb);
EXPORT_SYMBOL(BSP_IPF_GetDlRd);
EXPORT_SYMBOL(BSP_IPF_GetDlRdNum);
EXPORT_SYMBOL(BSP_IPF_Help);
EXPORT_SYMBOL(BSP_IPF_BDInfo);
EXPORT_SYMBOL(BSP_IPF_RDInfo);
EXPORT_SYMBOL(BSP_IPF_Info);
EXPORT_SYMBOL(BSP_IPF_MEM);
EXPORT_SYMBOL(BSP_IPF_ConfigDlAd);
EXPORT_SYMBOL(BSP_IPF_RegisterAdqEmptyDlCb);
EXPORT_SYMBOL(BSP_IPF_GetDlAdNum);
EXPORT_SYMBOL(BSP_IPF_GetControlFLagForCcoreReset);
MODULE_LICENSE("GPL");

#endif

#ifdef __cplusplus
}
#endif

/*lint -restore*/

