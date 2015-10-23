

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

#if 0
#include "excDrv.h"
#include "BSP.h"
#include "BSP_TIMER.h"
#include "drv_mailbox.h"
#include "bsp_mailbox.h"
#include "drv_mailbox_cfg.h"
#include "drv_mailbox_debug.h"
#include "drv_mailbox_gut.h"
#else
#include "drv_mailbox.h"
#include "drv_mailbox_cfg.h"
#include "drv_mailbox_debug.h"
#include "drv_mailbox_port_vxworks.h"
#endif

#include "drv_mailbox_test_vxworks.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define  Second_To_Millisecond                               1000

/*****************************************************************************
  1 可维可测信息中包含的C文件编号宏定义
*****************************************************************************/
#undef  _MAILBOX_FILE_
#define _MAILBOX_FILE_   "tm"

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

static const unsigned char g_tm_mailbox_data_str[] = {"Use this page to search the product documentation and Microsoft Partner sites for information. To access this page, choose Search from the Help menu.\
Security Note Microsoft does not guarantee or endorse search results provided by third-parties, such as Codezone Community participants. Use caution when downloading and installing components from online sources.\
<search box>Enter a word or phrase to search all text or select a previous search string from the drop-down list.\
SearchStarts the search based on the search expression you provided. By default, the search expression is applied to all of the text within all of the topics that are included in the scope of the current filter. Searches return a maximum of 500 matches.\
<filter area>Allows you to narrow your search results by selecting options in the Language, Technology, and Topic Type categories. \
Note Topics that do not use these categories to define their topics are excluded from the search even if the content in those topics does match your search query. Choose Unfiltered to search all topics.\
If your product includes pre-defined search filters, additional Help might appear below.\
Click the minus (-) sign to hide the filter-related UI to provide a larger viewing space for search results.\
LanguageLists the programming languages, such as C#, available to filter your search.\
TechnologyLists technologies, such as .NET Framework and Office Development, available to filter your search.\
Topic TypeLists content types for topics, such as Contains Code and How-Tos, available to filter your search.\
Search ResultsUse this section to review the list of topics, technical articles, and other information returned from a search. For each result, an abstract, source, and other information is displayed.Tip  \
You can choose not to display abstracts for search results in General, Help, Options Dialog Box.\
Searched for: <query>A list of results that meet your search criteria found in the selected Help source, such as Local Help or Questions. Sort by\
Specifies the criteria that results are sorted by. The default options available might not apply to all types of results. Available sorting criteria for a result is listed below the abstract for each item.\
For example, search results from Local Help do not include Topic Date or Rating, so selecting either of these options will not affect the order in which results are displayed for Local Help.\
Descending or AscendingDescending sorts the results from A-Z for the Sort by criteria selected. Ascending sorts the results from Z-A for the Sort by criteria selected.\
Source The physical source of the results, such as Local Help and MSDN Online. Choosing a source displays the search results for that source. You can change the sources searched and the order in which source results are displayed in Online, Help, Options Dialog Box.\
"};

struct tm_mb_handle g_tm_mailbox_handle = {0};
/*****************************************************************************
  3 函数定义
*****************************************************************************/

/*PC模拟板侧ST的收邮件通知函数*/

extern void st_mailbox_notify(unsigned long MailCode);
static int fls(int x)
{
    int r = 32;

    if (0 == x) {
        return 0;
    }
    if (0 == (x & 0xffff0000u)) {
        x <<= 16;
        r -= 16;
    }
    if (0 == (x & 0xff000000u)) {
        x <<= 8;
        r -= 8;
    }
    if (0 == (x & 0xf0000000u)) {
        x <<= 4;
        r -= 4;
    }
    if (0 == (x & 0xc0000000u)) {
        x <<= 2;
        r -= 2;
    }
    if (0 == (x & 0x80000000u)) {
        x <<= 1; /* [false alarm]:fortify */
        r -= 1;
    }
    return r;
}
MAILBOX_LOCAL unsigned long tm_mailbox_get_bound_width(
                unsigned long total_size,
                unsigned long time_diff)
{
    unsigned long temp_value;
    unsigned long bound_width = 0;

    if ((fls(total_size) + fls(Second_To_Millisecond)) < MAILBOX_BOARDST_BOUNDWIDTH_MUL_LIMIT) {
        temp_value = total_size * Second_To_Millisecond;
        if ((fls(temp_value) + fls(Second_To_Millisecond))  < MAILBOX_BOARDST_BOUNDWIDTH_MUL_LIMIT) {
            temp_value = temp_value * Second_To_Millisecond;
            bound_width = temp_value / TM_MAILBOX_BYTE_PER_KB / time_diff;
        } else {
            temp_value = temp_value / TM_MAILBOX_BYTE_PER_KB;
            if ((fls(temp_value) + fls(Second_To_Millisecond))  < MAILBOX_BOARDST_BOUNDWIDTH_MUL_LIMIT) {
                temp_value = temp_value * Second_To_Millisecond;
                bound_width = temp_value /  time_diff;
            }
            else {
                temp_value = temp_value / time_diff;
                bound_width = temp_value * Second_To_Millisecond;
            }
        }
    } else {
        temp_value = total_size / TM_MAILBOX_BYTE_PER_KB;
        if ((fls(temp_value) + fls(Second_To_Millisecond))  < MAILBOX_BOARDST_BOUNDWIDTH_MUL_LIMIT) {
            temp_value = temp_value * Second_To_Millisecond;
            if ((fls(temp_value) + fls(Second_To_Millisecond))  < MAILBOX_BOARDST_BOUNDWIDTH_MUL_LIMIT) {
                temp_value = temp_value * Second_To_Millisecond;
                bound_width = temp_value / time_diff;
            } else {
                temp_value = temp_value / time_diff;
                bound_width = temp_value * Second_To_Millisecond;
            }
        } else {
            temp_value = temp_value / time_diff; /* [false alarm]:fortify */
            bound_width = bound_width *  Second_To_Millisecond * Second_To_Millisecond;
        }
    }

    return bound_width;
}
MAILBOX_EXTERN unsigned long tm_mailbox_get_time_stamp(void)
{
    static unsigned long time_us = 0;

#ifndef _DRV_LLT_
        time_us = BSP_GetSliceValue();
#else
        time_us--;
#endif

    return  time_us;
}


char* tm_mailbox_msg_pack(unsigned long rcode, unsigned long size)
{
    struct mb_st_msg *usr_str = MAILBOX_NULL;
    int pack_size = sizeof(struct mb_st_msg) + size;

    if((pack_size > (int)sizeof(g_tm_mailbox_data_str)) ||
        (pack_size > MAILBOX_MCU_TEST_BUFF_SIZE)) {
        printf("error tm_mailbox_msg_pack ,size too large(0x%x),(%d)\n", (unsigned int)size, _MAILBOX_LINE_);
        return MAILBOX_NULL;
    }
    
    /* coverity[alloc_fn] */
    usr_str = (struct mb_st_msg *)calloc((unsigned long)(sizeof(struct mb_st_msg) + size), 1);
    if (MAILBOX_NULL == usr_str) {
        printf("error tm_mailbox_msg_pack ,calloc line(%d)\n",_MAILBOX_LINE_);
        return MAILBOX_NULL;
    }

    usr_str->protect = MAILBOX_BOARDST_USER_PROTECT1;
    usr_str->length = size;
    usr_str->back_code = rcode;
    usr_str->test_id = MAILBOX_BOARDST_ID_LOOP_SEND;

    memcpy((usr_str + 1), &g_tm_mailbox_data_str[0], size);

    return (char*)usr_str;
}
void tm_mailbox_msg_discard(void * buff)
{
    free(buff);
}


void tm_mailbox_msg_cb(
                void                   *usr_handle,
                void                   *mail_handle,
                unsigned long           mail_len)
{
    /* coverity[alloc_fn] */
    char* buff = (char*)calloc(mail_len, 1); /* [false alarm]:fortify */
    struct mb_st_msg *test_handle = 0;
    unsigned char* dst_buff = 0;
    struct tm_mb_cb* read_cb = usr_handle;
    const unsigned char* src_buff = 0;

    if (MAILBOX_NULL == buff) {
        printf("error tm_mailbox_msg_cb ,calloc line(%d)\n",_MAILBOX_LINE_);
        return ;
    }

    mailbox_read_msg_data(mail_handle, buff, &mail_len);

    test_handle = (struct mb_st_msg *)buff;

    if (MAILBOX_BOARDST_USER_PROTECT1 !=  test_handle->protect) {
        /*无效的消息*/
        printf("error tm_mailbox_msg_cb: PROTECT \n");
        if ((MAILBOX_NULL != read_cb) && (MAILBOX_NULL != read_cb->finish_sema)) {
            semGive(read_cb->finish_sema);
        }
        return ;
    }

#ifdef _DRV_LLT_
    /*PC 验证自发自收，不存在真正的 MAILBOX_BOARDST_ID_LOOP_SEND 阶段*/
    test_handle->test_id = MAILBOX_BOARDST_ID_LOOP_BACK;
#endif

    switch (test_handle->test_id)
    {
        case MAILBOX_BOARDST_ID_LOOP_SEND:
            /*接收到发送过来测试消息，通过消息中定义的返回通道返回同样的消息*/
            test_handle->test_id = MAILBOX_BOARDST_ID_LOOP_BACK;
            (void)mailbox_send_msg(test_handle->back_code, (void*)(test_handle), mail_len);

            break;
        case MAILBOX_BOARDST_ID_LOOP_BACK:

            if(MAILBOX_NULL == read_cb)
            {
                printf("eror tm_mailbox_msg_cb: read_cb is NULL\n"); /* [false alarm]:fortify */
                break;
            }

            read_cb->back_slice = read_cb->back_slice ? read_cb->back_slice : tm_mailbox_get_time_stamp(); /* [false alarm]:fortify */
            /*回环过来的消息，验证消息的正确性*/
            dst_buff = (unsigned char*)(test_handle + 1);
            src_buff = (g_tm_mailbox_data_str);

            /*compare data to check validity*/
            if (0 !=  memcmp((const void*)dst_buff, src_buff, test_handle->length ))
            {
                read_cb->check_ret = MAILBOX_ERRO;
            }
            else
            {
                if (MAILBOX_ERRO != read_cb->check_ret)
                {
                    read_cb->check_ret = MAILBOX_OK;
                }
            }

            read_cb->msg_count--;

            if (0 == read_cb->msg_count)
            {
                printf("TM_MAILBOX_UserCallBack: MsgCount goto 0\n");
                if (MAILBOX_NULL != read_cb->finish_sema)
                {
                    semGive(read_cb->finish_sema);
                }
            }
            break ;
        default:
                printf("eror tm_mailbox_msg_cb: TestID is Wrong\n");
                if((MAILBOX_NULL != read_cb) && (MAILBOX_NULL != read_cb->finish_sema))
                {
                    semGive(read_cb->finish_sema);
                }

            break;
    }

    if (MAILBOX_NULL != buff) {
        free(buff);
    }
}
unsigned long tm_mailbox_msg_task_create(FUNCPTR entry,
                unsigned long task_num,
                unsigned long priority,
                unsigned long send_code,
                unsigned long recv_code,
                unsigned long send_len,
                unsigned long send_num,
                unsigned long delay_tick,
                unsigned long delay_num,
                unsigned long try_times,
                struct tm_mb_cb * mb_cb
)
{
    char TaskName[255];
    
    /* coverity[secure_coding] */
    sprintf(TaskName,"tm_mailbox_msg_multi_send(%u)",(unsigned int)task_num);
    return taskSpawn(TaskName, priority, 0, 1024, entry,
            send_code, recv_code, send_len, send_num, delay_tick, delay_num, try_times, (unsigned long)mb_cb, 0, 0); /* [false alarm]:fortify */

}


void tm_mailbox_msg_process(
                unsigned long send_code,
                unsigned long recev_code,
                unsigned long send_size,
                unsigned long msg_num,
                unsigned long delay_tick,
                unsigned long delay_num,
                unsigned long try_times,
                struct tm_mb_cb * mb_cb)
{
    char* pData = 0;
    unsigned long RetVal = 0;
    unsigned long FullWaitTimes = try_times; /* [false alarm]:fortify */
    unsigned long maxsize = send_size + 50;
    unsigned long minsize = send_size;

    mb_cb->task_count-- ;

    if (0 == mb_cb->task_count) {
        if (MAILBOX_NULL != mb_cb->sync_sema) {
            mb_cb->start_slice = tm_mailbox_get_time_stamp();
            semGive(mb_cb->sync_sema);
        }
    }

    if (MAILBOX_NULL != mb_cb->sync_sema) {
        /*多进程同步*/
        semTake(mb_cb->sync_sema, WAIT_FOREVER);
        semGive(mb_cb->sync_sema);
    }

    while (msg_num) {
        if(MAILBOX_NULL == mb_cb->finish_sema) {
            break;
        }

        if (send_size > maxsize ) {
            send_size = minsize;
        } else {
            send_size ++;
        }
        /* coverity[alloc_fn] */
        pData = tm_mailbox_msg_pack(recev_code, send_size);
        if(MAILBOX_NULL == pData) {
            semGive(mb_cb->finish_sema);
            return;
        }

        /*call test object API*/
        mb_cb->prob_slice = mb_cb->prob_slice ? mb_cb->prob_slice : tm_mailbox_get_time_stamp();
        RetVal =  mailbox_send_msg(
            send_code, pData, send_size + sizeof(struct mb_st_msg)); /* [false alarm]:fortify */

#ifdef _DRV_LLT_
        /*PC需要模拟对方核发送中断*/
        st_mailbox_notify(send_code);
#endif
        if (MAILBOX_FULL == RetVal) {
            taskDelay(1); /* [false alarm]:fortify */
            FullWaitTimes--;

            if (0 !=  FullWaitTimes) {
                continue;
            } else {
                printf("error tm_mailbox_msg_process full too many times\n");
                tm_mailbox_msg_discard(pData);
                if(MAILBOX_NULL !=mb_cb->finish_sema)
                {
                    semGive(mb_cb->finish_sema);
                }
                return;
            }
        }
        else if (MAILBOX_OK != RetVal ) {
            tm_mailbox_msg_discard(pData);
            if (MAILBOX_NULL !=mb_cb->finish_sema) {
                semGive(mb_cb->finish_sema);
            }
            return;
        }

        if ((0 != delay_tick)  && (0 != delay_num) && (0 == msg_num%delay_num)) { /* [false alarm]:fortify */
            taskDelay(delay_tick); /* [false alarm]:fortify */
        }

        msg_num--;
        tm_mailbox_msg_discard(pData);
    }

}
unsigned long tm_mailbox_msg_get_code(
                unsigned long dst_id,
                unsigned long carrier_id,
                unsigned long *send_code,
                unsigned long *recv_code)
{
    unsigned long SrcID = MAILBOX_LOCAL_CPUID;

    *send_code = MAILBOX_MAILCODE_CHANNEL(SrcID, dst_id, carrier_id);
    *recv_code = MAILBOX_MAILCODE_CHANNEL(dst_id, SrcID, carrier_id);

    return MAILBOX_OK;

}


unsigned long tm_mailbox_msg_multi_send(
                unsigned long dst_id,
                unsigned long carrier_id,
                unsigned long task_num,
                unsigned long msg_len,
                unsigned long msg_num,
                unsigned long priority,
                unsigned long delay_tick,
                unsigned long delay_num,
                unsigned long try_times
)
{
    unsigned long ret_val = MAILBOX_OK;
    unsigned long send_code;
    unsigned long recv_code;
    unsigned long bound_width;
    unsigned long data_size;
    unsigned long time_diff;
    struct tm_mb_cb * mb_cb = 0;
    struct tm_mb_handle *tm_handle = &g_tm_mailbox_handle;

    if (MAILBOX_OK != tm_mailbox_msg_get_code(dst_id, carrier_id, &send_code, &recv_code) ) {
        return MAILBOX_ERRO;
    }

    if ((0 == msg_len) || (0 == msg_num)) {
        printf("error tm_mailbox_msg_multi_send input param,line(%d)\n",_MAILBOX_LINE_);
        return MAILBOX_ERRO;
    }

    if (0 == priority) {
        priority = 1;
    }

    /*保证每个测试的原子操作*/
    if(tm_handle->mutex) {
        /* coverity[lock] */
        semTake(tm_handle->mutex, WAIT_FOREVER);
    } else {
        printf("error TM_MAILBOX_MultiSend tm_handle->mutex is NULL\n");
        return MAILBOX_ERRO;
    }

    /*创建回调用户句柄*/
    mb_cb = calloc(sizeof(struct tm_mb_cb), 1);
    if (!mb_cb) {
        printf("error tm_mailbox_msg_multi_send calloc,line(%d)\n",_MAILBOX_LINE_); /* [false alarm]:fortify */
        return MAILBOX_ERRO;
    }

    mb_cb->finish_sema  = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY);
    mb_cb->sync_sema  = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY);
    mb_cb->check_ret = MAILBOX_CRIT_RET_START;

    /*注册消息响应接收回调*/
    mailbox_reg_msg_cb(recv_code, tm_mailbox_msg_cb, mb_cb);

    mb_cb->task_count = task_num;

    data_size = task_num * msg_num * (msg_len + task_num/2);

    /*创建诺干个任务，任务入口函数为本核往其他核发送消息的函数*/
    while (task_num) {
        mb_cb->msg_count += msg_num;
       printf("tm_mailbox_msg_multi_send(%d) MailCode(0x%08x), RecevCode(0x%08x), \n\r\
        msg_len(%d), \n\r\
        msg_num(%d),\n\r\
        delay_tick(%d),\n\r\
        delay_num(%d), \n\r\
        priority(%d), \n\r\
        try_times(%d)\n",
       (int)task_num, (int)send_code, (int)recv_code, (int)msg_len, (int)msg_num, (int)delay_tick, (int)delay_num, (int)priority, (int)try_times);

        tm_mailbox_msg_task_create((FUNCPTR)tm_mailbox_msg_process,
                     task_num,
                     priority,
                     send_code,
                     recv_code,
                     msg_len,
                     msg_num,
                     delay_tick,
                     delay_num,
                     try_times,
                     mb_cb
                    );

        task_num--;
        msg_len++;
    }

    semTake(mb_cb->finish_sema, WAIT_FOREVER);
    semDelete(mb_cb->finish_sema);
    mb_cb->finish_sema = MAILBOX_NULL;
    semDelete(mb_cb->sync_sema);
    mb_cb->sync_sema = MAILBOX_NULL;

    mb_cb->end_slice= tm_mailbox_get_time_stamp();

    time_diff = mailbox_get_slice_diff(mb_cb->start_slice, mb_cb->end_slice);

    bound_width = tm_mailbox_get_bound_width(data_size, time_diff);

    printf("total:%d(Byte), bindwidth: %d(KB/Sec), latency: %d (us)\n",
              (int)data_size, (int)bound_width, (int)time_diff);

    if(MAILBOX_OK != mb_cb->check_ret) {
        printf("tm_mailbox_msg_multi_send: CheckRet error\n");
        ret_val = MAILBOX_ERRO;
    }
    else if(0 != mb_cb->msg_count) {
        printf("tm_mailbox_msg_multi_send: MsgCount(%d) error\n", (int)mb_cb->msg_count);
        ret_val = MAILBOX_ERRO;
    } else {
        printf("tm_mailbox_msg_multi_send: Success!\n");
    }

    free(mb_cb);

    if(tm_handle->mutex) {
        semGive(tm_handle->mutex);
    }
    return ret_val;

}


unsigned long tm_mailbox_msg_single_test(
                unsigned long dst_id,
                unsigned long msg_len,
                unsigned long msg_num,
                unsigned long delay_num
                )
{
    unsigned long delay_tick = 1;
    unsigned long priority  = 100;
    unsigned long task_num   = 1;
    unsigned long carrier_id = 1;
    unsigned long try_times = 0;

    return tm_mailbox_msg_multi_send(
             dst_id,
             carrier_id,
             task_num,
             msg_len,
             msg_num,
             priority,
             delay_tick,
             delay_num,
             try_times
            );
}


unsigned long tm_mailbox_msg_multi_test(
                unsigned long dst_id,
                unsigned long task_num,
                unsigned long msg_num,
                unsigned long delay_num)
{
    unsigned long msg_len = 9;
    unsigned long delay_tick = 1;
    unsigned long priority = 100;
    unsigned long carrier_id = 1;
    unsigned long try_times  = 20;

    return tm_mailbox_msg_multi_send(
             dst_id,
             carrier_id,
             task_num,
             msg_len,
             msg_num,
             priority,
             delay_tick,
             delay_num,
             try_times
            );
}


void tm_mailbox_init(void)
{
    struct tm_mb_handle *tm_handle = &g_tm_mailbox_handle;

    if (TM_MAILBOX_INIT_FLAG == tm_handle->init_flag) {
        return;
    }

    tm_handle->mutex  = semMCreate(SEM_Q_PRIORITY | SEM_DELETE_SAFE | SEM_INVERSION_SAFE);

    tm_handle->init_flag = TM_MAILBOX_INIT_FLAG;
}


void tm_mailbox_msg_reg(unsigned long channel_id)
{
    /*通道中的第一个MailCode为保留测试号，这个MailCode和此通道的ChannelID一致*/
    unsigned long recv_code = channel_id; /* [false alarm]:fortify */

    tm_mailbox_init();

    /*注册消息响应接收回调*/
    mailbox_reg_msg_cb(recv_code, tm_mailbox_msg_cb, MAILBOX_NULL);

}

#ifndef _DRV_LLT_
#define IFC_ACORE_IFC_MAX_LEN                  (512)  /* the max length of written log. */

/*C核宏*/

#if 0
IFC_GEN_CALL2( MAILBOX_IFC_CCPU_TO_ACPU_PRINT, acore_print_p0,
                 IFC_INVAR, const char *  , string , 0,
                 IFC_INCNT, int, strlen, 0
                )

#if 0
IFC_GEN_CALL1( MAILBOX_IFC_CCPU_TO_ACPU_PRINT, ifc_print_p1,
                 IFC_INFIX, const char *, string ,sizeof(const char)
                )

IFC_GEN_EXEC1(ife_print_p1,
               IFC_INFIX, const char *, string , sizeof(const char))

IFC_GEN_CALL0( MAILBOX_IFC_CCPU_TO_ACPU_PRINT, ifc_print_p0)

IFC_GEN_EXEC0(ife_print_p0)
#endif

IFC_GEN_CALL4(MAILBOX_IFC_CCPU_TO_ACPU_FOPEN, acore_fopen,
                IFC_INVAR, const char *, filename, 0,
                IFC_INCNT, int , name_len , 0,
                IFC_INVAR, const char *, mode, 0,
                IFC_INCNT, int , mode_len , 0
                    )

IFC_GEN_CALL1(MAILBOX_IFC_CCPU_TO_ACPU_FCLOSE, acore_fclose,
                      IFC_INCNT, void* , stream , 0)

IFC_GEN_CALL3(MAILBOX_IFC_CCPU_TO_ACPU_FREAD, acore_fread,
                 IFC_OUTVAR, const char *, filename, 0,
                 IFC_INCNT, unsigned int  , count , 0,
                  IFC_INCNT, long  , fp , 0
                )

IFC_GEN_CALL3(MAILBOX_IFC_CCPU_TO_ACPU_FWRITE, acore_fwrite,
                 IFC_INVAR, const char *, filename, 0,
                 IFC_INCNT, unsigned int  , count , 0,
                 IFC_INCNT, long  , fp , 0
                )

IFC_GEN_CALL3(MAILBOX_IFC_CCPU_TO_ACPU_FSEEK, acore_fseek,
                 IFC_INCNT, void *  , fp , 0,
                 IFC_INCNT, long, offset, 0,
                 IFC_INCNT,  int  , whence , 0
                )

IFC_GEN_CALL2(MAILBOX_IFC_CCPU_TO_ACPU_REMOVE, acore_remove,
                 IFC_INVAR, const char *  , pathname , 0,
                 IFC_INCNT, int, path_len, 0
                )

IFC_GEN_CALL1(MAILBOX_IFC_CCPU_TO_ACPU_FTELL, acore_ftell,
                      IFC_INCNT, void* , fp , 0)

IFC_GEN_CALL4(MAILBOX_IFC_CCPU_TO_ACPU_RENAME, acore_rename,
                IFC_INVAR, const char *, oldname, 0,
                IFC_INCNT, int , oldlen , 0,
                IFC_INVAR, const char *, newname, 0,
                IFC_INCNT, int , newlen , 0
                    )

IFC_GEN_CALL3(MAILBOX_IFC_CCPU_TO_ACPU_ACCESS, acore_access,
                 IFC_INVAR, const char *  , filename , 0,
                 IFC_INCNT, int, namelen, 0,
                 IFC_INCNT,  int  , amode , 0
                )

IFC_GEN_CALL2(MAILBOX_IFC_CCPU_TO_ACPU_MKDIR, acore_mkdir,
                 IFC_INVAR, const char *  , pathname , 0,
                 IFC_INCNT, int, path_len, 0
                )

IFC_GEN_CALL2(MAILBOX_IFC_CCPU_TO_ACPU_RMDIR, acore_rmdir,
                 IFC_INVAR, const char *  , pathname , 0,
                 IFC_INCNT, int, path_len, 0
                )

int ifc_acore_printk(const char *fmt, ... )
{
    char            out_buf[IFC_ACORE_IFC_MAX_LEN];
    va_list         arg;
    unsigned int    ret_len;
    char * head_str = "ccore:";

    memcpy(out_buf, head_str, strlen(head_str));

    /* 将格式化字符串和可变参数转换为字符串 */
    va_start(arg, fmt);
    ret_len = vsnprintf(out_buf + strlen(head_str), IFC_ACORE_IFC_MAX_LEN, fmt, arg);
    va_end(arg);

    if ((ret_len > IFC_ACORE_IFC_MAX_LEN) || (ret_len <= 0))
    {
        return 1;
    }

    acore_print_p0(out_buf, strlen(out_buf), 0);

    return 0;
}

void* ifc_acore_fopen(const char *filename,  const char *mode)
{
    return (void*) acore_fopen( filename, strlen(filename) + 1 , mode, strlen(mode) + 1,  IFC_WAIT_FOREVER);
}

void ifc_acore_fclose(void *stream)
{
    (void)acore_fclose(stream, IFC_WAIT_FOREVER);
}

long ifc_acore_fread(void *ptr, unsigned int count, void *fp )
{
    return acore_fread( ptr,  count,  (long)fp, IFC_WAIT_FOREVER);
}

long ifc_acore_fwrite(void *buf, unsigned int count, void *fp )
{
    return acore_fwrite( buf,  count,  (long)fp, IFC_WAIT_FOREVER);
}

long ifc_acore_fseek(void *fp, long offset, int whence )
{
    return acore_fseek(fp, offset, whence, IFC_WAIT_FOREVER );
}

long ifc_acore_remove(const char *pathname)
{
    return acore_remove(pathname, strlen(pathname) + 1, IFC_WAIT_FOREVER);
}

long ifc_acore_ftell( void *fp )
{
    return acore_ftell(fp, IFC_WAIT_FOREVER);
}

long ifc_acore_rename(const char * oldname,  const char * newname )
{
    return acore_rename(oldname, strlen(oldname) + 1, newname, strlen(newname) + 1, IFC_WAIT_FOREVER);
}

long ifc_acore_access(const char * filename,  int amode)
{
    return acore_access(filename, strlen(filename) + 1, amode, IFC_WAIT_FOREVER);
}

long ifc_acore_mkdir(const char * pathname)
{
    return acore_mkdir(pathname, strlen(pathname) + 1, IFC_WAIT_FOREVER);
}

long ifc_acore_rmdir(const char * pathname)
{
    return acore_rmdir(pathname, strlen(pathname) + 1, IFC_WAIT_FOREVER);
}
#endif

long ccore_fopen(const char *filename, int name_len, const char *mode, int mode_len)
{

    printf("ccore_fopen:filename(%s), name_len(%d), mode(%s), mode_len(%d)\n", filename, name_len, mode, mode_len);

    return 0;
}

long ccore_memcpy(void * dest,  int dest_len, const void * src,  int src_len)
{
    return (long)memcpy(dest, src, dest_len);
}

long ccore_memcmp(const void * src, int src_len, const void * dest, int dest_len)
{
    return (long)memcmp(src, dest, dest_len);
}


#if 0

IFC_GEN_EXEC4(ccore_memcpy,
                IFC_OUTVAR, void *, dst, 0,
                IFC_INCNT, int, dst_len, 0,
                IFC_INVAR, const void *, src, 0,
                IFC_INCNT, int , src_len , 0 )

IFC_GEN_EXEC4(ccore_memcmp,
                IFC_INVAR, const void *, src, 0,
                IFC_INCNT, int, src_len, 0,
                IFC_INVAR, const void *, dst, 0,
                IFC_INCNT, int , dst_len , 0)

IFC_GEN_CALL4(MAILBOX_IFC_CCPU_TO_ACPU_TEST, acore_memcpy,
                IFC_OUTVAR, void *, dst, 0,
                IFC_INCNT, int, dst_len, 0,
                IFC_INVAR, const void *, src, 0,
                IFC_INCNT, int , src_len , 0)

IFC_GEN_CALL4(MAILBOX_IFC_CCPU_TO_MCU_TEST, mcu_memcpy,
                IFC_OUTVAR, void *, dst, 0,
                IFC_INCNT, int, dst_len, 0,
                IFC_INVAR, const void *, src, 0,
                IFC_INCNT, int , src_len , 0)

IFC_GEN_CALL4(MAILBOX_IFC_CCPU_TO_ACPU_TEST_CMP, acore_memcmp,
                IFC_INVAR, const void *, src, 0,
                IFC_INCNT, int, src_len, 0,
                IFC_INVAR, const void *, dst, 0,
                IFC_INCNT, int , dst_len , 0)

IFC_GEN_CALL4(MAILBOX_IFC_CCPU_TO_MCU_TEST_CMP, mcu_memcmp,
                IFC_INVAR, const void *, src, 0,
                IFC_INCNT, int, src_len, 0,
                IFC_INVAR, const void *, dst, 0,
                IFC_INCNT, int , dst_len , 0)


/******************************************************
*测试基本入参1:  a:INFIX, b:OUTFIX, c:BIFIX
******************************************************/
long ccpu_ifc_base_test1(const int *a, int *b, int *c)
{
    int tmp1 = *a;
    int tmp2 = *c;
    printf("ccpu_ifc_base_test1: a = %d, b = %d, c = %d\n", *a, *b, *c);
    *b = tmp1;
    *c = tmp2 + (*b);
    printf("ccpu_ifc_base_test1 end.\n");
    return 0;
}

/******************************************************
*测试基本入参2:  a:INVAR, b:OUTVAR, c:BIVAR
******************************************************/
long ccpu_ifc_base_test2(const int *a, int s1, int *b, int s2, int *c, int s3)
{
    int i = 0;
    printf("ccpu_ifc_base_test2:\n");
    printf("a = ");
    int c1 = s1/(sizeof(int)) ;
    int c2 = s2/(sizeof(int));
    int c3 = s3/(sizeof(int));

    for(i=0; i<c1; i++){
        printf("%d\t", *(a+i));
    }
    printf("\nb = ");
    for(i=0; i<c2; i++){
        printf("%d\t", *(b+i));
    }
    printf("\nc = ");
    for(i=0; i<c3; i++){
        printf("%d\t", *(c+i));
    }
    printf("\n");

    for(i=0; i<c1; i++){
        *(b+i) = *(a+i);
        *(c+i) += *(a+i);
    }
    printf("ccpu_ifc_base_test2 end.\n");
    return 0;
}

IFC_GEN_EXEC3(ccpu_ifc_base_test1,
                IFC_INFIX, const int *, a, sizeof(int),
                IFC_OUTFIX, int *, b, sizeof(int),
                IFC_BIFIX, int *, c, sizeof(int))

IFC_GEN_EXEC6(ccpu_ifc_base_test2,
                IFC_INVAR, const int *, a, 0,
                IFC_INCNT, int, len1, 0,
                IFC_OUTVAR, int *, b, 0,
                IFC_INCNT, int, len2, 0,
                IFC_BIVAR, int *, c, 0,
                IFC_INCNT, int, len3, 0)


 long ccpu_ifc_test_acpu(unsigned int len, unsigned int loop_num)
 {
    char *src = NULL;
    char *dst = NULL;
    unsigned int i = 0;
    unsigned int flag = 0;
    int result = 0;

    if(len <= 0 || loop_num < 0){
        printf("Error: ccpu_ifc_test_acpu: para must greater than 0!\n");
        return 1;
    }

    src = (char *)malloc(len);
    if(NULL == src){
        printf("Error: ccpu_ifc_test_acpu: src buff malloc fail!\n");
        return 1;
    }

    dst = (char *)malloc(len);
    if(NULL == dst){
        printf("Error: ccpu_ifc_test_acpu: dst buff malloc fail!\n");
        free(src);
        return 1;
    }

    if(loop_num == 0){
        flag = 1;
    }
    while((flag == 1) || (i < loop_num)){
        memset(src, 0x5a, len);
        memset(dst, 0, len);

        acore_memcpy(dst, len, src, len, IFC_WAIT_FOREVER);

        result = acore_memcmp(src, len, dst, len, IFC_WAIT_FOREVER);

        if(result == 0){
            i++;
            continue;
        }
        else{
            printf("Error: ccpu_ifc_test_acpu: test fail\n");
            printf("loop num = %d\n", i);
            return 1;
        }
    }
    printf("ccpu_ifc_test_acpu: test pass\n");
    free(src);
    free(dst);
    return 0;
 }

 long ccpu_ifc_test_mcu(unsigned int len, unsigned int loop_num)
 {
    char *src = NULL;
    char *dst = NULL;
    unsigned int i = 0;
    unsigned int flag = 0;
    int result = 0;

    if(len <= 0 || loop_num < 0){
        printf("Error: ccpu_ifc_test_mcu: para must greater than 0!\n");
        return 1;
    }

    src = (char *)malloc(len);
    if(NULL == src){
        printf("Error: ccpu_ifc_test_mcu: src buff malloc fail!\n");
        return 1;
    }

    dst = (char *)malloc(len);
    if(NULL == dst){
        printf("Error: ccpu_ifc_test_mcu: dst buff malloc fail!\n");
        free(src);
        return 1;
    }

    if(loop_num == 0){
        flag = 1;
    }
    while((flag == 1) || (i < loop_num)){
        memset(src, 0xaa, len);
        memset(dst, 0, len);

        mcu_memcpy(dst, len, src, len, IFC_WAIT_FOREVER);

        result = mcu_memcmp(src, len, dst, len, IFC_WAIT_FOREVER);

        if(result == 0){
            i++;
            continue;
        }
        else{
            printf("Error: ccpu_ifc_test_mcu: test fail\n");
            printf("loop num = %d\n", i);
            return 1;
        }
    }
    printf("ccpu_ifc_test_mcu: test pass\n");
    free(src);
    free(dst);
    return 0;
 }

 long ccpu_ifc_test_main(unsigned int len, unsigned int loop_num)
 {
    printf("ccpu_ifc_test_main start:\n");
    if(ERROR == (int)taskSpawn((char *)"ccpu_ifc_test_acpu", (int)100, (int)0,
                            (int)4096,
                            (FUNCPTR)ccpu_ifc_test_acpu,
                            (int)len,
                            (int)loop_num, (int)0, (int)0, (int)0, (int)0, (int)0, (int)0, (int)0, (int)0)) {
        printf("Error: ccpu_ifc_test_acpu start fail!\n");
        return 1;
    }
    else{
        printf("ccpu_ifc_test_acpu start.\n");
    }

    if(ERROR == (int)taskSpawn((char *)"ccpu_ifc_test_mcu", (int)100, (int)0,
                            (int)4096,
                            (FUNCPTR)ccpu_ifc_test_mcu,
                            (int)len,
                            (int)loop_num, (int)0, (int)0, (int)0, (int)0, (int)0, (int)0, (int)0, (int)0)) {
        printf("Error: ccpu_ifc_test_mcu start fail!\n");
        return 1;
    }
    else{
        printf("ccpu_ifc_test_mcu start.\n");
    }
    printf("ccpu_ifc_test_main end.\n");
    return 0;
 }


#endif

void mailbox_ifc_test_init(void)
{
#if 0
    mailbox_ifc_register(MAILBOX_IFC_ACPU_TO_CCPU_TEST,  ccore_memcpy);
    mailbox_ifc_register(MAILBOX_IFC_MCU_TO_CCPU_TEST,  ccore_memcpy);

    mailbox_ifc_register(MAILBOX_IFC_ACPU_TO_CCPU_BASE_TEST1, ccpu_ifc_base_test1);
    mailbox_ifc_register(MAILBOX_IFC_ACPU_TO_CCPU_BASE_TEST2, ccpu_ifc_base_test2);
    mailbox_ifc_register(MAILBOX_IFC_MCU_TO_CCPU_BASE_TEST1, ccpu_ifc_base_test1);
    mailbox_ifc_register(MAILBOX_IFC_MCU_TO_CCPU_BASE_TEST2, ccpu_ifc_base_test2);

    mailbox_ifc_register(MAILBOX_IFC_ACPU_TO_CCPU_TEST_CMP, ccore_memcmp);
    mailbox_ifc_register(MAILBOX_IFC_MCU_TO_CCPU_TEST_CMP, ccore_memcmp);
#endif

}

#endif
#if 0

long ifc_test_acore_fs(char* name, long size)
{
    FILE * fp = 0;
    void *string_w= 0; /*写文件入参字符串*/
    void *string_r= 0; /*读文件出参字符串*/
    long ret = 1;

    string_w = (void*)malloc(size);
    if (!string_w) {
        goto test_exit;
    }

    string_r = (void*)malloc(size);
    if (!string_r) {
        goto test_exit;
    }

    memset(string_w, 0x5a, size);
    memset(string_r, 0x00, size);

    fp = (FILE *)ifc_acore_fopen(name, "wb+");
    if (!fp) {
        printf("ifc_test_acore_fs open fail0!\n");
        goto test_exit;
    }

    if (size !=  ifc_acore_fwrite(( void *)string_w, size, fp)) {
        printf("ifc_test_acore_fs write error!\n");
        goto test_exit;
    }

    ifc_acore_fclose(fp);
    fp = 0;

    fp = (FILE *)ifc_acore_fopen(name, "rb");
    if (!fp) {
        printf("ifc_test_acore_fs open fail1!\n");
    }
    if (size !=  ifc_acore_fread((void *)string_r, size, fp)) {
        printf("ifc_test_acore_fs read error!\n");
        goto test_exit;
    }

    ifc_acore_fclose(fp);
    fp = 0;

    if (0 == memcmp(string_r, string_w, size)) {
        printf("ifc_test_acore_fs test success!\n");
        ret = 0;
    } else {
        printf("ifc_test_acore_fs test fail!\n");
    }

 test_exit:
    if (string_w)
        free(string_w);
    if (string_r)
        free(string_r);
    if (fp) {
        ifc_acore_fclose(fp);
        fp = 0;
    }

    return ret;
}

long tm_mailbox_ifc(void)
{
    int size;
    int ret;
    for (size = 1; size < 100; size ++) {
       ret = ifc_test_acore_fs("/data/ifc_testfile",  size);
       if (ret){
           break;
       }
    }

    return ret;
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif


