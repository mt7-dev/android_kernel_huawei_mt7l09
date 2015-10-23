

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include <vxWorks.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <logLib.h>
#include <semLib.h>
#include <intLib.h>
#include <memLib.h>
#include <sysLib.h>
#include <taskLib.h>

#include "drv_mailbox.h"
#include "drv_mailbox_cfg.h"
#include "drv_mailbox_debug.h"
#include "drv_mailbox_gut.h"
#include "osl_sem.h"
#include "drv_ipc.h"
#include "drv_timer.h"
#include "osl_bio.h"   /*lint !e537*/

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  1 可维可测信息中包含的C文件编号宏定义
*****************************************************************************/
#undef  _MAILBOX_FILE_
#define _MAILBOX_FILE_   "vxworks"

#define MAILBOX_MILLISEC_PER_SECOND                         1000

#define  MAILBOX_VXWORKS_SEND_FULL_DELAY_MS                 10          /*发送满延迟毫秒*/

#ifdef _DRV_LLT_
#define  MAILBOX_VXWORKS_SEND_FULL_DELAY_TIMES              500         /*发送满延迟尝试次数*/
#else
#define  MAILBOX_VXWORKS_SEND_FULL_DELAY_TIMES              0          /*发送满延迟尝试次数*/
#endif

#ifndef WAIT_FOREVER
#define WAIT_FOREVER	(-1)
#endif

/*****************************************************************************
  定义C核vxworks操作系统中邮箱数据的处理接口调用方式
*****************************************************************************/
enum MAILBOX_VXWORKS_PROC_STYLE_E
{
    MAILBOX_VXWORKS_PROC_STYLE_SEND_RESERVED   = 0,  /*0表示发送通道*/

    /*接收处理方式定义开始*/
    MAILBOX_VXWORKS_PROC_STYLE_RECEV_START,

    /*挂接在任务上的邮件处理队列开始*/
    MAILBOX_VXWORKS_PROC_STYLE_TASK_RESERVED,

    MAILBOX_VXWORKS_PROC_STYLE_TASK_MSG,
    MAILBOX_VXWORKS_PROC_STYLE_TASK_IFC ,

    /*挂接在任务上的邮件处理队列结束*/
    MAILBOX_VXWORKS_PROC_STYLE_TASK_BUTT,

    /*在中断中处理的邮件处理队列*/
    MAILBOX_VXWORKS_PROC_STYLE_INT_NORMAL,


    /*接收处理方式定义结束*/
    MAILBOX_VXWORKS_PROC_STYLE_RECEV_END

};

/*****************************************************************************
    定义C核邮箱单个任务挂接的邮箱工作队列及其数据
*****************************************************************************/
struct mb_vx_work
{
    unsigned long                               channel_id;      /*邮箱ID号，可能是核间连接ID,也可能是物理通道ID*/
    unsigned long                               data_flag;       /*此邮箱是否有数据的标志位*/
    long								(*cb)(unsigned long channel_id);
    struct mb_vx_work                           *next;          /*指向下一条*/
#ifdef MAILBOX_OPEN_MNTN
    struct mb_buff                              *mb_priv;
#endif

};

/*****************************************************************************
  定义C核邮箱任务相关的数据
*****************************************************************************/
struct mb_vx_proc
{
    signed char                                 name[16];   /*处理方式名*/
    unsigned long                               id;         /*处理方式ID号*/
    signed long                                 priority;   /*处理方式优先级*/
    signed long                                 stack;      /*任务栈大小*/
    osl_sem_id                                  sema;       /*任务等待的消息*/
    struct mb_vx_work                          *vxqueue;    /*此任务挂接的邮箱工作队列*/
};
/*****************************************************************************
  定义邮箱物理通道和任务，核间中断及接收回调函数之间的对应关系
*****************************************************************************/
struct mb_vx_cfg
{
    unsigned long                               channel_id;      /*邮箱通道ID。*/
    unsigned long                               proc_style;      /*如果是收通道，定义处理邮箱工作队列的任务ID。*/
    unsigned long                               Int_src;         /*邮箱通道所使用的核间中断资源号。*/
    unsigned long                               dst_cpu;          /*邮箱通道所使用的核间中断目标CPU号*/
};

//extern int read_efuse(unsigned int group, unsigned int *buf, unsigned int len);
void mailbox_usr_init(void);

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
/*邮箱任务属性列表*/
MAILBOX_LOCAL struct mb_vx_proc g_mailbox_vx_proc_tbl[] =
{
    /*处理方式名*/    /*处理方式ID号*/                         /*优先级*/   /*栈大小*/

    /*任务方式的邮件数据处理*/
    {"mailboxMsg"   ,MAILBOX_VXWORKS_PROC_STYLE_TASK_MSG,     4,          4096        },
    {"mailboxIfc"   ,MAILBOX_VXWORKS_PROC_STYLE_TASK_IFC,     5,          4096        },

    /*中断方式的邮件数据处理*/
    {"mailboxInt"   ,MAILBOX_VXWORKS_PROC_STYLE_INT_NORMAL,     0,          0         },

    /*请在此后新增处理方式项目，否则会影响UT用例*/

    /*结束*/
};

/*C核的邮箱通道资源与平台系统资源对应关系配置表*/
MAILBOX_LOCAL struct mb_vx_cfg g_mailbox_vx_cfg_tbl[] =
{
#if 0
    /*接收通道的配置*/
    /*ChannelID*/                                       /*通道处理方式ID号*/
    {MAILBOX_MAILCODE_RESERVED(MCU,  CCPU, MSG),    MAILBOX_VXWORKS_PROC_STYLE_TASK_MSG      },
    {MAILBOX_MAILCODE_RESERVED(HIFI, CCPU, MSG),    MAILBOX_VXWORKS_PROC_STYLE_TASK_MSG      },
    {MAILBOX_MAILCODE_RESERVED(ACPU, CCPU, MSG),    MAILBOX_VXWORKS_PROC_STYLE_TASK_MSG      },
    {MAILBOX_MAILCODE_RESERVED(ACPU, CCPU, IFC),    MAILBOX_VXWORKS_PROC_STYLE_TASK_IFC      },
    {MAILBOX_MAILCODE_RESERVED(MCU,  CCPU, IFC),    MAILBOX_VXWORKS_PROC_STYLE_TASK_IFC      },
    /*发送通道配置*/
    /*ChannelID*/                                       /*通道处理方式ID号*/
    {MAILBOX_MAILCODE_RESERVED( CCPU, MCU,  MSG),   MAILBOX_VXWORKS_PROC_STYLE_SEND_RESERVED    },
    {MAILBOX_MAILCODE_RESERVED( CCPU, HIFI, MSG),   MAILBOX_VXWORKS_PROC_STYLE_SEND_RESERVED    },
    {MAILBOX_MAILCODE_RESERVED( CCPU, ACPU, MSG),   MAILBOX_VXWORKS_PROC_STYLE_SEND_RESERVED    },
    {MAILBOX_MAILCODE_RESERVED( CCPU, ACPU, IFC),   MAILBOX_VXWORKS_PROC_STYLE_SEND_RESERVED    },
    {MAILBOX_MAILCODE_RESERVED( CCPU, MCU, IFC),    MAILBOX_VXWORKS_PROC_STYLE_SEND_RESERVED    },
#else
    /*接收通道的配置*/
    /*ChannelID*/                                       /*通道处理方式ID号*/
    {MAILBOX_MAILCODE_RESERVED(HIFI, CCPU, MSG),    MAILBOX_VXWORKS_PROC_STYLE_TASK_MSG      },
    /*发送通道配置*/
    /*ChannelID*/                                       /*通道处理方式ID号*/
    {MAILBOX_MAILCODE_RESERVED( CCPU, HIFI, MSG),   MAILBOX_VXWORKS_PROC_STYLE_SEND_RESERVED    },
#endif
    /*结束标志*/
    {MAILBOX_MAILCODE_INVALID, 0,0}
};

MAILBOX_LOCAL void *mailbox_mutex_create(void);
/*****************************************************************************
  3 函数定义
*****************************************************************************/

MAILBOX_LOCAL void mailbox_receive_task(
                osl_sem_id         *sema,
                struct mb_vx_work  **work_list )
{
    struct mb_vx_work    *work   =   MAILBOX_NULL;

    /* coverity[no_escape] */
    while (1) { /*lint !e716*/
        if (MAILBOX_OK == osl_sem_downtimeout(sema, WAIT_FOREVER)) {
            work = *work_list;
            while (MAILBOX_NULL != work) {
                /*遍历标志位，如果有置位，调用对应的邮箱ID号的回调函数*/
                if (MAILBOX_TRUE == work->data_flag) {
                    work->data_flag = MAILBOX_FALSE;
 #ifdef MAILBOX_OPEN_MNTN
                mailbox_record_sche_recv(work->mb_priv);
 #endif
                    if (work->cb) {
                        if (MAILBOX_OK !=  work->cb(work->channel_id)) {
                             (void)mailbox_logerro_p1(MAILBOX_ERR_VXWORKS_CALLBACK_ERRO, work->channel_id);
                        }
                    } else {
                        (void)mailbox_logerro_p1(MAILBOX_ERR_VXWORKS_CALLBACK_NOT_FIND, work->channel_id);
                        #ifdef _DRV_LLT_
                        /*PC UT 用例保证能够跳出*/
                        return MAILBOX_ERR_VXWORKS_CALLBACK_NOT_FIND;
                        #endif
                    }
                }
                work = work->next;
            }
        }
    }
}

/*本邮箱核的对外提供接口*/

/*lint -save -e64*/
MAILBOX_EXTERN long mailbox_init_platform(void)
{
    struct mb_vx_proc   *process    = &g_mailbox_vx_proc_tbl[0];
    unsigned long        count      =  sizeof(g_mailbox_vx_proc_tbl) /
                                       sizeof(struct mb_vx_proc);
    unsigned long        proc_id;

    /*创建平台任务中断信号量部分*/
    while (count) {
        /*为任务处理方式的邮箱通道创建任务*/
        proc_id = process->id;
        if ((proc_id > MAILBOX_VXWORKS_PROC_STYLE_TASK_RESERVED) && (proc_id < MAILBOX_VXWORKS_PROC_STYLE_TASK_BUTT)) {
            /* 创建邮箱接收任务等待信号量*/
            osl_sem_init(0, &process->sema);

            /* 创建邮箱收数据处理任务*/
            if (ERROR == (int)taskSpawn((char *)process->name, (int)process->priority, (int)0,
                            (int)process->stack,
                            (void*)mailbox_receive_task,
                            (int)(&process->sema),
                            (int)(&process->vxqueue),
                            (int)0, (int)0, (int)0, (int)0, (int)0, (int)0, (int)0, (int)0)) /*lint !e119 !e64*/
            {
                return mailbox_logerro_p1(MAILBOX_ERR_VXWORKS_TASK_CREATE, proc_id);
            }
        }
        count--;
        process++;
    }

    mailbox_usr_init();

    return MAILBOX_OK;
}
/*lint -restore*/

MAILBOX_LOCAL long mailbox_ipc_process(
                struct mb_vx_work  *work,
                struct mb_vx_proc  *proc,
                unsigned long channel_id,
                unsigned long proc_id)
{
    unsigned long is_find = MAILBOX_FALSE;

    while (work) {
        /*从邮箱工作队列中找到对应的邮箱，设置标志位并释放信号量通知处理任务*/
        if (channel_id  == work->channel_id) {
            if ((proc_id > MAILBOX_VXWORKS_PROC_STYLE_TASK_RESERVED) && (proc_id < MAILBOX_VXWORKS_PROC_STYLE_TASK_BUTT)) {
                is_find = MAILBOX_TRUE;

                /*设置任务邮箱工作队列链表中此邮箱的数据标志位*/
                work->data_flag = MAILBOX_TRUE;
 #ifdef MAILBOX_OPEN_MNTN
                mailbox_record_sche_send(work->mb_priv);
 #endif
                /*释放信号量，通知任务*/
                osl_sem_up(&proc->sema);
            } else if (MAILBOX_VXWORKS_PROC_STYLE_INT_NORMAL == proc_id) {
                 /*中断处理方式，在中断中直接处理邮箱数据*/
                if (work->cb) {
                    is_find = MAILBOX_TRUE;

                    if (MAILBOX_OK !=  work->cb(work->channel_id)) {
                         (void)mailbox_logerro_p1(MAILBOX_ERR_VXWORKS_CALLBACK_ERRO, work->channel_id);
                    }
                } else {
                     (void)mailbox_logerro_p1(MAILBOX_ERR_VXWORKS_CALLBACK_NOT_FIND, work->channel_id);
                }
            } else {
                is_find = MAILBOX_FALSE;
            }

        }
        work = work->next;
    }

    return (long)is_find;
}

int g_hifimailbox = 0;


MAILBOX_LOCAL long mailbox_ipc_int_handle(unsigned long int_src)
{
    struct mb_vx_cfg        *cfg    =  &g_mailbox_vx_cfg_tbl[0];
    struct mb_vx_proc       *proc   =   MAILBOX_NULL;
    struct mb_vx_work       *work   =   MAILBOX_NULL;
    unsigned long count = sizeof(g_mailbox_vx_proc_tbl)/sizeof(struct mb_vx_proc);
    unsigned long proc_id = 0;
    unsigned long channel_id = 0;
    unsigned long is_find = MAILBOX_FALSE;
    unsigned long ret_val = MAILBOX_OK;

    if(MAILBOX_INIT_MAGIC != g_mailbox_handle.init_flag)
    {
        return (long)ret_val;
    }

    g_hifimailbox++;

    /*找到传入ID对应的邮箱配置*/
    while (MAILBOX_MAILCODE_INVALID != cfg->channel_id) {
        /*处理所有挂接到这个中断号的接收邮箱通道*/
        proc_id = cfg->proc_style;
        if ((int_src == cfg->Int_src) && (MAILBOX_VXWORKS_PROC_STYLE_SEND_RESERVED != cfg->proc_style)) {
            channel_id = cfg->channel_id;

            proc   =  &g_mailbox_vx_proc_tbl[0];
            count          =   sizeof(g_mailbox_vx_proc_tbl)/sizeof(struct mb_vx_proc);
            while (count) {
                /*找到此邮箱通道对应的任务信息*/
                if (proc_id == proc->id) {
                    work = proc->vxqueue;
                    is_find = (unsigned long)mailbox_ipc_process( work,
                                                 proc,
                                                 channel_id,
                                                 proc_id);
                    break;
                }
                count--;
                proc++;
            }

            if (0 == count) {
                 ret_val = (unsigned long)mailbox_logerro_p1(MAILBOX_ERR_VXWORKS_MAIL_TASK_NOT_FIND, channel_id);
            }
        }
        cfg++;
    }

    if (MAILBOX_TRUE != is_find) {
         ret_val = (unsigned long)mailbox_logerro_p1(MAILBOX_ERR_VXWORKS_MAIL_INT_NOT_FIND, channel_id);
    }

    return (long)ret_val;
}
MAILBOX_EXTERN long mailbox_process_register(
                unsigned long channel_id,
                 long (*cb)(unsigned long channel_id),
                 void *priv)
{
    struct mb_vx_work       *work    =   MAILBOX_NULL;
    struct mb_vx_cfg        *cfg  =  &g_mailbox_vx_cfg_tbl[0];
    struct mb_vx_proc       *proc =  &g_mailbox_vx_proc_tbl[0];
    struct mb_vx_cfg*        cfg_find     =   MAILBOX_NULL;
    unsigned long count = sizeof(g_mailbox_vx_proc_tbl)/sizeof(struct mb_vx_proc);

    while (MAILBOX_MAILCODE_INVALID != cfg->channel_id) {
        /*找到与传入邮箱ID最适配的系统邮箱配置*/
        if (cfg->channel_id ==  channel_id) {
            cfg_find = cfg;
            break;
        }
        cfg++;
    }

    if (cfg_find) {
        /*加入邮箱任务对应的邮箱工作队列*/
        while (count) {
            if (cfg_find->proc_style == proc->id) {
                if (proc->vxqueue) {
                    work = proc->vxqueue;
                    while (MAILBOX_NULL != work->next) {
                        work = work->next;
                    }
                    work->next            = (struct mb_vx_work *)calloc(
                                                    sizeof(struct mb_vx_work), 1);
                    if (MAILBOX_NULL == work->next) {
                        return mailbox_logerro_p0(MAILBOX_ERR_VXWORKS_ALLOC_MEMORY);

                    }
                    work->next->channel_id = cfg_find->channel_id;
                    work->next->cb  = cb;
                    work->next->mb_priv = priv;
                } else {
                    proc->vxqueue     = (struct mb_vx_work *)calloc(
                                                   sizeof(struct mb_vx_work), 1);
                    if (MAILBOX_NULL == proc->vxqueue) {
                        return mailbox_logerro_p0(MAILBOX_ERR_VXWORKS_ALLOC_MEMORY);
                    }
                    proc->vxqueue->channel_id = cfg_find->channel_id;
                    proc->vxqueue->cb  = cb;
                    proc->vxqueue->mb_priv = priv;
                }
            }
            count--;
            proc++;
        }

        return MAILBOX_OK;
    }

    return  mailbox_logerro_p1(MAILBOX_ERR_VXWORKS_CHANNEL_NOT_FIND, channel_id);

}

extern void tm_mailbox_msg_reg(unsigned long channel_id);


MAILBOX_EXTERN long mailbox_channel_register(
                unsigned long channel_id,
                unsigned long int_src,
                unsigned long dst_id,
                unsigned long direct,
                void   **mutex)
{
    struct mb_vx_cfg    *cfg  =  &g_mailbox_vx_cfg_tbl[0];

    while (MAILBOX_MAILCODE_INVALID != cfg->channel_id) {
        if (channel_id == cfg->channel_id) {
            *mutex = mailbox_mutex_create();

            /*通道资源配置已找到，注册IPC中断*/
            cfg->Int_src = int_src;
            cfg->dst_cpu  = dst_id;
            if(MIALBOX_DIRECTION_RECEIVE == direct) {
                (void)BSP_IPC_IntConnect((IPC_INT_LEV_E)int_src , (VOIDFUNCPTR)mailbox_ipc_int_handle, int_src);
                (void)BSP_IPC_IntEnable ((IPC_INT_LEV_E)int_src);

                /*板侧ST用例通道注册*/
                tm_mailbox_msg_reg(channel_id);

                /*TODO: 如果开发单核下电重启特性，这里需要主动触发一次通道读回调:  */
                /*mailbox_ipc_int_handle(int_src);*/
            }

            return MAILBOX_OK;
        }
        cfg++;
    }

    /*如果适配层找不到对应的配置，报错*/
    return mailbox_logerro_p1(MAILBOX_ERR_VXWORKS_CHANNEL_NOT_FIND, channel_id);

}
MAILBOX_EXTERN long mailbox_delivery(unsigned long channel_id)
{
    struct mb_vx_cfg     *cfg       =  &g_mailbox_vx_cfg_tbl[0];
    struct mb_vx_cfg     *cfg_find  =   MAILBOX_NULL;

    while (MAILBOX_MAILCODE_INVALID != cfg->channel_id) {
        /*找到与传入邮箱ID最适配的系统邮箱配置*/
        if (cfg->channel_id == channel_id) {
            cfg_find = cfg;
            break;
        }
        cfg++;
    }

    cache_sync();

    if (MAILBOX_NULL != cfg_find) {
        return (long)BSP_IPC_IntSend((IPC_INT_CORE_E)cfg_find->dst_cpu, (IPC_INT_LEV_E)cfg_find->Int_src);
    }

    return mailbox_logerro_p1(MAILBOX_ERR_VXWORKS_CHANNEL_NOT_FIND, channel_id);
}
MAILBOX_LOCAL void *mailbox_mutex_create(void)
{
    return  (void *)semMCreate(SEM_Q_PRIORITY | SEM_INVERSION_SAFE);
}


MAILBOX_EXTERN long mailbox_mutex_lock(void **mutexId)
{

    return semTake(*mutexId, WAIT_FOREVER);
}



MAILBOX_EXTERN void mailbox_mutex_unlock(void **mutex_id)
{
    (void)semGive((SEM_ID)*mutex_id);
}
/*lint -save -e64*/
MAILBOX_EXTERN void *mailbox_init_completion(void)
{
    return  (void *)semBCreate(SEM_Q_PRIORITY, 0);
}
/*lint -restore*/

MAILBOX_EXTERN long  mailbox_wait_completion(void **sema_id, unsigned long timeout)
{
    long ret =  (long)semTake(*sema_id, (int)(timeout/10)); /*vxworks's tick is 10 millisecond*/
    return ret;
}

MAILBOX_EXTERN void mailbox_complete(void **sema_id)
{
    (void)semGive((SEM_ID)*sema_id);
}

MAILBOX_EXTERN void mailbox_del_completion(void **wait)
{
    (void)semDelete(*wait);
}
MAILBOX_EXTERN void *mailbox_memcpy(void *dst, const void *src, long size)
{
    return (void *)memcpy(dst, src, (unsigned int)size);  /*lint !e516*/
}
MAILBOX_EXTERN void *mailbox_memset(void * m, long c, unsigned long size)
{
    return memset(m, c, (u32)size);  /*lint !e516*/
}
/*lint -save -e715*/
MAILBOX_EXTERN void mailbox_assert(unsigned long ErroNo)
{
#ifndef _DRV_LLT_
    unsigned int ticks = 0;

    ticks = (unsigned int)sysClkRateGet();
    ticks++;

    /* coverity[no_escape] */
    while (1) /*lint !e716*/
    {
        printf("mb Assert!!");

        (void)taskDelay((int)ticks);

    }
#endif
}
/*lint -restore*/


/*lint -save -e18 -e64*/
MAILBOX_EXTERN long mailbox_int_context(void)
{
    return  INT_CONTEXT();
}
/*lint -restore*/


/*lint -save -e685 -e568*/
MAILBOX_EXTERN long mailbox_scene_delay(unsigned long scene_id, unsigned long *try_times)
{
    unsigned long go_on = MAILBOX_FALSE;
    unsigned long delay_ms = 0;
    unsigned int ticks = 0;
    
    switch (scene_id) {
        case MAILBOX_DELAY_SCENE_MSG_FULL: 
        case MAILBOX_DELAY_SCENE_IFC_FULL:
            delay_ms = MAILBOX_VXWORKS_SEND_FULL_DELAY_MS; 
            /* coverity[unsigned_compare] */
            go_on = (*try_times >= MAILBOX_VXWORKS_SEND_FULL_DELAY_TIMES) ? 
                     MAILBOX_FALSE : MAILBOX_TRUE;
            break;
        default:
            break;
    }

    if (MAILBOX_TRUE == go_on) {
        ticks = (delay_ms * sysClkRateGet()) / MAILBOX_MILLISEC_PER_SECOND; /*lint !e737*/
        ticks++;

    #ifndef _DRV_LLT_ /*taskDelay()影响UT覆盖率和测试效率*/
        (void)taskDelay((int)ticks);
    #endif
    }

    *try_times = *try_times + 1;
    return (long)go_on;
}
/*lint -restore*/

MAILBOX_EXTERN long mailbox_get_timestamp(void)
{

#ifndef _DRV_LLT_
    return (long)BSP_GetSliceValue();
#else
    return 0;
#endif
}

#if 0
#ifndef _DRV_LLT_
IFC_GEN_EXEC3(read_efuse,
			  IFC_INCNT, unsigned int, group, 0,
			  IFC_OUTVAR, unsigned int*, buf, 0,
			  IFC_INCNT, unsigned int, len, 0)

#else
extern int ife_read_efuse(unsigned int group, unsigned int *buf, unsigned int len);
#endif
#endif

void mailbox_usr_init(void)
{
//    MAILBOX_IFC_REG(MAILBOX_IFC_ACPU_TO_CCPU_READ_EFUSE, read_efuse);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

