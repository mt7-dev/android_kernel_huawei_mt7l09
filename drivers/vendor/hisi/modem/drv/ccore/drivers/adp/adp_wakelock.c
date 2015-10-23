
#include "drv_wakelock.h"
#include "bsp_wakelock.h"

unsigned int BSP_PWRCTRL_SleepVoteLock(PWC_CLIENT_ID_E id)
{
    struct wake_lock lock;
    lock.lockid = id;
    wake_lock(&lock);
    return 0;
}
unsigned int BSP_PWRCTRL_SleepVoteUnLock(PWC_CLIENT_ID_E id)
{
    struct wake_lock lock;
    lock.lockid = id;
    wake_unlock(&lock);
    return 0;
}

