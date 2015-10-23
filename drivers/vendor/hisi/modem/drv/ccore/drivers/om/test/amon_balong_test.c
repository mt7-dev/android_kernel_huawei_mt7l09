

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
#include <string.h>
#include <taskLib.h>
#include <intLib.h>
#include "amon_balong.h"
#include "bsp_memmap.h"

#define AXI_CHECK_RET(a, b)\
do{\
    u32 x;\
    x = (u32)a;\
    if(x != (u32)b)\
    {\
        printf("%s(%d): ret(0x%x) error...\n", __FUNCTION__, __LINE__, x);\
        return x;\
    }\
}while(0)

extern s32 axi_init();
extern s32 axi_capt_start(axi_config_enum_uint32 axi_mode);
extern s32 axi_capt_stop(axi_config_enum_uint32 config);

void axi_dump(u32 config, u32 off, u32 len);
void axi_soc_config(u32 edma_bindex);
void axi_cpufast_config(u32 edma_bindex);
void axi_set_data(u32 config, u32 len, u32 value);
s32 axi_mon_test_001(void);
s32 axi_mon_test_002(void);
s32 axi_mon_test_003(void);
s32 axi_mon_test_004(void);
s32 axi_mon_test_005(void);
s32 axi_mon_test_006(void);

/*****************************************************************************
 函 数 名  : axi_dump
 功能描述  : 打印内存信息
 输出参数  : 无
 返 回 值  : u32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年1月29日
    作    者   : f
    修改内容   : 新生成函数

*****************************************************************************/
void axi_dump(u32 config, u32 off, u32 len)
{
    u32 base_addr;
    u32 addr_offset;
    u32 offset;
    int i,j;
    u32 reg_value;
    u32 addr;

    if(AXI_CPUFAST_CONFIG == config)
    {
        base_addr = AXI_MON_CPUFAST_BASE_ADDR;
        addr = AXI_MON_CPUFAST_BASE_ADDR + off;
        addr_offset = off;
    }
    else
    {
        base_addr = AXI_MON_SOC_BASE_ADDR;
        addr = AXI_MON_SOC_BASE_ADDR + off;
        addr_offset = off;
    }

    printk("base_addr: %x, offset: %x\n", addr, addr_offset);
    offset = 0;
    for(i=0; i<(int)len; i+=16)
    {
        /* 打印第一行 */
        if(0 == i)
        {
            printk("0x%x: ", addr);
            /* 偏移补齐 */
            for(j=0; j<(int)offset/4; j++)
            {
                printk("         ");
            }
            for(j=0; j<(int)(16-offset)/4; j++)
            {
                reg_value = readl(base_addr + addr_offset);
                addr_offset += 4;
                printk("%08x ", reg_value);
            }
            printk("\n");
            continue;
        }
        /* 打印剩余行 */
        printk("0x%x: ", addr - offset + i);
        for(j=0; j<4; j++)
        {
            reg_value = readl(base_addr + addr_offset);
            addr_offset += 4;
            printk("%08x ", reg_value);
        }
        printk("\n");
    }
}

extern axi_global_stat_t g_st_stat_info;
extern ring_buf_info_t   g_st_ring_buf_info[2];
extern s32 axi_data_buf_update(ring_buf_info_t * ring_buf);

void axi_soc_config(u32 edma_bindex)
{
    g_st_stat_info.capt_mode = AXI_SOC_CONFIG;
    g_st_stat_info.edma_bindex = edma_bindex;
}

void axi_cpufast_config(u32 edma_bindex)
{
    g_st_stat_info.capt_mode = AXI_CPUFAST_CONFIG;
    g_st_stat_info.edma_bindex = edma_bindex;
}

/* 组包上报HSO */
void axi_set_data(u32 config, u32 len, u32 value)
{
    u32 end_len;
    u32 base;

    base = (u32)DDR_SOCP_ADDR;

    end_len = g_st_ring_buf_info[config].end - g_st_ring_buf_info[config].write + 1;
    if(end_len < len)
    {
        memset((void *)(base + g_st_ring_buf_info[config].write - g_st_ring_buf_info[config].start), value, end_len);
        memset((void *)base, value, len - end_len);
        g_st_ring_buf_info[config].write = g_st_ring_buf_info[config].start + len - end_len;
    }
    else
    {
        memset((void *)(base + g_st_ring_buf_info[config].write - g_st_ring_buf_info[config].start), value, len);
        g_st_ring_buf_info[config].write += len;
    }
    /* 组包，通过SOCP发送 */
    if(BSP_OK != axi_data_buf_update(&g_st_ring_buf_info[config]))
    {
        g_st_stat_info.socp_tran_fail++;
        printf("socp transfer fail\n");
        return ;
    }
}

/************************* 监控功能测试 *************************/
/* 空指针测试 */
s32 axi_mon_test_001(void)
{
    AXI_CHECK_RET(bsp_axi_mon_config(NULL, NULL), NULL);
    printf("axi_mon_test_001 passed\n");

    return BSP_OK;
}

/* 非SOC/CPUFAST模式测试 */
s32 axi_mon_test_002(void)
{
    u32 len;
    axi_mon_config_t cfg;

    cfg.mode = AXI_CONFIG_BUTT;

    AXI_CHECK_RET(bsp_axi_mon_config((u8 *)&cfg, &len), NULL);

    printf("axi_mon_test_002 passed\n");

    return BSP_OK;
}

/* 配置SOC测试 */
s32 axi_mon_test_003(void)
{
    u32 len;
    axi_mon_config_t cfg;
    AXI_MON_CONFIG_CNF_STRU * mon_cnf;

    cfg.mode   = AXI_SOC_CONFIG;
    cfg.win_en = AXI_WIN_DISABLE;

    memset(cfg.id_config, 0, sizeof(axi_mon_id_config_t)*AXI_MAX_ID);

    cfg.id_config[0].id_en  = 1;
    cfg.id_config[0].port   = 1;
    cfg.id_config[0].opt_type.incr_2  = 1;
    cfg.id_config[0].opt_type.but_fin = 1;
    cfg.id_config[0].id_value = 0x60;
    cfg.id_config[0].mask     = 0xf;
    cfg.id_config[0].addr_en  = 0;

    mon_cnf = bsp_axi_mon_config((u8 *)&cfg, &len);

    if(NULL == mon_cnf)
    {
        printk("axi_mon_test_003 failed\n");
        return BSP_ERROR;
    }

    printf("bsp_axi_mon_config cnf len: %u, reg cnt %u\n", len, mon_cnf->ulRegCnt);
    free(mon_cnf);

    printf("axi_mon_test_003 passed\n");

    return BSP_OK;
}

/* 启动SOC监控测试，axi_mon_test_003成功之后执行 */
s32 axi_mon_test_004(void)
{
    axi_ctrl_t ctrl_stu;

    ctrl_stu.ctrl_type  = AXI_CTRL_START;
    ctrl_stu.mode       = AXI_SOC_CONFIG;

    AXI_CHECK_RET(bsp_axi_mon_start((u8 *)&ctrl_stu), BSP_OK);

    printf("axi_mon_test_004 passed\n");

    return BSP_OK;
}

/* 停止SOC监控测试，axi_mon_test_004成功之后执行 */
s32 axi_mon_test_005(void)
{
    u32 len;
    axi_ctrl_t ctrl_stu;
    AXI_MON_TERMINATE_CNF_STRU * cnf;

    ctrl_stu.ctrl_type  = AXI_CTRL_STOP;
    ctrl_stu.mode       = AXI_SOC_CONFIG;

    cnf = bsp_axi_mon_terminate((u8 *)&ctrl_stu, &len);
    if(NULL == cnf)
    {
       printf("axi_mon_test_004 failed\n");
       return BSP_ERROR;
    }

    printf("bsp_axi_mon_terminate cnf len: %u\n", len);

    free(cnf);

    printf("axi_mon_test_005 passed\n");

    return BSP_OK;
}

/* CPUFAST时间窗测试 */
s32 axi_mon_test_006(void)
{
    u32 len;
    axi_mon_config_t cfg;
    AXI_MON_CONFIG_CNF_STRU * mon_cnf;
    axi_ctrl_t ctrl_stu;

    cfg.mode   = AXI_CPUFAST_CONFIG;
    cfg.win_en = AXI_WIN_ENABLE;

    memset(cfg.id_config, 0, sizeof(axi_mon_id_config_t)*AXI_MAX_ID);

    cfg.id_config[0].id_en  = 1;
    cfg.id_config[0].port   = 1;
    cfg.id_config[0].opt_type.incr_2  = 1;
    cfg.id_config[0].opt_type.but_fin = 1;
    cfg.id_config[0].id_value = 0x0;
    cfg.id_config[0].mask     = 0xff;
    cfg.id_config[0].addr_en  = 0;

    mon_cnf = bsp_axi_mon_config((u8 *)&cfg, &len);

    if(NULL == mon_cnf)
    {
        printf("axi_mon_test_006 config failed\n");
        return BSP_ERROR;
    }

    printf("bsp_axi_mon_config cnf len: %u, reg cnt %u\n", len, mon_cnf->ulRegCnt);
    free(mon_cnf);

    ctrl_stu.ctrl_type  = AXI_CTRL_START;
    ctrl_stu.mode       = AXI_CPUFAST_CONFIG;
    if(BSP_OK != bsp_axi_mon_start((u8 *)&ctrl_stu))
    {
        printf("axi_mon_test_006 start failed\n");
        return BSP_ERROR;
    }

    printf("axi_mon_test_006 start OK, please wait for win over...\n");

    return BSP_OK;
}

#ifdef __cplusplus
}
#endif


