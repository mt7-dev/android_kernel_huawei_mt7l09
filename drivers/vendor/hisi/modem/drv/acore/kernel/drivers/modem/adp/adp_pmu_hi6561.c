

#ifdef __cplusplus
extern "C"
{
#endif

#include <linux/err.h>
#include <linux/huawei/dsm_pub.h>
#include <osl_types.h>
#include <bsp_icc.h>
#include <bsp_om.h>
#include <bsp_pmu_hi6561.h>

#ifndef OK
#define OK      0
#endif

#ifndef ERROR
#define ERROR   -1
#endif

#define  pastar_print_always(fmt, ...)   (bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_PASTAR, "[PAStar]: "fmt, ##__VA_ARGS__))
#define  pastar_print_error(fmt, ...)   (bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_PASTAR, "[PAStar]: <%s> "fmt, __FUNCTION__, ##__VA_ARGS__))
#define  pastar_print_info(fmt, ...)    (bsp_trace(BSP_LOG_LEVEL_INFO, BSP_MODU_PASTAR, "[PAStar]: <%s> "fmt, __FUNCTION__, ##__VA_ARGS__))

static u8 reg_val[2] = {0,0};
static struct workqueue_struct *pastar_dsm_wq = NULL;
static struct work_struct pastar_dsm_work;
static struct dsm_client *pastar_dclient = NULL;
static struct dsm_dev dsm_pastar = {
	.name = "dsm_pastar",
	.fops = NULL,
	.buff_size = 1024,
};

/*****************************************************************************
 函 数 名  : pastar_dsm_work_func
 功能描述  : 上报异常信息
 输入参数  : @work - work结构体
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
void pastar_dsm_work_func(struct work_struct *work)
{
    pastar_print_info("Execuing ...");

    /* notify exception */
    if (!dsm_client_ocuppy(pastar_dclient)) 
    {
        /* record message */
		dsm_client_record(pastar_dclient, "PAStar 0x21 reg 0x%x, 0x6A reg 0x%x\n", reg_val[0], reg_val[1]);

        /* notify */
		dsm_client_notify(pastar_dclient, 30000);
	}

    return;
}

/*****************************************************************************
 函 数 名  : pastar_notify_exception
 功能描述  : 接收C核发送的数据
 输入参数  : @channel_id - icc 通道id
             @len - 数据长度
             @context - 参数地址
 输出参数  : 无
 返 回 值  : 0 - 成功; else - 失败
*****************************************************************************/
s32 pastar_notify_exception(u32 channel_id , u32 len, void* context)
{
    s32 ret = ERROR;
    
    pastar_print_info("Execuing ...");

    /* recive data from ccore */
    ret = bsp_icc_read(PASTAR_ICC_CHN_ID, (u8 *)reg_val, sizeof(reg_val));
    if(sizeof(reg_val) != ret)
    {
        pastar_print_error("icc read failed, ret = %d\n", ret);
        return ret;
    }

	queue_work(pastar_dsm_wq, &pastar_dsm_work);

    /* success */
    return OK;
}

/*****************************************************************************
 函 数 名  : pastar_adp_init
 功能描述  : pastar 适配层初始化
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 0 - 成功; else - 失败
*****************************************************************************/
s32 pastar_adp_init(void)
{    
    s32 ret = ERROR;

    pastar_print_info("Execuing ...");
    
    /* icc register */
    ret = bsp_icc_event_register(PASTAR_ICC_CHN_ID, pastar_notify_exception, NULL, NULL, NULL);
	if(ret != OK)
	{
        pastar_print_error("icc register failed, ret = %d\n", ret);
        return ret;
	}

    /* dclient register */
    if(!pastar_dclient) 
    {
		pastar_dclient = dsm_register_client(&dsm_pastar);
        if(!pastar_dclient)
    	{
            pastar_print_error("dsm_register_client failed\n");
            return ERROR;
    	}
	}

    /* create work queue */
    pastar_dsm_wq = create_singlethread_workqueue("pastar_dsm_wq");
	if (IS_ERR(pastar_dsm_wq))
    {
        pastar_print_error("create workqueue failed\n");
        return ERROR;
	}

	INIT_WORK(&pastar_dsm_work, pastar_dsm_work_func);

    pastar_print_always("PAStar init OK\n");

    /* success */
    return OK;
}

/*****************************************************************************
 函 数 名  : pastar_adp_exit
 功能描述  : pastar 驱动注销
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
void pastar_adp_exit(void)
{
    s32 ret = ERROR;

    pastar_print_info("Execuing ...");
    
    /* icc unregister */
    ret = bsp_icc_event_unregister(PASTAR_ICC_CHN_ID);
	if(ret != OK)
	{
        pastar_print_error("icc unregister failed, ret = %d\n", ret);
        return;
	}

    /* success */
    return;
}

module_init(pastar_adp_init);
module_exit(pastar_adp_exit);                  /*lint !e19 !e132 !e578*/

#ifdef __cplusplus
}
#endif
