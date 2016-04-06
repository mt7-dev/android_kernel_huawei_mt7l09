

#ifdef __cplusplus
extern "C"
{
#endif
/*lint -save -e537*/
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
#include "bsp_socp.h"
#include "bsp_om_api.h"
#include "bsp_version.h"
#include "amon_balong.h"

/*lint -restore*/
/******************************全局变量定义***********************************/
/*lint -save -e40*/
/* 初始化标志 */
int g_sl_init_flag = false;
/* 全局统计 */
axi_global_stat_t   g_st_stat_info;
/* 循环buffer信息维护 0:CPUFAST, 1:SOC*/
ring_buf_info_t     g_st_ring_buf_info[2];
/* EDMA通道号, 0:CPUFAST通道, 1:SOC通道 */
s32 g_sl_edma_channel[2] = {-1, -1};
/* 监控功能下，ID使能标识 */
u32 g_ul_id_en[AXI_MAX_ID] = {0};
/* 采集状态记录 */
axi_capt_state_enum_uint32 g_st_capt_buf_state = AXI_CAPT_NORMAL;
/* SOCP定时器 */
struct softtimer_list g_st_socp_timer;
/* Monitor时钟 */
axi_clk_ctrl_t  g_st_amon_clk;
/* K3V3 version */
u32 g_ul_k3_flag = false;

extern amon_config_stru g_amon_config;

/**********************************宏定义***********************************/
/* 初始化检查宏定义 */
#define AXI_CHECK_INIT() \
do{\
    if(false == g_sl_init_flag)\
    {\
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: not inited\n", __FUNCTION__);\
        return BSP_ERROR;\
    }\
}while(0)

/* AXI状态检查，是否正在运行，在运行时，不允许配置或者获取统计数据 */
#define AXI_CHECK_STATE(config) \
do{\
    if(BSP_OK != axi_state_check(config))\
    {\
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: AXI %d is running\n", __FUNCTION__, config);\
        return BSP_ERROR;\
    }\
}while(0)  

/* MODE检查，只支持SOC、CPUFAST */
#define AXI_CHECK_MODE(mode) \
do{\
    if(mode >= AXI_CONFIG_BUTT)\
    {\
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: AXI mode %d is invalid\n", __FUNCTION__, mode);\
        return BSP_ERROR;\
    }\
}while(0)

/*****************************************************************************
 函 数 名  : axi_reg_read
 功能描述  : AXI寄存器读操作, 一次只能读一类寄存器（CPUFAST/SOC）
 输入参数  : axi_config_enum_uint32 config
             u32 reg
 输出参数  : u32 * value
 返 回 值  : void
 调用函数  :
 被调函数  :
 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void axi_reg_read(axi_config_enum_uint32 config, u32 reg, u32 * value)
{
    /* 根据配置目标读取相应寄存器 */
    switch(config)
    {
        /* 读取CPUFAST寄存器 */
        case AXI_CPUFAST_CONFIG:
            *value = readl(AXI_MON_CPUFAST_BASE_ADDR + reg);
            break;

        /* 读取SOC寄存器 */
        case AXI_SOC_CONFIG:
            *value = readl(AXI_MON_SOC_BASE_ADDR + reg);
            break;

        default:
            break;
    }
}

/*****************************************************************************
 函 数 名  : axi_reg_write
 功能描述  : AXI寄存器写操作，一次只能写一类寄存器（CPUFAST/SOC）
 输入参数  : axi_config_enum_uint32 config
             u32 reg
             u32 value
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void axi_reg_write(axi_config_enum_uint32 config, u32 reg, u32 value)
{
    /* 根据配置目标写入相应寄存器 */
    switch(config)
    {
        /* 写CPUFAST寄存器 */
        case AXI_CPUFAST_CONFIG:
            writel(value, AXI_MON_CPUFAST_BASE_ADDR + reg);
            break;

        /* 写SOC寄存器 */
        case AXI_SOC_CONFIG:
            writel(value, AXI_MON_SOC_BASE_ADDR + reg);
            break;

        default:
            break;
    }
}

/*****************************************************************************
 函 数 名  : axi_reg_getbits
 功能描述  : AXI寄存器读位操作，一次只能读一类寄存器（CPUFAST/SOC）
 输入参数  : axi_config_enum_uint32 config
             u32 reg
             u32 pos
             u32 bits
 输出参数  : u32 * value
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void axi_reg_getbits(axi_config_enum_uint32 config, u32 reg, u32 pos, u32 bits, u32 * value)
{
    u32 reg_value = 0;

    /* 根据配置目标读取相应寄存器 */
    axi_reg_read(config, reg, &reg_value);
    /* 根据寄存器值取相应位 */
    *value = (reg_value >> pos) & (((u32)1 << (bits)) - 1);
}

/*****************************************************************************
 函 数 名  : axi_reg_setbits
 功能描述  : AXI寄存器写位操作，一次只能写一类寄存器（CPUFAST/SOC）
 输入参数  : axi_config_enum_uint32 config
             u32 reg
             u32 pos
             u32 bits
             u32 value
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void axi_reg_setbits(axi_config_enum_uint32 config, u32 reg, u32 pos, u32 bits, u32 value)
{
    u32 reg_value = 0;

    /* 根据配置目标读取相应寄存器 */
    axi_reg_read(config, reg, &reg_value);
    /* 计算写入寄存器的目标值 */
    reg_value = (reg_value & (~((((u32)1 << (bits)) - 1) << (pos)))) | ((u32)((value) & (((u32)1 << (bits)) - 1)) << (pos));
    /* 写入目的寄存器 */
    axi_reg_write(config, reg, reg_value);
}

/*****************************************************************************
 函 数 名  : axi_sc_clk_open
 功能描述  : 打开系统控制器AXI monitor时钟
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void axi_sc_clk_open(axi_config_enum_uint32 config)
{  
    if(g_ul_k3_flag)
    {
        hi_syscrg_amon_soc_clk_enable();
        g_st_amon_clk.is_clk_enable[config] = true;
    }
    else
    {
        /* coverity[check_return] */
        clk_enable(g_st_amon_clk.sc_clk[config]);
        g_st_amon_clk.is_clk_enable[config] = true; 
    }    
}

/*****************************************************************************
 函 数 名  : axi_sc_clk_close
 功能描述  : 关闭系统控制器AXI monitor时钟
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void axi_sc_clk_close(axi_config_enum_uint32 config)
{  
    if(g_ul_k3_flag)
    {
        hi_syscrg_amon_soc_clk_disable();
        g_st_amon_clk.is_clk_enable[config] = false;
    }
    else
    {    
        clk_disable(g_st_amon_clk.sc_clk[config]);
        g_st_amon_clk.is_clk_enable[config] = false;  
    }
}

/*****************************************************************************
 函 数 名  : axi_sc_mon_start
 功能描述  : AXI Monitor启动通过系统控制器控制
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void axi_sc_mon_start(axi_config_enum_uint32 config)
{
    /* 启动CPUFAST */
    if(AXI_CPUFAST_CONFIG == config)
    {
        hi_syssc_amon_cpufast_start();
    }
    /* 启动SOC */
    else if(AXI_SOC_CONFIG == config)
    {
        hi_syssc_amon_soc_start();
    }
    else
    {
        return;
    }
}

/*****************************************************************************
 函 数 名  : axi_sc_mon_stop
 功能描述  : AXI Monitor停止通过系统控制器控制
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void axi_sc_mon_stop(axi_config_enum_uint32 config)
{
    /* 停止CPUFAST */
    if(AXI_CPUFAST_CONFIG == config)
    {
        hi_syssc_amon_cpufast_stop();
    }
    /* 停止SOC */
    else if(AXI_SOC_CONFIG == config)
    {
        hi_syssc_amon_soc_stop();
    }
    else
    {
        return;
    }
}

/*****************************************************************************
 函 数 名  : axi_sc_reset
 功能描述  : monitor 软复位，每次启动采集之前，系统控制器软复位monitor
             复位完成之后需要解复位才能正常使用
             回片后新增需求，必须保证两个monitor同时复位
 输出参数  : 无
 返 回 值  : u32 复位
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void axi_sc_reset(void)
{
    if(!g_ul_k3_flag)
    {
        /* CPUFAST */
        /* 软复位 */
        hi_syscrg_amon_cpufast_soft_reset();
        /* 解复位 */
        hi_syscrg_amon_cpufast_rls_reset();
    }

    /* SOC */
    /* 软复位 */
    hi_syscrg_amon_soc_soft_reset();
    /* 解复位 */
    hi_syscrg_amon_soc_rls_reset();
}

/*****************************************************************************
 函 数 名  : axi_reset_pkg_buf
 功能描述  : 清空Monitor组包buffer，FIFO深度为AXI_FIFO_DEPTH
             读AXI_FIFO_DEPTH个数据即可清空，要求在配置Monitor之前执行该操作
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void axi_reset_pkg_buf(axi_config_enum_uint32 config)
{
    int i;
    u32 reg_base;

    /* CPUFAST */
    if(AXI_CPUFAST_CONFIG == config)
    {
        reg_base = AXI_MON_CPUFAST_BASE_ADDR;
    }
    /* SOC */
    else
    {
        reg_base = AXI_MON_SOC_BASE_ADDR;
    }

    /* 读FIFO，清空组包buffer */
    for(i=0; i<AXI_FIFO_DEPTH; i++)
    {
        readl(reg_base + AXI_CAPT_FIFO_ADDR);
    }
}

/*****************************************************************************
 函 数 名  : axi_get_data_buf
 功能描述  : 获取循环buffer数据区域信息
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void axi_get_data_buf(ring_data_buf_t * data_buf, ring_buf_info_t * ring_buf)
{
    /* 写指针大于等于读指针，直接计算 */
    if(ring_buf->write >= ring_buf->read)
    {
        data_buf->p_buf_1 = ring_buf->read;
        data_buf->size_1  = (unsigned int)(ring_buf->write - ring_buf->read);
        data_buf->p_buf_2 = NULL;
        data_buf->size_2  = 0;
    }
    /* 读指针大于写指针，需要考虑回卷 */
    else
    {
        data_buf->p_buf_1 = ring_buf->read;
        data_buf->size_1  = (unsigned int)(ring_buf->end - ring_buf->read + 1);
        data_buf->p_buf_2 = ring_buf->start;
        data_buf->size_2  = (unsigned int)(ring_buf->write - ring_buf->start);;
    }
}

/*****************************************************************************
 函 数 名  : axi_get_idle_buf
 功能描述  : 获取循环buffer空闲区域信息
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void axi_get_idle_buf(ring_idle_buf_t * idle_buf, ring_buf_info_t * ring_buf)
{
    /* 读指针大于写指针，直接计算 */
    if(ring_buf->write < ring_buf->read)
    {
        idle_buf->p_buf_1 = ring_buf->write;
        idle_buf->size_1  = (u32)(ring_buf->read - ring_buf->write - 1);
        idle_buf->p_buf_2 = NULL;
        idle_buf->size_2  = 0;
    }
    /* 写指针大于等于读指针，可能回卷 */
    else
    {
        /* read指针与起始指针不相等，写数据可能回卷 */
        if(ring_buf->read != ring_buf->start)
        {
            idle_buf->p_buf_1 = ring_buf->write;
            idle_buf->size_1  = (u32)(ring_buf->end - ring_buf->write + 1);
            idle_buf->p_buf_2 = ring_buf->start;
            idle_buf->size_2  = (u32)(ring_buf->read - ring_buf->start - 1);;
        }
        else
        {
            idle_buf->p_buf_1 = ring_buf->write;
            idle_buf->size_1  = (u32)(ring_buf->end - ring_buf->write);
            idle_buf->p_buf_2 = NULL;
            idle_buf->size_2  = 0;
        }
    }
}

/*****************************************************************************
 函 数 名  : axi_socp_src_chan_init
 功能描述  : monitor socp源通道初始化
 输出参数  : 无
 返 回 值  : s32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
s32 axi_socp_src_chan_init(void)
{
    SOCP_CODER_SRC_CHAN_STRU socp_chan;
    u8 * bd_addr = NULL;
    u8 * rd_addr = NULL;

    /* 申请BD空间 */
    bd_addr = (u8 *)cacheDmaMalloc((u32)SOCP_BD_BUF_SIZE);

    if(NULL == bd_addr)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: alloc BD buffer fail\n", __FUNCTION__);
        return BSP_ERROR;
    }

    /* 申请RD空间 */
    rd_addr = (u8 *)cacheDmaMalloc((u32)SOCP_RD_BUF_SIZE);

    if(NULL == rd_addr)
    {
        free((void *)bd_addr);
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: alloc RD buffer fail\n", __FUNCTION__);
        return BSP_ERROR;
    }
#if (FEATURE_OFF == FEATURE_MERGE_OM_CHAN)
    socp_chan.u32DestChanID = SOCP_CODER_DST_LOM_IND;    /*  目标通道ID */
#else
    socp_chan.u32DestChanID = SOCP_CODER_DST_OM_IND;    /*  目标通道ID */
#endif
    socp_chan.eDataType     = SOCP_DATA_TYPE_2;          /*  数据类型，指明数据封装协议，用于复用多平台 */
    socp_chan.eMode         = SOCP_ENCSRC_CHNMODE_LIST;  /*  通道数据模式 */
    socp_chan.ePriority     = SOCP_CHAN_PRIORITY_3;      /*  通道优先级 */
    socp_chan.u32BypassEn   = SOCP_HDLC_ENABLE;          /*  通道bypass使能 */
    socp_chan.eDataTypeEn   = SOCP_DATA_TYPE_EN;         /*  数据类型使能位 */
    socp_chan.eDebugEn      = SOCP_ENC_DEBUG_DIS;        /*  调试位使能 */

    socp_chan.sCoderSetSrcBuf.u32InputStart  = (u32)AXI_VITR_TO_PHYS(bd_addr);                         /* 输入通道起始地址 */
    socp_chan.sCoderSetSrcBuf.u32InputEnd    = (u32)AXI_VITR_TO_PHYS(bd_addr) + SOCP_BD_BUF_SIZE -1;   /* 输入通道结束地址 */
    socp_chan.sCoderSetSrcBuf.u32RDStart     = (u32)AXI_VITR_TO_PHYS(rd_addr);                         /* RD buffer起始地址 */
    socp_chan.sCoderSetSrcBuf.u32RDEnd       = (u32)AXI_VITR_TO_PHYS(rd_addr) + SOCP_RD_BUF_SIZE -1;   /* RD buffer结束地址 */
    socp_chan.sCoderSetSrcBuf.u32RDThreshold = 0;                                                                  /* RD buffer数据上报阈值 */

    /* 配置编码源通道 */
    if (BSP_OK != bsp_socp_coder_set_src_chan(AXI_SOCP_CHAN_ID, &socp_chan))
    {
        free((void *)bd_addr);
        free((void *)rd_addr);
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: set socp src chan fail\n", __FUNCTION__);
        return BSP_ERROR;
    }

    /* 启动编码源通道 */
    if(BSP_OK != bsp_socp_start(AXI_SOCP_CHAN_ID))
    {
        free((void *)bd_addr);
        free((void *)rd_addr);        
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: start socp src chan fail\n", __FUNCTION__);
        return BSP_ERROR;
    }

    return BSP_OK;
}

/*****************************************************************************
 函 数 名  : axi_gen_bd_packet
 功能描述  : 根据缓存数据组SOCP BD包
 输出参数  : tran_len: 组包成功长度
 返 回 值  : u32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
s32 axi_gen_bd_packet(ring_data_buf_t * data_buf, u32 * tran_len)
{
    u32 i;
    u32 cnt;
    u32 bd_cnt;
    u32 data_len = 0;
    u32 addr;
    u32 addr_tmp;
    SOCP_BUFFER_RW_STRU  wr_buf  = {0};
    SOCP_BD_DATA_STRU bd_data = {0};

    /* 获取SOCP BD buffer */
    if(BSP_OK != bsp_socp_get_write_buff(AXI_SOCP_CHAN_ID, &wr_buf))
    {
        g_st_stat_info.socp_tran_fail++;
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: get write buffer fail\n", __FUNCTION__);
        return BSP_ERROR;
    }

    if(wr_buf.u32Size + wr_buf.u32RbSize < SOCP_BD_PACKET_SIZE)
    {
        g_st_stat_info.socp_buf_full++;
        return BSP_ERROR;
    }

    /* 缓存数据组包, 每2k组一个包，回卷处需要分包 */
    cnt = data_buf->size_1/AXI_SOCP_PACKET_SIZE;
    if(0 != data_buf->size_1%AXI_SOCP_PACKET_SIZE)
    {
        cnt++;
    }
    bd_cnt = (wr_buf.u32Size + wr_buf.u32RbSize)/SOCP_BD_PACKET_SIZE;
    cnt = (cnt > bd_cnt)?bd_cnt:cnt;

    /* SOCP空闲区域首地址 */
    addr = (u32)AXI_PHYS_TO_VIRT(wr_buf.pBuffer);
    /* SOCP空闲区域回卷地址 */
    addr_tmp = (u32)AXI_PHYS_TO_VIRT(wr_buf.pBuffer) + wr_buf.u32Size;

    for(i=0; i<cnt; i++)
    {
        memset(&bd_data, 0x0, sizeof(SOCP_BD_DATA_STRU));
        /* BD包需要填写物理地址 */
        bd_data.pucData    = (u32)(data_buf->p_buf_1 + AXI_SOCP_PACKET_SIZE * i);

        /* 缓存回卷处，计算包的长度 */
        if((u32)(AXI_SOCP_PACKET_SIZE * (i+1)) > data_buf->size_1)
        {
            bd_data.usMsgLen   = data_buf->size_1%AXI_SOCP_PACKET_SIZE;
        }
        else
        {
            bd_data.usMsgLen   = AXI_SOCP_PACKET_SIZE;
        }
        bd_data.enDataType = SOCP_BD_DATA;
        memcpy((void *)addr, &bd_data, SOCP_BD_PACKET_SIZE);
        addr += SOCP_BD_PACKET_SIZE;

        /* SOCP BD buffer有回卷 */
        if(addr >= addr_tmp)
        {
            addr = (u32)AXI_PHYS_TO_VIRT(wr_buf.pRbBuffer);
        }

        data_len += bd_data.usMsgLen;
    }

    if(BSP_OK != bsp_socp_write_done(AXI_SOCP_CHAN_ID, cnt * SOCP_BD_PACKET_SIZE))
    {
        g_st_stat_info.socp_tran_fail++;
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: write buffer done fail\n", __FUNCTION__);
        return BSP_ERROR;
    }

    *tran_len = data_len;

    return BSP_OK;
}

/*****************************************************************************
 函 数 名  : axi_data_buf_update
 功能描述  : 处理缓存中已有数据，更新读指针
 输出参数  : 无
 返 回 值  : u32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
s32 axi_data_buf_update(ring_buf_info_t * ring_buf)
{
    ring_data_buf_t data_buf = {0};
    SOCP_BUFFER_RW_STRU socp_buf;
    u32 tran_len;

    /* 清RD */
    if(BSP_OK != bsp_socp_get_rd_buffer(AXI_SOCP_CHAN_ID, &socp_buf))
    {
        g_st_stat_info.socp_tran_fail++;
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: get rd buffer fail\n", __FUNCTION__);
        return BSP_ERROR;
    }

    /* 无数据时不需要清RD */
    if(0 != socp_buf.u32Size + socp_buf.u32RbSize)
    {
        if(BSP_OK != bsp_socp_read_rd_done(AXI_SOCP_CHAN_ID, socp_buf.u32Size + socp_buf.u32RbSize))
        {
            g_st_stat_info.socp_tran_fail++;
            bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: read rd done fail\n", __FUNCTION__);
            return BSP_ERROR;
        }
    }

    /* 获取缓存数据空间 */
    axi_get_data_buf(&data_buf, ring_buf);
    /* 无数据 */
    if(0 == data_buf.size_1 && 0 == data_buf.size_2)
    {
        return BSP_OK;
    }

    /* 构建BD包 */
    if(BSP_OK != axi_gen_bd_packet(&data_buf, &tran_len))
    {
        return BSP_ERROR;
    }

    /* 更新buffer读指针 */
    ring_buf->read += tran_len;
    if(ring_buf->read > ring_buf->end)
    {
        ring_buf->read -= ring_buf->end - ring_buf->start + 1;
    }

    return BSP_OK;
}

/*****************************************************************************
 函 数 名  : axi_edma_get_tran_len
 功能描述  : 获取传输长度，用于配置EDMA
 输出参数  : 无
 返 回 值  : s32 配置给EDMA的传输长度，返回-1表示无空闲
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
s32 axi_edma_get_tran_len(ring_buf_info_t * ring_buf)
{
    ring_idle_buf_t idle_buf;

    axi_get_idle_buf(&idle_buf, ring_buf);
    /* 空闲空间不足 */
    if(idle_buf.size_1 + idle_buf.size_2 < AXI_CAPT_THRESHOLD)
    {
        return BSP_ERROR;
    }
    /* 写指针到end指针之间的空间小于阈值，配置两个指针之间的长度 */
    if(idle_buf.size_1 < AXI_CAPT_THRESHOLD)
    {
        return (s32)idle_buf.size_1;
    }
    /* 配置阈值大小 */
    else
    {
        return AXI_CAPT_THRESHOLD;
    }
}

/*****************************************************************************
 函 数 名  : axi_get_edma_cnt
 功能描述  : 获取edma剩余传输长度
 输出参数  : 无
 返 回 值  : u32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
u32 axi_get_edma_cnt(u32 channel_id)
{
	u32 left_cnt;
	
	left_cnt = (u32)bsp_edma_current_cnt(channel_id);

    return left_cnt;
}

/*****************************************************************************
 函 数 名  : axi_socp_timer_handler
 功能描述  : socp timer处理函数
 输出参数  : 无
 返 回 值  : u32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void axi_socp_timer_handler(u32 param)
{
    s32 ret;
    s32 tran_len;
    u32 edma_src_addr;
    axi_config_enum_uint32 axi_mode;
    
    axi_mode = g_st_stat_info.capt_mode;
    if(AXI_SOC_CONFIG == axi_mode)
    {
        edma_src_addr = SOC_BUFF_BASE_ADDR;
    }
    else
    {
        edma_src_addr = CPUFAST_BUFF_BASE_ADDR;
    }    

    (void)axi_data_buf_update(&g_st_ring_buf_info[axi_mode]);
    
    tran_len = axi_edma_get_tran_len(&g_st_ring_buf_info[axi_mode]);
    if(BSP_ERROR == tran_len)
    {
        bsp_softtimer_add(&g_st_socp_timer);
        return ;
    }
    
    /* 保存当前传输长度 */
    g_st_stat_info.capt_edma_len = (u32)tran_len;
   
    /* 配置传输起始、目的地址，传输长度，使能EMDA通道 */
    ret = bsp_edma_channel_2vec_start((u32)g_sl_edma_channel[axi_mode], edma_src_addr, (u32)g_st_ring_buf_info[axi_mode].write, (u32)tran_len, g_st_stat_info.edma_bindex);
    if(EDMA_SUCCESS != ret)
    {
        g_st_stat_info.edma_tran_fail++;
    }
}

/*****************************************************************************
 函 数 名  : axi_socp_start_timer
 功能描述  : socp通道发送数据速度慢于源数据产生速度，创建timer
 输出参数  : 无
 返 回 值  : s32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
s32 axi_socp_start_timer(u32 time_out)
{  
    s32 ret = 0;
    g_st_socp_timer.func = axi_socp_timer_handler;
    g_st_socp_timer.para = 0;
    g_st_socp_timer.timeout = time_out;
    g_st_socp_timer.wake_type = SOFTTIMER_NOWAKE;

    ret =  bsp_softtimer_create(&g_st_socp_timer);
    if(ret)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: start timer fail\n", __FUNCTION__);
        return BSP_ERROR;
    }

    return BSP_OK;
}

/*****************************************************************************
 函 数 名  : axi_edma_int_handler
 功能描述  : EDMA中断处理函数
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void axi_edma_int_handler(u32 param, u32 int_status)
{
    s32 tran_len;
    s32 ret;
    u32 edma_src_addr;
    u32 edma_left_cnt;
    axi_config_enum_uint32 axi_mode;;

    axi_mode = g_st_stat_info.capt_mode;
    if(AXI_SOC_CONFIG == axi_mode)
    {
        edma_src_addr = SOC_BUFF_BASE_ADDR;
    }
    else
    {
        edma_src_addr = CPUFAST_BUFF_BASE_ADDR;
    }

    /* 读取edma上一次传输剩余长度 */
    edma_left_cnt = axi_get_edma_cnt((u32)g_sl_edma_channel[axi_mode]);
    /* 更新写指针 */
    g_st_ring_buf_info[axi_mode].write += g_st_stat_info.capt_edma_len - edma_left_cnt;
    /* 有回卷 */
    if(g_st_ring_buf_info[axi_mode].write > g_st_ring_buf_info[axi_mode].end)
    {
        g_st_ring_buf_info[axi_mode].write = g_st_ring_buf_info[axi_mode].start;
    }

    /* 组包，通过SOCP发送 */
    if(BSP_OK != axi_data_buf_update(&g_st_ring_buf_info[axi_mode]))
    {
        /* SOCP满，启动定时器 */
        bsp_softtimer_add(&g_st_socp_timer);
        return ;
    }

    /* 获取下一次传输长度 */
    tran_len = axi_edma_get_tran_len(&g_st_ring_buf_info[axi_mode]);
    if(BSP_ERROR == tran_len)
    {
        /* SOCP发送速度缓慢，剩余空间不足一次EDMA搬移长度，启动定时器 */
        g_st_stat_info.axi_buf_full++;
        bsp_softtimer_add(&g_st_socp_timer);
        return ;
    }

    /* 保存当前传输长度 */
    g_st_stat_info.capt_edma_len = (u32)tran_len;

    /* 配置传输起始、目的地址，传输长度，使能EMDA通道 */
    ret = bsp_edma_channel_2vec_start((u32)g_sl_edma_channel[axi_mode], edma_src_addr, (u32)g_st_ring_buf_info[axi_mode].write, (u32)tran_len, g_st_stat_info.edma_bindex);
    if(EDMA_SUCCESS != ret)
    {
        g_st_stat_info.edma_tran_fail++;
        return ;
    }

    g_st_stat_info.int_stat.edma_tran_over++;

    return ;
}

/*****************************************************************************
 函 数 名  : axi_edma_config_init
 功能描述  : EDMA初始化配置
 输入参数  : axi_config_enum_uint32 config
             u32 bindex EDMA二维传输步长
 输出参数  : 无
 返 回 值  : s32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
s32 axi_edma_config_init(axi_config_enum_uint32 config, u32 bindex)
{
    s32 channel_id;
    s32 ret;
    s32 tran_len;
    u32 req;
    u32 src_addr;

    if(AXI_CPUFAST_CONFIG == config)
    {
        req      = CPUFAST_EDMA_REQUEST;
        src_addr = CPUFAST_BUFF_BASE_ADDR;
    }
    else
    {
        req      = SOC_EDMA_REQUEST;
        src_addr = SOC_BUFF_BASE_ADDR;
    }

    /* 通道分配 */
    channel_id = bsp_edma_channel_init((BALONG_DMA_REQ)req, axi_edma_int_handler, config, EDMA_INT_DONE);
    if(-1 == channel_id)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s(%u): alloc edma channel fail\n", __FUNCTION__, config);
        return BSP_ERROR;
    }

    /* 通道传输配置 */
    ret = bsp_edma_channel_set_config((u32)channel_id, EDMA_P2M, EDMA_TRANS_WIDTH_64, EDMA_BUR_LEN_16);
    if(EDMA_SUCCESS != ret)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s(%u): edma set config fail\n", __FUNCTION__, config);
        return BSP_ERROR;
    }

    /* 配置传输长度 */
    tran_len = axi_edma_get_tran_len(&g_st_ring_buf_info[config]);
    if(BSP_ERROR == tran_len)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s(%u): get tran len fail\n", __FUNCTION__, config);
        return BSP_ERROR;
    }

    /* 配置传输起始、目的地址，传输长度，采用二维传输 */
    ret = bsp_edma_channel_2vec_start((u32)channel_id, src_addr, (u32)g_st_ring_buf_info[config].write, (u32)tran_len, bindex);
    if(EDMA_SUCCESS != ret)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s(%u): config src/dst fail\n", __FUNCTION__, config);
        return BSP_ERROR;
    }
    /* 保存通道ID */
    g_sl_edma_channel[config] = channel_id;

    /* 保存edma传输长度 */
    g_st_stat_info.capt_edma_len = AXI_CAPT_THRESHOLD;

    return BSP_OK;
}

/*****************************************************************************
 函 数 名  : axi_edma_free
 功能描述  : EDMA通道释放
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
s32 axi_edma_free(axi_config_enum_uint32 config)
{
    s32 addr;
    int i = 0;
    
    addr = bsp_edma_channel_stop((u32)g_sl_edma_channel[config]);

    if(addr < 0)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: stop edma channel fail\n", __FUNCTION__);
        return BSP_ERROR; 
    }
    
    /* 更新写指针 */ 
    g_st_ring_buf_info[config].write = (u8 *)addr;

    /* 传输剩余数据 */
    while((i++ < AXI_SOCP_TRAN_WAIT_CNT) && (g_st_ring_buf_info[config].write != g_st_ring_buf_info[config].read))
    {
        (void)axi_data_buf_update(&g_st_ring_buf_info[config]);
        taskDelay(10);     
    }

    if(g_st_ring_buf_info[config].write != g_st_ring_buf_info[config].read)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: socp transfer can not finish(write 0x%x read 0x%x)\n", __FUNCTION__,
                    (u32)g_st_ring_buf_info[config].write, (u32)g_st_ring_buf_info[config].read);
        return BSP_ERROR;        
    }

    if(BSP_OK != bsp_edma_channel_free((u32)g_sl_edma_channel[config]))
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: free edma channel fail\n", __FUNCTION__);
        return BSP_ERROR;
    }

    return BSP_OK;
}

/*****************************************************************************
 函 数 名  : axi_get_state
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
axi_state_enum_uint32 axi_get_state(axi_config_enum_uint32 config, axi_get_state_req_enum_uint32 state_req)
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
 函 数 名  : axi_state_check
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
s32 axi_state_check(axi_config_enum_uint32 config)
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
 函 数 名  : axi_win_over_int_handler
 功能描述  : 时间窗结束中断处理函数
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void axi_win_over_int_handler(void)
{
    u32 reg_value = 0;

    if(!g_ul_k3_flag)
    {
        /* 检查CPUFAST中断 */
        AXI_REG_GETBITS(AXI_CPUFAST_CONFIG, AXI_MON_CNT_STATE_INT, 0, 1, &reg_value);
        if(reg_value)
        {
            /* 清中断 */
            AXI_REG_SETBITS(AXI_CPUFAST_CONFIG, AXI_MON_INT_CLR, 0, 1, 0x1);
            g_st_stat_info.int_stat.win_over++;
            /* 组包上报 */
            axi_mon_info_gen_packet(AXI_CPUFAST_CONFIG);
            /* 时间窗结束，关闭时钟 */
            if(g_st_amon_clk.is_clk_enable[AXI_CPUFAST_CONFIG] == true)
            {
                axi_sc_clk_close(AXI_CPUFAST_CONFIG);
            } 
        }
    }

    /* 检查SOC中断 */
    reg_value = 0;
    AXI_REG_GETBITS(AXI_SOC_CONFIG, AXI_MON_CNT_STATE_INT, 0, 1, &reg_value);
    if(reg_value)
    {
        /* 清中断 */
        AXI_REG_SETBITS(AXI_SOC_CONFIG, AXI_MON_INT_CLR, 0, 1, 0x1);
        g_st_stat_info.int_stat.win_over++;
        /* 组包上报 */
        axi_mon_info_gen_packet(AXI_SOC_CONFIG);
        /* 时间窗结束，关闭时钟 */
        if(g_st_amon_clk.is_clk_enable[AXI_SOC_CONFIG] == true)
        {
            axi_sc_clk_close(AXI_SOC_CONFIG);
        } 
    }       
}

/*****************************************************************************
 函 数 名  : axi_tran_int_handler
 功能描述  : 采集传输中断处理函数
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void axi_tran_int_handler(void)
{
    u32 reg_value;
    u32 i;

    for(i=0; i<AXI_CONFIG_BUTT; i++)
    {
        AXI_REG_GETBITS(i, AXI_CAPT_INT_STATE, 0, 4, &reg_value);
        /* buffer满中断 */
        if(reg_value & (1 << 0))
        {
            /* 屏蔽满中断 */
            AXI_REG_SETBITS(i, AXI_CAPT_INT_MASK, 0, 1, 0x1);  
            g_st_stat_info.int_stat.buff_over++;
            g_st_capt_buf_state = AXI_CAPT_FULL;
        }
        /* 传输结束中断 */
        if(reg_value & (1 << 1))
        {
            AXI_REG_SETBITS(i, AXI_CAPT_INT_MASK, 1, 1, 0x1);
            AXI_REG_SETBITS(i, AXI_CAPT_INT_CLR, 1, 1, 0x1);
            g_st_stat_info.int_stat.tran_over++; 

            /* buffer满中断后传输完成，上报传输完成中断 */
            if(g_st_capt_buf_state & AXI_CAPT_FULL)
            {
                g_st_capt_buf_state = (g_st_capt_buf_state & ~(AXI_CAPT_FULL)) | AXI_CAPT_FINISHING;
                /* 停止采集 */
                AXI_REG_WRITE(i, AXI_CAPT_CTRL, 0x2);
            }
            /* buffer满，配置stop之后，上报传输中断，启动采集 */
            else if(g_st_capt_buf_state & AXI_CAPT_FINISHING)
            {               
                /* 清除满中断 */
                AXI_REG_SETBITS(i, AXI_CAPT_INT_CLR, 0, 1, 0x1);
                /* 清除满中断屏蔽 */
                AXI_REG_SETBITS(i, AXI_CAPT_INT_MASK, 0, 1, 0x0);   
                if(g_st_capt_buf_state & AXI_CAPT_STOP_REQ)
                {
                    g_st_stat_info.tran_over_flag = true;
                    osl_sem_up(&g_st_stat_info.capt_sem);  
                }
                else
                {
                    /* 启动采集 */
                    AXI_REG_WRITE(i, AXI_CAPT_CTRL, 0x1);
                }
                g_st_capt_buf_state = AXI_CAPT_NORMAL;
            }
            /* 周期采集stop请求，传输完成之后启动采集 */
            else if(g_st_capt_buf_state & AXI_CAPT_PERIOD_STOP_REQ)
            {
                /* 启动采集 */
                AXI_REG_WRITE(i, AXI_CAPT_CTRL, 0x1);
                g_st_capt_buf_state = AXI_CAPT_NORMAL;
            }
            /* 停止采集 */
            else
            {   
                g_st_stat_info.tran_over_flag = true;
                osl_sem_up(&g_st_stat_info.capt_sem);
            }
            AXI_REG_SETBITS(i, AXI_CAPT_INT_MASK, 1, 1, 0x0);
            AXI_REG_GETBITS(i, AXI_CAPT_INT_STATE, 0, 4, &reg_value);
        }
    }
}

/*****************************************************************************
 函 数 名  : axi_tran_int_handler_k3
 功能描述  : 采集传输中断处理函数
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void axi_tran_int_handler_k3(void)
{
    u32 reg_value;

    AXI_REG_GETBITS(AXI_SOC_CONFIG, AXI_CAPT_INT_STATE, 0, 4, &reg_value);
    /* buffer满中断 */
    if((reg_value & (1 << 0)) || (reg_value & (1 << 1)) || (reg_value & (1 << 2)))
    {
        /* 屏蔽满中断 */
        AXI_REG_SETBITS(AXI_SOC_CONFIG, AXI_CAPT_INT_MASK, 0, 3, 0xe);  
        g_st_stat_info.int_stat.buff_over++;
        g_st_capt_buf_state = AXI_CAPT_FULL;
    }
    /* 传输结束中断 */
    if(reg_value & (1 << 3))
    {
        AXI_REG_SETBITS(AXI_SOC_CONFIG, AXI_CAPT_INT_MASK, 3, 1, 0x1);
        AXI_REG_SETBITS(AXI_SOC_CONFIG, AXI_CAPT_INT_CLR, 3, 1, 0x1);
        g_st_stat_info.int_stat.tran_over++; 

        /* buffer满中断后传输完成，上报传输完成中断 */
        if(g_st_capt_buf_state & AXI_CAPT_FULL)
        {
            g_st_capt_buf_state = (g_st_capt_buf_state & ~(AXI_CAPT_FULL)) | AXI_CAPT_FINISHING;
            /* 停止采集 */
            AXI_REG_WRITE(AXI_SOC_CONFIG, AXI_CAPT_CTRL, 0x2);
        }
        /* buffer满，配置stop之后，上报传输中断，启动采集 */
        else if(g_st_capt_buf_state & AXI_CAPT_FINISHING)
        {               
            /* 清除满中断 */
            AXI_REG_SETBITS(AXI_SOC_CONFIG, AXI_CAPT_INT_CLR, 0, 3, 0xe);
            /* 清除满中断屏蔽 */
            AXI_REG_SETBITS(AXI_SOC_CONFIG, AXI_CAPT_INT_MASK, 0, 3, 0x0);   
            if(g_st_capt_buf_state & AXI_CAPT_STOP_REQ)
            {
                g_st_stat_info.tran_over_flag = true;
                osl_sem_up(&g_st_stat_info.capt_sem);  
            }
            else
            {
                /* 启动采集 */
                AXI_REG_WRITE(AXI_SOC_CONFIG, AXI_CAPT_CTRL, 0x1);
            }
            g_st_capt_buf_state = AXI_CAPT_NORMAL;
        }
        /* 周期采集stop请求，传输完成之后启动采集 */
        else if(g_st_capt_buf_state & AXI_CAPT_PERIOD_STOP_REQ)
        {
            /* 启动采集 */
            AXI_REG_WRITE(AXI_SOC_CONFIG, AXI_CAPT_CTRL, 0x1);
            g_st_capt_buf_state = AXI_CAPT_NORMAL;
        }
        /* 停止采集 */
        else
        {   
            g_st_stat_info.tran_over_flag = true;
            osl_sem_up(&g_st_stat_info.capt_sem);
        }
        AXI_REG_SETBITS(AXI_SOC_CONFIG, AXI_CAPT_INT_MASK, 3, 1, 0x0);
    }
}

/*****************************************************************************
 函 数 名  : axi_int_handler
 功能描述  : 中断处理函数
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void axi_int_handler(void)
{
    axi_tran_int_handler();
    axi_win_over_int_handler();
}

/*****************************************************************************
 函 数 名  : axi_int_handler_k3
 功能描述  : 中断处理函数
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void axi_int_handler_k3(void)
{
    axi_tran_int_handler_k3();
    axi_win_over_int_handler();
}

/*****************************************************************************
 函 数 名  : axi_reset
 功能描述  : AXI monitor软复位
 输出参数  : 无
 返 回 值  : s32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
s32 axi_reset(axi_config_enum_uint32 config)
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
    bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: time out\n", __FUNCTION__);
    return BSP_ERROR;
}

/*****************************************************************************
 函 数 名  : axi_init
 功能描述  : AXI monitor初始化
 输出参数  : 无
 返 回 值  : s32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
s32 axi_init(void)
{
    int i;
	u32 chip_type;
	
    if(true == g_sl_init_flag)
    {
        return BSP_OK;
    }

    /* 是否为K3V3版本 */
	chip_type = bsp_version_get_board_chip_type();
    if((HW_VER_K3V3_FPGA == chip_type) || (HW_VER_K3V3_UDP == chip_type))
    {
        g_ul_k3_flag = true;
    }
    
    /* 初始化SOCP通道 */
    if(BSP_OK != axi_socp_src_chan_init())
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: socp src chan init fail\n", __FUNCTION__);
        return BSP_ERROR;
    }

    /* 获取时钟 */
    if(!g_ul_k3_flag)
    {
        g_st_amon_clk.sc_clk[AXI_CPUFAST_CONFIG] = clk_get(NULL, "amon_cpufast_clk");
        g_st_amon_clk.sc_clk[AXI_SOC_CONFIG]     = clk_get(NULL, "amon_soc_clk");  
    }
    /* 打开系统时钟 */
    axi_sc_clk_open(AXI_CPUFAST_CONFIG);
    axi_sc_clk_open(AXI_SOC_CONFIG);
    /* 记录时钟状态 */
    g_st_amon_clk.is_clk_enable[AXI_CPUFAST_CONFIG] = true;
    g_st_amon_clk.is_clk_enable[AXI_SOC_CONFIG]     = true;
    
    /* 中断屏蔽 */
    AXI_REG_WRITE(AXI_SOC_CONFIG, AXI_ID_INT_MASK, 0xFFFF);
    AXI_REG_SETBITS(AXI_SOC_CONFIG, AXI_MON_CNT_CTRL, 2, 2, 0x3);
    if(g_ul_k3_flag)
    {
        AXI_REG_WRITE(AXI_SOC_CONFIG, AXI_CAPT_INT_MASK, 0x3F);
    }
    else
    {
        AXI_REG_WRITE(AXI_SOC_CONFIG, AXI_CAPT_INT_MASK, 0xF);
        AXI_REG_WRITE(AXI_CPUFAST_CONFIG, AXI_ID_INT_MASK, 0xFFFF);
        AXI_REG_WRITE(AXI_CPUFAST_CONFIG, AXI_CAPT_INT_MASK, 0xF);
        AXI_REG_SETBITS(AXI_CPUFAST_CONFIG, AXI_MON_CNT_CTRL, 2, 2, 0x3);        
    }

    if(g_ul_k3_flag)
    {
        /* 挂中断 */
        if(BSP_OK != request_irq(AXI_INT_VECTOR, (irq_handler_t)axi_int_handler_k3, 0, "AXI_MON_IRQ", NULL))
        {
            bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: INT connect fail\n", __FUNCTION__);
            return BSP_ERROR;
        }
    }
    else
    {
        /* 挂中断 */
        if(BSP_OK != request_irq(AXI_INT_VECTOR, (irq_handler_t)axi_int_handler, 0, "AXI_MON_IRQ", NULL))
        {
            bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: INT connect fail\n", __FUNCTION__);
            return BSP_ERROR;
        }
    }


    /* 信号量初始化 */
    osl_sem_init(0, &g_st_stat_info.capt_sem);

    /* 缓存空间分配，复用数采空间 */
    for(i=0; i<AXI_CONFIG_BUTT; i++)
    {
        /* 初始化循环buffer信息 */
        g_st_ring_buf_info[i].start = (u8 *)(DDR_SOCP_ADDR);
        g_st_ring_buf_info[i].end   = (u8 *)(DDR_SOCP_ADDR + DDR_SOCP_SIZE - 1);
        g_st_ring_buf_info[i].read  = (u8 *)(DDR_SOCP_ADDR);
        g_st_ring_buf_info[i].write = (u8 *)(DDR_SOCP_ADDR);
    }

    /* 全局变量初始化 */
    memset(&g_st_stat_info.int_stat, 0x0, sizeof(axi_int_stat_t));
    g_st_stat_info.capt_mode        = AXI_CONFIG_BUTT;
    g_st_stat_info.mon_mode         = AXI_CONFIG_BUTT;
    g_st_stat_info.edma_bindex      = AXI_EDMA_BINDEX;
    g_st_stat_info.capt_cycle       = AXI_CAPT_CYCLE_DEFAULT;
    g_st_stat_info.axi_buf_full     = 0;
    g_st_stat_info.edma_tran_fail   = 0;
    g_st_stat_info.socp_buf_full    = 0;
    g_st_stat_info.socp_tran_fail   = 0;
    g_st_stat_info.mon_ind_cnt      = 0;
    g_st_stat_info.mon_ind_fail_cnt = 0;
    g_st_stat_info.tran_over_flag   = true;

    g_st_capt_buf_state = AXI_CAPT_NORMAL;

    g_sl_init_flag = true;

    return BSP_OK;
}

/*****************************************************************************
 函 数 名  : axi_mon_start
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
s32 axi_mon_start(axi_config_enum_uint32 config)
{
    axi_state_enum_uint32 axi_state;
    int i = 0;

    /* 检查运行状态 */
    AXI_CHECK_STATE(config);

    /* 配置系统控制器启动Monitor */
    axi_sc_mon_start(config);

    /* 启动结束判定，等待启动标志置位 */
    do
    {
        axi_state = axi_get_state(config, AXI_GET_RUN_STATE_REQ);
        if(AXI_UNWIN_RUNNING == axi_state || AXI_WIN_RUNNING == axi_state)
        {
            return BSP_OK;
        }
    }while(i++ < AXI_WAIT_CNT);
    
    /* 启动超时异常 */
    bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: time out\n", __FUNCTION__);
    return BSP_ERROR;
}

/*****************************************************************************
 函 数 名  : axi_mon_stop
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
s32 axi_mon_stop(axi_config_enum_uint32 config)
{
    axi_state_enum_uint32 axi_state;
    int i = 0;

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
    bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: time out\n", __FUNCTION__);
    return BSP_ERROR;
}

/*****************************************************************************
 函 数 名  : axi_capt_start
 功能描述  : 启动数据采集
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
s32 axi_capt_start(axi_config_enum_uint32 axi_mode)
{
    s32 ret;
    axi_state_enum_uint32 axi_state;
    u32 lock_key;
    u32 tran_state;
    
    local_irq_save(lock_key);
    tran_state = g_st_stat_info.tran_over_flag;
    local_irq_restore(lock_key); 

    /* 启动之前检查上次传输是否结束 */
    if(false == tran_state)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: last transfer unfinished\n", __FUNCTION__);
        return BSP_ERROR;
    }  

    /* 首先获取运行状态，启动采集要求monitor已开始运行 */
    axi_state = axi_get_state(axi_mode, AXI_GET_RUN_STATE_REQ);
    if(axi_state == AXI_IDLE || axi_state == AXI_STOP)
    {
        /* 先启动monitor，监控功能采用上一次配置，不再重新配置 */
        ret = axi_mon_start(axi_mode);
        if(BSP_OK != ret)
        {
            bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: capture start fail\n", __FUNCTION__);
            return ret;
        }
    }

    /* 清中断，打开采集满中断、结束中断，保持阈值中断、请求中断屏蔽 */
    if(g_ul_k3_flag)
    {
        AXI_REG_WRITE(axi_mode, AXI_CAPT_INT_CLR, 0x3F);
        AXI_REG_WRITE(axi_mode, AXI_CAPT_INT_MASK, 0x30);
    }
    else
    {
        AXI_REG_WRITE(axi_mode, AXI_CAPT_INT_CLR, 0xF);
        AXI_REG_WRITE(axi_mode, AXI_CAPT_INT_MASK, 0xC);
    }

    /* 启动采集 */
    AXI_REG_WRITE(axi_mode, AXI_CAPT_CTRL, 0x1);

    return BSP_OK;
}

/*****************************************************************************
 函 数 名  : axi_capt_stop
 功能描述  : 停止数据采集
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
s32 axi_capt_stop(axi_config_enum_uint32 config)
{
    u32 lock_key;

    /* coverity[lock_acquire] */
    local_irq_save(lock_key);

    if(g_st_capt_buf_state != AXI_CAPT_NORMAL)
    {
        g_st_capt_buf_state |= AXI_CAPT_STOP_REQ;
    }
    else
    {
        /* 停止采集 */
        AXI_REG_WRITE(config, AXI_CAPT_CTRL, 0x2);
    }
    
    g_st_stat_info.tran_over_flag = false;
      
    local_irq_restore(lock_key); 
    
    /* 等待传输结束 */
    /* coverity[lock] */
    if(BSP_OK != osl_sem_downtimeout(&g_st_stat_info.capt_sem, AXI_CAPT_TRAN_OVER_TIME))
    {
        if(false == g_st_stat_info.tran_over_flag)
        {
            bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: capture transfer time out\n", __FUNCTION__);
            return BSP_ERROR;
        }
    }
    /* coverity[missing_unlock] */
    return BSP_OK;
}

/*****************************************************************************
 函 数 名  : axi_stop_timer
 功能描述  : 删除timer
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void axi_stop_timer(void)
{
    if(g_st_stat_info.axi_timer.init_flags != TIMER_INIT_FLAG)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: timer is null\n", __FUNCTION__);
        return;
    }

    (void)bsp_softtimer_delete_sync(&g_st_stat_info.axi_timer);
    (void)bsp_softtimer_free(&g_st_stat_info.axi_timer);
}

/*****************************************************************************
 函 数 名  : axi_timer_handler
 功能描述  : 定时器处理函数，采集周期到，需要执行stop, start操作
 输出参数  : 无
 返 回 值  : s32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void axi_timer_handler(u32 param)
{
    u32 lock_key;
 
    local_irq_save(lock_key);

    if(g_st_capt_buf_state == AXI_CAPT_NORMAL)
    {
        g_st_capt_buf_state = AXI_CAPT_PERIOD_STOP_REQ;
        /* 停止采集 */
        AXI_REG_WRITE(g_st_stat_info.capt_mode, AXI_CAPT_CTRL, 0x2);
    }
          
    local_irq_restore(lock_key); 

    /* 继续执行 */
    bsp_softtimer_add(&g_st_stat_info.axi_timer);
}

/*****************************************************************************
 函 数 名  : axi_start_timer
 功能描述  : 周期采集，创建timer
 输出参数  : 无
 返 回 值  : s32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
s32 axi_start_timer(u32 time_out)
{
    s32 ret = 0;

     g_st_stat_info.axi_timer.func = axi_timer_handler;
     g_st_stat_info.axi_timer.para = 0;
     g_st_stat_info.axi_timer.timeout = time_out;
     g_st_stat_info.axi_timer.wake_type = SOFTTIMER_NOWAKE;

    ret =  bsp_softtimer_create(&g_st_stat_info.axi_timer);
    if(ret)
    {
        return BSP_ERROR;
    }
    bsp_softtimer_add(&g_st_stat_info.axi_timer);

    return BSP_OK;
}

/*****************************************************************************
 函 数 名  : axi_capt_config
 功能描述  : 采集参数配置
 输入参数  : axi_capt_config_t * capt_config
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void axi_capt_config(axi_capt_config_t * capt_config)
{
    axi_config_enum_uint32 mode;
    u32 reg_value;
    u32 i;

    mode = capt_config->mode;

    /* 构建AXI_CAPT_TRAN_CONFIG配置:包括采集端口，DMA请求level, 操作类型 */
    /* AXI_CAPT_TRAN_CONFIG寄存器: bit5:6-dma, bit4-缓存选择, bit3-操作类型选择, bit0:2-port */
    reg_value = (capt_config->port & 0x7) | ((capt_config->op_type & 0x1) << 3) | ((capt_config->dma_req_level & 0x3) << 5);
    /* 缓存SOC, bit4清零 */
    if(mode == AXI_SOC_CONFIG)
    {
        reg_value &= 0xFFFFFFEF;
    }
    /* 缓存CPUFAST, bit4置1 */
    else
    {
        reg_value |= 0x10;
    }
    AXI_REG_WRITE(mode, AXI_CAPT_TRAN_CONFIG, reg_value);

    /* 配置Monitor阈值 */
    AXI_REG_WRITE(mode, AXI_CAPT_TRANS_DATA_LEVEL, capt_config->data_trans_thr);
    /* 配置命令包长 */
    AXI_REG_WRITE(mode, AXI_CAPT_CMD_PKG_SIZE, capt_config->cmd_pkg_len);
    /* 配置数据包长 */
    AXI_REG_WRITE(mode, AXI_CAPT_DATA_PKG_SIZE, capt_config->data_pkg_len);
    /* 配置附加包长 */
    AXI_REG_WRITE(mode, AXI_CAPT_EXT_PKG_SIZE, capt_config->ext_pkg_len);
    /* 配置DMA请求类型 */
    AXI_REG_WRITE(mode, AXI_CAPT_DMA_REQ_TYPE, capt_config->dma_req_type);

    /* 采集ID配置 */
    for(i=0; i<AXI_MAX_ID; i++)
    {
        reg_value = 0;
        if(0 != capt_config->id_cfg[i].id_en)
        {
            /* 配置ID使能 */
            AXI_REG_SETBITS(mode, AXI_CAPT_ID_EN, i, 1, 1);

            /* ID配置寄存器: bit0:14 ID, bit16:30 MASK */
            reg_value = (capt_config->id_cfg[i].id_value & 0x7FFF) | ((capt_config->id_cfg[i].mask & 0x7FFF) << 16);
            AXI_REG_WRITE(mode, AXI_CAPT_ID(i), reg_value);
        }
    }
}

/*****************************************************************************
 函 数 名  : axi_mon_config
 功能描述  : 监控参数配置
 输入参数  : axi_mon_config_t * mon_config
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void axi_mon_config(axi_mon_config_t * mon_config)
{
    axi_config_enum_uint32 mode;
    axi_mon_id_config_t * id_cfg;
    u32 reg_value;
    u32 i;

    mode = mon_config->mode;

    /* 构建监控系统配置 */
    /* 清除bypass */
    AXI_REG_SETBITS(mode, AXI_MON_CNT_CTRL, 5, 16, 0x0);
    /* 时间窗配置 */
    if(AXI_WIN_ENABLE == mon_config->win_en)
    {
        AXI_REG_SETBITS(mode, AXI_MON_CNT_CTRL, 1, 2, 0x1);
    }
    else
    {
        AXI_REG_SETBITS(mode, AXI_MON_CNT_CTRL, 1, 2, 0x0);
    }

    /* 构建监控ID配置 */
    for(i=0; i<AXI_MAX_ID; i++)
    {
        id_cfg = &mon_config->id_config[i];
        if(!id_cfg->id_en)
        {
            /* 保存ID使能标识 */
            g_ul_id_en[i] = 0;
        }
        else
        {
            /* 配置监控端口 */
            reg_value = id_cfg->port & 0x7;
            AXI_REG_SETBITS(mode, AXI_MON_PORT_SEL, i*3, 3, reg_value);

            /* 配置监控操作类型 */
            memcpy(&reg_value, &id_cfg->opt_type, sizeof(u32));
            AXI_REG_WRITE(mode, AXI_MON_CNT_TYPE(i), reg_value);

            /* 配置ID寄存器 */     
            reg_value = ((id_cfg->mask & 0x7FFF) << 15) | (id_cfg->id_value & 0x7FFF);
            /* master id监控使能 */
            if(id_cfg->id_mon_en == 1)
            {
                reg_value |= ((u32)1 << 31);
            }
            else
            {
                reg_value &= ~((u32)1 << 31);
            }          
            AXI_REG_WRITE(mode, AXI_MON_CNT_ID(i), reg_value);

            /* 配置地址监控 */
            if(id_cfg->addr_en)
            {
                /* 地址监控使能 */
                AXI_REG_SETBITS(mode, AXI_MON_CNT_ID(i), 30, 1, 1);
                /* 监控起始地址 */
                AXI_REG_WRITE(mode, AXI_MON_ID_ADDR_DES(i), id_cfg->addr_start);
                /* 监控结束地址 */
                AXI_REG_WRITE(mode, AXI_MON_ID_ADDR_DES_M(i), id_cfg->addr_end);
            }

            /* 保存ID使能标识 */
            g_ul_id_en[i] = 1;
        }
    }
}

/*****************************************************************************
 函 数 名  : axi_capt_set_cnf
 功能描述  : 构建采集配置命令回复，将当前配置的寄存器状态封装上报
 输入参数  : axi_config_enum_uint32 mode
 输出参数  : u32 * out_len 返回数据结构长度
 返 回 值  : AXI_DATA_CONFIG_CNF_STRU *
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
AXI_DATA_CONFIG_CNF_STRU * axi_capt_set_cnf(axi_config_enum_uint32 mode, u32 * out_len)
{
    AXI_DATA_CONFIG_CNF_STRU * config_cnf;
    u32 i;
    u32 offset;
    u32 len;

    /* 回复的寄存器数目以AXI_CAPT_CNF_REG_CNT定义，必须要实际上报个数相匹配 */
    len = sizeof(AXI_DATA_CONFIG_CNF_STRU) + sizeof(AXI_REG_CONFIG_STRU) * AXI_CAPT_CNF_REG_CNT;
    config_cnf = (AXI_DATA_CONFIG_CNF_STRU *)cacheDmaMalloc(len);
    if(NULL == config_cnf)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: malloc fail\n", __FUNCTION__);
        return NULL;
    }

    config_cnf->ulRegCnt = AXI_CAPT_CNF_REG_CNT;

    /* 上报寄存器空间不连续，分段填写 */
    for(i=0; i<10; i++)
    {
        offset = AXI_CAPT_ID_EN + i * 0x4;
        config_cnf->regConfig[i].ulRegOffset = offset;
        AXI_REG_READ(mode, offset, &config_cnf->regConfig[i].ulRegValue);
    }

    config_cnf->regConfig[10].ulRegOffset = AXI_CAPT_TRANS_WAIT_TIME;
    AXI_REG_READ(mode, AXI_CAPT_TRANS_WAIT_TIME, &config_cnf->regConfig[10].ulRegValue);

    for(i=11; i<AXI_CAPT_CNF_REG_CNT-1; i++)
    {
        offset = AXI_CAPT_TRANS_DATA_LEVEL + (i - 11) * 0x4;
        config_cnf->regConfig[i].ulRegOffset = offset;
        AXI_REG_READ(mode, offset, &config_cnf->regConfig[i].ulRegValue);
    }

    config_cnf->regConfig[AXI_CAPT_CNF_REG_CNT-1].ulRegOffset = AXI_CAPT_HEAD_FLAG;
    AXI_REG_READ(mode, AXI_CAPT_HEAD_FLAG, &config_cnf->regConfig[AXI_CAPT_CNF_REG_CNT-1].ulRegValue);

    *out_len = len;

    return  config_cnf;
}

/*****************************************************************************
 函 数 名  : axi_mon_set_cnf
 功能描述  : 构建监控配置命令回复，将当前配置的寄存器状态封装上报
 输入参数  : axi_config_enum_uint32 mode
 输出参数  : u32 * out_len 返回数据结构长度
 返 回 值  : AXI_MON_CONFIG_CNF_STRU *
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
AXI_MON_CONFIG_CNF_STRU * axi_mon_set_cnf(axi_config_enum_uint32 mode, u32 * out_len)
{
    AXI_MON_CONFIG_CNF_STRU * config_cnf;
    u32 i;
    u32 len;

    /* 回复的寄存器数目以AXI_CAPT_CNF_REG_CNT定义，必须要实际上报个数相匹配 */
    len = sizeof(AXI_MON_CONFIG_CNF_STRU) + sizeof(AXI_REG_CONFIG_STRU) * AXI_MON_CNF_REG_CNT;
    config_cnf = (AXI_MON_CONFIG_CNF_STRU *)cacheDmaMalloc(len);
    if(NULL == config_cnf)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: malloc fail\n", __FUNCTION__);
        return NULL;
    }

    config_cnf->ulRegCnt = AXI_MON_CNF_REG_CNT;

    /* 上报寄存器空间不连续，分段填写 */
    /* CNT_CTRL寄存器 */
    config_cnf->regConfig[0].ulRegOffset = AXI_MON_CNT_CTRL;
    AXI_REG_READ(mode, AXI_MON_CNT_CTRL, &config_cnf->regConfig[0].ulRegValue);

    /* 时间窗counter */
    config_cnf->regConfig[1].ulRegOffset = AXI_MON_WIN_COUNTER;
    AXI_REG_READ(mode, AXI_MON_WIN_COUNTER, &config_cnf->regConfig[1].ulRegValue);

    /* 监控端口配置PORT_SEL */
    config_cnf->regConfig[2].ulRegOffset = AXI_MON_PORT_SEL;
    AXI_REG_READ(mode, AXI_MON_PORT_SEL, &config_cnf->regConfig[2].ulRegValue);

    /* 监控操作类型, 8个ID */
    for(i=0; i<AXI_MAX_ID; i++)
    {
        config_cnf->regConfig[i+3].ulRegOffset = AXI_MON_CNT_TYPE(i);
        AXI_REG_READ(mode, AXI_MON_CNT_TYPE(i), &config_cnf->regConfig[i+3].ulRegValue);
    }

    /* ID配置、地址配置 */
    for(i=0; i<AXI_MAX_ID; i++)
    {
        /* ID配置 */
        config_cnf->regConfig[i*3 + 11].ulRegOffset = AXI_MON_CNT_ID(i);
        AXI_REG_READ(mode, AXI_MON_CNT_ID(i), &config_cnf->regConfig[i*3 + 11].ulRegValue);
        /* 起始地址 */
        config_cnf->regConfig[i*3 + 12].ulRegOffset = AXI_MON_ID_ADDR_DES(i);
        AXI_REG_READ(mode, AXI_MON_ID_ADDR_DES(i), &config_cnf->regConfig[i*3 + 12].ulRegValue);
        /* 结束地址 */
        config_cnf->regConfig[i*3 + 13].ulRegOffset = AXI_MON_ID_ADDR_DES_M(i);
        AXI_REG_READ(mode, AXI_MON_ID_ADDR_DES_M(i), &config_cnf->regConfig[i*3 + 13].ulRegValue);
    }

    *out_len = len;

    return  config_cnf;
}

/*****************************************************************************
 函 数 名  : axi_mon_get_reg_value
 功能描述  : 获取监控统计寄存器结果
 输入参数  : axi_config_enum_uint32 mode
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void axi_mon_get_reg_value(axi_config_enum_uint32 mode, AXI_MON_TERMINATE_CNF_STRU * ter_cnf)
{
    AXI_MON_ID_STATISTIC_DATA_STRU * id_stat;
    u32 i;
    
    AXI_REG_READ(mode, AXI_MON_IDLE_COUNTER, &ter_cnf->ulIdelCnt);

    for(i=0; i<AXI_MAX_PORT; i++)
    {
        AXI_REG_READ(mode, AXI_MON_RD_WAIT_CYCLE_PORTS(i), &ter_cnf->ulRdWaitCycle[i]);
        AXI_REG_READ(mode, AXI_MON_WR_WAIT_CYCLE_PORTS(i), &ter_cnf->ulWrWaitCycle[i]);
    }

    for(i=0; i<AXI_MAX_ID; i++)
    {
        id_stat = &(ter_cnf->idStat[i]);

        id_stat->ulIdEn = g_ul_id_en[i];
        AXI_REG_READ(mode, AXI_MON_INCR1_ID(i), &id_stat->ulIncr1Cnt);
        AXI_REG_READ(mode, AXI_MON_INCR2_ID(i), &id_stat->ulIncr2Cnt);
        AXI_REG_READ(mode, AXI_MON_INCR4_ID(i), &id_stat->ulIncr4Cnt);
        AXI_REG_READ(mode, AXI_MON_INCR8_ID(i), &id_stat->ulIncr8Cnt);
        AXI_REG_READ(mode, AXI_MON_INCR16_ID(i), &id_stat->ulIncr16Cnt);
        AXI_REG_READ(mode, AXI_MON_WRAP_ID(i), &id_stat->ulWrapCnt);
        AXI_REG_READ(mode, AXI_MON_BURST_ID(i), &id_stat->ulBurstSendCnt);
        AXI_REG_READ(mode, AXI_MON_FINISH_ID(i), &id_stat->ulBurstFinCnt);
        AXI_REG_READ(mode, AXI_MON_RD_WAIT_ID_LOW(i), &id_stat->ulRdDelayLow);
        AXI_REG_READ(mode, AXI_MON_RD_WAIT_ID_HIGH(i), &id_stat->ulRdDelayHigh);
        AXI_REG_READ(mode, AXI_MON_WR_WAIT_ID_LOW(i), &id_stat->ulWrDelayLow);
        AXI_REG_READ(mode, AXI_MON_WR_WAIT_ID_HIGH(i), &id_stat->ulWrDelayHigh);
        AXI_REG_READ(mode, AXI_MON_WR_WAIT_RESP_ID_LOW(i), &id_stat->ulWrRespDelayLow);
        AXI_REG_READ(mode, AXI_MON_WR_WAIT_RESP_ID_HIGH(i), &id_stat->ulWrRespDelayHigh);
        AXI_REG_READ(mode, AXI_MON_RD_MAX_WAIT_ID(i), &id_stat->ulRdMaxWait);
        AXI_REG_READ(mode, AXI_MON_WR_MAX_WAIT_ID(i), &id_stat->ulWrMaxWait);
        AXI_REG_READ(mode, AXI_MON_WR_MAX_WAIT_RESP_ID(i), &id_stat->ulWrRespMaxWait);
        AXI_REG_READ(mode, AXI_MON_RD_BYTES_ID_LOW(i), &id_stat->ulRdTotalCntLow);
        AXI_REG_READ(mode, AXI_MON_RD_BYTES_ID_HIGH(i), &id_stat->ulRdTotalCntHigh);
        AXI_REG_READ(mode, AXI_MON_WR_BYTES_ID_LOW(i), &id_stat->ulWrTotalCntLow);
        AXI_REG_READ(mode, AXI_MON_WR_BYTES_ID_HIGH(i), &id_stat->ulWrTotalCntHigh);
    }
}

/*****************************************************************************
 函 数 名  : axi_mon_set_statistic_data
 功能描述  : 构建监控终止命令回复，将当前监控统计值组包发送给上层
 输入参数  : axi_config_enum_uint32 mode
 输出参数  : u32 * out_len 返回数据结构长度
 返 回 值  : AXI_MON_TERMINATE_CNF_STRU *
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
AXI_MON_TERMINATE_CNF_STRU * axi_mon_set_statistic_data(axi_config_enum_uint32 mode, u32 * out_len)
{
    AXI_MON_TERMINATE_CNF_STRU * ter_cnf;

    ter_cnf = (AXI_MON_TERMINATE_CNF_STRU *)cacheDmaMalloc(sizeof(AXI_MON_TERMINATE_CNF_STRU));
    if(NULL == ter_cnf)
    {
       bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: alloc fail\n", __FUNCTION__);
       return NULL;
    }

    axi_mon_get_reg_value(mode, ter_cnf);
    *out_len = sizeof(AXI_MON_TERMINATE_CNF_STRU);
    
    return ter_cnf;
}

/*****************************************************************************
 函 数 名  : axi_mon_info_gen_packet
 功能描述  : 时间窗监控结束，组包上报
 输入参数  : axi_config_enum_uint32 mode
 输出参数  : 无
 返 回 值  : s32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
s32 axi_mon_info_gen_packet(axi_config_enum_uint32 mode)
{
    bsp_socp_head_s *   socp_header      = NULL;
    bsp_om_head_s   *   bsp_om_header    = NULL;
    u8 *    send_buf = NULL;
    u8 *    data;
    u32     data_len;

    /* buffer申请 */
    data_len = sizeof(AXI_MON_TERMINATE_CNF_STRU);
    send_buf = (u8 *)bsp_om_get_buf(BSP_OM_SOCP_BUF_TYPE, sizeof(bsp_socp_head_s) + sizeof(bsp_om_head_s) + data_len);
    if(NULL == send_buf)
    {
        g_st_stat_info.mon_ind_fail_cnt++;
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: get buf fail\n", __FUNCTION__);

        return BSP_ERROR;
    }

    /* 构建SOCP头 */
    socp_header = (bsp_socp_head_s *)send_buf;
    socp_header->service_id         = BSP_SOCP_SID_DIAG_SERVER;
    socp_header->s_service_id       = BSP_SOCP_HIGH4BIT_SSID_ACPU << 4;
    socp_header->service_session_id = BSP_SOCP_SERVER_SESSION_ID;
    socp_header->msg_type           = BSP_SOCP_MSG_TYPE_IND;
    socp_header->msg_trans_id       = g_st_stat_info.mon_ind_cnt++;

    /* 构建OM头 */
    bsp_om_header = (bsp_om_head_s *)(send_buf + sizeof(bsp_socp_head_s));
    bsp_om_header->om_id            = ((u32)BSP_STRU_ID_28_31_GROUP_BSP << 28) | ((u32)BSP_STRU_ID_16_23_BSP_AMON << 16);
    bsp_om_header->data_size        = data_len;

    /* 构建数据 */
    data = send_buf + sizeof(bsp_socp_head_s) + sizeof(bsp_om_head_s);
    axi_mon_get_reg_value(mode, (AXI_MON_TERMINATE_CNF_STRU *)data);

    /* 发送数据 */
    if(BSP_OK != bsp_om_send_coder_src(send_buf, sizeof(bsp_socp_head_s) + sizeof(bsp_om_head_s) + data_len))
    {
        g_st_stat_info.mon_ind_fail_cnt++;
        bsp_om_free_buf((u32)send_buf, sizeof(bsp_socp_head_s) + sizeof(bsp_om_head_s) + data_len);
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: send data fail\n", __FUNCTION__);

        return BSP_ERROR;
    }

    return BSP_OK;
}

/*****************************************************************************
 函 数 名  : axi_capt_ctrl_start
 功能描述  : 采集控制，启动命令处理
 输入参数  : axi_config_enum_uint32 mode
 输出参数  : 无
 返 回 值  : 成功: 0
             失败: 非0
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
s32 axi_capt_ctrl_start(axi_config_enum_uint32 mode)
{
    u32 edma_bindex;
    u32 edma_req_leve;
    u32 i;
    
    AXI_CHECK_INIT();

    /* 启动采集，打开时钟，采集结束时关闭 */
    for(i=0; i<AXI_CONFIG_BUTT; i++)
    {
        if(g_st_amon_clk.is_clk_enable[i] == false)
        {
            axi_sc_clk_open(i);
        }
    }

    
    /* 如果已下发启动命令，启动不可用 */
    AXI_CHECK_STATE(mode);

    /* EDMA req level取值0,1,2,3: 表示每8, 16, 24, 32个数据发一次请求，每个数据单位为8bytes */
    AXI_REG_GETBITS(mode, AXI_CAPT_TRAN_CONFIG, 5, 2, &edma_req_leve);
    edma_bindex = (edma_req_leve + 1) * 8 * 8;

    /* EDMA通道初始化 */
    if(BSP_OK != axi_edma_config_init(mode, edma_bindex))
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: config edma fail\n", __FUNCTION__);
        return BSP_ERROR;
    }

    g_st_stat_info.edma_bindex = edma_bindex;

    /* 创建周期采集timer */
    if(BSP_OK != axi_start_timer(g_st_stat_info.capt_cycle))
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: start capt timer fail\n", __FUNCTION__);
        return BSP_ERROR;
    }

    /* 创建SOCP timer，防止SOCP发送失败 */
    if(BSP_OK != axi_socp_start_timer(100))
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: start socp timer fail\n", __FUNCTION__);
        return BSP_ERROR;        
    }
    
    g_st_stat_info.capt_mode  = mode;

    return axi_capt_start(mode);
}

/*****************************************************************************
 函 数 名  : axi_capt_ctrl_start
 功能描述  : 采集控制，停止命令处理
 输入参数  : axi_config_enum_uint32 mode
 输出参数  : 无
 返 回 值  : 成功: 0
             失败: 非0
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
s32 axi_capt_ctrl_stop(axi_config_enum_uint32 mode)
{
    s32 ret;
    u32 i;
    axi_state_enum_uint32 axi_state;
    
    AXI_CHECK_INIT();
    
    axi_state = axi_get_state(mode, AXI_GET_RUN_STATE_REQ);
    /* AXI monitor未启动 */
    if(axi_state == AXI_IDLE || axi_state == AXI_STOP)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: monitor has been stopped\n", __FUNCTION__);
        return BSP_ERROR;
    }

     /* 停止timer */
    axi_stop_timer();

    if(BSP_OK != axi_capt_stop(mode))
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: stop capture fail\n", __FUNCTION__);
        return BSP_ERROR;
    }
    
    /* 释放EDMA通道 */
    if(BSP_OK != axi_edma_free(mode))
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: free edma fail\n", __FUNCTION__);
        return BSP_ERROR;
    }

    /* 停止Monitor */
    ret = axi_mon_stop(mode);

    /* 释放SOCP timer */
    if(g_st_socp_timer.init_flags == TIMER_INIT_FLAG)
    {
        (void)bsp_softtimer_free(&g_st_socp_timer);
    }
    
    /* 采集结束，关闭时钟 */
    for(i=0; i<AXI_CONFIG_BUTT; i++)
    {
        if(g_st_amon_clk.is_clk_enable[i] == true)
        {
            axi_sc_clk_close(i);
        } 
    }
    
    return ret;
}

/*****************************************************************************
 函 数 名  : axi_capt_ctrl_export
 功能描述  : 采集控制，导出命令处理
 输入参数  : axi_config_enum_uint32 mode
 输出参数  : 无
 返 回 值  : 成功: 0
             失败: 非0
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
s32 axi_capt_ctrl_export(axi_config_enum_uint32 mode)
{
    /* 如果已进行操作，导出将不可用 */
    if(g_sl_init_flag == true)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: something done before export\n", __FUNCTION__);
        return BSP_ERROR;       
    }

    bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: entered\n", __FUNCTION__);

    /* 复位后导出，需要先初始化 */
    if(BSP_OK != axi_init())
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: init fail\n", __FUNCTION__);
        return BSP_ERROR;
    }

    /* monitor内部软复位 */
    axi_reset(mode);
   
    /* 清空组包buffer */
    axi_reset_pkg_buf(mode);

    /* EDMA通道初始化 */
    if(BSP_OK != axi_edma_config_init(mode, AXI_EDMA_BINDEX))
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: config edma fail\n", __FUNCTION__);
        return BSP_ERROR;
    }

    /* 配置Monitor buffer选择，默认缓存SOC数据 */
    if(AXI_CPUFAST_CONFIG == mode)
    {
        AXI_REG_SETBITS(mode, AXI_CAPT_TRAN_CONFIG, 4, 1, 1);
    }

    /* 去除中断屏蔽，保持满中断屏蔽 */
    AXI_REG_WRITE(mode, AXI_CAPT_INT_MASK, 0xD);

    /* 启动monitor */
    if(BSP_OK != axi_mon_start(mode))
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: start monitor 0x%x fail\n", __FUNCTION__, mode);
        return BSP_ERROR;
    }

    if(BSP_OK != axi_capt_stop(mode))
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: stop capture fail\n", __FUNCTION__);
        return BSP_ERROR;
    }

    /* 停止monitor */
    if(BSP_OK != axi_mon_stop(mode))
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: stop monitor 0x%x fail\n", __FUNCTION__, mode);
        return BSP_ERROR;
    }
    
    /* 释放EDMA通道 */
    if(BSP_OK != axi_edma_free(mode))
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: free edma fail\n", __FUNCTION__);
        return BSP_ERROR;
    }

    /* 导出结束，关闭时钟 */
    if(g_st_amon_clk.is_clk_enable[mode] == true)
    {
        axi_sc_clk_close(mode);
    } 

    amon_log_print("%s: ok\n", __FUNCTION__);

    return BSP_OK;
}

s32 amon_addr_is_enable(void)
{
    if((g_amon_config.en_flag & AMON_SOC_MASK) || (g_amon_config.en_flag  & AMON_CPUFAST_MASK))
    {
        return BSP_OK;
    }

    return BSP_ERROR;
}

/*****************************************************************************
 函 数 名  : bsp_axi_capt_config
 功能描述  : 采集配置接口
 输入参数  : data    : 采集配置数据结构指针
 输出参数  : out_len : 采集寄存器配置状态数据结构长度
 返 回 值  : 配置成功: AXI_DATA_CONFIG_CNF_STRU *
             配置失败: NULL
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
AXI_DATA_CONFIG_CNF_STRU * bsp_axi_capt_config(u8 * data, u32 * out_len)
{
    axi_capt_config_t * capt_config;
    AXI_DATA_CONFIG_CNF_STRU * cfg_cnf;

    if(NULL == data || NULL == out_len)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: invalid para\n", __FUNCTION__);
        return NULL;
    }

    if(BSP_OK == amon_addr_is_enable())
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: addr monitor is enable\n", __FUNCTION__);
        return NULL;
    }
    
    capt_config = (axi_capt_config_t *)data;

    if(capt_config->mode >= AXI_CONFIG_BUTT)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: invalid mode %u\n", __FUNCTION__, capt_config->mode);
        return NULL;
    }

    if(g_ul_k3_flag && (capt_config->mode == AXI_CPUFAST_CONFIG))
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s(%u): cpufast is not supported for k3\n", __FUNCTION__, capt_config->mode);
        return NULL;
    }

    /* 非首次配置，打开时钟 */
    if(g_sl_init_flag == true && g_st_amon_clk.is_clk_enable[capt_config->mode] == false)
    {
        axi_sc_clk_open(capt_config->mode);
    }
    else
    {
        /* 首次配置前进行初始化 */
        if(BSP_OK != axi_init())
        {
            bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: init fail\n", __FUNCTION__);
            return NULL;
        }
    }
    
    if(BSP_OK != axi_state_check(capt_config->mode))
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: AXI %d is running\n", __FUNCTION__, capt_config->mode);
        return NULL;
    }

    /* Monitor内部软复位 */
    axi_reset(capt_config->mode);
    /* 系统控制器复位 */
    axi_sc_reset();

    /* 配置采集参数 */
    axi_capt_config(capt_config);

    /* 保存采集周期 */
    g_st_stat_info.capt_cycle = capt_config->capt_cycle;

    /* 用当前配置构建CNF */
    cfg_cnf = axi_capt_set_cnf(capt_config->mode, out_len);

    /* 关闭时钟 */
    axi_sc_clk_close(capt_config->mode);
    
    return cfg_cnf;
}

/*****************************************************************************
 函 数 名  : bsp_axi_reg_config
 功能描述  : 采集、监控寄存器配置接口
 输入参数  : data    : 采集、监控配置数据结构指针
 输出参数  : 无
 返 回 值  : 配置成功: 0
             配置失败: 非0
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
s32 bsp_axi_reg_config(u8 * data)
{
    axi_reg_config_t * reg_config;
    u32 i;

    if(NULL == data)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: invalid para\n", __FUNCTION__);
        return BSP_ERROR;
    }

    if(BSP_OK == amon_addr_is_enable())
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: addr monitor is enable\n", __FUNCTION__);
        return BSP_ERROR;
    }

    reg_config = (axi_reg_config_t *)data;

    AXI_CHECK_MODE(reg_config->mode);

    if(g_ul_k3_flag && (reg_config->mode == AXI_CPUFAST_CONFIG))
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s(%u): cpufast is not supported for k3\n", __FUNCTION__, reg_config->mode);
        return BSP_ERROR;
    }

    /* 非首次配置，打开时钟 */
    if(g_sl_init_flag == true && g_st_amon_clk.is_clk_enable[reg_config->mode] == false)
    {
        axi_sc_clk_open(reg_config->mode);
    }
    else
    {
        /* 首次配置前进行初始化 */
        if(BSP_OK != axi_init())
        {
            bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: init fail\n", __FUNCTION__);
            return BSP_ERROR;
        }
    }

    AXI_CHECK_STATE(reg_config->mode);

    /* Monitor内部软复位 */
    axi_reset(reg_config->mode);
    /* 系统控制器复位 */
    axi_sc_reset();

    /* 配置采集寄存器 */
    for(i=0; i<reg_config->reg_cnt; i++)
    {
        AXI_REG_WRITE(reg_config->mode, reg_config->reg_cfg[i].reg_offset, reg_config->reg_cfg[i].reg_value);
    }

    /* 保存采集周期 */
    g_st_stat_info.capt_cycle = reg_config->capt_cycle;

    /* 关闭时钟 */
    axi_sc_clk_close(reg_config->mode);

    return BSP_OK;
}

/*****************************************************************************
 函 数 名  : bsp_axi_capt_ctrl
 功能描述  : 采集控制命令处理
 输入参数  : data    : 采集控制命令数据结构指针
 输出参数  : 无
 返 回 值  : 成功: 0
             失败: 非0
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
s32 bsp_axi_capt_ctrl(u8 * data)
{
    axi_ctrl_t * ctrl_stu;
    axi_config_enum_uint32 mode;

    if(NULL == data)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: invalid para\n", __FUNCTION__);
        return BSP_ERROR;
    }

    if(BSP_OK == amon_addr_is_enable())
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: addr monitor is enable\n", __FUNCTION__);
        return BSP_ERROR;
    }

    ctrl_stu = (axi_ctrl_t *)data;
    mode     = (axi_config_enum_uint32)ctrl_stu->mode;

    AXI_CHECK_MODE(mode);

    if(ctrl_stu->ctrl_type >= AXI_CTRL_BUTT)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: invalid ctrl type %u\n", __FUNCTION__, ctrl_stu->ctrl_type);
        return BSP_ERROR;
    }

    if(g_ul_k3_flag && (mode == AXI_CPUFAST_CONFIG))
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s(%u): cpufast is not supported for k3\n", __FUNCTION__, mode);
        return BSP_ERROR;
    }

    /* 启动 */
    if(AXI_CTRL_START == ctrl_stu->ctrl_type)
    {
        return axi_capt_ctrl_start(mode);
    }
    /* 停止 */
    else if(AXI_CTRL_STOP == ctrl_stu->ctrl_type)
    {
        return axi_capt_ctrl_stop(mode);
    }
    /* 导出 */
    else
    {
        return axi_capt_ctrl_export(mode);
    }
}

/*****************************************************************************
 函 数 名  : bsp_axi_mon_config
 功能描述  : 监控配置接口
 输入参数  : data    : 监控配置数据结构指针
 输出参数  : out_len : 监控寄存器配置状态数据结构长度
 返 回 值  : 配置成功: AXI_MON_CONFIG_CNF_STRU *
             配置失败: NULL
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
AXI_MON_CONFIG_CNF_STRU * bsp_axi_mon_config(u8 * data, u32 * out_len)
{
    axi_mon_config_t * mon_config;
    AXI_MON_CONFIG_CNF_STRU * cfg_cnf;

    if(NULL == data || NULL == out_len)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: invalid para\n", __FUNCTION__);
        return NULL;
    }
    
    if(BSP_OK == amon_addr_is_enable())
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: addr monitor is enable\n", __FUNCTION__);
        return NULL;
    }

    mon_config = (axi_mon_config_t *)data;

    if(mon_config->mode >= AXI_CONFIG_BUTT)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: invalid mode %u\n", __FUNCTION__, mon_config->mode);
        return NULL;
    }

    if(g_ul_k3_flag && (mon_config->mode == AXI_CPUFAST_CONFIG))
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s(%u): cpufast is not supported for k3\n", __FUNCTION__, mon_config->mode);
        return NULL;
    }

    /* 非首次配置，打开时钟 */
    if(g_sl_init_flag == true && g_st_amon_clk.is_clk_enable[mon_config->mode] == false)
    {
        axi_sc_clk_open(mon_config->mode);
    }
    else
    {
        /* 首次配置前进行初始化 */
        if(BSP_OK != axi_init())
        {
            bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: init fail\n", __FUNCTION__);
            return NULL;
        }
    }

    if(BSP_OK != axi_state_check(mon_config->mode))
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: AXI %d is running\n", __FUNCTION__, mon_config->mode);
        return NULL;
    }

    /* Monitor内部软复位 */
    axi_reset(mon_config->mode);
    /* 系统控制器复位 */
    axi_sc_reset();

    /* 配置采集参数 */
    axi_mon_config(mon_config);

    /* 用当前配置构建CNF */
    cfg_cnf = axi_mon_set_cnf(mon_config->mode, out_len);

    /* 关闭时钟 */
    axi_sc_clk_close(mon_config->mode);
        
    return cfg_cnf;
}

/*****************************************************************************
 函 数 名  : bsp_axi_mon_start
 功能描述  : 监控启动命令处理
 输入参数  : data: 监控启动命令数据结构指针
 输出参数  : 无
 返 回 值  : 成功: 0
             失败: 非0
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
s32 bsp_axi_mon_start(u8 * data)
{
    axi_ctrl_t * ctrl_stu;

    if(NULL == data)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: null pointer\n", __FUNCTION__);
        return BSP_ERROR;
    }

    if(BSP_OK == amon_addr_is_enable())
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: addr monitor is enable\n", __FUNCTION__);
        return BSP_ERROR;
    }

    ctrl_stu = (axi_ctrl_t *)data;

    AXI_CHECK_MODE(ctrl_stu->mode);

    if(g_ul_k3_flag && (ctrl_stu->mode == AXI_CPUFAST_CONFIG))
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s(%u): cpufast is not supported for k3\n", __FUNCTION__, ctrl_stu->mode);
        return BSP_ERROR;
    }

    /* 命令类型检查 */
    if(AXI_CTRL_START != ctrl_stu->ctrl_type)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: invalid ctrl type %u\n", __FUNCTION__, ctrl_stu->ctrl_type);
        return BSP_ERROR;
    }

    /* 是否初始化，避免未经配置直接启动 */
    AXI_CHECK_INIT();

    /* 启动监控，打开时钟，监控结束时关闭 */
    if(g_st_amon_clk.is_clk_enable[ctrl_stu->mode] == false)
    {
        axi_sc_clk_open(ctrl_stu->mode);
    }

    return axi_mon_start(ctrl_stu->mode);
}

/*****************************************************************************
 函 数 名  : bsp_axi_mon_terminate
 功能描述  : 监控停止、导出命令处理
 输入参数  : data   : 监控终止命令数据结构指针
 输出参数  : out_len: 返回数据结构长度
 返 回 值  : 成功: AXI_MON_TERMINATE_CNF_STRU *
             失败: NULL
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
AXI_MON_TERMINATE_CNF_STRU * bsp_axi_mon_terminate(u8 * data, u32 * out_len)
{
    axi_ctrl_t * ctrl_stu;
    axi_config_enum_uint32 mode;
    AXI_MON_TERMINATE_CNF_STRU * ter_cnf;

    if(NULL == data || NULL == out_len)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: null pointer\n", __FUNCTION__);
        return NULL;
    }

    if(BSP_OK == amon_addr_is_enable())
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: addr monitor is enable\n", __FUNCTION__);
        return NULL;
    }

    ctrl_stu = (axi_ctrl_t *)data;
    mode     = (axi_config_enum_uint32)ctrl_stu->mode;

    if(mode >= AXI_CONFIG_BUTT)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: invalid mode %u\n", __FUNCTION__, mode);
        return NULL;
    }

    if(g_ul_k3_flag && (mode == AXI_CPUFAST_CONFIG))
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s(%u): cpufast is not supported for k3\n", __FUNCTION__, mode);
        return NULL;
    }

    if(ctrl_stu->ctrl_type != AXI_CTRL_STOP && ctrl_stu->ctrl_type != AXI_CTRL_EXPORT)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: invalid ctrl type %u\n", __FUNCTION__, ctrl_stu->ctrl_type);
        return NULL;
    }

    if(BSP_OK != axi_mon_stop(mode))
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s: stop monitor %u fail\n", __FUNCTION__, mode);
        return NULL;
    }

    ter_cnf = axi_mon_set_statistic_data(mode, out_len);

    /* 监控结束，关闭时钟 */
    if(g_st_amon_clk.is_clk_enable[mode] == true)
    {
        axi_sc_clk_close(mode);
    }   
    
    return ter_cnf;
}

/*****************************************************************************
 函 数 名  : axi_print_debug_info
 功能描述  : 打印信息
 输出参数  : 无
 返 回 值  : u32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void axi_print_debug_info(axi_config_enum_uint32 config)
{
    axi_state_enum_uint32 axi_state;
    u32 reg_value = 0;

    if(g_ul_k3_flag && (config == AXI_CPUFAST_CONFIG))
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_AMON, "%s(%u): cpufast is not supported for k3\n", __FUNCTION__, config);
        return;
    }

    axi_state = axi_get_state(config, AXI_GET_RUN_STATE_REQ);
    amon_log_print("AXI running state       : 0x%x\n", axi_state);

    AXI_REG_READ(config, AXI_MON_WIN_COUNTER, &reg_value);
    amon_log_print("AXI window counter      : 0x%x\n", reg_value);

    AXI_REG_READ(config, AXI_CAPT_ERR_STATE, &reg_value);
    amon_log_print("AXI capture state       : 0x%x\n", reg_value);

    AXI_REG_READ(config, AXI_CAPT_TRANS_DATA_CNT, &reg_value);
    amon_log_print("AXI trans data cnt      ; 0x%x\n", reg_value);

    AXI_REG_READ(config, AXI_CAPT_TRANS_DATA_CNT_HIGH, &reg_value);
    amon_log_print("AXI trans data cnt high ; 0x%x\n", reg_value);
    amon_log_print("AXI CPUFAST EDMA channel: %d\n", g_sl_edma_channel[AXI_CPUFAST_CONFIG]);
    amon_log_print("AXI SOC EDMA channel    : %d\n", g_sl_edma_channel[AXI_SOC_CONFIG]);
}

/*****************************************************************************
 函 数 名  : axi_print_int_cnt
 功能描述  : 打印中断统计信息
 输出参数  : 无
 返 回 值  : u32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void axi_print_int_cnt(void)
{
    amon_log_print("AXI win over cnt        : %d\n", g_st_stat_info.int_stat.win_over);
    amon_log_print("AXI buff over cnt       : %d\n", g_st_stat_info.int_stat.buff_over);
    amon_log_print("AXI tran threshold cnt  : %d\n", g_st_stat_info.int_stat.tran_thresh);
    amon_log_print("AXI tran req cnt        : %d\n", g_st_stat_info.int_stat.tran_req);
    amon_log_print("AXI tran over cnt       : %d\n", g_st_stat_info.int_stat.tran_over);
    amon_log_print("AXI edma tran over cnt  : %d\n", g_st_stat_info.int_stat.edma_tran_over);
    amon_log_print("AXI capt buff full cnt  : %d\n", g_st_stat_info.axi_buf_full);
    amon_log_print("AXI edma tran fail cnt  : %d\n", g_st_stat_info.edma_tran_fail);
    amon_log_print("AXI socp buff full cnt  : %d\n", g_st_stat_info.socp_buf_full);
    amon_log_print("AXI socp tran fail cnt  : %d\n", g_st_stat_info.socp_tran_fail);
    amon_log_print("AXI mon ind cnt         : %d\n", g_st_stat_info.mon_ind_cnt);
    amon_log_print("AXI mon ind fail cnt    : %d\n", g_st_stat_info.mon_ind_fail_cnt);
}

/*****************************************************************************
 函 数 名  : axi_print_ring_buf_info
 功能描述  : 打印循环buffer指针状态
 输出参数  : 无
 返 回 值  : u32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void axi_print_ring_buf_info(void)
{
    if(!g_ul_k3_flag)
    {
        amon_log_print("********* CPUFAST RING BUFFER *********\n");
        amon_log_print("CPUFAST START: 0x%x\n", (u32)g_st_ring_buf_info[0].start);
        amon_log_print("CPUFAST END  : 0x%x\n", (u32)g_st_ring_buf_info[0].end);
        amon_log_print("CPUFAST READ : 0x%x\n", (u32)g_st_ring_buf_info[0].read);
        amon_log_print("CPUFAST WRITE: 0x%x\n", (u32)g_st_ring_buf_info[0].write);
    }

    amon_log_print("*********   SOC RING BUFFER   *********\n");
    amon_log_print("SOC START    : 0x%x\n", (u32)g_st_ring_buf_info[1].start);
    amon_log_print("SOC END      : 0x%x\n", (u32)g_st_ring_buf_info[1].end);
    amon_log_print("SOC READ     : 0x%x\n", (u32)g_st_ring_buf_info[1].read);
    amon_log_print("SOC WRITE    : 0x%x\n", (u32)g_st_ring_buf_info[1].write);
}

/*lint -restore +e40*/
#ifdef __cplusplus
}
#endif

