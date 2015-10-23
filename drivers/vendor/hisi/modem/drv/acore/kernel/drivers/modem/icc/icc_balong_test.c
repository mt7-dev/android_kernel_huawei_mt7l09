
#ifdef __cplusplus /* __cplusplus */
extern "C"
{
#endif /* __cplusplus */

#include <osl_thread.h>
#include "icc_balong.h"
#include "icc_balong_test.h"

/*lint --e{958} */
extern struct icc_control g_icc_ctrl;
#if defined(__KERNEL__) || defined(__VXWORKS__)
extern struct icc_init_info g_icc_init_info[];
#endif

struct icc_test_case g_icc_test;

/* mcore测试用例 */
#define SINGLE_MSG_LEN   (20)
#define CCORE_MSG_POS    (0)
#define ACORE_MSG_POS    (CCORE_MSG_POS + SINGLE_MSG_LEN)
#define CCORE_DATA_START (0x55)
#define ACORE_DATA_START (0x32)

#ifdef __KERNEL__
#include <linux/delay.h>

s32 icc_task_delay(s32 msecs)
{
	msleep((u32)msecs);
	return ICC_OK;
}

#elif defined(__VXWORKS__)
s32 icc_task_delay(s32 msecs);
s32 icc_send_test_001(void);
s32 icc_send_test_002(void);
s32 icc_send_test_003(void);
s32 icc_send_test_004(void);
s32 icc_send_test_005(void);
s32 icc_send_test_006(void);
s32 icc_send_test_007(void);
s32 bsp_icc_test_uninit(void);
s32 icc_mcore_send_test_001(void);
s32 icc_mcore_send_test_002(void);

s32 icc_task_delay(s32 msecs)
{
	return taskDelay(msecs / 10);
}

#elif defined(__CMSIS_RTOS)
osSemaphoreDef(multi_send_confirm_sem);
osSemaphoreDef(confirm_sem01);
osSemaphoreDef(confirm_sem02);
osSemaphoreDef(confirm_sem03);
osSemaphoreDef(confirm_sem04);
extern osStatus osDelay(u32);

s32 icc_task_delay(s32 msecs)
{
	return osDelay((u32)msecs);
}

#endif

/* 测试发送函数 */
static s32 icc_send_test(enum CPU_ID send_cpu_id, u32 channel_id, u8 *buf, u32 data_len, u32 start, u32 timeout)
{
	u32 i = 0;

	for(i = 0; i < data_len; i++)
	{
		buf[i]= (u8)(i + start);
	}

	icc_print_debug("enter: channel_id=0x%x\n", channel_id);

	if((s32)data_len != bsp_icc_send(send_cpu_id, channel_id, buf, data_len))
	{
		icc_print_error("bsp_icc_send error\n");
		return ICC_ERR;
	}

	if(osl_sem_downtimeout(&(g_icc_test.channels[GET_CHN_ID(channel_id)].sub_channels[GET_FUNC_ID(channel_id)].confirm_sem), (long)timeout)) /*lint !e732 */
	{
		icc_print_error("icc_sem_take error\n");
		return ICC_ERR;
	}

	return ICC_OK;
}


/* 1. 比较发过来的信息是否是正确的(事先约定);
 * 2. 往对方核发确认信息，确认发过来的信息正确与否
 */
static s32 icc_send_test_001_cb0(u32 channel_id , u32 len, void* context)
{
	u32 i = 0;
	s32  confirm = ICC_RECV_OK;
	u32 start = (u32)context;
	u32 channel_idx = GET_CHN_ID(channel_id);
	u32 sub_chn_idx = GET_FUNC_ID(channel_id);

	icc_print_debug("====start:0x%x\n", start);
	
	if((s32)len != bsp_icc_read(channel_id, g_icc_test.channels[channel_idx].sub_channels[sub_chn_idx].rd_buf, len))
	{
		confirm = ICC_RECV_ERR;
		goto out;
	}

	for(i = 0; i < len; i++)
	{
		if(((u8)i + start) != g_icc_test.channels[channel_idx].sub_channels[sub_chn_idx].rd_buf[i])
		{
			confirm = ICC_RECV_ERR;
			goto out;
		}
	}

	confirm = ICC_RECV_OK;
	
out:
	channel_id = ((channel_idx << 16) | (sub_chn_idx + 1));
	bsp_icc_send(g_icc_dbg.send_cpu_id, channel_id, (u8 *)&confirm, sizeof(confirm));

	return ICC_OK;
	
}

/* 回复确认信息或者核间函数调用结果返回 */
static s32 icc_send_test_001_cb1(u32 channel_id , u32 len, void* context)
{
	s32 confirm = ICC_RECV_OK;
	u8 *buf = NULL;

	buf = g_icc_test.channels[GET_CHN_ID(channel_id)].sub_channels[GET_FUNC_ID(channel_id)].rd_buf;
	if(!buf)
	{
		icc_print_error("malloc mem error!\n");
		return ICC_ERR;
	}

	icc_print_debug("channel_id:0x%x\n", channel_id);
	//icc_task_delay(20);
	icc_print_debug("confirm_sem:0x%x\n", &(g_icc_test.channels[GET_CHN_ID(channel_id)].sub_channels[GET_FUNC_ID(channel_id)-1].confirm_sem)); /*lint !e40 */

	if((s32)len == bsp_icc_read(channel_id, buf, len))
	{
		memcpy(&confirm, buf, sizeof(confirm));
		icc_print_debug("confirm:0x%x\n", confirm);
		if(ICC_RECV_OK == confirm)
		{
			g_icc_test.channels[GET_CHN_ID(channel_id)].sub_channels[GET_FUNC_ID(channel_id)-1].success_cnt++;
			osl_sem_up(&(g_icc_test.channels[GET_CHN_ID(channel_id)].sub_channels[GET_FUNC_ID(channel_id)-1].confirm_sem)); /*lint !e40 !e516 */
		}
	}

	return ICC_SEND_SYNC_RET;
}


/* 该回调用于接收核不读数据,直接回确认信息给发送核 */
s32 icc_send_test_cb0_new(u32 channel_id , u32 len, void* context)
{
	u8  confirm     = ICC_CONF_MSG_TYPE1;
	u32 channel_idx = GET_CHN_ID(channel_id);
	u32 sub_chn_idx = GET_FUNC_ID(channel_id);
	u32 send_len    = sizeof(confirm);

	icc_print_debug("enter: channel_id=0x%x\n", ((channel_idx << 16) | (sub_chn_idx + 1)));

	if((s32)send_len != bsp_icc_send(g_icc_dbg.send_cpu_id, ((channel_idx << 16) | (sub_chn_idx + 1)), &confirm, send_len))
	{
		icc_print_error("send error!\n");
	}

	return ICC_OK;
}

/* 收对方核回过来的确认信息: 如果信息是ICC_CONF_MSG_TYPE1 */
s32 icc_send_test_cb1_new(u32 channel_id , u32 len, void* context)
{
	u8  confirm = (u8)ICC_RECV_OK;
	u32 channel_idx = GET_CHN_ID(channel_id);
	u32 sub_chn_idx = GET_FUNC_ID(channel_id);
	s32 read_len = sizeof(confirm);

	icc_print_debug("enter\n");

	if(read_len == bsp_icc_read(channel_id, &confirm, len))
	{
		icc_print_debug("confirm:0x%x\n", confirm);
		if(ICC_CONF_MSG_TYPE1 == confirm)
		{
			g_icc_test.channels[channel_idx].sub_channels[sub_chn_idx-1].success_cnt++;
			osl_sem_up(&(g_icc_test.channels[channel_idx].sub_channels[sub_chn_idx-1].confirm_sem)); /*lint !e40 !e516 */
		}
		else if(ICC_CONF_MSG_TYPE2 == confirm)
		{
			g_icc_test.channels[channel_idx].sub_channels[sub_chn_idx+1].success_cnt++;
			osl_sem_up(&(g_icc_test.channels[channel_idx].sub_channels[sub_chn_idx+1].confirm_sem)); /*lint !e40 !e516 */
		}
		else
		{
			icc_print_error("error: wrong confirm:0x%x\n", confirm);
		}
	}
	else
	{
		icc_print_error("icc read error\n", confirm);
	}

	return ICC_OK;
}

/* 该回调用于唤醒自己处理bsp_icc_read任务 */
s32 icc_send_test_cb2_new(u32 channel_id , u32 len, void* context)
{
	icc_print_debug("enter\n");
	osl_sem_up(&g_icc_test.recv_task_sem);
	return ICC_SEND_SYNC_RET;
}

/* 任务处理函数,读取通道里的数据 */
s32 icc_read_task_entry(void *obj)
{
	u32 channel_id  = (u32)obj;
	u32 channel_idx = GET_CHN_ID(channel_id);
	u32 sub_chn_idx = GET_FUNC_ID(channel_id);
	s32 read_len    = 0;
	u32 start       = 0;
	u8  confirm     = (u8)ICC_RECV_OK;
	s32 i           = 0;
	u8 buf[ICC_NEW_DATA_LEN]    = {0};

	icc_print_debug("enter!!!");

	for(;g_icc_test.recv_task_flag;)
	{
		osl_sem_down(&g_icc_test.recv_task_sem);

		read_len = bsp_icc_read(channel_id, buf, ICC_NEW_DATA_LEN);
		if(read_len < 0 || read_len > ICC_NEW_DATA_LEN)
		{
			confirm = (u8)ICC_RECV_ERR;
		}
		else
		{
			for(i = 0; i < read_len; i++)
			{
				if(((u8)i + start) != buf[i])
				{
					confirm = (u8)ICC_RECV_ERR;
				}
			}
			confirm = ICC_CONF_MSG_TYPE2;
		}

		icc_print_debug("channel_id=0x%x, confirm=0x%x\n", channel_id, confirm);
		bsp_icc_send(g_icc_dbg.send_cpu_id, (channel_idx << 16) | (sub_chn_idx - 1), &confirm, sizeof(confirm));
	}

	return 0;
}


/* 子通道重新分配, 每个通道分配64个子通道, 0-31为实际使用子通道, 32-64为测试用子通道 */
static s32 icc_test_channel_reinit(u32 channel_idx, u32 new_sub_chn_size)
{
	struct icc_channel *channel = g_icc_ctrl.channels[channel_idx];
	struct icc_channel_vector *buf = NULL;
	
	/* 备份原通道的子通道 */
	g_icc_test.channels[channel_idx].vec_bak = channel->rector;
	g_icc_test.channels[channel_idx].func_size_bak = channel->func_size;

	/* 分配64个子通道 */
	buf = (struct icc_channel_vector *)osl_malloc(sizeof(struct icc_channel_vector) * new_sub_chn_size);
	if (!buf)
	{
		icc_print_error("malloc memory failed\n");
		return ICC_MALLOC_VECTOR_FAIL;
	}
	memset(buf, 0, sizeof(struct icc_channel_vector) * new_sub_chn_size); /*lint !e665 */

	/* 实际使用通道保留，实际子通道不超过32个 */
	memcpy(buf, channel->rector, sizeof(struct icc_channel_vector) * channel->func_size);/*lint !e516 */

	/* 实际通道+测试用通道 */
	channel->rector = buf;
	channel->func_size = new_sub_chn_size;
	channel->fifo_send->read = channel->fifo_send->write= 0;

	/* fifo长度减小，以便进行压力测试 */
	g_icc_test.channels[channel_idx].fifo_size_bak = channel->fifo_send->size;
	channel->fifo_recv->size = channel->fifo_send->size = ICC_TEST_FIFO_SIZE;

	return ICC_OK;
}

/* 子通道初始化 */
static s32 icc_test_sub_chn_init(u32 channel_idx, u32 test_sub_chn_size, u32 start)
{
	u32 i = 0;
	u8 *buf = NULL;
	read_cb_func read_cb = NULL;
	u32 sub_chn_idx = 0;
	struct icc_test_sub_channel *sub_channel = NULL;

	/* 读写buffer一起分配 */
	buf = (u8 *)osl_malloc(ICC_TEST_FIFO_SIZE * test_sub_chn_size * 2); /*lint !e516 */
	if(NULL == buf)
	{
		icc_print_error("buf=0x%x, \n", buf);
		return ICC_MALLOC_MEM_FAIL;
	}
	memset(buf, 0x00,  ICC_TEST_FIFO_SIZE * test_sub_chn_size * 2); /*lint !e665 */

	for(i = 0; i < test_sub_chn_size; i++)
	{
		/* 32-64为测试用子通道 */
		sub_chn_idx = i + start;
		sub_channel = &(g_icc_test.channels[channel_idx].sub_channels[sub_chn_idx]);
		sub_channel->wr_buf = buf + i * ICC_TEST_FIFO_SIZE;
		sub_channel->rd_buf = buf + i * (ICC_TEST_FIFO_SIZE + test_sub_chn_size);
#if defined(__KERNEL__) || defined(__VXWORKS__)
		osl_sem_init(ICC_SEM_EMPTY, &(sub_channel->confirm_sem)); /*lint !e40 */
#endif
		/* 子通道交替注册cb0和cb1 */
		read_cb = ((i & 0x01)? icc_send_test_001_cb1: icc_send_test_001_cb0);
		g_icc_ctrl.channels[channel_idx]->rector[sub_chn_idx].read_cb = read_cb;
		g_icc_ctrl.channels[channel_idx]->rector[sub_chn_idx].read_context = (void *)sub_chn_idx;

#ifdef ICC_HAS_DEBUG_FEATURE		
		icc_print_debug("i:0x%x, read_cb: 0x%x", i, read_cb);
		if(g_icc_dbg.msg_print_sw)
		{
			icc_channel_vector_dump(channel_idx,sub_chn_idx);
		}
#endif

	}

	return ICC_OK;
}

/* 原acore/ccore代码开始 */
#if defined(__KERNEL__) || defined(__VXWORKS__)
static s32 icc_test_multi_send_func(void *obj)
{
	u32 channel_id = (u32)obj;
	u32 i = 0;
	u8 *buf = g_icc_test.channels[GET_CHN_ID(channel_id)].sub_channels[GET_FUNC_ID(channel_id)].wr_buf;

	for(i = 0; i < ICC_TEST_SEND_CNT; i++)
	{
		icc_task_delay(200);
		icc_print_debug("try to send channel%d's data, buf: 0x%x", channel_id, buf);
		if(ICC_OK != icc_send_test(ICC_SEND_CPU, channel_id, buf, 30, GET_FUNC_ID(channel_id), ICC_TAKE_SEM_TIMEOUT_JIFFIES))
		{
			icc_print_info("[FAIL] asynchronous send\n");
			return ICC_TEST_FAIL;
		}
	}

	icc_print_info("[PASS] asynchronous send\n");

	return ICC_TEST_PASS;
}

#ifdef ICC_HAS_SYNC_SEND_FEATURE
static s32 icc_test_multi_send_sync_func(void *obj)
{
	u32 channel_id = (u32)obj;
	u32 channel_idx = GET_CHN_ID(channel_id);
	u32 sub_chn_idx = GET_FUNC_ID(channel_id);
	u32 i = 0;
	u32 send_data = 0x10305070;

#ifdef ICC_HAS_SYNC_SEND_FEATURE
	for(i = 0; i < ICC_TEST_SEND_CNT; i++)
	{
		if (ICC_SEND_SYNC_RET != bsp_icc_send_sync(ICC_SEND_CPU,channel_id,(u8*)&send_data,sizeof(send_data),MAX_SCHEDULE_TIMEOUT)) /*lint !e40 !e516*/
		{
			icc_print_error("[FAIL] synchronous send!\n");
			return ICC_TEST_PASS;
		}
		g_icc_test.channels[channel_idx].sub_channels[sub_chn_idx].success_cnt++;
	}
#endif

	icc_print_info("[PASS] synchronous send\n");

	return ICC_TEST_PASS;
}
#endif

static s32 icc_test_recv_check(u32 channel_idx, u32 sub_chn_idx, u32 *cnt, u32 total_cnt)
{	
	if(ICC_TEST_SEND_CNT == g_icc_test.channels[channel_idx].sub_channels[sub_chn_idx].success_cnt)
	{
		(*cnt)++;
		if(total_cnt == (*cnt))
		{
			osl_sem_up(&g_icc_test.multi_send_confirm_sem); /*lint !e40 !e516*/
			return ICC_OK;
		}
	}

	icc_task_delay(20);

	return ICC_ERR;
}

/* 为多通道或子通道发送接收,创建的任务和信号量, 以同步用例执行*/
static s32 icc_test_wait(void *obj)
{
	u32 flag = (u32)obj;
	u32 i = 0;
	u32 cnt = 0;
	u32 channel_idx = 0;
	u32 sub_chn_idx = 0;

	icc_print_debug("************entry");

	for(;;)
	{
		/* 多子通道异步发送 */
		if(ICC_TEST_MUL_SUB_CHN_SEND_FLAG == flag)
		{
			channel_idx = ICC_CHN_IFC;
			for(i = ICC_TEST_CHANNEL_SIZE / 2; i < ICC_TEST_CHANNEL_SIZE / 2 + ICC_TEST_MULTI_SEND_CHN_SIZE; i++)
			{
				sub_chn_idx = i * 2 - 32;
				if (ICC_OK == icc_test_recv_check(channel_idx, sub_chn_idx, &cnt, ICC_TEST_MULTI_SEND_CHN_SIZE))
				{
					return ICC_OK;
				}
			}
		}
		/* 多子通道同步发送 */
		if(ICC_TEST_MUL_SUB_CHN_SEND_SYNC_FLAG == flag)
		{
			channel_idx = ICC_CHN_IFC;
			for(i = ICC_TEST_CHANNEL_SIZE / 2; i < ICC_TEST_CHANNEL_SIZE / 2 + ICC_TEST_MULTI_SEND_CHN_SIZE; i++)
			{
				sub_chn_idx = i * 2 - 31;
				if (ICC_OK == icc_test_recv_check(channel_idx, sub_chn_idx, &cnt, ICC_TEST_MULTI_SEND_CHN_SIZE))
				{
					return ICC_OK;
				}
			}
		}
		/* 多通道异步发送 */
		else if(ICC_TEST_MUL_CHN_SEND_FLAG == flag)
		{
			sub_chn_idx = ICC_TEST_DEFAULT_ASYN_SUBCHN_NUM;
			for(i = 0; i < ICC_VALIDE_CHN_NUM; i++)
			{
				channel_idx = g_icc_init_info[i].real_channel_id;
				if (ICC_OK == icc_test_recv_check(channel_idx, sub_chn_idx, &cnt, ICC_VALIDE_CHN_NUM))
				{
					return ICC_OK;
				}
			}
		}
		/* 多通道同步发送 */
		else if(ICC_TEST_MULTI_CHN_SEND_SYNC_FLAG == flag)
		{
			sub_chn_idx = ICC_TEST_DEFAULT_SYNC_SUBCHN_NUM;
			for(i = 0; i < ICC_VALIDE_CHN_NUM; i++)
			{
				channel_idx = g_icc_init_info[i].real_channel_id;
				if (ICC_OK == icc_test_recv_check(channel_idx, sub_chn_idx, &cnt, ICC_VALIDE_CHN_NUM))
				{
					return ICC_OK;
				}
			}
		}

		icc_task_delay(20);
	}
	
}

static s32 icc_test_multi_send_wait_ack_init(u32 flag)
{
	icc_print_debug("************entry");
#if defined(__KERNEL__) || defined(__VXWORKS__)
	osl_sem_init(ICC_SEM_EMPTY, &(g_icc_test.multi_send_confirm_sem)); /*lint !e40 */
	if(ICC_ERR == osl_task_init("icc_wait", ICC_TEST_TASK_PRI+1, ICC_TASK_STK_SIZE,
			(task_entry)icc_test_wait, (void*)(flag), &g_icc_test.task_id))
    {
        icc_print_error("create icc_wait task error\n");
        return ICC_ERR;
    }
#elif defined(__CMSIS_RTOS)
	g_icc_test.multi_send_confirm_sem = osl_sem_init(1, multi_send_confirm_sem);
	g_icc_test.task_id = osThreadCreate(osThread(icc_test_wait), (void*)flag);
	if(NULL == g_icc_test.task_id)
	{
        icc_print_error("create icc_wait task error\n");
        return ICC_ERR;
    }
#endif

	return ICC_OK;
}

static s32 icc_test_multi_send_test_init(u32 sn, u32 channel_id, task_entry entry)
{
	u32 channel_idx = GET_CHN_ID(channel_id);
	u32 sub_chn_idx = GET_FUNC_ID(channel_id);
	s32 *task_id = NULL;
	u8 name[30] = {0};
	
	icc_print_debug("************entry");

	g_icc_test.channels[channel_idx].sub_channels[sub_chn_idx].success_cnt = 0;
	icc_print_debug("i:0x%x, channel_id:0x%x", sn, channel_id);
		
	task_id = &(g_icc_test.channels[channel_idx].sub_channels[sub_chn_idx].task_id);
	sprintf((char*)name, "icc_mul%d", channel_id);

#if defined(__KERNEL__) || defined(__VXWORKS__)
	if(ICC_ERR == osl_task_init((char *)name, ICC_TEST_TASK_PRI, ICC_TASK_STK_SIZE,
		(task_entry)entry, (void*)(channel_id), (u32 *)task_id))
    {
        icc_print_error("create send test task error\n");
        return ICC_ERR;
    }
#elif defined(__CMSIS_RTOS)
	*task_id = osThreadCreate(osThread(icc_test_multi_send_func), (void*)channel_id);
	if(NULL == *task_id)
	{
		icc_print_error("create icc_wait task error\n");
		return ICC_ERR;
	}
#endif
	
	icc_print_debug("create send test task successfully\n");

	return ICC_OK;

}

s32 bsp_icc_test_init(void)
{
	u32 i = 0;
	s32 ret = ICC_OK;
	
	if(ICC_TEST_CASE_INITILIZED == g_icc_test.state)
	{
		icc_print_error("icc_test has intilized\n");
		return ICC_OK;
	}

	memset(&g_icc_test, 0, sizeof(g_icc_test)); /*lint !e665 */

	icc_print_debug("icc_test initilizing\n");

	for(i = 0; i < ICC_VALIDE_CHN_NUM; i++)
	{
		ret = icc_test_channel_reinit(g_icc_init_info[i].real_channel_id, ICC_TEST_CHANNEL_SIZE);
		if(ICC_OK != ret)
		{
			icc_print_error("icc_test_channel_reinit error\n");
			return ICC_ERR;
		}

		ret = icc_test_sub_chn_init(g_icc_init_info[i].real_channel_id, ICC_TEST_CHANNEL_SIZE/2, ICC_TEST_CHANNEL_START);
		if(ICC_OK != ret)
		{
			icc_print_error("icc_test_sub_chn_init error\n");
			return ICC_ERR;
		}
	}

#ifdef ICC_HAS_DEBUG_FEATURE
	/* 测试打开打印开关 */
	icc_dbg_print_sw(0);
#endif

	ret = icc_test_channel_reinit(ICC_CHN_CSHELL, SUPPORT_ICC_READ_IN_OTHER_TASK);
	if(ICC_OK != ret)
	{
		icc_print_error("cshell_channel_reinit error\n");
		return ICC_ERR;
	}

	ret = icc_test_sub_chn_init(ICC_CHN_CSHELL, SUPPORT_ICC_READ_IN_OTHER_TASK, 0);
	if(ICC_OK != ret)
	{
		icc_print_error("cshell_test_sub_chn_init error\n");
		return ICC_ERR;
	}

	bsp_icc_event_unregister(ICC_CHN_CSHELL << 16 | 0);
	bsp_icc_event_unregister(ICC_CHN_CSHELL << 16 | 1);
	bsp_icc_event_unregister(ICC_CHN_CSHELL << 16 | 2);

	bsp_icc_event_register(ICC_CHN_CSHELL << 16 | 0, (read_cb_func)icc_send_test_cb0_new, NULL, NULL, NULL);
	bsp_icc_event_register(ICC_CHN_CSHELL << 16 | 1, (read_cb_func)icc_send_test_cb1_new, NULL, NULL, NULL);
	bsp_icc_event_register(ICC_CHN_CSHELL << 16 | 2, (read_cb_func)icc_send_test_cb2_new, NULL, NULL, NULL);

	osl_sem_init(ICC_SEM_EMPTY,  &g_icc_test.recv_task_sem); /*lint !e40 */

	g_icc_test.recv_task_flag = 1;
	if(ICC_ERR == osl_task_init("icc_read_test", ICC_TEST_TASK_PRI+1, ICC_TASK_STK_SIZE,
			(task_entry)icc_read_task_entry, (void *)(ICC_CHN_CSHELL << 16 | 2), (u32 *)&g_icc_test.recv_task_id))
    {
        icc_print_error("create icc_read_test task error\n");
        return ICC_ERR;
    }

	icc_debug_init(bsp_icc_channel_size_get());

	/* 状态最后更新 */
	g_icc_test.state = ICC_TEST_CASE_INITILIZED;

	return ICC_OK;
}

/* icc异步发送基本测试 */
s32 icc_send_test_001(void)
{
	/* 异步发送, 默认要用测试专用的子通道, 即32 */
	u32 channel_idx = ICC_CHN_RFILE;
	u32 sub_chn_idx = ICC_TEST_DEFAULT_ASYN_SUBCHN_NUM;
	u32 channel_id = (channel_idx << 16 | sub_chn_idx);

	if(ICC_OK != icc_send_test(ICC_SEND_CPU, channel_id, g_icc_test.channels[channel_idx].sub_channels[sub_chn_idx].wr_buf,
				 60, sub_chn_idx, ICC_TAKE_SEM_TIMEOUT_JIFFIES))
	{
		icc_print_info("[FAIL] single channel, asynchronous send\n");
		return ICC_TEST_FAIL;
	}
	
	icc_print_info("[PASS] single channel, asynchronous send\n");
	return ICC_TEST_PASS;	
}

/* icc同步发送基本测试 */
#ifdef ICC_HAS_SYNC_SEND_FEATURE
s32 icc_send_test_002(void)
{
	/* 同步发送, 默认要用测试专用的子通道, 即33 */
	u32 channel_id = (ICC_CHN_RFILE << 16 | ICC_TEST_DEFAULT_SYNC_SUBCHN_NUM);
	u32 send_data=0x10305070;

	if (ICC_SEND_SYNC_RET == bsp_icc_send_sync(ICC_SEND_CPU, channel_id, (u8*)&send_data, sizeof(send_data), MAX_SCHEDULE_TIMEOUT))/*lint !e40 !e516*/
	{
		icc_print_info("[PASS] single channel, synchronous send!\n");
		return ICC_TEST_PASS;
	}
	icc_print_info("[FAIL] single channel, synchronous send!\n");
	return ICC_TEST_FAIL;
}
#endif

/* 多子通道异步并行发送测试 */
s32 icc_send_test_003(void)
{
	u32 i = 0;
	u32 channel_id = 0;
	u32 sub_chn_idx = 0;

	if(ICC_OK != icc_test_multi_send_wait_ack_init(ICC_TEST_MUL_SUB_CHN_SEND_FLAG))
	{
		return ICC_TEST_FAIL;
	}

	/* 编号为偶数的通道用来发送数据, 编号为奇数的通道用来给对方回确认消息 */
	for(i = ICC_TEST_CHANNEL_SIZE / 2; i < ICC_TEST_CHANNEL_SIZE / 2 + ICC_TEST_MULTI_SEND_CHN_SIZE; i++)
	{
		sub_chn_idx = i * 2 - 32;
		channel_id = ((ICC_CHN_IFC << 16) | sub_chn_idx);

		icc_print_debug("channel_id: 0x%x\n", channel_id);

		if(ICC_OK != icc_test_multi_send_test_init(i, channel_id, icc_test_multi_send_func))
		{
			return ICC_TEST_FAIL;
		}
	}

	if(osl_sem_downtimeout(&g_icc_test.multi_send_confirm_sem, 5000)) /*lint !e40 !e516 */
	{
		return ICC_TEST_FAIL;
	}

	return ICC_TEST_PASS;
}

#ifdef ICC_HAS_SYNC_SEND_FEATURE
/* 多子通道同步并行发送测试 */
s32 icc_send_test_004(void)
{
	u32 i = 0;
	u32 channel_id = 0;
	u32 sub_chn_idx = 0;

	if(ICC_OK != icc_test_multi_send_wait_ack_init(ICC_TEST_MUL_SUB_CHN_SEND_SYNC_FLAG))
	{
		return ICC_TEST_FAIL;
	}
	
	/* 编号为偶数的通道用来发送数据, 编号为奇数的通道用来给对方回确认消息 */
	for(i = ICC_TEST_CHANNEL_SIZE / 2; i < ICC_TEST_CHANNEL_SIZE / 2 + ICC_TEST_MULTI_SEND_CHN_SIZE; i++)
	{
		sub_chn_idx = i * 2 - 31;
		channel_id = ((ICC_CHN_IFC << 16) | sub_chn_idx);
		
		if(ICC_OK != icc_test_multi_send_test_init(i, channel_id, icc_test_multi_send_sync_func))
		{
			return ICC_TEST_FAIL;
		}
	}

	if(osl_sem_downtimeout(&g_icc_test.multi_send_confirm_sem, 5000)) /*lint !e40 !e516 */
	{
		return ICC_TEST_FAIL;
	}

	return ICC_TEST_PASS;
}
#endif

/* 多通道异步并行发送测试, 子通道32用来做异步发送用 */
s32 icc_send_test_005(void)
{
	u32 i = 0;
	u32 channel_id = 0;
	u32 channel_idx = 0;
	u32 sub_chn_idx = ICC_TEST_DEFAULT_ASYN_SUBCHN_NUM;

	if(ICC_OK != icc_test_multi_send_wait_ack_init(ICC_TEST_MUL_CHN_SEND_FLAG))
	{
		return ICC_TEST_FAIL;
	}

	for(i = 0; i < ICC_VALIDE_CHN_NUM; i++)
	{
		channel_idx = g_icc_init_info[i].real_channel_id;
		channel_id = ((channel_idx << 16) | sub_chn_idx);
		
		if(ICC_OK != icc_test_multi_send_test_init(i, channel_id, icc_test_multi_send_func))
		{
			return ICC_TEST_FAIL;
		}
	}

	if(osl_sem_downtimeout(&g_icc_test.multi_send_confirm_sem, 5000)) /*lint !e40 !e516 */
	{
		return ICC_TEST_FAIL;
	}

	return ICC_TEST_PASS;
}

#ifdef ICC_HAS_SYNC_SEND_FEATURE
/* 多通道同步并行发送测试, 子通道33用于同步发送用 */
s32 icc_send_test_006(void)
{
	u32 i = 0;
	u32 channel_id = 0;
	u32 channel_idx = 0;
	u32 sub_chn_idx = ICC_TEST_DEFAULT_SYNC_SUBCHN_NUM;

	if(ICC_OK != icc_test_multi_send_wait_ack_init(ICC_TEST_MULTI_CHN_SEND_SYNC_FLAG))
	{
		return ICC_TEST_FAIL;
	}
	
	for(i = 0; i < ICC_VALIDE_CHN_NUM; i++)
	{
		channel_idx = g_icc_init_info[i].real_channel_id;
		channel_id = ((channel_idx << 16) | sub_chn_idx);
		
		if(ICC_OK != icc_test_multi_send_test_init(i, channel_id, icc_test_multi_send_sync_func))
		{
			return ICC_TEST_FAIL;
		}
	}
	
	if(osl_sem_downtimeout(&g_icc_test.multi_send_confirm_sem, 5000)) /*lint !e40 !e516 */
	{
		return ICC_TEST_FAIL;
	}

	return ICC_TEST_PASS;
}
#endif

/* 边界测试: icc数据包从1到fifo大小的发送测试 */
s32 icc_send_test_007(void)
{
	u32 i = 0;
	u32 j = 0;
	u32 fifo_size = g_icc_ctrl.channels[ICC_CHN_RFILE]->fifo_recv->size;
	u32 turn_back_cnt = 3;
	u32 channel_idx = ICC_CHN_RFILE;
	u32 sub_chn_idx = ICC_TEST_DEFAULT_ASYN_SUBCHN_NUM;
	u32 channel_id = (channel_idx << 16 | sub_chn_idx);

	fifo_size -= sizeof(struct icc_channel_packet);
	for(i = 1; i <= fifo_size; i++)
	{
		for(j = 0; j < fifo_size * turn_back_cnt / i + 1; j++)
		{
			if(ICC_OK != icc_send_test(ICC_SEND_CPU, channel_id,
						 g_icc_test.channels[channel_idx].sub_channels[sub_chn_idx].wr_buf, i,0, ICC_TAKE_SEM_TIMEOUT_JIFFIES))
			{
				icc_print_info("[FAIL] single channel, asynchronous send\n");
				return ICC_TEST_FAIL;
			}
		}
	}
	
	icc_print_info("[PASS] single channel, asynchronous send\n");
	return ICC_TEST_PASS;	
}

/* icc异步发送基本测试, 发空数据 */
s32 icc_send_test_008(void)
{
	/* 异步发送, 默认要用测试专用的子通道, 即32 */
	u32 channel_idx = ICC_CHN_CSHELL;
	u32 sub_chn_idx = 0;
	u32 channel_id = (channel_idx << 16 | sub_chn_idx);

	if(ICC_OK != icc_send_test(ICC_SEND_CPU, channel_id, g_icc_test.channels[channel_idx].sub_channels[sub_chn_idx].wr_buf,
				 0, 0, ICC_TAKE_SEM_TIMEOUT_JIFFIES))
	{
		icc_print_info("[FAIL] single channel, asynchronous send\n");
		return ICC_TEST_FAIL;
	}
	
	icc_print_info("[PASS] single channel, asynchronous send\n");
	return ICC_TEST_PASS;	
}

/* icc同步发送基本测试，发空数据 */
#ifdef ICC_HAS_SYNC_SEND_FEATURE
s32 icc_send_test_009(void)
{
	u32 channel_id = (ICC_CHN_CSHELL << 16 | 2);
	u8  send_data  = 0x0;

	if (ICC_SEND_SYNC_RET == bsp_icc_send_sync(ICC_SEND_CPU, channel_id, &send_data, 0, MAX_SCHEDULE_TIMEOUT))/*lint !e40 !e516*/
	{
		icc_print_info("[PASS] single channel, synchronous send!\n");
		return ICC_TEST_PASS;
	}
	icc_print_info("[FAIL] single channel, synchronous send!\n");
	return ICC_TEST_FAIL;
}
#endif

/* icc异步发送基本测试, 发非空数据,不在icc任务而在自己的任务中调用 */
s32 icc_send_test_010(void)
{
	/* 异步发送, 默认要用测试专用的子通道, 即32 */
	u32 channel_idx = ICC_CHN_CSHELL;
	u32 sub_chn_idx = 2;
	u32 channel_id = (channel_idx << 16 | sub_chn_idx);

	if(ICC_OK != icc_send_test(ICC_SEND_CPU, channel_id, g_icc_test.channels[channel_idx].sub_channels[sub_chn_idx].wr_buf,
				 ICC_NEW_DATA_LEN, ICC_NEW_START_DATA, ICC_TAKE_SEM_TIMEOUT_JIFFIES))
	{
		icc_print_info("[FAIL] single channel, asynchronous send\n");
		return ICC_TEST_FAIL;
	}
	
	icc_print_info("[PASS] single channel, asynchronous send\n");
	return ICC_TEST_PASS;	
}

/* 通道去初始化 */
s32 bsp_icc_test_uninit(void)
{
	u32 i = 0;
	u32 channel_idx = 0;
	struct icc_channel *channel = NULL;
	struct icc_test_sub_channel *sub_channel = NULL;

	for(i = 0; i < ICC_VALIDE_CHN_NUM + 1; i++)
	{
		channel_idx = g_icc_init_info[i].real_channel_id;
		channel = g_icc_ctrl.channels[channel_idx];
		sub_channel =  &(g_icc_test.channels[i].sub_channels[0]);
		
		icc_safe_free(channel->rector);
		channel->func_size = g_icc_test.channels[channel_idx].func_size_bak;
		channel->rector = g_icc_test.channels[i].vec_bak;
		channel->fifo_recv->size = channel->fifo_send->size = g_icc_test.channels[i].fifo_size_bak;

		/* 分配的时候，子通道0的为buffer的起始地址 */
		icc_safe_free(sub_channel->wr_buf);
	}

	osl_sem_up(&g_icc_test.recv_task_sem);
	g_icc_test.recv_task_flag = 0;
	osl_sema_delete(&g_icc_test.recv_task_sem);

	return ICC_OK;
}

void icc_msg_queue_test(u32 channel_id, u32 len)
{
	struct icc_uni_msg_info msg = {0};

	memset((void *)&msg, 0, sizeof(struct icc_uni_msg_info));
	msg.channel_id     = channel_id;
	msg.len            = len;
	msg.duration_prev  = bsp_get_slice_value();
	msg.send_task_id = icc_taskid_get();
	msg.recv_task_id = 0;

	icc_msg_queue_in(&(g_icc_dbg.msg_stat.send), &msg);

	return;
}

void icc_dump_test(void)
{
	struct icc_uni_msg_info *msg = NULL;

	msg->channel_id = 0; /*lint !e413 */

	return;
}

#endif /* 原acore/core测试代码结束 */

/* 新增mcore后, 三核共用的测试代码开始 2012/06/13*/
s32 bsp_icc_mcore_test_init(void)
{
	u32 mcoore_test_channel_size = 0;
	u32 channel_idx[2]={0};
	s32 ret = ICC_OK;
	u32 i = 0;
	
	if(ICC_TEST_CASE_INITILIZED == g_icc_test.mcore_test_state)
	{
		icc_print_error("icc mcore test has initilized\n");
		return ICC_OK;
	}

	icc_print_debug("icc mcore test initilizing\n");

#ifdef __KERNEL__
	mcoore_test_channel_size = 1;
	channel_idx[0] = ICC_CHN_MCORE_ACORE;
#elif defined( __VXWORKS__)
	mcoore_test_channel_size = 1;
	channel_idx[0] = ICC_CHN_MCORE_CCORE;
#elif defined(__CMSIS_RTOS) /* rtx(cm3 os) */
	mcoore_test_channel_size = 2;
	channel_idx[0] = ICC_CHN_MCORE_CCORE;
	channel_idx[1] = ICC_CHN_MCORE_ACORE;
#endif

	for(i = 0; i < mcoore_test_channel_size; i++)
	{
		/* 20个子通道, 现有16个子通道的基础上再分配4个 */
		ret = icc_test_channel_reinit(channel_idx[i], ICC_TEST_MCORE_CHN_SIZE);
		if(ICC_OK != ret)
		{
			icc_print_error("icc_test_channel_reinit error\n");
			return ICC_ERR;
		}

		/* 使用后4个子通道进行测试 */
		ret = icc_test_sub_chn_init(channel_idx[i], ICC_TEST_USE_CHN_SIZE, ICC_TEST_MCORE_START_CHN);
		if(ICC_OK != ret)
		{
			icc_print_error("icc_test_sub_chn_init error\n");
			return ICC_ERR;
		}
	}
	
#ifdef __CMSIS_RTOS
		g_icc_test.channels[ICC_CHN_MCORE_CCORE].sub_channels[16].confirm_sem = osl_sem_init(1, confirm_sem01);/*lint !e416 !e831*/
		g_icc_test.channels[ICC_CHN_MCORE_CCORE].sub_channels[18].confirm_sem = osl_sem_init(1, confirm_sem02);/*lint !e416 !e831*/
		g_icc_test.channels[ICC_CHN_MCORE_ACORE].sub_channels[16].confirm_sem = osl_sem_init(1, confirm_sem03);/*lint !e416 !e831*/
		g_icc_test.channels[ICC_CHN_MCORE_ACORE].sub_channels[18].confirm_sem = osl_sem_init(1, confirm_sem04);/*lint !e416 !e831*/

		osl_sem_down(&(g_icc_test.channels[ICC_CHN_MCORE_CCORE].sub_channels[16].confirm_sem));/*lint !e416 !e831*/
		osl_sem_down(&(g_icc_test.channels[ICC_CHN_MCORE_CCORE].sub_channels[18].confirm_sem));/*lint !e416 !e831*/
		osl_sem_down(&(g_icc_test.channels[ICC_CHN_MCORE_ACORE].sub_channels[16].confirm_sem));/*lint !e416 !e831*/
		osl_sem_down(&(g_icc_test.channels[ICC_CHN_MCORE_ACORE].sub_channels[18].confirm_sem));/*lint !e416 !e831*/
#endif

#ifdef ICC_HAS_DEBUG_FEATURE
	/* 测试打开打印开关 */
	icc_dbg_print_sw(0);
#endif

	/* 状态最后更新 */
	g_icc_test.mcore_test_state = ICC_TEST_CASE_INITILIZED;

	return ICC_OK;
}

/* acore与mcore消息异步互发*/
s32 icc_mcore_send_test_001(void)
{
	/* 异步发送, 默认要用测试专用的子通道, 即16 */
#if defined(__KERNEL__) || defined(__CMSIS_RTOS)
	u32 channel_idx = ICC_CHN_MCORE_ACORE;
	u32 sub_chn_idx = ICC_TEST_DEFAULT_ASYN_SUBCHN_NUM / 2;
	u32 channel_id = (channel_idx << 16 | sub_chn_idx);
	u8 *wr_buff =  g_icc_test.channels[channel_idx].sub_channels[sub_chn_idx].wr_buf;/*lint !e416 !e831*/
#endif

/* acore发给mcore */
#if defined(__KERNEL__)
	if(ICC_OK != icc_send_test(ICC_CPU_MCU, channel_id, wr_buff , 10, sub_chn_idx, ICC_TAKE_SEM_TIMEOUT_JIFFIES))
	{
		icc_print_info("[FAIL] single channel, asynchronous send to cm3\n");
		return ICC_TEST_FAIL;
	}

/* mcore发给acore */
#elif defined(__CMSIS_RTOS)
	if(ICC_OK != icc_send_test(ICC_CPU_APP, channel_id, wr_buff , 10, sub_chn_idx, ICC_TAKE_SEM_TIMEOUT_MS))
	{
		icc_print_info("[FAIL] single channel, asynchronous send to acore\n");
		return ICC_TEST_FAIL;
	}
#endif
	icc_print_info("[PASS] single channel, asynchronous send\n");
	return ICC_TEST_PASS;	
}

/* ccore与mcore消息异步互发*/
s32 icc_mcore_send_test_002(void)
{
	/* 异步发送, 默认要用测试专用的子通道, 即16 */
#if defined(__VXWORKS__) || defined(__CMSIS_RTOS)
	u32 channel_idx = ICC_CHN_MCORE_CCORE;
	u32 sub_chn_idx = ICC_TEST_DEFAULT_ASYN_SUBCHN_NUM / 2;
	u32 channel_id = (channel_idx << 16 | sub_chn_idx);
	u8 *wr_buff = g_icc_test.channels[channel_idx].sub_channels[sub_chn_idx].wr_buf; /*lint !e416 !e831*/
#endif

/* ccore发给mcore */
#if defined(__VXWORKS__)
	if(ICC_OK != icc_send_test(ICC_CPU_MCU, channel_id, wr_buff , 10, sub_chn_idx, ICC_TAKE_SEM_TIMEOUT_JIFFIES))
	{
		icc_print_info("[FAIL] single channel, asynchronous send to cm3\n");
		return ICC_TEST_FAIL;
	}

/* mcore发给ccore */
#elif defined(__CMSIS_RTOS)
	if(ICC_OK != icc_send_test(ICC_CPU_MODEM, channel_id, wr_buff , 10, sub_chn_idx, ICC_TAKE_SEM_TIMEOUT_MS))
	{
		icc_print_info("[FAIL] single channel, asynchronous send to ccore\n");
		return ICC_TEST_FAIL;
	}

#endif
	icc_print_info("[PASS] single channel, asynchronous send\n");
	return ICC_TEST_PASS;	
}

s32 icc_test_stub(void)
{
	return 0;
}

#ifdef __cplusplus /* __cplusplus */
}
#endif             /* __cplusplus */
