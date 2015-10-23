/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  wakelock_debug.c
*
*   作    者 :  xujingcui
*
*   描    述 :  用于A核wakelock debug
*
*   修改记录 :  2013年5月10日  v1.00 xujingcui创建
**************************************************************************
*此处接口用于调试，请勿使用
*
*************************************************************************/
#include "wakelock_debug.h"

/**************************************************************************
*此处接口用于wakelock LLT，请勿使用
***************************************************************************/
#define LOCK_TEST_OK        0
#define LOCK_TEST_ERROR  1

static struct wake_lock llt_lock;

int wake_lock_init_case0(void)
{
    wake_lock_init(&llt_lock, WAKE_LOCK_SUSPEND, "test_wakelock");
    return LOCK_TEST_OK;
}
int wake_lock_case0(void)
{
    int ret = 0;
    wake_lock(&llt_lock);
    ret = wake_lock_active (&llt_lock);
    if (ret)
        return LOCK_TEST_OK;
    else
        return LOCK_TEST_ERROR;
}

int wake_unlock_case0(void)
{
    int ret = 0;
    wake_unlock(&llt_lock);
    ret = wake_lock_active (&llt_lock);
    if (!ret)
        return LOCK_TEST_OK;
    else
        return LOCK_TEST_ERROR;
}

void debug_wakelock(void)
{
    int i = 0;
   vote_printf("All module vote status(1: oppose sleeping, 0: approve sleeping)\n");
    for (i = 0; i < wakelock_count; i++){
        if(hi6930_wakelock[i].lock){
                int ret = ((unsigned int)hi6930_wakelock[i].lock->flags >> 9) & 0x3;
                if (ret)
                    ret = 1;
                else
                    ret = 0;
                vote_printf("lock name : %-20s, %d, LOCK_ID:%d\n",hi6930_wakelock[i].lock->name, ret, hi6930_wakelock[i].lock_id);
        }
   }
    vote_printf("wake lock:         debug_wake_lock(LOCK_ID)\n");
    vote_printf("wake unlock:       debug_wake_unlock(LOCK_ID)\n");
    vote_printf("wake_lock_destroy: debug_wake_lock_destroy(LOCK_ID)\n");
    return;
}

void debug_wake_lock(int id)
{
      int ret = 0;
      struct wake_lock *lock = hi6930_wakelock[id].lock;
      wake_lock(lock);
      ret = ((unsigned int)lock->flags >> 9) & 0x3;
      if (ret){
            ret = 1;
            vote_printf("wak lock name : %s, status :%d \n", lock->name, ret);
            return;
      }
      ret = 0;
      vote_printf("wak lock name : %s, status :%d \n", lock->name, ret);
      return;
}

void debug_wake_unlock(int id)
{
      int ret = 0;
      struct wake_lock *lock = hi6930_wakelock[id].lock;
      wake_unlock(lock);
      ret = ((unsigned int)lock->flags >> 9) & 0x3;
      if (ret){
            ret = 1;
            vote_printf("wak lock name : %s, status :%d \n", lock->name, ret);
            return;
      }
      ret = 0;
      vote_printf("wak lock name : %s, status :%d \n", lock->name, ret);
      return;
}
void debug_wake_lock_destroy(int id)
{
      struct wake_lock *lock = hi6930_wakelock[id].lock;
      wake_lock_destroy(lock);
      return;
}
/*以下部分用于测试MNTN功能 */
void test_wakelock(int num)
{
	/* coverity[var_decl] */
	struct wake_lock test_lock;
	int i = 0;
	while(i <= num){
        wake_lock_init(&test_lock, WAKE_LOCK_SUSPEND, "test_lock");
		/* coverity[uninit_use_in_call] */
	    wake_lock_destroy(&test_lock);
		i++;
	}
	return;
}
void test_wakelock32(int num)
{
    int i = 0;
	for(i = 0; i < num; i++){
	    struct wake_lock test_lock;
	    wake_lock_init(&test_lock, WAKE_LOCK_SUSPEND, "test1_lock");
	}
	return;
}

