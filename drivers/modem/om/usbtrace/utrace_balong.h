
#ifndef _UTRACE_BALONG_H_
#define _UTRACE_BALONG_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "bsp_memmap.h"
#include "osl_types.h"      

/**************************************************************************
           寄存器定义(以下寄存器都属于ARM CoreSight寄存器)
**************************************************************************/
/* ======================PTM 寄存器======================= */
#ifndef BSP_CONFIG_HI3630 /* V7R2 */
#define PTM0_REG_BASE           HI_CORESIGHT_PTM0_BASE_ADDR_VIRT    /* Core0 PTM基址 */
#endif
#define PTM1_REG_BASE           HI_CORESIGHT_PTM1_BASE_ADDR_VIRT    /* Core1 PTM基址 */
#define PTM_CTRL                0x000                               /* 控制寄存器 */
#define PTM_TRIGGER             0x008                               /* Trigger Event */
#define PTM_STATUS              0x010                               /* 状态寄存器 */
#define PTM_TEEVR               0x020                               /* Trace Enable Event */
#define PTM_TECR                0x024                               /* Trace Enable Contorl */
#define PTM_ACVR(n)             (0x40 + n*0x4)                      /* Address Comparator Value */
#define PTM_ACTR(n)             (0x80 + n*0x4)                      /* Address Comparator Access Type */
#define PTM_SYNCFR              0x1E0                               /* Sync Frequency */
#define PTM_TSEVR               0x1F8                               /* Timestamp Event */
#define PTM_TRACEID             0x200                               /* Trace ID */
#define PTM_LOCK_ACCESS         0xFB0                               /* Lock寄存器，解锁码0xC5ACCE55 */
#define PTM_LOCK_STATUS         0xFB4                               /* Lock状态寄存器 */

/* ======================FUNNEL 寄存器==================== */
#define FUNNEL_REG_BASE         HI_CORESIGHT_FUNNEL_BASE_ADDR_VIRT  /* FUNNEL基址 */
#define FUNNEL_CTRL             0x000                               /* FUNNEL控制寄存器 */
#define FUNNEL_LOCK_ACCESS      0xFB0                               /* Lock寄存器，解锁码0xC5ACCE55 */
#define FUNNEL_LOCK_STATUS      0xFB4                               /* Lock状态寄存器 */

#ifndef BSP_CONFIG_HI3630 /* V7R2 */

/* ======================STM 寄存器======================= */
#define STM_REG_BASE            HI_CORESIGHT_STM_BASE_ADDR_VIRT     /* STM基址 */
#define STM_PORT_ENABLE         0xE00                               /* Stimulus Port使能寄存器 */
#define STM_PORT_TRIGGER_ENABLE 0xE20                               /* Stimulus Port Trigger使能寄存器 */
#define STM_TRIGGER_CTRL        0xE70                               /* Trigger控制状态寄存器 */
#define STM_TRACE_CTRL          0xE80                               /* STM Trace控制状态寄存器 */
#define STM_LOCK_ACCESS         0xFB0                               /* Lock寄存器，解锁码0xC5ACCE55 */
#define STM_LOCK_STATUS         0xFB4                               /* Lock状态寄存器 */

/* ======================ETF 寄存器======================= */
#define ETF_REG_BASE            HI_CORESIGHT_ETF_BASE_ADDR_VIRT     /* ETF基址 */
#define ETF_RAM_SIZE            0x004                               /* RAM size寄存器 */
#define ETF_STATUS              0x00C                               /* 状态寄存器，指示TMCReady,Empty,Full,MemErr */
#define ETF_RAM_RD_DATA         0x010                               /* FIFO数据读寄存器 */
#define ETF_RAM_RD_POINTER      0x014                               /* 读指针 */
#define ETF_RAM_WR_POINTER      0x018                               /* 写指针 */
#define ETF_CTRL                0x020                               /* 控制寄存器，控制Trace使能 */
#define ETF_RAM_WR_DATA         0x024                               /* 数据写寄存器 */
#define ETF_MODE                0x028                               /* Mode寄存器，Circular buffer, Hardware FIFO, Software FIFO */
#define ETF_BUF_WATER_MARK      0x034                               /* FULL信号上报水线 */
#define ETF_FORMAT_FLUSH_STATUS 0x300                               /* Formatter, Flush状态寄存器 */
#define ETF_FORMAT_FLUSH_CTRL   0x304                               /* Formatter, Flush控制寄存器 */
#define ETF_LOCK_ACCESS         0xFB0                               /* Lock寄存器，解锁码0xC5ACCE55 */
#define ETF_LOCK_STATUS         0xFB4                               /* Lock状态寄存器 */

/* ======================ETR 寄存器======================= */
#define ETR_REG_BASE            HI_CORESIGHT_ETR_BASE_ADDR_VIRT     /* ETR基址 */
#define ETR_RAM_SIZE            0x004                               /* RAM size寄存器, 配置为DDR空间大小 */
#define ETR_STATUS              0x00C                               /* 状态寄存器，指示TMCReady,Empty,Full,MemErr */
#define ETR_RAM_RD_DATA         0x010                               /* FIFO数据读寄存器 */
#define ETR_RAM_RD_POINTER      0x014                               /* 读指针 */
#define ETR_RAM_WR_POINTER      0x018                               /* 写指针 */
#define ETR_CTRL                0x020                               /* 控制寄存器，控制Trace使能 */
#define ETR_RAM_WR_DATA         0x024                               /* 数据写寄存器 */
#define ETR_MODE                0x028                               /* Mode寄存器，Circular buffer, Hardware FIFO, Software FIFO */
#define ETR_BUF_WATER_MARK      0x034                               /* FULL信号上报水线 */
#define ETR_DATA_BUF_ADDR       0x118                               /* 系统DDR基址寄存器，ETR将数据传输到该地址 */
#define ETR_FORMAT_FLUSH_STATUS 0x300                               /* Formatter, Flush状态寄存器 */
#define ETR_FORMAT_FLUSH_CTRL   0x304                               /* Formatter, Flush控制寄存器 */
#define ETR_LOCK_ACCESS         0xFB0                               /* Lock寄存器，解锁码0xC5ACCE55 */
#define ETR_LOCK_STATUS         0xFB4                               /* Lock状态寄存器 */
#endif
/**************************************************************************
                                  宏定义
**************************************************************************/
#define UTRACE_DDR_SIZE         0x2000                              /* DDR缓存空间大小，字节为单位 */
#define UTRACE_WATER_MARK       0x3                                 /* ETR水线配置，当剩余空间小于水线时，ETR上报full中断 */
#define UTRACE_UNLOCK_CODE      0xC5ACCE55                          /* 解锁码 */
#define UTRACE_LOCK_CODE        0x1                                 /* 锁定CoreSight寄存器 */
#define MAX_WAIT_CNT            0x2000                              /* 最大等待计数 */
#define UTRACE_ONSTART_BUF_SIZE (8*1024)                            /* 启动运行模式，ETR使用的循环buffer空间大小 */
#define UTRACE_MAGIC_NUM        0x89ABCDEF                          /* 可维可测扩展备份区，UTRACE标识码 */

/**************************************************************************
                               数据结构定义
**************************************************************************/
/* 配置模块定义，SoC Trace通路需要配置STM, FUNNEL, ETF, ETR */
enum trace_config_enum
{
    TRACE_STM,
    TRACE_PTM0,
    TRACE_PTM1,
    TRACE_FUNNEL,
    TRACE_ETF,
    TRACE_ETR,
    TRACE_BUTT
};
typedef unsigned int trace_config_enum_u32;

/* ETF, ETR模式配置(ETF, ETR是TMC的不同配置) */
enum tmc_mode_enum
{
    TMC_MODE_CIRCULAR_BUFF, /* 循环buffer */
    TMC_MODE_SOFTWARE_FIFO, /* 软FIFO */    
    TMC_MODE_HARDWARE_FIFO, /* 硬FIFO */
    TMC_MODE_BUTT           /* 无效值 */    
};
typedef unsigned int tmc_mode_enum_u32;

/* FUNNEL端口分配 */
enum funnel_port_enum
{
#ifndef BSP_CONFIG_HI3630 /* V7R2 */
    FUNNEL_STM_PORT,            /* STM端口 */
    FUNNEL_PTM_APPA9_PORT,      /* APPA9 PTM端口 */
    FUNNEL_PTM_MODEMA9_PORT,    /* MODEMA9 PTM端口 */
    FUNNEL_M3_PORT = 6          /* M3 ITM端口 */
#else
    FUNNEL_PTM_MODEMA9_PORT = 0,/* MODEMA9 PTM端口 */
    FUNNEL_PTM_BBE16_PORT       /* BBE16 PTM端口 */
#endif
};
typedef unsigned int funnel_port_enum_u32;

/* Trace采集配置 */
enum capt_ctrl_enum
{
    APPA9_ONLY,             /* 只启用A core */
    MODEMA9_ONLY,           /* 只启用C core */
    CAPT_CTRL_BUTT
};
typedef unsigned int capt_ctrl_enum_u32;

/* ETR配置 */
typedef struct
{
    tmc_mode_enum_u32   mode;
    u32                 buf_size;
    u32                 buf_addr;
} etr_config_t;

#ifdef __cplusplus
}
#endif

#endif /* _UTRACE_BALONG_H_ */


