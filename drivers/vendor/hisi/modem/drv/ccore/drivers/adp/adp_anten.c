/*lint -save -e537*/
#include <product_config.h>
#include <drv_anten.h>
#include <drv_gpio.h>
#include <drv_nv_id.h>
#include <drv_nv_def.h>
#include <drv_anten.h>
#include <hi_anten.h>
#include <gpio_balong.h>
#include <bsp_om.h>
#include <bsp_nvim.h>
#include <anten_balong.h>

/*lint -restore*/

#ifdef HI_K3_ANTEN
#define ANTEN_MODEM0_GPIO GPIO_158
#define ANTEN_MODEM1_GPIO GPIO_159
#else
#define ANTEN_MODEM0_GPIO             (GPIO_0_15)
#define ANTEN_MODEM1_GPIO             (GPIO_0_15)
#endif

#define anten_print_error(fmt, ...)    (bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_ANTEN, "[anten]: <%s> <%d> "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__))
#define anten_print_info(fmt, ...)     (bsp_trace(BSP_LOG_LEVEL_ERROR,  BSP_MODU_ANTEN, "[anten]: "fmt, ##__VA_ARGS__))

DRV_DRV_ANTEN_GPIO_STRU anten_stru;

int anten_nv_init(void)
{
    unsigned int ret = 0;

    /* Get ANTEN NV data by id.*/
    ret = bsp_nvm_read(NV_ID_DRV_ANTEN_CFG,(u8*)&anten_stru,sizeof(DRV_DRV_ANTEN_GPIO_STRU));
    if (ret !=  0)
    {
        anten_print_error("anten_gpio read NV=0x%x, ret = %d \n",NV_ID_DRV_ANTEN_CFG, ret);
        return -1;
    }

    return 0;
}

int modem_id_to_gpio(unsigned int modem_id)
{
    unsigned int i = 0;
    unsigned int size = sizeof(DRV_DRV_ANTEN_GPIO_STRU)/sizeof(DRV_DRV_ANTEN_GPIO_CFG);

    for(i = 0;i < size;i++)
    {
        if(modem_id == anten_stru.anten_gpio[i].modem_id && (1 == anten_stru.anten_gpio[i].used))
        {
            return (int)(anten_stru.anten_gpio[i].gpio);
        }
        else if(modem_id == anten_stru.anten_gpio[i].modem_id && (1 != anten_stru.anten_gpio[i].used))
        {
            anten_print_error("anten gpio nv is not config.\n");
            return GPIO_ERROR;
        }

    }
    return GPIO_ERROR;

}

/*****************************************************************************
 函 数 名  : drv_anten_lock_status_get
 功能描述  : 获取有线与无线的连接状态
 输入参数  : none
 输出参数  : 0 - 无线连接
             1 - 有线连接
 返 回 值  : 0 - 成功
             其它为失败

*****************************************************************************/
int drv_anten_lock_status_get(PWC_COMM_MODEM_E modem_id, unsigned int *status)
{
#ifdef CONFIG_ANTEN
    int value = 0;
    int gpio = 0;
#endif
    if(NULL == status || (modem_id >= PWC_COMM_MODEM_BUTT))
    {
        anten_print_error("para is error, modem id = %d, status = 0x%x.\n", modem_id, status);
        return ANTEN_ERROR;
    }
    
#ifdef CONFIG_ANTEN
    gpio = modem_id_to_gpio((unsigned int)modem_id);
    if(GPIO_ERROR == gpio)
    {
        anten_print_error("modem_id_to_gpio is fail, value = %d.\n", gpio);
        return -1;
    }   

    value = bsp_gpio_get_value((unsigned int )gpio);/* [false alarm]:屏蔽Fortify错误 */
    if(GPIO_ERROR == value)
    {
        anten_print_error("bsp_gpio_get_value is fail, value = %d.\n", value);
        return -1;
    }

    *status = value;/*lint !e732*/
#else
    *status = 1;
#endif
	return 0;
}


/*****************************************************************************
 函 数 名  : drv_anten_int_install
 功能描述  : 中断注册函数，用来获得当前的天线状态
 输入参数  : routine   - 中断处理函数
             para      - 保留字段
 输出参数  : none
 返 回 值  : void

*****************************************************************************/
void drv_anten_int_install(PWC_COMM_MODEM_E modem_id, void* routine, int para)
{
    if(NULL == routine || (modem_id >= PWC_COMM_MODEM_BUTT))
    {
        anten_print_error("para is error, modem id = %d, routine = 0x%x, para = %d.\n", modem_id, routine, para);
        return ;
    }
#ifdef CONFIG_ANTEN
	bsp_anten_int_install(modem_id, routine, para);
#endif
}

int drv_anten_ipc_reg_fun(void)
{
    return 0;
}




