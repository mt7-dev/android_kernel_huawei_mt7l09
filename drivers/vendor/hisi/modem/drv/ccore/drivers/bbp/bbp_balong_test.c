

/*lint --e{537}*/
#include <vxWorks.h>
#include <string.h>

#include <osl_thread.h>

#include "drv_dpm.h"
#include "soc_memmap.h"
#include "hi_base.h"
#include "hi_syscrg.h"
#include "hi_syssc.h"
#include "hi_pwrctrl_interface.h"

#include "bsp_memmap.h"
#include <bsp_shared_ddr.h>
#include <bsp_hardtimer.h>
#include <bsp_busstress.h>

#include "bsp_bbp.h"
#include "bbp_balong.h"

/*压力测试代码*/
//#ifdef CFG_CONFIG_MODULE_BUSSTRESS
s32 bbp_stress_test_stat = 0;
u32 bbp_test_interval = 0;/*压力测试中间间歇时长*/
int bbp_edma_chn = 0;/*默认使用通道0*/
int config_a[1024]={0};
int config_b[1024]={0};
#define BBP_TEST_TCM_A 0x48f80000
#define BBP_TEST_TCM_B 0x48f85000
int test_err_num = 0;
int test_num = 0;

extern int sysClkRateGet(void);
static void bbp_msleep(int x)
{
    taskDelay(sysClkRateGet()*x/1000+1);
}
/*****************************************************************************
* 函 数 : bsp_bbp_dma_chn_set
* 功 能 : 设置测试要用的bbp_dma通道
* 输 入 : bbp_edma_chn:BBP EDMA通道号,默认是使用通道0
* 输 出 : void
* 返 回 :
* 说 明 :
*****************************************************************************/
void bbp_dma_chn_set(int edma_chn)
{
    bbp_edma_chn = edma_chn;
    bbp_print_info("bbp_edma_chn is  %d!\n",bbp_edma_chn);
}
/*****************************************************************************
* 函 数 : bbp_dma_test_init
* 功 能 : 初始化测试数据
* 输 入 : void
* 输 出 : void
* 返 回 :
* 说 明 :
*****************************************************************************/
void bbp_dma_test_init(void)
{
    int i = 0;
    /*初始化tcm配置数据*/
    for(i=0;i<1024;i++)
    {
        config_a[i]=i;
        config_b[i]=0;
    }
    bsp_bbe_load_muti();

    memcmp((void*)BBP_TEST_TCM_A,config_a,1024);
}
/*****************************************************************************
* 函 数 : bsp_bbp_dma_cqi_test
* 功 能 : 使用bbp_dma快速配置cqi寄存器,可以验证bbp_dma功能
* 输 入 : bbp_edma_chn:BBP EDMA通道号
* 输 出 : void
* 返 回 :
* 说 明 : edma功能默认是dsp使用
*****************************************************************************/
void bsp_bbp_dma_cqi_set(int edma_chn)
{
    writel(0xffffffff,0x90000030);
    bsp_tcxo_enable(0,1);
    hi_pwrctrl_tcxo1_abb_en_enable();
    bsp_pmu_tcxo1_en_enable(1);
    udelay(3000);
    DRV_PWRCTRL_PLLENABLE(PWC_COMM_MODE_WCDMA,PWC_COMM_MODULE_ABB,0);
    DRV_PWRCTRL_PLLENABLE(PWC_COMM_MODE_WCDMA,PWC_COMM_MODULE_ABB,1);
    DRV_PWRCTRL_PLLENABLE(PWC_COMM_MODE_GSM,PWC_COMM_MODULE_ABB,0);
    DRV_PWRCTRL_PLLENABLE(PWC_COMM_MODE_GSM,PWC_COMM_MODULE_ABB,1);
    DRV_PWRCTRL_PLLENABLE(PWC_COMM_MODE_WCDMA,PWC_COMM_MODULE_BBP_DRX,1);
    bbp_poweron();

    set_hi_crg_clken5_ltebbp_pd_clk_en(1);/*bbp dma开钟*/
    writel(0x1ffff,0x20700160);/*打开bbp所有时钟*/

    /*数据搬移的目的地址，cqi的起始地址*/
	writel(HI_CQI_BASE_ADDR,HI_BBP_DMA_BASE_ADDR+BBP_FAST_CH0_BBP_ADDR_OFFSET+0x10*bbp_edma_chn);
	/*数据搬移的源地址,配置数据的tcm地址*/
	//writel((u32)SHM_BBP_BUSSSTRESS_ADDR,HI_BBP_DMA_BASE_ADDR+BBP_FAST_CH0_SOC_ADDR_OFFSET+0x10*bbp_edma_chn);
    writel(BBP_TEST_TCM_A,HI_BBP_DMA_BASE_ADDR+BBP_FAST_CH0_SOC_ADDR_OFFSET+0x10*bbp_edma_chn);
    /*配置*/
    writel(0x111003ff,HI_BBP_DMA_BASE_ADDR+BBP_FAST_CH0_CTL_OFFSET+0x10*bbp_edma_chn);
}
/*****************************************************************************
* 函 数 : bsp_bbp_dma_cqi_test
* 功 能 : 读取cqi寄存器配置
* 输 入 : bbp_edma_chn:BBP EDMA通道号
* 输 出 : void
* 返 回 :
* 说 明 : edma功能默认是dsp使用
*****************************************************************************/
void bsp_bbp_dma_cqi_get(int edma_chn)
{

    writel(0xffffffff,0x90000030);
    bsp_tcxo_enable(0,1);
    hi_pwrctrl_tcxo1_abb_en_enable();
    bsp_pmu_tcxo1_en_enable(1);
    udelay(3000);
    DRV_PWRCTRL_PLLENABLE(PWC_COMM_MODE_WCDMA,PWC_COMM_MODULE_ABB,0);
    DRV_PWRCTRL_PLLENABLE(PWC_COMM_MODE_WCDMA,PWC_COMM_MODULE_ABB,1);
    DRV_PWRCTRL_PLLENABLE(PWC_COMM_MODE_GSM,PWC_COMM_MODULE_ABB,0);
    DRV_PWRCTRL_PLLENABLE(PWC_COMM_MODE_GSM,PWC_COMM_MODULE_ABB,1);
    DRV_PWRCTRL_PLLENABLE(PWC_COMM_MODE_WCDMA,PWC_COMM_MODULE_BBP_DRX,1);
    bbp_poweron();

    set_hi_crg_clken5_ltebbp_pd_clk_en(1);/*bbp dma开钟*/
    writel(0x1ffff,0x20700160);/*打开bbp所有时钟*/

    /*数据搬移的目的地址，cqi的起始地址*/
	writel(HI_CQI_BASE_ADDR,HI_BBP_DMA_BASE_ADDR+BBP_FAST_CH0_BBP_ADDR_OFFSET+0x10*bbp_edma_chn);
	/*数据搬移的源地址,配置数据的tcm地址*/
	//writel((u32)SHM_BBP_BUSSSTRESS_ADDR,HI_BBP_DMA_BASE_ADDR+BBP_FAST_CH0_SOC_ADDR_OFFSET+0x10*bbp_edma_chn);
    writel(BBP_TEST_TCM_B,HI_BBP_DMA_BASE_ADDR+BBP_FAST_CH0_SOC_ADDR_OFFSET+0x10*bbp_edma_chn);
    /*配置*/
    writel(0x111103ff,HI_BBP_DMA_BASE_ADDR+BBP_FAST_CH0_CTL_OFFSET+0x10*bbp_edma_chn);
}
/*****************************************************************************
* 函 数 : bbp_dma_test_init
* 功 能 : 初始化测试数据
* 输 入 : void
* 输 出 : void
* 返 回 :
* 说 明 :
*****************************************************************************/
void bbp_dma_test_init_byddr(void)
{
    int i = 0;
    /*初始化tcm配置数据*/
    for(i=0;i<1024;i++)
    {
        config_a[i]=i;
        config_b[i]=0;
    }
}
/*****************************************************************************
* 函 数 : bsp_bbp_dma_cqi_test
* 功 能 : 使用bbp_dma快速配置cqi寄存器,可以验证bbp_dma功能
* 输 入 : bbp_edma_chn:BBP EDMA通道号
* 输 出 : void
* 返 回 :
* 说 明 : edma功能默认是dsp使用
*****************************************************************************/
void bsp_bbp_dma_cqi_set_byddr(int edma_chn)
{
    //set_hi_crg_clken5_ltebbp_pd_clk_en(1);/*开钟*/
    writel(0xffffffff,0x90000030);
    bsp_tcxo_enable(0,1);
    hi_pwrctrl_tcxo1_abb_en_enable();
    bsp_pmu_tcxo1_en_enable(1);
    udelay(3000);
    DRV_PWRCTRL_PLLENABLE(PWC_COMM_MODE_WCDMA,PWC_COMM_MODULE_ABB,0);
    DRV_PWRCTRL_PLLENABLE(PWC_COMM_MODE_WCDMA,PWC_COMM_MODULE_ABB,1);
    DRV_PWRCTRL_PLLENABLE(PWC_COMM_MODE_GSM,PWC_COMM_MODULE_ABB,0);
    DRV_PWRCTRL_PLLENABLE(PWC_COMM_MODE_GSM,PWC_COMM_MODULE_ABB,1);
    DRV_PWRCTRL_PLLENABLE(PWC_COMM_MODE_WCDMA,PWC_COMM_MODULE_BBP_DRX,1);
    bbp_poweron();

    set_hi_crg_clken5_ltebbp_pd_clk_en(1);/*bbp dma开钟*/
    writel(0x1ffff,0x20700160);/*打开bbp所有时钟*/
    /*数据搬移的目的地址，cqi的起始地址*/
	writel(HI_CQI_BASE_ADDR,HI_BBP_DMA_BASE_ADDR+BBP_FAST_CH0_BBP_ADDR_OFFSET+0x10*bbp_edma_chn);
	/*数据搬移的源地址,配置数据的tcm地址*/
	//writel((u32)SHM_BBP_BUSSSTRESS_ADDR,HI_BBP_DMA_BASE_ADDR+BBP_FAST_CH0_SOC_ADDR_OFFSET+0x10*bbp_edma_chn);
    writel((u32)config_a,HI_BBP_DMA_BASE_ADDR+BBP_FAST_CH0_SOC_ADDR_OFFSET+0x10*bbp_edma_chn);
    /*配置*/
    writel(0x111003ff,HI_BBP_DMA_BASE_ADDR+BBP_FAST_CH0_CTL_OFFSET+0x10*bbp_edma_chn);
}
/*****************************************************************************
* 函 数 : bsp_bbp_dma_cqi_test
* 功 能 : 读取cqi寄存器配置
* 输 入 : bbp_edma_chn:BBP EDMA通道号
* 输 出 : void
* 返 回 :
* 说 明 : edma功能默认是dsp使用
*****************************************************************************/
void bsp_bbp_dma_cqi_get_byddr(int edma_chn)
{

    writel(0xffffffff,0x90000030);
    bsp_tcxo_enable(0,1);
    hi_pwrctrl_tcxo1_abb_en_enable();
    bsp_pmu_tcxo1_en_enable(1);
    udelay(3000);
    DRV_PWRCTRL_PLLENABLE(PWC_COMM_MODE_WCDMA,PWC_COMM_MODULE_ABB,0);
    DRV_PWRCTRL_PLLENABLE(PWC_COMM_MODE_WCDMA,PWC_COMM_MODULE_ABB,1);
    DRV_PWRCTRL_PLLENABLE(PWC_COMM_MODE_GSM,PWC_COMM_MODULE_ABB,0);
    DRV_PWRCTRL_PLLENABLE(PWC_COMM_MODE_GSM,PWC_COMM_MODULE_ABB,1);
    DRV_PWRCTRL_PLLENABLE(PWC_COMM_MODE_WCDMA,PWC_COMM_MODULE_BBP_DRX,1);
    bbp_poweron();

    set_hi_crg_clken5_ltebbp_pd_clk_en(1);/*bbp dma开钟*/
    writel(0x1ffff,0x20700160);/*打开bbp所有时钟*/

    /*数据搬移的目的地址，cqi的起始地址*/
	writel(HI_CQI_BASE_ADDR,HI_BBP_DMA_BASE_ADDR+BBP_FAST_CH0_BBP_ADDR_OFFSET+0x10*bbp_edma_chn);
	/*数据搬移的源地址,配置数据的tcm地址*/
	//writel((u32)SHM_BBP_BUSSSTRESS_ADDR,HI_BBP_DMA_BASE_ADDR+BBP_FAST_CH0_SOC_ADDR_OFFSET+0x10*bbp_edma_chn);
    writel((u32)config_b,HI_BBP_DMA_BASE_ADDR+BBP_FAST_CH0_SOC_ADDR_OFFSET+0x10*bbp_edma_chn);
    /*配置*/
    writel(0x111103ff,HI_BBP_DMA_BASE_ADDR+BBP_FAST_CH0_CTL_OFFSET+0x10*bbp_edma_chn);
}
void bsp_bbp_cqi_compare(void)
{
    int i;

    for(i = 0; i < 1024; i++)
    {
        if(config_a[i] != config_b[i])
        {
            test_err_num++;
            break;
        }
    }
}



void set_bbp_test_stat(int run_or_stop)
{
	bbp_stress_test_stat = run_or_stop;
}
int get_bbp_test_stat(void)
{
    return bbp_stress_test_stat;
}
/*****************************************************************************
* 函 数 : bbp_stress_test_routine
* 功 能 : 循环使用bbp_dma快速配置cqi寄存器,使得edma所在的bbp master持续工作
* 输 入 : void
* 输 出 : void
* 返 回 :
* 说 明 : 主要为了验证在多master同时工作时总线是否正常
*****************************************************************************/
s32 bbp_stress_test_routine(void)
{
    while(get_bbp_test_stat()){
        bsp_bbp_dma_cqi_set_byddr(bbp_edma_chn);
        bsp_bbp_dma_cqi_get_byddr(bbp_edma_chn);
        bsp_bbp_cqi_compare();
        test_num++;
        bbp_msleep(bbp_test_interval);
    }
    return 0;
}
/*****************************************************************************
* 函 数 : bbp_stress_test_start
* 功 能 : bbp压力测试开始
* 输 入 : void
* 输 出 : void
* 返 回 :
* 说 明 :
*****************************************************************************/
s32 bbp_stress_test_start(u32 priority , u32 interval)
{
    u32 task_id;

    bbp_dma_test_init_byddr();
    set_bbp_test_stat(TEST_RUN);
    bbp_test_interval = interval;

    osl_task_init("BbpStressTask",priority,BBP_TASK_STK_SIZE,bbp_stress_test_routine,NULL,&task_id);

    return 0;
}
/*****************************************************************************
* 函 数 : lcd_emi_edma_stress_test_stop
* 功 能 : bbp压力测试结束
* 输 入 : void
* 输 出 : void
* 返 回 :
* 说 明 :
*****************************************************************************/
s32 bbp_stress_test_stop()
{
    set_bbp_test_stat(TEST_STOP);
    /*输出测试结果*/
    bbp_print_error("test %d times, fail %d times!\n",test_num,test_err_num);

	return 0;
}
//#endif
/*压力测试代码结束*/
