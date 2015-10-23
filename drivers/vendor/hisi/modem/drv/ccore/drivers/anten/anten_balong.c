/*************************************************************************
*   版权所有(C) 1987-2004, 深圳华为技术有限公司.
*
*   文 件 名 :  anten_balong.c
*
*   作    者 :  liuwenhui
*
*   描    述 :  ANTEN驱动文件
*
*   修改记录 :  2012年11月27日
*
*************************************************************************/
/*lint -save -e537*/
#include <osl_types.h>
#include <osl_sem.h>
#include <osl_thread.h>
#include <bsp_om.h>
#include <bsp_icc.h>
#include <anten_balong.h>
/*lint -restore */


#define anten_print_error(fmt, ...)    (bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_ANTEN, "[anten]: <%s> <%d> "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__))
#define anten_print_info(fmt, ...)     (bsp_trace(BSP_LOG_LEVEL_ERROR,  BSP_MODU_ANTEN, "[anten]: "fmt, ##__VA_ARGS__))

struct anten_msg_stru
{
    unsigned int modem_id;
    unsigned int status;
};

#define  EFUSE_TASK_STACK_SIZE  (0x1000) 

typedef void (*ANTEN_FUNCPTR)(void);

ANTEN_FUNCPTR antenna_routine[PWC_COMM_MODEM_BUTT] = {0};

static struct anten_msg_stru anten_msg = {0,0};

static osl_sem_id anten_sem_id;

s32 bsp_anten_data_receive(void);

void anten_event_report_oam(void *para)
{
    struct anten_msg_stru msg = {0, 0};
    /*lint -save -e716*/
    /* coverity[no_escape] */
    while(1)
    {
        osl_sem_down(&anten_sem_id);
        
        msg = *((struct anten_msg_stru*)para);

        anten_print_info("anten modem_id = %d,status = %d.\n", msg.modem_id, msg.status);

        if(msg.modem_id < PWC_COMM_MODEM_BUTT && (NULL != antenna_routine[msg.modem_id]))
        {
            (void) (*(antenna_routine[msg.modem_id]))();
        }
    }
	/*lint -restore */

}

/*****************************************************************************
 * 函 数 名  :bsp_anten_init
 *
 * 功能描述  : 天线插拔核间通信，C核初始化函数
 *
 * 输入参数  : 无
 * 输出参数  : 无
 *
 * 返 回 值  : ANTAN_OK:    操作成功
 *             ANTAN_ERROR: 操作失败
 *
 * 其它说明  : 无
 *
 *****************************************************************************/
s32 bsp_anten_init(void)
{
    int ret = 0;
    u32 task_id = 0;
    u32 chan_id = ICC_CHN_IFC << 16 | IFC_RECV_FUNC_ANTEN;

    if(0 != anten_nv_init())
    {
        anten_print_error("anten nv init fail.\n");
        return -1;

    }

    osl_sem_init(0, &anten_sem_id);
    
    if(-1 == osl_task_init("anten_task", VX_DEFAULT_PRIORITY, EFUSE_TASK_STACK_SIZE, anten_event_report_oam, &anten_msg, &task_id))
    {
        anten_print_error("osl_task_init is error.\n");
        return ANTEN_ERROR;
    }

    ret = bsp_icc_event_register(chan_id, (read_cb_func)bsp_anten_data_receive, NULL, NULL, NULL);
    if(0 != ret)
    {
        anten_print_error("register is error.\n");
        return ANTEN_ERROR;
    }    

    anten_print_info("anten init ok,task id is %d.\n", task_id);

    return ANTEN_OK;
}

s32 bsp_anten_data_receive(void)
{	
    int len = 0;

    struct anten_msg_stru msg = {0};
    u32 channel_id = ICC_CHN_IFC << 16 | IFC_RECV_FUNC_ANTEN;
      
	len = bsp_icc_read(channel_id, (u8*)&msg, sizeof(struct anten_msg_stru));
	if(len != (int)sizeof(struct anten_msg_stru))
	{
		anten_print_error("read len(%x) != expected len(%x).\n", len, sizeof(struct anten_msg_stru));
		return ANTEN_ERROR;
	}
    
    anten_msg = msg;
    osl_sem_up(&anten_sem_id);

    return ANTEN_OK;
}


/****************************************************
Function:   bsp_anten_int_install   
Description:      中断注册函数，用来获得当前的天线状态，打桩（ASIC平台下需要实现）
Input:        NA;
Output:       NA;
Return:       NA;
Others:       NA;
****************************************************/ 
void bsp_anten_int_install(PWC_COMM_MODEM_E modem_id, void* routine,int para)
{
    if(NULL == routine || (modem_id >= PWC_COMM_MODEM_BUTT))
    {
        anten_print_error("para is error, modem_id = %d, routine = 0x%x, para = %d.\n", modem_id, routine, para);
        return ;
    }
    antenna_routine[modem_id] = (ANTEN_FUNCPTR)routine;
}


