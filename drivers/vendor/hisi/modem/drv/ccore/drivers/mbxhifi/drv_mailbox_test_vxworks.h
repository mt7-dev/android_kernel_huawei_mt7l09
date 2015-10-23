
#ifndef __DRV_MAILBOX_MNTN_H__
#define __DRV_MAILBOX_MNTN_H__

/*****************************************************************************
  1 头文件包含
*****************************************************************************/



#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define TM_MAILBOX_INIT_FLAG          (0x5aa55aa5)

#define TM_MAILBOX_BYTE_PER_KB        (1024)

 /*计算带宽的乘法上限*/
#define MAILBOX_BOARDST_BOUNDWIDTH_MUL_LIMIT   30

/*****************************************************************************
  3 枚举定义
*****************************************************************************/

/*****************************************************************************
  4 消息头定义
*****************************************************************************/


/*****************************************************************************
  5 消息定义
*****************************************************************************/


/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/

/*****************************************************************************
 实 体 名  : struct tm_mb_handle
 功能描述  : 板侧测试模块控制句柄
*****************************************************************************/
struct tm_mb_handle
{
    void *          mutex;
    unsigned long   init_flag;
};

/*****************************************************************************
 实 体 名  : struct tm_mb_cb
 功能描述  : 板侧测试的用户回调句柄
*****************************************************************************/
struct tm_mb_cb
{
    void *           finish_sema;
    void *           sync_sema;
    unsigned long    start_slice;       /*for calculate boundwidth*/
    unsigned long    end_slice;         /*for calculate boundwidth*/
    unsigned long    prob_slice;        /*for calculate latency*/
    unsigned long    back_slice;        /*for calculate latency*/
    unsigned long    msg_count;
    unsigned long    task_count;
    unsigned long    check_ret;
};


/*****************************************************************************
  7 UNION定义
*****************************************************************************/

/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/
extern unsigned long tm_mailbox_msg_multi_send(
                unsigned long dst_id, 
                unsigned long carrier_id,
                unsigned long task_num, 
                unsigned long msg_len,
                unsigned long msg_num, 
                unsigned long priority,
                unsigned long delay_tick,
                unsigned long delay_num,
                unsigned long try_times
);

extern unsigned long tm_mailbox_msg_multi_test(
                unsigned long dst_id, 
                unsigned long task_num, 
                unsigned long msg_num, 
                unsigned long delay_num);

extern unsigned long tm_mailbox_msg_single_test(
                unsigned long dst_id, 
                unsigned long msg_len, 
                unsigned long msg_num, 
                unsigned long try_times);

extern void tm_mailbox_msg_reg(unsigned long channel_id);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of tm_drv_mailbox.h */
