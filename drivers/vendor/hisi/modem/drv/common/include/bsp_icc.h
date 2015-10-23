

#ifndef __BSP_ICC_H__
#define __BSP_ICC_H__

#ifdef __cplusplus /* __cplusplus */
extern "C"
{
#endif /* __cplusplus */

#include <osl_common.h>
#include <drv_icc.h>
#include <bsp_ipc.h>

#define ICC_BUSY              (0x03 | NOTIFY_STOP_MASK)
#define ICC_OK                (0)
#define ICC_ERR               (-1)

/* CPU ID 分配 */
enum CPU_ID
{
	ICC_CPU_MIN = IPC_CORE_ACORE,
	ICC_CPU_APP = IPC_CORE_ACORE,
	ICC_CPU_MODEM = IPC_CORE_CCORE,
	ICC_CPU_MCU = IPC_CORE_MCORE,
	ICC_CPU_TLDSP = IPC_CORE_LDSP,
	ICC_CPU_HIFI = IPC_CORE_HiFi,
	ICC_CPU_MAX
};

/* 错误码定义 */
enum ICC_ERR_NO
{
	ICC_CHN_INIT_FAIL = (0x80000000 + (0 << 16)),
	ICC_MALLOC_CHANNEL_FAIL,
	ICC_MALLOC_VECTOR_FAIL,
	ICC_CREATE_TASK_FAIL,
	ICC_DEBUG_INIT_FAIL,
	ICC_CREATE_SEM_FAIL,
	ICC_REGISTER_INT_FAIL,
	ICC_INVALID_PARA,
	ICC_INVALID_NO_FIFO_SPACE,
	ICC_WAIT_SEM_TIMEOUT,
	ICC_SEND_ERR,
	ICC_RECV_ERR,
	ICC_REGISTER_CB_FAIL,
	ICC_REGISTER_DPM_FAIL,
	ICC_MALLOC_MEM_FAIL,
	ICC_NULL_PTR
};

/* 通道id分配 */
enum ICC_CHN_ID
{
	ICC_CHN_ACORE_CCORE_MIN=0, /* add for modem reset : A/C核通道开始标志 */
	ICC_CHN_IFC = 0,     /* acore与ccore之间的IFC的物理通道 */
	ICC_CHN_RFILE = 1,   /* acore与ccore之间的RFILE的物理通道 */
	ICC_CHN_NV = 2,      /* acore与ccore之间的NV的物理通道 */
	ICC_CHN_GUOM0 = 3,  /* acore与ccore之间的GUOM0的物理通道 */
	ICC_CHN_GUOM1,       /* acore与ccore之间的GUOM1的物理通道 */
	ICC_CHN_GUOM2,       /* acore与ccore之间的GUOM2的物理通道 */
	ICC_CHN_GUOM3,       /* acore与ccore之间的GUOM3的物理通道 */
	ICC_CHN_GUOM4,       /* acore与ccore之间的GUOM4的物理通道 */
	ICC_CHN_GUOM5,       /* acore与ccore之间的GUOM5的物理通道 */
	ICC_CHN_CSHELL,      /* acore与ccore之间的CSHELL的物理通道 */
	ICC_CHN_ACORE_CCORE_MAX, /* add for modem reset : A/C核通道结束标志 */
	ICC_CHN_MCORE_CCORE, /* mcore与ccore之间的唯一的物理通道 */
	ICC_CHN_MCORE_ACORE, /* mcore与acore之间的唯一的物理通道 */
	ICC_CHN_ID_MAX
};

/* 说明: 接收回调函数ID,子通道必须放置在, "通道名称_xxx=0和通道名称_RECV_FUNC_ID_MAX之间，
 * 如: 要在IFC通道添加一个子通道，名称为IFC_RECV_ONLY_TEST，则需要:
    IFC_RECV_FUNC_RTC_SETTIME = 0,
	IFC_RECV_FUNC_BBE,
	IFC_RECV_ONLY_TEST,
	IFC_RECV_FUNC_ID_MAX
 */
enum ICC_RECV_FUNC_ID{
	IFC_RECV_FUNC_RTC_SETTIME = 0,
	IFC_RECV_FUNC_BBE,
	IFC_RECV_FUNC_ONOFF,
	IFC_RECV_FUNC_ANTEN,
	IFC_RECV_FUNC_EFUSE,
	IFC_RECV_FUNC_GPIO,
	IFC_RECV_FUNC_SIM0,
	IFC_RECV_FUNC_SIM1,
	IFC_RECV_FUNC_CSHELL,
	IFC_RECV_FUNC_UART,
    IFC_RECV_FUNC_LED,
    IFC_RECV_FUNC_RESET,
    FC_RECV_FUNC_GPSCLK,
	IFC_RECV_FUNC_PASTAR,
    IFC_RECV_FUNC_PA_RF,
	/* 若要在ICC_CHN_IFC物理通道上定义子通道,请在该注释行之前定义 */
	IFC_RECV_FUNC_ID_MAX,

	RFILE_RECV_FUNC_ID = 0,
	/* 若要在ICC_CHN_RFILE物理通道上定义子通道,请在该注释行之前定义 */
	RFILE_RECV_FUNC_ID_MAX,

    NV_RECV_FUNC_AC = 0,
	/* 若要在ICC_CHN_NV物理通道上定义子通道,请在该注释行之前定义 */
	NV_RECV_FUNC_ID_MAX,

	GUOM0_TEST1 = 0,
	/* 若要在ICC_CHN_GUOM0物理通道上定义子通道,请在该注释行之前定义 */
	GUOM0_RECV_FUNC_ID_MAX,

	GUOM1_TEST1 = 0,
	/* 若要在ICC_CHN_GUOM1物理通道上定义子通道,请在该注释行之前定义 */
	GUOM1_RECV_FUNC_ID_MAX,

	GUOM2_TEST1 = 0,
	/* 若要在ICC_CHN_GUOM1物理通道上定义子通道,请在该注释行之前定义 */
	GUOM2_RECV_FUNC_ID_MAX,

	GUOM3_TEST1 = 0,
	/* 若要在ICC_CHN_GUOM2物理通道上定义子通道,请在该注释行之前定义 */
	GUOM3_RECV_FUNC_ID_MAX,

	GUOM4_TEST1 = 0,
	/* 若要在ICC_CHN_GUOM4物理通道上定义子通道,请在该注释行之前定义 */
	GUOM4_RECV_FUNC_ID_MAX,

	GUOM5_TEST1 = 0,
	/* 若要在ICC_CHN_GUOM5物理通道上定义子通道,请在该注释行之前定义 */
	GUOM5_RECV_FUNC_ID_MAX,

	CSHELL_TEST1 = 0,
	/* 若要在ICC_CHN_CSHELL物理通道上定义子通道,请在该注释行之前定义 */
	CSHELL_RECV_FUNC_ID_MAX,

	MCORE_CCORE_FUNC_TEST1 = 0,
	MCORE_CCORE_FUNC_TEST2,
	MCORE_CCORE_FUNC_HKADC,
	MCU_CCORE_CPUFREQ,
	MCU_CCORE_WSRC,
	MCORE_CCORE_FUNC_WAKEUP,
	MCORE_CCORE_FUNC_REGULATOR,
	MCORE_CCORE_FUNC_SIM0,
	MCORE_CCORE_FUNC_SIM1,
	MCORE_CCORE_FUNC_SOCP,
	MCORE_CCORE_FUNC_UART,
	/* 若要在ICC_CHN_MCORE_CCORE物理通道上定义子通道,请在该注释行之前定义 */
	MCORE_CCORE_FUNC_ID_MAX,

	MCORE_ACORE_FUNC_TEST1 = 0,
	MCORE_ACORE_FUNC_TEST2,
	MCORE_ACORE_FUNC_HKADC,
	MCU_ACORE_CPUFREQ,
	MCU_ACORE_WSRC,
	NV_RECV_FUNC_AM,
	MCORE_ACORE_FUNC_TEMPERATURE,
	MCU_ACORE_RFILE,    /* for rfile */
	MCORE_ACORE_FUNC_WAKEUP,
	MCORE_ACORE_FUNC_DUMP,
	MCORE_ACORE_FUNC_RESET,
	/* 若要在ICC_CHN_MCORE_ACORE物理通道上定义子通道,请在该注释行之前定义 */
	MCORE_ACORE_FUNC_ID_MAX
};
/* 枚举定义end */

struct icc_channel_packet
{
	u32 channel_id;               /* 该数据包记录的通道id */
	u32 len;                      /* 该数据包长度 */
	u32 src_cpu_id;               /* 该数据包的发送者的cpu id */
	u32 seq_num;                  /* 该数据包的序列号: 同步发送的等待队列需要用到 */
	u32 need_responsed:1;         /* 该数据包是否需要回复给对方核标识bit位 */
	u32 is_responsed:1;           /* 该数据包是否是对方核回复过来的标识bit位 */
	u32 reserved:30;              /* 保留bit位 */
	s32 data;                     /* 数据包头的context */
	u32 timestamp;                /* 该数据包的时间戳 */
	u32 task_id;                  /* 该数据包的发送者的任务id */
};

#define ICC_CHANNEL_PAYLOAD                        (sizeof(struct icc_channel_packet) + 1)
#define ICC_CHANNEL_ID_MAKEUP(channel_id, func_id) ((channel_id << 16) | (func_id))

typedef s32 (*read_cb_func)(u32 channel_id , u32 len, void* context);
typedef s32 (*write_cb_func)(u32 channel_id , void* context);
/* 对外接口声明start */
/*****************************************************************************
* 函 数 名  : bsp_icc_send
* 功能描述  : icc异步发送数据接口
* 输入参数  : u32 cpuid       数据要发送的cpu的编号，使用约束: 要使用enum CPU_ID定义成员的枚举值
*             u32 channel_id  channel_id = 通道id << 16 || function_id, 使用约束:
                               1) channel_id高16bit为通道标识ID，使用enum ICC_CHN_ID的枚举值
                               2) 低16bit为回调函数标识ID，使用ICC_RECV_FUNC_ID对应通道id的枚举值
*             u8 *buffer      要发送的数据buffer的指针
*             u32 data_len    要发送的数据的大小, 使用约束: 最大值 < fifo_size - ICC_CHANNEL_PACKET_LEN
* 输出参数  : 无
* 返 回 值  : 正确:  实际写入fifo的大小;  错误: 负值
* 说    明  : 1) 公共通道：配套使用的注册在对方核的回调函数里必须要使用bsp_icc_read()函数把数据读走
*            2) 专用通道：支持在改通道使用模块的任务上下文中使用bsp_icc_read()函数把数据读走，但发送端必须有反压（通道数据发送缓慢）处理机制。

*****************************************************************************/
s32 bsp_icc_send(u32 cpuid,u32 channel_id,u8 *buffer,u32 data_len);

/*****************************************************************************
* 函 数 名  : bsp_icc_send_sync
* 功能描述  : icc同步发送数据接口
* 输入参数  : u32 cpuid       数据要发送的cpu的编号，使用约束: 要使用enum CPU_ID定义成员的枚举值
*             u32 channel_id  channel_id = 通道id << 16 || function_id, 使用约束:
                               1) channel_id高16bit为通道标识ID，使用enum ICC_CHN_ID的枚举值
                               2) 低16bit为回调函数标识ID，使用ICC_RECV_FUNC_ID对应子通道id的枚举值
*             u8 *buffer      要发送的数据buffer的指针
*             u32 data_len    要发送的数据的大小, 使用约束: 最大值 < fifo_size - ICC_CHANNEL_PACKET_LEN
*             s32 timeout     等回复超时间，使用约束: 要无限等需要使用宏 MAX_SCHEDULE_TIMEOUT
* 输出参数  : 无
* 返 回 值  : 正确: 接收核回调函数的返回值;  错误: 负值
* 说    明  : 1) 配套使用的注册在对方核的回调函数里必须要使用bsp_icc_read()函数把数据读走
*             2) 调用该接口会导致调用者任务睡眠直到超时时间到，或者对方核返回函数调用结果
*             3) 该接口适用于核间函数调用(IFC)的情形，核间函数调用只能返回系统自带的简单数据类型，不能返回结构体等用户自定义类型
*             4) 该接口不能在icc回调中使用，会引起死锁和icc任务延迟的严重后果

*****************************************************************************/
s32 bsp_icc_send_sync(u32 cpuid,u32 channel_id,u8 * data,u32 data_len,s32 timeout);

/*****************************************************************************
* 函 数 名  : bsp_icc_read
* 功能描述  : icc从fifo读取数据接口
* 输入参数  : u32 channel_id  channel_id = 通道id << 16 || function_id, 使用约束:
                1) channel_id高16bit为通道标识ID，使用enum ICC_CHN_ID
                2) 低16bit为回调函数标识ID，使用ICC_RECV_FUNC_ID对应通道id的枚举值
*             u8 *buf        要读取fifo的数据buffer的指针
*             u32 buf_len    数据buffer的大小
* 输出参数  : 无
* 返 回 值  : 正确:  实际从通道读取数据大小;  错误: 负值
* 说    明  : 1) 可在回调函数中使用，或使用者自己任务上下文中使用，但发送端必须有反压（通道数据发送缓慢）处理机制，以保持核间通信数据流畅通
*****************************************************************************/
s32 bsp_icc_read(u32 channel_id,u8 * buf,u32 buf_len);

/*****************************************************************************
* 函 数 名  : bsp_icc_event_register
* 功能描述  : 使用icc通道注册回调函数接口
* 输入参数  : u32 channel_id  channel_id = 通道id << 16 || function_id, 使用约束:
                1) channel_id高16bit为通道标识ID，使用enum ICC_CHN_ID
                2) 低16bit为回调函数标识ID，使用ICC_RECV_FUNC_ID对应通道id的枚举值
*             read_cb_func read_cb      读回调
*             void *read_context        传给接收任务的上下文信息，做扩展用
*             write_cb_func write_cb    读回调
*             void *write_context       传给接收任务的上下文信息，做扩展用
* 输出参数  : 无
* 返 回 值  : 正确:  0;  错误: 错误码
* 说    明  : 回调函数中不能有任何会引起任务阻塞、睡眠的函数调用，如：
*             1) taskDelay()
*             2) 信号量获取
*             3) printf()
*             4) malloc()
*             5) bsp_icc_send_sync()
*****************************************************************************/
s32 bsp_icc_event_register(u32 channel_id, read_cb_func read_cb, void *read_context,
                                              write_cb_func write_cb, void *write_context);

/*****************************************************************************
* 函 数 名  : bsp_icc_event_unregister
* 功能描述  : 使用icc通道去注册回调函数接口
* 输入参数  : u32 channel_id  channel_id = 通道id << 16 || function_id, 使用约束:
                1) channel_id高16bit为通道标识ID，使用enum ICC_CHN_ID
                2) 低16bit为回调函数标识ID，使用ICC_RECV_FUNC_ID对应通道id的枚举值
* 输出参数  : 无
* 返 回 值  : 正确:  0;  错误: 负值
*****************************************************************************/
s32 bsp_icc_event_unregister(u32 channel_id);

/*****************************************************************************
* 函 数 名  : bsp_icc_debug_register
* 功能描述  : 使用icc通道注册回调函数接口
* 输入参数  : u32 channel_id  channel_id = 通道id << 16 || function_id, 使用约束:
                1) channel_id高16bit为通道标识ID，使用enum ICC_CHN_ID
                2) 低16bit为回调函数标识ID，使用ICC_RECV_FUNC_ID对应通道id的枚举值
*             FUNCPTR_1 debug_routine     带一个参数的低功耗唤醒回调
*             int param       传给回调函数的参数
* 输出参数  : 无
* 返 回 值  : 正确:  0;  错误: 错误码
* 说    明  : 回调函数中不能有任何会引起任务阻塞、睡眠的函数调用，如：
*             1) taskDelay()
*             2) 信号量获取
*             3) printf()
*             4) malloc()
*             5) bsp_icc_send_sync()
*****************************************************************************/
s32 bsp_icc_debug_register(u32 channel_id, FUNCPTR_1 debug_routine, int param);

s32 bsp_icc_init(void);
void bsp_icc_release(void);
s32 bsp_icc_suspend(void);
void icc_dump_hook(void);

#ifdef __cplusplus /* __cplusplus */
}
#endif /* __cplusplus */

#endif    /*  __BSP_ICC_H__ */
