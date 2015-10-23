/*----------------------------------------------------------------------------
 *      balongv7r2 m3 pm
 *----------------------------------------------------------------------------
 *      Name:    pm.C
 *      Purpose: RTX example program
 *----------------------------------------------------------------------------
 *      This code is part of balongv7r2 PWR.
 *---------------------------------------------------------------------------*/

#include "cmsis_os.h"

#include "osl_types.h"
#include "osl_bio.h"
#include "osl_irq.h"

#include "m3_cpufreq.h"
#include "pm_api.h"
#include "bsp_icc.h"
#include "bsp_hardtimer.h"

static void cpufreq_icc_init(void);
static s32 cpufreq_icc_read_cb(u32 id , u32 len, void* context);
static u32 cpufreq_get_cur_profile(void);
static u32 cpufreq_find_profile(u32 core_type, u32 action, u32 profile);
static s32 cpufreq_icc_send_acore(T_CPUFREQ_MAIL *msg);
static s32 cpufreq_icc_send_ccore(T_CPUFREQ_MAIL *msg);
static void cpufreq_recv_mail(T_CPUFREQ_MAIL  *rmail);
static void thread_cpufreq (void const *arg);

/*lint --e{64, 958} */
osMailQDef(cpufreq_mail, 32, T_CPUFREQ_MAIL);/*lint !e133*/
osMailQId  cpufreq_mail;

osThreadId thread_cpufreq_id;
/*lint --e{133} */
osThreadDef(thread_cpufreq, osPriorityNormal, 1, 512);


T_CPUFREQ_ST g_cpufreq;
/*lint -e64*/
//u8* g_cpufreq_string1[] = {"acore","ccore","mcore"};
//u8* g_cpufreq_string2[] = {"up","up2dst","down","down2dst"};

void cpufreq_init(void)
{
    dfs_ddrc_calc();
	//dfs_to_max();
	memset(&g_cpufreq, 0x0, sizeof(T_CPUFREQ_ST));
	/* 初始化记录的上一次需要调的频率值 */
	g_cpufreq.maxprof = CPUFREQ_MAX_PROFILE;
	g_cpufreq.minprof = CPUFREQ_MIN_PROFILE;
	g_cpufreq.curprof = cpufreq_get_cur_profile();
    M3_CUR_CPUFREQ_PROFILE = g_cpufreq.curprof;
    M3_MAX_CPUFREQ_PROFILE = g_cpufreq.maxprof;
    M3_MIN_CPUFREQ_PROFILE = g_cpufreq.minprof;
    M3_CPUFREQ_DOWN_FLAG(0) = 0;
    M3_CPUFREQ_DOWN_FLAG(1) = 0;

	cpufreq_mail = osMailCreate(osMailQ(cpufreq_mail), NULL);

	thread_cpufreq_id = osThreadCreate (osThread (thread_cpufreq), NULL);
	if (thread_cpufreq_id == NULL)
	{
		M3CPUFREQ_PRINT(" thread create error\n");
	}

	/* icc channel */
	cpufreq_icc_init();

}

static void cpufreq_icc_init(void)
{
	s32 ret;
	u32 channel_id_set = 0;

	/* m3 acore icc */
	channel_id_set = (ICC_CHN_MCORE_ACORE << 16) | MCU_ACORE_CPUFREQ;
	ret = bsp_icc_event_register(channel_id_set, cpufreq_icc_read_cb, NULL, (write_cb_func)NULL, (void *)NULL);
	if(ret != ICC_OK)
	{
		M3CPUFREQ_PRINT("iccinit 1 error\n");
	}

	channel_id_set = (ICC_CHN_MCORE_CCORE << 16) | MCU_CCORE_CPUFREQ;
	ret = bsp_icc_event_register(channel_id_set, cpufreq_icc_read_cb, NULL, (write_cb_func)NULL, (void *)NULL);
	if(ret != ICC_OK)
	{
		M3CPUFREQ_PRINT("iccinit 2 error\n");
	}
}

static s32 cpufreq_icc_read_cb(u32 id , u32 len, void* context)
{
	s32 ret = 0;
	u8 data[32];
	T_CPUFREQ_MAIL *smail;

	if((len == 0) || (len > 32))
	{
		M3CPUFREQ_PRINT("readcb len is 0 \n");
        return -1;
	}

	ret = bsp_icc_read(id, data, len);
	if(len != ret)/*lint !e737*/
	{
		M3CPUFREQ_PRINT("readcb error \r\n");
		return -1;
	}
	/*lint --e{569} */
	smail = osMailCAlloc(cpufreq_mail, osWaitForever);
	memcpy(smail, data, len);
	osMailPut(cpufreq_mail, smail);

	return 0;
}

static u32 cpufreq_get_cur_profile(void)
{
	//s32 tmp = 0;
	u32 tmp = 0;
	tmp = dfs_get_profile();
	if(tmp == CPUFREQ_INVALID_PROFILE)
	{
		M3CPUFREQ_PRINT("cut profile invalid\n");
		return 0;
	}
	return tmp;
}

static u32 cpufreq_down_1_step(u32 curprof, u32 core_type)
{
    u32 i = 0;
    u32 dstprof = CPUFREQ_INVALID_PROFILE;

    if(curprof <= g_cpufreq.minprof)
	{
		M3CPUFREQ_PRINT("already min profile\n");
		g_cpufreq.todo[core_type].flag = 0;
		M3_CPUFREQ_DOWN_FLAG(core_type) = 0;
       return dstprof;
	}
	else
	{
		g_cpufreq.todo[core_type].flag = 1;
		M3_CPUFREQ_DOWN_FLAG(core_type) = 1;
		g_cpufreq.todo[core_type].profile = curprof - 1;
		M3_CPUFREQ_DOWN_PROFILE(core_type) = curprof - 1;
		for(i = 0; i < CPUFREQ_NUM; i++)
		{
            /* 没有降频需求且没有睡眠，频率降不了 */
			if((g_cpufreq.todo[i].flag == 0)&&(g_cpufreq.todo[i].sleepflag == 0))
			{
				return dstprof;
			}
		}
		dstprof = curprof - 1;
		for(i = 0; i < CPUFREQ_NUM; i++)
		{
            /* 频率降到目标及以下 或者睡眠了，降频需求清空*/
			if((dstprof <= g_cpufreq.todo[i].profile)||(g_cpufreq.todo[i].sleepflag == 1))
			{
				g_cpufreq.todo[i].flag = 0;
				M3_CPUFREQ_DOWN_FLAG(i) = 0;
			}
		}
	}
    return dstprof;
}
static u32 cpufreq_down_to_dst(u32 profile, u32 curprof, u32 core_type)
{
    u32 i = 0;
    u32 tmp = 0;
    u32 dstprof = CPUFREQ_INVALID_PROFILE;

    if((profile < g_cpufreq.minprof) || (curprof <= profile))
	{
		M3CPUFREQ_PRINT("cpufreq down  dst: %d, cur:%d, max: %d\n", profile, curprof, g_cpufreq.maxprof);
		g_cpufreq.todo[core_type].flag = 0;
		M3_CPUFREQ_DOWN_FLAG(core_type) = 0;
		return dstprof;
	}
	else
	{
		g_cpufreq.todo[core_type].flag = 1;
		M3_CPUFREQ_DOWN_FLAG(core_type) = 1;
		g_cpufreq.todo[core_type].profile = profile;
		M3_CPUFREQ_DOWN_PROFILE(core_type) = profile;
		for(i = 0; i < CPUFREQ_NUM; i++)
		{
			/* 没有降频需求且没有睡眠，频率降不了 */
			if((g_cpufreq.todo[i].flag == 0)&&(g_cpufreq.todo[i].sleepflag == 0))
			{
				return dstprof;
			}
		}
		for(i = 0; i < CPUFREQ_NUM; i++)
		{
			if((tmp < g_cpufreq.todo[i].profile)&&(g_cpufreq.todo[i].sleepflag == 0))
			{
				tmp  = g_cpufreq.todo[i].profile;
			}
		}
		dstprof = tmp;
		for(i = 0; i < CPUFREQ_NUM; i++)
		{
			/* 频率降到目标及以下 或者睡眠了，降频需求清空*/
			if((dstprof <= g_cpufreq.todo[i].profile)||(g_cpufreq.todo[i].sleepflag == 1))
			{
				g_cpufreq.todo[i].flag = 0;
				M3_CPUFREQ_DOWN_FLAG(i) = 0;
			}
		}
	}
    return dstprof;
}

static u32 cpufreq_find_profile(u32 core_type, u32 action, u32 profile)
{
	u32 curprof = CPUFREQ_INVALID_PROFILE;
	u32 dstprof = CPUFREQ_INVALID_PROFILE;

	curprof = cpufreq_get_cur_profile();
	switch(action)
	{
		case 0://升
			g_cpufreq.todo[core_type].flag = 0;
			M3_CPUFREQ_DOWN_FLAG(core_type) = 0;
			if(curprof < g_cpufreq.maxprof)
			{
				dstprof = curprof + 1;
			}
			else
			{
				M3CPUFREQ_PRINT("already max profile\n");
				dstprof = CPUFREQ_INVALID_PROFILE;/* invalid */
			}
			break;
		case 1://升目标
			g_cpufreq.todo[core_type].flag = 0;
			M3_CPUFREQ_DOWN_FLAG(core_type) = 0;
			if((curprof < profile) && (profile <= g_cpufreq.maxprof))
			{
				dstprof = profile;
			}
			else
			{
				M3CPUFREQ_PRINT("cpufreq up  dst: %d, cur:%d, max: %d\n", profile, curprof, g_cpufreq.maxprof);
				dstprof = CPUFREQ_INVALID_PROFILE;/* invalid or not change*/
			}
			break;
		case 2://降
			dstprof = cpufreq_down_1_step(curprof, core_type);
			break;
		case 3:
			dstprof = cpufreq_down_to_dst(profile, curprof, core_type);
			break;
		case 4:
			if(profile > g_cpufreq.maxprof)
			{
                M3CPUFREQ_PRINT("1 profile not in the range\n");

			}
			else
			{
				g_cpufreq.minprof = profile;
                M3_MIN_CPUFREQ_PROFILE = profile;
				if(curprof < profile)
				{
					dstprof = profile;
				}
				else
				{
					dstprof = CPUFREQ_INVALID_PROFILE;
				}
			}
			break;
		case 5:
			if((profile > CPUFREQ_MAX_PROFILE)||(profile < g_cpufreq.minprof))
			{
                M3CPUFREQ_PRINT("2 profile not in the range\n");
			}
			else
			{
				g_cpufreq.maxprof = profile;
                M3_MAX_CPUFREQ_PROFILE = profile;
				if(curprof > profile)
				{
					dstprof = profile;
				}
				else
				{
					dstprof = CPUFREQ_INVALID_PROFILE;
				}
			}
			break;
		default :
			break;
	}
	return dstprof;
}

static void cpufreq_set_profile_debug(u32 action, u32 profile)
{
	int irqlock = 0;
	u32 i = 0;

	local_irq_save(irqlock);
	switch(action)
	{
		case 0:
			if((profile > g_cpufreq.maxprof) || (profile < g_cpufreq.minprof) || (profile == g_cpufreq.curprof))
			{
				M3CPUFREQ_PRINT("case 0 input error\n");
				local_irq_restore(irqlock);
				return;
			}
			if((!g_cpufreq.debuglockflag)&&(!g_cpufreq.ccorelockflag))
        	{
        		dfs_set_profile(profile);
        		g_cpufreq.curprof = profile;
                M3_CUR_CPUFREQ_PROFILE = profile;
        	}
            else
            {
                M3CPUFREQ_PRINT("case 0 cpufreq lock\n");
            }
			break;
		case 1:
			for(i = 0; i < CPUFREQ_NUM; i++)
			{
				g_cpufreq.todo[i].flag = 0;
				M3_CPUFREQ_DOWN_FLAG(i) = 0;
			}
			g_cpufreq.debuglockflag = 1;
			break;
		case 2:
			for(i = 0; i < CPUFREQ_NUM; i++)
			{
				g_cpufreq.todo[i].flag = 0;
				M3_CPUFREQ_DOWN_FLAG(i) = 0;
			}
			g_cpufreq.debuglockflag = 0;
			break;
		default:
			break;
	}
	local_irq_restore(irqlock);
}

static void cpufreq_set_ccorelockflag(u32 action)
{
    int irqlock = 0;

    local_irq_save(irqlock);
    switch(action)
	{
        case 0:
            g_cpufreq.ccorelockflag = 1;
            break;
        case 1:
            g_cpufreq.ccorelockflag = 0;
            break;
        default:
            break;
    }
    local_irq_restore(irqlock);
}

void cpufreq_change_ccorelockflag(u32 flag)
{
    g_cpufreq.ccorelockflag = flag;
}

void cpufreq_set_sleepflag(u32 core_type, u32 flag)
{
    g_cpufreq.todo[core_type].sleepflag = flag;
    g_cpufreq.todo[core_type].flag = flag;
    M3_CPUFREQ_DOWN_FLAG(core_type) = flag;
}
static s32 cpufreq_icc_send_acore(T_CPUFREQ_MAIL *msg)
{
	u32 channel_id = (ICC_CHN_MCORE_ACORE << 16) | MCU_ACORE_CPUFREQ;
	s32 ret = 0;
	u32 msglen = sizeof(T_CPUFREQ_MAIL);

	ret = bsp_icc_send(ICC_CPU_APP, channel_id, (u8*)msg, msglen);
	if(ret != msglen)/*lint !e737*/
	{
		M3CPUFREQ_PRINT("send_acore error\n");
		return -1;
	}
    return 0;
}

static s32 cpufreq_icc_send_ccore(T_CPUFREQ_MAIL *msg)
{
	u32 channel_id = (ICC_CHN_MCORE_CCORE << 16) | MCU_CCORE_CPUFREQ;
	s32 ret = 0;
	u32 msglen = sizeof(T_CPUFREQ_MAIL);

	ret = bsp_icc_send(ICC_CPU_MODEM, channel_id, (u8*)msg, msglen);
	if(ret != msglen)/*lint !e737*/
	{
		M3CPUFREQ_PRINT("send_ccore error\n");
		return -1;
	}
    return 0;
}

static void cpufreq_recv_mail(T_CPUFREQ_MAIL  *rmail)
{
	int irqlock = 0;
	u32 dstprof = 0;
	T_CPUFREQ_MAIL msgsend;

	if(rmail == NULL)
	{
		M3CPUFREQ_PRINT("recv_mail input error\n");
		return;
	}

    //M3CPUFREQ_PRINT(" msgtype:%d coretype:%d action:%d profile=%d\n", rmail->msgtype, rmail->coretype, rmail->action, rmail->profile);
	switch(rmail->msgtype)
	{
		case 1:
			local_irq_save(irqlock);
			if((!g_cpufreq.debuglockflag)&&(!g_cpufreq.ccorelockflag))
			{
				dstprof = cpufreq_find_profile(rmail->coretype, rmail->action, rmail->profile);
				if(dstprof != CPUFREQ_INVALID_PROFILE)
				{
					dfs_set_profile(dstprof);
					g_cpufreq.curprof = dstprof;
                    M3_CUR_CPUFREQ_PROFILE = dstprof;
				}
			}
            else
            {
                M3CPUFREQ_PRINT("cpufreq lock \n");
            }
			local_irq_restore(irqlock);
			break;
		case 2:
			cpufreq_set_profile_debug(rmail->action, rmail->profile);
			break;
		case 3:
			msgsend.msgtype = 4;
			msgsend.coretype = CPUFREAQ_MCORE;
			msgsend.profile = cpufreq_get_cur_profile();
			if(rmail->coretype == CPUFREAQ_ACORE)
			{
				cpufreq_icc_send_acore(&msgsend);
			}
			else if(rmail->coretype == CPUFREAQ_CCORE)
			{
				cpufreq_icc_send_ccore(&msgsend);
			}
			else
			{
				M3CPUFREQ_PRINT(" case 3 mail error\n");
			}
			break;
        case 5:
            cpufreq_set_ccorelockflag(rmail->action);
            break;
		default:
			M3CPUFREQ_PRINT("msgtype error\n");
			break;
	}
}

static void thread_cpufreq (void const *arg)
{
	T_CPUFREQ_MAIL  *rmail = NULL;
	osEvent  evt;

	for(;;)
	{
		/*lint --e{569} */
		evt = osMailGet(cpufreq_mail, osWaitForever);
		if (evt.status == osEventMail)
		{
			rmail = evt.value.p;
			cpufreq_recv_mail(rmail);
			osMailFree(cpufreq_mail, rmail);
		}
	}
}

void cpufreq_print_debug(void)
{
	printk("default max profile:%d\n", CPUFREQ_MAX_PROFILE);
	printk("default min profile:%d\n", CPUFREQ_MIN_PROFILE);
	printk("current max profile:%d\n", g_cpufreq.maxprof);
	printk("current min profile:%d\n", g_cpufreq.minprof);
	printk("current profile:%d\n", g_cpufreq.curprof);
}

