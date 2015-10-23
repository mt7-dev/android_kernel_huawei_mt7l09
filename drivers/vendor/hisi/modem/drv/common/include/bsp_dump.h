#ifndef __BSP_DUMP_H__
#define __BSP_DUMP_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "osl_common.h"
#include "drv_dump.h"
#include "bsp_om.h"

/*****************************************************************************
  1 宏定义
*****************************************************************************/
#define  DUMP_TASK_SWT_THRESHOLD_SIZE   16*1024
#define  DUMP_INT_LOCK_THRESHOLD_SIZE     4*1024
/*****************************************************************************
  2 枚举定义
*****************************************************************************/
typedef DUMP_SAVE_MOD_ENUM          dump_save_modid_t;
typedef DUMP_SAVE_FILE_ENUM         dump_save_file_t;
typedef DUMP_SAVE_FILE_MODE_ENUM    dump_save_file_mode_t;

typedef enum{
    DUMP_SYSVIEW_TASKSWITCH       = 0,
    DUMP_SYSVIEW_INTSWITCH,
    DUMP_SYSVIEW_INTLOCK,
    DUMP_SYSVIEW_BUTT
}dump_sysview_t;

#define NVID_DUMP   0xD111
typedef struct
{
    union
    {
        u32 uintValue;
        struct
        {
            u32 dump_switch    : 2; /* 00: excdump, 01: usbdump, 1x: no dump */
            u32 ARMexc         : 1; /* 2 ARM异常检测开关*/
            u32 stackFlow      : 1; /* 3 堆栈溢出检测开关*/
            u32 taskSwitch     : 1; /* 4 任务切换记录开关*/
            u32 intSwitch      : 1; /* 5 中断记录开关*/
            u32 intLock        : 1; /* 6 锁中断记录开关*/
            u32 appRegSave1    : 1; /* 7 寄存器组1记录开关 */
            u32 appRegSave2    : 1; /* 8 寄存器组2记录开关*/
            u32 appRegSave3    : 1; /* 9 寄存器组3记录开关*/
            u32 commRegSave1   : 1; /* 10 寄存器组1记录开关 */
            u32 commRegSave2   : 1; /* 11 寄存器组2记录开关*/
            u32 commRegSave3   : 1; /* 12 寄存器组3记录开关*/
            u32 sysErrReboot   : 1; /* 13 systemError复位开关*/
            u32 reset_log      : 1; /* 14 强制记录开关，暂未使用*/
            u32 fetal_err      : 1; /* 15 强制记录开关，暂未使用*/
            u32 log_ctrl       : 2; /* bsp_trsce 输出控制*/
            u32 reserved1      : 14;
        } Bits;
    } dump_cfg;

    u32 appRegAddr1;
    u32 appRegSize1;
    u32 appRegAddr2;
    u32 appRegSize2;
    u32 appRegAddr3;
    u32 appRegSize3;

    u32 commRegAddr1;
    u32 commRegSize1;
    u32 commRegAddr2;
    u32 commRegSize2;
    u32 commRegAddr3;
    u32 commRegSize3;

    u32 traceOnstartFlag;           /* 0:开机启动Trace, 非0:开机不启动Trace */
    u32 traceCoreSet;               /* 0:采集A核Trace, 1:采集C核Trace, 2:采集双核Trace */
} dump_nv_s;

typedef enum
{
    DUMP_SAVE_FILE_NORMAL = 0x0,
    DUMP_SAVE_FILE_NEED   = 0xAABB,
    DUMP_SAVE_FILE_END    = 0xCCDD
}dump_save_flag_t;

typedef enum
{
    DUMP_START_POWER_ON = 0,
    DUMP_START_REBOOT   = 0x5A5A1111,
    DUMP_START_EXCH     = 0x5A5A2222,
    DUMP_START_CRASH    = 0x5A5A3333
}dump_start_flag_e;

#ifndef BSP_CONFIG_HI3630
typedef enum _dump_reboot_cpu_e
{
    DUMP_CPU_APP     = 0xaa,
    DUMP_CPU_COMM    = 0xcc,
    DUMP_CPU_M3   = 0xee
}dump_reboot_cpu_t;
#else
typedef enum _dump_reboot_cpu_e
{
    DUMP_CPU_APP     = 0x01000000,
    DUMP_CPU_COMM    = 0x02000000,
    DUMP_CPU_BBE     = 0x03000000,
    DUMP_CPU_LPM3    = 0x04000000,
    DUMP_CPU_IOM3    = 0x05000000,
    DUMP_CPU_HIFI    = 0x06000000,
    DUMP_CPU_TEEOS   = 0x07000000
}dump_reboot_cpu_t;

typedef struct
{
	u32 magic;
	u32 version;
	u32 area_number;
	u32 reserve;
	u8 product_name[16];   /* the pruduct name */
	u8 product_version[16];/* the pruduct version */
	u8 build_time[16];
	u32 except_core;
	u32 except_reason;
}dump_top_head_t;

#endif

typedef enum _dump_reboot_reason_e
{
    DUMP_REASON_NORMAL      = 0x0,
    DUMP_REASON_ARM         = 0x1,
    DUMP_REASON_STACKFLOW   = 0x2,
    DUMP_REASON_WDT         = 0x3,
    DUMP_REASON_PMU         = 0x4,
    DUMP_REASON_REBOOT      = 0x5,
    DUMP_REASON_UNDEF       = 0xff
}dump_reboot_reason_t;

/* 底软调用system_error，module id划分 */
typedef enum _dump_errno_e
{
    DRV_ERRNO_DUMP_ARM_EXC      = 0x1000,   /* arm异常 */
    DRV_ERRNO_DUMP_WDT          = 0x1001,   /* watch dog异常 */
    DRV_ERRNO_ICC_RING_BUFFER   = 0x1010,   /* icc循环buffer错误 */
    DRV_ERRNO_PMU_OVER_CUR      = 0x1020,   /* pmu过流 */
    DRV_ERRNO_PMU_OVER_TEMP     = 0x1021,   /* pmu过温 */
    DRV_ERRNO_PMU_UNDEF_VOL     = 0x1022,   /* pmu欠压 */
    DRV_ERRNO_NV_ICC_FIFO_FULL  = 0x1030,   /* nv icc通道满 */
    DRV_ERRNO_AMON_SOC_WR       = 0x1040,   /* amon soc写操作异常 */
    DRV_ERRNO_AMON_SOC_RD       = 0x1041,   /* amon soc读操作异常 */
    DRV_ERRNO_AMON_CPUFAST_WR   = 0x1042,   /* amon fast写操作异常 */
    DRV_ERRNO_AMON_CPUFAST_RD   = 0x1043,   /* amon fast读操作异常 */
    DRV_ERRNO_MBX_WR_FULL       = 0x1050,   /* 下行邮箱满 */
    DRV_ERRNO_MBX_DSP_IPC       = 0x1051,   /* 上行邮箱满，DSP IPC通知ARM */
    DRV_ERRNO_MBX_PS_TIMEOUT    = 0x1052,   /* PS等DSP消息超时 */
    DRV_ERRNO_MBX_UP_WR         = 0x1053,   /* 上行邮箱写指针异常 */
    DRV_ERRNO_MBX_DL_MOD        = 0x1054,   /* 下行邮箱读指针MOD异常 */
    DRV_ERRNO_BUTT              = 0x1FFF
}dump_errno_t;

typedef struct
{
    u32 init_flag;           /*0xA0 */
    u32 start_flag;
    u32 save_flag;
    u32 ext_save_flag;

    u32 inInterrupt;           /*0xA0 */
    u32 taskSwitchStart;           /*0xA0 */
    u32 IntSwitchStart;           /*0xA0 */

    dump_save_flag_t save_file_flag;           /*0xA0 */
    u32 start_wait_flag;           /*0xA0 */
}dump_core_internal_t;

typedef struct
{
    dump_core_internal_t app_internal;
    dump_core_internal_t comm_internal;
    dump_core_internal_t m3_internal;
	u32 hso_connect_flag;  /* HIDS connect status --by yangzhi */
}dump_global_internal_t;

typedef struct
{
    u32 version;            /* 可维可测版本，用于和PC端版本对应  */
    u8  product[16];        /* 产品名称  */
    u8  product_version[32];/* 软件版本  */
    u8  build_date[16];     /* 版本编译日期  */
    u8  build_time[16];     /* 版本编译时间  */
    u8  build_sha[16];      /* 版本编译索引号  */
    u32 reboot_cpu;         /* 异常CPU  */
    u32 reboot_reason;      /* 异常原因  */
    u32 reboot_time;        /* 异常时间  */
    dump_global_internal_t  internal_info;
}dump_global_info_t;

#ifdef BSP_CONFIG_HI3630
typedef struct _dump_top_head_s {
	u32 magic;
	u32 version;
	u32 area_number;
	u32 reserve;
	u8  product_name[16];/* the pruduct name of rdr data */
	u8  product_version[16];/* the pruduct version of rdr data */
	u8  build_time[16];
	u32 except_core;
	u32 except_reason;
	u8  padding1[0x100 - 0x40 - 8];
}dump_top_head_info_t;

typedef struct _dump_area_map_s {
	u32 offset; /* offset from area, unit is bytes(1 bytes) */
	u32 length; /* unit is bytes */
}dump_area_map_t;

#define RDR_AREA_MAX 8

typedef struct _dump_save_s {
	dump_top_head_info_t top_head;
	dump_area_map_t      area_info[8];
}dump_save_t;

/* dump area head  */
typedef struct _dump_area_head_s
{
    u32 magic_num;  
    u32 field_num;   
    u8  name[8]; 
    u8  version[16]; /* area format version */
}dump_area_head_t;

/* dump area OAM field */
typedef struct _dump_area_field_s
{
    u32 field_id;
    u32 offset_addr;
    u32 length;
    u32 status;
}dump_area_field_t;

/* CP MNTN area maximum filed number */
#define CP_AREA_FIELD_MAX_NUM 64

/* dump field status */
enum
{
    DUMP_FIELD_UNUSED = 0,
    DUMP_FIELD_USED   = 1,
};

/* CP dump area  */
typedef struct _dump_area_s
{
    dump_area_head_t   head;
    dump_area_field_t  fields[CP_AREA_FIELD_MAX_NUM];
    u8                 data[4];    
}dump_area_t;
#endif
/*****************************************************************************
  3 函数声明
*****************************************************************************/

typedef DUMP_SAVE_HOOK dump_save_hook;
typedef void(*func_void)(void);

#ifdef ENABLE_BUILD_OM

void system_error (u32 mod_id, u32 arg1, u32 arg2, char *data, u32 length);
s32  bsp_dump_init(void);
s32  bsp_dump_register_hook(dump_save_modid_t mod_id, dump_save_hook func);
s32  bsp_dump_get_buffer(dump_save_modid_t mod_id, char** buffer, u32* length);
u8*  bsp_dump_register_field(u32 mod_id, u32 length); 
s32  bsp_dump_save_file(dump_save_file_t file_id, char* buffer, u32 length, dump_save_file_mode_t mode);
void bsp_dump_int_lock(bsp_module_e mod_id);
void bsp_dump_int_unlock(bsp_module_e mod_id);
void bsp_dump_set_hso_conn_flag(u32 flag);
s32  bsp_dump_get_sysview_addr(dump_sysview_t mod_id, void *addr);
s32  bsp_dump_register_sysview_hook(dump_sysview_t mod_id, dump_save_hook func);
void bsp_dump_trace_stop(void);
void int_lock_hook_add(func_void in_func,func_void out_func);
void bsp_dump_init_task_name(void);
#ifdef BSP_CONFIG_HI3630
u32  bsp_dump_get_buffer_addr(dump_save_modid_t mod_id);
#endif
#else

static void inline system_error (u32 mod_id, u32 arg1, u32 arg2, char *data, u32 length)
{
    return ;
}

static s32 inline bsp_dump_init(void)
{
    return 0;
}

static s32 inline bsp_dump_register_hook(dump_save_modid_t mod_id, dump_save_hook func)
{
    return 0;
}

static s32 inline bsp_dump_get_buffer(dump_save_modid_t mod_id, char** buffer, u32* length)
{
    return 0;
}

static u8* bsp_dump_register_field(u32 mod_id, u32 length)
{
    return BSP_NULL;
}

static s32  inline bsp_dump_save_file(dump_save_file_t file_id, char* buffer, u32 length, dump_save_file_mode_t mode)
{
    return 0;
}

static void inline bsp_dump_int_lock(bsp_module_e mod_id)
{
    return ;
}

static void inline bsp_dump_int_unlock(bsp_module_e mod_id)
{
    return ;
}

static s32  inline bsp_dump_get_sysview_addr(dump_sysview_t mod_id, void *addr)
{
    return 0;
}


static s32 inline bsp_dump_register_sysview_hook(dump_sysview_t mod_id, dump_save_hook func)
{
    return 0;
}


static void inline bsp_dump_trace_stop(void)
{
    return;
}

static void inline int_lock_hook_add(func_void in_func,func_void out_func)
{
    return ;
}

static void inline bsp_dump_init_task_name(void)
{
    return;
}

#ifdef BSP_CONFIG_HI3630
static u32 inline bsp_dump_get_buffer_addr(dump_save_modid_t mod_id)
{
    return 0;
}
#endif
#endif

/*****************************************************************************
  4 错误码声明
*****************************************************************************/
#define BSP_ERR_DUMP_BASE               (int)(0x80000000 | (BSP_DEF_ERR(BSP_MODU_DUMP, 0)))
#define BSP_ERR_DUMP_INIT_FAILED        (BSP_ERR_DUMP_BASE + 0x1)
#define BSP_ERR_DUMP_INVALID_MODULE     (BSP_ERR_DUMP_BASE + 0x1)
#define BSP_ERR_DUMP_INVALID_FILE       (BSP_ERR_DUMP_BASE + 0x2)
#define BSP_ERR_DUMP_INVALID_PARAM      (BSP_ERR_DUMP_BASE + 0x3)
#define BSP_ERR_DUMP_NO_BUF             (BSP_ERR_DUMP_BASE + 0x4)
#define BSP_ERR_DUMP_SOCP_ERR           (BSP_ERR_DUMP_BASE + 0x5)


#ifdef __cplusplus
}
#endif


#endif


