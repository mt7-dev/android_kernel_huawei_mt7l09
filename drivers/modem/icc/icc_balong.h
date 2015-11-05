

#ifndef __BALONG_ICC_H__
#define __BALONG_ICC_H__

#ifdef __cplusplus /* __cplusplus */
extern "C"
{
#endif /* __cplusplus */

#ifdef __KERNEL__ /* linux */
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/string.h>
#include <linux/suspend.h>

#elif defined(__VXWORKS__) /* vxworks */
#include <stdio.h>
#include <string.h>

#elif defined(__CMSIS_RTOS) /* rtx(cm3 os) */
#include <cmsis_os.h>

#else
#endif

#include <osl_common.h>
#include <osl_sem.h>
#include <osl_spinlock.h>
#include <osl_wait.h>
#include <osl_list.h>
#include <osl_malloc.h>
#include <bsp_sram.h>
#include <bsp_shared_ddr.h>
#include <bsp_hardtimer.h>
#include <bsp_dump.h>
#include <bsp_om.h>
#include <bsp_ipc.h>
#include <bsp_icc.h>

#ifdef __KERNEL__ /* linux */
/* 特性控制开关 */
#undef   ICC_HAS_INDEPENDENT_CHN_FEATURE  /* 是否有高性能通道，需要起独立任何和使用单独的ipc中断 */
#define  ICC_HAS_SYNC_SEND_FEATURE        /* 是否有同步发送接口 */
#define  ICC_HAS_DEBUG_FEATURE            /* 是否有基本的debug功能 */
#undef   ICC_DEBUG_FUNCTION_EXT           /* 是否有扩展的debug功能 */

#define  ICC_SRC_CPU                  (ICC_CPU_APP)
#define  ICC_SEND_CPU                 (ICC_CPU_MODEM)
#define  ICC_RECV_IPC_SHARED          (IPC_ACPU_INT_SRC_ICC)
#define  ICC_TASK_STK_SIZE            (0x1000)
#define  ICC_TASK_PRIVATE_PRI         (91)
#define  ICC_TASK_SHARED_PRI          (90)
#define  ICC_DUMP_SAVE_MOD            DUMP_SAVE_MOD_ICC_ACORE

typedef  u32 icc_task_id;
typedef  s32 (*recv_task_func)(void const *obj);

#define  icc_print_error(fmt, ...)    (bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_ICC, "[icc]: <%s> "fmt, __FUNCTION__, ##__VA_ARGS__))
#define  icc_print_info               printk

struct icc_init_info
{
	u32 real_channel_id;
	u32 mode;
	u32 fifo_size;
	u32 recv_addr;
	u32 send_addr;
	u32 ipc_recv_irq_id;
	u32 ipc_send_irq_id;
	u32 func_size;
	const char *name;
};

#elif defined(__VXWORKS__) /* vxworks */
/* 特性控制开关 */
#undef   ICC_HAS_INDEPENDENT_CHN_FEATURE  /* 是否有高性能通道，需要起独立任何和使用单独的ipc中断 */
#define  ICC_HAS_SYNC_SEND_FEATURE        /* 是否有同步发送接口 */
#define  ICC_HAS_DEBUG_FEATURE            /* 是否有基本的debug功能 */
#undef   ICC_DEBUG_FUNCTION_EXT           /* 是否有扩展的debug功能 */

#define  ICC_SRC_CPU                  (ICC_CPU_MODEM)
#define  ICC_SEND_CPU                 (ICC_CPU_APP)
#define  ICC_RECV_IPC_SHARED          (IPC_CCPU_INT_SRC_ICC)
#define  ICC_TASK_STK_SIZE            (0x1000)
#define  ICC_TASK_PRIVATE_PRI         (4)
#define  ICC_TASK_SHARED_PRI          (4)
#define  NOTIFY_STOP_MASK             (0x8000)
#define  ICC_DUMP_SAVE_MOD            DUMP_SAVE_MOD_ICC_CCORE

typedef  u32 icc_task_id;
typedef  s32 (*recv_task_func)(void const *obj);

#define  icc_print_error(fmt, ...)    (bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_ICC, "[icc]: <%s> "fmt, __FUNCTION__, ##__VA_ARGS__))
#define  icc_print_info(fmt, ...)     (bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_ICC, fmt, ##__VA_ARGS__))

struct icc_init_info
{
	u32 real_channel_id;
	u32 mode;
	u32 fifo_size;
	u32 send_addr;
	u32 recv_addr;
	u32 ipc_send_irq_id;
	u32 ipc_recv_irq_id;
	u32 func_size;
	const char *name;
};

#elif defined(__CMSIS_RTOS) /* rtx(cm3 os) */
/* 特性控制开关 */
#undef   ICC_HAS_INDEPENDENT_CHN_FEATURE  /* 是否有高性能通道，需要起独立任何和使用单独的ipc中断 */
#undef   ICC_HAS_SYNC_SEND_FEATURE        /* 是否有同步发送接口 */
#undef   ICC_HAS_DEBUG_FEATURE            /* 是否有基本的debug功能 */
#undef   ICC_DEBUG_FUNCTION_EXT           /* 是否有扩展的debug功能 */

#define  ICC_SRC_CPU                  (ICC_CPU_MCU)
#define  ICC_SEND_CPU                 (ICC_CPU_APP)
#define  ICC_RECV_IPC_SHARED          (IPC_MCU_INT_SRC_ICC)
#define  ICC_TASK_STK_SIZE            (512)
#define  ICC_TASK_SHARED_PRI          (osPriorityAboveNormal)
#define  MAX_SCHEDULE_TIMEOUT         (0xff)
#define  NOTIFY_STOP_MASK             (0x8000)
#define  ICC_DUMP_SAVE_MOD            DUMP_SAVE_MOD_ICC_MCORE

typedef  osSemaphoreId osl_sem_id;
typedef  osThreadId icc_task_id;
typedef  s32 spinlock_t;
typedef  s32 wait_queue_head_t;
typedef void (*recv_task_func)(void const *obj);

#define  icc_print_error(fmt, ...)    (printk("[icc]: <%s> "fmt"\n", __FUNCTION__, ##__VA_ARGS__))
#define  icc_print_info               printk


struct icc_init_info
{
	u32 real_channel_id;
	u32 mode;
	u32 fifo_size;
	u32 send_addr;
	u32 recv_addr;
	u32 ipc_send_irq_id;
	u32 ipc_recv_irq_id;
	u32 func_size;
	const char *name;
};

/* 注意: M3使用osl_sem_init的第二个参数与linux和vxworks含义不一样 */
#define osl_sem_init(val, name)  osSemaphoreCreate(osSemaphore(name), val)

static __inline__ osStatus osl_sem_up(osl_sem_id *sem)
{
	return osSemaphoreRelease(*sem);
}

static __inline__ s32 osl_sem_down(osl_sem_id *sem)
{
	return osSemaphoreWait(*sem, osWaitForever);
}

/* 注意: M3第二个参数为毫秒与linux和vxworks含义不一样 */
static __inline__ s32 osl_sem_downtimeout(osl_sem_id *sem, u32 ms)
{
	if(osSemaphoreWait(*sem, ms))
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

static __inline__ osStatus osl_sema_delete(osl_sem_id *sem)
{
	return osSemaphoreDelete(*sem);
}

#define kthread_stop(id) osThreadTerminate(id)

#endif /* end of __KERNEL__ */


extern struct icc_dbg g_icc_dbg;

#define  icc_print_debug(fmt, ...) \
do {                               \
    if (g_icc_dbg.msg_print_sw)    \
        icc_print_error(fmt, ##__VA_ARGS__);\
} while (0)

#define icc_safe_free(p) \
do \
{ \
	if(p) \
	{ \
		osl_free((void *)p); \
		p = NULL; \
	} \
} \
while(0)

#define ICC_MIN(x, y)             ((x) < (y) ? (x) : (y))
#define GET_CHN_ID(channel_id)    (((channel_id)>>16) & 0xff)
#define GET_FUNC_ID(channel_id)   ((channel_id) & 0xff)

/* 宏定义start */
#define ICC_CHN_MAGIC_SIGN    (0x87651234)
#define ICC_CHN_MAGIC_UNSIGN  (0xabcd8765)

#define STRU_SIZE             (sizeof(struct icc_channel_packet))
#define ICC_IFC_SIZE          (SZ_4K)
#define ICC_RFILE_SIZE        (SZ_4K)
#define ICC_NV_SIZE           (SZ_4K)
#define ICC_GUOM0_SIZE        (SZ_16K)
#define ICC_GUOM1_SIZE        (SZ_4K)
#define ICC_GUOM2_SIZE        (SZ_4K)
#define ICC_GUOM3_SIZE        (SZ_4K)
#define ICC_GUOM4_SIZE        (SZ_128K)
#define ICC_GUOM5_SIZE        (SZ_16K)
#define ICC_CSHELL_SIZE       (SZ_8K)
#define ICC_MCCORE_SIZE       (SZ_512)
#define ICC_MACORE_SIZE       (SZ_512)

/* ddr共享内存通道地址定义, acore和ccore */
#define ICC_DBG_MSG_LEN_IN_DDR  (0x20)
#define ICC_DBG_MSG_ADDR_IN_DDR (((SHM_MEM_ICC_ADDR) + 3) & ~3)
#define ADDR_IFC_SEND         (ICC_DBG_MSG_ADDR_IN_DDR + ICC_DBG_MSG_LEN_IN_DDR)
#define ADDR_IFC_RECV         (ADDR_IFC_SEND    + STRU_SIZE + ICC_IFC_SIZE)
#define ADDR_RFILE_SEND       (ADDR_IFC_RECV    + STRU_SIZE + ICC_IFC_SIZE)
#define ADDR_RFILE_RECV       (ADDR_RFILE_SEND  + STRU_SIZE + ICC_RFILE_SIZE)
#define ADDR_NV_SEND          (ADDR_RFILE_RECV  + STRU_SIZE + ICC_RFILE_SIZE)
#define ADDR_NV_RECV          (ADDR_NV_SEND     + STRU_SIZE + ICC_NV_SIZE)
#define ADDR_GUOM0_SEND       (ADDR_NV_RECV     + STRU_SIZE + ICC_NV_SIZE)
#define ADDR_GUOM0_RECV       (ADDR_GUOM0_SEND  + STRU_SIZE + ICC_GUOM0_SIZE)
#define ADDR_GUOM1_SEND       (ADDR_GUOM0_RECV  + STRU_SIZE + ICC_GUOM0_SIZE)
#define ADDR_GUOM1_RECV       (ADDR_GUOM1_SEND  + STRU_SIZE + ICC_GUOM1_SIZE)
#define ADDR_GUOM2_SEND       (ADDR_GUOM1_RECV  + STRU_SIZE + ICC_GUOM1_SIZE)
#define ADDR_GUOM2_RECV       (ADDR_GUOM2_SEND  + STRU_SIZE + ICC_GUOM2_SIZE)
#define ADDR_GUOM3_SEND       (ADDR_GUOM2_RECV  + STRU_SIZE + ICC_GUOM2_SIZE)
#define ADDR_GUOM3_RECV       (ADDR_GUOM3_SEND  + STRU_SIZE + ICC_GUOM3_SIZE)
#define ADDR_GUOM4_SEND       (ADDR_GUOM3_RECV  + STRU_SIZE + ICC_GUOM3_SIZE)
#define ADDR_GUOM4_RECV       (ADDR_GUOM4_SEND  + STRU_SIZE + ICC_GUOM4_SIZE)
#define ADDR_GUOM5_SEND       (ADDR_GUOM4_RECV  + STRU_SIZE + ICC_GUOM4_SIZE)
#define ADDR_GUOM5_RECV       (ADDR_GUOM5_SEND  + STRU_SIZE + ICC_GUOM5_SIZE)
#define ADDR_CSHELL_SEND      (ADDR_GUOM5_RECV  + STRU_SIZE + ICC_GUOM5_SIZE)
#define ADDR_CSHELL_RECV      (ADDR_CSHELL_SEND + STRU_SIZE + ICC_CSHELL_SIZE)
#ifndef BSP_ICC_MCHANNEL_USE_LPM3TCM /* BALONGV7R2 */
/* SRAM(AXI)通道地址定义, mcore和ccore; mcore和acore */
#define ADDR_MCCORE_SEND      (SRAM_ICC_ADDR)
#else                          /* K3V3 */
#define ADDR_MCCORE_SEND      (ADDR_CSHELL_RECV + STRU_SIZE + ICC_CSHELL_SIZE)
#endif
#define ADDR_MCCORE_RECV      (ADDR_MCCORE_SEND + STRU_SIZE + ICC_MCCORE_SIZE)
#define ADDR_MACORE_SEND      (ADDR_MCCORE_RECV + STRU_SIZE + ICC_MCCORE_SIZE)
#define ADDR_MACORE_RECV      (ADDR_MACORE_SEND + STRU_SIZE + ICC_MACORE_SIZE)

#define PRIVATE_MODE          ((ICC_TASK_PRIVATE << 1) |(ICC_IPC_PRIVATE))
#define SHARED_MODE           ((ICC_TASK_SHARED << 1) |(ICC_IPC_SHARED))
#define ICC_SHARED_IPC_CHN_IDX (0xffffffff)

#define ICC_SEM_FULL          (1)
#define ICC_SEM_EMPTY         (0)

/* 枚举定义start */
/* icc状态 */
enum ICC_STATUS
{
	ICC_UNINITIALIZED = 0,
	ICC_INITIALIZED,
	ICC_STATUS_MAX
};

/* 任务类别分配 */
enum ICC_TASK_TYPE
{
	ICC_TASK_PRIVATE = 0,
	ICC_TASK_SHARED,
	ICC_TASK_TYPE_MAX
};

enum IPC_VEC_TYPE
{
	ICC_IPC_PRIVATE = 0,
	ICC_IPC_SHARED,
	IPC_VEC_TYPE_MAX
};

/* 通道状态 */
enum ICC_CHN_STAT
{
	ICC_CHN_CLOSED = 0,
	ICC_CHN_OPENED,
	ICC_CHN_STAT_MAX
};

enum ICC_PACKET_RETURN
{
	ICC_PACKET_NONEED_RESPONSE = 0,
	ICC_PACKET_NEED_RESPONSE,
	ICC_PACKET_RETURN_MAX
};

/* 枚举定义end */


/* 数据类型定义start */
struct icc_channel_fifo
{
	u32 magic;     /* fifo魔数，标识通道fifo的状态 */
	u32 size;      /* fifo大小 */
	u32 write;     /* fifo读指针 */
	u32 read;      /* fifo写指针 */
	u8  data[4];   /* fifo的context */
};
struct icc_pm_debug{
	FUNCPTR_1 debug_routine;
	int para;
};

struct icc_channel_vector
{
	read_cb_func  read_cb;        /* 接收向量的读回调函数指针 */
	void          *read_context;  /* 接收向量的读回调函数context */
	write_cb_func write_cb;       /* 接收向量的写回调函数指针 */
	void          *write_context; /* 接收向量的写回调函数context */
	s32           return_data;    /* 接收向量的返回值，用于同步发送的函数结果的保存 */
	struct icc_pm_debug pm_debug;
};

union ipc_task_shared
{
	u32 val;
	struct
	{
		u32 ipc_shared :1;
		u32 task_shared:1;
		u32 reserved   :30;
	}union_stru;
};

struct icc_channel
{
	u32                       id;               /* 通道id */
	const char                *name;            /* 通道名称 */
	u32                       state;            /* 通道状态: 标识发送通道是否可用 */
	u32                       ready_recv;       /* 通道状态: 标识接收通道是否可用 */
	union ipc_task_shared     mode;             /* 通道接收处理任务类型: shared|private */
	icc_task_id               private_task_id;  /* 通道私有的处理任务id */
	u32                       ipc_send_irq_id;  /* 通道私有的发送数据使用的ipc中断 */
	u32                       ipc_recv_irq_id;  /* 通道私有的接收数据使用的ipc中断 */
	osl_sem_id                private_task_sem; /* 唤醒通道私有的接收任务的信号量 */
	wait_queue_head_t         sen_sync_wq;      /* 用于同步发送的等待队列 */
	u32                       seq_num_send;     /* 用于同步发送的发送序列号: 递增 */
	osl_sem_id                seq_num_send_sem; /* 用于保护seq_num_send全局变量的信号量 */
	u32                       seq_num_recv;     /* 用于同步发送的，记录从数据包里读取的序列号 */
	struct icc_channel_fifo   *fifo_recv;       /* 接收fifo的指针 */
	struct icc_channel_fifo   *fifo_send;       /* 发送fifo的指针 */
	spinlock_t                write_lock;       /* 保护通道写互斥的spin锁 */
	spinlock_t                read_lock;        /* 保护通道读互斥的spin锁 */
	struct icc_channel_vector *rector;          /* 接收向量(子通道)指针 */
	u32                       func_size;        /* 接收向量(子通道)大小 */
};

struct icc_control
{
	u32                cpu_id;                    /* 当前核cpu id */
	u32                state;                     /* icc控制结构体状态: 可用|不可用 */
	icc_task_id        shared_task_id;            /* 通道共享任务id */
	u32                shared_recv_ipc_irq_id;    /* 通道共享的接收数据使用ipc中断 */
	osl_sem_id         shared_task_sem;           /* 唤醒通道共享任务的信号量 */
	u32                wake_up_flag;
	struct icc_channel *channels[ICC_CHN_ID_MAX]; /* icc_channel的结构体指针数组 */
	u32                channel_size;              /* 通道数目 */
#ifdef __KERNEL__
	struct notifier_block pm_notify;
#endif
};

struct icc_wait_data
{
	u32 channel_id;
	u32 packet_seq_num;
};

/* 消息类型定义for debug */
enum ICC_MSG_TYPE
{
	ICC_MSG_SEND = 0,
	ICC_MSG_RECV,
	ICC_MSG_TYPE_MAX
};

#ifdef ICC_HAS_DEBUG_FEATURE
#define  ICC_STAT_MSG_NUM   10

/*** 发送/接收消息统计，以消息为维度，需要调用dump接口，以备异常时分析 ***/
struct icc_uni_msg_info
{
	u32 channel_id;
	u32 send_task_id;
	u32 recv_task_id;
	u32 len;
	u32 pos;
	u32 duration_prev;
	u32 duration_post;
};

struct icc_msg_fifo
{
	u32 front;
	u32 rear;
	u32 size;
	struct icc_uni_msg_info msg[ICC_STAT_MSG_NUM];
};

struct icc_msg_info
{
	struct icc_msg_fifo send;
	struct icc_msg_fifo recv;
};

/*** 发送/接收消息统计，以通道为维度，分析通道数据 ***/
struct icc_channel_stat_info
{
	u32 sum_len;
	u32 sum_num;
	u32 sum_num_prev;
	u32 task_id;
};

struct icc_uni_channel_info
{
	struct icc_channel_stat_info total;
	struct icc_channel_stat_info *sub_chn;
	u32 func_size;
};

struct icc_channel_info
{
	u32 id;
	struct icc_uni_channel_info send;
	struct icc_uni_channel_info recv;
};


struct icc_dbg
{
	u32 state;
	u32 ipc_int_cnt;
	u32 msg_print_sw;
	u32 send_cpu_id;
	u32 recv_task_id;
	u32 timestamp;
	char *dump_buf_addr;
	u32 dump_buf_size;
	struct icc_msg_info msg_stat;
	struct icc_channel_info *channel_stat[ICC_CHN_ID_MAX];
};

/* 函数声明 */
void icc_control_dump(void);
void icc_init_info_dump(u32 real_channel_id);
void icc_channel_dump(u32 real_channel_id);
void icc_channel_fifo_dump(u32 real_channel_id);
void icc_channel_vector_dump(u32 channel_id, u32 func_id);
void icc_channel_packet_dump(struct icc_channel_packet *packet);
void icc_dbg_info_print(const char *fifo_name, u32 channel_id, u8 *data, u32 data_len);
void icc_dbg_print_sw(u32 sw);
u32  icc_taskid_get(void);
void icc_msg_queue_init(struct icc_msg_fifo *queue);
void icc_msg_queue_in(struct icc_msg_fifo *queue, struct icc_uni_msg_info *msg);
s32  icc_debug_init(u32 channel_num);
void icc_channel_msg_stat(struct icc_channel_stat_info *channel, u32 msg_len, u32 recv_task_id);

#else

struct icc_dbg
{
	u32 state;
	u32 ipc_int_cnt;
	u32 msg_print_sw;
	u32 send_cpu_id;
	u32 recv_task_id;
	u32 timestamp;
};

#endif /* ICC_HAS_DEBUG_FEATURE */

/* 函数声明start */
void icc_ipc_isr(u32 data);
u32 bsp_icc_channel_status_get(u32 real_channel_id, u32 *channel_stat);
u32 bsp_icc_channel_size_get(void);
void bsp_icc_channel_uninit(u32 real_channel_id);
u32 fifo_packet_put(struct icc_channel_fifo *fifo, u8 *head_buf, u32 head_len, u8 *data_buf, u32 data_len);
u32 fifo_get(struct icc_channel_fifo *fifo, u8 *data_buf, u32 data_len, u32 *read);
s32 bsp_icc_test_init(void);
s32 bsp_icc_mcore_test_init(void);
/* 函数声明end */

#ifdef __cplusplus /* __cplusplus */
}
#endif /* __cplusplus */

#endif    /*  __BALONG_ICC_H__ */
