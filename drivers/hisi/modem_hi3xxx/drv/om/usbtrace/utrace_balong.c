

#ifdef __cplusplus
extern "C"
{
#endif

#include "bsp_dump.h"
#include "bsp_nvim.h"
#include "bsp_clk.h"
#include "utrace_balong.h"

/* 初始化标志 */
u32 g_ul_init_flag = false;
/* 监控启动标志 */
u32 g_ul_capt_start_flag = false;
/* ETB数据导出地址 */
u32 g_ul_etb_data_base = 0;
/* ETB数据分配空间长度 */
u32 g_ul_etb_data_len = 0;
/* ETB数据长度 */
u32 g_ul_etb_len = 0;

#ifndef BSP_CONFIG_HI3630 /* V7R2 */

/*****************************************************************************
 函 数 名  : utrace_reg_read
 功能描述  : 寄存器读操作
 输入参数  : trace_config_enum_u32 config
             u32 reg
 输出参数  : u32 * value
 返 回 值  : void
 调用函数  :
 被调函数  :
 修改历史      :
  1.日    期   : 2013年3月26日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void utrace_reg_read(trace_config_enum_u32 config, u32 reg, u32 * value)
{
    /* 根据配置目标读取相应寄存器 */
    switch(config)
    {
        case TRACE_STM:
            *value = readl(STM_REG_BASE + reg);
            break;

        case TRACE_PTM0:
            *value = readl(PTM0_REG_BASE + reg);
            break;

        case TRACE_PTM1:
            *value = readl(PTM1_REG_BASE + reg);
            break;

        case TRACE_FUNNEL:
            *value = readl(FUNNEL_REG_BASE + reg);
            break;

        case TRACE_ETF:
            *value = readl(ETF_REG_BASE + reg);
            break;

        case TRACE_ETR:
            *value = readl(ETR_REG_BASE + reg);
            break;

        default:
            break;
    }
}

/*****************************************************************************
 函 数 名  : utrace_reg_write
 功能描述  : 寄存器写操作
 输入参数  : trace_config_enum_u32 config
             u32 reg
             u32 value
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年3月26日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void utrace_reg_write(trace_config_enum_u32 config, u32 reg, u32 value)
{
    /* 根据配置目标写入相应寄存器 */
    switch(config)
    {
        case TRACE_STM:
            writel(value, STM_REG_BASE + reg);
            break;

        case TRACE_PTM0:
            writel(value, PTM0_REG_BASE + reg);
            break;

        case TRACE_PTM1:
            writel(value, PTM1_REG_BASE + reg);
            break;

        case TRACE_FUNNEL:
            writel(value, FUNNEL_REG_BASE + reg);
            break;

        case TRACE_ETF:
            writel(value, ETF_REG_BASE + reg);
            break;

        case TRACE_ETR:
            writel(value, ETR_REG_BASE + reg);
            break;

        default:
            break;
    }
}
#else

void __iomem *g_a9ptmBase = NULL;
void __iomem *g_a9funnelBase = NULL;

/*****************************************************************************
 函 数 名  : utrace_reg_read
 功能描述  : 寄存器读操作
 输入参数  : trace_config_enum_u32 config
             u32 reg
 输出参数  : u32 * value
 修改历史      :
  1.日    期   : 2013年3月26日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void utrace_reg_read(trace_config_enum_u32 config, u32 reg, u32 * value)
{
    if((NULL == g_a9ptmBase) || (NULL == g_a9funnelBase))
    {
        return ;
    }
    
    /* 根据配置目标读取相应寄存器 */
    switch(config)
    {
        case TRACE_PTM1:
            *value = readl((void*)(g_a9ptmBase + reg));
            break;
            
        case TRACE_FUNNEL:
            *value = readl((void*)(g_a9funnelBase + reg));
            break;
            
        default:
            break;
    }
}

/*****************************************************************************
 函 数 名  : utrace_reg_write
 功能描述  : 寄存器写操作
 输入参数  : trace_config_enum_u32 config
             u32 reg
             u32 value
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年3月26日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void utrace_reg_write(trace_config_enum_u32 config, u32 reg, u32 value)
{
    /* 根据配置目标写入相应寄存器 */
    switch(config)
    {

        case TRACE_PTM1:
            writel(value, (void*)(g_a9ptmBase + reg));
            break;
            
        case TRACE_FUNNEL:
            writel(value, (void*)(g_a9funnelBase + reg));
            break;

        default:
            break;
    }
}

#endif

/*****************************************************************************
 函 数 名  : utrace_reg_getbits
 功能描述  : 寄存器读位操作
 输入参数  : trace_config_enum_u32 config
             u32 reg
             u32 pos
             u32 bits
 输出参数  : u32 * value
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年3月26日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void utrace_reg_getbits(trace_config_enum_u32 config, u32 reg, u32 pos, u32 bits, u32 * value)
{
    u32 reg_value = 0;

    /* 根据配置目标读取相应寄存器 */
    utrace_reg_read(config, reg, &reg_value);
    /* 根据寄存器值取相应位 */
    *value = (reg_value >> pos) & (((u32)1 << (bits)) - 1);
}

/*****************************************************************************
 函 数 名  : utrace_reg_setbits
 功能描述  : 寄存器写位操作
 输入参数  : trace_config_enum_u32 config
             u32 reg
             u32 pos
             u32 bits
             u32 value
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年3月26日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void utrace_reg_setbits(trace_config_enum_u32 config, u32 reg, u32 pos, u32 bits, u32 value)
{
    u32 reg_value = 0;

    /* 根据配置目标读取相应寄存器 */
    utrace_reg_read(config, reg, &reg_value);
    /* 计算写入寄存器的目标值 */
    reg_value = (reg_value & (~((((u32)1 << (bits)) - 1) << (pos)))) | ((u32)((value) & (((u32)1 << (bits)) - 1)) << (pos));
    /* 写入目的寄存器 */
    utrace_reg_write(config, reg, reg_value);
}


/*****************************************************************************
 函 数 名  : utrace_funnel_config
 功能描述  : funnel配置接口
 输入参数  : funnel_port_enum_u32 port --- 需要使能的端口
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年3月26日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void utrace_funnel_config(funnel_port_enum_u32 port)
{
    /* 使能funnel对应端口 */
    utrace_reg_setbits(TRACE_FUNNEL, FUNNEL_CTRL, port, 1, 1);
}

/*****************************************************************************
 函 数 名  : utrace_ptm_config
 功能描述  : PTM配置接口
 输入参数  : trace_config_enum_u32 index --- APPA9 PTM or MODMEA9 PTM
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年3月26日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void utrace_ptm_config(trace_config_enum_u32 index)
{
    /* 清除PowerDown */
    utrace_reg_setbits(index, PTM_CTRL, 0, 1, 0x0);
    /* 设置Program Bit, bit10 */
    utrace_reg_setbits(index, PTM_CTRL, 10, 1, 0x1);
    /* 配置Trace ID */
    utrace_reg_write(index, PTM_TRACEID, index + 1);
#if 0
    /* 配置地址对 */
    utrace_reg_write(index, PTM_ACVR(0), DDR_SOCP_ADDR);
    utrace_reg_write(index, PTM_ACVR(1), DDR_SOCP_ADDR + DDR_SOCP_SIZE);
#endif
    /* 配置地址访问类型 */
    utrace_reg_write(index, PTM_ACTR(0), 0x1);
    utrace_reg_write(index, PTM_ACTR(1), 0x1);
    /* 配置TraceEnable Event(监控所有) */
    utrace_reg_write(index, PTM_TEEVR, 0x376f);
    /* 配置TraceEnable控制项(监控所有) */
    utrace_reg_write(index, PTM_TECR, 0x01000000);
    /* 配置Trigger Event */
    utrace_reg_write(index, PTM_TRIGGER, 0x406F);
    /* 配置Timestamp Event */
    utrace_reg_write(index, PTM_TSEVR, 0x406F);
    /* 去使能Timestamp */
    utrace_reg_setbits(index, PTM_CTRL, 28, 1, 0x0);
    /* 配置Context ID, Bit14:15 */
    utrace_reg_setbits(index, PTM_CTRL, 14, 2, 0x3);
    /* 配置Sync Frequency寄存器 */
    utrace_reg_write(index, PTM_SYNCFR, 0x400);
}

/*****************************************************************************
 函 数 名  : utrace_ptm_enable
 功能描述  : PTM使能
 输入参数  : trace_config_enum_u32 index --- APPA9 PTM or MODMEA9 PTM
 输出参数  : 无
 返 回 值  : u32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年3月26日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void utrace_ptm_enable(trace_config_enum_u32 index)
{
    utrace_reg_setbits(index, PTM_CTRL, 10, 1, 0x0);
}

/*****************************************************************************
 函 数 名  : utrace_ptm_disable
 功能描述  : PTM去使能
 输入参数  : trace_config_enum_u32 index --- APPA9 PTM or MODMEA9 PTM
 输出参数  : 无
 返 回 值  : u32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年3月26日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void utrace_ptm_disable(trace_config_enum_u32 index)
{
    utrace_reg_setbits(index, PTM_CTRL, 10, 1, 0x1);
}

#ifndef BSP_CONFIG_HI3630 /* V7R2 */

/*****************************************************************************
 函 数 名  : utrace_reg_unlock
 功能描述  : STM, FUNNEL, ETF, ETR配置寄存器默认为锁定状态，需要在初始化时进行解锁
 输入参数  : 无
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年3月26日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void utrace_reg_unlock(void)
{
    /* PTM0解锁 */
    utrace_reg_write(TRACE_PTM0, PTM_LOCK_ACCESS, UTRACE_UNLOCK_CODE);
    /* FUNNEL解锁 */
    utrace_reg_write(TRACE_FUNNEL, FUNNEL_LOCK_ACCESS, UTRACE_UNLOCK_CODE);
    /* ETF解锁 */
    utrace_reg_write(TRACE_ETF, ETF_LOCK_ACCESS, UTRACE_UNLOCK_CODE);
    /* ETR解锁 */
    utrace_reg_write(TRACE_ETR, ETR_LOCK_ACCESS, UTRACE_UNLOCK_CODE);
}

/*****************************************************************************
 函 数 名  : utrace_reg_lock
 功能描述  : STM, FUNNEL, ETF, ETR操作完成之后，锁定
 输入参数  : 无
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年3月26日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void utrace_reg_lock(void)
{
    /* PTM0锁定 */
    utrace_reg_write(TRACE_PTM0, PTM_LOCK_ACCESS, UTRACE_LOCK_CODE);
    /* FUNNEL锁定 */
    utrace_reg_write(TRACE_FUNNEL, FUNNEL_LOCK_ACCESS, UTRACE_LOCK_CODE);
    /* ETF锁定 */
    utrace_reg_write(TRACE_ETF, ETF_LOCK_ACCESS, UTRACE_LOCK_CODE);
    /* ETR锁定 */
    utrace_reg_write(TRACE_ETR, ETR_LOCK_ACCESS, UTRACE_LOCK_CODE);
}

/*****************************************************************************
 函 数 名  : utrace_stm_config
 功能描述  : STM配置接口
 输入参数  : u32 port --- 配置端口
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年3月26日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void utrace_stm_config(u32 port)
{
    /* ATB Trigger使能 */
    utrace_reg_write(TRACE_STM, STM_TRIGGER_CTRL, 0x8);
    /* stimulus port使能 */
    utrace_reg_setbits(TRACE_STM, STM_PORT_ENABLE, port, 1, (u32)1 << port);
    /* stimulus trigger port使能 */
    utrace_reg_setbits(TRACE_STM, STM_PORT_TRIGGER_ENABLE, port, 1, (u32)1 << port);
    /* Trace ID配置 */
    utrace_reg_setbits(TRACE_STM, STM_TRACE_CTRL, 16, 7, TRACE_STM + 1);
}

/*****************************************************************************
 函 数 名  : utrace_stm_enable
 功能描述  : STM使能
 输入参数  :
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年3月26日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void utrace_stm_enable(void)
{
    utrace_reg_setbits(TRACE_STM, STM_TRACE_CTRL, 0, 1, 1);
}

/*****************************************************************************
 函 数 名  : utrace_stm_disable
 功能描述  : STM去使能
 输入参数  :
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年3月26日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void utrace_stm_disable(void)
{
    utrace_reg_setbits(TRACE_STM, STM_TRACE_CTRL, 0, 1, 0);
}

/*****************************************************************************
 函 数 名  : utrace_etb_config
 功能描述  : ETB配置接口，配置ETB基本参数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年3月26日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void utrace_etb_config(void)
{
    /* 配置为ETF，硬FIFO模式 */
    //utrace_reg_setbits(TRACE_ETF, ETF_MODE, 0, 2, TMC_MODE_HARDWARE_FIFO);
    /* 配置为ETB，循环buffer */
    utrace_reg_setbits(TRACE_ETF, ETF_MODE, 0, 2, TMC_MODE_CIRCULAR_BUFF);
    /* 配置FFCR EnTl(bit1), EnFt(bit0) */
    utrace_reg_setbits(TRACE_ETF, ETR_FORMAT_FLUSH_CTRL, 0, 2, 0x3);
    /* 配置水线 */
    utrace_reg_write(TRACE_ETF, ETF_BUF_WATER_MARK, 0x3);
}

/*****************************************************************************
 函 数 名  : utrace_etr_config
 功能描述  : ETR配置接口，配置ETR基本参数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年3月26日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void utrace_etr_config(etr_config_t * config)
{
    /* 配置ETR DDR缓存空间大小 */
    utrace_reg_write(TRACE_ETR, ETR_RAM_SIZE, (config->buf_size)/4);
    /* 配置ETR工作模式 */
    utrace_reg_setbits(TRACE_ETR, ETF_MODE, 0, 2, config->mode);
    /* 配置FFCR EnTl(bit1), EnFt(bit0) */
    utrace_reg_setbits(TRACE_ETR, ETR_FORMAT_FLUSH_CTRL, 0, 2, 0x3);
    /* 配置水线 */
    utrace_reg_write(TRACE_ETR, ETF_BUF_WATER_MARK, UTRACE_WATER_MARK);
    /* 配置DDR基址 */
    utrace_reg_write(TRACE_ETR, ETR_DATA_BUF_ADDR, config->buf_addr);
}

/*****************************************************************************
 函 数 名  : utrace_etb_enable
 功能描述  : ETB使能
 输入参数  : 无
 输出参数  : 无
 返 回 值  : u32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年3月26日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void utrace_etb_enable(void)
{
    utrace_reg_write(TRACE_ETF, ETR_CTRL, 0x1);
}

/*****************************************************************************
 函 数 名  : utrace_etb_stop
 功能描述  : ETB停止采集
 输入参数  : 无
 输出参数  : 无
 返 回 值  : u32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年3月26日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
s32 utrace_etb_stop(void)
{
    int i;
    u32 reg_value;

    /* FFCR StopOnFl */
    utrace_reg_setbits(TRACE_ETF, ETF_FORMAT_FLUSH_CTRL, 12, 1, 1);
    /* FFCR FlushMem */
    utrace_reg_setbits(TRACE_ETF, ETF_FORMAT_FLUSH_CTRL, 6, 1, 1);

    /* 等待TMCReady */
    for(i=0; i<MAX_WAIT_CNT; i++)
    {
        utrace_reg_read(TRACE_ETF, ETF_STATUS, &reg_value);
        /* bit2为TMCReady指示位 */
        if(0 != (reg_value & 0x4))
        {
            break;
        }
    }

    /* 超时判断 */
    if(i >= MAX_WAIT_CNT)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_UTRACE, "%s: stop etb timeout\n", __FUNCTION__);
        return BSP_ERROR;
    }

    return BSP_OK;
}

/*****************************************************************************
 函 数 名  : utrace_etb_disable
 功能描述  : ETB去使能
 输入参数  : 无
 输出参数  : 无
 返 回 值  : u32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年3月26日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void utrace_etb_disable(void)
{
    /* 去使能ETB */
    utrace_reg_write(TRACE_ETF, ETF_CTRL, 0);
}

/*****************************************************************************
 函 数 名  : utrace_etr_enable
 功能描述  : ETR使能
 输入参数  : 无
 输出参数  : 无
 返 回 值  : u32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年3月26日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void utrace_etr_enable(void)
{
    utrace_reg_write(TRACE_ETR, ETR_CTRL, 0x1);
}

/*****************************************************************************
 函 数 名  : utrace_etr_disable
 功能描述  : ETR去使能
 输入参数  : 无
 输出参数  : 无
 返 回 值  : s32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年3月26日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
s32 utrace_etr_disable(void)
{
    int i;
    u32 reg_value;

    /* FFCR StopOnFl */
    utrace_reg_setbits(TRACE_ETR, ETR_FORMAT_FLUSH_CTRL, 12, 1, 1);
    /* FFCR FlushMem */
    utrace_reg_setbits(TRACE_ETR, ETR_FORMAT_FLUSH_CTRL, 6, 1, 1);
    /* 等待TMCReady */
    for(i=0; i<MAX_WAIT_CNT; i++)
    {
        utrace_reg_read(TRACE_ETR, ETR_STATUS, &reg_value);
        /* bit2为TMCReady指示位 */
        if(0 != (reg_value & 0x4))
        {
            break;
        }
    }

    /* 超时判断 */
    if(i >= MAX_WAIT_CNT)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_UTRACE, "%s: stop etr timeout\n", __FUNCTION__);
        return BSP_ERROR;
    }

    /* 去使能ETR */
    utrace_reg_write(TRACE_ETR, ETR_CTRL, 0);

    return BSP_OK;
}


/*****************************************************************************
 函 数 名  : utrace_dump_etb
 功能描述  : 系统异常，停止Trace采集，保存信息
 输入参数  : 无
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年3月26日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
s32 utrace_dump_etb(void)
{
    u32     reg_value;
    u32     i;
    u32  *  data;

    utrace_reg_unlock();
    
    /* 停止ETB */
    if(BSP_OK != utrace_etb_stop())
    {
        utrace_reg_lock();
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_UTRACE, "%s: stop etb fail\n", __FUNCTION__);
        return BSP_ERROR;
    }
#if 0
    /* 将数据读空，保持ETB读写指针相等 */
    for(i=0; i<UTRACE_ONSTART_BUF_SIZE/4; i++)
    {
        utrace_reg_read(TRACE_ETF, ETF_RAM_RD_DATA, &reg_value);
        if(reg_value == 0xffffffff)
        {
            break;
        }                
    }

    utrace_etb_disable();
#endif    
    memset((void *)g_ul_etb_data_base, 0x0, g_ul_etb_data_len);
    data = (u32 *)(g_ul_etb_data_base + 8);
    for(i=0; i<UTRACE_ONSTART_BUF_SIZE/4; i++)
    {
        utrace_reg_read(TRACE_ETF, ETF_RAM_RD_DATA, &reg_value);
        *data = reg_value;
        data++;
        if(reg_value == 0xffffffff)
        {
            break;
        }          
    }

    /* 0-3字节存放标识码 */
    *((u32 *)g_ul_etb_data_base) = (u32)UTRACE_MAGIC_NUM;
    /* 4-7个字节存放ETB数据长度 */
    *((u32 *)g_ul_etb_data_base + 1) = i*4; /* [false alarm]:屏蔽Fortify错误 */

    printk("%s: dump etb data ok(dst_buf addr: 0x%x)\n", __FUNCTION__, g_ul_etb_data_base);
    
    utrace_reg_lock();

    return BSP_OK;
}

/*****************************************************************************
 函 数 名  : utrace_capt_config
 功能描述  : Trace采集，配置接口
 输入参数  : 无
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年3月26日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void utrace_capt_config(void)
{
    utrace_reg_unlock();
    utrace_etb_disable();
    utrace_etb_config();
    utrace_etb_enable();
    utrace_funnel_config(FUNNEL_PTM_APPA9_PORT);
    utrace_ptm_disable(TRACE_PTM0);
    utrace_ptm_config(TRACE_PTM0);
    utrace_ptm_enable(TRACE_PTM0);
    utrace_reg_lock();
}

/*****************************************************************************
 函 数 名  : bsp_utrace_init
 功能描述  : 初始化
 输入参数  : 无
 输出参数  : 无
 返 回 值  : s32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年3月26日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
s32 bsp_utrace_init(void)
{
    dump_nv_s    dump_nv;
    char *       dst_buf;
    u32          len;
    struct clk * cs_clk;

    if(true == g_ul_init_flag)
    {
        return BSP_OK;
    }
    
    /*lint -save -e539*/
	cs_clk = clk_get(NULL, "cs_clk");
	if(BSP_OK != clk_enable(cs_clk))
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_UTRACE, "%s: CoreSight clk open failed.\n", __FUNCTION__);
		return BSP_ERROR;
	}
    /*lint -restore +e539*/

    /* 获取异常备份区 */
    if(BSP_OK != bsp_dump_get_buffer(DUMP_SAVE_MOD_UTRACE, &dst_buf, &len))
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_UTRACE, "%s:  get dump buffer fail\n", __FUNCTION__);
        return BSP_ERROR;
    }
    if(len < UTRACE_ONSTART_BUF_SIZE)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_UTRACE, "%s:  dump buffer(0x%x) less than data buffer(0x%x)\n", __FUNCTION__, len, UTRACE_ONSTART_BUF_SIZE);
        return BSP_ERROR;
    }
    g_ul_etb_data_base = (u32)dst_buf;
    g_ul_etb_data_len  = len;

    /* 通过NV控制监控是否启用 */
    if(BSP_OK != bsp_nvm_read(NVID_DUMP, (u8 *)&dump_nv, sizeof(dump_nv_s)))
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_UTRACE, "%s:  read nv %d fail\n", __FUNCTION__, NVID_DUMP);
        return BSP_ERROR;
    }

    /* 监控APP A9 */
    if((1 == dump_nv.traceOnstartFlag) && (APPA9_ONLY == dump_nv.traceCoreSet))
    {
        g_ul_capt_start_flag = true;
        utrace_capt_config();
    }
    else
    {
        g_ul_init_flag = true;
        return BSP_OK;
    }

    /* EXC DUMP注册 */
    if(bsp_dump_register_hook(DUMP_SAVE_MOD_UTRACE, (dump_save_hook)utrace_dump_etb) != BSP_OK)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_UTRACE, "%s:  dump register fail\n", __FUNCTION__);
        return BSP_ERROR;
    }
    
    g_ul_init_flag = true;

    printk("utrace init ok\n");
    return BSP_OK;
}

/*****************************************************************************
 函 数 名  : bsp_utrace_stop
 功能描述  : 对外接口，停止Trace采集
 输入参数  : 无
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年3月26日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void bsp_utrace_stop(void)
{
    if(true == g_ul_capt_start_flag)
    {
        utrace_reg_unlock();
        utrace_ptm_disable(TRACE_PTM0);
        utrace_reg_lock();
    }
}

/*****************************************************************************
 函 数 名  : utrace_show_etb_base
 功能描述  : debug接口，获取ETB数据存放基地址
 输入参数  : 无
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年3月26日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void utrace_show_etb_base(void)
{   
    printk("etb base addr: 0x%x len: 0x%x\n", g_ul_etb_data_base, g_ul_etb_data_len);
}

/*****************************************************************************
 函 数 名  : bsp_utrace_suspend
 功能描述  : 低功耗流程，去使能Trace采集，保存ETB数据
 输入参数  : 无
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年3月26日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void bsp_utrace_suspend(void)
{
#if 0    
    int i;
    u32 reg_value;
#endif
    if(false == g_ul_capt_start_flag)
    {
        return;
    }

    utrace_reg_unlock();
    utrace_ptm_disable(TRACE_PTM0);   
    utrace_etb_stop();
    utrace_etb_disable();
#if 0    
    for(i=0; i<UTRACE_ONSTART_BUF_SIZE/4; i++)
    {
        utrace_reg_read(TRACE_ETF, ETF_RAM_RD_DATA, &reg_value);
        if(reg_value == 0xffffffff)
        {
            break;
        }
        *((u32 *)g_ul_etb_data_base + i) = reg_value;
    }

    g_ul_etb_len = i;
#endif    
    utrace_reg_lock();   
}

/*****************************************************************************
 函 数 名  : bsp_utrace_resume
 功能描述  : 低功耗流程，恢复Trace采集
 输入参数  : 无
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年3月26日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void bsp_utrace_resume(void)
{
#if 0    
    int i;
    u32 reg_value;
#endif
    if(false == g_ul_capt_start_flag)
    {
        return;
    }
    
    utrace_reg_unlock();
    utrace_etb_disable();
    utrace_etb_config();

#if 0
    for(i=0; i<(int)g_ul_etb_len; i++)
    {
        reg_value = *((u32 *)g_ul_etb_data_base + i);
        utrace_reg_write(TRACE_ETF, ETF_RAM_WR_DATA, reg_value);
    }
#endif    
    utrace_etb_enable();
    utrace_funnel_config(FUNNEL_PTM_APPA9_PORT);
    utrace_ptm_disable(TRACE_PTM0);
    utrace_ptm_config(TRACE_PTM0);
    utrace_ptm_enable(TRACE_PTM0);

    utrace_reg_lock();
}

#else


void utrace_reg_unlock(void)
{
    /* PTM1解锁 */
    utrace_reg_write(TRACE_PTM1, PTM_LOCK_ACCESS, UTRACE_UNLOCK_CODE);
    /* FUNNEL解锁 */
    utrace_reg_write(TRACE_FUNNEL, FUNNEL_LOCK_ACCESS, UTRACE_UNLOCK_CODE);
}


void utrace_reg_lock(void)
{
    /* PTM1锁定 */
    utrace_reg_write(TRACE_PTM1, PTM_LOCK_ACCESS, UTRACE_LOCK_CODE);
    /* FUNNEL锁定 */
    utrace_reg_write(TRACE_FUNNEL, FUNNEL_LOCK_ACCESS, UTRACE_LOCK_CODE);
}


void utrace_capt_config(void)
{
    utrace_reg_unlock();
    utrace_funnel_config(FUNNEL_PTM_MODEMA9_PORT);
    utrace_ptm_disable(TRACE_PTM1);
    utrace_ptm_config(TRACE_PTM1);
    utrace_ptm_enable(TRACE_PTM1);
    utrace_reg_lock();
}


void bsp_utrace_stop(void)
{
    if(true == g_ul_capt_start_flag)
    {
        utrace_reg_unlock();
        utrace_ptm_disable(TRACE_PTM1);
        utrace_reg_lock();
    }
}

extern void top_tmc_enable(void);
extern void top_tmc_disable(void);


s32 bsp_utrace_init(void)
{
    dump_nv_s    dump_nv;

    if(true == g_ul_init_flag)
    {
        printk("bsp_utrace_init %p, %p.\n", g_a9ptmBase, g_a9funnelBase);
        return BSP_OK;
    }

    g_a9ptmBase = ioremap(HI_CORESIGHT_PTM1_BASE_ADDR, HI_CORESIGHT_PTM1_SIZE);
    g_a9funnelBase = ioremap(HI_CORESIGHT_FUNNEL_BASE_ADDR, HI_CORESIGHT_FUNNEL_SIZE);
    
    /* 通过NV控制监控是否启用 */
    if(BSP_OK != bsp_nvm_read(NVID_DUMP, (u8 *)&dump_nv, sizeof(dump_nv_s)))
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_UTRACE, "%s:  read nv %d fail\n", __FUNCTION__, NVID_DUMP);
        return BSP_ERROR;
    }
    
    /* 监控Modem A9 */
    if((1 == dump_nv.traceOnstartFlag) && (MODEMA9_ONLY == dump_nv.traceCoreSet))
    {
        printk("g_ul_capt_start_flag = true; ok\n");
        top_tmc_enable();
    }
    
    g_ul_init_flag = true;

    printk("utrace init ok\n");
    return BSP_OK;
}


void a9_utrace_start(void)
{
    top_tmc_enable();
    utrace_capt_config();
    g_ul_capt_start_flag = true;
}

void a9_utrace_stop(void)
{
    bsp_utrace_stop();
    top_tmc_disable();
    g_ul_capt_start_flag = false;
}


#endif

/*lint -save -e19*/
module_init(bsp_utrace_init);
/*lint -restore +e19*/

#ifdef __cplusplus
}
#endif

