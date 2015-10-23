
#include <bsp_dpm.h>
#include <bsp_hardtimer.h>
#include "dpm_balong.h"
/*lint --e{826, 525 } */
/*记录可维可测回调级别*/
enum dpm_stat_enum{
	PREPARE,
	SUS_EARLY,
	SUSPEND,
	LATE,
	EARLY,
	RESUME,
	RES_LATE,
	COMPLETE
};
static struct dpm_control dpm_ctrl;
s32 bsp_device_pm_add(struct dpm_device *dev)
{
	 /*lint -save -e516*/
	 int i = 0;
	 unsigned long flags = 0;
	 spin_lock_irqsave(&(dpm_ctrl.dpm_list_mtx),flags);
	if(NULL!=dev)
	{
		INIT_LIST_HEAD(&dev->entry);
		for(i = 0;i<DPM_CALLBACK_NUM;i++)
			dev->debug_count[i] = 0;
		list_add_tail(&dev->entry,&(dpm_ctrl.dpm_list));
	} 
	else
	{
		dpm_print("the dev to add is NULL\n");
		spin_unlock_irqrestore(&(dpm_ctrl.dpm_list_mtx),flags);
		return ERROR;
	}
	spin_unlock_irqrestore(&(dpm_ctrl.dpm_list_mtx),flags);/*lint !e40*/
	return OK;
}

s32 bsp_device_pm_remove(struct dpm_device *dev)
{
	unsigned long flags = 0;
	spin_lock_irqsave(&(dpm_ctrl.dpm_list_mtx),flags);/*lint !e40*/
	if((NULL!=dev)&&(!list_empty(&dev->entry)))
	{
		list_del_init(&dev->entry);
	}
	else
	{
		dpm_print("the dev to remove is NULL or is already removed\n");
		spin_unlock_irqrestore(&(dpm_ctrl.dpm_list_mtx),flags);/*lint !e40*/
		return ERROR;
	}
	spin_unlock_irqrestore(&(dpm_ctrl.dpm_list_mtx),flags);/*lint !e40*/
	return OK;
}

static void dpm_resume_early()
{
	unsigned long flags = 0;
	struct dpm_device *dev,*next;
	s32 ret=0;
	spin_lock_irqsave(&(dpm_ctrl.dpm_list_mtx),flags);/*lint !e40*/
	list_for_each_entry_safe(dev,next,&(dpm_ctrl.dpm_late_early_list),entry)
	{
		if(dev->resume_early)
		{
			dpm_ctrl.current_callback = dev->resume_early;
			dpm_ctrl.device_name = dev->device_name;
			dpm_ctrl.flag = 1;
			dev->debug_time[EARLY][0] = bsp_get_slice_value();
           DWB(); 
			ret = dev->resume_early(dev);
			dev->debug_time[EARLY][1] = bsp_get_slice_value();
			dpm_ctrl.flag = 0;
           DWB();
			if(OK!=ret)
			{
				/*如果resume_early失败，则打印，并跳过此设备继续resume下一个设备*/
				dev->debug_count[EARLY]++;
				continue;
			}
		}
		list_move_tail(&dev->entry,&(dpm_ctrl.dpm_suspend_list));
	} 
	spin_unlock_irqrestore(&(dpm_ctrl.dpm_list_mtx),flags);/*lint !e40*/
}
static void dpm_resume()
{
	struct dpm_device *dev,*next;
	s32 ret=0;
	unsigned long flags = 0;
	spin_lock_irqsave(&(dpm_ctrl.dpm_list_mtx),flags);/*lint !e40*/
	list_for_each_entry_safe(dev,next,&(dpm_ctrl.dpm_suspend_list),entry)
	{
		if(dev->resume)
		{
			dpm_ctrl.current_callback = dev->resume;
			dpm_ctrl.device_name = dev->device_name;
			dpm_ctrl.flag = 1;
			dev->debug_time[RESUME][0] = bsp_get_slice_value();
           DWB();
			ret = dev->resume(dev);
			dev->debug_time[RESUME][1] = bsp_get_slice_value();
			dpm_ctrl.flag = 0;
           DWB();
			if(OK!=ret)
			{
				/*如果resume失败，则打印，并跳过此设备继续resume下一个设备*/
				dev->debug_count[RESUME]++;
				continue;
			}
		}
		list_move_tail(&dev->entry,&(dpm_ctrl.dpm_sus_early_list));
	} 
	spin_unlock_irqrestore(&(dpm_ctrl.dpm_list_mtx),flags);/*lint !e40*/
}

static void dpm_resume_late()
{
	struct dpm_device *dev,*next;
	s32 ret=0;
	unsigned long flags = 0;
	spin_lock_irqsave(&(dpm_ctrl.dpm_list_mtx),flags);/*lint !e40*/
	list_for_each_entry_safe(dev,next,&(dpm_ctrl.dpm_sus_early_list),entry)
	{
		if(dev->resume_late)
		{
			dpm_ctrl.current_callback = dev->resume_late;
			dpm_ctrl.device_name = dev->device_name;
			dpm_ctrl.flag = 1;
			dev->debug_time[RES_LATE][0] = bsp_get_slice_value();
           DWB();
			ret = dev->resume_late(dev);
			dev->debug_time[RES_LATE][1] = bsp_get_slice_value();
			dpm_ctrl.flag = 0;
           DWB();
			if(OK!=ret)
			{
				/*如果resume失败，则打印，并跳过此设备继续resume下一个设备*/
				dev->debug_count[RES_LATE]++;
				continue;
			}
		}
		list_move_tail(&dev->entry,&(dpm_ctrl.dpm_prepared_list));
	} 
	spin_unlock_irqrestore(&(dpm_ctrl.dpm_list_mtx),flags);/*lint !e40*/
}


static void dpm_complete()
{
	struct dpm_device *dev,*next;
	s32 ret=0;
	unsigned long flags = 0;
	spin_lock_irqsave(&(dpm_ctrl.dpm_list_mtx),flags);/*lint !e40*/
	list_for_each_entry_safe(dev,next,&(dpm_ctrl.dpm_prepared_list),entry)
	{
		if(dev->complete)
		{
			dpm_ctrl.current_callback = dev->complete;
			dpm_ctrl.device_name = dev->device_name;
			dpm_ctrl.flag = 1;
			dev->debug_time[COMPLETE][0] = bsp_get_slice_value();
           DWB();
			ret = dev->complete(dev);
			dev->debug_time[COMPLETE][1] = bsp_get_slice_value();
			dpm_ctrl.flag = 0;
           DWB();
			if(OK!=ret)
			{
				/*如果complete失败，则打印，并跳过此设备继续complete下一个设备*/
				dev->debug_count[COMPLETE]++;
				continue;
			}
		}
		list_move_tail(&dev->entry,&(dpm_ctrl.dpm_list));
	} 
	spin_unlock_irqrestore(&(dpm_ctrl.dpm_list_mtx),flags);/*lint !e40*/
}
void bsp_dpm_resume()
{  
	dpm_resume_early();
	dpm_resume();
	dpm_resume_late();
	dpm_complete();
	dpm_ctrl.debug_count[5]++;/*resume times*/
}
static s32 dpm_prepare()
{
	struct dpm_device *dev,*next;
	s32 ret=0;
	unsigned long flags = 0;
	spin_lock_irqsave(&(dpm_ctrl.dpm_list_mtx),flags);/*lint !e40*/
	list_for_each_entry_safe(dev,next,&(dpm_ctrl.dpm_list),entry)
	{
		if(dev->prepare)
		{
			dpm_ctrl.current_callback = dev->prepare;
			dpm_ctrl.device_name = dev->device_name;
			dpm_ctrl.flag = 1;
			dev->debug_time[PREPARE][0] = bsp_get_slice_value();
           DWB();
			ret = dev->prepare(dev);
			dev->debug_time[PREPARE][1] = bsp_get_slice_value();
			dpm_ctrl.flag = 0;
           DWB();
			if(OK!=ret)
			{
				dev->debug_count[PREPARE]++;
				spin_unlock_irqrestore(&(dpm_ctrl.dpm_list_mtx),flags);/*lint !e40*/
				goto complete;
			}
		}
		list_move_tail(&dev->entry,&(dpm_ctrl.dpm_prepared_list));
	} 
	spin_unlock_irqrestore(&(dpm_ctrl.dpm_list_mtx),flags);/*lint !e40*/
	return OK;
	
	complete:
		bsp_dpm_resume();
		return ERROR;	
}

static s32 dpm_suspend_early()
{
	struct dpm_device *dev,*next;
	s32 ret=0;
	unsigned long flags = 0;
	spin_lock_irqsave(&(dpm_ctrl.dpm_list_mtx),flags);/*lint !e40*/
	list_for_each_entry_safe(dev,next,&(dpm_ctrl.dpm_prepared_list),entry)
	{
		if(dev->suspend_early)
		{
			dpm_ctrl.current_callback = dev->suspend_early;
			dpm_ctrl.device_name = dev->device_name;
			dpm_ctrl.flag = 1;
			dev->debug_time[SUS_EARLY][0] = bsp_get_slice_value();
           DWB();
			ret = dev->suspend_early(dev);
			dev->debug_time[SUS_EARLY][1] = bsp_get_slice_value();
			dpm_ctrl.flag = 0;
           DWB();
			if(OK!=ret)
			{
				dev->debug_count[SUS_EARLY]++;
				spin_unlock_irqrestore(&(dpm_ctrl.dpm_list_mtx),flags);/*lint !e40*/
				goto resume;
			}
		}
		list_move_tail(&dev->entry,&(dpm_ctrl.dpm_sus_early_list));
	} 
	spin_unlock_irqrestore(&(dpm_ctrl.dpm_list_mtx),flags);/*lint !e40*/
	return OK;
	
	resume:
		bsp_dpm_resume();/*suspend阶段出错，需要回退执行已经suspend的各设备的resume和complete回调*/
		return ERROR;
	
}

static s32 dpm_suspend()
{
	struct dpm_device *dev,*next;
	s32 ret=0;
	unsigned long flags = 0;
	spin_lock_irqsave(&(dpm_ctrl.dpm_list_mtx),flags);/*lint !e40*/
	list_for_each_entry_safe(dev,next,&(dpm_ctrl.dpm_sus_early_list),entry)
	{
		if(dev->suspend)
		{
			dpm_ctrl.current_callback = dev->suspend;
			dpm_ctrl.device_name = dev->device_name;
			dpm_ctrl.flag = 1;
			dev->debug_time[SUSPEND][0] = bsp_get_slice_value();
           DWB();
			ret = dev->suspend(dev);
			dev->debug_time[SUSPEND][1] = bsp_get_slice_value();
			dpm_ctrl.flag = 0;
           DWB();
			if(OK!=ret)
			{
				dev->debug_count[SUSPEND]++;
				spin_unlock_irqrestore(&(dpm_ctrl.dpm_list_mtx),flags);/*lint !e40*/
				goto resume;
			}
		}
		list_move_tail(&dev->entry,&(dpm_ctrl.dpm_suspend_list));
	} 
	spin_unlock_irqrestore(&(dpm_ctrl.dpm_list_mtx),flags);/*lint !e40*/
	return OK;
	
	resume:
		bsp_dpm_resume();/*suspend阶段出错，需要回退执行已经suspend的各设备的resume和complete回调*/
		return ERROR;
	
}
static s32 dpm_suspend_late()
{
	struct dpm_device *dev,*next;
	s32 ret=0;
	unsigned long flags = 0;
	spin_lock_irqsave(&(dpm_ctrl.dpm_list_mtx),flags);/*lint !e40*/
	list_for_each_entry_safe(dev,next,&(dpm_ctrl.dpm_suspend_list),entry)
	{
		if(dev->suspend_late)
		{
			dpm_ctrl.current_callback = dev->suspend_late;
			dpm_ctrl.device_name = dev->device_name;
			dpm_ctrl.flag = 1;
			dev->debug_time[LATE][0] = bsp_get_slice_value();
           DWB();
			ret = dev->suspend_late(dev);
			dev->debug_time[LATE][1] = bsp_get_slice_value();
			dpm_ctrl.flag = 0;
           DWB();
			if(OK!=ret)
			{
				dev->debug_count[LATE]++;
				spin_unlock_irqrestore(&(dpm_ctrl.dpm_list_mtx),flags);/*lint !e40*/
				goto resume;
			}
		}
		list_move_tail(&dev->entry,&(dpm_ctrl.dpm_late_early_list));
	} 
	spin_unlock_irqrestore(&(dpm_ctrl.dpm_list_mtx),flags);/*lint !e40*/
	return OK;
	
	resume:
		bsp_dpm_resume();
		return ERROR;
	
}

s32 bsp_dpm_suspend()
{
	s32 ret = 0;
	dpm_ctrl.debug_count[4]++;/*total suspend times*/
	ret = dpm_prepare();
	if(OK!=ret)
	{
		dpm_ctrl.debug_count[0]++;/*prepare error times*/
		return ERROR;
	}
	ret = dpm_suspend_early();
	if(OK!=ret)
	{
		dpm_ctrl.debug_count[1]++;/*suspend error times*/
		return ERROR;
	}
	ret = dpm_suspend();
	if(OK!=ret)
	{
		dpm_ctrl.debug_count[2]++;/*suspend late error times*/
		return ERROR;
	}
	ret = dpm_suspend_late();/*suspend realy late error times*/
	if(OK!=ret)
	{
		dpm_ctrl.debug_count[3]++;
		return ERROR;
	}
	return OK;
}

/*在串口调试使用*/
s32 bsp_dpm_suspend_single(char *dev_name)
{
	s32 ret = 0;
	struct dpm_device *cur;
	unsigned long flags = 0;
	if(NULL == dev_name)
	{
		dpm_print("null para\n");
		return ERROR;
	}
	else
	{
		spin_lock_irqsave(&(dpm_ctrl.dpm_list_mtx),flags);/*lint !e40*/
		list_for_each_entry(cur,&(dpm_ctrl.dpm_list),entry)
		{
			ret = strcmp(cur->device_name,dev_name);
			if(0==ret)
			{
				dpm_ctrl.device_name = dev_name;
				break;
			}
		}
		/*如果未查到匹配项返回错误*/
		if(&cur->entry == &(dpm_ctrl.dpm_list))
		{
			dpm_print("dev not in list\n");
			goto failed;
		}
		if(cur->prepare)
		{
			dpm_ctrl.current_callback = cur->prepare;
			dpm_ctrl.flag = 1;
			cur->debug_time[PREPARE][0]=bsp_get_slice_value();
			ret = cur->prepare(cur);
			cur->debug_time[PREPARE][1]=bsp_get_slice_value();
			dpm_ctrl.flag = 0;
			if(OK!=ret)
			{
				cur->debug_count[PREPARE]++;
				goto failed;
			}
		}
		/*prepare成功执行后，移入prepared链表*/
		list_move_tail(&cur->entry,&(dpm_ctrl.dpm_prepared_list));
		if(cur->suspend_early)
		{
			dpm_ctrl.current_callback = cur->suspend_early;
			dpm_ctrl.flag = 1;
			cur->debug_time[SUS_EARLY][0]=bsp_get_slice_value();
			ret = cur->suspend_early(cur);
			cur->debug_time[SUS_EARLY][1]=bsp_get_slice_value();
			dpm_ctrl.flag = 0;
			if(OK!=ret)
			{
				cur->debug_count[SUS_EARLY]++;
				if(cur->complete)
				{
					dpm_ctrl.current_callback = cur->suspend;
					dpm_ctrl.flag = 1;
					cur->debug_time[COMPLETE][0]=bsp_get_slice_value();
					ret = cur->complete(cur);
					cur->debug_time[COMPLETE][1]=bsp_get_slice_value();
					dpm_ctrl.flag = 0;
					if(ret != OK)
					{
						cur->debug_count[COMPLETE]++;
					}
				}
				goto failed;
			}
		}
		list_move_tail(&cur->entry,&(dpm_ctrl.dpm_sus_early_list));
		if(cur->suspend)
		{
			dpm_ctrl.current_callback = cur->suspend;
			dpm_ctrl.flag = 1;
			cur->debug_time[SUSPEND][0]=bsp_get_slice_value();
			ret = cur->suspend(cur);
			cur->debug_time[SUSPEND][1]=bsp_get_slice_value();
			dpm_ctrl.flag = 0;
			if(OK!=ret)
			{
				cur->debug_count[SUSPEND]++;
				if(cur->resume_late)
				{
					dpm_ctrl.current_callback = cur->resume_late;
					dpm_ctrl.flag = 1;
					cur->debug_time[RES_LATE][0]=bsp_get_slice_value();
					ret = cur->resume_late(cur);
					cur->debug_time[RES_LATE][1]=bsp_get_slice_value();
					dpm_ctrl.flag = 0;
					if(ret != OK)
					{
						cur->debug_count[RES_LATE]++;
					}
					goto failed;
				}
				if(cur->complete)
				{
					dpm_ctrl.current_callback = cur->suspend;
					dpm_ctrl.flag = 1;
					cur->debug_time[COMPLETE][0]=bsp_get_slice_value();
					/* coverity[copy_paste_error] */
					ret = cur->complete(cur);
					cur->debug_time[COMPLETE][1]=bsp_get_slice_value();
					dpm_ctrl.flag = 0;
					if(ret != OK)
					{
						cur->debug_count[COMPLETE]++;
					}
					goto failed;
				}				
			}
		}
		/*suspend成功执行后，移入suspend链表*/
		list_move_tail(&cur->entry,&(dpm_ctrl.dpm_suspend_list));
		if(cur->suspend_late)
		{
			dpm_ctrl.current_callback = cur->suspend_late;
			dpm_ctrl.flag = 1;
			cur->debug_time[LATE][0]=bsp_get_slice_value();
			ret = cur->suspend_late(cur);
			cur->debug_time[LATE][1]=bsp_get_slice_value();
			dpm_ctrl.flag = 0;
			if(OK!=ret)
			{
				cur->debug_count[LATE]++;
				if(cur->resume)
				{
					dpm_ctrl.current_callback = cur->resume;
					dpm_ctrl.flag = 1;
					cur->debug_time[RESUME][0]=bsp_get_slice_value();
					ret = cur->resume(cur);
					cur->debug_time[RESUME][1]=bsp_get_slice_value();
					dpm_ctrl.flag = 0;
					if(ret != OK)
					{
						cur->debug_count[RESUME]++;
					}
					goto failed;
				}
				if(cur->resume_late)
				{
					dpm_ctrl.current_callback = cur->resume_late;
					dpm_ctrl.flag = 1;
					cur->debug_time[RES_LATE][0]=bsp_get_slice_value();
					ret = cur->resume_late(cur);
					cur->debug_time[RES_LATE][1]=bsp_get_slice_value();
					dpm_ctrl.flag = 0;
					if(ret != OK)
					{
						cur->debug_count[RES_LATE]++;
					}
					goto failed;
				}
				if(cur->complete)
				{
					dpm_ctrl.current_callback = cur->complete;
					dpm_ctrl.flag = 1;
					cur->debug_time[COMPLETE][0]=bsp_get_slice_value();
					/* coverity[original] */
					ret = cur->complete(cur);
					cur->debug_time[COMPLETE][1]=bsp_get_slice_value();
					dpm_ctrl.flag = 0;
					if(ret != OK)
					{
						cur->debug_count[COMPLETE]++;
					}
					goto failed;
				}		
			}
		}
		/*suspend late成功执行后，移入late early链表*/
		list_move_tail(&cur->entry,&(dpm_ctrl.dpm_late_early_list));
		spin_unlock_irqrestore(&(dpm_ctrl.dpm_list_mtx),flags);/*lint !e40*/
		return OK;
	}
	failed:
		spin_unlock_irqrestore(&(dpm_ctrl.dpm_list_mtx),flags);/*lint !e40*/
		return ERROR;
}
/*在串口调试使用*/
s32 bsp_dpm_resume_single(char *dev_name)
{
	s32 ret = 0;
	struct dpm_device *cur;
	unsigned long flags = 0;
	if(NULL == dev_name)
	{
		dpm_print("[%s]:null para\n");
		return ERROR;
	}
	else
	{
		spin_lock_irqsave(&(dpm_ctrl.dpm_list_mtx),flags);/*lint !e40*/
		list_for_each_entry(cur,&(dpm_ctrl.dpm_late_early_list),entry)
		{
			ret = strcmp(cur->device_name,dev_name);
			if(0==ret)
			{
				dpm_ctrl.device_name = dev_name;
				break;
			}
		}
		/*如果未查到匹配项返回错误*/
		if(&cur->entry == &(dpm_ctrl.dpm_late_early_list))
		{
			dpm_print("dev not in list\n");
			goto failed;
		}
		if(cur->resume_early)
		{
			dpm_ctrl.current_callback = cur->resume_early;
			dpm_ctrl.flag = 1;
			cur->debug_time[EARLY][0]=bsp_get_slice_value();
			ret = cur->resume_early(cur);
			cur->debug_time[EARLY][1]=bsp_get_slice_value();
			dpm_ctrl.flag = 0;
			if(OK != ret)
			{
				cur->debug_count[EARLY]++;
				goto failed;
			}
		}
		/*resume early 成功执行后，移入到suspend链表*/
		list_move(&cur->entry,&(dpm_ctrl.dpm_suspend_list));
		if(cur->resume)
		{
			dpm_ctrl.current_callback = cur->resume;
			dpm_ctrl.flag = 1;
			cur->debug_time[RESUME][0]=bsp_get_slice_value();
			ret = cur->resume(cur);
			cur->debug_time[RESUME][1]=bsp_get_slice_value();
			dpm_ctrl.flag = 0;
			if(OK != ret)
			{
				cur->debug_count[RESUME]++;
				goto failed;
			}
		}
		/*resume成功执行后，移入到prepared链表*/
		list_move(&cur->entry,&(dpm_ctrl.dpm_sus_early_list));
		if(cur->resume_late)
		{
			dpm_ctrl.current_callback = cur->resume_late;
			dpm_ctrl.flag = 1;
			cur->debug_time[RES_LATE][0]=bsp_get_slice_value();
			ret = cur->resume_late(cur);
			cur->debug_time[RES_LATE][1]=bsp_get_slice_value();
			dpm_ctrl.flag = 0;
			if(OK != ret)
			{
				cur->debug_count[RES_LATE]++;
				goto failed;
			}
		}
		/*resume成功执行后，移入到prepared链表*/
		list_move(&cur->entry,&(dpm_ctrl.dpm_prepared_list));
		if(cur->complete)
		{
			dpm_ctrl.current_callback = cur->complete;
			dpm_ctrl.flag = 1;
			cur->debug_time[COMPLETE][0]=bsp_get_slice_value();
			ret = cur->complete(cur);
			cur->debug_time[COMPLETE][1]=bsp_get_slice_value();
			dpm_ctrl.flag = 0;
			if(OK != ret)
			{
				cur->debug_count[COMPLETE]++;
				goto failed;
			}
		}
		/*complete成功执行后，移入到dpm_list链表*/
		list_move(&cur->entry,&(dpm_ctrl.dpm_list));
		spin_unlock_irqrestore(&(dpm_ctrl.dpm_list_mtx),flags);/*lint !e40*/
		return OK;
	}
	failed:
		spin_unlock_irqrestore(&(dpm_ctrl.dpm_list_mtx),flags);/*lint !e40*/
		return ERROR;
}

void bsp_dpm_show()
{
	struct dpm_device *dev;
	dpm_print("devnode_name=%s,current_callback = 0x%x,  flag = %d\n",dpm_ctrl.device_name,dpm_ctrl.current_callback,dpm_ctrl.flag);
	dpm_print("total_suspend   pre_err  sus_early_err  sus_err late_err  total_resume\n");
	dpm_print("     %d               %d           %d           %d      %d          %d\n",dpm_ctrl.debug_count[4],dpm_ctrl.debug_count[0],dpm_ctrl.debug_count[1],dpm_ctrl.debug_count[2],dpm_ctrl.debug_count[4]);
	dpm_print("-------------------dpm list info:  dev_name:-----------------------\n");
	list_for_each_entry(dev,&(dpm_ctrl.dpm_list),entry)
	{
		dpm_print("\t\t%s:\n",dev->device_name);
		dpm_print("\t prepare time begin: %d\n",dev->debug_time[PREPARE][0]);
		dpm_print("\t prepare time end: %d\n",dev->debug_time[PREPARE][1]);
		dpm_print("\t sus early time begin: %d\n",dev->debug_time[SUS_EARLY][0]);
		dpm_print("\t sus early time end: %d\n",dev->debug_time[SUS_EARLY][1]);
		dpm_print("\t suspend time begin: %d\n",dev->debug_time[SUSPEND][0]);
		dpm_print("\t suspend time end: %d\n",dev->debug_time[SUSPEND][1]);
		dpm_print("\t suspend_late time begin: %d\n",dev->debug_time[LATE][0]);
		dpm_print("\t suspend_late time end: %d\n",dev->debug_time[LATE][1]);
		dpm_print("\t resume early  time begin: %d\n",dev->debug_time[EARLY][0]);
		dpm_print("\t resume early  time end: %d\n",dev->debug_time[EARLY][1]);
		dpm_print("\t resume time begin: %d\n",dev->debug_time[RESUME][0]);
		dpm_print("\t resume time end: %d\n",dev->debug_time[RESUME][1]);
		dpm_print("\t resume late time begin: %d\n",dev->debug_time[RES_LATE][0]);
		dpm_print("\t resume late time end: %d\n",dev->debug_time[RES_LATE][1]);
		dpm_print("\t complete time begin: %d\n",dev->debug_time[COMPLETE][0]);
		dpm_print("\t complete time end: %d\n",dev->debug_time[COMPLETE][1]);
		dpm_print("\t pre_failed:%d,susearly_failed:%d,sus_failed:%d,late_failed:%d",\
					dev->debug_count[PREPARE],dev->debug_count[SUS_EARLY],dev->debug_count[SUSPEND],dev->debug_count[LATE]);
		dpm_print("\t ear_failed:%d,res_failed:%d,reslate_failed:%d,com_failed:%d",\
					dev->debug_count[EARLY],dev->debug_count[RESUME],dev->debug_count[RES_LATE],dev->debug_count[COMPLETE]);
	}
	dpm_print("----------------------------------------------------------------\n");
	dpm_print("-------------------dpm prepared  list info:  dev_name:-----------------------\n");
	list_for_each_entry(dev,&(dpm_ctrl.dpm_prepared_list),entry)
	{
		dpm_print("\t\t%s:\n",dev->device_name);
	}
	dpm_print("----------------------------------------------------------------\n");
	dpm_print("--------------------dpm sus early list info:   dev_name:---------------------------\n");
	list_for_each_entry(dev,&(dpm_ctrl.dpm_sus_early_list),entry)
	{
		dpm_print("\t\t%s:\n",dev->device_name);
	}
	dpm_print("--------------------dpm suspend list info:   dev_name:---------------------------\n");
	list_for_each_entry(dev,&(dpm_ctrl.dpm_suspend_list),entry)
	{
		dpm_print("\t\t%s:\n",dev->device_name);
	}
	dpm_print("----------------------------------------------------------------\n");
	dpm_print("--------------------dpm late early list info:   dev_name:---------------------------\n");
	list_for_each_entry(dev,&(dpm_ctrl.dpm_late_early_list),entry)
	{
		dpm_print("\t\t%s:\n",dev->device_name);
	}
	dpm_print("----------------------------------------------------------------\n");
}

void bsp_dpm_init(void)
{
	int i = 0;
	INIT_LIST_HEAD(&(dpm_ctrl.dpm_list));
	INIT_LIST_HEAD(&(dpm_ctrl.dpm_prepared_list));
	INIT_LIST_HEAD(&(dpm_ctrl.dpm_sus_early_list));
	INIT_LIST_HEAD(&(dpm_ctrl.dpm_suspend_list));
	INIT_LIST_HEAD(&(dpm_ctrl.dpm_late_early_list));
	spin_lock_init(&dpm_ctrl.dpm_list_mtx);
	for(i = 0;i<DPM_CALLBACK_NUM;i++)
		dpm_ctrl.debug_count[i]=0;
	dpm_ctrl.current_callback = NULL;
	dpm_ctrl.flag = 0;
	bsp_trace(BSP_LOG_LEVEL_ERROR,BSP_MODU_DPM,"c core dpm init success\n");
	return;
}
/*lint -restore +e516*/


