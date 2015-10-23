
#ifndef _AMON_BALONG_H_
#define _AMON_BALONG_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "bsp_memmap.h"
#include "osl_types.h"
#include "drv_amon.h"
#include "hi_amon.h"
#include "bsp_shared_ddr.h"

/**************************************************************************
                                 寄存器定义
**************************************************************************/
/* AXI monitor有两种配置方案: CPUFAST, SOC */
#define AXI_MON_CPUFAST_BASE_ADDR       HI_AMON_CPUFAST_REGBASE_ADDR_VIRT /* AXI monitor CPUFAST基地址 */
#define AXI_MON_SOC_BASE_ADDR           HI_AMON_SOC_REGBASE_ADDR_VIRT     /* AXI monitor SOC基地址 */

/* AXI monitor寄存器偏移地址定义 begin */
#define AXI_MON_CNT_RESET               HI_AMON_CNT_RESET_OFFSET                /* AXI monitor统计计数器软复位寄存器  */
#define AXI_MON_CNT_CTRL                HI_AMON_CNT_CTRL_OFFSET                 /* AXI monitor系统配置寄存器 */
#define AXI_MON_CNT_STATE_INT           HI_AMON_CNT_STATE_INT_OFFSET            /* AXI monitor系统状态寄存器 */
#define AXI_MON_WIN_COUNTER             HI_AMON_WIN_COUNTER_OFFSET              /* AXI monitor系统时间窗计数器 */
#define AXI_MON_IDLE_COUNTER            HI_AMON_IDLE_COUNTER_OFFSET             /* AXI monitor系统idle统计计数器 */
#define AXI_MON_PORT_SEL                HI_AMON_PORT_SEL_OFFSET                 /* AXI monitor监控ID的AXI port配置 */
#define AXI_MON_INT_CLR                 HI_AMON_INT_CLR_OFFSET                  /* AXI monitor中断清除寄存器 */
#define AXI_ID_INT_SRC                  HI_AMON_ID_INT_SRC_OFFSET               /* ID读写监控中断原始寄存器 */
#define AXI_ID_INT_MASK                 HI_AMON_ID_INT_MASK_OFFSET              /* ID读写监控中断屏蔽寄存器 */
#define AXI_ID_INT_STAT                 HI_AMON_ID_INT_STAT_OFFSET              /* ID读写监控中断状态寄存器 */

/* n为port，取值范围0-7 */
#define AXI_MON_RD_WAIT_CYCLE_PORTS(n)  (HI_AMON_RD_WAIT_CYCLE_PORTS_0_OFFSET + n*0x10)   /* 监控PORTs为精确定位读等待计数而消耗的时间周期寄存器 */
#define AXI_MON_WR_WAIT_CYCLE_PORTS(n)  (HI_AMON_WR_WAIT_CYCLE_PORTS_0_OFFSET + n*0x10)   /* 监控PORTs为精确定位写等待计数而消耗的时间周期寄存器 */
/* n为ID, 取值范围0-7 */
#define AXI_MON_CNT_TYPE(n)             (HI_AMON_CNT_TYPE_0_OFFSET + n*0x4)         /* 监控ID的监控类型选择 */
#define AXI_MON_CNT_ID(n)               (HI_AMON_CNT_ID_0_OFFSET + n*0x100)         /* AXI monitor系统ID信息寄存器 */
#define AXI_MON_ID_ADDR_DES(n)          (HI_AMON_ID_ADDR_DES_0_OFFSET + n*0x100)    /* 监控地址下边界 */
#define AXI_MON_ID_ADDR_DES_M(n)        (HI_AMON_ID_ADDR_DES_M_0_OFFSET + n*0x100)  /* 监控地址上边界 */

/* 统计计数器 begin */
#define AXI_MON_INCR1_ID(n)             (HI_AMON_INCR1_ID_0_OFFSET + n*0x100)               /* 监控ID n发起INCR1操作计数器 */
#define AXI_MON_INCR2_ID(n)             (HI_AMON_INCR2_ID_0_OFFSET + n*0x100)               /* 监控ID n发起INCR2操作计数器 */
#define AXI_MON_INCR4_ID(n)             (HI_AMON_INCR4_ID_0_OFFSET + n*0x100)               /* 监控ID n发起INCR4操作计数器 */
#define AXI_MON_INCR8_ID(n)             (HI_AMON_INCR8_ID_0_OFFSET + n*0x100)               /* 监控ID n发起INCR8操作计数器 */
#define AXI_MON_INCR16_ID(n)            (HI_AMON_INCR16_ID_0_OFFSET + n*0x100)              /* 监控ID n发起INCR16操作计数器 */
#define AXI_MON_WRAP_ID(n)              (HI_AMON_WRAP_ID_0_OFFSET + n*0x100)                /* 监控ID n发起WRAP操作计数器 */
#define AXI_MON_BURST_ID(n)             (HI_AMON_BURST_ID_0_OFFSET + n*0x100)               /* 监控ID n发起BURST操作计数器 */
#define AXI_MON_FINISH_ID(n)            (HI_AMON_FINISH_ID_0_OFFSET + n*0x100)              /* 监控ID n完成BURST操作计数器 */
#define AXI_MON_RD_WAIT_ID_LOW(n)       (HI_AMON_READ_WAIT_ID_0_OFFSET + n*0x100)           /* 监控ID n读等待周期计数器，低32位(64位寄存器，0~43有效) */
#define AXI_MON_RD_WAIT_ID_HIGH(n)      (HI_AMON_READ_WAIT_ID_0_OFFSET + 0x4 + n*0x100)     /* 监控ID n读等待周期计数器，高32位(64位寄存器，0~43有效) */
#define AXI_MON_WR_WAIT_ID_LOW(n)       (HI_AMON_WRITE_WAIT_ID_0_OFFSET + n*0x100)          /* 监控ID n写等待周期计数器，低32位(64位寄存器，0~43有效) */
#define AXI_MON_WR_WAIT_ID_HIGH(n)      (HI_AMON_WRITE_WAIT_ID_0_OFFSET + 0x4 + n*0x100)    /* 监控ID n写等待周期计数器，高32位(64位寄存器，0~43有效) */
#define AXI_MON_WR_WAIT_RESP_ID_LOW(n)  (HI_AMON_WRITE_WAIT_RESP_ID_0_OFFSET + n*0x100)     /* 监控ID n写响应等待周期计数器，低32位(64位寄存器，0~43有效) */
#define AXI_MON_WR_WAIT_RESP_ID_HIGH(n) (HI_AMON_WRITE_WAIT_RESP_ID_0_OFFSET + 0x4 + n*0x100)/* 监控ID n写响应等待周期计数器，高32位(64位寄存器，0~43有效) */
#define AXI_MON_RD_MAX_WAIT_ID(n)       (HI_AMON_READ_MAX_WAIT_ID_0_OFFSET + n*0x100)       /* 监控ID n读最大等待周期计数器 */
#define AXI_MON_WR_MAX_WAIT_ID(n)       (HI_AMON_WRITE_MAX_WAIT_ID_0_OFFSET + n*0x100)      /* 监控ID n写最大等待周期计数器 */
#define AXI_MON_WR_MAX_WAIT_RESP_ID(n)  (HI_AMON_WRITE_MAX_WAIT_RESP_ID_0_OFFSET + n*0x100) /* 监控ID n写响应最大等待周期计数器 */
#define AXI_MON_RD_BYTES_ID_LOW(n)      (HI_AMON_READ_BYTES_ID_0_OFFSET + n*0x100)          /* 监控ID n读数据总量，byte为单位，低32位(64位寄存器，0~39有效) */
#define AXI_MON_RD_BYTES_ID_HIGH(n)     (HI_AMON_READ_BYTES_ID_0_OFFSET + 0x4 + n*0x100)    /* 监控ID n读数据总量，byte为单位，高32位(64位寄存器，0~39有效) */
#define AXI_MON_WR_BYTES_ID_LOW(n)      (HI_AMON_WRITE_BYTES_ID_0_OFFSET + n*0x100)         /* 监控ID n写数据总量，byte为单位，低32位(64位寄存器，0~39有效) */
#define AXI_MON_WR_BYTES_ID_HIGH(n)     (HI_AMON_WRITE_BYTES_ID_0_OFFSET + 0x4 + n*0x100)   /* 监控ID n写数据总量，byte为单位，高32位(64位寄存器，0~39有效) */
/* 统计计数器 end */
/* AXI monitor寄存器偏移地址定义 end */

#define AXI_MON_BACK_ADDR               (SHM_MEM_M3_MNTN_ADDR + 32*1024)
#define AXI_MON_BACK_SIZE               (2*1024)
/**************************************************************************
                               数据结构定义
**************************************************************************/
/* AXI配置选择 */
enum axi_config_enum
{
    AXI_CPUFAST_CONFIG,         /* CPUFAST配置 */
    AXI_SOC_CONFIG,             /* SOC配置 */
    AXI_CONFIG_BUTT
};
typedef unsigned int axi_config_enum_uint32;

/* AXI monitor状态 */
enum axi_state_enum
{
    AXI_IDLE            = 0x0,  /* IDLE状态 */
    AXI_WIN_RUNNING     = 0x4,  /* 带时间窗的监控状态 */
    AXI_UNWIN_RUNNING   = 0x8,  /* 不带时间窗的监控状态 */
    AXI_STOP            = 0xC,  /* STOP状态 */
    AXI_RESET_FINISH    = 0x10, /* 软复位完成 */
    AXI_STATE_BUTT      = 0xFFFFFFFF
};
typedef unsigned int axi_state_enum_uint32;

/* 低功耗: 寄存器备份、恢复 */
typedef struct
{ 
    u32     ctrl_reg;                           /* 控制寄存器 begin */
    u32     port_reg;
    u32     int_mask;
    u32     id_reg[AXI_MAX_CONFIG_ID];
    u32     addr_start_reg[AXI_MAX_CONFIG_ID];
    u32     addr_end_reg[AXI_MAX_CONFIG_ID];   /* 控制寄存器 end */
    u32     incr1_reg[AXI_MAX_CONFIG_ID];      /* 统计寄存器 begin */
    u32     incr2_reg[AXI_MAX_CONFIG_ID]; 
    u32     incr4_reg[AXI_MAX_CONFIG_ID];
    u32     incr8_reg[AXI_MAX_CONFIG_ID]; 
    u32     incr16_reg[AXI_MAX_CONFIG_ID]; 
    u32     wrap_reg[AXI_MAX_CONFIG_ID]; 
    u32     burst_send_reg[AXI_MAX_CONFIG_ID]; 
    u32     burst_fin_reg[AXI_MAX_CONFIG_ID]; 
    u32     rd_cnt_low_reg[AXI_MAX_CONFIG_ID];
    u32     rd_cnt_high_reg[AXI_MAX_CONFIG_ID];
    u32     wr_cnt_low_reg[AXI_MAX_CONFIG_ID];
    u32     wr_cnt_high_reg[AXI_MAX_CONFIG_ID];/* 统计寄存器 end */   
} axi_reg_bak_t;
void bsp_amon_suspend();
void bsp_amon_resume();


#ifdef __cplusplus
}
#endif

#endif /* _AMON_BALONG_H_ */

