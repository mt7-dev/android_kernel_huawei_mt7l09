/* cshell.h -  balong cshell driver header file */

/* Copyright (C) 2012 Huawei Corporation */

#ifndef    _CSHELL_H_
#define    _CSHELL_H_

/**************************************************************************
                                              头文件定义
**************************************************************************/

#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/semaphore.h>


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**************************************************************************
                                               宏定义
**************************************************************************/
#define ACM_CHN_CSHELL                  2
#define CSHELL_BUFFER_SIZE              8192
#define CSHELL_ICC_CHANNEL_FIFO_SIZE    8192
#define CSHELL_ICC_CHANNEL_ID           31
#define CSHELL_ICC_CHANNEL_INVALID_ID   0xFFFF
#define CSHELL_SEND_BUFFER_NUM          8
#define CSHELL_OK                       0
#define CSHELL_ERROR                    (-1)
#define CHAR_BIT_NUM                    8
#define RECV_CMD_STR_LEN                256

enum usb_filenode_open_list
{
	USB_CSHELL,
	FILE_CSHELL,
	AUART_CSHELL,
	ALL_CSHELL,
	INVAL_CSHELL
};

/**************************************************************************
                                                    STRUCT定义
**************************************************************************/
typedef struct cshell_mem_handle
{
    u8  *   buf;
    u32     buf_size;
    u32     buf_valid;
    u32     index;
    struct  cshell_mem_handle *next;
}cshell_mem_handle_t;

typedef struct
{
    u32                 icc_chan_opened;
    u32                 icc_channel_id ;
    u32                 acm_channel_id;
    s32                 cshell_acm_fd;
    u32                 valid;
    cshell_mem_handle_t recv_mem;
    cshell_mem_handle_t send_mem[CSHELL_SEND_BUFFER_NUM];
    struct semaphore    cshell_send_sem;
    struct semaphore    cshell_recv_sem;
    u32                 cshell_send_index;
    u8 *                recv_buf;
    u8 *                send_buf;
    struct task_struct *send_task;  //A->PC
    struct task_struct *recv_task;  //A->ICC

    u32                 reset_infor_fail_times;
    u8                  acshell_permit;        /* 0:not permit to send log to acore; others, permit*/
    spinlock_t          cshell_spin_loc_permit;/* lock to pretect  acshell_permit*/
    u8                  ccshell_work_flg;      /* 1:ccore working; 0:ccore stop */
    u8                  recv_cmd_str[RECV_CMD_STR_LEN];

    u8*                 usb_send_buf;
    u32                 usb_send_buf_size;
    u32                 usb_send_buf_r;
    u32                 usb_send_buf_w;
    struct semaphore    cshell_usb_send_sem;
    struct task_struct *send_task_usb;

}cshell_ctx_t;

struct cshell_debug_log
{
	u32 usb_buf_drop_times;
	u32 send_mem_drop_times;
	u32 auart_send_cmd_times;
	u32 bluetooth_send_cmd_times;
};

/**************************************************************************
                                                    函数声明
**************************************************************************/
int cshell_set_bit(int num_from_zero);
int cshell_clear_bit(int num_from_zero);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _CSHELL_H_ */

