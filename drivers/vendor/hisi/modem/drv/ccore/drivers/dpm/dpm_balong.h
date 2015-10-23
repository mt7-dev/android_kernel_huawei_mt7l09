#ifndef __DPM_BALONG_H__
#define __DPM_BALONG_H__
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <osl_list.h>
#include <osl_types.h>
#include <osl_spinlock.h>

#ifndef OK
#define OK 0
#endif
#ifndef ERROR
#define ERROR (-1)
#endif

struct dpm_control
{
    struct list_head dpm_list;                      /*记录在dpm模块注册过的模块                 */
    struct list_head dpm_prepared_list;             /*记录已经prepare和resume_late过的模块                   */
    struct list_head dpm_sus_early_list;            /*记录已经suspend early和resume模块 */
    struct list_head dpm_suspend_list;              /*记录已经suspend和resume early的模块                 */
    struct list_head dpm_late_early_list;           /*记录已经suspend late模块 */
    spinlock_t    dpm_list_mtx;                      /*链表访问互斥信号量，两个链表用一个互斥信号量*/
    u32              debug_count[DPM_CALLBACK_NUM];/*0:pre failed,1:suspend failed,2:late failed,3:total suspend,4:total resume*/
    s32 (*current_callback)(struct dpm_device *dev);
    char               *device_name;                    /*记录模块名字*/
    s32 flag;
};
#endif
