#ifndef __BSP_SOFTTIMER_H__
#define __BSP_SOFTTIMER_H__

#include <osl_common.h>
#include <osl_list.h>
#include <product_config.h>
typedef void (*softtimer_func)(u32);
#define TIMER_TRUE 1
#define TIMER_FALSE 0
#define TIMER_INIT_FLAG     0X5A5A5A5A
/*结构体和枚举定义*/
enum wakeup
{
    SOFTTIMER_WAKE,
    SOFTTIMER_NOWAKE
};
struct softtimer_list
{
   softtimer_func func;
   u32 para;
   u32 timeout;
   enum wakeup wake_type;
   
   /*使用者不用关注和配置以下信息*/
   struct list_head entry;
   u32 timer_id;
   u32 count_num;/*原始配置起始计数时间*/
   u32 is_running;
   u32 init_flags;
};

#ifdef CONFIG_MODULE_TIMER
int  bsp_softtimer_init(void);
/*****************************************************************************
* 函 数 名  :bsp_softtimer_create_timer
*
* 功能描述  : 创建一个软timer，分配软timer id，
*
* 输入参数  :  要softtimer  创建的软timer的信息
* 输出参数  : 无
* 返 回 值  : OK&ERROR
*
* 修改记录  :  2013年2月28日   lixiaojie     
*****************************************************************************/
s32 bsp_softtimer_create(struct softtimer_list *softtimer);
/*****************************************************************************
* 函 数 名  :     bsp_softtimer_delete
*
* 功能描述  : 从软timer队列删除一个还没有到超时时间的
                            单次执行的定时器
*
* 输入参数  :  timer 要删除的timer 
                             的参数
* 输出参数  : 无
*
* 返 回 值  :     0   删除成功
                             1   要删除的定时器不在超时队列
                             -1 传入的timer指针为空
*
* 修改记录  :  2013年2月28日   lixiaojie     
*****************************************************************************/
s32 bsp_softtimer_delete(struct softtimer_list * softtimer);
/*****************************************************************************
* 函 数 名  :bsp_softtimer_delete_sync
*
* 功能描述  : 从软timer队列删除一个重复执行的软timer
*
* 输入参数  :  timer 要删除的timer ，为bsp_softtimer_create_timer函数的返回值
* 输出参数  : 无
*
* 返 回 值  : 0   删除成功
                             1   要删除的定时器不在超时队列
                             -1 传入的timer指针为空
*
* 修改记录  :  2013年2月28日   lixiaojie     
*****************************************************************************/

s32 bsp_softtimer_delete_sync(struct softtimer_list * timer);

/*****************************************************************************
* 函 数 名  :bsp_softtimer_modify
*
* 功能描述  : 修改软timer超时时间，修改完成后，需要调用
*                           bsp_softtimer_add进行添加操作
*
* 输入参数  :  timer: 要修改的软timer，为bsp_softtimer_create_timer函数传出
                             的参数
                             new_expire_time: 新超时时间
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 修改记录  :  2013年2月28日   lixiaojie     
*****************************************************************************/

s32 bsp_softtimer_modify(struct softtimer_list * softtimer,u32 new_expire_time);
/*****************************************************************************
* 函 数 名  :   bsp_softtimer_add
*
* 功能描述  : 将softtimer加入超时队列，如果超时定时器需要多次重复执行，
*                           需要在回调函数中调用本接口进行添加操作
*
* 输入参数  :  要加入队列的定时器指针
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 修改记录  :  2013年2月28日   lixiaojie     
*****************************************************************************/

void bsp_softtimer_add(struct softtimer_list * timer);
/*****************************************************************************
* 函 数 名  :bsp_softtimer_free
*
* 功能描述  : 释放定时器占用的资源
*
* 输入参数  :  要释放的定时器指针
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 修改记录  :  2013年2月28日   lixiaojie     
*****************************************************************************/

s32 bsp_softtimer_free(struct softtimer_list *timer);
#else
static inline int  bsp_softtimer_init(void) {return 0;}
static inline s32  bsp_softtimer_create(struct softtimer_list *softtimer) {return 0;}
static inline s32 bsp_softtimer_delete(struct softtimer_list * softtimer) {return 0;}
static inline s32 bsp_softtimer_delete_sync(struct softtimer_list * timer) {return 0;}
static inline s32 bsp_softtimer_modify(struct softtimer_list * softtimer,u32 new_expire_time) {return 0;}
static inline void bsp_softtimer_add(struct softtimer_list * timer) {}
static inline s32 bsp_softtimer_free(struct softtimer_list *timer) {return 0;}
#endif
#endif
