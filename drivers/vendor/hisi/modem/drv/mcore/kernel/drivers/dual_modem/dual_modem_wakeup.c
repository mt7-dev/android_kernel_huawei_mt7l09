

#include "bsp_icc.h"
#include "osl_common.h"
#include "uart.h"
#include "gpio.h"


#define VIA_WAKEUP_BALONG_PIN	GPIO_26_3
#define LPm3_UART5_IQR_ENABLE	(0xaa)
#define VIA_WAKEUP_BALONG		(0x01)
#define dm_print_info(fmt, ...)    (printk(""fmt"\n", ##__VA_ARGS__))

/*****************************************************************************
 函 数 名  : recv_modem_rsg_by_icc
 功能描述  : modem发送icc，ap侧控制gpio唤醒对方modem
 输入参数  : 
 输出参数  : 无
 返 回 值  : void
*****************************************************************************/
int recv_modem_rsg_by_icc(u32 chan_id, u32 len, void* context)
{	
	s32 read_size = 0;
	u8 flag = 0;
	context = context;		//防止编译告警

 	read_size = bsp_icc_read((ICC_CHN_MCORE_CCORE << 16)|MCORE_CCORE_FUNC_UART, &flag, len); 
	if ((read_size > (s32)len) && (read_size <= 0))
    {
		return 1;
    }
	if(flag == LPm3_UART5_IQR_ENABLE)
	{
#ifdef UART5_IRQ_NOTIFY_MODEM
		uart5_init();
#endif
		bsp_icc_send((u32)ICC_CPU_MODEM,(ICC_CHN_MCORE_CCORE << 16)|MCORE_CCORE_FUNC_UART,&flag,sizeof(flag));
	}
	return 0;
}
/*****************************************************************************
 函 数 名  : via_wakeup_balong_handler
 功能描述  : gpio中断处理函数，发送icc给modem
 输入参数  : 
 输出参数  : 
 返 回 值  : void
*****************************************************************************/
void via_wakeup_balong_handler(u32 gpio_no)
{
	u8 flag = 0;

	flag = VIA_WAKEUP_BALONG;
	bsp_icc_send((u32)ICC_CPU_MODEM,(ICC_CHN_MCORE_CCORE << 16)|MCORE_CCORE_FUNC_UART,&flag,sizeof(flag));
}
/*****************************************************************************
 函 数 名  : wakeup_modem_init
 功能描述  : ap侧gpio初始化
 输入参数  : 
 输出参数  : 
 返 回 值  : 0:成功，-1:失败
*****************************************************************************/
int wakeup_modem_init(void)
{
	int ret = 0;
	
	gpio_set_direction(VIA_WAKEUP_BALONG_PIN,0);
	ret = gpio_irq_request(VIA_WAKEUP_BALONG_PIN,via_wakeup_balong_handler,IRQ_TYPE_EDGE_FALLING|IRQF_AWAKE);
	if(ret < 0)
	{
		printk("irq fail\n");
		goto err_irq_req;
	}
	
	/* 注册ICC读写回调 */
    if(0 !=bsp_icc_event_register((ICC_CHN_MCORE_CCORE << 16)|MCORE_CCORE_FUNC_UART,recv_modem_rsg_by_icc , NULL, NULL, NULL))
    {
        printk("reg icc cb fail\n");
    }
return 0;
		
err_irq_req:	
	gpio_free_irq(VIA_WAKEUP_BALONG_PIN);
	
return -1;
}

