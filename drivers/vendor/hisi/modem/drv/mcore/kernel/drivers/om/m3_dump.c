
/*lint -save -e537*/
#include "product_config.h"
#include "osl_types.h"
#include "bsp_dump.h"
#include "bsp_ipc.h"
#include "bsp_icc.h"
#include "m3_dump.h"
#include "ring_buffer.h"
#include "console.h"
#include "bsp_hardtimer.h"
#include "bsp_wdt.h"
#include "hi_syssc.h"
#include "bsp_memmap.h"
#include "cmsis_os.h"
#include "libc.h"
#include <drv_onoff.h>
#include <rt_Task.h>
/*lint -restore*/
dump_nv_s            g_dump_cfg = {{0}};
dump_global_info_t  *g_dump_global_info = BSP_NULL;
dump_base_info_t    *g_dump_base_info = BSP_NULL;
dump_task_info_t    *g_dump_task_info = BSP_NULL;
dump_core_map_t     *g_dump_core_map = BSP_NULL;
dump_ext_map_t      *g_dump_ext_map = BSP_NULL;
/* r0~r14,pc,xpsr,msp,psp,lr(saving in msp) */
/* 软件主动system_error，只记录r0~r14,pc,xpsr */
u32                 g_dump_regs[20]= {0};
u32                 g_sys_err_flag = true;
/*dump_save_hook       g_dump_hook[DUMP_SAVE_MOD_BUTT] = {0};*/

extern struct simple_console g_console;
/*lint -save -e452*/
typedef void (*funcptr)(int, int);
/*lint -restore +e452*/

#ifdef BSP_CONFIG_HI3630
typedef int (*funcptr_int)(int,int);
funcptr_int g_pIntSwitchHook = BSP_NULL;
#endif

/*
#define dump_save_ext_done()\
    do{\
        g_dump_global_info->internal_info.m3_internal.ext_save_flag = DUMP_SAVE_SUCCESS;\
    }while(0)
*/

#define dump_save_m3_done()\
        do{\
            g_dump_global_info->internal_info.m3_internal.save_flag = DUMP_SAVE_SUCCESS;\
        }while(0)

#define DUMP_T_TASK_ERROR(mod_id)      (mod_id & (1<<24))

static void QueueInit(dump_queue_t *Q, u32 elementNum)
{
    Q->maxNum = elementNum;
    Q->front = 0;
    Q->rear = 0;
    Q->num = 0;
    memset((void *)Q->data, 0, (elementNum*sizeof(u32)));
}

__inline__ s32 QueueIn(dump_queue_t *Q, u32 element)
{
    if (Q->num == Q->maxNum)
    {
        return -1;
    }

    Q->data[Q->rear] = element;
    Q->rear = (Q->rear+1) % Q->maxNum;
    Q->num++;

    return 0;
}

__inline__ s32 QueueLoopIn(dump_queue_t *Q, u32 element)
{
    if (Q->num < Q->maxNum)
    {
        return QueueIn(Q, element);
    }
    else
    {
        Q->data[Q->rear] = element;
        Q->rear = (Q->rear+1) % Q->maxNum;
        Q->front = (Q->front+1) % Q->maxNum;
    }

    return 0;
}

__inline__ void dump_interrrupt_enter(void)
{
    g_dump_global_info->internal_info.m3_internal.inInterrupt++;
}

__inline__ void dump_interrrupt_exit(void)
{
    g_dump_global_info->internal_info.m3_internal.inInterrupt--;
}

__inline__ int dump_in_interrrupt(void)
{
    return (int)g_dump_global_info->internal_info.m3_internal.inInterrupt;
}

u32 om_timer_tick_get(void)
{
    u32 omTimerValue;
    /*lint -save -e958*/
	u64 omTick;
    /*lint -restore*/
	omTimerValue = bsp_get_slice_value();
	omTick=((unsigned long long)omTimerValue*100)>>(15);
	return (UINT32)omTick;
}

__inline__ void dump_wait_for_reboot(void)
{
#define DUMP_WAIT_MAX   700   //7S
    u32 time_start, time_cur;

    time_start = om_timer_tick_get();

    do{
        if(DUMP_SAVE_FILE_END == g_dump_global_info->internal_info.app_internal.save_file_flag)
        {
            g_dump_global_info->internal_info.app_internal.save_file_flag = (dump_save_flag_t)1;
        }

        time_cur = om_timer_tick_get();
        if(DUMP_WAIT_MAX < (time_cur - time_start))
        {
            g_dump_global_info->internal_info.app_internal.save_file_flag = (dump_save_flag_t)2;
            break;
        }
    }while(1);

    /* 配置DDR进入自刷新 */

    /* 重启系统 */
    g_dump_global_info->internal_info.app_internal.save_file_flag = DUMP_SAVE_FILE_NEED;
    if(1 == g_dump_cfg.dump_cfg.Bits.sysErrReboot)
    {
        bsp_drv_power_reboot_direct();
    }
}



static s32 dump_config_init(void)
{
    u32 ret;

    ret   = bsp_nvm_read(NVID_DUMP, (u8 *)&g_dump_cfg, sizeof(dump_nv_s));

    if (BSP_OK != ret)
    {
        /*使用默认值*/
         g_dump_cfg.dump_cfg.uintValue = 0;

        g_dump_cfg.dump_cfg.Bits.dump_switch     = 0x1;
        g_dump_cfg.dump_cfg.Bits.ARMexc          = 0x1;
        g_dump_cfg.dump_cfg.Bits.stackFlow       = 0x1;
        g_dump_cfg.dump_cfg.Bits.taskSwitch      = 0x1;
        g_dump_cfg.dump_cfg.Bits.intSwitch       = 0x1;
        g_dump_cfg.dump_cfg.Bits.intLock         = 0x1;
        g_dump_cfg.dump_cfg.Bits.sysErrReboot    = 0x1;
        g_dump_cfg.dump_cfg.Bits.reset_log       = 0x1;
        g_dump_cfg.dump_cfg.Bits.fetal_err       = 0x1;
    }

    return BSP_OK;
}

void dump_trace_stop(void)
{
    int_switch_hook_delete();
    task_switch_hook_del();
    g_dump_global_info->internal_info.m3_internal.taskSwitchStart = BSP_FALSE;
    g_dump_global_info->internal_info.m3_internal.IntSwitchStart = BSP_FALSE;

    return;
}

static void dump_save_base_info(u32 mod_id, u32 arg1, u32 arg2, char *data, u32 length)
{
    if(BSP_MODU_OTHER_CORE == mod_id)
    {
        g_dump_base_info->reboot_task = (u32)(-1);
        g_dump_base_info->reboot_int = (u32)(-1);
    }
    else
    {
        /* 当前处于任务 */
        if (0 == dump_in_interrrupt())
        {
            g_dump_base_info->reboot_task = g_dump_base_info->current_task;
            memset(g_dump_base_info->taskName, 0, sizeof(g_dump_base_info->taskName));
            g_dump_base_info->reboot_int = (u32)(-1);
        }
        /* 当前处于中断 */
        else
        {
            g_dump_base_info->reboot_task = (u32)(-1);
            memset(g_dump_base_info->taskName, 0, sizeof(g_dump_base_info->taskName));
            g_dump_base_info->reboot_int = g_dump_base_info->current_int;
        }
    }

    /*记录当前状态是任务态还是中断态*/
    if (!dump_in_interrrupt())
    {
        g_dump_base_info->reboot_context = DUMP_CTX_TASK;
    }
    else
    {
        g_dump_base_info->reboot_context = DUMP_CTX_INT;
    }

    g_dump_base_info->modId = mod_id;
    g_dump_base_info->arg1 = arg1;
    g_dump_base_info->arg2 = arg2;
    g_dump_base_info->arg3 = (u32)data;
    g_dump_base_info->arg3_length = length;

    return;
}


void dump_save_exc_task(void)
{
    P_TCB ptcb = (P_TCB)g_dump_base_info->current_task;
    u16 len = 1024;

    /* 保存任务栈 */
    //len = ptcb->priv_stack;

    len = (len> DUMP_M3_TASK_STACK_SIZE) ? DUMP_M3_TASK_STACK_SIZE : (len);

    memcpy((void * )DUMP_M3_TASK_STACK_ADDR, (const void * )(ptcb->stack ), len);
    g_dump_core_map->sec_task_stack.length = len;

    /*获取tadk_id*/
    //g_dump_base_info->current_task = (*(g_dump_base_info->current_task))&0xff;
    return;
}

__inline__ void dump_save_arm_regs(void)
{    
    asm volatile(
        "push {r14}\n"
        "ldr r14, =g_dump_regs\n"
        "str r0, [r14,#0x00]\n"
        "str r1, [r14,#0x04]\n"
        "str r2, [r14,#0x08]\n"
        "str r3, [r14,#0x0C]\n"
        "str r4, [r14,#0x10]\n"
        "str r5, [r14,#0x14]\n"
        "str r6, [r14,#0x18]\n"
        "str r7, [r14,#0x1C]\n"
        "str r8, [r14,#0x20]\n"
        "str r9, [r14,#0x24]\n"
        "str r10, [r14,#0x28]\n"
        "str r11, [r14,#0x2C]\n"
        "str r12, [r14,#0x30]\n"
        "str r13, [r14,#0x34]\n"
        "ldr r0, =g_dump_regs\n"
        "pop {r14}\n"
        "str r14, [r0,#0x38]\n"
        "push {r1}\n"
        "mov r1, pc\n"
        "str r1, [r0,#0x3C]\n"
        "mrs r1, xpsr\n"
        "str r1, [r0,#0x40]\n"      
        "pop {r1}\n"
    );
}

void dump_save_mcore_sys_reg(void)
{
    u32 len;
    u32 i;
    u32 * dst_buf;
    u32 reg_addr;

    dst_buf = (u32 *)DUMP_M3_REGS_ADDR;
    len = DUMP_M3_REGS_SIZE/4;

    for(i=0; i<len; i++)
    {
        reg_addr = M3_SYSCTRL_BASE + i*4;
        *dst_buf = readl(reg_addr);
        dst_buf++;
    }
}

void dump_save_mcore_tcm(void)
{
    u32 i;
    u32 * src_buf;
    u32 * dst_buf;
    u32 len;

    /* 保存TCM0 */
    src_buf = (u32 *)HI_M3TCM0_MEM_ADDR;
    dst_buf = (u32 *)DUMP_M3_TCM0_ADDR;
    len = DUMP_M3_TCM0_SIZE/4;
    
    for(i=0; i<len; i++)
    {
        *dst_buf = *src_buf;
        src_buf++;
        dst_buf++;
    }

    /* 保存TCM1 */
    src_buf = (u32 *)HI_M3TCM1_MEM_ADDR;
    dst_buf = (u32 *)DUMP_M3_TCM1_ADDR;
    len = DUMP_M3_TCM1_SIZE/4;
    
    for(i=0; i<len; i++)
    {
        *dst_buf = *src_buf;
        src_buf++;
        dst_buf++;
    }    
}

void dump_int_handle(s32 param)
{
    if (DUMP_INIT_FLAG !=g_dump_global_info->internal_info.m3_internal.init_flag)
    {
        /*等待重启*/
        dump_wait_for_reboot();
        return;
    }

    dump_trace_stop();

     /* 首先保存系统异常基本信息 */
    dump_save_base_info(BSP_MODU_OTHER_CORE, 0, 0, BSP_NULL, 0);

    /*保存异常任务信息*/
    dump_save_exc_task();

    /*保存打印信息*/
    dump_save_log();

    /*保存寄存器信息*/
    //dump_save_regs();

    /*保存所有任务信息*/
     /*M3数据保存完成 */
    dump_save_m3_done();

    dump_wait_for_reboot();;

    return;
}

static void dump_map_init(void)
{
    g_dump_core_map = (dump_core_map_t*)DUMP_M3_MAP_ADDR;

    memset(g_dump_core_map,0,sizeof(dump_core_map_t));

    g_dump_core_map->sec_base.offset = DUMP_M3_BASE_INFO_ADDR - DUMP_MEM_BASE;
    g_dump_core_map->sec_base.length = DUMP_M3_BASE_INFO_SIZE;
    g_dump_core_map->sec_task_switch.offset = DUMP_M3_TASK_SWITCH_ADDR- DUMP_MEM_BASE;
    g_dump_core_map->sec_task_switch.length = DUMP_M3_TASK_SWITCH_SIZE;
    g_dump_core_map->sec_intlock.offset = DUMP_M3_INTLOCK_ADDR- DUMP_MEM_BASE;
    g_dump_core_map->sec_intlock.length = DUMP_M3_INTLOCK_SIZE;
    g_dump_core_map->sec_task_stack.offset = DUMP_M3_TASK_STACK_ADDR - DUMP_MEM_BASE;
    g_dump_core_map->sec_task_stack.length = DUMP_M3_TASK_STACK_SIZE;
    g_dump_core_map->sec_int_stack.offset = DUMP_M3_INT_STACK_ADDR- DUMP_MEM_BASE;
    g_dump_core_map->sec_int_stack.length = DUMP_M3_INT_STACK_SIZE;
    g_dump_core_map->sec_all_task.offset = DUMP_M3_ALLTASK_ADDR- DUMP_MEM_BASE;
    g_dump_core_map->sec_all_task.length = DUMP_M3_ALLTASK_SIZE;
    g_dump_core_map->sec_all_task_tcb.offset = DUMP_M3_ALLTASK_TCB_ADDR- DUMP_MEM_BASE;
    g_dump_core_map->sec_all_task_tcb.length = DUMP_M3_ALLTASK_TCB_SIZE;
    g_dump_core_map->sec_print.offset = DUMP_M3_PRINT_ADDR- DUMP_MEM_BASE;
    g_dump_core_map->sec_print.length = DUMP_M3_PRINT_SIZE;


    g_dump_base_info = (dump_base_info_t *)DUMP_M3_BASE_INFO_ADDR;
    g_dump_task_info = (dump_task_info_t *)DUMP_M3_ALLTASK_TCB_ADDR;

    g_dump_ext_map   = (dump_ext_map_t *)DUMP_EXT_MAP_ADDR;


    return;
}

static s32 dump_global_info_init(void)
{
    g_dump_global_info->internal_info.m3_internal.start_flag = DUMP_START_CRASH;
    g_dump_global_info->internal_info.m3_internal.save_flag = BSP_FALSE;
    g_dump_global_info->internal_info.m3_internal.ext_save_flag = BSP_FALSE;
    g_dump_global_info->internal_info.m3_internal.inInterrupt = 0;
    g_dump_global_info->internal_info.m3_internal.taskSwitchStart = BSP_FALSE;
    g_dump_global_info->internal_info.m3_internal.IntSwitchStart = BSP_FALSE;

    return BSP_OK;
}

static s32 dump_base_info_init(void)
{
    int i;

    memset(g_dump_base_info, 0, sizeof(dump_base_info_t));
    g_dump_base_info->vec = 0xff;

   // memset(g_dump_reg_map, 0, sizeof(dump_regs_map_t));

    memset((void *)DUMP_M3_ALLTASK_TCB_ADDR, 0, DUMP_M3_ALLTASK_TCB_SIZE);
    for(i=0; i<DUMP_M3_TASK_NUM_MAX; i++)
    {
        g_dump_task_info[i].pid = 0xffffffff;
    }

    return BSP_OK;
}

static void dump_queue_t_init(void)
{
    /* 初始化任务切换队列 */
    QueueInit((dump_queue_t *)(DUMP_M3_TASK_SWITCH_ADDR), (DUMP_M3_TASK_SWITCH_SIZE - 0x10) / 0x4);

    /* 初始化任务名队列 */
    QueueInit((dump_queue_t *)(DUMP_M3_ALLTASK_ADDR), (DUMP_M3_ALLTASK_SIZE - 0x10) / 0x4);

    return;
}

/*lint -save -e101*/
void dump_task_switch_hook( P_TCB new_tcb)
{
    /* 将要切换的任务ID和时间入队*/
    u32 addr = DUMP_M3_TASK_SWITCH_ADDR;

    if(BSP_FALSE == g_dump_global_info->internal_info.comm_internal.taskSwitchStart)
    {
        return;
    }

    QueueLoopIn((dump_queue_t *)addr, (u32)(new_tcb));
    QueueLoopIn((dump_queue_t *)addr, bsp_get_slice_value());

    g_dump_base_info->current_task = (u32)new_tcb;

    return;
}
/*lint -restore*/

void dump_int_switch_hook(u32 dir, u32 newVec)
{
    u32 addr = DUMP_M3_TASK_SWITCH_ADDR;

    if(BSP_FALSE == g_dump_global_info->internal_info.comm_internal.IntSwitchStart)
    {
        return;
    }

    /*lint -save -e648 */
    /* 将触发的中断ID和时间入队*/
    if (0 == dir)/*中断进入*/
    {
        QueueLoopIn((dump_queue_t *)addr, ((DUMP_INT_IN_FLAG<<16)|newVec));
        dump_interrrupt_enter();
    }
    else/*中断退出*/
    {
        QueueLoopIn((dump_queue_t *)addr, ((DUMP_INT_EXIT_FLAG<<16)|newVec));
        dump_interrrupt_exit();
    }
    QueueLoopIn((dump_queue_t *)addr, bsp_get_slice_value());
    /*lint -restore +e648 */
    g_dump_base_info->current_int = newVec;

    return;
}
/*lint -save -e550*/

void dump_exc_hook( int error_id,int currentTaskId)
{
    s32 lock_key = 0;

    /*lint -save -e26 -e515 -e718 -e746*/
    local_irq_save(lock_key);
    /*lint -restore*/
    if(lock_key)
    {}
    
    g_dump_base_info->vec = (u32)error_id;
    /*g_dump_base_info->cpsr = ;*/
    //g_dump_base_info->current_task = (u32)currentTaskId;
    g_dump_global_info->reboot_reason= DUMP_REASON_ARM;

    /*拷贝寄存器信息*/
    /*memcpy((void * )(g_dump_base_info->regSet), (const void * )(((P_TCB *)currentTaskId)->pExcRegSet), (size_t )(ARM_REGS_NUM*sizeof(u32)));*/

    /*进行全局复位*/
    system_error(DRV_ERRNO_DUMP_ARM_EXC, DUMP_REASON_ARM, 0, 0, 0);

    return;
}
/*lint -restore +e550*/

static void dump_cs_etb(void)
{
    u32     reg_value;
    u32     i;
    u32  *  data;
    u32     dst_addr = DUMP_EXT_OM_UTRACE_DATA_ADDR;

    /* unlock etb, 配置ETF_LOCK_ACCESS */
    writel(0xC5ACCE55, HI_CORESIGHT_ETF_BASE_ADDR_VIRT + 0xFB0);

    /* stop etb, 配置ETF_FORMAT_FLUSH_CTRL */
    reg_value = readl(HI_CORESIGHT_ETF_BASE_ADDR_VIRT + 0x304);
    /* FFCR StopOnFl */
    reg_value |= 1 << 12;
    /* FFCR FlushMem */
    reg_value |= 1 << 6;
    writel(reg_value, HI_CORESIGHT_ETF_BASE_ADDR_VIRT + 0x304);

    /* 等待TMCReady */
    for(i=0; i<2000; i++)
    {
        /* read etb status, 读取ETF_STATUS */
        reg_value = readl(HI_CORESIGHT_ETF_BASE_ADDR_VIRT + 0xc);
        /* bit2为TMCReady指示位 */
        if(0 != (reg_value & 0x4))
        {
            break;
        }
    }

    /* 超时判断 */
    if(i >= 2000)
    {
        return ;
    }
#if 0
    /* 将数据读空，保持ETB读写指针相等，etb内部空间为8k，一次读出4字节 */
    for(i=0; i<(1024*8)/4; i++)
    {
        /* read etb, 读取ETF_RAM_RD_DATA */
        reg_value = readl(HI_CORESIGHT_ETF_BASE_ADDR_VIRT + 0x10);
        if(reg_value == 0xffffffff)
        {
            break;
        }
    }

    /* disable etb, 配置ETF_CTRL */
    writel(0, HI_CORESIGHT_ETF_BASE_ADDR_VIRT + 0x20);
#endif
    /* 导出etb数据 */
    memset((void *)dst_addr, 0x0, DUMP_EXT_OM_UTRACE_DATA_SIZE);
    data = (u32 *)(dst_addr + 8);
    for(i=0; i<(1024*8)/4; i++)
    {
        /* read etb, 读取ETF_RAM_RD_DATA */
        reg_value = readl(HI_CORESIGHT_ETF_BASE_ADDR_VIRT + 0x10);
        *data = reg_value;
        data++;
        if(reg_value == 0xffffffff)
        {
            break;
        }
    }

    /* 0-3字节存放标识码 */
    *((u32 *)dst_addr) = (u32)0x89ABCDEF;
    /* 4-7个字节存放ETB数据长度 */
    *((u32 *)dst_addr + 1) = i*4;

    /* lock etb, 配置ETF_LOCK_ACCESS */
    writel(0x1, HI_CORESIGHT_ETF_BASE_ADDR_VIRT + 0xFB0);
}

void dump_wdt_hook(s32 core)
{
    u32 reg_value;

    g_dump_global_info->reboot_reason= DUMP_REASON_WDT;

    if(ACORE_WDT_TIMEOUT == core)
    {
        g_dump_global_info->reboot_cpu = DUMP_CPU_APP;
        g_dump_global_info->internal_info.app_internal.start_flag = DUMP_START_EXCH;
    }
    else if(CCORE_WDT_TIMEOUT == core)
    {
        g_dump_global_info->reboot_cpu = DUMP_CPU_COMM;
        g_dump_global_info->internal_info.comm_internal.start_flag = DUMP_START_EXCH;
    }
    else
    {
        g_dump_global_info->reboot_cpu = DUMP_CPU_M3;
        g_dump_global_info->internal_info.comm_internal.start_flag = DUMP_START_EXCH;
    }

    /* AXI桥锁死状态寄存器0 */
    reg_value = readl(HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT9_OFFSET);
    g_dump_base_info->axi_dlock_reg[0] = reg_value;
     /* AXI桥锁死状态寄存器1 */
    reg_value = readl(HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT10_OFFSET);
    g_dump_base_info->axi_dlock_reg[1] = reg_value;
    /* AXI桥锁死状态寄存器2 */
    reg_value = readl(HI_SYSCTRL_BASE_ADDR_VIRT + HI_SC_STAT22_OFFSET);
    g_dump_base_info->axi_dlock_reg[2] = reg_value;

    /* dump etb */
    if(1 == g_dump_cfg.traceOnstartFlag)
    {
        /* 监控CPU发生挂死 */
        if(((g_dump_global_info->reboot_cpu == DUMP_CPU_APP) && (g_dump_cfg.traceCoreSet == 0))
            || ((g_dump_global_info->reboot_cpu == DUMP_CPU_COMM) && (g_dump_cfg.traceCoreSet == 1)))
        {
            dump_cs_etb();
        }
    }
    /*进行全局复位*/
    system_error(DRV_ERRNO_DUMP_WDT, DUMP_REASON_WDT, 0, 0, 0);
}


static void dump_register_hook(void)
{
    /*lint -save -e64 */
    /*向操作系统注册钩子函数*/
    if(1 == g_dump_cfg.dump_cfg.Bits.taskSwitch)
    {
        //task_switch_hook_add((func_void_t)dump_task_switch_hook); 

    }
    if(1 == g_dump_cfg.dump_cfg.Bits.intSwitch)
    {
        //int_switch_hook_add((func_void_t)dump_int_switch_hook);
    }
    if(1 == g_dump_cfg.dump_cfg.Bits.ARMexc)
    {
        exc_dump_hook_add((exc_funcptr)dump_exc_hook);
    }
    /*lint -restore +e64 */
     /*缺看门狗中断*/
    bsp_wdt_register_hook(dump_wdt_hook);

    g_dump_global_info->internal_info.m3_internal.taskSwitchStart = BSP_TRUE;
    g_dump_global_info->internal_info.m3_internal.IntSwitchStart = BSP_TRUE;

    return;
}

static void dump_init(void)
{
    s32 ret;

    g_dump_global_info = (dump_global_info_t*)DUMP_MEM_BASE;

    g_dump_global_info->internal_info.m3_internal.start_wait_flag = 0;

    /*获取配置*/
    dump_config_init();

    /* 注册中断 */
    ret = bsp_ipc_int_connect(IPC_MCU_INT_SRC_DUMP, (voidfuncptr)dump_int_handle, 0);
    if(OK != ret)
    {
        return ;
    }

    ret = bsp_ipc_int_enable(IPC_MCU_INT_SRC_DUMP);
    if(OK != ret)
    {
        return ;
    }

     /*内存布局*/
    dump_map_init();

    /*全局信息初始化*/
    dump_global_info_init();

    dump_base_info_init();

    /*初始化队列*/
    dump_queue_t_init();

    /*系统运行回调函数注册*/
    dump_register_hook();

    /* 清空PRINT缓存 */
    memset((void *)DUMP_M3_PRINT_ADDR, 0, DUMP_M3_PRINT_SIZE);
    
    g_dump_global_info->internal_info.m3_internal.init_flag = DUMP_INIT_FLAG;

    return ;
}

static s32 m3_dump_icc_read_cb(u32 id , u32 len, void* context)
{
    s32 ret = 0;
    /*lint -save -e813*/
    u8 data[len];
    /*lint -restore*/
    ret = bsp_icc_read(id, data, len);
    if(len != (u32)ret)
    {
        return -1;
    }

    /*目前A核dump模块与M3 dump模块只有初始化同步，后续如果有其他功能可以扩展*/
    dump_init();

    return BSP_OK;

}

void m3_dump_init(void)
{
    s32 ret;
	u32 channel_id = 0;

	/* m3 acore icc */
	channel_id= (ICC_CHN_MCORE_ACORE << 16) | MCORE_ACORE_FUNC_DUMP;
	ret = bsp_icc_event_register(channel_id, m3_dump_icc_read_cb, NULL, (write_cb_func)NULL, (void *)NULL);
	if(ret != BSP_OK)
	{
        /*lint -save -e18*/
		printk("register m3_dump icc error\n");
        /*lint -restore +e18*/
	}
}

 static void dump_save_global_info(void)
{
    //如果是上层复位，标志复位状态
    if(DUMP_REASON_UNDEF == g_dump_global_info->reboot_reason)
    {
        g_dump_global_info->reboot_reason = DUMP_REASON_NORMAL;
    }

    if(DUMP_REASON_WDT != g_dump_global_info->reboot_reason)
    {
        g_dump_global_info->reboot_cpu= DUMP_CPU_M3;
        g_dump_global_info->internal_info.m3_internal.start_flag = DUMP_START_EXCH;
    }

    g_dump_global_info->reboot_time= bsp_get_slice_value();

    return;
}

void dump_save_log(void)
{   /*lint -save -e18 */
    struct s_ring_buffer *rb = g_console.dev.get_tx_char_context;
    /*lint -restore +e18 */
    unsigned char * pre_pos;
    unsigned int    pre_size;
    unsigned int    next_size;
    
    if(rb->in > rb->size)
    {
        next_size = (rb->in)%(rb->size);
        pre_pos   = rb->buffer + next_size;
        pre_size  = rb->size - next_size;
        memcpy((char*)DUMP_M3_PRINT_ADDR, pre_pos, pre_size);
        memcpy((char*)DUMP_M3_PRINT_ADDR+pre_size, rb->buffer, next_size);        
    }
    else
    {
        pre_size = rb->in;
        memcpy((char*)DUMP_M3_PRINT_ADDR, rb->buffer, pre_size);
    }   
}

/*lint -esym(18, system_error) */
/*lint -esym(516, system_error) */
/*lint -esym(532, system_error) */
void system_error (u32 mod_id, u32 arg1, u32 arg2, char *data, u32 length)
{
    if(g_sys_err_flag == true)
    {
        dump_save_arm_regs();
        dump_save_mcore_sys_reg();
        dump_save_mcore_tcm(); 
    }
    printk("system error!\n");

    if (DUMP_INIT_FLAG != g_dump_global_info->internal_info.m3_internal.init_flag)
    {
        return;
    }

    dump_trace_stop();

    /* 首先保存系统异常基本信息 */
    dump_save_global_info();
    dump_save_base_info(mod_id, arg1, arg2, data, length);

    /*保存异常任务信息*/
    dump_save_exc_task();

     /* 通知ACPU进行异常处理 */
    bsp_ipc_int_send(IPC_CORE_ACORE, IPC_ACPU_INT_MCU_SRC_DUMP);

    /*保存打印信息*/
    dump_save_log();

    /*保存寄存器信息*/
   /* dump_save_regs();*/

    /* 保存扩展区数据 */
    /*dump_save_ext();*/

    /*保存所有任务信息*/
    /*M3数据保存完成 */
    dump_save_m3_done();

    dump_wait_for_reboot();

    return;
}

__inline__ void dump_save_arm_exc_regs(void)
{
    asm volatile(
        "ldr    r0, =g_dump_regs\n"
        "str    r4, [r0, #16]\n"
        "str    r5, [r0, #20]\n"
        "str    r6, [r0, #24]\n"
        "str    r7, [r0, #28]\n"
        "str    r8, [r0, #32]\n"
        "str    r9, [r0, #36]\n"
        "str    r10, [r0, #40]\n"
        "str    r11, [r0, #44]\n"
        "mrs    r2, msp\n"
        "mrs    r3, psp\n"
        "/* save lr, lr is in stack */\n"
        "ldr    r1, [r2]\n"
        "str    r1, [r0, #76]\n"
        "/* push lr to msp when in handler, true msp is one word above */\n"
        "add    r2, r2, #4\n"
        "/* make sure which stack is in use */\n"
        "tst    r1, #4\n"
        "/* m3 will push r0~r3,r12,lr,pc,xpsr to stack before go into handler */\n"
        "ittee  eq\n"
        "addeq  r2, r2, #32\n"
        "moveq  r1, r2\n"
        "addne  r3, r3, #32\n"
        "movne  r1, r3\n"
        "/* save r13,msp,psp */\n"
        "str    r1, [r0, #52]\n"
        "str    r2, [r0, #68]\n"
        "str    r3, [r0, #72]\n"
        "sub    r2, r1, #32\n"
        "/* save r0~r3,r12,lr,pc,xpsr */\n"
        "ldr    r1, [r2]\n"
        "str    r1, [r0]\n"
        "ldr    r1, [r2, #4]\n"
        "str    r1, [r0, #4]\n"
        "ldr    r1, [r2, #8]\n"
        "str    r1, [r0, #8]\n"
        "ldr    r1, [r2, #12]\n"
        "str    r1, [r0, #12]\n"
        "ldr    r1, [r2, #16]\n"
        "str    r1, [r0, #48]\n"
        "ldr    r1, [r2, #20]\n"
        "str    r1, [r0, #56]\n"
        "ldr    r1, [r2, #24]\n"
        "str    r1, [r0, #60]\n"
        "ldr    r1, [r2, #28]\n"
        "str    r1, [r0, #64]\n"
    );
}

/*lint -save -e718 -e746*/
void hardfault_dump(void)
{
    //dump_save_arm_regs();
    dump_save_arm_exc_regs();
    dump_save_mcore_sys_reg();
    dump_save_mcore_tcm();
    g_sys_err_flag = false;  
    dump_exc_hook(0xc,(int)rt_tsk_self());
}

void mem_manage_dump(void)
{
    //dump_save_arm_regs();
	dump_save_arm_exc_regs();
    dump_save_mcore_sys_reg();
    dump_save_mcore_tcm(); 
    g_sys_err_flag = false;
    dump_exc_hook(0x10,(int)rt_tsk_self());
}

void bus_fault_dump(void)
{
    //dump_save_arm_regs();
	dump_save_arm_exc_regs();
    dump_save_mcore_sys_reg();
    dump_save_mcore_tcm();  
    g_sys_err_flag = false;
    dump_exc_hook(0x14,(int)rt_tsk_self());
}

void usage_fault_dump(void)
{
    //dump_save_arm_regs();
    dump_save_arm_exc_regs();
    dump_save_mcore_sys_reg();
    dump_save_mcore_tcm();
    g_sys_err_flag = false;
    dump_exc_hook(0x18,(int)rt_tsk_self());
}
/*lint -restore*/

