

#ifdef __cplusplus
extern "C"
{
#endif

#include <vxWorks.h>
#include <stdlib.h>
#include <logLib.h>
#include <stdio.h>
#include <errno.h>
#include <errnoLib.h>
#include <memLib.h>
#include <cacheLib.h>
#include <string.h>
#include <taskLib.h>
#include <intLib.h>
#include "osl_irq.h"
#include "hi_syscrg_interface.h"
#include "hi_syssc_interface.h"
#include "bsp_memmap.h"
#include "bsp_om_api.h"
#include "bsp_version.h"
#include "bsp_nvim.h"
#include "drv_nv_id.h"
#include "bsp_dump.h"
#include "bsp_dpm.h"
#include "amon_balong.h"

/* AXI状态检查，是否正在运行，在运行时，不允许配置或者获取统计数据 */
#define AXI_CHECK_STATE(config) \
do{\
    if(BSP_OK != amon_state_check(config))\
    {\
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: AXI %d is running\n", __FUNCTION__, config);\
        return BSP_ERROR;\
    }\
}while(0)  

/* axi monitor配置 */
amon_config_stru g_amon_config;
/* 监控统计 */
amon_stat_t g_amon_stat;
/* log记录 */
amon_buff_info_t g_amon_buff;

#ifdef BSP_CONFIG_HI3630  
#ifdef CONFIG_CCORE_PM
axi_reg_bak_t g_st_reg_bak;
#endif
#endif

void amon_int_handler(void);
axi_state_enum_uint32 amon_get_state(axi_config_enum_uint32 config, axi_get_state_req_enum_uint32 state_req);
s32 amon_state_check(axi_config_enum_uint32 config);
s32 amon_reset(axi_config_enum_uint32 config);
s32 amon_start(axi_config_enum_uint32 config);
s32 amon_stop(axi_config_enum_uint32 config);
void amon_config(axi_config_enum_uint32 config);
void amon_debug_reset(void);
void amon_debug_show(void);
void amon_save_log(axi_config_enum_uint32 mode, u32 id, u32 opt_type, amon_config_t * config);

void read_test(u32 addr);
void write_test(u32 addr);
void read_test(u32 addr)
{
    amon_debug("read 0x%x: 0x%x", addr, readl(addr));
}
void write_test(u32 addr)
{
    writel(0x9999aaaa, addr);
}

/* K3上在modem实现dpm, V7R2在M3实现 */
#ifdef BSP_CONFIG_HI3630  
#ifdef CONFIG_CCORE_PM
s32 amon_suspend(struct dpm_device *dev);
s32 amon_resume(struct dpm_device *dev);

struct dpm_device amon_dpm_device={
	.device_name = "amon_dpm",
	.prepare = NULL,
	.suspend_late = NULL,
	.suspend = amon_suspend,
	.resume = amon_resume,
	.resume_early = NULL,
	.complete = NULL,
};
#endif
#endif

/*****************************************************************************
 函 数 名  : amon_save_log
 功能描述  : 匹配到监控项，记录log
             保存格式: timestamp    soc/cpufast    id          port 
                       masterid     rd/wr          addr_start  addr_end
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void amon_save_log(axi_config_enum_uint32 mode, u32 id, u32 opt_type, amon_config_t * config)
{
    u32 * data_wr = (u32 *)(g_amon_buff.buff + g_amon_buff.write_offset);

    *data_wr     = om_timer_get();
    *(data_wr+1) = mode;
    *(data_wr+2) = id;
    *(data_wr+3) = config->port;
    *(data_wr+4) = config->master_id;
    *(data_wr+5) = opt_type;
    *(data_wr+6) = config->start_addr;
    *(data_wr+7) = config->end_addr;

    g_amon_buff.write_offset = (g_amon_buff.write_offset + 0x20)%(g_amon_buff.buff_size);
}

/*****************************************************************************
 函 数 名  : amon_int_handler
 功能描述  : 中断处理函数，处理读写中断
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void amon_int_handler(void)
{
    int i;
    u32 reg_value;
    u32 mask;

    /* SOC读写中断处理 */
    AXI_REG_READ(AXI_SOC_CONFIG, AXI_ID_INT_STAT, &reg_value);
    AXI_REG_READ(AXI_SOC_CONFIG, AXI_ID_INT_MASK, &mask);
    AXI_REG_WRITE(AXI_SOC_CONFIG, AXI_ID_INT_MASK, (reg_value | mask));
    AXI_REG_WRITE(AXI_SOC_CONFIG, AXI_MON_INT_CLR, reg_value<<16);
    for(i=0; i<AXI_MAX_CONFIG_ID; i++)
    {
        /* 写中断 */
        if(reg_value & (1<<(i*2)))
        {
            g_amon_stat.soc_wr_cnt[i]++;
            amon_save_log(AXI_SOC_CONFIG, i, AMON_OPT_WRITE, &g_amon_config.soc_config[i]);
            if(g_amon_config.soc_config[i].reset_flag == 1)
            {
                amon_error("soc id 0x%x wr hit, reboot\n", i);
                /* 屏蔽所有读写中断 */
                AXI_REG_WRITE(AXI_SOC_CONFIG, AXI_ID_INT_MASK, 0xffff);
                system_error(DRV_ERRNO_AMON_SOC_WR, i, 0, 0, 0);
                return;
            }
            AXI_REG_SETBITS(AXI_SOC_CONFIG, AXI_ID_INT_MASK, i*2, 1, 0);
        }
        /* 读中断 */
        if(reg_value & (1<<(i*2+1)))
        {
            g_amon_stat.soc_rd_cnt[i]++;
            amon_save_log(AXI_SOC_CONFIG, i, AMON_OPT_READ, &g_amon_config.soc_config[i]);
            if(g_amon_config.soc_config[i].reset_flag == 1)
            {
                amon_error("soc id 0x%x rd hit, reboot\n", i);
                /* 屏蔽所有读写中断 */
                AXI_REG_WRITE(AXI_SOC_CONFIG, AXI_ID_INT_MASK, 0xffff);
                system_error(DRV_ERRNO_AMON_SOC_RD, i, 1, 0, 0);
                return;
            }

            AXI_REG_SETBITS(AXI_SOC_CONFIG, AXI_ID_INT_MASK, i*2+1, 1, 0);
        }        
    }

#ifndef BSP_CONFIG_HI3630 
    /* CPUFAST读写中断处理 */
    AXI_REG_READ(AXI_CPUFAST_CONFIG, AXI_ID_INT_STAT, &reg_value);
    AXI_REG_READ(AXI_CPUFAST_CONFIG, AXI_ID_INT_MASK, &mask);
    AXI_REG_WRITE(AXI_CPUFAST_CONFIG, AXI_ID_INT_MASK, (mask | reg_value));
    AXI_REG_WRITE(AXI_CPUFAST_CONFIG, AXI_MON_INT_CLR, reg_value<<16);
    for(i=0; i<AXI_MAX_CONFIG_ID; i++)
    {
        /* 写中断 */
        if(reg_value & (1<<(i*2)))
        {
            g_amon_stat.cpufast_wr_cnt[i]++;
            amon_save_log(AXI_CPUFAST_CONFIG, i, AMON_OPT_WRITE, &g_amon_config.cpufast_config[i]);
            if(g_amon_config.cpufast_config[i].reset_flag == 1)
            {
                amon_error("cpufast id 0x%x wr hit, reboot\n", i);
                /* 屏蔽所有读写中断 */
                AXI_REG_WRITE(AXI_CPUFAST_CONFIG, AXI_ID_INT_MASK, 0xffff);
                system_error(DRV_ERRNO_AMON_CPUFAST_WR, i, 2, 0, 0);
                return;
            }
            AXI_REG_SETBITS(AXI_CPUFAST_CONFIG, AXI_ID_INT_MASK, i*2, 1, 0);
        }
        /* 读中断 */
        if(reg_value & (1<<(i*2+1)))
        {
            g_amon_stat.cpufast_rd_cnt[i]++;
            amon_save_log(AXI_CPUFAST_CONFIG, i, AMON_OPT_READ, &g_amon_config.cpufast_config[i]);
            if(g_amon_config.cpufast_config[i].reset_flag == 1)
            {
                amon_error("cpufast id 0x%x rd hit, reboot\n", i);
                /* 屏蔽所有读写中断 */
                AXI_REG_WRITE(AXI_CPUFAST_CONFIG, AXI_ID_INT_MASK, 0xffff);
                system_error(DRV_ERRNO_AMON_CPUFAST_RD, i, 3, 0, 0);
                return;
            }
            AXI_REG_SETBITS(AXI_CPUFAST_CONFIG, AXI_ID_INT_MASK, i*2+1, 1, 0);
        }        
    }
#endif    
}

/*****************************************************************************
 函 数 名  : amon_get_state
 功能描述  : 获取AXI monitor运行/软复位状态，不能同时获取CPUFAST/SOC状态
 输入参数  : axi_config_enum_uint32 config
             axi_get_state_req_enum_uint32 state_req
 输出参数  : 无
 返 回 值  : axi_state_enum_uint32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
axi_state_enum_uint32 amon_get_state(axi_config_enum_uint32 config, axi_get_state_req_enum_uint32 state_req)
{
    u32 reg_value = 0;

    /* 从状态寄存器获取运行状态 */
    AXI_REG_READ(config, AXI_MON_CNT_STATE_INT, &reg_value);
    /* 获取运行状态请求 */
    if(AXI_GET_RUN_STATE_REQ == state_req)
    {
        reg_value = reg_value & AXI_RUN_STATE_MASK;
    }
    /* 获取软复位状态请求 */
    else if(AXI_GET_RESET_STATE_REQ == state_req)
    {
        reg_value = reg_value & AXI_RESET_STATE_MASK;
    }
    /* 无效请求 */
    else
    {
        return (axi_state_enum_uint32)AXI_STATE_BUTT;
    }

    return reg_value;
}

/*****************************************************************************
 函 数 名  : amon_state_check
 功能描述  : 状态判定函数，是否正在运行
 输出参数  : 无
 返 回 值  : s32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
s32 amon_state_check(axi_config_enum_uint32 config)
{
    axi_state_enum_uint32 axi_state;

    axi_state = axi_get_state(config, AXI_GET_RUN_STATE_REQ);
    /* AXI monitor正在运行 */
    if(axi_state != AXI_IDLE && axi_state != AXI_STOP)
    {
        return BSP_ERROR;
    }

    return BSP_OK;
}

/*****************************************************************************
 函 数 名  : amon_reset
 功能描述  : monitor内部软复位
 输出参数  : 无
 返 回 值  : s32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
s32 amon_reset(axi_config_enum_uint32 config)
{
    axi_state_enum_uint32 axi_state;
    int i = 0;

    /* 检查运行状态 */
    AXI_CHECK_STATE(config);

    /* 软复位 */
    AXI_REG_WRITE(config, AXI_MON_CNT_RESET, AXI_SOFT_RESET);
    /* 软复位结束判定，AXI monitor状态寄存器指示软复位结束 */
    do       
    {
        axi_state = axi_get_state(config, AXI_GET_RESET_STATE_REQ);
        if(AXI_RESET_FINISH == axi_state)
        {
            return BSP_OK;
        }
    }while(i++ < AXI_WAIT_CNT);
    
    /* 软复位超时异常 */
    amon_error("reset 0x%x time out\n", config);
    return BSP_ERROR;
}

/*****************************************************************************
 函 数 名  : amon_start
 功能描述  : 启动AXI monitor，由系统控制器启动
 输入参数  : axi_config_enum_uint32 config
 输出参数  : 无
 返 回 值  : s32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
s32 amon_start(axi_config_enum_uint32 config)
{
    axi_state_enum_uint32 axi_state;
    amon_config_t * amon_config;
    int i = 0;
    u32 reg_value = 0xffff;

    /* 检查运行状态 */
    AXI_CHECK_STATE(config);

    /* 清中断 */
    AXI_REG_WRITE(config, AXI_MON_INT_CLR, 0xFFFFFFFF);

    if(config == AXI_SOC_CONFIG)
    {
        amon_config = (amon_config_t *)g_amon_config.soc_config;
    }
    else
    {
        amon_config = (amon_config_t *)g_amon_config.cpufast_config;
    }
    
    /* 解除读写中断屏蔽，根据NV配置构造 */
    for(i=0; i<AXI_MAX_CONFIG_ID; i++)
    {
        /* ID i监控读操作 */
        if(amon_config->opt_type & AMON_OPT_READ)
        {
            reg_value = reg_value & (~(1<<(i*2+1)));
        }
        /* ID i监控写操作 */
        if(amon_config->opt_type & AMON_OPT_WRITE)
        {
            reg_value = reg_value & (~(1<<(i*2)));
        }
        amon_config++;
    }

    AXI_REG_WRITE(config, AXI_ID_INT_MASK, reg_value);
    
    /* 配置系统控制器启动Monitor */
    axi_sc_mon_start(config);

    /* 启动结束判定，等待启动标志置位 */
    i=0;
    do
    {
        axi_state = axi_get_state(config, AXI_GET_RUN_STATE_REQ);
        if(AXI_UNWIN_RUNNING == axi_state || AXI_WIN_RUNNING == axi_state)
        {
            return BSP_OK;
        }
    }while(i++ < AXI_WAIT_CNT);
    
    /* 启动超时异常 */
    amon_error("start 0x%x time out\n", config);
    return BSP_ERROR;
}

/*****************************************************************************
 函 数 名  : amon_stop
 功能描述  : 停止AXI monitor，由系统控制器停止
 输入参数  : axi_config_enum_uint32 config
 输出参数  : 无
 返 回 值  : s32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
s32 amon_stop(axi_config_enum_uint32 config)
{
    axi_state_enum_uint32 axi_state;
    int i = 0;

    /* 屏蔽读写中断 */
    AXI_REG_WRITE(config, AXI_ID_INT_MASK, 0xFFFF);

    /* 配置系统控制器停止Monitor */
    axi_sc_mon_stop(config);

    /* 停止结束判定 */
    do
    {
        axi_state = axi_get_state(config, AXI_GET_RUN_STATE_REQ);
        if(AXI_STOP == axi_state)
        {
            /* 停止之后，强制进入IDLE态 */
            AXI_REG_WRITE(config, AXI_MON_CNT_RESET, AXI_RESET_TO_IDLE);
            return BSP_OK;
        }
    }while(i++ < AXI_WAIT_CNT);
    
    /* 停止Monitor超时异常 */
    amon_error("stop 0x%x time out\n", config);
    return BSP_ERROR;
}

/*****************************************************************************
 函 数 名  : amon_config
 功能描述  : 配置monitor监控ID
 输入参数  : axi_config_enum_uint32 config
 输出参数  : 无
 返 回 值  : s32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void amon_config(axi_config_enum_uint32 config)
{
    int i;
    
    if(AXI_SOC_CONFIG == config)
    {
        for(i=0; i<AXI_MAX_CONFIG_ID; i++)
        {
            /* 配置端口 */
            AXI_REG_SETBITS(config, AXI_MON_PORT_SEL, i*3, 3, g_amon_config.soc_config[i].port&0x7);
            /* 配置masterid */
            AXI_REG_WRITE(config, AXI_MON_CNT_ID(i), g_amon_config.soc_config[i].master_id);
            /* 配置起始地址 */
            AXI_REG_WRITE(config, AXI_MON_ID_ADDR_DES(i), g_amon_config.soc_config[i].start_addr);
            /* 配置结束地址 */
            AXI_REG_WRITE(config, AXI_MON_ID_ADDR_DES_M(i), g_amon_config.soc_config[i].end_addr);
        }   
    }

    if(AXI_CPUFAST_CONFIG == config)
    {
        for(i=0; i<AXI_MAX_CONFIG_ID; i++)
        {
            /* 配置端口 */
            AXI_REG_SETBITS(config, AXI_MON_PORT_SEL, i*3, 3, g_amon_config.cpufast_config[i].port&0x7);
            /* 配置masterid */
            AXI_REG_WRITE(config, AXI_MON_CNT_ID(i), g_amon_config.cpufast_config[i].master_id);
            /* 配置起始地址 */
            AXI_REG_WRITE(config, AXI_MON_ID_ADDR_DES(i), g_amon_config.cpufast_config[i].start_addr);
            /* 配置结束地址 */
            AXI_REG_WRITE(config, AXI_MON_ID_ADDR_DES_M(i), g_amon_config.cpufast_config[i].end_addr);
        }   
    }
}

/*****************************************************************************
 函 数 名  : bsp_amon_init
 功能描述  : axi monitor初始化，读取NV配置监控ID
 输入参数  : 无
 输出参数  : 无
 返 回 值  : s32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
s32 bsp_amon_init(void)
{
#ifndef BSP_CONFIG_HI3630  
    struct clk * soc_clk;
    struct clk * cpufast_clk;
#endif

    /* 读取NV配置 */
    if(BSP_OK != bsp_nvm_read(NV_ID_DRV_AMON, (u8*)&g_amon_config, sizeof(amon_config_stru)))
    {
        amon_error("read nv 0x%x fail\n", NV_ID_DRV_AMON);
        return BSP_ERROR;
    }

    /* 未使能，不初始化 */
    if(g_amon_config.en_flag == 0)
    {
        return BSP_OK;
    }

    /* 使能monitor，注册dpm */
#ifdef BSP_CONFIG_HI3630
#ifdef CONFIG_CCORE_PM
    if(BSP_OK != bsp_device_pm_add(&amon_dpm_device))
    {
        amon_error("add amon_dpm_device fail\n");
        return BSP_ERROR;
    }
    memset(&g_st_reg_bak, 0x0, sizeof(axi_reg_bak_t));
#endif
#endif

    /* SOC初始化 */
    if(g_amon_config.en_flag & AMON_SOC_MASK)
    {
#ifndef BSP_CONFIG_HI3630        
        soc_clk = clk_get(NULL, "amon_soc_clk");
        clk_enable(soc_clk);
#else
        hi_syscrg_amon_soc_clk_enable();
#endif
        /* SOC系统控制器复位 */
        hi_syscrg_amon_soc_soft_reset();
        hi_syscrg_amon_soc_rls_reset();
        /* monitor内部复位 */
        if(BSP_OK != amon_reset(AXI_SOC_CONFIG))
        {
            amon_error("soc reset fail\n");
            return BSP_ERROR;
        }
        /* 中断屏蔽 */
        AXI_REG_WRITE(AXI_SOC_CONFIG, AXI_ID_INT_MASK, 0xFFFF);
        AXI_REG_SETBITS(AXI_SOC_CONFIG, AXI_MON_CNT_CTRL, 2, 2, 0x3);
        AXI_REG_WRITE(AXI_SOC_CONFIG, AXI_CAPT_INT_MASK, 0xF);        
    }

#ifndef BSP_CONFIG_HI3630
     /* CPUFAST初始化 */
    if(g_amon_config.en_flag & AMON_CPUFAST_MASK)
    {   
        cpufast_clk = clk_get(NULL, "amon_cpufast_clk");
        clk_enable(cpufast_clk);
        /* CPUFAST系统控制器复位 */
        hi_syscrg_amon_cpufast_soft_reset();
        hi_syscrg_amon_cpufast_rls_reset();
        if(BSP_OK != amon_reset(AXI_CPUFAST_CONFIG))
        {
            amon_error("cpufast reset fail\n");
            return BSP_ERROR;
        }
        AXI_REG_WRITE(AXI_CPUFAST_CONFIG, AXI_ID_INT_MASK, 0xFFFF);
        AXI_REG_WRITE(AXI_CPUFAST_CONFIG, AXI_CAPT_INT_MASK, 0xF);
        AXI_REG_SETBITS(AXI_CPUFAST_CONFIG, AXI_MON_CNT_CTRL, 2, 2, 0x3);
    }     
#endif

    if(BSP_OK != bsp_dump_get_buffer(DUMP_SAVE_MOD_AMON, &g_amon_buff.buff, &g_amon_buff.buff_size))
    {
        amon_error("get buffer fail\n");
        return BSP_ERROR;
    }
    g_amon_buff.write_offset = 0;
    memset(g_amon_buff.buff, 0, g_amon_buff.buff_size);

    /* 挂中断 */
    if(BSP_OK != request_irq(AXI_INT_VECTOR, (irq_handler_t)amon_int_handler, 0, "AXI_MON_IRQ", NULL))
    {
        amon_error("INT connect fail\n");
        return BSP_ERROR;
    }
    
    /* 启动SOC监控 */
    if(g_amon_config.en_flag & AMON_SOC_MASK)
    {
        amon_config(AXI_SOC_CONFIG);
        if(BSP_OK != amon_start(AXI_SOC_CONFIG))
        {
            amon_error("soc start fail\n");
            return BSP_ERROR;
        }
    }
    
#ifndef BSP_CONFIG_HI3630
    /* 启动CPUFAST监控 */
    if(g_amon_config.en_flag & AMON_CPUFAST_MASK)
    {
        amon_config(AXI_CPUFAST_CONFIG);
        if(BSP_OK != amon_start(AXI_CPUFAST_CONFIG))
        {
            amon_error("cpufast start fail\n");
            return BSP_ERROR;
        }
    }
#endif

    amon_debug("%s init ok\n", __FUNCTION__);
    
    return BSP_OK;
}

void amon_debug_reset(void)
{
    int i;

    for(i=0; i<AXI_MAX_CONFIG_ID; i++)
    {
        g_amon_stat.soc_rd_cnt[i]     = 0;
        g_amon_stat.soc_wr_cnt[i]     = 0;
#ifndef BSP_CONFIG_HI3630
        g_amon_stat.cpufast_rd_cnt[i] = 0;
        g_amon_stat.cpufast_wr_cnt[i] = 0;
#endif
    }   
}

void amon_debug_show(void)
{
    int i;
    u32 reg_value_low;
    u32 reg_value_high;

    /* SOC统计信息 */
    amon_debug("************SOC STAT************\n");
    for(i=0; i<AXI_MAX_CONFIG_ID; i++)
    {
        AXI_REG_READ(AXI_SOC_CONFIG, AXI_MON_RD_BYTES_ID_LOW(i), &reg_value_low);
        AXI_REG_READ(AXI_SOC_CONFIG, AXI_MON_RD_BYTES_ID_HIGH(i), &reg_value_high);
        amon_debug("=======ID %d statistics=======\n", i);
        amon_debug("rd int cnt       : 0x%x\n", g_amon_stat.soc_rd_cnt[i]);
        amon_debug("wr int cnt       : 0x%x\n", g_amon_stat.soc_wr_cnt[i]);
        amon_debug("rd total bytes(h): 0x%x\n", reg_value_high);
        amon_debug("rd total bytes(l): 0x%x\n", reg_value_low);
        AXI_REG_READ(AXI_SOC_CONFIG, AXI_MON_WR_BYTES_ID_LOW(i), &reg_value_low);
        AXI_REG_READ(AXI_SOC_CONFIG, AXI_MON_WR_BYTES_ID_HIGH(i), &reg_value_high);
        amon_debug("wr total bytes(h): 0x%x\n", reg_value_high);
        amon_debug("wr total bytes(l): 0x%x\n", reg_value_low);        
    }

    amon_debug("\n");

#ifndef BSP_CONFIG_HI3630 
    /* CPUFAST统计信息 */
    amon_debug("************CPUFAST STAT************\n");
    for(i=0; i<AXI_MAX_CONFIG_ID; i++)
    {
        AXI_REG_READ(AXI_CPUFAST_CONFIG, AXI_MON_RD_BYTES_ID_LOW(i), &reg_value_low);
        AXI_REG_READ(AXI_CPUFAST_CONFIG, AXI_MON_RD_BYTES_ID_HIGH(i), &reg_value_high);        
        amon_debug("=======ID %d statistics=======\n", i);        
        amon_debug("rd int cnt: 0x%x\n", g_amon_stat.cpufast_rd_cnt[i]);
        amon_debug("wr int cnt: 0x%x\n", g_amon_stat.cpufast_wr_cnt[i]);
        amon_debug("rd total bytes(h): 0x%x\n", reg_value_high);
        amon_debug("rd total bytes(l): 0x%x\n", reg_value_low);
        AXI_REG_READ(AXI_CPUFAST_CONFIG, AXI_MON_WR_BYTES_ID_LOW(i), &reg_value_low);
        AXI_REG_READ(AXI_CPUFAST_CONFIG, AXI_MON_WR_BYTES_ID_HIGH(i), &reg_value_high);
        amon_debug("wr total bytes(h): 0x%x\n", reg_value_high);
        amon_debug("wr total bytes(l): 0x%x\n", reg_value_low);          
    }
#endif
}

#ifdef BSP_CONFIG_HI3630  
#ifdef CONFIG_CCORE_PM
s32 amon_suspend(struct dpm_device *dev)
{
    u32 j;
    u32 reg_value;
    /*lint -save -e958*/
    unsigned long long data_cnt;
    /*lint -restore*/
    u32 data_low;
    u32 data_high;
    axi_reg_bak_t * reg_bak;

    /* 备份前，先停止monitor */
    axi_sc_mon_stop(AXI_SOC_CONFIG);

    reg_bak = &g_st_reg_bak;
    /* 控制寄存器备份 */
    axi_reg_read(AXI_SOC_CONFIG, AXI_MON_CNT_CTRL, &(reg_bak->ctrl_reg));
    axi_reg_read(AXI_SOC_CONFIG, AXI_MON_PORT_SEL, &(reg_bak->port_reg));
    axi_reg_read(AXI_SOC_CONFIG, AXI_ID_INT_MASK, &(reg_bak->int_mask));
    for(j=0; j<AXI_MAX_CONFIG_ID; j++)
    {
        axi_reg_read(AXI_SOC_CONFIG, AXI_MON_CNT_ID(j), &(reg_bak->id_reg[j]));
        axi_reg_read(AXI_SOC_CONFIG, AXI_MON_ID_ADDR_DES(j), &(reg_bak->addr_start_reg[j]));
        axi_reg_read(AXI_SOC_CONFIG, AXI_MON_ID_ADDR_DES_M(j), &(reg_bak->addr_end_reg[j]));
    }

    /* 统计寄存器累加 */
    for(j=0; j<AXI_MAX_CONFIG_ID; j++)
    {
        axi_reg_read(AXI_SOC_CONFIG, AXI_MON_INCR1_ID(j), &reg_value);
        reg_bak->incr1_reg[j] += reg_value;
        axi_reg_read(AXI_SOC_CONFIG, AXI_MON_INCR2_ID(j), &reg_value);
        reg_bak->incr2_reg[j] += reg_value; 
        axi_reg_read(AXI_SOC_CONFIG, AXI_MON_INCR4_ID(j), &reg_value);
        reg_bak->incr4_reg[j] += reg_value; 
        axi_reg_read(AXI_SOC_CONFIG, AXI_MON_INCR8_ID(j), &reg_value);
        reg_bak->incr8_reg[j] += reg_value;  
        axi_reg_read(AXI_SOC_CONFIG, AXI_MON_INCR16_ID(j), &reg_value);
        reg_bak->incr16_reg[j] += reg_value; 
        axi_reg_read(AXI_SOC_CONFIG, AXI_MON_WRAP_ID(j), &reg_value);
        reg_bak->wrap_reg[j] += reg_value; 
        axi_reg_read(AXI_SOC_CONFIG, AXI_MON_BURST_ID(j), &reg_value);
        reg_bak->burst_send_reg[j] += reg_value; 
        axi_reg_read(AXI_SOC_CONFIG, AXI_MON_FINISH_ID(j), &reg_value);
        reg_bak->burst_fin_reg[j] += reg_value; 
        /* 读写数据总量40bit长度，需要计算溢出值 */
        axi_reg_read(AXI_SOC_CONFIG, AXI_MON_RD_BYTES_ID_LOW(j), &data_low);
        axi_reg_read(AXI_SOC_CONFIG, AXI_MON_RD_BYTES_ID_HIGH(j), &data_high); 
        data_cnt = (unsigned long long)data_low + (unsigned long long)reg_bak->rd_cnt_low_reg[j];
        reg_bak->rd_cnt_high_reg[j] += data_high; 
        if(data_cnt >> 32)
        {
            reg_bak->rd_cnt_high_reg[j] += 1;
        }
        reg_bak->rd_cnt_low_reg[j] = (u32)data_cnt;
        axi_reg_read(AXI_SOC_CONFIG, AXI_MON_WR_BYTES_ID_LOW(j), &data_low);
        axi_reg_read(AXI_SOC_CONFIG, AXI_MON_WR_BYTES_ID_HIGH(j), &data_high); 
        data_cnt = (unsigned long long)data_low + (unsigned long long)reg_bak->wr_cnt_low_reg[j];
        reg_bak->wr_cnt_high_reg[j] += data_high; 
        if(data_cnt >> 32)
        {
            reg_bak->wr_cnt_high_reg[j] += 1;
        }
        reg_bak->wr_cnt_low_reg[j] = (u32)data_cnt;            
    }

    return BSP_OK;
}

s32 amon_resume(struct dpm_device *dev)
{
    u32 j;
    axi_reg_bak_t * reg_bak;

    reg_bak = &g_st_reg_bak;

    /* 如果未开钟，先打开时钟 */
    if(!hi_syscrg_amon_soc_get_clk_status())
    {
        hi_syscrg_amon_soc_clk_enable();
    }

    axi_reset(AXI_SOC_CONFIG);

    /* 恢复配置寄存器 */
    axi_reg_write(AXI_SOC_CONFIG, AXI_MON_CNT_CTRL, reg_bak->ctrl_reg);
    axi_reg_write(AXI_SOC_CONFIG, AXI_MON_PORT_SEL, reg_bak->port_reg);
    axi_reg_write(AXI_SOC_CONFIG, AXI_ID_INT_MASK, reg_bak->int_mask);
    for(j=0; j<AXI_MAX_CONFIG_ID; j++)
    {
        axi_reg_write(AXI_SOC_CONFIG, AXI_MON_CNT_ID(j), reg_bak->id_reg[j]);
        axi_reg_write(AXI_SOC_CONFIG, AXI_MON_ID_ADDR_DES(j), reg_bak->addr_start_reg[j]);
        axi_reg_write(AXI_SOC_CONFIG, AXI_MON_ID_ADDR_DES_M(j), reg_bak->addr_end_reg[j]);
    } 

    /* 启动monitor */
    axi_sc_mon_start(AXI_SOC_CONFIG);

    return BSP_OK;
}
#endif
#endif

#ifdef __cplusplus
}
#endif

