
#ifdef __cplusplus
extern "C"
{
#endif

/*lint --e{537} */
#include "icc_balong.h"

struct icc_dbg g_icc_dbg ={0};

#define ICC_MSG_QUEUE_OK      (0)
#define ICC_MSG_QUEUE_FULL    (-1)
#define ICC_MSG_QUEUE_EMPTY   (-2)

#ifdef ICC_HAS_DEBUG_FEATURE

extern struct icc_control g_icc_ctrl;
extern struct icc_init_info g_icc_init_info[];

void icc_help(void);

static void channel_fifo_dump(u32 real_channel_id, u32 fifo_type, const char* fifo_name)
{
	u32 base_addr = 0;

	struct icc_channel* channel = g_icc_ctrl.channels[real_channel_id%g_icc_ctrl.channel_size];
	struct icc_channel_fifo* fifo = NULL;

	if(!channel)
	{
		icc_print_error("channel pointer is NULL!\n");
		return;
	}
	
	fifo = (0 == fifo_type)? channel->fifo_recv: channel->fifo_send;
	if(!fifo)
	{
		icc_print_error("fifo pointer is NULL!\n");
		return;
	}
	
	base_addr = (u32)fifo + sizeof(struct icc_channel_fifo);

	icc_print_info("***************************%s**********************************\n", fifo_name);
	icc_print_info("icc_channel_fifo address:      0x%x\n", (u32)fifo);
	icc_print_info("icc_channel_fifo.base_addr:    0x%x\n", base_addr);
	icc_print_info("icc_channel_fifo.magic:        0x%x\n", fifo->magic);
	icc_print_info("icc_channel_fifo.size:         0x%x\n", fifo->size);
	icc_print_info("icc_channel_fifo.write:        0x%x\n", fifo->write);
	icc_print_info("icc_channel_fifo.read:         0x%x\n", fifo->read);
	icc_print_info("icc_channel_fifo.data:         0x%x\n\n", *((s32*)(fifo->data)));
}

void icc_control_dump(void)
{
	u32 i = 0;

	icc_print_info("***********************************************************************\n");
	icc_print_info("icc_control.cpuid:                  0x%x\n", g_icc_ctrl.cpu_id);
	icc_print_info("icc_control.state:                  0x%x\n", g_icc_ctrl.state);
	icc_print_info("icc_control.shared_task_id:         0x%x\n", g_icc_ctrl.shared_task_id);
	icc_print_info("icc_control.shared_recv_ipc_irq_id: 0x%x\n", g_icc_ctrl.shared_recv_ipc_irq_id);
	for(i = 0; i < ICC_CHN_ID_MAX; i++)
	{
		if(i < 10)
		{
			icc_print_info("icc_control.channels[%d]:            0x%x\n", i, (s32)g_icc_ctrl.channels[i]);
		}
		else
		{
			icc_print_info("icc_control.channels[%d]:           0x%x\n", i, (s32)g_icc_ctrl.channels[i]);
		}
	}
	icc_print_info("icc_control.channel_size:           0x%x\n", g_icc_ctrl.channel_size);
	icc_print_info("***********************************************************************\n");
}

void icc_channel_dump(u32 real_channel_id)
{
	struct icc_channel* channel = g_icc_ctrl.channels[real_channel_id%g_icc_ctrl.channel_size];
	u32 i = 0;

	if(!channel)
	{
		icc_print_error("channel pointer is NULL!\n");
		return;
	}

	icc_print_info("***********************************************************************\n");
	icc_print_info("icc_channel address:                  0x%x\n", (u32)channel);
	icc_print_info("icc_channel.id:                       0x%x\n", channel->id);
	icc_print_info("icc_channel.state:                    0x%x\n", channel->state);
	icc_print_info("icc_channel.mode:                     0x%x\n", channel->mode.val);
	icc_print_info("icc_channel.task_id:                  0x%x\n", channel->private_task_id);
	icc_print_info("icc_channel.ipc_send_irq_id:          0x%x\n", channel->ipc_send_irq_id);
	icc_print_info("icc_channel.ipc_recv_irq_id:          0x%x\n", channel->ipc_recv_irq_id);
	icc_print_info("icc_channel.fifo_recv:                0x%x\n", (s32)channel->fifo_recv);
	icc_print_info("icc_channel.fifo_send:                0x%x\n", (s32)channel->fifo_send);
	for(i = 0; i < channel->func_size; i++)
	{
		if(i < 10)
		{
			icc_print_info("icc_channel.channel->rector[%d]:       0x%x\n", i, (s32)&channel->rector[i]);
		}
		else
		{
			icc_print_info("icc_channel.channel->rector[%d]:      0x%x\n", i, (s32)&channel->rector[i]);
		}
	}
	icc_print_info("icc_channel.func_size:                0x%x\n", channel->func_size);
	icc_print_info("icc_channel.seq_num_recv              0x%x\n", channel->seq_num_recv);
	icc_print_info("icc_channel.seq_num_send:             0x%x\n", channel->seq_num_send);
	icc_print_info("***********************************************************************\n");
}

void icc_channel_fifo_dump(u32 real_channel_id)
{
	channel_fifo_dump(real_channel_id, 0, "recv fifo");
	channel_fifo_dump(real_channel_id, 1, "send fifo");
	icc_print_info("**********************************************************************\n");
}

void icc_channel_vector_dump(u32 channel_id, u32 func_id)
{
	struct icc_channel_vector* vector = NULL;
	channel_id %= g_icc_ctrl.channel_size;
	func_id    %= g_icc_ctrl.channels[channel_id]->func_size;
	vector = &(g_icc_ctrl.channels[channel_id]->rector[func_id]);

	if(!vector)
	{
		icc_print_error("vector pointer is NULL!\n");
		return;
	}
	icc_print_info("***********************************************************************\n");
	icc_print_info("icc_channel_vector base addr:      0x%x\n", (s32)vector);
	icc_print_info("icc_channel_vector.read_cb:        0x%x\n", (s32)vector->read_cb);
	icc_print_info("icc_channel_vector.read_context:   0x%x\n", (s32)vector->read_context);
	icc_print_info("icc_channel_vector.write_cb:       0x%x\n", (s32)vector->write_cb);
	icc_print_info("icc_channel_vector.write_context:  0x%x\n", (s32)vector->write_context);
	icc_print_info("icc_channel_vector.return_data:    0x%x\n", vector->return_data);
	icc_print_info("***********************************************************************\n");
}

void icc_channel_packet_dump(struct icc_channel_packet *packet)
{
	if ((0 == g_icc_dbg.msg_print_sw) && (0 == g_icc_ctrl.wake_up_flag))
	{
		return;
	}

	if (1 == g_icc_ctrl.wake_up_flag)
	{
		g_icc_ctrl.wake_up_flag = 0;
		icc_print_info("[C SR]icc recv msg dump\n");
	}

	if(!packet)
	{
		icc_print_error("packet pointer is NULL!\n");
		return;
	}

	icc_print_info("***********************************************************************\n");
	icc_print_info("icc_channel_packet.channel_id:     0x%x\n", packet->channel_id);
	icc_print_info("icc_channel_packet.len:            0x%x\n", packet->len);
	icc_print_info("icc_channel_packet.src_cpu_id:     0x%x\n", packet->src_cpu_id);
	icc_print_info("icc_channel_packet.timestamp:      0x%x\n", packet->timestamp);
	icc_print_info("icc_channel_packet.task_id:        0x%x\n", packet->task_id);
	icc_print_info("***********************************************************************\n");
}

void icc_init_info_dump(u32 real_channel_id)
{
	struct icc_init_info* info = &(g_icc_init_info[real_channel_id%g_icc_ctrl.channel_size]);

	if(!info)
	{
		icc_print_error("init_info pointer is NULL!\n");
		return;
	}
	icc_print_info("***********************************************************************\n");
	icc_print_info("icc_init_fifo.real_channel_id:     0x%x\n", info->real_channel_id);
	icc_print_info("icc_init_fifo.mode:                0x%x\n", info->mode);
	icc_print_info("icc_init_fifo.send_addr:           0x%x\n", info->send_addr);
	icc_print_info("icc_init_fifo.fifo_size:           0x%x\n", info->fifo_size);
	icc_print_info("icc_init_fifo.recv_addr:           0x%x\n", info->recv_addr);
	icc_print_info("icc_init_fifo.func_size:           0x%x\n", info->func_size);
	icc_print_info("icc_init_fifo.ipc_send_irq_id:     0x%x\n", info->ipc_send_irq_id);
	icc_print_info("icc_init_fifo.ipc_recv_irq_id:     0x%x\n", info->ipc_recv_irq_id);
	icc_print_info("***********************************************************************\n");
}

void icc_dbg_print_sw(u32 sw)
{
	g_icc_dbg.msg_print_sw = sw;
}

void icc_dbg_info_print(const char *fifo_name, u32 channel_id, u8 *data, u32 data_len)
{
	if(g_icc_dbg.msg_print_sw)
	{
		icc_print_error("%s: channel[0x%x], msg[0x%x], len[%d]:\n", fifo_name, channel_id, data, data_len);
	}
}

#ifdef __VXWORKS__
extern s32  intCnt;
#endif

/* 如果在中断中，则返回0 */
u32 icc_taskid_get(void)
{
	
#ifdef __KERNEL__
	if (in_interrupt()) /*lint !e737 */
	{
		return 0;
	}
	else
	{
		icc_print_debug("%s\n", current->comm);
		return (u32)(current->pid);
	}

#elif defined(__VXWORKS__)
	if (intCnt)
	{
		return 0;
	}
	else
	{
		icc_print_debug("%s\n", taskName(taskIdSelf()));
		return (u32)(taskIdSelf());
	}

#elif defined(__CMSIS_RTOS)
	if (in_interrupt())
	{
		return 0;
	}
	else
	{
		osThreadId task = osThreadGetId();
		return (u32)(task->task_id);
	}

#endif
}

void icc_msg_queue_init(struct icc_msg_fifo *queue)
{
	queue->front = queue->rear = 0;
	queue->size = 0;
}

void icc_msg_queue_in(struct icc_msg_fifo *queue, struct icc_uni_msg_info *msg)
{
	/* when full, abandon old message */
	if((queue->front == queue->rear) && (queue->size == ICC_STAT_MSG_NUM))
	{
		queue->front = (queue->front + 1) % ICC_STAT_MSG_NUM;
		queue->size = queue->size - 1;
	}

	memcpy((void *)(&queue->msg[queue->rear]), (void *)msg, sizeof(struct icc_uni_msg_info));
	queue->rear = (queue->rear + 1) % ICC_STAT_MSG_NUM;
	queue->size = queue->size + 1;
}

void icc_msg_dump(struct icc_msg_fifo *queue, u32 start, u32 end)
{
	u32 i = 0;
	for(i = start; i < end; i++)
	{
		icc_print_info("[C SR]*******the latest [%d] msg*******\n", i-start);
		icc_print_info("channel_id  : 0x%x\n", queue->msg[i].channel_id);
		icc_print_info("send_task_id: 0x%x\n", queue->msg[i].send_task_id);
		icc_print_info("recv_task_id: 0x%x\n", queue->msg[i].recv_task_id);
		icc_print_info("length      : 0x%x\n", queue->msg[i].len);
		icc_print_info("pos_in_fifo : 0x%x\n", queue->msg[i].pos);
	}

}

void icc_msg_record_show(u32 msg_type, u32 msg_num)
{
	u32 part1 = 0;
	u32 part2 = 0;
	struct icc_msg_fifo *msg_queue = NULL;

	msg_num = (msg_num - 1) % ICC_STAT_MSG_NUM  + 1;
	msg_queue = msg_type? &(g_icc_dbg.msg_stat.send): &(g_icc_dbg.msg_stat.recv);

	/* get part1: from queue front to queue end(queue is always full) */
	part1 = ICC_MIN(msg_num, (ICC_STAT_MSG_NUM - msg_queue->front));
	icc_msg_dump(msg_queue, msg_queue->front, msg_queue->front + part1);

	/* get part2(if left): from queue start to queue rear */
	part2 = ICC_MIN((msg_num - part1), msg_queue->rear);
	icc_msg_dump(msg_queue, 0, part2);
}

/* msg_type: 0, recv; 1, send */
void icc_channel_info_show(u32 msg_type, u32 real_channel_id)
{
	struct icc_uni_channel_info *channel = NULL;
	channel = msg_type? &(g_icc_dbg.channel_stat[real_channel_id]->send): &(g_icc_dbg.channel_stat[real_channel_id]->recv);

	icc_print_info("******************channel[0x%x]****************\n", real_channel_id);
	icc_print_info("msg_len     : 0x%x\n", channel->total.sum_len);
	icc_print_info("msg_num     : 0x%x\n", channel->total.sum_num);
	icc_print_info("msg_num_prev: 0x%x\n", channel->total.sum_num_prev);
	icc_print_info("func_size   : 0x%x\n", channel->func_size);
	channel->total.sum_num_prev = channel->total.sum_num;
}

void icc_sub_channel_info_show(u32 msg_type, u32 real_channel_id, u32 func_id)
{
	struct icc_uni_channel_info *channel = NULL;
	channel = msg_type? &(g_icc_dbg.channel_stat[real_channel_id]->send): &(g_icc_dbg.channel_stat[real_channel_id]->recv);
	func_id %= channel->func_size;

	icc_print_info("***********sub channel[0x%x]***********\n", func_id);
	icc_print_info("msg_len     : 0x%x\n", channel->sub_chn[func_id].sum_len);
	icc_print_info("msg_num     : 0x%x\n", channel->sub_chn[func_id].sum_num);
	icc_print_info("sum_num_prev: 0x%x\n", channel->sub_chn[func_id].sum_num_prev);
	channel->sub_chn[func_id].sum_num_prev = channel->sub_chn[func_id].sum_num;
}

void icc_sub_channel_info_show_all(u32 msg_type, u32 real_channel_id)
{
	struct icc_uni_channel_info *channel = NULL;
	u32 i = 0;
	channel = msg_type? &(g_icc_dbg.channel_stat[real_channel_id]->send): &(g_icc_dbg.channel_stat[real_channel_id]->recv);

	for(i = 0; i < channel->func_size; i++)
	{
		if(channel->sub_chn[i].sum_num_prev != channel->sub_chn[i].sum_num)
		{
			icc_print_info("***********sub channel[0x%x]***********\n", i);
			icc_print_info("msg_len     : 0x%x\n", channel->sub_chn[i].sum_len);
			icc_print_info("msg_num     : 0x%x\n", channel->sub_chn[i].sum_num);
			icc_print_info("sum_num_prev: 0x%x\n", channel->sub_chn[i].sum_num_prev);
		}
		channel->sub_chn[i].sum_num_prev = channel->sub_chn[i].sum_num;
	}
}

void icc_recv_ipc_int_show(void)
{
	icc_print_info("*******icc收到的ipc中断统计*******\n");
	icc_print_info("ipc_int_cnt: %d\n", g_icc_dbg.ipc_int_cnt);
}

void icc_errno_show(void)
{
	icc_print_info("******************icc错误码信息******************\n");
	icc_print_info("0x%x: channel init error\n",ICC_CHN_INIT_FAIL);
	icc_print_info("0x%x: malloc channel memory fail\n",ICC_MALLOC_CHANNEL_FAIL);
	icc_print_info("0x%x: malloc rector memory fail\n",ICC_MALLOC_VECTOR_FAIL);
	icc_print_info("0x%x: create task fail\n",ICC_CREATE_TASK_FAIL);
	icc_print_info("0x%x: debug init error\n",ICC_DEBUG_INIT_FAIL);
	icc_print_info("0x%x: create semaphore fail\n",ICC_CREATE_SEM_FAIL);
	icc_print_info("0x%x: register ipc int fail\n",ICC_REGISTER_INT_FAIL);
	icc_print_info("0x%x: interface parameter error\n",ICC_INVALID_PARA);
	icc_print_info("0x%x: fifo is full\n",ICC_INVALID_NO_FIFO_SPACE);
	icc_print_info("0x%x: sem take timeout\n",ICC_WAIT_SEM_TIMEOUT);
	icc_print_info("0x%x: send len != expected\n",ICC_SEND_ERR);
	icc_print_info("0x%x: recv len != expected\n",ICC_RECV_ERR);
	icc_print_info("0x%x: register read callback fail\n",ICC_REGISTER_CB_FAIL);
	icc_print_info("0x%x: register dpm fail\n",ICC_REGISTER_DPM_FAIL);
	icc_print_info("0x%x: malloc memory fail\n",ICC_MALLOC_MEM_FAIL);
	icc_print_info("0x%x: null pointer\n",ICC_NULL_PTR);
}

void icc_print_level_set(u32 level)
{
    (void)bsp_mod_level_set(BSP_MODU_ICC, level);
    icc_print_info("bsp_mod_level_set(BSP_MODU_ICC=%d, level=%d)\n", BSP_MODU_ICC, level);
}

void icc_struct_dump(void)
{
	icc_print_info("***************************icc结构体信息***************************\n");
	icc_print_info("icc初始化信息            : icc_init_info_dump channel_id\n");
	icc_print_info("icc_control结构体        : icc_control_dump\n");
	icc_print_info("icc_channel结构体        : icc_channel_dump channel_id\n");
	icc_print_info("icc_channel_fifo结构体   : icc_channel_fifo_dump channel_id\n");
	icc_print_info("icc_channel_vector结构体 : icc_channel_vector_dump channel_id func_id\n");
	icc_print_info("结构体显示函数参数说明   : channel_id, 主通道号; fucn_id,子通道号\n");
}

void icc_help(void)
{
	icc_print_info("***************************************icc调试帮助信息***************************************\n");
	icc_print_info("结构体信息dump          : icc_struct_dump\n");
	icc_print_info("调试打印开关            : icc_dbg_print_sw 0|1\n");
	icc_print_info("错误码说明              : icc_errno_show\n");
	icc_print_info("收到ipc中断统计         : icc_recv_ipc_int_show\n");
	icc_print_info("最近n条收发消息显示     : icc_msg_record_show msg_type msg_num\n");
	icc_print_info("单个通道统计信息        : icc_channel_info_show msg_type channel_id\n");
	icc_print_info("所有子通道统计信息      : icc_sub_channel_info_show_all msg_type channel_id\n");
	icc_print_info("单个子通道统计信息      : icc_sub_channel_info_show msg_type channel_id func_id\n");
	icc_print_info("收发消息显示函数参数说明: msg_type, 消息类型(0: 接收; 1: 发送); msg_num, 显示消息个数(<=10)\n");
	icc_print_info("通道统计信息函数参数说明: msg_type, 同上; channel_id, 主通道号; fucn_id, 子通道号\n");
}

void icc_dump_hook(void)
{
    char *dump_buf = g_icc_dbg.dump_buf_addr;
    u32  dump_size = g_icc_dbg.dump_buf_size;

    /* do nothing, if om init fail */
    if (0 == dump_size || NULL == dump_buf)
    {
        return;
    }

	memcpy((void *)g_icc_dbg.dump_buf_addr, (void *)&g_icc_dbg.msg_stat, sizeof(struct icc_msg_info));

    return;
}

void icc_dump_init(void)
{
    /* reg the dump callback to om */
	if(bsp_dump_register_hook(ICC_DUMP_SAVE_MOD,(dump_save_hook)icc_dump_hook))
	{
	    goto err_ret;
	}

	if(bsp_dump_get_buffer(ICC_DUMP_SAVE_MOD,&g_icc_dbg.dump_buf_addr, &g_icc_dbg.dump_buf_size))
	{
	    goto err_ret;
	}

    return;

err_ret:
    g_icc_dbg.dump_buf_addr = NULL;
    g_icc_dbg.dump_buf_size = 0;
	icc_print_error("try to use dump fail!\n");
    return;
}

s32  icc_debug_init(u32 channel_num)
{
	u32 i = 0;
	struct icc_channel_info * channel = NULL;
	struct icc_channel_stat_info *sub_channel = NULL;
	struct icc_channel *icc_channel = NULL;
	
	memset(&g_icc_dbg, 0, sizeof(struct icc_dbg));

	for(i = 0; i < channel_num; i++)
	{
		/* 使用g_icc_ctrl而不是g_icc_init_info，因为测试编进去以后，统计通道不需要再处理 */
		icc_channel = g_icc_ctrl.channels[g_icc_init_info[i].real_channel_id];
		channel = (struct icc_channel_info *)osl_malloc(sizeof(struct icc_channel_info));
		if (!channel)
		{
			icc_print_error("malloc icc_channel_info memory fail!\n");
			return (s32)ICC_ERR;
		}

		/* 收发子通道一同分配 */
		sub_channel = (struct icc_channel_stat_info *)osl_malloc(sizeof(struct icc_channel_stat_info) * icc_channel->func_size * 2);
		if (!sub_channel)
		{
			osl_free((void *)channel);
			icc_print_error("malloc icc_channel_stat_info memory fail!\n");
			return (s32)ICC_ERR;
		}

		/* channel init */
		memset((void *)channel, 0, sizeof(struct icc_channel_info));
		channel->id = icc_channel->id;
		channel->recv.func_size= icc_channel->func_size;
		channel->send.func_size= icc_channel->func_size;

		/* sub channel init */
		memset((void *)sub_channel, 0, sizeof(struct icc_channel_stat_info) * icc_channel->func_size * 2); /*lint !e665 */
		channel->send.sub_chn  = sub_channel;
		channel->recv.sub_chn  = &(sub_channel[icc_channel->func_size]);
		
		g_icc_dbg.channel_stat[channel->id] = channel;
	}

	icc_dump_init();

	return ICC_OK;
}

void icc_channel_msg_stat(struct icc_channel_stat_info *channel, u32 msg_len, u32 recv_task_id)
{
	if(!channel)
	{
		return;
	}
	channel->task_id = recv_task_id;
	channel->sum_len += msg_len;
	channel->sum_num++;
}

#endif /* ICC_HAS_DEBUG_FEATURE */

#ifdef __cplusplus
}
#endif
