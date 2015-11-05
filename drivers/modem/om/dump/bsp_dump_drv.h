

#ifndef	__BSP_DUMP_DRV_H__
#define __BSP_DUMP_DRV_H__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <bsp_memmap.h>
#ifndef __ASSEMBLY__
#include <linux/semaphore.h>
#include "product_config.h"
#include "osl_types.h"
#endif/*#ifndef __ASSEMBLY__*/

#ifndef BSP_CONFIG_HI3630
/**************************************************************************
  宏定义
**************************************************************************/

/*全局内存分配*/
#define DUMP_MEM_BASE                       DDR_MNTN_ADDR_VIRT
#define DUMP_MEM_TOTAL_SIZE                 DDR_MNTN_SIZE
#define DUMP_GLOBAL_INFO_ADDR               DUMP_MEM_BASE
#define DUMP_GLOBAL_INFO_SIZE               0x100
#define DUMP_GLOBAL_MAP_ADDR                (DUMP_MEM_BASE+DUMP_GLOBAL_INFO_SIZE)
#define DUMP_GLOBAL_MAP_SIZE                0x28
#define DUMP_LOAD_INFO_ADDR                 (DUMP_GLOBAL_MAP_ADDR+DUMP_GLOBAL_MAP_SIZE)
#define DUMP_LOAD_INFO_SIZE                 (0x30)
#define DUMP_APP_SECTION_ADDR               (DUMP_MEM_BASE+0x200)
#define DUMP_APP_SECTION_SIZE               0x2FE00
#define DUMP_COMM_SECTION_ADDR              (DUMP_MEM_BASE+0x30000)
#define DUMP_COMM_SECTION_SIZE              0x40000
#define DUMP_M3_SECTION_ADDR              (DUMP_MEM_BASE+0x30000+0x40000)
#define DUMP_M3_SECTION_SIZE               0x10000
#define DUMP_FASTBOOT_ADDR                  (DUMP_M3_SECTION_ADDR + DUMP_M3_SECTION_SIZE)
#define DUMP_FASTBOOT_SIZE                   0x100
#define DUMP_M3_REGS_ADDR                   (DUMP_FASTBOOT_ADDR+DUMP_FASTBOOT_SIZE)
#define DUMP_M3_REGS_SIZE                   (0x100)
#define DUMP_M3_TCM0_ADDR                   (DUMP_M3_REGS_ADDR+DUMP_M3_REGS_SIZE)
#define DUMP_M3_TCM0_SIZE                   (HI_M3TCM0_MEM_SIZE)
#define DUMP_M3_TCM1_ADDR                   (DUMP_M3_TCM0_ADDR+DUMP_M3_TCM0_SIZE)
#define DUMP_M3_TCM1_SIZE                   (HI_M3TCM1_MEM_SIZE)
#define DUMP_EXT_SECTION_ADDR               (DUMP_M3_TCM1_ADDR+DUMP_M3_TCM1_SIZE)
#define DUMP_EXT_SECTION_SIZE               (DUMP_EXT_MEM_END - DUMP_EXT_SECTION_ADDR)
#define DUMP_MEM_USED_SIZE                  (DUMP_EXT_MEM_END - DUMP_MEM_BASE)

/*APP段内存分配*/
#define DUMP_CORE_BASE_ADDR                 (DUMP_APP_SECTION_ADDR)
#define DUMP_MAP_ADDR                       (DUMP_CORE_BASE_ADDR)
#define DUMP_MAP_SIZE                       (0x80)
#define DUMP_BASE_INFO_ADDR                 (DUMP_MAP_ADDR+DUMP_MAP_SIZE)
#define DUMP_BASE_INFO_SIZE                 (0x180)
#define DUMP_TASK_SWITCH_ADDR               (DUMP_BASE_INFO_ADDR+DUMP_BASE_INFO_SIZE)
#define DUMP_TASK_SWITCH_SIZE               (0x10000)
#define DUMP_INTLOCK_ADDR                   (DUMP_TASK_SWITCH_ADDR+DUMP_TASK_SWITCH_SIZE)
#define DUMP_INTLOCK_SIZE                   (0x1000)
#define DUMP_TASK_STACK_ADDR                (DUMP_INTLOCK_ADDR+DUMP_INTLOCK_SIZE)
#define DUMP_TASK_STACK_SIZE                (0x2000)
#define DUMP_INT_STACK_ADDR                 (DUMP_TASK_STACK_ADDR+DUMP_TASK_STACK_SIZE)
#define DUMP_INT_STACK_SIZE                 (0x0)
#define DUMP_ALLTASK_ADDR                   (DUMP_INT_STACK_ADDR+DUMP_INT_STACK_SIZE)
#define DUMP_ALLTASK_SIZE                   (0x800)
#define DUMP_ALLTASK_TCB_ADDR               (DUMP_ALLTASK_ADDR+DUMP_ALLTASK_SIZE)
#define DUMP_ALLTASK_TCB_SIZE               (0x10000)
#define DUMP_PRINT_ADDR                     (DUMP_ALLTASK_TCB_ADDR+DUMP_ALLTASK_TCB_SIZE)
#define DUMP_PRINT_SIZE                     (0x4000)
#define DUMP_REGS_ADDR                      (DUMP_PRINT_ADDR+DUMP_PRINT_SIZE)
#define DUMP_REGS_SIZE                      (0x3000)                                     // 待定
#define DUMP_CPUVIEW_ADDR                   (DUMP_REGS_ADDR+DUMP_REGS_SIZE)
#define DUMP_CPUVIEW_SIZE                   (0)
#define DUMP_MEMINFO_ADDR                   (DUMP_CPUVIEW_ADDR+DUMP_CPUVIEW_SIZE)
#define DUMP_MEMINFO_SIZE                   (0x3000)
#define DUMP_USER_DATA_ADDR                 (DUMP_MEMINFO_ADDR+DUMP_MEMINFO_SIZE)
#define DUMP_USER_DATA_SIZE                 (0x1000)
#define DUMP_RSV_ADDR                       (DUMP_USER_DATA_ADDR+DUMP_USER_DATA_SIZE)
#define DUMP_RSV_SIZE                       (0)

/*扩展段内存分配*/
#define DUMP_EXT_MAP_ADDR                   (DUMP_EXT_SECTION_ADDR)
#define DUMP_EXT_MAP_SIZE                   (0x100)
#define DUMP_EXT_USB_DATA_ADDR              (DUMP_EXT_MAP_ADDR+DUMP_EXT_MAP_SIZE)
#define DUMP_EXT_USB_DATA_SIZE              (0x2000)
#define DUMP_EXT_SCI0_DATA_ADDR             (DUMP_EXT_USB_DATA_ADDR+DUMP_EXT_USB_DATA_SIZE)
#define DUMP_EXT_SCI0_DATA_SIZE             (0x4000)
#define DUMP_EXT_SCI1_DATA_ADDR             (DUMP_EXT_SCI0_DATA_ADDR+DUMP_EXT_SCI0_DATA_SIZE)
#define DUMP_EXT_SCI1_DATA_SIZE             (0x4000)
#define DUMP_EXT_OM_MSG_ADDR                (DUMP_EXT_SCI1_DATA_ADDR+DUMP_EXT_SCI1_DATA_SIZE)
#define DUMP_EXT_OM_MSG_SIZE                (0)
#define DUMP_EXT_OM_LOG_ADDR                (DUMP_EXT_OM_MSG_ADDR+DUMP_EXT_OM_MSG_SIZE)
#define DUMP_EXT_OM_LOG_SIZE                (0)
#define DUMP_EXT_OM_UTRACE_DATA_ADDR        (DUMP_EXT_OM_LOG_ADDR+DUMP_EXT_OM_LOG_SIZE)
#define DUMP_EXT_OM_UTRACE_DATA_SIZE        (0x2400)
#define DUMP_EXT_OM_DRX_ACORE_ADDR        (DUMP_EXT_OM_UTRACE_DATA_ADDR+DUMP_EXT_OM_UTRACE_DATA_SIZE)
#define DUMP_EXT_OM_DRX_ACORE_SIZE        (0x2000)
#define DUMP_EXT_OM_DRX_CCORE_ADDR        (DUMP_EXT_OM_DRX_ACORE_ADDR+DUMP_EXT_OM_DRX_ACORE_SIZE)
#define DUMP_EXT_OM_DRX_CCORE_SIZE        (0x2000)
#define DUMP_EXT_LOW_POWER_ACORE_ADDR       (DUMP_EXT_OM_DRX_CCORE_ADDR+DUMP_EXT_OM_DRX_CCORE_SIZE)
#define DUMP_EXT_LOW_POWER_ACORE_SIZE       (0x400)
#define DUMP_EXT_LOW_POWER_CCORE_ADDR       (DUMP_EXT_LOW_POWER_ACORE_ADDR+DUMP_EXT_LOW_POWER_ACORE_SIZE)
#define DUMP_EXT_LOW_POWER_CCORE_SIZE       (0x400)
#define DUMP_EXT_LOW_POWER_ACORE_BUSERROR                (DUMP_EXT_LOW_POWER_CCORE_ADDR+DUMP_EXT_LOW_POWER_CCORE_SIZE)
#define DUMP_EXT_LOW_POWER_ACORE_BUSERROR_SIZE       (0x400)
#define DUMP_EXT_LOW_POWER_CCORE_BUSERROR                 (DUMP_EXT_LOW_POWER_ACORE_BUSERROR+DUMP_EXT_LOW_POWER_ACORE_BUSERROR_SIZE)
#define DUMP_EXT_LOW_POWER_CCORE_BUSERROR_SIZE       (0x400)
#define DUMP_EXT_LOW_POWER_MCORE_BUSERROR                 (DUMP_EXT_LOW_POWER_CCORE_BUSERROR+DUMP_EXT_LOW_POWER_CCORE_BUSERROR_SIZE)
#define DUMP_EXT_LOW_POWER_MCORE_BUSERROR_SIZE       (0x400)

/* icc */
#define DUMP_EXT_ICC_ACORE_ADDR             (DUMP_EXT_LOW_POWER_MCORE_BUSERROR+DUMP_EXT_LOW_POWER_MCORE_BUSERROR_SIZE)
#define DUMP_EXT_ICC_ACORE_SIZE             (0x400)
#define DUMP_EXT_ICC_CCORE_ADDR             (DUMP_EXT_ICC_ACORE_ADDR+DUMP_EXT_ICC_ACORE_SIZE)
#define DUMP_EXT_ICC_CCORE_SIZE             (0x400)
#define DUMP_EXT_ICC_MCORE_ADDR             (DUMP_EXT_ICC_CCORE_ADDR+DUMP_EXT_ICC_CCORE_SIZE)
#define DUMP_EXT_ICC_MCORE_SIZE             (0x400)
#define DUMP_EXT_BOOST_MCORE_ADDR           (DUMP_EXT_ICC_MCORE_ADDR+DUMP_EXT_ICC_MCORE_SIZE)
#define DUMP_EXT_BOOST_MCORE_SIZE           (0x400)

#define DUMP_TASK_TCB_ADDR                  (DUMP_EXT_BOOST_MCORE_ADDR+DUMP_EXT_BOOST_MCORE_SIZE)
#define DUMP_TASK_TCB_SIZE                  (0x1000)

#define DUMP_EXT_MSP_SLEEP_ADDR             (DUMP_TASK_TCB_ADDR+DUMP_TASK_TCB_SIZE)
#define DUMP_EXT_MSP_SLEEP_SIZE             (0x1000)

#define DUMP_EXT_AMON_ADDR                  (DUMP_EXT_MSP_SLEEP_ADDR+DUMP_EXT_MSP_SLEEP_SIZE)
#define DUMP_EXT_AMON_SIZE                  (0x1000)

#define DUMP_EXT_MEM_END                    (DUMP_EXT_AMON_ADDR+DUMP_EXT_AMON_SIZE)

/*扩展段用于内部的内存分配，这部分内存不计入dump文件，留给各模块自由使用*/
#define DUMP_INTER_OSA_APP_ADDR             (DUMP_EXT_MEM_END)
#define DUMP_INTER_OSA_APP_SIZE             (64*1024)
#define DUMP_INTER_OSA_COMM_ADDR            (DUMP_INTER_OSA_APP_ADDR+DUMP_INTER_OSA_APP_SIZE)
#define DUMP_INTER_OSA_COMM_SIZE            (64*1024)



#define DUMP_INIT_FLAG_PHASE1               (0x5B5B0000)
#define DUMP_INIT_FLAG_PHASE2               (0x5B5B0001)
#define DUMP_INIT_FLAG                      (0x5B5B0002)
#define DUMP_INIT_FLAG_WAIT                 (0x5C5C5C5C)

#define DUMP_TASK_JOB_RESET_LOG             (0x00000001)
#define DUMP_TASK_JOB_SAVE                  (0x00000002)
#define DUMP_TASK_JOB_INIT                  (0x00000004)
#define DUMP_TASK_JOB_REBOOT                (0x00000008)
#define DUMP_TASK_JOB_SAVE_INIT             (DUMP_TASK_JOB_SAVE | DUMP_TASK_JOB_INIT)
#define DUMP_TASK_JOB_SAVE_REBOOT           (DUMP_TASK_JOB_SAVE | DUMP_TASK_JOB_REBOOT)

#ifndef __ASSEMBLY__

struct hpm_tem_print
{
    unsigned int hpm_hvt_opc;
	unsigned int hpm_uhvt_opc;
	unsigned int hpm_svt_opc;
	signed int	 temperature;
	unsigned char up_volt;
};

/**************************************************************************
  STRUCT定义
**************************************************************************/
#ifndef FUNC_VOID
typedef void (*func_void_t)(void);
#define FUNC_VOID func_void_t
#endif
//typedef s32 (*FUNCPTR)(void);

typedef struct _dump_ctrl_s
{
    u32 dump_task_id;
    u32 dump_task_job;
    u32 current_task_info;           /*0xA0 */
    struct semaphore sem_dump_task;
}dump_ctrl_s;

typedef struct
{
    u32 wdtSwitch;
    u32 wdtTimerOut;  /*看门狗超时时间，单位ms*/
    u32 sys_view;

    union
    {
        u32 uintValue;
        struct
        {
            u32 sci_switch     : 1; /* 00: excdump, 01: usbdump, 1x: no dump */
            u32 usb_switch     : 1; /* 2 ARM异常检测开关*/
            u32 pmu_switch     : 1; /* 3 堆栈溢出检测开关*/
            u32 sd_switch      : 1; /* 3 堆栈溢出检测开关*/
            u32 icc_switch     : 1; /* 3 堆栈溢出检测开关*/
            u32 ifc_switch     : 1; /* 4 任务切换记录开关*/
            u32 reserved1      : 26;
        } Bits;
    } mod_cfg;
} OM_NV;


/**************************************************************************
  UNION定义
**************************************************************************/

/**************************************************************************
  OTHERS定义
**************************************************************************/
#define dump_debug(fmt, ...)    (bsp_trace(BSP_LOG_LEVEL_ERROR,   BSP_MODU_DUMP, "[dump]: <%s> "fmt, __FUNCTION__, ##__VA_ARGS__))
#define dump_warning(fmt, ...)  (bsp_trace(BSP_LOG_LEVEL_WARNING, BSP_MODU_DUMP, "[dump]: <%s> "fmt, __FUNCTION__, ##__VA_ARGS__))
#define dump_error(fmt, ...)    (bsp_trace(BSP_LOG_LEVEL_ERROR,   BSP_MODU_DUMP, "[dump]: <%s> "fmt, __FUNCTION__, ##__VA_ARGS__))
#define dump_fetal(fmt, ...)    (bsp_trace(BSP_LOG_LEVEL_FATAL,   BSP_MODU_DUMP, "[dump]: <%s> "fmt, __FUNCTION__, ##__VA_ARGS__))

/**************************************************************************
  函数声明
**************************************************************************/


#endif/*#ifndef __ASSEMBLY__*/
#else /*hi3630*/
#define DUMP_INIT_FLAG                      (0x5B5B0002)
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif    /* End #define __BSP_DUMP_H__ */




