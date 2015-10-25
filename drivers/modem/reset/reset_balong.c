
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/kthread.h>
#include <linux/slab.h>
#include <linux/reboot.h>
#include <linux/huawei/rdr.h>
#include <linux/delay.h>
#include <linux/of.h>
#include <linux/of_address.h>
#ifdef CONFIG_HISI_RPROC
#include <linux/huawei/hisi_rproc.h>
#endif

#include <DrvInterface.h>
#include <hifidrvinterface.h>
#include <drv_nv_id.h>


#include <bsp_ipc.h>
#include <bsp_icc.h>
#include <bsp_dump.h>
#include <bsp_dsp.h>
#include <bsp_nvim.h>
#include <bsp_hardtimer.h>
#include <bsp_reset.h>
#include "load_image.h"
#include "reset_balong.h"

/*lint --e{746, 732, 516, 958, 666} */

struct modem_reset_ctrl g_modem_reset_ctrl = {0};
struct modem_reset_debug g_reset_debug = {0};

extern void ccore_ipc_enable(void);
extern void ccore_ipc_disable(void);

void reset_reboot_system(enum RESET_TYPE type)
{
	bsp_reset_bootflag_set(CCORE_BOOT_NORMAL);
	g_modem_reset_ctrl.modem_action = MODEM_NORMAL;
	hisi_system_error(HISI_RDR_MOD_CP_DO_RESET, (u32)type, g_modem_reset_ctrl.reset_cnt, NULL, 0);
}

s32 reset_prepare(enum MODEM_ACTION action)
{
	unsigned long flags = 0;
	u32 current_action = (u32)action;
	u32 global_action = g_modem_reset_ctrl.modem_action;

	if (current_action == global_action)
	{
		return RESET_OK;
	}
	else if ((u32)MODEM_NORMAL != global_action)
	{
		reset_print_err("action(%d) is doing, abundon action(%d)\n", global_action, action);
		return RESET_ERROR;
	}

	g_reset_debug.main_stage = 0;

	wake_lock(&(g_modem_reset_ctrl.wake_lock));
	reset_print_debug("(%d) wake_lock\n", ++g_reset_debug.main_stage);

	spin_lock_irqsave(&g_modem_reset_ctrl.action_lock, flags);
	g_modem_reset_ctrl.modem_action = action;
	spin_unlock_irqrestore(&g_modem_reset_ctrl.action_lock, flags);

	if ((MODEM_POWER_OFF ==  current_action) || (MODEM_RESET ==  current_action))
	{
		ccore_ipc_disable();
	}
	osl_sem_up(&(g_modem_reset_ctrl.task_sem));

	return RESET_OK;
}

extern int cshell_mode_reset_cb(DRV_RESET_CALLCBFUN_MOMENT eparam, int usrdata);
extern s32 bsp_rfile_reset_cb(DRV_RESET_CALLCBFUN_MOMENT eparam, s32 usrdata);
extern s32 bsp_icc_channel_reset(DRV_RESET_CALLCBFUN_MOMENT stage, int usrdata);
extern void ipc_modem_reset_cb(DRV_RESET_CALLCBFUN_MOMENT eparam, int usrdata);
extern s32 bsp_mem_ccore_reset_cb(DRV_RESET_CALLCBFUN_MOMENT enParam, int userdata);

s32 drv_reset_cb (DRV_RESET_CALLCBFUN_MOMENT stage, int userdata)
{
	s32 ret = RESET_ERROR;

	switch(stage)
	{
		case DRV_RESET_CALLCBFUN_RESET_BEFORE:
			ret = cshell_mode_reset_cb(stage, 0);
			if (0 != ret)
			{
				goto fail;
			}

			ret = bsp_rfile_reset_cb(stage, 0);
			if (0 != ret)
			{
				goto fail;
			}

			ret = bsp_icc_channel_reset(stage, 0);
			if (0 != ret)
			{
				goto fail;
			}
			(void)bsp_mem_ccore_reset_cb(stage, 0);

			ipc_modem_reset_cb(stage, 0);

			break;

		case DRV_RESET_CALLCBFUN_RESETING:
			ipc_modem_reset_cb(stage, 0);
			/* 在C核drv启动之后，icc需要准备好 */
			ccore_msg_switch_on(g_modem_reset_ctrl.multicore_msg_switch, CCORE_STATUS);
			break;

		case DRV_RESET_CALLCBFUN_RESET_AFTER:
			(void)bsp_mem_ccore_reset_cb(stage, 0);

			ret = bsp_icc_channel_reset(stage, 0);
			if (0 != ret)
			{
				goto fail;
			}

			ret = bsp_rfile_reset_cb(stage, 0);
			if (0 != ret)
			{
				goto fail;
			}
			ret = cshell_mode_reset_cb(stage, 0);
			if (0 != ret)
			{
				goto fail;
			}

			break;

		default:
			break;
	}

	return RESET_OK;

fail:
	reset_print_err("drv cb func run fail, err_code=0x%x\n", ret);
	return RESET_ERROR;
}

s32 invoke_reset_cb(DRV_RESET_CALLCBFUN_MOMENT stage)
{
	struct reset_cb_list *p = g_modem_reset_ctrl.list_head;
	s32 ret = RESET_ERROR;

	reset_print_debug("(%d) @reset %d\n", ++g_reset_debug.main_stage, (u32)stage);

	/*根据回调函数优先级，调用回调函数*/
    while (NULL != p)
    {
        if (NULL != p->cb_info.cbfun)
        {
			reset_print_debug("%s callback invoked\n", p->cb_info.name);
            ret = p->cb_info.cbfun(stage, p->cb_info.userdata);
            if (RESET_OK != ret)
            {
                reset_print_err("fail to run cbfunc of %s, at stage%d return %d\n", p->cb_info.name, stage, ret);
                return RESET_ERROR;
            }
        }
        p = p->next;
    }

	return RESET_OK;
}

#ifdef CONFIG_HISI_RPROC

s32 send_sync_msg_to_mcore(u32 reset_info, u32 *ack_val)
{
	s32 ret = 0;
	/*如果是modem处于复位状态，则调用AP侧IPC*/
	/*ap receive mabx 0,send mbx 13*/
	rproc_msg_t tx_buffer[2] ;
	rproc_msg_t ack_buffer[2] ;
	/*发送标志，用于LPM3上接收时解析*/
	tx_buffer[0] = (0<<24|9<<16|3<<8);
	tx_buffer[1] = reset_info;
	ret = RPROC_SYNC_SEND(HISI_RPROC_LPM3, tx_buffer, 2, SYNC_MSG, ack_buffer, 2);
	if (ret)
	{
		*ack_val = -1;
	}
	else
	{
		*ack_val = (u32)ack_buffer[1];
	}
	return ret;
}

#else
s32 send_sync_msg_to_mcore(u32 reset_info, u32 *ack_val)
{
	return 0;
}

#endif
s32 send_msg_to_hifi(DRV_RESET_CALLCBFUN_MOMENT stage)
{
	s32 ret = RESET_ERROR;
	AP_HIFI_CCPU_RESET_REQ_STRU hifi_mailbox = {0};

	reset_print_debug("(%d) stage%d,ID_AP_HIFI_CCPU_RESET_REQ=%d\n", ++g_reset_debug.main_stage, (s32)stage, (s32)ID_AP_HIFI_CCPU_RESET_REQ);

	if (DRV_RESET_CALLCBFUN_RESET_BEFORE == stage)
	{
		/*消息ID*/
		hifi_mailbox.uhwMsgId = ID_AP_HIFI_CCPU_RESET_REQ;
		ret = DRV_MAILBOX_SENDMAIL(MAILBOX_MAILCODE_ACPU_TO_HIFI_CCORE_RESET_ID, (void *)(&hifi_mailbox), sizeof(hifi_mailbox)); /*lint !e713 */
		if(MAILBOX_OK != ret)
		{
			reset_print_err("fail to send msg to hifi\n");
			return RESET_ERROR;
		}
	}

	/* 如果有必要，其他阶段也通知hifi */
	return RESET_OK;
}

void let_modem_master_in_idle(void)
{
	u32 regval = 0;

	/* nmi开关 */
	regval = readl((volatile const void *)(g_modem_reset_ctrl.crg_base + 0x12c));
	reset_print_debug("org ccore nmi regval[0x%x]=0x%x\n", (g_modem_reset_ctrl.crg_base + 0x12c), regval);
	regval &= (~((u32)1 << 12));
	writel(regval,(volatile void *)(g_modem_reset_ctrl.crg_base + 0x12c));
	regval = readl((volatile const void *)(g_modem_reset_ctrl.crg_base + 0x12c));
	reset_print_debug("(%d) ccore nmi regval: 0x%x\n", ++g_reset_debug.main_stage, regval);

	return;
}

s32 wait_for_ccore_reset_done(u32 timeout)
{
	reset_print_debug("(%d) waiting the reply from modem A9\n", ++g_reset_debug.main_stage);

	if (osl_sem_downtimeout(&(g_modem_reset_ctrl.wait_ccore_reset_ok_sem), msecs_to_jiffies(timeout)))/*lint !e713 */
	{
		reset_print_err("Get response from ccore reset timeout within %d\n", timeout);
	    return RESET_ERROR;
	}

	reset_print_debug("(%d) has received the reply from modem A9\n", ++g_reset_debug.main_stage);
	return RESET_OK;
}

void from_hifi_mailbox_readcb(void  *usr_handle, void *mail_handle, unsigned long mail_len)/*lint !e438 */
{
    unsigned long   ret = 0;
    unsigned long   msg_len = sizeof(HIFI_AP_CCPU_RESET_CNF_STRU);
    HIFI_AP_CCPU_RESET_CNF_STRU msg_hifi = {0};

    ret = DRV_MAILBOX_READMAILDATA(mail_handle, (unsigned char *)(&msg_hifi), &msg_len);
	reset_print_debug("ret=%lu, uhwMsgId=%d, uhwResult=%d\n", ret, msg_hifi.uhwMsgId, msg_hifi.uhwResult);
    if (ID_HIFI_AP_CCPU_RESET_CNF == msg_hifi.uhwMsgId && 0 == msg_hifi.uhwResult)
    {
        up(&(g_modem_reset_ctrl.wait_hifi_reply_sem));
    }
    else
    {
	    up(&(g_modem_reset_ctrl.wait_hifi_reply_sem));
        reset_print_err("unkown msg from hifi\n");
		reset_reboot_system(RESET_TYPE_RECV_HIFI_MSG_FAIL);
    }

}

void master_in_ile_timestamp_dump(void)
{
	reset_print_err("**\nmaster in idle timestamp info start**\n");
	reset_print_err("0x%8x 0x%8x 0x%8x 0x%8x\n", readl((volatile const void *)SHM_MEM_CCORE_RESET_ADDR), readl((volatile const void *)STAMP_RESET_FIQ_COUNT),
												 readl((volatile const void *)STAMP_RESET_IDLE_FAIL_COUNT), readl((volatile const void *)STAMP_RESET_MASTER_ENTER_IDLE));
	reset_print_err("0x%8x 0x%8x 0x%8x 0x%8x\n", readl((volatile const void *)STAMP_RESET_CIPHER_SOFT_RESET), readl((volatile const void *)STAMP_RESET_CIPHER_DISABLE_CHANNLE), 
												 readl((volatile const void *)STAMP_RESET_CIPHER_ENTER_IDLE), readl((volatile const void *)STAMP_RESET_EDMA_STOP_BUS));
	reset_print_err("0x%8x 0x%8x 0x%8x 0x%8x\n", readl((volatile const void *)STAMP_RESET_EDMA_ENTER_IDLE), readl((volatile const void *)STAMP_RESET_UPACC_ENTER_IDLE_1), 
												 readl((volatile const void *)STAMP_RESET_UPACC_ENTER_IDLE_2), readl((volatile const void *)STAMP_RESET_UPACC_ENTER_IDLE_3));
	reset_print_err("0x%8x 0x%8x 0x%8x 0x%8x\n", readl((volatile const void *)STAMP_RESET_CICOM0_SOFT_RESET), readl((volatile const void *)STAMP_RESET_CICOM1_SOFT_RESET), 
												 readl((volatile const void *)STAMP_RESET_IPF_SOFT_RESET), readl((volatile const void *)STAMP_RESET_IPF_ENTER_IDLE));
	reset_print_err("0x%8x 0x%8x 0x%8x 0x%8x\n", readl((volatile const void *)STAMP_RESET_BBP_DMA_ENTER_IDLE), readl((volatile const void *)STAMP_RESET_WBBP_MSTER_STOP), 
												 readl((volatile const void *)STAMP_RESET_WBBP_SLAVE_STOP), readl((volatile const void *)STAMP_RESET_WBBP_ENTER_IDLE));
	reset_print_err("0x%8x\n", readl((volatile const void *)STAMP_RESET_MASTER_IDLE_QUIT));
	reset_print_err("**\nmaster in idle timestamp info end**\n");
}

s32 do_power_off(u16 action)
{
	u32 msg = 0;
	s32 ret = RESET_ERROR;
	u32 ack_val = 0xff;

	/* 设置启动模式为C核单独复位 */
	bsp_reset_bootflag_set(CCORE_IS_REBOOT);
	g_modem_reset_ctrl.boot_mode = readl((volatile const void *)SCBAKDATA13);
	reset_print_debug("(%d) set boot mode:0x%x\n", ++g_reset_debug.main_stage, g_modem_reset_ctrl.boot_mode);

	/* 唤醒ccore */
	ret = bsp_ipc_int_send(IPC_CORE_CCORE, g_modem_reset_ctrl.ipc_send_irq_wakeup_ccore);
	if(ret != 0)
	{
		reset_print_err("wakeup ccore failt\n");
	}

	/* 复位前各组件回调 */
	ret = invoke_reset_cb(DRV_RESET_CALLCBFUN_RESET_BEFORE);
	if(ret < 0)
	{
		reset_reboot_system(RESET_TYPE_CB_INVOKE_BEFORE);
		return RESET_ERROR;
	}

	/* 阻止核间通信 */
	ccore_msg_switch_off(g_modem_reset_ctrl.multicore_msg_switch, CCORE_STATUS);
	reset_print_debug("(%d) switch off msg connect:%d\n", ++g_reset_debug.main_stage, g_modem_reset_ctrl.multicore_msg_switch);

	/* 通知hifi，停止与modem交互 */
	ret = send_msg_to_hifi(DRV_RESET_CALLCBFUN_RESET_BEFORE);
	if(ret < 0)
	{
		reset_print_err("send_msg_to_hifi=0x%x fail\n", ret);
		reset_reboot_system(RESET_TYPE_SEND_MSG2_M3_FAIL_BEFORE);
		return RESET_ERROR;
	}
	/*  等待hifi处理完成 */
	if (osl_sem_downtimeout(&(g_modem_reset_ctrl.wait_hifi_reply_sem), msecs_to_jiffies((u32)RESET_WAIT_RESP_TIMEOUT)))/*lint !e713 */
	{
		reset_print_err("waiting the reply from hifi timeout(%d)!\n", RESET_WAIT_RESP_TIMEOUT);
		reset_reboot_system(RESET_TYPE_WAIT_HIFI_REPLY_FAIL_BEFORE);
		return RESET_ERROR;
	}
	reset_print_debug("(%d) has received the reply from hifi\n", ++g_reset_debug.main_stage);

	/* 通知modem master进idle态,并等待ccore回复 */
	let_modem_master_in_idle();
	ret = osl_sem_downtimeout(&(g_modem_reset_ctrl.wait_modem_master_in_idle_sem), 
		msecs_to_jiffies(RESET_WAIT_MODEM_IN_IDLE_TIMEOUT));/*lint !e713 */
	if (ret)
	{
		reset_print_debug("(%d) let modem master in idle timeout\n", ++g_reset_debug.main_stage);
		master_in_ile_timestamp_dump();
	}
	else
	{
		reset_print_debug("(%d) let modem master in idle successfully\n", ++g_reset_debug.main_stage);
	}

	/* 通知m3进行复位前辅助处理 */
	msg = RESET_INFO_MAKEUP(action, DRV_RESET_CALLCBFUN_RESET_BEFORE); /*lint !e701 */
	ret = RESET_ERROR;
	ret = send_sync_msg_to_mcore(msg, &ack_val);
	if(ret)
	{
		reset_print_err("send_sync_msg_to_mcore(0x%x) before reset fail!\n", ret);
		reset_reboot_system(RESET_TYPE_SEND_MSG2_M3_FAIL_BEFORE);
		return RESET_ERROR;
	}
	else if(RESET_MCORE_BEFORE_RESET_OK != ack_val)
	{
		reset_print_err("bus error probed on m3, ack_val=0x%x, fail!\n", ack_val);
		reset_reboot_system(RESET_TYPE_SEND_MSG2_M3_FAIL_BEFORE);
		return RESET_ERROR;
	}
	reset_print_debug("(%d) before reset stage has communicated with lpm3 succeed\n", ++g_reset_debug.main_stage);

	return RESET_OK;
}

s32 do_power_on(u16 action)
{
	u32 msg = 0;
	s32 ret = RESET_ERROR;
	u32 ack_val = 0xff;

	/* C核和dsp镜像加载 */
	ret = load_modem_image();
	if(ret < 0)
	{
		reset_reboot_system(RESET_TYPE_LOAD_MODEM_IMG_FAIL);
		return RESET_ERROR;
	}
    
    /* 清除C核dump区域 */
    rdr_modem_reset_dumpmem();

	/* 通知m3进行A9解复位及相关处理 */
	msg = RESET_INFO_MAKEUP(action, (u32)DRV_RESET_CALLCBFUN_RESETING); /*lint !e701 */
	ret = RESET_ERROR;
	ret = send_sync_msg_to_mcore(msg, &ack_val);
	if(ret)
	{
		reset_print_err("send_sync_msg_to_mcore(0x%x) at resting stage fail!\n", ret);
		reset_reboot_system(RESET_TYPE_SEND_MSG2_M3_FAIL_RESTING);
		return RESET_ERROR;
	}
	else if(RESET_MCORE_RESETING_OK != ack_val)
	{
		reset_print_err("modem unreset fail on m3, ack_val=0x%x, fail!\n", ack_val);
		reset_reboot_system(RESET_TYPE_RECV_WRONG_MSG_FROM_M3_RESTING);
		return RESET_ERROR;
	}
	reset_print_debug("(%d) at reseting stage has communicated with lpm3 succeed\n", ++g_reset_debug.main_stage);

	/* 复位中相关处理:与C核需要交互的模块在此阶段准备好 */
	if ((MODEM_POWER_ON == action) || (MODEM_RESET == action))
	{
		ccore_ipc_enable();
	}
	ret = drv_reset_cb(DRV_RESET_CALLCBFUN_RESETING, 0);
	if(ret < 0)
	{
		reset_reboot_system(RESET_TYPE_CB_INVOKE_RESTING);
		return RESET_ERROR;
	}
	
	ret = wait_for_ccore_reset_done(RESET_WAIT_CCPU_STARTUP_TIMEOUT);
	if(ret < 0)
	{
		reset_reboot_system(RESET_TYPE_WAIT_CCORE_RELAY_TIMEOUT);
		return RESET_ERROR;
	}


	/* 复位后相关处理 */
	ret = invoke_reset_cb(DRV_RESET_CALLCBFUN_RESET_AFTER);
	if(ret < 0)
	{
		reset_reboot_system(RESET_TYPE_CB_INVOKE_AFTER);
		return RESET_ERROR;
	}

	/* 复位后通知M3进行相关处理 */
	msg = RESET_INFO_MAKEUP(action, DRV_RESET_CALLCBFUN_RESET_AFTER); /*lint !e701 */
	ret = RESET_ERROR;
	ret = send_sync_msg_to_mcore(msg, &ack_val);
	if(ret)
	{
		reset_print_err("send_sync_msg_to_mcore(0x%x) after reset stage fail!\n", ret);
		reset_reboot_system(RESET_TYPE_SEND_MSG2_M3_FAIL_AFTER);
		return RESET_ERROR;
	}
	else if(RESET_MCORE_AFTER_RESET_OK != ack_val)
	{
		reset_print_err("after reset handle failed on m3, ack_val=0x%x, fail!\n", ack_val);
		reset_reboot_system(RESET_TYPE_RECV_WRONG_MSG_FROM_M3_AFTER);
		return RESET_ERROR;
	}
	reset_print_debug("(%d) after reset stage has communicated with lpm3 succeed\n", ++g_reset_debug.main_stage);

	/* 将启动模式置回普通模式 */
	bsp_reset_bootflag_set(CCORE_BOOT_NORMAL);

	return RESET_OK;
}

s32 do_reset(u16 action)
{
	s32 ret = RESET_ERROR;

	ret = do_power_off(action);
	if(ret < 0)
	{
		return RESET_ERROR;
	}

	ret = do_power_on(action);
	if(ret < 0)
	{
		return RESET_ERROR;
	}

	return RESET_OK;
}

int modem_reset_task(void *arg)
{
	u16 action = 0;
	unsigned long flags = 0;

	for( ; ;)
	{
		osl_sem_down(&(g_modem_reset_ctrl.task_sem));
		action = (u16)g_modem_reset_ctrl.modem_action;
		reset_print_debug("(%d)has taken task_sem, action=%d\n", ++g_reset_debug.main_stage, action);

		if (MODEM_POWER_OFF == action)
		{
			(void)do_power_off(action);
		}
		else if (MODEM_POWER_ON == action)
		{
			(void)do_power_on(action);
		}
		else if (MODEM_RESET == action)
		{
			(void)do_reset(action);
			reset_print_err("reset count: %d\n", ++g_modem_reset_ctrl.reset_cnt);
		}
		if (action == g_modem_reset_ctrl.modem_action)
		{
			spin_lock_irqsave(&g_modem_reset_ctrl.action_lock, flags);
			g_modem_reset_ctrl.modem_action = MODEM_NORMAL;
			spin_unlock_irqrestore(&g_modem_reset_ctrl.action_lock, flags);
		}
		wake_unlock(&(g_modem_reset_ctrl.wake_lock));
		
		g_modem_reset_ctrl.exec_time = get_timer_slice_delta(g_modem_reset_ctrl.exec_time, bsp_get_slice_value());
		reset_print_debug("execute done, elapse time %d\n", g_modem_reset_ctrl.exec_time);
	}

}

struct reset_cb_list *sort_list_insert(struct reset_cb_list *list, struct reset_cb_list *node)
{
	struct reset_cb_list *head = list;
    struct reset_cb_list *curr = list;
    struct reset_cb_list *tail = list;

    if (NULL == list || NULL == node)
    {
        return NULL;
    }
    while (NULL != curr)
    {
        /* 由小到大, 按优先级插入 */
        if (curr->cb_info.priolevel > node->cb_info.priolevel)
        {
            if (head == curr)
            {
                node->next = curr;
                head = node;
            }
            else
            {
                tail->next = node;
                node->next = curr;
            }
            break;
        }
        tail = curr;
        curr = curr->next;
    }
    if (NULL == curr)
    {
        tail->next = node;
    }
    return head;
}

struct reset_cb_list *do_cb_func_register(struct reset_cb_list * list_head, const char* func_name, pdrv_reset_cbfun func, int user_data, int prior)
{
    struct reset_cb_list *cb_func_node = NULL;
	u32 name_len = 0;

    if (!func_name || !func || (prior < RESET_CBFUNC_PRIO_LEVEL_LOWT || prior > RESET_CBFUNC_PRIO_LEVEL_HIGH))
    {
        reset_print_err("register fail, name:%s, cbfun=0x%x, prio=%d\n", func_name, (unsigned int)func, prior);
        return list_head;
    }

    cb_func_node = (struct reset_cb_list *)kmalloc(sizeof(struct reset_cb_list), GFP_KERNEL);
    if (cb_func_node)
    {
		name_len = (u32)min((u32)DRV_RESET_MODULE_NAME_LEN, (u32)strlen(func_name));
		memset((void*)cb_func_node, 0, (sizeof(struct reset_cb_list)));
		memcpy((void*)cb_func_node->cb_info.name, (void*)func_name, (int)name_len);
		cb_func_node->cb_info.priolevel = prior;
		cb_func_node->cb_info.userdata = user_data;
		cb_func_node->cb_info.cbfun = func;
    }

	if (!list_head)
    {
        list_head = cb_func_node;
    }
	else
    {
        list_head = sort_list_insert(list_head, cb_func_node);
    }
	++g_modem_reset_ctrl.list_use_cnt;

    return list_head;
}

void modem_reset_do_work(struct work_struct *work)
{
	reset_print_debug("\n");
	g_modem_reset_ctrl.exec_time = bsp_get_slice_value();
	if (1 == g_modem_reset_ctrl.in_suspend_state)
	{
		if (!wait_for_completion_timeout(&(g_modem_reset_ctrl.suspend_completion), HZ*10))
		{
			machine_restart("system halt"); /* 调systemError */
			return;
		}
	}
	reset_prepare(MODEM_RESET);
}

void modem_power_off_do_work(struct work_struct *work)
{
	reset_print_debug("\n");
	if (1 == g_modem_reset_ctrl.in_suspend_state)
	{
		if (!wait_for_completion_timeout(&(g_modem_reset_ctrl.suspend_completion), HZ*10))
		{
			machine_restart("system halt"); /* todo: 是否需要调用system_error */
			return;
		}
	}
	reset_prepare(MODEM_POWER_OFF);
}

s32 reset_pm_notify(struct notifier_block *notify_block,
                    unsigned long mode, void *unused)
{
    switch (mode) 
	{
    case PM_HIBERNATION_PREPARE:
    case PM_SUSPEND_PREPARE:
        g_modem_reset_ctrl.in_suspend_state = 1;
        break;

    case PM_POST_SUSPEND:
    case PM_POST_HIBERNATION:
    case PM_POST_RESTORE:
        complete(&(g_modem_reset_ctrl.suspend_completion));
        g_modem_reset_ctrl.in_suspend_state = 0;
        break;
	default:
		break;
    }

    return 0;
}

void reset_feature_on(u32 sw)
{
	g_modem_reset_ctrl.nv_config.is_feature_on = (sw? 1: 0);
}

void reset_ril_on(u32 sw)
{
	g_modem_reset_ctrl.nv_config.is_connected_ril = (sw? 1: 0);
}

void reset_ctrl_debug_show(void)
{
	reset_print_err("in_suspend_state: 0x%x\n", g_modem_reset_ctrl.in_suspend_state);
	reset_print_err("reset_cnt       : 0x%x\n", g_modem_reset_ctrl.reset_cnt);
	reset_print_err("nv.feature_on   : 0x%x\n", g_modem_reset_ctrl.nv_config.is_feature_on);
	reset_print_err("nv.ril_flag     : 0x%x\n", g_modem_reset_ctrl.nv_config.is_connected_ril);
	reset_print_err("state           : 0x%x\n", g_modem_reset_ctrl.state);
}

void modem_power_on_do_work(struct work_struct *work)
{
	reset_print_debug("\n");
	if (1 == g_modem_reset_ctrl.in_suspend_state)
	{
		if (!wait_for_completion_timeout(&(g_modem_reset_ctrl.suspend_completion), HZ*10))
		{
			machine_restart("system halt");
			return;
		}
	}
	reset_prepare(MODEM_POWER_ON);
}

void reset_ipc_isr_idle(u32 data)
{
	reset_print_debug("\n");
	osl_sem_up(&(g_modem_reset_ctrl.wait_modem_master_in_idle_sem));
}

void reset_ipc_isr_reboot(u32 data)
{
	reset_print_debug("\n");
	osl_sem_up(&(g_modem_reset_ctrl.wait_ccore_reset_ok_sem));
}

u32 bsp_reset_is_feature_on(void)
{
	return g_modem_reset_ctrl.nv_config.is_feature_on;
}

u32 bsp_reset_is_connect_ril(void)
{
	return g_modem_reset_ctrl.nv_config.is_connected_ril;
}

int bsp_modem_reset(void)
{
	reset_print_debug("\n");
	if (g_modem_reset_ctrl.nv_config.is_feature_on)
	{
		queue_work(g_modem_reset_ctrl.reset_wq, &(g_modem_reset_ctrl.work_reset));
		return 0;
	}
	else
	{
		reset_print_err("reset fearute is off\n");
		return -1;
	}
}

void bsp_modem_power_off(void)
{
	reset_print_debug("\n");;
	if (g_modem_reset_ctrl.nv_config.is_feature_on)
	{
		queue_work(g_modem_reset_ctrl.reset_wq, &(g_modem_reset_ctrl.work_power_off));
	}
	else
	{
		reset_print_err("reset fearute is off\n");
	}
}

void bsp_modem_power_on(void)
{
	reset_print_debug("\n");;
	if (g_modem_reset_ctrl.nv_config.is_feature_on)
	{
		queue_work(g_modem_reset_ctrl.reset_wq, &(g_modem_reset_ctrl.work_power_on));
	}
	else
	{
		reset_print_err("reset fearute is off\n");
	}
}

s32 bsp_reset_cb_func_register(const char *name, pdrv_reset_cbfun func, int user_data, int prior)
{
	u32 use_cnt = 0;
	struct reset_cb_list *head = g_modem_reset_ctrl.list_head;

	use_cnt = g_modem_reset_ctrl.list_use_cnt;
	g_modem_reset_ctrl.list_head = do_cb_func_register(head, name, func, user_data, prior);
	if (use_cnt == g_modem_reset_ctrl.list_use_cnt)
	{
		return RESET_ERROR;
	}

    return RESET_OK;
}

struct modem_reset_ctrl *bsp_reset_control_get(void)
{
	return &g_modem_reset_ctrl;
}

s32 bsp_reset_ccpu_status_get(void)
{
	enum RESET_MULTICORE_CHANNEL_STATUS channel_status = CCORE_STATUS;

	if (0 == g_modem_reset_ctrl.state)
	{
		return 1;
	}
	else
	{
		return (g_modem_reset_ctrl.multicore_msg_switch & channel_status) ? 1 : 0;
	}
}

int __init bsp_reset_init(void)
{
	s32 ret = RESET_ERROR;
	struct device_node *np = NULL;

	memset(&(g_modem_reset_ctrl), 0, sizeof(g_modem_reset_ctrl));
	memset(&g_reset_debug, 0, sizeof(g_reset_debug));
	g_reset_debug.print_sw = 1;

	/* NV控制是否打开单独复位功能以及与RIL的对接 */	
	if(BSP_OK != bsp_nvm_read(NV_ID_DRV_CCORE_RESET, (u8*)&(g_modem_reset_ctrl.nv_config), sizeof(DRV_CCORE_RESET_STRU)))
	{
		reset_print_err("nv read fail, use default value\n");
	}

	np = of_find_compatible_node(NULL, NULL, "hisilicon,crgctrl");
	g_modem_reset_ctrl.crg_base = (u32)of_iomap(np, 0);
	if (!g_modem_reset_ctrl.crg_base)
	{
		reset_print_err("get crg_base fail!\n");
		return RESET_ERROR;
	}

	bsp_reset_bootflag_set(CCORE_BOOT_NORMAL);

	/* 置上acore与ccore之间通信状态可用标识 */
	g_modem_reset_ctrl.multicore_msg_switch = 1;
	g_modem_reset_ctrl.modem_action = MODEM_NORMAL;

	osl_sem_init(0, &g_modem_reset_ctrl.task_sem);
	osl_sem_init(0, &g_modem_reset_ctrl.wait_mcore_reply_sem);
	osl_sem_init(0, &g_modem_reset_ctrl.wait_mcore_reply_reseting_sem);
	osl_sem_init(0, &g_modem_reset_ctrl.wait_hifi_reply_sem);
	osl_sem_init(0, &g_modem_reset_ctrl.wait_ccore_reset_ok_sem);
	osl_sem_init(0, &g_modem_reset_ctrl.wait_modem_master_in_idle_sem);

	wake_lock_init(&g_modem_reset_ctrl.wake_lock, WAKE_LOCK_SUSPEND, "modem_reset wake");
	spin_lock_init(&g_modem_reset_ctrl.action_lock);

    g_modem_reset_ctrl.task = kthread_run(modem_reset_task,  NULL, "modem_reset");
	if(!g_modem_reset_ctrl.task)
	{
		reset_print_err("create modem_reset thread fail!\n");
		return RESET_ERROR;
	}

	ret = bsp_reset_cb_func_register("drv", drv_reset_cb, 0, DRV_RESET_CB_PIOR_ALL);
	if(ret !=  RESET_OK)
	{
		reset_print_err("register drv reset callback fail!\n");
		return RESET_ERROR;
	}

	ret = DRV_MAILBOX_REGISTERRECVFUNC(MAILBOX_MAILCODE_HIFI_TO_ACPU_CCORE_RESET_ID, (mb_msg_cb)from_hifi_mailbox_readcb, NULL);/*lint !e713 */
	if(MAILBOX_OK != ret)
	{
		reset_print_err("register mailbox callback fail");
		return RESET_ERROR;
	}

	init_completion(&(g_modem_reset_ctrl.suspend_completion));
	g_modem_reset_ctrl.pm_notify.notifier_call = reset_pm_notify;
	register_pm_notifier(&g_modem_reset_ctrl.pm_notify);

	g_modem_reset_ctrl.reset_wq = create_singlethread_workqueue("reset_wq");
    BUG_ON(!g_modem_reset_ctrl.reset_wq); /*lint !e548 */
	INIT_WORK(&(g_modem_reset_ctrl.work_reset), modem_reset_do_work);
	INIT_WORK(&(g_modem_reset_ctrl.work_power_off), modem_power_off_do_work);
	INIT_WORK(&(g_modem_reset_ctrl.work_power_on), modem_power_on_do_work);

	g_modem_reset_ctrl.ipc_recv_irq_idle = IPC_ACPU_INT_SRC_CCPU_RESET_IDLE;
	g_modem_reset_ctrl.ipc_recv_irq_reboot = IPC_ACPU_INT_SRC_CCPU_RESET_SUCC;
	g_modem_reset_ctrl.ipc_send_irq_wakeup_ccore = IPC_CCPU_INT_SRC_ACPU_RESET;
	if (bsp_ipc_int_connect(g_modem_reset_ctrl.ipc_recv_irq_idle, (voidfuncptr)reset_ipc_isr_idle, 0))
	{
		reset_print_err("connect idle ipc fail!\n");
		return RESET_ERROR;
	}
	if (bsp_ipc_int_enable(g_modem_reset_ctrl.ipc_recv_irq_idle))
	{
		reset_print_err("enable idle ipc fail!\n");
		return RESET_ERROR;
	}

	if (bsp_ipc_int_connect(g_modem_reset_ctrl.ipc_recv_irq_reboot, (voidfuncptr)reset_ipc_isr_reboot, 0))
	{
		reset_print_err("connect reboot ipc fail!\n");
		return RESET_ERROR;
	}
	if (bsp_ipc_int_enable(g_modem_reset_ctrl.ipc_recv_irq_reboot))
	{
		reset_print_err("enable reboot ipc fail!\n");
		return RESET_ERROR;
	}

	g_modem_reset_ctrl.state = 1;

	reset_print_err("ok\n");

	return 0;
}

module_init(bsp_reset_init); /*lint !e19*/

