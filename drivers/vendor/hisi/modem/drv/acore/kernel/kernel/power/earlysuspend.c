/* kernel/power/earlysuspend.c
 *
 * Copyright (C) 2005-2008 Google, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/earlysuspend.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/rtc.h>
#include <linux/syscalls.h> /* sys_sync */
#include <linux/wakelock.h>
#include <linux/workqueue.h>
#include <osl_types.h>
#include "power.h"

/*for debug --xujingcui*/
#ifdef CONFIG_WAKELOCK_BALONG
#include <bsp_lowpower_mntn.h>
#include "wakelock_debug.h"
#endif

enum {
	DEBUG_USER_STATE = 1U << 0,
	DEBUG_SUSPEND = 1U << 2,
	DEBUG_VERBOSE = 1U << 3,
};
static int debug_mask = DEBUG_USER_STATE|DEBUG_SUSPEND|DEBUG_VERBOSE;
module_param_named(debug_mask, debug_mask, int, S_IRUGO | S_IWUSR | S_IWGRP);

static DEFINE_MUTEX(early_suspend_lock);
static LIST_HEAD(early_suspend_handlers);
static void early_suspend(struct work_struct *work);
static void late_resume(struct work_struct *work);
static DECLARE_WORK(early_suspend_work, early_suspend);
static DECLARE_WORK(late_resume_work, late_resume);
static DEFINE_SPINLOCK(state_lock);
enum {
	SUSPEND_REQUESTED = 0x1,
	SUSPENDED = 0x2,
	SUSPEND_REQUESTED_AND_SUSPENDED = SUSPEND_REQUESTED | SUSPENDED,
};
static int state = 0;

void register_early_suspend(struct early_suspend *handler)
{
	struct list_head *pos = NULL;

	mutex_lock(&early_suspend_lock);
	list_for_each(pos, &early_suspend_handlers) {
		struct early_suspend *e;
		e = list_entry(pos, struct early_suspend, link);
		if (e->level > handler->level)
        {
            earlysuspend_print_dbg("find the target");
			break;
        }
	}
	list_add_tail(&handler->link, pos);
	if ((state & SUSPENDED) && handler->suspend)
		handler->suspend(handler);
    earlysuspend_print_dbg("state = %x", state);
	mutex_unlock(&early_suspend_lock);
}
EXPORT_SYMBOL(register_early_suspend);

void unregister_early_suspend(struct early_suspend *handler)
{
	mutex_lock(&early_suspend_lock);
	list_del(&handler->link);
	mutex_unlock(&early_suspend_lock);
}
EXPORT_SYMBOL(unregister_early_suspend);

static void early_suspend(struct work_struct *work)
{
	struct early_suspend *pos = NULL;
	unsigned long irqflags = 0;
	int abort = 0;

	mutex_lock(&early_suspend_lock);
	spin_lock_irqsave(&state_lock, irqflags);
	if (state == SUSPEND_REQUESTED)
		state |= SUSPENDED;
	else
		abort = 1;    
#ifdef CONFIG_WAKELOCK_BALONG
    suspend_info_record.enter_early_suspend_count ++;
    suspend_info_record.enter_early_suspend_time_stamp = bsp_get_slice_value();
#endif
	spin_unlock_irqrestore(&state_lock, irqflags);

	if (abort) {
		if (debug_mask & DEBUG_SUSPEND)
			earlysuspend_print_dbg("early_suspend: abort, state %d", state);
		mutex_unlock(&early_suspend_lock);
		goto abort;
	}

	if (debug_mask & DEBUG_SUSPEND)
		earlysuspend_print_dbg("early_suspend: call handlers");
	list_for_each_entry(pos, &early_suspend_handlers, link) {
		if (pos->suspend != NULL) {
			if (debug_mask & DEBUG_VERBOSE)
				earlysuspend_print_dbg("early_suspend: calling %pf", pos->suspend);            
			pos->suspend(pos);
		}
	}
	mutex_unlock(&early_suspend_lock);

	if (debug_mask & DEBUG_SUSPEND)
		earlysuspend_print_dbg("early_suspend: sync");

	/*sys_sync();此接口不能保证系统调用完成，所以替换为suspend_sys_sync_queue()*/
    sys_sync();
abort:
	spin_lock_irqsave(&state_lock, irqflags);
	if (state == SUSPEND_REQUESTED_AND_SUSPENDED)
		wake_unlock(&main_wake_lock);    
#ifdef CONFIG_WAKELOCK_BALONG
        suspend_info_record.quit_early_suspend_count ++;
        suspend_info_record.quit_early_suspend_time_stamp = bsp_get_slice_value();
#endif
	spin_unlock_irqrestore(&state_lock, irqflags);
}

static void late_resume(struct work_struct *work)
{
	struct early_suspend *pos = NULL;
	unsigned long irqflags = 0;
	int abort = 0;

	mutex_lock(&early_suspend_lock);
	spin_lock_irqsave(&state_lock, irqflags);
	if (state == SUSPENDED)
		state = state&(~SUSPENDED);
	else
		abort = 1;
    
#ifdef CONFIG_WAKELOCK_BALONG
        suspend_info_record.enter_late_resume_count ++;
        suspend_info_record.enter_late_resume_time_stamp = bsp_get_slice_value();
#endif
	spin_unlock_irqrestore(&state_lock, irqflags);

	if (abort) {
		if (debug_mask & DEBUG_SUSPEND)
			earlysuspend_print_dbg("late_resume: abort, state %d", state);
		goto abort;
	}
	if (debug_mask & DEBUG_SUSPEND)
		earlysuspend_print_dbg("late_resume: call handlers\n");
	list_for_each_entry_reverse(pos, &early_suspend_handlers, link) {
		if (pos->resume != NULL) {
			if (debug_mask & DEBUG_VERBOSE)
				earlysuspend_print_dbg("late_resume: calling %pf", pos->resume);
			pos->resume(pos);
		}
	}
	if (debug_mask & DEBUG_SUSPEND)
		earlysuspend_print_dbg("late_resume: done");
abort:    
#ifdef CONFIG_WAKELOCK_BALONG
    suspend_info_record.quit_late_resume_count ++;
    suspend_info_record.quit_late_resume_time_stamp = bsp_get_slice_value();
#endif
	mutex_unlock(&early_suspend_lock);
}

void request_suspend_state(suspend_state_t new_state)
{
	unsigned long irqflags = 0;
	int old_sleep = 0;

	spin_lock_irqsave(&state_lock, irqflags);
    earlysuspend_print_dbg("the state = %d", state);

	old_sleep = state & SUSPEND_REQUESTED;
	if (debug_mask & DEBUG_USER_STATE) {
		struct timespec ts;
		struct rtc_time tm;
		getnstimeofday(&ts);
		rtc_time_to_tm(ts.tv_sec, &tm);
		earlysuspend_print_dbg("request_suspend_state: %s (%d->%d) at %lld "
			"(%d-%02d-%02d %02d:%02d:%02d.%09lu UTC)\n",
			new_state != PM_SUSPEND_ON ? "sleep" : "wakeup",
			requested_suspend_state, new_state,
			ktime_to_ns(ktime_get()),
			tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
			tm.tm_hour, tm.tm_min, tm.tm_sec, ts.tv_nsec);
	}
	/*如果新状态为休眠态*/
	if (!old_sleep && new_state != PM_SUSPEND_ON)
    {
		state = state|SUSPEND_REQUESTED;
		/*执行浅度睡眠的工作队列*/
		queue_work(suspend_work_queue, &early_suspend_work);
	}
	/*如果新状态为唤醒状态*/
	else if (old_sleep && new_state == PM_SUSPEND_ON)
	    {
		state = state&(~SUSPEND_REQUESTED);
		/*激活内核锁*/
		wake_lock(&main_wake_lock);
                /*执行浅度唤醒的工作队列*/
		queue_work(suspend_work_queue, &late_resume_work);
	    }
	/*更新全局状态*/
	requested_suspend_state = new_state;
	spin_unlock_irqrestore(&state_lock, irqflags);
}

suspend_state_t get_suspend_state(void)
{
	return requested_suspend_state;
}
