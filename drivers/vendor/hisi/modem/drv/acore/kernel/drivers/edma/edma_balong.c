
/*lint --e{537}*/
/* Warning 537: (Warning -- Repeated include  */
/*lint *****--e{537,713,732,701,438,830,958,737}*/

#ifdef __KERNEL__
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <hi_base.h>
#include <osl_types.h>
#elif defined(__VXWORKS__)
#include "drv_hw_adp.h"
#include <taskLibCommon.h>
#include <string.h>
#include <bsp_dpm.h>
#endif

#include "osl_bio.h"
#include "osl_irq.h"
#include "osl_module.h"
#include <hi_syssc_interface.h>
#include "drv_version.h"
#include "bsp_edma.h"
#include "bsp_ipc.h"
#include "bsp_vic.h"
#include <bsp_om.h>

#ifdef __KERNEL__
#define OSDRV_MODULE_VERSION_STRING  "ANDROID_4.2-EDMAC @HiV7R2 SFT ACPU"
#define DRIVER_NAME  "edmac_device"
#endif

#define edma_cache_sync()    cache_sync()

struct edma_id_int_isr
{
    struct chan_int_service edma_trans1_isr[ EDMA_CH16_NUM ];
    struct chan_int_service edma_trans2_isr[ EDMA_CH16_NUM ];
    struct chan_int_service edma_err1_isr[ EDMA_CH16_NUM ];
    struct chan_int_service edma_err2_isr[ EDMA_CH16_NUM ];
    struct chan_int_service edma_err3_isr[ EDMA_CH16_NUM ];
};

struct edma_drv_info
{
    void *edma_base_addr[EDMA_NUMBER];
    /*  the edmac register pointer    static */
    struct edma_reg_struct * edma_reg_str[EDMA_NUMBER];/*2 edma :16 channel & 4 channel */
    /*edma Call back Function*/
    struct edma_id_int_isr  edma_int_isr[EDMA_NUMBER];
};

struct edma_drv_info g_edma_drv_info;

/* edma通道分配方式 */
u32 g_edma_chan_distri[EDMA_REQ_MAX] = {0};

void edma_set_trace_level(u32 level);
u32 edma_id_valid(int edma_id);
void edma_init (void);
s32 edma_channel_int_enable(u32 channel_id);
s32 edma_channel_int_disable(u32 channel_id);
s32 bsp_edma_remove(struct platform_device *pdev);
void edma_isr_install(u32 channel_id, channel_isr pFunc, u32 channel_arg, u32 int_flag);

void edma_set_trace_level(u32 level)
{
    (void)bsp_mod_level_set(BSP_MODU_EDMA, level);
    hiedmac_trace(BSP_LOG_LEVEL_ERROR,"bsp_mod_level_set(BSP_MODU_EDMA=%d, %d)\n",BSP_MODU_EDMA,level);
}

/* inline  两个 edma  检测是否已经初始化。
    若已经初始化，返回 1 ；否则返回 0 */
 u32 edma_id_valid(int edma_id)
{
    if( g_edma_drv_info.edma_reg_str[edma_id])
    {return true;}
    else
    {
        return false;
    }
}

/*******************************************************************************
  函数名:     static void edma_channel_isr_init(void)
  功能描述:   初始化所有通道的中断回调函数
  输入参数:   无
  输出参数:   无
  返回值:     无
  全局变量初始化为零 该函数可能不需要保留。
*******************************************************************************/
#if 0
static void edma_channel_isr_init(void)
{
    u32 index;
    for (index = 0; index < EDMA_CHANNEL_NUM; index++)
    {
        g_edma_drv_info.edma_trans1_isr[index].chan_isr = NULL;
        g_edma_drv_info.edma_trans1_isr[index].chan_arg = 0;
        g_edma_drv_info.edma_trans1_isr[index].int_status = 0;

        g_edma_drv_info.edma_trans2_isr[index].chan_isr = NULL;
        g_edma_drv_info.edma_trans2_isr[index].chan_arg = 0;
        g_edma_drv_info.edma_trans2_isr[index].int_status = 0;

        g_edma_drv_info.edma_err1_isr[index].chan_isr = NULL;
        g_edma_drv_info.edma_err1_isr[index].chan_arg = 0;
        g_edma_drv_info.edma_err1_isr[index].int_status = 0;

        g_edma_drv_info.edma_err2_isr[index].chan_isr = NULL;
        g_edma_drv_info.edma_err2_isr[index].chan_arg = 0;
        g_edma_drv_info.edma_err2_isr[index].int_status = 0;

        g_edma_drv_info.edma_err3_isr[index].chan_isr = NULL;
        g_edma_drv_info.edma_err3_isr[index].chan_arg = 0;
        g_edma_drv_info.edma_err3_isr[index].int_status = 0;


    }
}
#endif
/*******************************************************************************
  函数名:      static void edma_isr_install(u32 channel_id,
                       channel_isr pFunc, u32 channel_arg, u32 int_flag)
  功能描述:    根据int_flag，注册通道对应的中断回调函数
  输入参数:    channel_id : 通道ID，调用 bsp_edma_channel_init 函数的返回值
               pFunc : 调用者创建的通道中断回调函数
               channel_arg : pFunc的入参1
               int_flag : pFunc的入参2, 中断类型，可组合
#define EDMA_INT_DONE           1          EDMA传输完成中断
#define EDMA_INT_LLT_DONE       2          链式EDMA节点传输完成中断
#define EDMA_INT_CONFIG_ERR     4          EDMA配置错误导致的中断
#define EDMA_INT_TRANSFER_ERR   8          EDMA传输错误导致的中断
#define EDMA_INT_READ_ERR       16         EDMA链表读错误导致的中断
  输出参数:    无
  返回值:      无
*******************************************************************************/

/*可能要改为 bsp_edma_isr_install */
void edma_isr_install(u32 channel_id, channel_isr pFunc, u32 channel_arg, u32 int_flag)
{
    u32 int_judge = 0;
    u32 chan_index = 0;

    s32 edma_id = -1;
    s32 edma_id_chan_id = -1;
    /*EDMAC_REG_STRU * edma_reg_str = 0;*****/
    struct edma_id_int_isr *edma_int_isr = 0;

    edma_id = EDMA_ID(channel_id);
    if(!edma_id_valid(edma_id))
    {
        hiedmac_trace(BSP_LOG_LEVEL_ERROR, "channel_id %d not supported \n", channel_id);
        return;
    }
    edma_id_chan_id = (s32)EDMA_ID_CHAN_ID(channel_id);
    edma_int_isr = &(g_edma_drv_info.edma_int_isr[edma_id]);

    if (channel_id >= EDMA_CHANNEL_END)
    {
        hiedmac_trace(BSP_LOG_LEVEL_ERROR,"EDMA_CHANNEL_INVALID\n");
        return;
    }
    else
    {
        if ((NULL == pFunc)||(0==int_flag))
        {
            hiedmac_trace(BSP_LOG_LEVEL_WARNING,"edma_isr is NULL or int_flag is 0!\n");
            return;
        }
        else
        {
            /*0x1F,低5位全1，对应5种中断类型*/
            int_judge = EDMA_INT_ALL & int_flag;
            chan_index = (u32)edma_id_chan_id;
            if (int_judge & EDMA_INT_DONE)
            {
                edma_int_isr->edma_trans1_isr[chan_index].chan_isr = (channel_isr)pFunc;
                edma_int_isr->edma_trans1_isr[chan_index].chan_arg = channel_arg;
                edma_int_isr->edma_trans1_isr[chan_index].int_status = int_flag;
            }
            else{}
			if (int_judge & EDMA_INT_LLT_DONE)
            {
                edma_int_isr->edma_trans2_isr[chan_index].chan_isr = (channel_isr)pFunc;
                edma_int_isr->edma_trans2_isr[chan_index].chan_arg = channel_arg;
                edma_int_isr->edma_trans2_isr[chan_index].int_status = int_flag;
            }
            else{}
			if (int_judge & EDMA_INT_CONFIG_ERR)
            {
                edma_int_isr->edma_err1_isr[chan_index].chan_isr = (channel_isr)pFunc;
                edma_int_isr->edma_err1_isr[chan_index].chan_arg = channel_arg;
                edma_int_isr->edma_err1_isr[chan_index].int_status = int_flag;
            }
            else{}
			if (int_judge & EDMA_INT_TRANSFER_ERR)
            {
                edma_int_isr->edma_err2_isr[chan_index].chan_isr = (channel_isr)pFunc;
                edma_int_isr->edma_err2_isr[chan_index].chan_arg = channel_arg;
                edma_int_isr->edma_err2_isr[chan_index].int_status = int_flag;
            }
             else{}
			 if (int_judge & (EDMA_INT_READ_ERR))
            {
                edma_int_isr->edma_err3_isr[chan_index].chan_isr = (channel_isr)pFunc;
                edma_int_isr->edma_err3_isr[chan_index].chan_arg = channel_arg;
                edma_int_isr->edma_err3_isr[chan_index].int_status = int_flag;
            }
			else{}
            hiedmac_trace(BSP_LOG_LEVEL_DEBUG,"edma_isr_install!\n");
        }
    }

}

/*******************************************************************************
  函数名:      static void edma_ip_init(void)
  功能描述:    初始化各中断寄存器、中断屏蔽寄存器
  输入参数:    无
  输出参数:    无
  返回值:      无
*******************************************************************************/
static void edma_ip_init(void)
{
/*共享通道初始化*/

    /* 按照 2013.04.11 edma通道分配表 */
    /* 所有通道均已分配，通道7给ldsp  */
    g_edma_chan_distri[EDMA_SPI1_TX]      = (u32)EDMA_CH_DRV_LCD;        /* Acore drv lcd 刷屏       实际分配通道 ch16 - 14 */
    g_edma_chan_distri[EDMA_LTESIO_RX]    = (u32)-1;                /* HIFI                     实际分配通道 ch16 - 8  */
    g_edma_chan_distri[EDMA_LTESIO_TX]    = (u32)-1;                /* HIFI                     实际分配通道 ch16 - 9  */
    g_edma_chan_distri[EDMA_HSUART_RX]    = (u32)EDMA_CH_HSUART_RX;      /* Acore drv 蓝牙语音场景   实际分配通道 ch16 - 12 */
    g_edma_chan_distri[EDMA_HSUART_TX]    = (u32)EDMA_CH_HSUART_TX;      /* Acore drv 蓝牙语音场景   实际分配通道 ch16 - 13 */
    g_edma_chan_distri[EDMA_SCI0_RX]      = (u32)EDMA_CH_DRV_SIM_0;      /* Acore drv SIM卡1收       实际分配通道 ch16 - 10 */
    g_edma_chan_distri[EDMA_SCI0_TX]      = (u32)EDMA_CH_DRV_SIM_0;      /* Acore drv SIM卡1发       实际分配通道 ch16 - 10 */
    g_edma_chan_distri[EDMA_SCI1_RX]      = (u32)EDMA_CH_DRV_SIM_1;      /* Acore drv SIM卡2收       实际分配通道 ch16 - 11 */
    g_edma_chan_distri[EDMA_SCI1_TX]      = (u32)EDMA_CH_DRV_SIM_1;      /* Acore drv SIM卡2发       实际分配通道 ch16 - 11 */
    g_edma_chan_distri[EDMA_BBP_DBG]      = (u32)EDMA_CH_GUDSP_MEMORY_9; /* gudsp 的请求 bbp搬数     实际分配通道 ch16 - 6  */
    g_edma_chan_distri[EDMA_BBP_GRIF]     = (u32)EDMA_CH_GUDSP_MEMORY_9; /* gudsp 的请求 bbp采数     实际分配通道 ch16 - 6  */
    g_edma_chan_distri[EDMA_AMON_SOC]     = (u32)EDMA_CH_DRV_AXIMON;     /* Acore drv AXIMON         实际分配通道 ch16 - 15 */
    g_edma_chan_distri[EDMA_AMON_CPUFAST] = (u32)EDMA_CH_DRV_AXIMON;     /* Acore drv AXIMON         实际分配通道 ch16 - 15 */


    /* gudsp 的请求 M2M,  */
    g_edma_chan_distri[EDMA_MEMORY_DSP_1] = (u32)EDMA_CH_GUDSP_MEMORY_1; /* 用于turbo译码            实际分配通道 ch4  - 0 */
    g_edma_chan_distri[EDMA_MEMORY_DSP_2] = (u32)EDMA_CH_GUDSP_MEMORY_2; /* 用于viterb译码           实际分配通道 ch4  - 1 */
    g_edma_chan_distri[EDMA_MEMORY_DSP_3] = (u32)EDMA_CH_GUDSP_MEMORY_3; /* 用于CQI表搬移和LDSP复用  实际分配通道 ch16 - 2 == */
    g_edma_chan_distri[EDMA_MEMORY_DSP_4] = (u32)EDMA_CH_GUDSP_MEMORY_4; /* 用于上行编码             实际分配通道 ch4  - 2 */
    g_edma_chan_distri[EDMA_MEMORY_DSP_5] = (u32)EDMA_CH_GUDSP_MEMORY_6; /* 用于APT表搬移            实际分配通道 ch4  - 3 */
    g_edma_chan_distri[EDMA_MEMORY_DSP_6] = (u32)EDMA_CH_GUDSP_MEMORY_7; /* GUDSP加载/AHB加载复用    实际分配通道 ch16 - 7 */

    g_edma_chan_distri[EDMA_PWC_LDSP_TCM] = (u32)EDMA_CH_LOAD_LDSP_TCM;  /* 底软备份恢复LDSP TCM     实际分配通道 ch16 - 0 == */
    g_edma_chan_distri[EDMA_PWC_TDSP_TCM] = (u32)EDMA_CH_LOAD_TDSP_TCM;  /* 底软备份恢复TDSP TCM     实际分配通道 ch16 - 1 == */

    /*LDSP的通道，arm中不应该使用  实际分配通道 ch16  0 - 5 */
    g_edma_chan_distri[EDMA_LDSP_API_USED_0] = (u32)EDMA_CH_LDSP_API_USED_0;
    /*
    g_edma_chan_distri[EDMA_LDSP_API_USED_1] = EDMA_CH_LDSP_API_USED_1;
    g_edma_chan_distri[EDMA_LDSP_LCS_SDR]    = EDMA_CH_LDSP_LCS_SDR;
    g_edma_chan_distri[EDMA_LDSP_CSU_SDR]    = EDMA_CH_LDSP_CSU_SDR;
    g_edma_chan_distri[EDMA_LDSP_EMU_SDR]    = EDMA_CH_LDSP_EMU_SDR;
    g_edma_chan_distri[EDMA_LDSP_NV_LOADING] = EDMA_CH_LDSP_NV_LOADING;
    */

    /* Clear interrupt */
    if(edma_id_valid(EDMA_CH16_ID))
    {
        g_edma_drv_info.edma_reg_str[EDMA_CH16_ID]->ulIntTC1Raw  = 0;
        g_edma_drv_info.edma_reg_str[EDMA_CH16_ID]->ulIntTC2Raw  = 0;
        g_edma_drv_info.edma_reg_str[EDMA_CH16_ID]->ulIntERR1Raw = 0;
        g_edma_drv_info.edma_reg_str[EDMA_CH16_ID]->ulIntERR2Raw = 0;
    	g_edma_drv_info.edma_reg_str[EDMA_CH16_ID]->ulIntERR3Raw = 0;
    }
    else{}

    if(edma_id_valid(EDMA_CH4_ID))
    {
        g_edma_drv_info.edma_reg_str[EDMA_CH4_ID]->ulIntTC1Raw  = 0;
        g_edma_drv_info.edma_reg_str[EDMA_CH4_ID]->ulIntTC2Raw  = 0;
        g_edma_drv_info.edma_reg_str[EDMA_CH4_ID]->ulIntERR1Raw = 0;
        g_edma_drv_info.edma_reg_str[EDMA_CH4_ID]->ulIntERR2Raw = 0;
    	g_edma_drv_info.edma_reg_str[EDMA_CH4_ID]->ulIntERR3Raw = 0;
    }
    else{}
}

void edma_init (void)
 {
    /*edma_channel_isr_init();*/
    edma_ip_init();
 }

/*******************************************************************************
function     :static void edma_channel_int_enable(u32 channel_id)

description  :Clear and enable int for [channel_id]. Used when a channe_id is
              applied and channel_isr is given.

input        :channel_id
output       :none
return       :none
*******************************************************************************/
s32 edma_channel_int_enable(u32 channel_id)
{
    unsigned long lock_key = 0;

    s32 edma_id = -1;
    s32 edma_id_chan_id = -1;
    struct edma_reg_struct * edma_reg_str = 0;
    u32 chan_int_mask = 0;

    edma_id = EDMA_ID(channel_id);
    if(!edma_id_valid(edma_id))
    {
        hiedmac_trace(BSP_LOG_LEVEL_ERROR, "channel_id %d not supported \n", channel_id);
        return EDMA_FAIL;
    }
    edma_id_chan_id = (s32)EDMA_ID_CHAN_ID(channel_id);
    edma_reg_str = g_edma_drv_info.edma_reg_str[edma_id];

    chan_int_mask = (u32)0x1<<edma_id_chan_id;
    /* Clear interrupt :write 1 clear; write 0 no change */
    edma_reg_str->ulIntTC1Raw  = chan_int_mask;
    edma_reg_str->ulIntTC2Raw  = chan_int_mask;
    edma_reg_str->ulIntERR1Raw = chan_int_mask;
    edma_reg_str->ulIntERR2Raw = chan_int_mask;
	edma_reg_str->ulIntERR3Raw = chan_int_mask;

    /* Enable interrupt: 0-mask, 1-do not mask */
    local_irq_save(lock_key);
    /*DONOT need multicore mutual exclusion*/

    edma_reg_str->stCpuXReg[EDMAC_DT_ARM].ulIntTC1Mask  |= chan_int_mask;
    edma_reg_str->stCpuXReg[EDMAC_DT_ARM].ulIntTC2Mask  |= chan_int_mask;
    edma_reg_str->stCpuXReg[EDMAC_DT_ARM].ulIntErr1Mask |= chan_int_mask;
    edma_reg_str->stCpuXReg[EDMAC_DT_ARM].ulIntErr2Mask |= chan_int_mask;
    edma_reg_str->stCpuXReg[EDMAC_DT_ARM].ulIntErr3Mask |= chan_int_mask;

    local_irq_restore(lock_key);
    return EDMA_SUCCESS;
}

/*******************************************************************************
function     :static void edma_int_disable(u32 channel_id)

description  :Clear and enable int for [channel_id]. Used when a channe_id is
              applied and channel_isr is given.

input        :channel_id
output       :none
return       :none
*******************************************************************************/
s32 edma_channel_int_disable(u32 channel_id)
{
    unsigned long  lock_key = 0;

    s32 edma_id = -1;
    s32 edma_id_chan_id = -1;
    struct edma_reg_struct * edma_reg_str = 0;
    u32 chan_int_mask = 0;
    edma_id = EDMA_ID(channel_id);
    edma_id_chan_id = (s32)EDMA_ID_CHAN_ID(channel_id);
    edma_reg_str = g_edma_drv_info.edma_reg_str[edma_id];
    if(!edma_id_valid(edma_id))
    {
        hiedmac_trace(BSP_LOG_LEVEL_ERROR, "channel_id %d not supported \n", channel_id);
        return EDMA_FAIL;
    }

    chan_int_mask = (u32)0x1<<edma_id_chan_id;
    /* Clear interrupt :write 1 clear; write 0 no change */
    edma_reg_str->ulIntTC1Raw  = chan_int_mask;
    edma_reg_str->ulIntTC2Raw  = chan_int_mask;
    edma_reg_str->ulIntERR1Raw = chan_int_mask;
    edma_reg_str->ulIntERR2Raw = chan_int_mask;
	edma_reg_str->ulIntERR3Raw = chan_int_mask;

    chan_int_mask = ~chan_int_mask;
    /* Enable interrupt: 0-mask, 1-do not mask */
    local_irq_save(lock_key);
    /*DONOT need multicore mutual exclusion*/
    edma_reg_str->stCpuXReg[EDMAC_DT_ARM].ulIntTC1Mask  &= chan_int_mask;
    edma_reg_str->stCpuXReg[EDMAC_DT_ARM].ulIntTC2Mask  &= chan_int_mask;
    edma_reg_str->stCpuXReg[EDMAC_DT_ARM].ulIntErr1Mask &= chan_int_mask;
    edma_reg_str->stCpuXReg[EDMAC_DT_ARM].ulIntErr2Mask &= chan_int_mask;
    edma_reg_str->stCpuXReg[EDMAC_DT_ARM].ulIntErr3Mask &= chan_int_mask;
    local_irq_restore(lock_key);
    return EDMA_SUCCESS;
}


/*******************************************************************************
function     :void peri_set_scctrl54_dma_sel(enum edma_req_id request, u32 channel_id)

description  :set sc_Ctrl54 dma_sel for peri. ( only called in bsp_edma_channel_init() )

input        :channel_id
output       :none
return       :none
*******************************************************************************/
void edma_set_peri_dma_sel(enum edma_req_id request, u32 channel_id)
{
    s32 edma_id = -1;

    if (channel_id >= EDMA_CHANNEL_END)
    {
        hiedmac_trace(BSP_LOG_LEVEL_ERROR," ----channel ID= %d, WRONG \n",  channel_id );
        return;
    }
    else
    {
        edma_id = EDMA_ID(channel_id);
        if(!edma_id_valid(edma_id))
        {
            hiedmac_trace(BSP_LOG_LEVEL_ERROR, "channel_id %d not supported \n", channel_id);
            return;
        }
        else{}
    }
    switch(request)
    {
        case EDMA_SPI0_RX:
        case EDMA_SPI0_TX:
            edma_dw_ssi0_dma_sel((u32)edma_id);
            break;
        case EDMA_SPI1_RX:
        case EDMA_SPI1_TX:
            edma_dw_ssi1_dma_sel((u32)edma_id);
            break;
        case EDMA_LTESIO_RX:
        case EDMA_LTESIO_TX:
            edma_sio_dma_sel((u32)edma_id);
            break;
        case EDMA_HSUART_RX:
        case EDMA_HSUART_TX:
            edma_hs_uart_dma_sel((u32)edma_id);
            break;
        case EDMA_UART0_RX:
        case EDMA_UART0_TX:
            edma_uart0_dma_sel((u32)edma_id);
            break;
        case EDMA_UART1_RX:
        case EDMA_UART1_TX:
            edma_uart1_dma_sel((u32)edma_id);
            break;
        case EDMA_UART2_RX:
        case EDMA_UART2_TX:
            edma_uart2_dma_sel((u32)edma_id);
            break;
        case EDMA_SCI0_RX:
        case EDMA_SCI0_TX:
            edma_sci0_dma_sel((u32)edma_id);
            break;
        case EDMA_SCI1_RX:
        case EDMA_SCI1_TX:
            edma_sci1_dma_sel((u32)edma_id);
            break;
            
        case EDMA_GBBP0_DBG:
            edma_bbp_dbg_dma_sel((u32)edma_id);
            break;
        case EDMA_GBBP1_GRIF:
            edma_bbp_grif_dma_sel((u32)edma_id);
            break;
        case EDMA_AMON_SOC:
            edma_amon_soc_dma_sel((u32)edma_id);
            break;
        case EDMA_AMON_CPUFAST:
            edma_amon_fast_dma_sel((u32)edma_id);
            break;
        default:
            break;
     }
}


/*******************************************************************************
  函数名:      s32 bsp_edma_channel_init (enum edma_req_id request,
                          channel_isr channel_isr, u32 channel_arg,
                          u32 int_flag)
  函数描述:    根据外设号分配通道，注册通道中断回调函数、初始化传输完成信号量、
               将外设号写入config寄存器
  输入参数:    req : 外设请求号
               channel_isr : 上层模块注册的EDMA通道中断处理函数，NULL时表明不注册
               channel_arg : channel_isr的入参1，
                             channel_isr为NULL，不需要设置这个参数
               int_flag : channel_isr的入参2, 产生的中断类型，取值范围为
                        EDMA_INT_DONE、EDMA_INT_LLT_DONE、
                        EDMA_INT_CONFIG_ERR、EDMA_INT_TRANSFER_ERR、
                        EDMA_INT_READ_ERR之一，或者组合。
                        channel_isr为NULL，不需要设置这个参数
  输出参数:    无
  返回值:      成功：通道号
               失败：负数
*******************************************************************************/
s32 bsp_edma_channel_init (enum edma_req_id request, channel_isr pFunc, u32 channel_arg, u32 int_flag)
{
    s32 channel_id = -1;
    u32 edma_id = 0;
    u32 edma_id_chan_id = 0;
    struct edma_reg_struct * edma_reg_str = 0;

    if((u32)request>=EDMA_REQ_MAX)
    {
        hiedmac_trace(BSP_LOG_LEVEL_ERROR,"request= 0x%x is INVALID! \n",request);
        return channel_id;
    }
    else
    {
        channel_id = (s32)g_edma_chan_distri[request];
    }

    if ((channel_id >= 0)&&(channel_id<EDMA_CHANNEL_END))
    {
        edma_id = EDMA_ID(channel_id);
        edma_id_chan_id = (u32)EDMA_ID_CHAN_ID(channel_id);
        edma_reg_str = g_edma_drv_info.edma_reg_str[edma_id];
        edma_reg_str->stTransferConfig[edma_id_chan_id].lli_node_info.config = EDMAC_TRANSFER_CONFIG_REQUEST(request);
        edma_set_peri_dma_sel(request, (u32)channel_id);
        
        if(pFunc)
        {
            edma_isr_install((u32)channel_id, pFunc, channel_arg, int_flag);
            (void)edma_channel_int_enable( channel_id );/*lint !e732*/
        }
        else
        {}
        return channel_id;
    }
    else
    {
         hiedmac_trace(BSP_LOG_LEVEL_ERROR," request= 0x%x FAILED, no chan to give! \n",request);
         return EDMA_FAIL;
    }
}

/*******************************************************************************
  函数名:       s32 bsp_edma_current_transfer_address(u32 channel_id)
  函数描述:     获得某通道当前传输的内存地址
  输入参数:     channel_id : 通道ID，调用 bsp_edma_channel_init函数的返回值
  输出参数:     无
  返回值:       成功：通道当前传输的内存地址
                失败：-1
*******************************************************************************/
s32 bsp_edma_current_transfer_address(u32 channel_id)
{
    u32 get_addr_type = 0;
    s32 ret_addr = -1;
    s32 edma_id = -1;
    s32 edma_id_chan_id = -1;
    struct edma_reg_struct * edma_reg_str = 0;
    edma_id = EDMA_ID(channel_id);
    edma_id_chan_id = (s32)EDMA_ID_CHAN_ID(channel_id);
    edma_reg_str = g_edma_drv_info.edma_reg_str[edma_id];
    if(!edma_id_valid(edma_id))
    {
        hiedmac_trace(BSP_LOG_LEVEL_ERROR, "channel_id %d not supported \n", channel_id);
        return EDMA_FAIL;
    }

    /*查询config寄存器的高两位，源或目的地址为内存时为1*/
    if (channel_id < EDMA_CHANNEL_END)
    {
        get_addr_type = EDMAC_TRANSFER_CONFIG_BOTH_INC\
                        & edma_reg_str->stTransferConfig[edma_id_chan_id].lli_node_info.config ;
        switch (get_addr_type)
        {
            case (EDMAC_TRANSFER_CONFIG_DEST_INC|EDMAC_TRANSFER_CONFIG_SOUR_INC):
            case EDMAC_TRANSFER_CONFIG_SOUR_INC:
                ret_addr = (s32)(edma_reg_str->stCurrStatusReg[edma_id_chan_id].ulCxCurrSrcAddr);
                break;
            case EDMAC_TRANSFER_CONFIG_DEST_INC:
                ret_addr = (s32)(edma_reg_str->stCurrStatusReg[edma_id_chan_id].ulCxCurrDesAddr);
                break;
            default:
                break;
        }
    }
    return ret_addr;
}


/*******************************************************************************
  函数名:       u64 bsp_edma_current_cnt(u32 channel_id)
  函数描述:     获得某通道传输剩余 size
                (二维传输最大2G 若不再支持3维，可不使用 u64  )
  输入参数:     channel_id : 通道ID，调用 bsp_edma_channel_init函数的返回值
  输出参数:     无
  返回值:       成功：获得某通道传输剩余 size
                失败：-1
*******************************************************************************/
u64 bsp_edma_current_cnt(u32 channel_id)
{
    u64 size = 0;/*lint !e958*/
    u32 chan_cnt0, chan_cnt1;
    u32 a_count,b_count,c_count;

    u32 chan_curr_cnt0, chan_curr_cnt1;
    u32 curr_a_count, curr_b_count;
    u32 curr_c_count;

    s32 edma_id = -1;
    s32 edma_id_chan_id = -1;
    struct edma_reg_struct * edma_reg_str = 0;
    volatile struct edma_cb * lli_node_tmp = 0;

    edma_id = EDMA_ID(channel_id);
    if(!edma_id_valid(edma_id))
    {
        hiedmac_trace(BSP_LOG_LEVEL_ERROR, "channel_id %d not supported \n", channel_id);
        return (u64)EDMA_FAIL;
    }
    edma_id_chan_id = (s32)EDMA_ID_CHAN_ID(channel_id);
    edma_reg_str = g_edma_drv_info.edma_reg_str[edma_id];

    lli_node_tmp = &(edma_reg_str->stTransferConfig[edma_id_chan_id].lli_node_info);

    if (channel_id >= EDMA_CHANNEL_END)
    {
        hiedmac_trace(BSP_LOG_LEVEL_ERROR," ----channel ID= %d, WRONG \n",  channel_id );
        return (u64)EDMA_FAIL;
    }

    /* read the config cnt */
    chan_cnt0 = lli_node_tmp->cnt0;
    chan_cnt1 = lli_node_tmp->cnt1;
    a_count = (chan_cnt0&0xFFFF);
    b_count = (chan_cnt0&0xFFFF0000)>>16;
    c_count = (chan_cnt1&0xFFFF);
    hiedmac_trace(BSP_LOG_LEVEL_DEBUG, \
                  "a_count:0x%x, b_count:0x%x, c_count:0x%x \n",\
                   a_count, b_count, c_count);

    /* read curr cnt */
    chan_curr_cnt0 = (u32)edma_reg_str->stCurrStatusReg[edma_id_chan_id].ulCxCurrCnt0;
    chan_curr_cnt1 = (u32)edma_reg_str->stCurrStatusReg[edma_id_chan_id].ulCxCurrCnt1;
    curr_a_count = (chan_curr_cnt0&0xFFFF);
    curr_b_count = (chan_curr_cnt0&0xFFFF0000)>>16;
    curr_c_count = (chan_curr_cnt1&0xFFFF);
    hiedmac_trace(BSP_LOG_LEVEL_DEBUG, \
                  "curr_a_count:0x%x, curr_b_count:0x%x, curr_c_count:0x%x \n",\
                   curr_a_count, curr_b_count, curr_c_count);

    /*compute the size left*/
    hiedmac_trace(BSP_LOG_LEVEL_DEBUG, \
                  "curr_count_1vec:0x%x, curr_count_2vec:0x%x, curr_count_3vec:0x%llx \n",\
                   curr_a_count, (a_count*curr_b_count), (a_count*b_count*curr_c_count));

    size = (u64)(curr_a_count) /* (long): Warning 647: (Warning -- Suspicious truncation)*/
          +(u64)(a_count*curr_b_count)
          +(a_count*b_count*curr_c_count); /*lint !e647*/
    hiedmac_trace(BSP_LOG_LEVEL_DEBUG, \
                  " ------------ size:0x%llx, \n",size);
    return size;
}

/*******************************************************************************
  函数名:       s32 bsp_edma_channel_stop(u32 channel_id)
  函数描述:     停止指定的EDMA通道
  输入参数:     channel_id : 通道ID，调用 bsp_edma_channel_init函数的返回值
  输出参数:     无
  返回值:       成功：通道当前传输的内存地址
                失败：-1
*******************************************************************************/
s32 bsp_edma_channel_stop(u32 channel_id)
{
    s32 curr_addr = 0;
    s32 edma_id = -1;
    s32 edma_id_chan_id = -1;
    struct edma_reg_struct * edma_reg_str = 0;

    edma_id = EDMA_ID(channel_id);
    if(!edma_id_valid(edma_id))
    {
        hiedmac_trace(BSP_LOG_LEVEL_ERROR, "channel_id %d not supported \n", channel_id);
        return EDMA_FAIL;
    }
    edma_id_chan_id = (s32)EDMA_ID_CHAN_ID(channel_id);
    edma_reg_str = g_edma_drv_info.edma_reg_str[edma_id];

    if (channel_id < EDMA_CHANNEL_END)
    {
        edma_reg_str->stTransferConfig[edma_id_chan_id].lli_node_info.config &= ~(EDMA_CHANNEL_ENABLE);
        curr_addr = bsp_edma_current_transfer_address(channel_id);
        return curr_addr;
    }
    else
    {
	    hiedmac_trace(BSP_LOG_LEVEL_ERROR, "Channel id is wrong!\n");
        return EDMA_FAIL;
    }
}

/*******************************************************************************
  函数名:       s32 bsp_edma_channel_is_idle (u32 channel_id)
  函数描述:     查询EDMA通道是否空闲
  输入参数:     channel_id : 通道ID，调用 bsp_edma_channel_init函数的返回值
  输出参数:     无
  返回值:       0 : 通道忙碌
                1 : 通道空闲
                负数 : 失败
*******************************************************************************/
s32 bsp_edma_channel_is_idle (u32 channel_id)
{
    u32  ChannelState = 0;
    s32 edma_id = -1;
    s32 edma_id_chan_id = -1;
    struct edma_reg_struct * edma_reg_str = 0;

    edma_id = EDMA_ID(channel_id);
    if(!edma_id_valid(edma_id))
    {
        hiedmac_trace(BSP_LOG_LEVEL_ERROR, "channel_id %d not supported \n", channel_id);
        return EDMA_FAIL;
    }
    edma_id_chan_id = (s32)EDMA_ID_CHAN_ID(channel_id);
    edma_reg_str = g_edma_drv_info.edma_reg_str[edma_id];

    if (channel_id < (EDMA_CHANNEL_END))
    {
        ChannelState = edma_reg_str->ulChannelState;
        if ((1UL << edma_id_chan_id) & ChannelState)
        {
            return EDMA_CHN_BUSY;    /* The channel is active    */
        }
        else
        {
            return EDMA_CHN_FREE;    /* The channel is idle      */
        }
    }
    else
    {
        hiedmac_trace(BSP_LOG_LEVEL_ERROR, "channel :%d is NOT VALID \n", channel_id);
        return EDMA_CHANNEL_INVALID;
    }
}

/*******************************************************************************
  函数名:       s32 bsp_edma_chanmap_is_idle (u32 channel_map)
  函数描述:     查询EDMA的几个通道是否空闲
  输入参数:     channel_map : 通道IDs bit X 为1 表示查询 x通道
  输出参数:     无
  返回值:       0 : 有通道忙碌
                1 : 查询的所有通道空闲
                负数 : 失败
*******************************************************************************/
s32 bsp_edma_chanmap_is_idle (u32 channel_map)
{
    u32  chan_stat_0 = 0;
    u32  chan_stat_1 = 0;
    struct edma_reg_struct * edma_reg_str_0 = 0;
    struct edma_reg_struct * edma_reg_str_1 = 0;

    edma_reg_str_0 = g_edma_drv_info.edma_reg_str[EDMA_CH16_ID];
    edma_reg_str_1 = g_edma_drv_info.edma_reg_str[EDMA_CH4_ID];

    if ((channel_map>=(0x1<<EDMA_CHANNEL_START))&&(channel_map < (0x1<<EDMA_CHANNEL_END)))
    {
        chan_stat_0 = edma_reg_str_0->ulChannelState;
        chan_stat_1 = edma_reg_str_1->ulChannelState;
        if ((channel_map) & (chan_stat_0|(chan_stat_1<<16)))
        {
            return EDMA_CHN_BUSY;
        }
        else
        {
            return EDMA_CHN_FREE;
        }
    }
    else
    {
        hiedmac_trace(BSP_LOG_LEVEL_ERROR, "channel_map :0x%x is NOT VALID \n", channel_map);
        return EDMA_CHANNEL_INVALID;
    }
}


/*******************************************************************************
  函数名:       s32 bsp_edma_channel_free (u32 channel_id)
  函数描述:     用于释放共享通道。主要工作:去使能通道中断，清零isr
  输入参数:     channel_id : 通道ID，调用 bsp_edma_channel_init函数的返回值
  输出参数:     无
  返回值:       0 : 成功释放
                负数 : 失败
*******************************************************************************/
s32 bsp_edma_channel_free (u32 channel_id)
{

    u32 chan_index = 0;
    s32 edma_id = -1;
    s32 edma_id_chan_id = -1;

    struct edma_id_int_isr *edma_int_isr = 0;

    edma_id = EDMA_ID(channel_id);
    if(!edma_id_valid(edma_id))
    {
        hiedmac_trace(BSP_LOG_LEVEL_ERROR, "channel_id %d not supported \n", channel_id);
        return EDMA_FAIL;
    }
    edma_id_chan_id = (s32)EDMA_ID_CHAN_ID(channel_id);

    edma_int_isr = &(g_edma_drv_info.edma_int_isr[edma_id]);

    if (channel_id >= EDMA_CHANNEL_END)
    {
        hiedmac_trace(BSP_LOG_LEVEL_ERROR," ----channel ID= %d, WRONG \n",  channel_id );
        return EDMA_FAIL;
    }
    else
    {
        (void)edma_channel_int_disable(channel_id);
        chan_index = (u32)edma_id_chan_id;
        /*do not need lock when freed */
        edma_int_isr->edma_trans1_isr[chan_index].chan_isr = NULL;
        edma_int_isr->edma_trans1_isr[chan_index].chan_arg = 0;
        edma_int_isr->edma_trans1_isr[chan_index].int_status = 0;

        edma_int_isr->edma_trans2_isr[chan_index].chan_isr = NULL;
        edma_int_isr->edma_trans2_isr[chan_index].chan_arg = 0;
        edma_int_isr->edma_trans2_isr[chan_index].int_status = 0;

        edma_int_isr->edma_err1_isr[chan_index].chan_isr = NULL;
        edma_int_isr->edma_err1_isr[chan_index].chan_arg = 0;
        edma_int_isr->edma_err1_isr[chan_index].int_status = 0;

        edma_int_isr->edma_err2_isr[chan_index].chan_isr = NULL;
        edma_int_isr->edma_err2_isr[chan_index].chan_arg = 0;
        edma_int_isr->edma_err2_isr[chan_index].int_status = 0;

        edma_int_isr->edma_err3_isr[chan_index].chan_isr = NULL;
        edma_int_isr->edma_err3_isr[chan_index].chan_arg = 0;
        edma_int_isr->edma_err3_isr[chan_index].int_status = 0;

        hiedmac_trace(BSP_LOG_LEVEL_DEBUG," ----channel %d is freed \n",  channel_id );
        return EDMA_SUCCESS;
    }
}

/*  特殊打印：包括中断服务程序中的打印 和 低级别的打印
    默认不执行 bsp_trace， 可调用 set_edma_irq_trace 1 打开 */
u32 edma_irq_trace = 0;
void set_edma_irq_trace(u32 irq_trace_flag)
{
    edma_irq_trace = irq_trace_flag;
}
#define hiedmac_irq_trace(level, fmt, ...) do{ \
    if(edma_irq_trace)\
        (bsp_trace(level, BSP_MODU_EDMA, "[edma]:<%s> <%d> "fmt, __FUNCTION__,__LINE__, ##__VA_ARGS__)); \
}while(0)

/*******************************************************************************
  函数名:       static void edma_execute_channel_isr( chan_int_service *int_serve )
  函数描述:     对中断判断完成后 [哪个通道哪种中断]，执行具体的中断处理函数。
  输入参数:     int_serve : chan_int_service结构体的 isr, 包含具体的执行函数和参数
  输出参数:     无
  返回值:       无
*******************************************************************************/
static void edma_execute_channel_isr(struct chan_int_service *int_serve , u32 int_flag)
{
    if(int_serve->chan_isr != NULL)
    {
        hiedmac_irq_trace(BSP_LOG_LEVEL_DEBUG,"chan_isr is executed !\n");
        (int_serve->chan_isr)(int_serve->chan_arg, int_flag);
    }
    else
    {
        hiedmac_irq_trace(BSP_LOG_LEVEL_DEBUG,"chan_isr is NULL, no irq executed\n");
    }
}


/*******************************************************************************
  函数名:       static void edma_isr_channel(u32 channel_id)
  函数描述:     对中断判断的通道后，该通道的中断处理。
  输入参数:     channel_id : 产生中断的通道
  输出参数:     无
  返回值:       无
*******************************************************************************/
/*lint -save -e438*/
/* Warning 438: (Warning -- Last value assigned   not used) */
static void edma_isr_channel(u32 edma_id_chan_id, u32 edma_id, u32 int_stat)
{
    u32 int_tc1    = 0;
    u32 int_tc2    = 0;
    u32 int_err1   = 0;
    u32 int_err2   = 0;
    u32 int_err3   = 0;
    u32 current_bit = 0;
    u32 chan_index = 0;
    u32 channel_id = 0;

    UNUSED(int_stat);

    struct edma_reg_struct * edma_reg_str = 0;
    struct edma_id_int_isr* edma_id_int_isr;

    edma_reg_str = g_edma_drv_info.edma_reg_str[edma_id];
    edma_id_int_isr = &(g_edma_drv_info.edma_int_isr[edma_id]);

    int_tc1    = edma_reg_str->stCpuXReg[EDMAC_DT_ARM].ulIntTC1;
    int_tc2    = edma_reg_str->stCpuXReg[EDMAC_DT_ARM].ulIntTC2;
    int_err1   = edma_reg_str->stCpuXReg[EDMAC_DT_ARM].ulIntErr1;
    int_err2   = edma_reg_str->stCpuXReg[EDMAC_DT_ARM].ulIntErr2;
    int_err3   = edma_reg_str->stCpuXReg[EDMAC_DT_ARM].ulIntErr3;

    current_bit = (u32)0x1<<edma_id_chan_id;
    chan_index = edma_id_chan_id;
    channel_id = ( (1==edma_id) ? (edma_id_chan_id+16) : edma_id_chan_id );

    if (int_tc1 & current_bit)
    {
        hiedmac_irq_trace(BSP_LOG_LEVEL_DEBUG, "channel_id %d channel transfer success!\n",channel_id);
        /* Clear TC / Link_TC Interrupt */
        edma_reg_str->ulIntTC1Raw = current_bit;
        edma_execute_channel_isr(&(edma_id_int_isr->edma_trans1_isr[chan_index]), EDMA_INT_DONE);
    }
    else if (int_tc2 & current_bit)
    {
		hiedmac_irq_trace(BSP_LOG_LEVEL_DEBUG, "channel_id %d LLI transfer success!\n",channel_id);
        /* Clear LLI TC / Link_TC Interrupt */
        edma_reg_str->ulIntTC2Raw = current_bit;
        edma_execute_channel_isr(&(edma_id_int_isr->edma_trans2_isr[chan_index]), EDMA_INT_LLT_DONE);
	}
    else if (int_err1 & current_bit)
    {
		hiedmac_irq_trace(BSP_LOG_LEVEL_DEBUG, "channel_id %d transfer config_error1!\n", channel_id);
		/* Clear the Error / Link_Error interrupt */
		edma_reg_str->ulIntERR1Raw = current_bit;
        edma_execute_channel_isr(&(edma_id_int_isr->edma_err1_isr[chan_index]), EDMA_INT_CONFIG_ERR);
    }
	else if (int_err2 & current_bit)
	{
		hiedmac_irq_trace(BSP_LOG_LEVEL_DEBUG, "channel_id %d transfer data_error2\n", channel_id);
		edma_reg_str->ulIntERR2Raw        = current_bit;
        edma_execute_channel_isr(&(edma_id_int_isr->edma_err2_isr[chan_index]), EDMA_INT_TRANSFER_ERR);
	}
	else if (int_err3 & current_bit)
	{
        hiedmac_irq_trace(BSP_LOG_LEVEL_DEBUG, "channel_id %d transfer read_lli_error3!\n", channel_id);
		/* Clear the Error / Link_Error interrupt */
		edma_reg_str->ulIntERR3Raw    = current_bit;
        edma_execute_channel_isr(&(edma_id_int_isr->edma_err3_isr[chan_index]), EDMA_INT_READ_ERR);
    }
    else
    {
        hiedmac_irq_trace(BSP_LOG_LEVEL_WARNING,"Error in edma_isr!\n");
		return;
    }
}
/*lint -restore +e438*/
/*******************************************************************************
  函数名:       static irqreturn_t edma_isr(s32 irq, void *private)
  函数描述:     EDMA驱动程序中断服务程序，中断产生时被调用；函数判断是哪个
                通道发生了中断，调用相应通道的中断处理程序
  输入参数:     irq : 中断号，调用request_irq函数注册中断服务程序时的req参数值
                private : 调用request_irq函数注册中断服务程序时的dev参数值
  输出参数:     无
  返回值:       IRQ_HANDLED ：中断已经被处理
*******************************************************************************/
static OSL_IRQ_FUNC(irqreturn_t, edma_isr, irq, dev_id)
{
    s32 edma_id = -1;
    s32 edma_id_chan_id = -1;
    struct edma_reg_struct * edma_reg_str = 0;
    u32 int_stat = 0;
    u32 curr_bit = 0;

    edma_id = (s32)dev_id;
    edma_reg_str = g_edma_drv_info.edma_reg_str[edma_id];

    int_stat = edma_reg_str->stCpuXReg[EDMAC_DT_ARM].ulIntState;

    hiedmac_irq_trace(BSP_LOG_LEVEL_DEBUG," ----shouldn't be 0, ulChannel_Int_Status_Reg = 0x%X \n", int_stat);
    /* decide which channel has trigger the interrupt*/
    for (edma_id_chan_id = EDMA_CHANNEL_START; edma_id_chan_id < EDMA_CH16_NUM; edma_id_chan_id++)
    {
        curr_bit = 1UL << edma_id_chan_id;
        if (int_stat & curr_bit)
        {
            hiedmac_irq_trace(BSP_LOG_LEVEL_DEBUG," intterupt from channel %d \n", edma_id_chan_id);
            edma_isr_channel((u32)edma_id_chan_id, (u32)edma_id , int_stat);/* Handle channel_id interrupt */
        }
    }
	return IRQ_HANDLED;
}

/*******************************************************************************
  函数名:      s32 bsp_edma_channel_set_config (u32 channel_id,
                       u32 direction, u32 burst_width, u32 burst_len)
  函数描述:    非链式EDMA传输时，调用本函数配置通道参数。
               链式EDMA传输时，不需要使用本函数。
  输入参数:    channel_id : 通道ID，调用 bsp_edma_channel_init函数的返回值
               direction : EDMA传输方向, 取值为EDMA_P2M、EDMA_M2P、
                           EDMA_M2M之一
               burst_width：取值为0、1、2、3，表示的burst位宽为8、16、32、64bit
               burst_len：取值范围0~15，表示的burst长度为1~16
  输出参数:    无
  返回值:      成功：0
               失败：负数
*******************************************************************************/
s32 bsp_edma_channel_set_config (u32 channel_id, u32 direction, u32 burst_width, u32 burst_len)
{
    u32 chan_cfg = 0;
    s32 edma_id = -1;
    s32 edma_id_chan_id = -1;
    struct edma_reg_struct * edma_reg_str = 0;

    edma_id = EDMA_ID(channel_id);
    if(!edma_id_valid(edma_id))
    {
        hiedmac_trace(BSP_LOG_LEVEL_ERROR, "channel_id %d not supported \n", channel_id);
        return EDMA_FAIL;
    }
    edma_id_chan_id = (s32)EDMA_ID_CHAN_ID(channel_id);
    edma_reg_str = g_edma_drv_info.edma_reg_str[edma_id];

    chan_cfg = edma_reg_str->stTransferConfig[edma_id_chan_id].lli_node_info.config;
    if( (channel_id <  EDMA_CHANNEL_END) \
         && (burst_width <= EDMA_TRANS_WIDTH_64) \
         && (burst_len   <= EDMA_BUR_LEN_16) )
    {
        chan_cfg &= (u32)0x1F0; /*CX_CONFIG_PERI bit位 4-9, bit9暂未用*/
        switch (direction)
        {
            case EDMA_P2M:
                chan_cfg |= P2M_CONFIG;
                break;
            case EDMA_M2P:
                chan_cfg |= M2P_CONFIG;
                break;
            case EDMA_M2M:
                chan_cfg |= M2M_CONFIG;
                break;
            default:
                hiedmac_trace(BSP_LOG_LEVEL_ERROR,\
                    "ChannelConfing direction=%d, ERROR \n", direction);
                break;
        }
        chan_cfg |= EDMAC_BASIC_CONFIG(burst_width, burst_len);
        edma_reg_str->stTransferConfig[edma_id_chan_id].lli_node_info.config = chan_cfg;/*lint !e662 !e661*/
        hiedmac_trace(BSP_LOG_LEVEL_DEBUG,"stChannelConfing->config = 0x%X!\n", chan_cfg);
        return EDMA_SUCCESS;
    }
    else
    {
         hiedmac_trace(BSP_LOG_LEVEL_ERROR,\
                       "ChanConfing ERROR chan=%d, burstWidth=%d, Len=%d\n",\
                        channel_id, burst_width, burst_len);
         return EDMA_CONFIG_ERROR;
    }
}

/*******************************************************************************
  函数名:      int balong_dma_channel_dest_set_config (UINT32 channel_id,
                       UINT32 burst_width, UINT32 burst_len)
  函数描述:    BBP GRIF需要单独配置目的数据位宽和长度，调用本函数，其它情形不使用。
               使用时，该接口在balong_dma_channel_set_config之后调用。
  输入参数:    channel_id：通道ID，调用balong_dma_channel_init函数的返回值
               burst_width：取值为0、1、2、3，表示的burst位宽为8、16、32、64bit
               burst_len：取值范围0~15，表示的burst长度为1~16
  输出参数:    无
  返回值:      成功：0
               失败：负数
*******************************************************************************/
s32 bsp_edma_channel_dest_set_config (u32 channel_id, u32 dest_width, u32 dest_len)
{
    u32 chan_cfg = 0;
    s32 edma_id = -1;
    s32 edma_id_chan_id = -1;
    struct edma_reg_struct * edma_reg_str = 0;

    edma_id = EDMA_ID(channel_id);
    if(!edma_id_valid(edma_id))
    {
        hiedmac_trace(BSP_LOG_LEVEL_ERROR, "channel_id %d not supported \n", channel_id);
        return EDMA_FAIL;
    }
    edma_id_chan_id = (s32)EDMA_ID_CHAN_ID(channel_id);
    edma_reg_str = g_edma_drv_info.edma_reg_str[edma_id];

    chan_cfg = edma_reg_str->stTransferConfig[edma_id_chan_id].lli_node_info.config;

    /* 重新配置目的数据burst位宽和长度,以提高存数性能 */
    chan_cfg &=(~(EDMAC_TRANSFER_CONFIG_DEST_WIDTH_MASK|EDMAC_TRANSFER_CONFIG_DEST_BURST_LENGTH_MASK));
    chan_cfg |= EDMAC_TRANSFER_CONFIG_DEST_WIDTH(dest_width);
    chan_cfg |= EDMAC_TRANSFER_CONFIG_DEST_BURST_LENGTH(dest_len);

    edma_reg_str->stTransferConfig[edma_id_chan_id].lli_node_info.config = chan_cfg;

    return EDMA_SUCCESS;
}

#ifdef __VXWORKS__
extern int sysClkRateGet(void);
static void msleep(int x)
{
    (void)taskDelay(sysClkRateGet()*x/1000+1);
}
#endif

/*******************************************************************************
  函数名:      s32 bsp_edma_channel_start (u32 channel_id, u32 src_addr,
                       u32 des_addr, u32 len)
  函数描述:    启动一次同步EDMA传输, EDMA传输完成后，才返回
               使用本函数时，不需要注册中断处理函数
               len<65536(64K):一维传输
               65536(64K)<=len<16384*65536(1G)时:二维传输
               len>16384*65536:不支持
               二维传输len为16K整数倍，非整数倍将不支持

  输入参数:    channel_id：通道ID,调用 bsp_edma_channel_init函数的返回值
               src_addr：数据传输源地址，必须是物理地址
               des_addr：数据传输目的地址，必须是物理地址
               len：数据传输长度，单位：字节；一次传输数据的最大长度是65535字节
  输出参数:    无
  返回值:      成功：0
               失败：负数
*******************************************************************************/
s32 bsp_edma_channel_start (u32 channel_id, u32 src_addr, u32 des_addr, u32 len)
{
    u32 reg_value = 0;
    u32 loop_index = 0;
    u32 wait_loop = EDMA_DATA_TIMEOUT;

    u32 bindx = 0;
    u32 cnt0 = 0;

    u32 b_count = 0;
    u32 a_count = 0;

    s32 edma_id = -1;
    s32 edma_id_chan_id = -1;
    struct edma_reg_struct * edma_reg_str = 0;
    volatile struct edma_cb * lli_node_tmp = 0;

    edma_id = EDMA_ID(channel_id);
    if(!edma_id_valid(edma_id))
    {
        hiedmac_trace(BSP_LOG_LEVEL_ERROR, "channel_id %d not supported \n", channel_id);
        return EDMA_FAIL;
    }
    edma_id_chan_id = (s32)EDMA_ID_CHAN_ID(channel_id);
    edma_reg_str = g_edma_drv_info.edma_reg_str[edma_id];

    hiedmac_irq_trace(BSP_LOG_LEVEL_DEBUG,"--------------------------edma_start!\n");
    reg_value = edma_reg_str->stTransferConfig[edma_id_chan_id].lli_node_info.config;
    hiedmac_irq_trace(BSP_LOG_LEVEL_DEBUG,"---------pre stChannelConfing->config = 0x%X\n",reg_value);

    if(len >= SZ_64K)  /* 2 vec  -  align_size = 16k */
    {
        b_count = (!(len%SZ_16K)) ? (len/SZ_16K - 1) \
                : (!(len%SZ_8K )) ? (len/SZ_8K  - 1) \
                : (!(len%SZ_4K )) ? (len/SZ_4K  - 1) : 0;
        if(0==b_count)
        {
            hiedmac_trace(BSP_LOG_LEVEL_ERROR,"try 2vec failed! not aligned. channel:%d size:0x%x \n",channel_id ,len);
            return EDMA_TRXFERSIZE_INVALID;
        }
        else
        {
            a_count = len / (b_count+1);
            bindx = (((reg_value & EDMAC_TRANSFER_CONFIG_SOUR_INC) ? (a_count << 16) : 0) | \
                     ((reg_value & EDMAC_TRANSFER_CONFIG_DEST_INC) ? a_count : 0));
            cnt0  =  (b_count<<16)|a_count;
            wait_loop *= 2;
        }
    }
    else    /* 1 vec  -  len < 64k */
    {
        cnt0 = len;
    }

    if (channel_id >= EDMA_CHANNEL_END)
    {
        hiedmac_trace(BSP_LOG_LEVEL_ERROR,"EDMA_CHANNEL_INVALID!\n");
        return EDMA_CHANNEL_INVALID;
    }
    else
    {
        lli_node_tmp = &(edma_reg_str->stTransferConfig[edma_id_chan_id].lli_node_info);
        lli_node_tmp->config  = EDMA_CHANNEL_DISABLE;

        lli_node_tmp->lli       = 0;
        lli_node_tmp->bindx     = bindx;
        lli_node_tmp->cindx     = 0;
        lli_node_tmp->cnt1      = 0;
        lli_node_tmp->cnt0      = cnt0;
        lli_node_tmp->src_addr  = src_addr;
        lli_node_tmp->des_addr  = des_addr;
        edma_cache_sync();
        lli_node_tmp->config   = reg_value | EDMA_CHANNEL_ENABLE;
        edma_cache_sync();
        reg_value = lli_node_tmp->config;
        hiedmac_irq_trace(BSP_LOG_LEVEL_DEBUG,"--------later stChannelConfing->config = 0x%X\n",reg_value);

        for( loop_index=0; loop_index<wait_loop; loop_index++ )
        {
            msleep(1);
            if ( bsp_edma_channel_is_idle(channel_id) )
            {
                break;
            }
        }
        if(wait_loop==loop_index)
        {
            hiedmac_irq_trace(BSP_LOG_LEVEL_WARNING,"edma_start trans is timeout!\n");
            return EDMA_TRXFER_ERROR;
        }
        else
        {
            hiedmac_trace(BSP_LOG_LEVEL_DEBUG,"edma_start trans_complete!\n");
            return EDMA_SUCCESS;
        }
    }
}



/*******************************************************************************
  函数名:      s32 bsp_edma_channel_2vec_start (u32 channel_id, u32 src_addr,
                       u32 des_addr, u32 len)
  函数描述:    用于dsp 大块数据的传输。启动一次同步--2维--EDMA传输, EDMA传输完成后，才返回
               使用本函数时，不需要注册中断处理函数
  输入参数:    channel_id：通道ID,调用 bsp_edma_channel_init函数的返回值
               src_addr：数据传输源地址，必须是物理地址
               des_addr：数据传输目的地址，必须是物理地址
               len：数据传输长度，单位：字节；可用于传输大于65535字节的数据。
               size_align: 二维传输时 要求 按 size_align 对齐， 且 len/size_align < 0x10000。
               ***建议，使用 2维传输时，set_config建议 burst_width-64bit burst_len-16
  输出参数:    无
  返回值:      成功：0
               失败：负数
*******************************************************************************/

s32 bsp_edma_channel_2vec_start (u32 channel_id, u32 src_addr, u32 des_addr, u32 len, u32 size_align)
{
    u32 edma_ret = 0;
    u32 reg_value = 0;
    u32 array_cnt = 0;
    u32 bindx_set = 0;
    s32 edma_id = -1;
    s32 edma_id_chan_id = -1;
    struct edma_reg_struct * edma_reg_str = 0;
    volatile struct edma_cb * lli_node_tmp = 0;

    edma_id = EDMA_ID(channel_id);
    if(!edma_id_valid(edma_id))
    {
        hiedmac_trace(BSP_LOG_LEVEL_ERROR, "channel_id %d not supported \n", channel_id);
        return EDMA_FAIL;
    }
    edma_id_chan_id = (s32)EDMA_ID_CHAN_ID(channel_id);
    edma_reg_str = g_edma_drv_info.edma_reg_str[edma_id];
    lli_node_tmp = &(edma_reg_str->stTransferConfig[edma_id_chan_id].lli_node_info);

    if((size_align>=0x8000)||(0==size_align))
    {
        hiedmac_trace(BSP_LOG_LEVEL_ERROR," input size_align=0x%X is invalide. \n",size_align);
        return ERROR;
    }
    edma_ret = len%size_align;
    if( 0 != edma_ret)
    {
        hiedmac_trace(BSP_LOG_LEVEL_ERROR,"input len=0x%X not aligned . \n",len);
        return ERROR;
    }

    array_cnt = (len/size_align)-1;
    if( 0x10000 <= array_cnt)
    {
        hiedmac_trace(BSP_LOG_LEVEL_ERROR,"input len=0x%X too big, try increase size_align. \n",len);
        return ERROR;
    }

    hiedmac_trace(BSP_LOG_LEVEL_DEBUG,"--------------------------edma_2vec_start!\n");
    reg_value = lli_node_tmp->config;
    hiedmac_trace(BSP_LOG_LEVEL_DEBUG,"---------pre stChannelConfing->config = 0x%X\n",reg_value);

    /* if src_index is inc, src_bindx should be set to size_align;
       if not, src_bindx is set to 0 */
    if(reg_value&EDMAC_TRANSFER_CONFIG_SOUR_INC)
    {
        bindx_set = bindx_set|(size_align<<16);
    }
    if(reg_value&EDMAC_TRANSFER_CONFIG_DEST_INC)
    {
        bindx_set = bindx_set|(size_align);
    }

    if (channel_id >= EDMA_CHANNEL_END)
    {
        hiedmac_trace(BSP_LOG_LEVEL_ERROR,"EDMA_CHANNEL_INVALID!\n");
        return EDMA_CHANNEL_INVALID;
    }
    else
    {
        lli_node_tmp->config  = EDMA_CHANNEL_DISABLE;

        lli_node_tmp->lli       = 0;
        lli_node_tmp->bindx     = bindx_set;
        lli_node_tmp->cindx     = 0;
        lli_node_tmp->cnt1      = 0;
        lli_node_tmp->cnt0      = (array_cnt<<16|size_align);
        lli_node_tmp->src_addr  = src_addr;
        lli_node_tmp->des_addr  = des_addr;
        edma_cache_sync();
        lli_node_tmp->config   = reg_value | EDMA_CHANNEL_ENABLE;
        edma_cache_sync();
        reg_value = lli_node_tmp->config;
        hiedmac_trace(BSP_LOG_LEVEL_DEBUG,"--------later stChannelConfing->config = 0x%X\n",reg_value);

        return EDMA_SUCCESS;
    }
}

/*******************************************************************************
  函数名:      s32 bsp_edma_channel_async_start (u32 channel_id,
                u32 src_addr, u32 des_addr, u32 len)
  函数描述:    启动一次异步EDMA传输。启动EDMA传输后，就返回。不等待EDMA传输完成。
               使用本函数时，注册中断处理函数，中断处理函数中处理EDMA传输完成事件
               或者，不注册中断处理函数，使用 bsp_edma_channel_is_idle函数查询
               EDMA传输是否完成
  输入参数:    channel_id：通道ID,调用 bsp_edma_channel_init函数的返回值
               src_addr：数据传输源地址，必须是物理地址
               des_addr：数据传输目的地址，必须是物理地址
               len：数据传输长度，单位：字节；一次传输数据的最大长度是65535字节
  输出参数:    无
  返回值:      成功：0
               失败：负数
*******************************************************************************/
s32 bsp_edma_channel_async_start (u32 channel_id, u32 src_addr, u32 des_addr, u32 len)
{
    u32 reg_value = 0;
    s32 edma_id = -1;
    s32 edma_id_chan_id = -1;
    struct edma_reg_struct * edma_reg_str = 0;
    volatile struct edma_cb * lli_node_tmp = 0;


    edma_id = EDMA_ID(channel_id);
    if(!edma_id_valid(edma_id))
    {
        hiedmac_trace(BSP_LOG_LEVEL_ERROR, "channel_id %d not supported \n", channel_id);
        return EDMA_FAIL;
    }
    edma_id_chan_id = (s32)EDMA_ID_CHAN_ID(channel_id);
    edma_reg_str = g_edma_drv_info.edma_reg_str[edma_id];
    lli_node_tmp = &(edma_reg_str->stTransferConfig[edma_id_chan_id].lli_node_info);

    hiedmac_trace(BSP_LOG_LEVEL_DEBUG,"--------------------------edma_start!\n");
    reg_value = lli_node_tmp->config;
    hiedmac_trace(BSP_LOG_LEVEL_DEBUG,"---------pre stChannelConfing->config = 0x%X\n",reg_value);

    if (channel_id >= EDMA_CHANNEL_END)
    {
        hiedmac_trace(BSP_LOG_LEVEL_ERROR,"EDMA_CHANNEL_INVALID\n");
        return EDMA_CHANNEL_INVALID;
    }
    else
    {
        lli_node_tmp->config  = EDMA_CHANNEL_DISABLE;

        lli_node_tmp->lli       = 0;
        lli_node_tmp->bindx     = 0;
        lli_node_tmp->cindx     = 0;
        lli_node_tmp->cnt1      = 0;
        lli_node_tmp->cnt0      = len;
        lli_node_tmp->src_addr  = src_addr;
        lli_node_tmp->des_addr  = des_addr;
        edma_cache_sync();
        lli_node_tmp->config   = reg_value | EDMA_CHANNEL_ENABLE;
        edma_cache_sync();

        return EDMA_SUCCESS;
    }
}

/*******************************************************************************
  函数名:      struct edma_cb *bsp_edma_channel_get_lli_addr(u32 channel_id)
  函数描述:    获取指定EDMA通道的链表控制块的起始地址
  输入参数:    channel_id：通道ID,调用 bsp_edma_channel_init函数的返回值
  输出参数:    无
  返回值:      成功：链表控制块起始地址
               失败：0
*******************************************************************************/
struct edma_cb *bsp_edma_channel_get_lli_addr (u32 channel_id)
{
    s32 edma_id = -1;
    s32 edma_id_chan_id = -1;
    struct edma_reg_struct * edma_reg_str = 0;

    edma_id = EDMA_ID(channel_id);
    if(!edma_id_valid(edma_id))
    {
        hiedmac_trace(BSP_LOG_LEVEL_ERROR, "channel_id %d not supported \n", channel_id);
        return NULL;
    }
    edma_id_chan_id = (s32)EDMA_ID_CHAN_ID(channel_id);
    edma_reg_str = g_edma_drv_info.edma_reg_str[edma_id];

    if (channel_id >= EDMA_CHANNEL_END)
    {
        hiedmac_trace(BSP_LOG_LEVEL_ERROR,"EDMA_CHANNEL_INVALID\n");
        return NULL;
    }
    else
    {
        /*warning : return discards 'volatile' qualifier*/
        return (struct edma_cb *)&(edma_reg_str->stTransferConfig[edma_id_chan_id].lli_node_info);
    }
}

/*******************************************************************************
  函数名:      s32 bsp_edma_channel_lli_start (u32 channel_id)
  函数描述:    启动链式EDMA传输。在链式EDMA的所有节点传输都全部完成后才返回。
               链式EDMA的每个节点的数据最大传输长度为65535字节。
               注意：调用此函数前，必须设置好链表控制块。
  输入参数:    channel_id：通道ID,调用 bsp_edma_channel_init函数的返回值
  输出参数:    无
  返回值:      成功：0
               失败：负数
*******************************************************************************/
s32 bsp_edma_channel_lli_start (u32 channel_id)
{
    u32 reg_value = 0;
    u32 loop_index = 0;
    u32 wait_loop = EDMA_DATA_TIMEOUT;

    s32 edma_id = -1;
    s32 edma_id_chan_id = -1;
    struct edma_reg_struct * edma_reg_str = 0;
    volatile struct edma_cb * lli_node_tmp = 0;

    edma_id = EDMA_ID(channel_id);
    if(!edma_id_valid(edma_id))
    {
        hiedmac_trace(BSP_LOG_LEVEL_ERROR, "channel_id %d not supported \n", channel_id);
        return EDMA_FAIL;
    }
    edma_id_chan_id = (s32)EDMA_ID_CHAN_ID(channel_id);
    edma_reg_str = g_edma_drv_info.edma_reg_str[edma_id];
    lli_node_tmp = &(edma_reg_str->stTransferConfig[edma_id_chan_id].lli_node_info);


    hiedmac_trace(BSP_LOG_LEVEL_DEBUG,"--------------------------edma_start!\n");
    reg_value = lli_node_tmp->config;
    hiedmac_trace(BSP_LOG_LEVEL_DEBUG,"-----------pre_stChannelConfing->config = 0x%X\n",reg_value);
    if (channel_id >= EDMA_CHANNEL_END)
    {
        hiedmac_trace(BSP_LOG_LEVEL_ERROR,"EDMA_CHANNEL_INVALID\n");
        return EDMA_CHANNEL_INVALID;
    }
    else
    {
        edma_cache_sync();
        lli_node_tmp->config = reg_value | EDMA_CHANNEL_ENABLE;
        edma_cache_sync();

        reg_value = lli_node_tmp->config;
        hiedmac_trace(BSP_LOG_LEVEL_DEBUG,"---------later stChannelConfing->config = 0x%X\n",reg_value);

        for(loop_index=0;loop_index<wait_loop;loop_index++)
        {
            msleep(1);
            if (bsp_edma_channel_is_idle(channel_id))
            {
                break;
            }
        }
        if(wait_loop==loop_index)
        {
            hiedmac_trace(BSP_LOG_LEVEL_ERROR,"edma_lli_start trans is timeout!\n");
            return EDMA_TRXFER_ERROR;
        }
        else
        {
            hiedmac_irq_trace(BSP_LOG_LEVEL_DEBUG,"edma_lli_start trans_complete!\n");
            return EDMA_SUCCESS;
        }
    }
}

/*******************************************************************************
  函数名:      s32 bsp_edma_channel_lli_start (u32 channel_id)
  函数描述:    启动链式EDMA传输，然后立即返回，不等待EDMA传输完成。
               链式EDMA的每个节点的数据最大传输长度为65535字节。
               注意：调用此函数前，必须设置好链表控制块。
  输入参数:    channel_id：通道ID,调用 bsp_edma_channel_init函数的返回值
  输出参数:    无
  返回值:      成功：0
               失败：负数
*******************************************************************************/
s32 bsp_edma_channel_lli_async_start (u32 channel_id)
{
    u32 reg_value = 0;
    s32 edma_id = -1;
    s32 edma_id_chan_id = -1;
    struct edma_reg_struct * edma_reg_str = 0;
    volatile struct edma_cb * lli_node_tmp = 0;

    edma_id = EDMA_ID(channel_id);
    if(!edma_id_valid(edma_id))
    {
        hiedmac_trace(BSP_LOG_LEVEL_ERROR, "channel_id %d not supported \n", channel_id);
        return EDMA_FAIL;
    }
    edma_id_chan_id = (s32)EDMA_ID_CHAN_ID(channel_id);
    edma_reg_str = g_edma_drv_info.edma_reg_str[edma_id];
    lli_node_tmp = &(edma_reg_str->stTransferConfig[edma_id_chan_id].lli_node_info);

    hiedmac_trace(BSP_LOG_LEVEL_DEBUG,"--------------------------edma_start!\n");
    reg_value = lli_node_tmp->config;
    hiedmac_trace(BSP_LOG_LEVEL_DEBUG,"-----------pre_stChannelConfing->config = 0x%X\n",reg_value);
    if (channel_id >= EDMA_CHANNEL_END)
    {
        hiedmac_trace(BSP_LOG_LEVEL_ERROR,"EDMA_CHANNEL_INVALID\n");
        return EDMA_CHANNEL_INVALID;
    }
    else
    {
        edma_cache_sync();
        lli_node_tmp->config = reg_value | EDMA_CHANNEL_ENABLE;
        edma_cache_sync();

        reg_value = lli_node_tmp->config;
        hiedmac_irq_trace(BSP_LOG_LEVEL_DEBUG,"--------later stChannelConfing->config = 0x%X\n",reg_value);
        return EDMA_SUCCESS;
    }
}

/*******************************************************************************
  函数名:       static s32 __devinit bsp_edma_suspend (struct platform_device *dev)
  函数描述:     系统下电前被调用，用于保存EDMA硬件的状态，做其他一些必要的处理
                确保经过suspend和resume流程后，EDMA驱动程序正常工作
  输入参数:     dev：表示当前设备
  输出参数:     无
  返回值:       成功：0
                失败：非0
*******************************************************************************/
u32 edma_ctrl_bak = 0;
static s32 bsp_edma_suspend (struct platform_device *dev,struct pm_message pmmsg)
{

#ifdef HI_EDMA_CH4_REGBASE_ADDR_VIRT
	edma_ctrl_bak = g_edma_drv_info.edma_reg_str[EDMA_CH4_ID]->ulDmaCtrl;
#endif
	return 0;
}

/*******************************************************************************
  函数名:       static s32 __devinit bsp_edma_resume(struct platform_device *dev)
  函数描述:     系统下电前被调用，用于保存EDMA硬件的状态，做其他一些必要的处理
                确保经过suspend和resume流程后，EDMA驱动程序正常工作
  输入参数:     dev：表示当前设备
  输出参数:     无
  返回值:       成功：0
                失败：非0
*******************************************************************************/
static s32 bsp_edma_resume(struct platform_device *dev)
{
#ifdef HI_EDMA_CH4_REGBASE_ADDR_VIRT

        /* edma_ch4 outstanding 降为4 */
        g_edma_drv_info.edma_reg_str[EDMA_CH4_ID]->ulDmaCtrl |= (edma_ctrl_bak&(0x1<<4));

#endif
	return 0;
}

/*******************************************************************************
  函数名:       static void __devinit bsp_edma_shutdown (struct platform_device *dev)
  函数描述:     在关机流程中被调用，做一些必要的关机前处理。
  输入参数:     dev：表示当前设备
  输出参数:     无
  返回值:       无
*******************************************************************************/
static void bsp_edma_shutdown (struct platform_device *dev)
{
	 return;
}

/*******************************************************************************
  函数名:       static s32 bsp_edma_probe
  函数描述:     把EDMAC的寄存器物理地址映射成虚拟地址；注册中断服务程序；
                EDMA全局寄存器不在此初始化，在vxWorks EDMA驱动程序中初始化。
  输入参数:     pdev：指向EDMAC设备的指针。
  输出参数:     无
  返回值:       成功：0
                失败：非0
*******************************************************************************/
static s32 bsp_edma_probe(struct platform_device *pdev)
{
    s32 ret = -1;

    if((u32)g_edma_drv_info.edma_base_addr[EDMA_CH16_ID]== HI_EDMA_REGBASE_ADDR_VIRT)
    {
        hiedmac_trace(BSP_LOG_LEVEL_WARNING, "edma ch16 already probed \n" );
        return ret;
    }    

    memset((void*)g_edma_chan_distri,-1,sizeof(g_edma_chan_distri));
    memset((void*)(&g_edma_drv_info),0,sizeof(struct edma_drv_info));

    hiedmac_trace(BSP_LOG_LEVEL_DEBUG,"edmac probe begin!*******************isr print*************\n");
    g_edma_drv_info.edma_base_addr[EDMA_CH16_ID] = (void *)HI_EDMA_REGBASE_ADDR_VIRT;
    g_edma_drv_info.edma_reg_str[EDMA_CH16_ID] =\
        (struct edma_reg_struct *)g_edma_drv_info.edma_base_addr[EDMA_CH16_ID];

#ifdef HI_EDMA_CH4_REGBASE_ADDR_VIRT
/*    ret = readl(HI_SYSSC_BASE_ADDR_VIRT + HI_SC_CTRL72_OFFSET);
    writel(ret & 0xFFFFFF0F , HI_SYSSC_BASE_ADDR_VIRT + HI_SC_CTRL72_OFFSET);*/

    if(BOARD_TYPE_GUL == bsp_get_board_mode_type())
	{
        if((u32)g_edma_drv_info.edma_base_addr[EDMA_CH4_ID]== (HI_EDMA_CH4_REGBASE_ADDR_VIRT))
        {
            hiedmac_trace(BSP_LOG_LEVEL_WARNING, "edma ch4 already probed \n" );
            return ret;
        }
        g_edma_drv_info.edma_base_addr[EDMA_CH4_ID]= (void *)(HI_EDMA_CH4_REGBASE_ADDR_VIRT);
        g_edma_drv_info.edma_reg_str[EDMA_CH4_ID] =\
            (struct edma_reg_struct *)g_edma_drv_info.edma_base_addr[EDMA_CH4_ID];
        /* 互联优先级配置，将 edma_ch4 优先级降到最低 */
        edma_ch4_set_axi_mst_min();
        /* edma_ch4 outstanding 降为4 */
        while(g_edma_drv_info.edma_reg_str[EDMA_CH4_ID]->ulChannelState &0xFFFF)
        {}
        g_edma_drv_info.edma_reg_str[EDMA_CH4_ID]->ulDmaCtrl |= (0x1<<4);
    }
    else
    {/*单模没有edma_ch4*/
    }
#endif
    edma_init();

    hiedmac_trace(BSP_LOG_LEVEL_DEBUG,"edmac request irq = %d\n", INT_LVL_EDMAC);
    if(request_irq(INT_LVL_EDMAC, (irq_handler_t)edma_isr, IRQF_DISABLED, "Balong_V7R2_EDMAC", (void *)EDMA_CH16_ID))
    {
        hiedmac_trace(BSP_LOG_LEVEL_CRIT,"EDMA_CH16 Irq request failed!\n");
        ret = EDMA_FAIL;
        return ret;
	}
#ifdef HI_EDMA_CH4_REGBASE_ADDR_VIRT
    if(BOARD_TYPE_GUL == bsp_get_board_mode_type())
	{
    #ifdef CONFIG_MODULE_VIC
        /* 多模 p531_fpga使用 vic */
        bsp_vic_connect(INT_LVL_EDMAC_CH4+32, edma_isr, EDMA_CH4_ID);
        bsp_vic_enable(INT_LVL_EDMAC_CH4+32);
    #else
        hiedmac_trace(BSP_LOG_LEVEL_DEBUG,"edmac request irq = %d\n", INT_LVL_EDMAC_CH4);
        if(request_irq(INT_LVL_EDMAC_CH4, edma_isr, IRQF_DISABLED, "Balong_V7R2_EDMAC_CH4", (void *)EDMA_CH4_ID))
        {
            hiedmac_trace(BSP_LOG_LEVEL_CRIT,"EDMA_CH4 Irq request failed!\n");
            ret = EDMA_FAIL;
            return ret;
    	}
    #endif
    }

 else
    {/*单模没有edma_ch4*/
    }
#endif
    hiedmac_trace(BSP_LOG_LEVEL_DEBUG, "edmac probe OK!\n");
    return EDMA_SUCCESS;
}

s32 bsp_edma_remove(struct platform_device *pdev)
{
    free_irq(INT_LVL_EDMAC,(void *)EDMA_CH16_ID);

#ifdef HI_EDMA_CH4_REGBASE_ADDR_VIRT
    if(BOARD_TYPE_GUL == bsp_get_board_mode_type())
	{
    #ifdef CONFIG_MODULE_VIC
        bsp_vic_disable(INT_LVL_EDMAC_CH4+32);
        bsp_vic_disconnect(INT_LVL_EDMAC_CH4+32);
    #else
        free_irq(INT_LVL_EDMAC_CH4,(void *)EDMA_CH4_ID);
    #endif
    }
    else
    {/* P531单模没有edma_ch4*/
    }
#endif

    return 0;
}

static struct platform_driver edmac_driver = {
	.probe = bsp_edma_probe,
	.remove = bsp_edma_remove,
	.shutdown = bsp_edma_shutdown,
	.suspend = bsp_edma_suspend,
	.resume = bsp_edma_resume,
	.driver = {
		.name = DRIVER_NAME,
		.owner  = THIS_MODULE,
		.pm     = NULL,
	},
};

static struct platform_device edmac_device =
{
    .name = DRIVER_NAME,
    .id       = 1,
    .num_resources = 0,
};

static s32 __init bsp_edma_init(void)
{
    s32 retval;

    hiedmac_trace(BSP_LOG_LEVEL_DEBUG,"bsp_edmac_module_init start\n");
    retval = platform_device_register(&edmac_device);
    if (retval)
    {
        hiedmac_trace(BSP_LOG_LEVEL_CRIT,"hisilicon platform_device_register failed!\n");
        return retval;
    }

    retval = platform_driver_register(&edmac_driver);

    if (retval)
    {
        hiedmac_trace(BSP_LOG_LEVEL_CRIT,"hisilicon platform devicer register Failed!\n");
        platform_device_unregister(&edmac_device);
        return retval;
    }

    hiedmac_trace(BSP_LOG_LEVEL_DEBUG,"bsp_edmac_module_init end\n");
    return retval;
}

static void __exit bsp_edma_exit(void)
{
    platform_driver_unregister(&edmac_driver);
    platform_device_unregister(&edmac_device);
}

subsys_initcall(bsp_edma_init);
module_exit(bsp_edma_exit);/*lint !e19*/

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hisilicon Drive Group");
MODULE_VERSION("HI_VERSION=" OSDRV_MODULE_VERSION_STRING);



