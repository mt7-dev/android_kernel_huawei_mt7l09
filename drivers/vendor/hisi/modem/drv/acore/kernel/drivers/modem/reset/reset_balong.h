

#ifndef __RESET_BALONG_H__
#define __RESET_BALONG_H__

#ifdef __cplusplus /* __cplusplus */
extern "C"
{
#endif /* __cplusplus */

#include <linux/wakelock.h>
#include <linux/printk.h>
#include <linux/suspend.h>
#include <linux/workqueue.h>
#include <linux/completion.h>

#include <osl_sem.h>
#include <osl_spinlock.h>
#include <drv_nv_def.h>

#include <bsp_reset.h>

/************************** 宏和枚举定义 **************************/
#define RESET_WAIT_RESP_TIMEOUT          (5000)   /*ms, time of wating for reply from hifi/mcu*/
#define RESET_WAIT_MODEM_IN_IDLE_TIMEOUT (310)   /*ms, time of wating for reply from modem in idle*/
#define RESET_WAIT_M3_RESETING_REPLY_TIMEOUT (200)   /*ms, time of wating for reply from modem in idle*/
#define RESET_WAIT_CCPU_STARTUP_TIMEOUT  (20000)
#define DRV_RESET_MODULE_NAME_LEN        (9)
#define reset_print_err(fmt, ...)        (printk(KERN_ERR "[reset]: <%s> "fmt, __FUNCTION__, ##__VA_ARGS__))

enum RESET_TYPE
{
    RESET_TYPE_CB_INVOKE_BEFORE,
    RESET_TYPE_SEND_MSG2_M3_FAIL_BEFORE,
    RESET_TYPE_RECV_WRONG_MSG_FROM_M3_BEFORE,
    RESET_TYPE_SEND_MSG2_HIFI_FAIL_BEFORE,
    RESET_TYPE_WAIT_HIFI_REPLY_FAIL_BEFORE,
    RESET_TYPE_LOAD_MODEM_IMG_FAIL,
    RESET_TYPE_SEND_MSG2_M3_FAIL_RESTING,
    RESET_TYPE_RECV_WRONG_MSG_FROM_M3_RESTING,
    RESET_TYPE_M3_REPLY_FAIL_RESTING,
    RESET_TYPE_SEND_MSG2_M3_FAIL_AFTER,
    RESET_TYPE_RECV_WRONG_MSG_FROM_M3_AFTER,
    RESET_TYPE_CB_INVOKE_RESTING,
    RESET_TYPE_WAIT_CCORE_RELAY_TIMEOUT,
    RESET_TYPE_CB_INVOKE_AFTER,
    RESET_TYPE_ICC_READ_FAIL,
    RESET_TYPE_RECV_M3_MSG_FAIL,
    RESET_TYPE_RECV_M3_MSG_BUS_ERR,
    RESET_TYPE_RECV_CCORE_MSG_FAIL,
    RESET_TYPE_RECV_HIFI_MSG_FAIL,

    /*add new above,please!!*/
    RESET_TYPE_FOR_TEST,        /*用于测试*/
    RESET_TYPE_MAX
} ;

/* 结构体定义 */

/*Record information of callback functions*/
struct reset_cb_info
{
    char name[DRV_RESET_MODULE_NAME_LEN + 1];
    u32 priolevel;
    pdrv_reset_cbfun cbfun;
    s32 userdata;
};

struct reset_cb_list
{
	struct reset_cb_info cb_info;
	struct reset_cb_list *next;
};

struct modem_reset_ctrl
{
	u32 boot_mode;            /* 表示ccore启动状态: 正常启动/单独复位后启动 */
	u32 multicore_msg_switch; /* 核间消息开关: 1, 表示打开; 0表示关闭 */
	struct task_struct* task;
	u32 modem_action;
	spinlock_t action_lock;
	osl_sem_id task_sem;
	osl_sem_id wait_mcore_reply_sem;
	osl_sem_id wait_mcore_reply_reseting_sem;
	osl_sem_id wait_hifi_reply_sem;
	osl_sem_id wait_ccore_reset_ok_sem;
	osl_sem_id wait_modem_master_in_idle_sem;
	struct reset_cb_list *list_head;
	struct workqueue_struct *reset_wq;
	struct work_struct  work_reset;
	struct work_struct  work_power_off;
	struct work_struct  work_power_on;
	struct wake_lock wake_lock;
	struct notifier_block pm_notify;
	struct completion suspend_completion;
	IPC_INT_LEV_E ipc_send_irq_wakeup_ccore;
	IPC_INT_LEV_E ipc_recv_irq_idle;
	IPC_INT_LEV_E ipc_recv_irq_reboot;
	s32 in_suspend_state;
	u32 list_use_cnt;
	u32 reset_cnt;
	u32 crg_base;
	u32 exec_time;
	DRV_CCORE_RESET_STRU nv_config;
	u32 state;
};

struct modem_reset_debug
{
	u32 print_sw;
	u32 main_stage;
};

extern struct modem_reset_debug g_reset_debug;

#define reset_has_debug_print
#ifdef reset_has_debug_print

#define  reset_print_debug(fmt, ...) \
do {                               \
    if (g_reset_debug.print_sw)    \
        reset_print_err(fmt, ##__VA_ARGS__);\
} while (0)

#else
#define  reset_print_debug(fmt, ...) 
#endif

/* 函数声明 */
struct modem_reset_ctrl *bsp_reset_control_get(void);
s32 send_sync_msg_to_mcore(u32 reset_info, u32 *ack_val);

#ifdef __cplusplus /* __cplusplus */
}
#endif /* __cplusplus */

#endif    /*  __RESET_BALONG_H__ */
