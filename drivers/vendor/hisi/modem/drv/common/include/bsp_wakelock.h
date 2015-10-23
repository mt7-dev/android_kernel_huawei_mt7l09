

#ifndef _BSP_WAKELOCK_H
#define _BSP_WAKELOCK_H
#ifdef __cplusplus
extern "C" {
#endif

#include <product_config.h>
#include <drv_wakelock.h>
#include <bsp_om.h>

#define  vote_printf(fmt, ...)    (bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_WAKELOCK, "[wakelock]: <%s> "fmt"", __FUNCTION__, ##__VA_ARGS__))

struct wake_lock {
	unsigned int   lockid;  /*lockid 来自于drv_wakelock.h中定义的LOCKID的值*/
      const char   *name;
};


/***********************************************************************
*唤醒锁初始化
*入参:唤醒锁指针，锁ID，唤醒锁名称
*返回值:无
*lock_id 来自于drv_enum.h中定义的LOCKID的值
***********************************************************************/
 void wake_lock_init(struct wake_lock *lock, int lock_id, const char *name);

/***********************************************************************
*获取唤醒锁
*入参:唤醒锁指针
*返回值:无
***********************************************************************/
 void wake_lock(struct wake_lock *lock);

/***********************************************************************
*释放唤醒锁
*入参:唤醒锁指针
*返回值:无
***********************************************************************/
void wake_unlock(struct wake_lock *lock);

/***********************************************************************
*查询某一个唤醒锁状态
*入参:唤醒锁指针
*返回值:
*1:上锁
*0:释放
***********************************************************************/
int wake_lock_active(struct wake_lock *lock);

/***********************************************************************
*查询某一类型唤醒锁状态
*入参:唤醒锁类型WAKE_LOCK_SUSPEND
*返回值:
*0:无锁
*-1:有锁
***********************************************************************/
unsigned int  has_wake_lock(int type);

/***********************************************************************
*此函数用于产品线standby低功耗模式清理所有投票
*入参:NO
*返回值:
***********************************************************************/
void all_vote_wake_unlock(void);
#ifdef __cplusplus
}
#endif

#endif

