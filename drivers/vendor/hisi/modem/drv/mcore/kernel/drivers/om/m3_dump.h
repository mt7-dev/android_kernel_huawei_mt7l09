
#ifndef	__M3_DUMP_H__
#define __M3_DUMP_H__


#include "bsp_memmap.h"
#include "product_config.h"
#include "osl_types.h"


/**************************************************************************
  宏定义
**************************************************************************/
struct hpm_tem_print
{
    unsigned int hpm_hvt_opc;
	unsigned int hpm_uhvt_opc;
	unsigned int hpm_svt_opc;
	signed int	 temperature;
	unsigned char up_volt;
};

/*全局内存分配*/
#define DUMP_MEM_BASE                       (DDR_MNTN_ADDR_VIRT)
#define DUMP_MEM_TOTAL_SIZE                 (DDR_MNTN_SIZE)
#define DUMP_GLOBAL_INFO_ADDR               (DUMP_MEM_BASE)
#define DUMP_GLOBAL_INFO_SIZE               (0x100)
#define DUMP_GLOBAL_MAP_ADDR                (DUMP_MEM_BASE+DUMP_GLOBAL_INFO_SIZE)
#define DUMP_GLOBAL_MAP_SIZE                (0x28)
#define DUMP_LOAD_INFO_ADDR                 (DUMP_GLOBAL_MAP_ADDR+DUMP_GLOBAL_MAP_SIZE)
#define DUMP_LOAD_INFO_SIZE                 (0x30)
#define DUMP_APP_SECTION_ADDR               (DUMP_MEM_BASE+0x200)
#define DUMP_APP_SECTION_SIZE               (0x2FE00)
#define DUMP_COMM_SECTION_ADDR              (DUMP_MEM_BASE+0x30000)
#define DUMP_COMM_SECTION_SIZE              (0x40000)
#define DUMP_M3_SECTION_ADDR                (DUMP_MEM_BASE+0x30000+0x40000)
#define DUMP_M3_SECTION_SIZE                (0x10000)
#define DUMP_FASTBOOT_ADDR                  (DUMP_M3_SECTION_ADDR + DUMP_M3_SECTION_SIZE)
#define DUMP_FASTBOOT_SIZE                  0x100
#define DUMP_M3_REGS_ADDR                   (DUMP_FASTBOOT_ADDR+DUMP_FASTBOOT_SIZE)
#define DUMP_M3_REGS_SIZE                   (0x100)
#define DUMP_M3_TCM0_ADDR                   (DUMP_M3_REGS_ADDR+DUMP_M3_REGS_SIZE)
#define DUMP_M3_TCM0_SIZE                   (HI_M3TCM0_MEM_SIZE)
#define DUMP_M3_TCM1_ADDR                   (DUMP_M3_TCM0_ADDR+DUMP_M3_TCM0_SIZE)
#define DUMP_M3_TCM1_SIZE                   (HI_M3TCM1_MEM_SIZE)
#define DUMP_EXT_SECTION_ADDR               (DUMP_M3_TCM1_ADDR+DUMP_M3_TCM1_SIZE)

/*m3 dump 内存分配*/
#define DUMP_M3_MEM_BASE                    (DUMP_M3_SECTION_ADDR)
#define DUMP_M3_MEM_TOTAL_SIZE              (DUMP_M3_SECTION_SIZE)
#define DUMP_M3_MAP_ADDR                    (DUMP_M3_MEM_BASE)
#define DUMP_M3_MAP_SIZE                    (0x80)
#define DUMP_M3_BASE_INFO_ADDR              (DUMP_M3_MEM_BASE+DUMP_M3_MAP_SIZE)
#define DUMP_M3_BASE_INFO_SIZE              0x180
#define DUMP_M3_TASK_SWITCH_ADDR            (DUMP_M3_BASE_INFO_ADDR+DUMP_M3_BASE_INFO_SIZE)
#define DUMP_M3_TASK_SWITCH_SIZE            (0x1000)
#define DUMP_M3_INTLOCK_ADDR                (DUMP_M3_TASK_SWITCH_ADDR+DUMP_M3_TASK_SWITCH_SIZE)
#define DUMP_M3_INTLOCK_SIZE                (0)
#define DUMP_M3_TASK_STACK_ADDR             (DUMP_M3_INTLOCK_ADDR+DUMP_M3_INTLOCK_SIZE)
#define DUMP_M3_TASK_STACK_SIZE             (0x1000)
#define DUMP_M3_INT_STACK_ADDR              (DUMP_M3_TASK_STACK_ADDR+DUMP_M3_TASK_STACK_SIZE)
#define DUMP_M3_INT_STACK_SIZE              (0)
#define DUMP_M3_ALLTASK_ADDR                (DUMP_M3_INT_STACK_ADDR+DUMP_M3_INT_STACK_SIZE)
#define DUMP_M3_ALLTASK_SIZE                (0x800)
#define DUMP_M3_ALLTASK_TCB_ADDR            (DUMP_M3_ALLTASK_ADDR+DUMP_M3_ALLTASK_SIZE)
#define DUMP_M3_ALLTASK_TCB_SIZE            (0x1000)
#define DUMP_M3_PRINT_ADDR                  (DUMP_M3_ALLTASK_TCB_ADDR+DUMP_M3_ALLTASK_TCB_SIZE)
#define DUMP_M3_PRINT_SIZE                  (0x1000)
#define DUMP_M3_RSV_ADDR                    (DUMP_M3_PRINT_ADDR+DUMP_M3_PRINT_SIZE)
#define DUMP_M3_RSV_SIZE                    (0)


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
#define DUMP_EXT_OM_DRX_ACORE_ADDR          (DUMP_EXT_OM_UTRACE_DATA_ADDR+DUMP_EXT_OM_UTRACE_DATA_SIZE)
#define DUMP_EXT_OM_DRX_ACORE_SIZE          (0x2000)
#define DUMP_EXT_OM_DRX_CCORE_ADDR          (DUMP_EXT_OM_DRX_ACORE_ADDR+DUMP_EXT_OM_DRX_ACORE_SIZE)
#define DUMP_EXT_OM_DRX_CCORE_SIZE          (0x2000)
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

/*用户空间*/

/**/
#define DUMP_INIT_FLAG                      (0x5B5B0002)
#define DUMP_INIT_FLAG_WAIT                 (0x5C5C5C5C)


#define DUMP_INT_IN_FLAG                    0xAAAA
#define DUMP_INT_EXIT_FLAG                  0xBBBB
#define DUMP_INT_UNLOCK_FLAG                0xCCCC
#define DUMP_SAVE_SUCCESS                   0xA4A4A4A4

#define DUMP_TASK_INFO_SIZE                 0x200
#define DUMP_TASK_INFO_STACK_SIZE           (DUMP_TASK_INFO_SIZE - 32*4)

#define DUMP_M3_TASK_NUM_MAX         32

/* M3 sysctrl reg base */
#define M3_SYSCTRL_BASE                     (0xE000ED00)

typedef enum _dump_reboot_ctx_e
{
    DUMP_CTX_TASK        = 0x0,
    DUMP_CTX_INT         = 0x1
}dump_reboot_ctx_t;

typedef enum _dump_vertor_e
{
    STACK_OVERFLOW      = 0x1,
    FIFO_OVERFLOW         = 0x2,
    MAILBOX_OVERFLOW   = 0x3,
    HARD_FAULT= 0xC,
    MEM_MSG         = 0x10,
    BUS_FAULT= 0x14,
    USAGE_FAULT = 0x18,
    END_ERROR
}dump_vertor_e;

typedef struct
{
    u32 reboot_context;     /*0x00  */
    u32 reboot_task;        /*0x04  */
    u8  taskName[16];       /*0x08  */
    u32 reboot_int;         /*0x18  */

    u32 modId;              /*0x1c  */
    u32 arg1;               /*0x20  */
    u32 arg2;               /*0x24  */
    u32 arg3;               /*0x28  */
    u32 arg3_length;        /*0x2c  */

    u32 vec;                /*0x30  */
    u32 cpsr;               /*0x34  */
    u32 regSet[17];         /*0x38  */

    u32 current_task;       /*0x7c */
    u32 current_int;        /*0x80 */

    u32 cpu_usage;          /*0x84 */
    u32 mem_free;           /*0x88 */
    u32 axi_dlock_reg[3];   /*0x94 --- AXI总线挂死寄存器，0x624,0x628,0x658*/
}dump_base_info_t;

// ALL_TASK_TCB
typedef struct
{
    u32 pid;
    u32 entry;
    u32 status;
    u32 policy;
    u32 priority;
    u32 stack_base;
    u32 stack_end;
    u32 stack_high;
    u32 stack_current;
    u8  name[16];
    u32 regs[17];
    u32 offset;
    u32 rsv[1];
    char dump_stack[DUMP_TASK_INFO_STACK_SIZE];
} dump_task_info_t;


typedef struct _dump_map_section_desc_t
{
	u32  offset;
	u32  length;
    char name[8];
}dump_map_section_desc_t;

typedef struct _dump_ext_map_s
{
	dump_map_section_desc_t reserved1;
	dump_map_section_desc_t reserved2;
	dump_map_section_desc_t reserved3;
	dump_map_section_desc_t reserved4;
	dump_map_section_desc_t reserved5;
	dump_map_section_desc_t reserved6;
	dump_map_section_desc_t reserved7;
	dump_map_section_desc_t reserved8;
	dump_map_section_desc_t reserved9;
	dump_map_section_desc_t reserved10;
	dump_map_section_desc_t reserved11;
	dump_map_section_desc_t reserved12;
	dump_map_section_desc_t reserved13;
	dump_map_section_desc_t reserved14;
	dump_map_section_desc_t reserved15;
	dump_map_section_desc_t reserved16;

}dump_ext_map_t;

typedef struct _dump_section_desc_t
{
	u32 offset;
	u32 length;
}dump_section_desc_t;

typedef struct _dump_app_map_s
{
	dump_section_desc_t sec_base;
	dump_section_desc_t sec_task_switch;
	dump_section_desc_t sec_intlock;
	dump_section_desc_t sec_task_stack;
	dump_section_desc_t sec_int_stack;
	dump_section_desc_t sec_all_task;
	dump_section_desc_t sec_all_task_tcb;
	dump_section_desc_t sec_print;
	dump_section_desc_t sec_reg;
	dump_section_desc_t sec_cpu_view;
	dump_section_desc_t sec_mem_info;
	dump_section_desc_t sec_user_data;
	dump_section_desc_t sec_rsv;
}dump_core_map_t;

typedef struct
{
    u32 maxNum;
    u32 front;
    u32 rear;
    u32 num;
    u32 data[1];
} dump_queue_t;

#endif
typedef void (*exc_funcptr)(int, int,int,int,int);

void dump_save_log(void);
void m3_dump_init(void);
extern void printk(const char *fmt, ...);
extern void exc_dump_hook_add(exc_funcptr phookfun);
extern int task_switch_hook_del ( void);


