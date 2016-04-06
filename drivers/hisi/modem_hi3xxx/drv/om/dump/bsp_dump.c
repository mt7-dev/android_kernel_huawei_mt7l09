
/*lint -save -e537*/
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/thread_info.h>
#include <linux/syslog.h>
#include <linux/errno.h>
#include <linux/kthread.h>
#include <linux/semaphore.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/spinlock.h>
#include <asm/current.h>
#include <asm/string.h>
#include <asm/traps.h>
#include "product_config.h"
#include "osl_types.h"
#include "osl_io.h"
#include "osl_bio.h"
#include "DrvInterface.h"
#include "bsp_om_api.h"
#include "bsp_dump.h"
#include "bsp_dump_drv.h"
#include "bsp_om_save.h"
#include "bsp_ipc.h"
#include "bsp_memmap.h"
#include "bsp_wdt.h"
#include "bsp_pmu.h"
#include "bsp_icc.h"
#include "hi_syssc.h"
#include "bsp_utrace.h"
#include "drv_onoff.h"
#include "bsp_nvim.h"
#include "bsp_softtimer.h"
#include "bsp_version.h"
#include "bsp_sram.h"
/*lint -restore*/
dump_global_info_t  *g_dump_global_info = BSP_NULL;
dump_global_map_t   *g_dump_global_map = BSP_NULL;
dump_core_map_t     *g_dump_core_map = BSP_NULL;
dump_ext_map_t      *g_dump_ext_map = BSP_NULL;
dump_base_info_t    *g_dump_base_info = BSP_NULL;
dump_base_info_t    *g_dump_comm_base_info = BSP_NULL;
dump_task_info_t    *g_dump_task_info = BSP_NULL;
dump_regs_map_t     *g_dump_reg_map = BSP_NULL;
dump_ctrl_s          g_dump_ctrl;
dump_nv_s            g_dump_cfg = {{0}};
dump_save_hook       g_dump_hook[DUMP_SAVE_MOD_BUTT] = {0};
dump_save_hook       g_dump_trace_read_cb[DUMP_SYSVIEW_BUTT] = {BSP_NULL};
dump_debug_info_t g_dump_ptr_info = {0};
func_void g_intlock_in_hook = NULL;
func_void g_intlock_out_hook = NULL;
bool g_exc_flag = false;
/*lint -save -e512*/
spinlock_t g_dump_lock;
/*lint -restore */
struct softtimer_list g_dump_timer ;
u32 g_dump_save_task_name = false;

/*lint -esym(526, log_buf_len) */
extern int log_buf_len;
/*lint -esym(526, log_buf) */
extern char *log_buf ;
/*lint -esym(526, log_end) */
extern unsigned log_end;

/*lint -esym(526, exc_hook_add) */
extern void exc_hook_add(FUNC_VOID pHookFunc);
/*lint -save -e762*/
extern void show_mem(unsigned int filter);
/*lint -restore*/
/*lint -esym(526, dump_stack_bl) */
extern void dump_stack_bl(struct task_struct *tsk);

s32 dump_init_phase2(void);
s32 dump_global_info_clear(void);

#define PID_PPID_GET(taskTCB)  ((((struct task_struct *)taskTCB)->pid & 0xffff)| \
                                 ((((struct task_struct *)taskTCB)->real_parent->pid & 0xffff)<< 16))

#define DUMP_T_TASK_ERROR(mod_id)      (mod_id & (1<<24))
#define DUMP_LINUX_TASK_NUM_MAX         128
#define DUMP_REG_SET_MAX                4096

void QueueInit(dump_queue_t *Q, u32 elementNum)
{
    Q->maxNum = elementNum;
    Q->front = 0;
    Q->rear = 0;
    Q->num = 0;
    memset((void *)Q->data, 0, (size_t )(elementNum*sizeof(u32)));
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

s32 QueueOut(dump_queue_t *Q, u32 *element)
{
    if (0 == Q->num)
    {
        return -1;
    }

    *element = Q->data[Q->front];
    Q->front = (Q->front+1) % Q->maxNum;
    Q->num--;

    return 0;
}

int QueueFrontDel(dump_queue_t *Q, u32 del_num)
{
    if (del_num > Q->num)
    {
        return -1;
    }
    Q->front = (Q->front+del_num) % Q->maxNum;
    Q->num   = Q->num - del_num;

    return 0;
}


s32 QueueReadLast(dump_queue_t *Q, u32 *element)
{
    if (0 == Q->num)
    {
        return -1;
    }

    *element = Q->data[((Q->rear+Q->maxNum)-1) % Q->maxNum];

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

u32 QueueNumGet(dump_queue_t *Q)
{
    return Q->num;
}
__inline__ void dump_interrrupt_enter(void)
{
    g_dump_global_info->internal_info.app_internal.inInterrupt++;
}

__inline__ void dump_interrrupt_exit(void)
{
    g_dump_global_info->internal_info.app_internal.inInterrupt--;
}

__inline__ int dump_in_interrrupt(void)
{
    return (int)g_dump_global_info->internal_info.app_internal.inInterrupt;
}

/*lint -save -e123 -e530 -e701 -e713 -e718 -e732 -e737 -e746*/
void dump_task_switch_hook(void *old_tcb, void *new_tcb)
{
    /* 将要切换的任务ID和时间入队*/
    u32 pid_ppid = 0;
    u32 addr = DUMP_TASK_SWITCH_ADDR;
    unsigned long lock_flag;

    if(BSP_FALSE == g_dump_global_info->internal_info.app_internal.taskSwitchStart)
    {
        return;
    }

    pid_ppid = PID_PPID_GET((int)new_tcb);
    spin_lock_irqsave(&g_dump_lock, lock_flag);
    QueueLoopIn((dump_queue_t *)addr, (u32)pid_ppid);
    QueueLoopIn((dump_queue_t *)addr, om_timer_get());

    g_dump_ptr_info.task_swt_ptr = DUMP_TASK_SWITCH_ADDR +0x10
                                                            + (((dump_queue_t *)addr)->rear )*4;

    spin_unlock_irqrestore(&g_dump_lock, lock_flag);

    if(BSP_NULL != g_dump_trace_read_cb[DUMP_SYSVIEW_TASKSWITCH])
    {
        if( (QueueNumGet((dump_queue_t *)addr)*sizeof(u32)) >= DUMP_TASK_SWT_THRESHOLD_SIZE)
        {
            g_dump_trace_read_cb[DUMP_SYSVIEW_TASKSWITCH]();
        }
    }

    g_dump_base_info->current_task = (u32)new_tcb;

    /* 定时器超时，正在记录任务名，任务切换不做记录 */
    if(g_dump_save_task_name == true)
    {
        return;
    }

    /* 开始记录 */
    g_dump_save_task_name = true;

    /* 因为kthreadd派生出来的任务，第一次运行时，任务名都叫kthreadd，所以任务第二次进入时，才记录 */
    if(((struct task_struct*)new_tcb)->dump_magic == (int)new_tcb)
    {
        g_dump_save_task_name = false;
        return;
    }
    else if(((struct task_struct*)new_tcb)->dump_magic == (int)new_tcb + 1)
    {
        u32 task_name_addr = DUMP_ALLTASK_ADDR;
     
        QueueLoopIn((dump_queue_t *)task_name_addr, pid_ppid);
        QueueLoopIn((dump_queue_t *)task_name_addr, *((u32 *)(((struct task_struct *)(new_tcb))->comm)));
        QueueLoopIn((dump_queue_t *)task_name_addr, *((u32 *)((((struct task_struct *)new_tcb)->comm)+4)));
        QueueLoopIn((dump_queue_t *)task_name_addr, *((u32 *)((((struct task_struct *)new_tcb)->comm)+8)));
        ((struct task_struct*)new_tcb)->dump_magic = (int)new_tcb;
    }
    else
    {
        ((struct task_struct*)new_tcb)->dump_magic = (int)new_tcb + 1;
    }

    g_dump_save_task_name = false;    
}

void dump_int_switch_hook(u32 dir, u32 oldVec, u32 newVec)
{
    u32 addr = DUMP_TASK_SWITCH_ADDR;
    unsigned long lock_flag;

    if(BSP_FALSE == g_dump_global_info->internal_info.app_internal.taskSwitchStart)
    {
        return;
    }

    spin_lock_irqsave(&g_dump_lock, lock_flag);
    /* 将触发的中断ID和时间入队*/
    if (0 == dir)/*中断进入*/
    {
        QueueLoopIn((dump_queue_t *)addr, (((u32)DUMP_INT_IN_FLAG<<16)|newVec));
        dump_interrrupt_enter();
    }
    else/*中断退出*/
    {
        QueueLoopIn((dump_queue_t *)addr, (((u32)DUMP_INT_EXIT_FLAG<<16)|newVec));
        dump_interrrupt_exit();
    }
    QueueLoopIn((dump_queue_t *)addr, om_timer_get());

    g_dump_ptr_info.task_swt_ptr = DUMP_TASK_SWITCH_ADDR +0x10
                                                            + (((dump_queue_t *)addr)->rear )*4;

    spin_unlock_irqrestore(&g_dump_lock, lock_flag);

    if(BSP_NULL != g_dump_trace_read_cb[DUMP_SYSVIEW_TASKSWITCH])
    {
        if( (QueueNumGet((dump_queue_t *)addr)*sizeof(u32)) >= DUMP_TASK_SWT_THRESHOLD_SIZE)
        {
            g_dump_trace_read_cb[DUMP_SYSVIEW_TASKSWITCH]();
        }
    }

    g_dump_base_info->current_int = newVec;

    return;
}

void dump_exc_hook(void * currentTaskId, s32 vec, u32* pReg)
{
    if(g_exc_flag)
    {
        return;
    }

    bsp_utrace_stop();

    g_dump_base_info->vec = (u32)vec;
    /*lint -save -e124*/
    g_dump_base_info->cpsr = *(&pReg[ARM_REGS_NUM-1]);
    /*lint -restore +e124*/
    g_dump_base_info->current_task = (u32)currentTaskId;
    g_dump_global_info->reboot_reason= DUMP_REASON_ARM;

    /*拷贝寄存器信息*/
    memcpy((void * )(g_dump_base_info->regSet), (const void * )(pReg), (size_t )(ARM_REGS_NUM*sizeof(u32)));

    /*进行全局复位*/
    system_error(DRV_ERRNO_DUMP_ARM_EXC, DUMP_REASON_ARM, 0, 0, 0);;

    return;
}

void dump_wdt_hook(s32 core)
{
    u32 reg_value;

    g_dump_global_info->reboot_reason= DUMP_REASON_WDT;

    if(ACORE_WDT_TIMEOUT == core)
    {
        g_dump_global_info->reboot_cpu = DUMP_CPU_APP;
    }
    else
    {
        g_dump_global_info->reboot_cpu = DUMP_CPU_COMM;
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

    /*进行全局复位*/
    system_error(DRV_ERRNO_DUMP_WDT, DUMP_REASON_WDT, 0, 0, 0);
}

void dump_stack_hook(int where)
{
    dump_task_info_t *task = (dump_task_info_t *)g_dump_ctrl.current_task_info;

    if( (DUMP_TASK_INFO_STACK_SIZE - task->offset) > (strlen((void*)where)+12) )
    {
        /* coverity[secure_coding] */
    	task->offset += sprintf((char *)task->dump_stack+task->offset, "[%08lx]%pS\n", (unsigned long)where, (void *)where);
    }

    return;
}

void dump_onoff_callback(void)
{
    /* 异常之后，不再置正常重启标志 */
    if(g_exc_flag)
    {
        return;
    }
    
    g_dump_global_info->internal_info.app_internal.start_flag = DUMP_START_REBOOT;
    g_dump_global_info->internal_info.comm_internal.start_flag = DUMP_START_REBOOT;
    g_dump_global_info->internal_info.m3_internal.start_flag = DUMP_START_REBOOT;
    /* 正常重启，置复位标志 */
    g_dump_global_info->reboot_reason = DUMP_REASON_REBOOT;
    g_dump_global_info->reboot_cpu= DUMP_CPU_APP;
}

void dump_register_hook(void)
{
    s32 ret;

    /*向操作系统注册钩子函数*/
    if(1 == g_dump_cfg.dump_cfg.Bits.taskSwitch)
    {
        ret = task_switch_hook_add((FUNC_VOID)dump_task_switch_hook);
        if(OK != ret)
        {
            dump_error("dump_register_hook[%d]: reg task hook failed!\n", __LINE__);
        }
    }
    if(1 == g_dump_cfg.dump_cfg.Bits.intSwitch)
    {
        int_switch_hook_add((FUNC_VOID)dump_int_switch_hook);
    }
    if(1 == g_dump_cfg.dump_cfg.Bits.ARMexc)
    {
        exc_hook_add((FUNC_VOID) dump_exc_hook);
    }

    if(1 == g_dump_cfg.dump_cfg.Bits.intLock)
    {
        int_lock_hook_add((func_void)bsp_dump_int_lock,(func_void)bsp_dump_int_unlock);
    }

    /*缺看门狗中断*/
    ret = bsp_wdt_register_hook(dump_wdt_hook);
    if(OK != ret)
    {
        dump_error("dump_register_hook[%d]: reg wdt hook failed!\n", __LINE__);
    }

    dump_stack_add_hook((traps_hook)dump_stack_hook);

    /* 注册开关机处理 */
    bsp_reboot_callback_register(dump_onoff_callback);

    g_dump_global_info->internal_info.app_internal.taskSwitchStart = BSP_TRUE;
    g_dump_global_info->internal_info.app_internal.IntSwitchStart = BSP_TRUE;

    return;
}


bool dump_need_save(void)
{
    /* 检查PMU异常 */
    if(PMU_STATE_OK != bsp_pmu_get_boot_state())
    {
        g_dump_global_info->reboot_reason = DUMP_REASON_PMU;
        return BSP_TRUE;
    }
    
    /* 先检查ACORE */
    if((DUMP_START_EXCH == g_dump_global_info->internal_info.app_internal.start_flag)
        ||(DUMP_START_EXCH == g_dump_global_info->internal_info.comm_internal.start_flag)
        ||(DUMP_START_EXCH == g_dump_global_info->internal_info.m3_internal.start_flag))
    {
        // 异常重启
        if(DUMP_SAVE_FILE_NEED == g_dump_global_info->internal_info.app_internal.save_file_flag)
        {
            return BSP_TRUE;
        }
        else
        {
            return BSP_FALSE;
        }
    }
    else if((DUMP_START_REBOOT == g_dump_global_info->internal_info.app_internal.start_flag)
        || (DUMP_START_REBOOT == g_dump_global_info->internal_info.comm_internal.start_flag))
    {
        return BSP_FALSE;
    }
    else if((DUMP_START_CRASH == g_dump_global_info->internal_info.app_internal.start_flag)
        || (DUMP_START_CRASH == g_dump_global_info->internal_info.comm_internal.start_flag))
    {
        g_dump_global_info->reboot_reason = DUMP_REASON_UNDEF;
        return BSP_TRUE;
    }
    else
    {
        return BSP_FALSE;
    }
}

__inline__ void dump_save_global_info(void)
{
    //如果是上层复位，标志复位状态
    if(DUMP_REASON_UNDEF == g_dump_global_info->reboot_reason)
    {
        g_dump_global_info->reboot_reason = DUMP_REASON_NORMAL;
    }

    if(DUMP_REASON_WDT != g_dump_global_info->reboot_reason)
    {
        g_dump_global_info->reboot_cpu= DUMP_CPU_APP;
    }

    g_dump_global_info->reboot_time= om_timer_tick_get();
    g_dump_global_info->internal_info.app_internal.start_flag = DUMP_START_EXCH;

    return;
}

void dump_save_base_info(u32 mod_id, u32 arg1, u32 arg2, char *data, u32 length)
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
            //if(!DUMP_T_TASK_ERROR(mod_id))
            if(mod_id == 0x11000025 || mod_id == 0x1100002A)
            {
                /* A核VOS只记录的任务的pid，没有任务的task_struct指针，不记录任务名 */
                g_dump_base_info->reboot_task = arg1;
            }
            else
            {

                g_dump_base_info->reboot_task = g_dump_base_info->current_task;
                if(NULL != (void*)g_dump_base_info->reboot_task)
                {
                    memcpy(g_dump_base_info->taskName, ((struct task_struct *)(g_dump_base_info->reboot_task))->comm, 16);
                }                
            }

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
    if (0 == dump_in_interrrupt())
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

void dump_save_log(void)
{
    u32 log_end_index = 0;

    /* 如果不是异常向量，再把任务异常栈打印一次 */
    if (!(DUMP_REASON_ARM == g_dump_base_info->arg1))
    {
        dump_error("###########show mem and current task stack start##############!\n");
        show_mem(0);
         if(DUMP_T_TASK_ERROR(g_dump_base_info->modId))
        {
            show_stack(find_task_by_vpid(g_dump_base_info->arg1),NULL);
        }
        else
        {
            show_stack(NULL,NULL);
        }
        dump_error("###########show mem and current task stack end################!\n");
    }

    /* 下面开始保存打印数据 */
    //log_end_index = ((log_end)&(DUMP_PRINT_SIZE-1));
    log_end_index = (u32)(log_end&(log_buf_len-1));

    if (log_buf_len <= DUMP_PRINT_SIZE)
    {
        u32 ulOffsetSize = 0x00;

        ulOffsetSize = log_buf_len - log_end_index;
        memcpy((void * )(DUMP_PRINT_ADDR), (const void * )(log_buf+log_end_index), ulOffsetSize); /* [false alarm]:屏蔽Fortify错误 */
        memcpy((void * )(DUMP_PRINT_ADDR+ulOffsetSize), (const void * )(log_buf), log_end_index); /* [false alarm]:屏蔽Fortify错误 */
    }
    else if ( log_end_index >= DUMP_PRINT_SIZE)
    {
         memcpy((void * )(DUMP_PRINT_ADDR), (const void * )(log_buf + log_end_index - DUMP_PRINT_SIZE), DUMP_PRINT_SIZE); /* [false alarm]:屏蔽Fortify错误 */
    }
    else
    {
        u32 ulOffsetSize = 0x00;

        ulOffsetSize = DUMP_PRINT_SIZE - log_end_index;
        memcpy((void * )(DUMP_PRINT_ADDR), (const void * )(log_buf+log_buf_len-ulOffsetSize), ulOffsetSize); /* [false alarm]:屏蔽Fortify错误 */
        memcpy((void * )(DUMP_PRINT_ADDR+ulOffsetSize), (const void * )(log_buf), log_end_index); /* [false alarm]:屏蔽Fortify错误 */
    }

    return;
}

__inline__ void dump_save_arm_regs(u32 addr)
{    
    asm volatile(
        "str r0, [r0,#0x00]\n"
        "str r1, [r0,#0x04]\n"
        "str r2, [r0,#0x08]\n"
        "str r3, [r0,#0x0C]\n"
        "str r4, [r0,#0x10]\n"
        "str r5, [r0,#0x14]\n"
        "str r6, [r0,#0x18]\n"
        "str r7, [r0,#0x1C]\n"
        "str r8, [r0,#0x20]\n"
        "str r9, [r0,#0x24]\n"
        "str r10, [r0,#0x28]\n"
        "str r11, [r0,#0x2C]\n"
        "str r12, [r0,#0x30]\n"
        "str r13, [r0,#0x34]\n"
        "str r14, [r0,#0x38]\n"
        "push {r1}\n"
        "mov r1, pc\n"
        "str r1, [r0,#0x3C]\n"
        "mrs r1, cpsr\n"
        "str r1, [r0,#0x40]\n"
        "pop {r1}\n"
    );
}

void dump_save_exc_task(u32 addr)
{
    //struct task_struct *task = (struct task_struct *)g_dump_base_info->current_task;
    struct task_struct *task = get_current();

    dump_save_arm_regs(addr);

    g_dump_base_info->current_task = (u32)task;

    /* 目前只保存stack, 保存全部8K数据 */
    memcpy((void * )DUMP_TASK_STACK_ADDR , (const void * )task->stack, (size_t )THREAD_SIZE);/* [false alarm]:屏蔽Fortify错误 */

    return;
}

void dump_save_all_task(void)
{
    struct task_struct *pTid;
    struct thread_info *pThread;
    dump_task_info_t *task_info;
    int ulTaskNum = 0x00;

    for_each_process(pTid)
    {
        if(ulTaskNum >=  DUMP_LINUX_TASK_NUM_MAX)
        {
            return;
        }

        if(pTid == NULL)
        {
            continue;
        }

        pThread = (struct thread_info*)pTid->stack;
        // 首先保存任务信息
        task_info = &g_dump_task_info[ulTaskNum];
        task_info->pid = PID_PPID_GET(pTid);
        task_info->entry = (u32)BSP_NULL;                        // linux暂不支持
        task_info->status = pTid->state;
        task_info->policy = pTid->policy;
        task_info->priority = pTid->prio;
        task_info->stack_base = (u32)((u32)pTid->stack + THREAD_SIZE);
        task_info->stack_end = (u32)end_of_stack(pTid);
        task_info->stack_high = 0;                          // linux暂不支持
        /* coverity[buffer_size_warning] */
        strncpy((char *)task_info->name, pTid->comm, 16);
        task_info->regs[0] = 0;                             // 前四个通用寄存器无意义
        task_info->regs[1] = 0;
        task_info->regs[2] = 0;
        task_info->regs[3] = 0;
        memcpy(&task_info->regs[4], &pThread->cpu_context, 12*sizeof(u32));

        task_info->offset = 0;
        g_dump_ctrl.current_task_info = (u32)task_info;
        dump_stack_bl(pTid);

        ulTaskNum++;
    }

    return;
}

void dump_save_regs(void)
{
    int size;

    if(g_dump_cfg.dump_cfg.Bits.appRegSave1)
    {
        size = (g_dump_cfg.appRegSize1 < DUMP_REG_SET_MAX) ? g_dump_cfg.appRegSize1 : DUMP_REG_SET_MAX;
        memcpy_fromio((char *)DUMP_REGS_ADDR + sizeof(dump_regs_map_t), (void*)g_dump_cfg.appRegAddr1, size);
        g_dump_reg_map->reg_addr1 = g_dump_cfg.appRegAddr1;
        g_dump_reg_map->reg_size1 = size;
    }
    if(g_dump_cfg.dump_cfg.Bits.appRegSave2)
    {
        size = (g_dump_cfg.appRegSize2 < DUMP_REG_SET_MAX) ? g_dump_cfg.appRegSize2 : DUMP_REG_SET_MAX;
        memcpy_fromio((char *)DUMP_REGS_ADDR+ DUMP_REG_SET_MAX + sizeof(dump_regs_map_t), (void*)g_dump_cfg.appRegAddr2, size);
        g_dump_reg_map->reg_addr2 = g_dump_cfg.appRegAddr2;
        g_dump_reg_map->reg_size2 = size;
    }
    if(g_dump_cfg.dump_cfg.Bits.appRegSave3)
    {
        size = (g_dump_cfg.appRegSize3 < (DUMP_REG_SET_MAX-sizeof(dump_regs_map_t)))
            ? g_dump_cfg.appRegSize3
            : (DUMP_REG_SET_MAX-sizeof(dump_regs_map_t));
        memcpy_fromio((char *)DUMP_REGS_ADDR+ DUMP_REG_SET_MAX*2 + sizeof(dump_regs_map_t), (void*)g_dump_cfg.appRegAddr3, size);
        g_dump_reg_map->reg_addr3 = g_dump_cfg.appRegAddr3;
        g_dump_reg_map->reg_size3 = size;
    }

    return;
}


void dump_save_usr_data(char *data, u32 length)
{
    s32 len;

    /*保存用户信息*/
    if ((NULL != data) && (length))
    {
        len = (length > DUMP_USER_DATA_SIZE) ? DUMP_USER_DATA_SIZE : length;
        memcpy((void *)DUMP_USER_DATA_ADDR, (const void * )data, (size_t)len); /* [false alarm]:屏蔽Fortify错误 */
        g_dump_core_map->sec_user_data.length = len;
    }

    return;
}


#define dump_save_app_done()\
    do{\
        g_dump_global_info->internal_info.app_internal.save_flag = DUMP_SAVE_SUCCESS;\
    }while(0)

#define dump_save_ext_done()\
    do{\
        g_dump_global_info->internal_info.app_internal.ext_save_flag = DUMP_SAVE_SUCCESS;\
    }while(0)

#define dump_save_done()\
    do{\
        g_dump_global_info->internal_info.app_internal.save_file_flag = DUMP_SAVE_FILE_END;\
    }while(0)


void dump_save_ext(void)
{
    s32 i;

    for(i=0; i<DUMP_SAVE_MOD_BUTT; i++)
    {
        if(BSP_NULL != g_dump_hook[i])
        {
            g_dump_hook[i]();
        }
    }

    dump_save_ext_done();

    return;
}

void save_last_profile(void)
{
    writel(*(u32*)SRAM_MAX_CPUFREQ_PROFILE_ADDR, DUMP_FASTBOOT_ADDR);
    writel(*(u32*)SRAM_MAX_CPUFREQ_PROFILE_ADDR, DUMP_FASTBOOT_ADDR + 4);
    writel(*(u32*)SRAM_MIN_CPUFREQ_PROFILE_ADDR, DUMP_FASTBOOT_ADDR + 8);
}

s32 dump_save(void)
{
    dump_debug("dump_save[%d]: enter!\n", __LINE__);

    bsp_om_save_loop_file(OM_ROOT_PATH, OM_DUMP_HEAD, (void*)DUMP_MEM_BASE, DUMP_MEM_USED_SIZE);

	//dump_global_info_clear();

    dump_save_done();

    return BSP_OK;
}

void dump_trace_stop(void)
{
    int_switch_hook_delete();
    g_dump_global_info->internal_info.app_internal.taskSwitchStart = BSP_FALSE;
    g_dump_global_info->internal_info.app_internal.IntSwitchStart = BSP_FALSE;

    return;
}

__inline__ void dump_wait_for_reboot(void)
{
    /* coverity[no_escape] */
    do{
    }while(1);
}


void dump_save_and_reboot(void)
{
    /*保存文件在高优先级任务中完成*/
    g_dump_ctrl.dump_task_job = DUMP_TASK_JOB_SAVE_REBOOT;
    up(&g_dump_ctrl.sem_dump_task);

//    preempt_enable();

    return;
}


void dump_system_error_enter(void)
{
    /* 停止记录任务切换/中断等 */
    dump_trace_stop();

    /* 禁止抢占 */
//    preempt_disable();

    return;
}

void dump_notify_target(void)
{
    bsp_ipc_int_send(IPC_CORE_CCORE, IPC_CCPU_SRC_ACPU_DUMP);
}

void dump_save_task_name(void)
{
    struct task_struct *pTid;
    int ulTaskNum = 0x00;
    u32 task_name_addr = DUMP_ALLTASK_ADDR;
    u32 pid_ppid = 0;
    char idle_task_name[12] = {"swapper"};

    /* 任务切换正在记录，直接返回 */
    if(g_dump_save_task_name == true)
    {
        return;
    }

    /* 开始记录 */
    g_dump_save_task_name = true;
    
    for_each_process(pTid)
    {
        if(ulTaskNum >=  DUMP_LINUX_TASK_NUM_MAX)
        {
            break;
        }
        
        if(pTid == NULL)
        {
            continue;
        }

        pid_ppid = PID_PPID_GET(pTid);
   
        QueueLoopIn((dump_queue_t *)task_name_addr, pid_ppid);
        QueueLoopIn((dump_queue_t *)task_name_addr, *((int *)(pTid->comm)));
        QueueLoopIn((dump_queue_t *)task_name_addr, *((int *)((pTid->comm)+4)));
        QueueLoopIn((dump_queue_t *)task_name_addr, *((int *)((pTid->comm)+8)));

        ulTaskNum++;
    }

    QueueLoopIn((dump_queue_t *)task_name_addr, 0);
    QueueLoopIn((dump_queue_t *)task_name_addr, *((int *)(idle_task_name)));
    QueueLoopIn((dump_queue_t *)task_name_addr, *((int *)(idle_task_name+4)));
    QueueLoopIn((dump_queue_t *)task_name_addr, *((int *)(idle_task_name+8))); 

    /* 记录完成 */
    g_dump_save_task_name = false;
}

void dump_timer_handler(u32 param)
{
    if(param)
    {}
    dump_save_task_name();
    bsp_softtimer_add(&g_dump_timer);
}

s32 dump_start_timer(u32 time_out)
{
    s32 ret = 0;
    g_dump_timer.func = dump_timer_handler;
    g_dump_timer.para = 0;
    g_dump_timer.timeout = time_out;
    g_dump_timer.wake_type = SOFTTIMER_NOWAKE;

    ret =  bsp_softtimer_create(&g_dump_timer);
    if(ret)
    {
        return BSP_ERROR;
    }
    bsp_softtimer_add(&g_dump_timer);

    return BSP_OK;
}

void dump_stop_timer(void)
{
    if(g_dump_timer.init_flags!=TIMER_INIT_FLAG)
    {
        dump_fetal("timer is null\n");
        return;
    }

    (void)bsp_softtimer_delete_sync(&g_dump_timer);
    (void)bsp_softtimer_free(&g_dump_timer);
}

void dump_int_handle(s32 param)
{
    /* 已经发生异常，不再记录 */
    if(g_exc_flag)
    {
        return;
    }
    else
    {
        g_exc_flag = true;
    }

    /*保存异常任务信息*/
    dump_save_exc_task((u32)(&(g_dump_base_info->regSet[0])));

    bsp_utrace_stop();

    dump_fetal("[0x%x]================ acore enter system error agent! ================\n", om_timer_get());

	if (DUMP_INIT_FLAG != g_dump_global_info->internal_info.app_internal.init_flag)
    {
        /*等待重启*/
        dump_wait_for_reboot();

        return;
    }

    if(param == 0)
    {
         /* C核异常了，发送中断通知M3*/
         bsp_ipc_int_send(IPC_CORE_MCORE, IPC_MCU_INT_SRC_DUMP);
    }
    else if(param == 1)
    {
         /* M3异常了，发送中断通知C核*/
        bsp_ipc_int_send(IPC_CORE_CCORE, IPC_CCPU_SRC_ACPU_DUMP);
    }

    dump_system_error_enter();

    /* 首先保存系统异常基本信息 */
    dump_save_base_info(BSP_MODU_OTHER_CORE, 0, 0, BSP_NULL, 0);

    /*保存打印信息*/
    dump_save_log();

    /*保存所有任务信息*/
    dump_save_all_task();

    /*保存寄存器信息*/
    dump_save_regs();

    /* 保存扩展区数据 */
    dump_save_ext();

    /*APP数据保存完成 */
    dump_save_app_done();

    /* 停止定时器，刷新任务名 */
    dump_stop_timer();
    dump_save_task_name();

    /*保存异常文件并重启*/
    dump_save_and_reboot();

    return;
}

s32 dump_config_init(void)
{
    s32 ret;

    ret = bsp_nvm_read(NVID_DUMP, (u8*)&g_dump_cfg, sizeof(dump_nv_s));
    //ret = -1;
    if (BSP_OK != ret)
    {
        /*使用默认值*/
        g_dump_cfg.dump_cfg.Bits.dump_switch     = 0x1;
        g_dump_cfg.dump_cfg.Bits.ARMexc          = 0x1;
        g_dump_cfg.dump_cfg.Bits.stackFlow       = 0x1;
        g_dump_cfg.dump_cfg.Bits.taskSwitch      = 0x1;
        g_dump_cfg.dump_cfg.Bits.intSwitch       = 0x1;
        g_dump_cfg.dump_cfg.Bits.intLock         = 0x1;
        g_dump_cfg.dump_cfg.Bits.appRegSave1     = 0x0;
        g_dump_cfg.dump_cfg.Bits.appRegSave2     = 0x0;
        g_dump_cfg.dump_cfg.Bits.appRegSave3     = 0x0;
        g_dump_cfg.dump_cfg.Bits.commRegSave1    = 0x0;
        g_dump_cfg.dump_cfg.Bits.commRegSave2    = 0x0;
        g_dump_cfg.dump_cfg.Bits.commRegSave3    = 0x0;
        g_dump_cfg.dump_cfg.Bits.sysErrReboot    = 0x1;
        g_dump_cfg.dump_cfg.Bits.reset_log       = 0x1;
        g_dump_cfg.dump_cfg.Bits.fetal_err       = 0x1;
        dump_error("read NV failed, use the default value!.ret = %d nv id = 0x%x\n", ret, NVID_DUMP);
    }

    return BSP_OK;
}

void dump_map_init(void)
{
    int i;
    dump_map_section_desc_t *sec_desc;
    dump_load_info_t * dump_load;

    dump_load = (dump_load_info_t *)DUMP_LOAD_INFO_ADDR;
    dump_load->magic_num = DUMP_LOAD_MAGIC;
    dump_load->app_load_addr = 0xC0000000;
    dump_load->share_load_addr_a= DDR_SHARED_MEM_VIRT_ADDR;
    dump_load->mntn_load_addr_a = DDR_MNTN_ADDR_VIRT;    
    dump_load->modem_load_addr = DDR_MCORE_ADDR;
    dump_load->share_load_addr_m= DDR_SHARED_MEM_ADDR;
    dump_load->mntn_load_addr_m = DDR_MNTN_ADDR;  

    g_dump_global_map = (dump_global_map_t*)DUMP_GLOBAL_MAP_ADDR;
    g_dump_global_map->sec_app.offset = DUMP_APP_SECTION_ADDR - DUMP_MEM_BASE;
    g_dump_global_map->sec_app.length = DUMP_APP_SECTION_SIZE;
    g_dump_global_map->sec_comm.offset = DUMP_COMM_SECTION_ADDR - DUMP_MEM_BASE;
    g_dump_global_map->sec_comm.length = DUMP_COMM_SECTION_SIZE;
    g_dump_global_map->sec_m3.offset = DUMP_M3_SECTION_ADDR - DUMP_MEM_BASE;
    g_dump_global_map->sec_m3.length = DUMP_M3_SECTION_SIZE;
    g_dump_global_map->sec_ext.offset = DUMP_EXT_SECTION_ADDR - DUMP_MEM_BASE;
    g_dump_global_map->sec_ext.length = DUMP_EXT_SECTION_SIZE;
    g_dump_global_map->sec_rsv.offset = (u32)DUMP_SECTION_INVALID;
    g_dump_global_map->sec_rsv.length = 0;

    g_dump_core_map = (dump_core_map_t*)DUMP_MAP_ADDR;
    g_dump_core_map->sec_base.offset = DUMP_BASE_INFO_ADDR - DUMP_MEM_BASE;
    g_dump_core_map->sec_base.length = DUMP_BASE_INFO_SIZE;
    g_dump_core_map->sec_task_switch.offset = DUMP_TASK_SWITCH_ADDR- DUMP_MEM_BASE;
    g_dump_core_map->sec_task_switch.length = DUMP_TASK_SWITCH_SIZE;
    g_dump_core_map->sec_intlock.offset = DUMP_INTLOCK_ADDR- DUMP_MEM_BASE;
    g_dump_core_map->sec_intlock.length = DUMP_INTLOCK_SIZE;
    g_dump_core_map->sec_task_stack.offset = DUMP_TASK_STACK_ADDR - DUMP_MEM_BASE;
    g_dump_core_map->sec_task_stack.length = DUMP_TASK_STACK_SIZE;
    g_dump_core_map->sec_int_stack.offset = DUMP_INT_STACK_ADDR- DUMP_MEM_BASE;
    g_dump_core_map->sec_int_stack.length = DUMP_INT_STACK_SIZE;
    g_dump_core_map->sec_all_task.offset = DUMP_ALLTASK_ADDR- DUMP_MEM_BASE;
    g_dump_core_map->sec_all_task.length = DUMP_ALLTASK_SIZE;
    g_dump_core_map->sec_all_task_tcb.offset = DUMP_ALLTASK_TCB_ADDR- DUMP_MEM_BASE;
    g_dump_core_map->sec_all_task_tcb.length = DUMP_ALLTASK_TCB_SIZE;
    g_dump_core_map->sec_print.offset = DUMP_PRINT_ADDR- DUMP_MEM_BASE;
    g_dump_core_map->sec_print.length = DUMP_PRINT_SIZE;
    g_dump_core_map->sec_reg.offset = DUMP_REGS_ADDR- DUMP_MEM_BASE;
    g_dump_core_map->sec_reg.length = DUMP_REGS_SIZE;
    g_dump_core_map->sec_cpu_view.offset = DUMP_CPUVIEW_ADDR- DUMP_MEM_BASE;
    g_dump_core_map->sec_cpu_view.length = DUMP_CPUVIEW_SIZE;
    g_dump_core_map->sec_mem_info.offset = DUMP_MEMINFO_ADDR- DUMP_MEM_BASE;
    g_dump_core_map->sec_mem_info.length = DUMP_MEMINFO_SIZE;
    g_dump_core_map->sec_user_data.offset = DUMP_USER_DATA_ADDR- DUMP_MEM_BASE;
    g_dump_core_map->sec_user_data.length = 0;
    g_dump_core_map->sec_rsv.offset = (u32)DUMP_SECTION_INVALID;
    g_dump_core_map->sec_rsv.length = 0;

    g_dump_ext_map = (dump_ext_map_t*)DUMP_EXT_MAP_ADDR;
    g_dump_ext_map->reserved1.offset = DUMP_EXT_USB_DATA_ADDR- DUMP_MEM_BASE;
    g_dump_ext_map->reserved1.length = DUMP_EXT_USB_DATA_SIZE;
    strncpy(g_dump_ext_map->reserved1.name, "USB", 8);
    g_dump_ext_map->reserved2.offset = DUMP_EXT_SCI0_DATA_ADDR- DUMP_MEM_BASE;
    g_dump_ext_map->reserved2.length = DUMP_EXT_SCI0_DATA_SIZE;
    strncpy(g_dump_ext_map->reserved2.name, "SCI0", 8);
    g_dump_ext_map->reserved3.offset = DUMP_EXT_SCI1_DATA_ADDR- DUMP_MEM_BASE;
    g_dump_ext_map->reserved3.length = DUMP_EXT_SCI1_DATA_SIZE;
    strncpy(g_dump_ext_map->reserved3.name, "SCI1", 8);
    g_dump_ext_map->reserved4.offset = DUMP_EXT_OM_MSG_ADDR- DUMP_MEM_BASE;
    g_dump_ext_map->reserved4.length = DUMP_EXT_OM_MSG_SIZE;
    strncpy(g_dump_ext_map->reserved4.name, "OM_MSG", 8);
    g_dump_ext_map->reserved5.offset = DUMP_EXT_OM_LOG_ADDR- DUMP_MEM_BASE;
    g_dump_ext_map->reserved5.length = DUMP_EXT_OM_LOG_SIZE;
    strncpy(g_dump_ext_map->reserved5.name, "OM_LOG", 8);
    g_dump_ext_map->reserved6.offset = DUMP_EXT_OM_UTRACE_DATA_ADDR- DUMP_MEM_BASE;
    g_dump_ext_map->reserved6.length = DUMP_EXT_OM_UTRACE_DATA_SIZE;
    strncpy(g_dump_ext_map->reserved6.name, "OM_CS", 8);
    g_dump_ext_map->reserved7.offset = DUMP_EXT_OM_DRX_ACORE_ADDR- DUMP_MEM_BASE;
    g_dump_ext_map->reserved7.length = DUMP_EXT_OM_DRX_ACORE_SIZE;
    strncpy(g_dump_ext_map->reserved7.name, "DRX_A", 8);
    g_dump_ext_map->reserved8.offset = DUMP_EXT_OM_DRX_CCORE_ADDR- DUMP_MEM_BASE;
    g_dump_ext_map->reserved8.length = DUMP_EXT_OM_DRX_CCORE_SIZE;
    strncpy(g_dump_ext_map->reserved8.name, "DRX_C", 8);
    g_dump_ext_map->reserved9.offset = DUMP_EXT_LOW_POWER_ACORE_ADDR - DUMP_MEM_BASE;
    g_dump_ext_map->reserved9.length = DUMP_EXT_LOW_POWER_ACORE_SIZE + DUMP_EXT_LOW_POWER_CCORE_SIZE;
    /* coverity[buffer_size_warning] */
    strncpy(g_dump_ext_map->reserved9.name, "LP_MNTN", 8);
    g_dump_ext_map->reserved10.offset = DUMP_EXT_LOW_POWER_ACORE_BUSERROR - DUMP_MEM_BASE;
    g_dump_ext_map->reserved10.length = DUMP_EXT_LOW_POWER_ACORE_BUSERROR_SIZE + DUMP_EXT_LOW_POWER_CCORE_BUSERROR_SIZE + DUMP_EXT_LOW_POWER_MCORE_BUSERROR_SIZE;
    /* coverity[buffer_size_warning] */
    strncpy(g_dump_ext_map->reserved10.name, "BUS_ERR", 8);
    /* icc */
    g_dump_ext_map->reserved11.offset = DUMP_EXT_ICC_ACORE_ADDR - DUMP_MEM_BASE;
    g_dump_ext_map->reserved11.length = DUMP_EXT_ICC_ACORE_SIZE + DUMP_EXT_ICC_CCORE_SIZE + DUMP_EXT_ICC_MCORE_SIZE;
    strncpy(g_dump_ext_map->reserved11.name, "ICC", 8);
    g_dump_ext_map->reserved12.offset = DUMP_EXT_BOOST_MCORE_ADDR - DUMP_MEM_BASE;
    g_dump_ext_map->reserved12.length = DUMP_EXT_BOOST_MCORE_SIZE;
    /* coverity[buffer_size_warning] */
    strncpy(g_dump_ext_map->reserved12.name, "HPM_TMP", 8);
    g_dump_ext_map->reserved13.offset = DUMP_TASK_TCB_ADDR - DUMP_MEM_BASE;
    g_dump_ext_map->reserved13.length = DUMP_TASK_TCB_SIZE;
    strncpy(g_dump_ext_map->reserved13.name, "TCB_PC", 8);
    g_dump_ext_map->reserved14.offset = DUMP_EXT_MSP_SLEEP_ADDR - DUMP_MEM_BASE;
    g_dump_ext_map->reserved14.length = DUMP_EXT_MSP_SLEEP_SIZE;
    /* coverity[buffer_size_warning] */
    strncpy(g_dump_ext_map->reserved14.name, "MSP_DRX", 8);
    g_dump_ext_map->reserved15.offset = DUMP_EXT_AMON_ADDR - DUMP_MEM_BASE;
    g_dump_ext_map->reserved15.length = DUMP_EXT_AMON_SIZE;
    strncpy(g_dump_ext_map->reserved15.name, "AMON", 8);
    sec_desc = &g_dump_ext_map->reserved16;
    for(i=0; i<1; i++)
    {
        sec_desc->offset = DUMP_EXT_MEM_END;
        sec_desc->length = 0;
        sec_desc->name[0] = 0;
//        strcpy(sec_desc->name, "reserve");
        sec_desc++;
    }

    g_dump_base_info = (dump_base_info_t*)DUMP_BASE_INFO_ADDR;
    g_dump_task_info = (dump_task_info_t*)DUMP_ALLTASK_TCB_ADDR;
    g_dump_comm_base_info = (dump_base_info_t*)(DUMP_COMM_SECTION_ADDR + DUMP_MAP_SIZE);

    g_dump_reg_map   = (dump_regs_map_t*)DUMP_REGS_ADDR;
    g_dump_reg_map->reg_addr1 = 0;
    g_dump_reg_map->reg_size1 = 0;
    g_dump_reg_map->reg_addr2 = 0;
    g_dump_reg_map->reg_size2 = 0;
    g_dump_reg_map->reg_addr3 = 0;
    g_dump_reg_map->reg_size3 = 0;

    return;
}

s32 dump_global_info_init(void)
{
    char * str_tmp;
    
    g_dump_global_info->version = 0x100;        // 1.00

    strncpy((char *)g_dump_global_info->product, "Balong V700R200", 16);

    /* 软件版本号 */
    str_tmp = bsp_version_get_firmware();
    if(str_tmp != NULL)
    {
        /* coverity[buffer_size_warning] */
        strncpy((char *)g_dump_global_info->product_version, str_tmp, 32);
    }
    
    strncpy((char *)g_dump_global_info->build_date, __DATE__, 16);
    strncpy((char *)g_dump_global_info->build_time, __TIME__, 16);

    /* 读取change-id, 只拷贝前9位 */
    memset((void *)g_dump_global_info->build_sha, 0, 16);

    g_dump_global_info->reboot_cpu = 0;
    g_dump_global_info->reboot_reason = 0xff;
    g_dump_global_info->reboot_time = 0;

    g_dump_global_info->internal_info.app_internal.start_flag = DUMP_START_CRASH;
    g_dump_global_info->internal_info.app_internal.save_flag = BSP_FALSE;
    g_dump_global_info->internal_info.app_internal.ext_save_flag = BSP_FALSE;
    g_dump_global_info->internal_info.app_internal.inInterrupt = 0;
    g_dump_global_info->internal_info.app_internal.taskSwitchStart = BSP_FALSE;
    g_dump_global_info->internal_info.app_internal.IntSwitchStart = BSP_FALSE;

    return BSP_OK;
}


s32 dump_global_info_clear(void)
{
#if 0
    dump_global_info_init();

    g_dump_global_info->internal_info.comm_internal.start_flag = DUMP_START_CRASH;
    g_dump_global_info->internal_info.comm_internal.save_flag = BSP_FALSE;
    g_dump_global_info->internal_info.comm_internal.ext_save_flag = BSP_FALSE;
    g_dump_global_info->internal_info.comm_internal.inInterrupt = 0;
    g_dump_global_info->internal_info.comm_internal.taskSwitchStart = BSP_FALSE;
    g_dump_global_info->internal_info.comm_internal.IntSwitchStart = BSP_FALSE;
#endif
    g_dump_global_info->internal_info.app_internal.start_flag = DUMP_START_REBOOT;
    g_dump_global_info->internal_info.comm_internal.start_flag = DUMP_START_REBOOT;

    return BSP_OK;
}

s32 dump_base_info_init(void)
{
    int i;

    memset(g_dump_base_info, 0, sizeof(dump_base_info_t));
    g_dump_base_info->vec = DUMP_ARM_VEC_UNKNOW;

    memset(g_dump_reg_map, 0, sizeof(dump_regs_map_t));

    memset((void*)DUMP_ALLTASK_TCB_ADDR, 0, DUMP_ALLTASK_TCB_SIZE);
    for(i=0; i<DUMP_LINUX_TASK_NUM_MAX; i++)
    {
        g_dump_task_info[i].pid = 0xffffffff;
    }

    return BSP_OK;
}

void dump_queue_t_init(void)
{
    /* 初始化任务切换队列 */
    QueueInit((dump_queue_t *)(DUMP_TASK_SWITCH_ADDR), (DUMP_TASK_SWITCH_SIZE - 0x10) / 0x4);
    g_dump_ptr_info.task_swt_ptr = DUMP_TASK_SWITCH_ADDR +0x10;

    /* 初始化中断队列 */
    QueueInit((dump_queue_t *)(DUMP_INTLOCK_ADDR), (DUMP_INTLOCK_SIZE - 0x10) / 0x4);
    g_dump_ptr_info.int_lock_ptr = DUMP_INTLOCK_ADDR +0x10;

    /* 初始化任务名队列 */
    QueueInit((dump_queue_t *)(DUMP_ALLTASK_ADDR), (DUMP_ALLTASK_SIZE - 0x10) / 0x4);

    return;
}
/*lint -restore */

/*lint -save -e514 -e527 -e716*/
int dump_save_task(void *data)
{
    u32 time_start, time_cur = 0;

#define DUMP_SAVE_WAIT_MAX   500   //200tick
#define DUMP_INIT_WAIT_MAX   60    //60s
    /* coverity[no_escape] */
    while(1)
    {
        down(&g_dump_ctrl.sem_dump_task);

        /**************************************** 检查文件系统可用 ***********************************************/
        do{
            if(bsp_om_fs_check())
                break;

            msleep(1000);
            time_cur++;
        }while(time_cur < DUMP_INIT_WAIT_MAX);

        /**************************************** 保存reset log   ***********************************************/
        if(g_dump_ctrl.dump_task_job & DUMP_TASK_JOB_RESET_LOG)
        {
            bsp_om_save_resetlog();
        }

        /**************************************** 异常启动，保存dump ********************************************/
        if(DUMP_TASK_JOB_SAVE_INIT == (g_dump_ctrl.dump_task_job & DUMP_TASK_JOB_SAVE_INIT))
        {
            // 保存异常文件
            dump_save();

            dump_init_phase2();
        }

        /**************************************** 系统异常，保存dump ********************************************/
        if(DUMP_TASK_JOB_SAVE_REBOOT == (g_dump_ctrl.dump_task_job & DUMP_TASK_JOB_SAVE_REBOOT))
        {
           //dump_save_ext();

           time_start = om_timer_tick_get();
            do{
                if((DUMP_SAVE_SUCCESS == g_dump_global_info->internal_info.comm_internal.save_flag)
                    &(DUMP_SAVE_SUCCESS == g_dump_global_info->internal_info.m3_internal.save_flag))
                {
                    break;
                }

                time_cur = om_timer_tick_get();
                if(DUMP_SAVE_WAIT_MAX < time_cur - time_start)
                {
                    break;
                }

                msleep(10);
            }while(1);

		     save_last_profile();
		     
            // 保存异常文件
            dump_save();

            /* 配置DDR进入自刷新 */

            /* 重启系统 */
            if(1 == g_dump_cfg.dump_cfg.Bits.sysErrReboot)
            {
                bsp_drv_power_reboot_direct();
            }
        }

        g_dump_ctrl.dump_task_job = 0;
    }

    return BSP_OK;
}
/*lint -restore*/

s32 dump_init_icc_send(u32 flag)
{

    u32 channel_id = ICC_CHN_MCORE_ACORE << 16 | MCORE_ACORE_FUNC_DUMP;
    s32 ret = 0;
    u32 init_flag  = flag;

    ret = bsp_icc_send(ICC_CPU_MCU, channel_id, (u8 *)&init_flag, sizeof(u32));

    if(ret != (int)sizeof(u32))
    {
        return BSP_ERROR;
    }

    return BSP_OK;
}


__inline__ void dump_init_done(void)
{
    g_dump_global_info->internal_info.app_internal.init_flag = DUMP_INIT_FLAG;
    return;
}

s32 dump_init_phase1(void)
{
    s32 ret;
    u32 pid;
    struct sched_param  param;

    g_dump_global_info = (dump_global_info_t*)DUMP_MEM_BASE;

    /*获取配置*/
    ret = dump_config_init();
    if(BSP_OK != ret)
    {
        // 读取配置失败，异常处理
        dump_error("dump_init_phase1[%d]: dump_config_init failed! ret = %d\n", __LINE__, ret);
    }

    /* 注册中断 */
    ret = bsp_ipc_int_connect(IPC_ACPU_SRC_CCPU_DUMP, (voidfuncptr)dump_int_handle, 0);
    if(OK != ret)
    {
        dump_error("dump_init_phase1[%d]: bsp_ipc_int_connect failed! ret = %d\n", __LINE__, ret);
        return BSP_ERR_DUMP_INIT_FAILED;
    }

    ret = bsp_ipc_int_enable(IPC_ACPU_SRC_CCPU_DUMP);
    if(OK != ret)
    {
        dump_error("dump_init_phase1[%d]: bsp_ipc_int_enable failed! ret = %d\n", __LINE__, ret);
        return BSP_ERR_DUMP_INIT_FAILED;
    }

    /* 注册中断，MCU ->ACPU*/
    ret = bsp_ipc_int_connect(IPC_ACPU_INT_MCU_SRC_DUMP, (voidfuncptr)dump_int_handle, 1);
    if(OK != ret)
    {
        dump_error("dump_init_phase1[%d]: bsp_ipc_int_connect failed! ret = %d\n", __LINE__, ret);
        return BSP_ERR_DUMP_INIT_FAILED;
    }

    ret = bsp_ipc_int_enable(IPC_ACPU_INT_MCU_SRC_DUMP);
    if(OK != ret)
    {
        dump_error("dump_init_phase1[%d]: bsp_ipc_int_enable failed! ret = %d\n", __LINE__, ret);
        return BSP_ERR_DUMP_INIT_FAILED;
    }

    sema_init(&g_dump_ctrl.sem_dump_task, 0);
    g_dump_ctrl.dump_task_job = 0;

	pid = (u32)kthread_run(dump_save_task, 0, "dump_save");
	if (IS_ERR((void*)pid))
	{
        dump_error("dump_init_phase1[%d]: create kthread task failed! ret=%d\n", __LINE__, pid);
		return BSP_ERR_DUMP_INIT_FAILED;
	}
    g_dump_ctrl.dump_task_id = pid;

    param.sched_priority = 99;
    if (BSP_OK != sched_setscheduler((struct task_struct*)pid, SCHED_FIFO, &param))
    {
        dump_error("dump_init_phase1[%d]: sched_setscheduler failed!\n", __LINE__);
        return BSP_ERR_DUMP_INIT_FAILED;
    }

    g_dump_global_info->internal_info.app_internal.init_flag = DUMP_INIT_FLAG_PHASE1;

    return BSP_OK;
}


s32 dump_init_phase2(void)
{
    g_dump_global_info->internal_info.app_internal.save_file_flag = DUMP_SAVE_FILE_NORMAL;
    g_dump_global_info->internal_info.comm_internal.start_wait_flag = DUMP_INIT_FLAG_WAIT;
    g_dump_global_info->internal_info.m3_internal.start_wait_flag = DUMP_INIT_FLAG_WAIT;

    /* 通知M3_DUMP初始化*/
    dump_init_icc_send(1);

    /*内存布局*/
    dump_map_init();

    /*全局信息初始化*/
    dump_global_info_init();

    /*全局信息初始化*/
    dump_base_info_init();

    /*初始化队列*/
    dump_queue_t_init();

    /*系统运行回调函数注册*/
    dump_register_hook();

    /*系统运行回调函数注册*/
    dump_init_done();

    /* 清空PRINT分区缓存 */
    memset((void *)DUMP_PRINT_ADDR, 0, (u32)DUMP_PRINT_SIZE); /* [false alarm]:屏蔽Fortify错误 */
    
    /* 启动任务名记录定时器 */
    dump_start_timer(60000);

    dump_fetal("dump init success!\n");

    return BSP_OK;
}

s32 __init bsp_dump_init(void)
{
    s32 ret;
    bool need_save;
    u32 * dump_flag = (u32 *)SRAM_DUMP_POWER_OFF_FLAG_ADDR;
    g_exc_flag = false;
    *dump_flag = DUMP_START_CRASH;

    /*初始化部分结构体*/
    ret  = dump_init_phase1();
    if(BSP_OK != ret)
    {
        dump_init_icc_send(0);
        return BSP_ERROR;
    }

    /*检查启动类型，因为当前文件系统不可用，需要做以下动作:
      1. 首先把reset_log记录到内存;
      2. 检查当前是否是异常启动，
         如果否，启动任务记录reset_log到文件系统，同时继续初始化;
         如果是，暂停初始化，启动任务记录reset_log和dump内容到文件，然后继续初始化
      3. 记录的任务中，每1s检查一次文件系统是否可用，直到可用之后开始记录文件。
         检查有超时周期，60s之后，无论如何，开始启动下一阶段
    */
    bsp_om_record_resetlog();
    need_save = dump_need_save();
    if(need_save)
    {
        g_dump_ctrl.dump_task_job = (DUMP_TASK_JOB_RESET_LOG | DUMP_TASK_JOB_SAVE_INIT);
        up(&g_dump_ctrl.sem_dump_task);
    }
    else
    {
        g_dump_ctrl.dump_task_job = DUMP_TASK_JOB_RESET_LOG;
        up(&g_dump_ctrl.sem_dump_task);
        dump_init_phase2();
    }

    return BSP_OK;
}

void system_error(u32 mod_id, u32 arg1, u32 arg2, char *data, u32 length)
{
    /* 已经发生异常，不再记录 */
    if(g_exc_flag)
    {
        return;
    }
    else
    {
        g_exc_flag = true;
    }
    /*保存异常任务信息*/
    dump_save_exc_task((u32)(&(g_dump_base_info->regSet[0])));

    bsp_utrace_stop();

	if (DUMP_INIT_FLAG != g_dump_global_info->internal_info.app_internal.init_flag)
    {
        // 重启
		//wdtReboot();
		return;
	}

    dump_system_error_enter();

    dump_fetal("[0x%x]================ acore enter system error! ================\n", om_timer_get());

    /* 通知comm CPU进行异常处理 */
    dump_notify_target();

    /* A核异常了，发送中断通知M3*/
     bsp_ipc_int_send(IPC_CORE_MCORE, IPC_MCU_INT_SRC_DUMP);

    /* 首先保存系统异常基本信息 */
    dump_save_global_info();
    dump_save_base_info(mod_id, arg1, arg2, data, length);

    /*保存打印信息*/
    dump_save_log();

    /*保存所有任务信息*/
    dump_save_all_task();

    /*保存寄存器信息*/
    dump_save_regs();

    /*保存上层用户数据*/
    dump_save_usr_data(data, length);

    /*APP数据保存完成 */
    dump_save_app_done();

    /* 保存扩展区数据 */
    dump_save_ext();

    /* 停止定时器，刷新任务名 */
    dump_stop_timer();
    dump_save_task_name();
    
    /*设置异常文件记录标志*/
    dump_save_and_reboot();

    return;
}

s32 bsp_dump_register_hook(dump_save_modid_t mod_id, dump_save_hook func)
{
    if(DUMP_SAVE_MOD_BUTT <= mod_id)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_DUMP, "bsp_dump_register_hook[%d]:  mod_id is invalid! mod=%d\n", __LINE__, mod_id);
        return BSP_ERR_DUMP_INVALID_MODULE;
    }

    g_dump_hook[mod_id] = func;

    return BSP_OK;
}

s32 bsp_dump_get_buffer(dump_save_modid_t mod_id, char** buffer, u32* length)
{
    if((!buffer) || (!length))
    {
        dump_error("bsp_dump_get_buffer, invalid param!.buffer=0x%x length=0x%x\n", buffer, length);
	    return BSP_ERR_DUMP_INVALID_PARAM;
    }

    switch(mod_id)
    {
        case DUMP_SAVE_MOD_USB:
            *buffer = (char*)DUMP_EXT_USB_DATA_ADDR;
            *length = (u32)DUMP_EXT_USB_DATA_SIZE;
            break;
        case DUMP_SAVE_MOD_SIM0:
            *buffer = (char*)DUMP_EXT_SCI0_DATA_ADDR;
            *length = (u32)DUMP_EXT_SCI0_DATA_SIZE;
            break;
        case DUMP_SAVE_MOD_SIM1:
            *buffer = (char*)DUMP_EXT_SCI1_DATA_ADDR;
            *length = (u32)DUMP_EXT_SCI1_DATA_SIZE;
            break;
        case DUMP_SAVE_MOD_OM_MSG:
            *buffer = (char*)DUMP_EXT_OM_MSG_ADDR;
            *length = (u32)DUMP_EXT_OM_MSG_SIZE;
            break;
        case DUMP_SAVE_MOD_OM_LOG:
            *buffer = (char*)DUMP_EXT_OM_LOG_ADDR;
            *length = (u32)DUMP_EXT_OM_LOG_SIZE;
            break;
        case DUMP_SAVE_MOD_OSA_APP:
            *buffer = (char*)DUMP_INTER_OSA_APP_ADDR;
            *length = (u32)DUMP_INTER_OSA_APP_SIZE;
            break;
        case DUMP_SAVE_MOD_OSA_COMM:
            *buffer = (char*)DUMP_INTER_OSA_COMM_ADDR;
            *length = (u32)DUMP_INTER_OSA_COMM_SIZE;
            break;
        case DUMP_SAVE_MOD_UTRACE:
            *buffer = (char *)DUMP_EXT_OM_UTRACE_DATA_ADDR;
            *length = (u32)DUMP_EXT_OM_UTRACE_DATA_SIZE;
            break;
        case DUMP_SAVE_MOD_DRX_ACORE:
            *buffer = (char *)DUMP_EXT_OM_DRX_ACORE_ADDR;
            *length = (u32)DUMP_EXT_OM_DRX_ACORE_SIZE;
            break;
        case DUMP_SAVE_MOD_DRX_CCORE:
            *buffer = (char *)DUMP_EXT_OM_DRX_CCORE_ADDR;
            *length = (u32)DUMP_EXT_OM_DRX_CCORE_SIZE;
            break;
        case DUMP_SAVE_MOD_OSA_MEM:
            *buffer = (char *)DUMP_MEMINFO_ADDR;
            *length = (u32)DUMP_MEMINFO_SIZE;
            break;
        case DUMP_SAVE_MOD_LP_ACORE:
            *buffer = (char *)DUMP_EXT_LOW_POWER_ACORE_ADDR;
            *length = (u32)DUMP_EXT_LOW_POWER_ACORE_SIZE;
            break;
        case DUMP_SAVE_MOD_LP_CCORE:
            *buffer = (char *)DUMP_EXT_LOW_POWER_CCORE_ADDR;
            *length = (u32)DUMP_EXT_LOW_POWER_CCORE_SIZE;
            break;		
        case DUMP_SAVE_MOD_LP_ACORE_BUSERROR:
            *buffer = (char *)DUMP_EXT_LOW_POWER_ACORE_BUSERROR;
            *length = (u32)DUMP_EXT_LOW_POWER_ACORE_BUSERROR_SIZE;
            break;			
	    case DUMP_SAVE_MOD_LP_CCORE_BUSERROR:
	         *buffer = (char *)DUMP_EXT_LOW_POWER_CCORE_BUSERROR;
	         *length = (u32)DUMP_EXT_LOW_POWER_CCORE_BUSERROR_SIZE;
	         break;
	    case DUMP_SAVE_MOD_LP_MCORE_BUSERROR:
	         *buffer = (char *)DUMP_EXT_LOW_POWER_MCORE_BUSERROR;
	          *length = (u32)DUMP_EXT_LOW_POWER_MCORE_BUSERROR_SIZE;
            	break;

		/* icc */
		case DUMP_SAVE_MOD_ICC_ACORE:
			*buffer = (char *)DUMP_EXT_ICC_ACORE_ADDR;
			*length = (u32)DUMP_EXT_ICC_ACORE_SIZE;
			break;			
		case DUMP_SAVE_MOD_ICC_CCORE:
			*buffer = (char *)DUMP_EXT_ICC_CCORE_ADDR;
			*length = (u32)DUMP_EXT_ICC_CCORE_ADDR;
			break;
		case DUMP_SAVE_MOD_ICC_MCORE:
			*buffer = (char *)DUMP_EXT_ICC_MCORE_ADDR;
			*length = (u32)DUMP_EXT_ICC_MCORE_ADDR;
			break;
	    case DUMP_SAVE_MOD_BOOST:
	         *buffer = (char *)DUMP_EXT_BOOST_MCORE_ADDR;
	         *length = (u32)DUMP_EXT_BOOST_MCORE_SIZE;
	         break;
        case DUMP_SAVE_MOD_MSP_SLEEP:
             *buffer = (char *)DUMP_EXT_MSP_SLEEP_ADDR;
              *length = (u32)DUMP_EXT_MSP_SLEEP_SIZE;
                break;
        default:
            dump_error("bsp_dump_get_buffer[%d]:  mod_id is invalid! mod=%d\n", __LINE__, mod_id);
            return BSP_ERR_DUMP_INVALID_MODULE;
    }

    return BSP_OK;
}

s32 bsp_dump_save_file(dump_save_file_t file_id, char* buffer, u32 length, dump_save_file_mode_t mode)
{
    char *filename;

    if((DUMP_SAVE_FILE_BUTT <= file_id) || ((DUMP_SAVE_FILE_MODE_BUTT <= mode)) || (!buffer) || (!length))
    {
        dump_error("bsp_dump_save_file[%d]:  invalid param! %d 0x%x %d %d\n", __LINE__, file_id, buffer, length, mode);
        return BSP_ERR_DUMP_INVALID_PARAM;
    }

    switch(file_id)
    {
        case DUMP_SAVE_FILE_OM:
            filename = OM_OAM_LOG;
            break;
        case DUMP_SAVE_FILE_DIAG:
            filename = OM_DIAG_LOG;
            break;
        case DUMP_SAVE_FILE_ZSP:
            filename = OM_ZSP_DUMP;
            break;
        case DUMP_SAVE_FILE_HIFI:
            filename = OM_HIFI_DUMP;
            break;
        case DUMP_SAVE_FILE_NCM:
            filename = OM_NCM_LOG;
            break;
        default:
            dump_error("bsp_dump_save_file[%d]:  file_id is invalid! file=%d\n", __LINE__, file_id);
            return BSP_ERR_DUMP_INVALID_FILE;
    }

    bsp_om_save_file(filename, buffer, length, mode);

    return BSP_OK;
}

void bsp_dump_int_lock(bsp_module_e mod_id)
{
    u32 addr = DUMP_INTLOCK_ADDR;

    if(1 == g_dump_cfg.dump_cfg.Bits.intLock)
    {
        QueueLoopIn((dump_queue_t *)addr, (mod_id<<16));
        QueueLoopIn((dump_queue_t *)addr, om_timer_get());

        g_dump_ptr_info.int_lock_ptr= DUMP_INTLOCK_ADDR +0x10
                                                            + (((dump_queue_t *)addr)->rear )*4;

        if(BSP_NULL != g_dump_trace_read_cb[DUMP_SYSVIEW_INTLOCK])
        {
            if( (QueueNumGet((dump_queue_t *)addr)*sizeof(u32)) >= DUMP_INT_LOCK_THRESHOLD_SIZE)
            {
                g_dump_trace_read_cb[DUMP_SYSVIEW_INTLOCK]();
            }
        }
    }
}

void bsp_dump_int_unlock(bsp_module_e mod_id)
{
     u32 addr = DUMP_INTLOCK_ADDR;

    if(1 == g_dump_cfg.dump_cfg.Bits.intLock)
    {
        QueueLoopIn((dump_queue_t *)addr, (((u32)DUMP_INT_UNLOCK_FLAG<<16) |mod_id));
        QueueLoopIn((dump_queue_t *)addr, om_timer_get());

        g_dump_ptr_info.int_lock_ptr= DUMP_INTLOCK_ADDR +0x10
                                                            + (((dump_queue_t *)addr)->rear )*4;

        if(BSP_NULL != g_dump_trace_read_cb[DUMP_SYSVIEW_INTLOCK])
        {
            if( (QueueNumGet((dump_queue_t *)addr)*sizeof(u32)) >= DUMP_INT_LOCK_THRESHOLD_SIZE)
            {
                g_dump_trace_read_cb[DUMP_SYSVIEW_INTLOCK]();
            }
        }
    }
}
s32 bsp_dump_get_sysview_addr(dump_sysview_t mod_id, void *addr)
{
    return BSP_ERROR;
}

s32 bsp_dump_register_sysview_hook(dump_sysview_t mod_id, dump_save_hook func)
{
    if(mod_id >= DUMP_SYSVIEW_BUTT )
    {
        return BSP_ERR_DUMP_INVALID_MODULE;
    }
    else
    {
        g_dump_trace_read_cb[mod_id] = func;
    }

    return BSP_OK;
}

void bsp_dump_trace_stop(void)
{
    dump_trace_stop();
}

/*****************************************************************************
* 函 数 名  : bsp_om_set_hso_conn_flag
*
* 功能描述  :该接口需要MSP的诊断模块在HSO连接或者去连接的时候调用
*
* 输入参数  : flag :连接标志，1表示连接，0表示断开
*
*
* 输出参数  :无
*
* 返 回 值  : 无
*****************************************************************************/

void bsp_dump_set_hso_conn_flag(u32 flag)
{
    g_dump_global_info->internal_info.hso_connect_flag = flag;
}

void dump_show_map(void)
{
    printk("APP DUMP MEMORY MAP:\n");
    printk("DUMP_MAP_ADDR:         addr=0x%x, ofsfet=0x%x, size=0x%x\n", DUMP_MAP_ADDR,         DUMP_MAP_ADDR-DUMP_MEM_BASE,          DUMP_MAP_SIZE);
    printk("DUMP_BASE_INFO_ADDR:   addr=0x%x, ofsfet=0x%x, size=0x%x\n", DUMP_BASE_INFO_ADDR,   DUMP_BASE_INFO_ADDR-DUMP_MEM_BASE,    DUMP_BASE_INFO_SIZE);
    printk("DUMP_TASK_SWITCH_ADDR: addr=0x%x, ofsfet=0x%x, size=0x%x\n", DUMP_TASK_SWITCH_ADDR, DUMP_TASK_SWITCH_ADDR-DUMP_MEM_BASE,  DUMP_TASK_SWITCH_SIZE);
    printk("DUMP_INTLOCK_ADDR:     addr=0x%x, ofsfet=0x%x, size=0x%x\n", DUMP_INTLOCK_ADDR,     DUMP_INTLOCK_ADDR-DUMP_MEM_BASE,      DUMP_INTLOCK_SIZE);
    printk("DUMP_TASK_STACK_ADDR:  addr=0x%x, ofsfet=0x%x, size=0x%x\n", DUMP_TASK_STACK_ADDR,  DUMP_TASK_STACK_ADDR-DUMP_MEM_BASE,   DUMP_TASK_STACK_SIZE);
    printk("DUMP_INT_STACK_ADDR:   addr=0x%x, ofsfet=0x%x, size=0x%x\n", DUMP_INT_STACK_ADDR,   DUMP_INT_STACK_ADDR-DUMP_MEM_BASE,    DUMP_TASK_STACK_SIZE);
    printk("DUMP_ALLTASK_ADDR:     addr=0x%x, ofsfet=0x%x, size=0x%x\n", DUMP_ALLTASK_ADDR,     DUMP_ALLTASK_ADDR-DUMP_MEM_BASE,      DUMP_ALLTASK_SIZE);
    printk("DUMP_ALLTASK_TCB_ADDR: addr=0x%x, ofsfet=0x%x, size=0x%x\n", DUMP_ALLTASK_TCB_ADDR, DUMP_ALLTASK_TCB_ADDR-DUMP_MEM_BASE,  DUMP_ALLTASK_TCB_SIZE);
    printk("DUMP_PRINT_ADDR:       addr=0x%x, ofsfet=0x%x, size=0x%x\n", DUMP_PRINT_ADDR,       DUMP_PRINT_ADDR-DUMP_MEM_BASE,        DUMP_PRINT_SIZE);
    printk("DUMP_REGS_ADDR:        addr=0x%x, ofsfet=0x%x, size=0x%x\n", DUMP_REGS_ADDR,        DUMP_REGS_ADDR-DUMP_MEM_BASE,         DUMP_REGS_SIZE);
    printk("DUMP_CPUVIEW_ADDR:     addr=0x%x, ofsfet=0x%x, size=0x%x\n", DUMP_CPUVIEW_ADDR,     DUMP_CPUVIEW_ADDR-DUMP_MEM_BASE,      DUMP_CPUVIEW_SIZE);
    printk("DUMP_MEMINFO_ADDR:     addr=0x%x, ofsfet=0x%x, size=0x%x\n", DUMP_MEMINFO_ADDR,     DUMP_MEMINFO_ADDR-DUMP_MEM_BASE,      DUMP_MEMINFO_SIZE);
    printk("DUMP_USER_DATA_ADDR:   addr=0x%x, ofsfet=0x%x, size=0x%x\n", DUMP_USER_DATA_ADDR,   DUMP_USER_DATA_ADDR-DUMP_MEM_BASE,    DUMP_USER_DATA_SIZE);
    printk("DUMP_RSV_ADDR:         addr=0x%x, ofsfet=0x%x, size=0x%x\n", DUMP_RSV_ADDR,         DUMP_RSV_ADDR-DUMP_MEM_BASE,          DUMP_RSV_SIZE);
}

/*lint -save -e19*/
void int_lock_hook_add(func_void in_func,func_void out_func)
{
    g_intlock_in_hook = in_func;
    g_intlock_out_hook = out_func;
}

void int_lock_hook(void)
{
    if(g_intlock_in_hook)
    {
        (g_intlock_in_hook)();
    }
}
EXPORT_SYMBOL(int_lock_hook);

void int_unlock_hook(void)
{
    if(g_intlock_out_hook)
    {
        (g_intlock_out_hook)();
    }
}

EXPORT_SYMBOL(int_unlock_hook);


module_init(bsp_dump_init);
/*lint -restore +e19*/

//subsys_initcall(bsp_dump_init);

