
#include <linux/earlysuspend.h>
#include <linux/suspend.h>
#include "osl_types.h"
#include <linux/workqueue.h>

extern struct workqueue_struct *suspend_work_queue;
extern struct work_struct early_suspend_work;
extern struct work_struct late_resume_work;

u32 g_ulEarlySuspend1 = 0;
u32 g_ulEarlySuspend2 = 0;

void earlysuspend_suspend0 (struct early_suspend *h)
{
    g_ulEarlySuspend1 = 1;
    earlysuspend_print_dbg("earlysuspend_suspend0");
}
void earlysuspend_resume0(struct early_suspend *h)
{
    g_ulEarlySuspend1 = 0;
    earlysuspend_print_dbg("earlysuspend_resume0");
}
void earlysuspend_suspend1 (struct early_suspend *h)
{
    g_ulEarlySuspend2 = 1;
    earlysuspend_print_dbg("earlysuspend_suspend1");

}
void earlysuspend_resume1(struct early_suspend *h)
{
    g_ulEarlySuspend2 = 0;
    earlysuspend_print_dbg("earlysuspend_resume1");

}

u32 earlysuspend_show()
{
    printk("g_ulEarlySuspend1: = %x", g_ulEarlySuspend1);
    printk("g_ulEarlySuspend2: = %x", g_ulEarlySuspend2);
}

struct early_suspend earlysuspend_struct_test01 =
{
    NULL,
    EARLY_SUSPEND_LEVEL_BLANK_SCREEN,
    earlysuspend_suspend0,
    earlysuspend_resume0
};
struct early_suspend earlysuspend_struct_test02 =
{
    NULL,
    EARLY_SUSPEND_LEVEL_STOP_DRAWING,
    earlysuspend_suspend1,
    earlysuspend_resume1
};



s32 earlysuspend_test_register()
{
    register_early_suspend(&earlysuspend_struct_test01);
    register_early_suspend(&earlysuspend_struct_test02);
    return 0;
}

s32 earlysuspend_test_unregister()
{
    unregister_early_suspend(&earlysuspend_struct_test01);
    unregister_early_suspend(&earlysuspend_struct_test02);
    return 0;
}

