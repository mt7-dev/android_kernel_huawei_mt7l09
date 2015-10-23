/******************************************************************************/
/*  Copyright (C), 2007-2013, Hisilicon Technologies Co., Ltd. */
/******************************************************************************/
/* File name     : i2c_balong.c */
/* Version        : 1.0 */
/* Created       : 2013-04-07*/
/* Last Modified : */
/* Description   :  The C union definition file for the module LTE_PMU*/
/* Function List : */
/* History       : */
/* 1 Date        : */
/* Modification  : Create file */
/******************************************************************************/
/*lint --e{537}*/
#include <i2c_balong.h>
#include "osl_bio.h"
#include "osl_types.h"
#include "hi_i2c.h"
#include "bsp_memmap.h"
#include "osl_spinlock.h"
#include "bsp_clk.h"
#include "bsp_dpm.h"
#include "bsp_ipc.h"

#define MAX_NAME_LEN 48
#define MAX_SLAVE_NUM 20
struct balong_i2c_ctrl {
	spinlock_t		lock;
	u32		suspended:1;
	u32		regs;/*register base addr*/
	char* 	master_name;
	struct clk	*clk;
	u8		i2c_slave_list[MAX_SLAVE_NUM];/*slave list*/
	u8		slave_num;
	u8		reserved[3];
};

#ifdef HI_I2C_REGBASE_ADDR
static struct balong_i2c_ctrl p531_i2c;
#endif

#ifdef HI_I2C1_REGBASE_ADDR
static struct balong_i2c_ctrl v7r2_i2c0;

static struct balong_i2c_ctrl v7r2_i2c1;

#endif


int bsp_i2c_slave_register(enum i2c_master i2c,u8 slave_id)
{
	/*lint -save -e958*/
	u8 slave_num;
	u8 idx;
	struct balong_i2c_ctrl *i2c_master_cur = NULL;
	/*lint -restore*/
#ifdef HI_I2C_REGBASE_ADDR
	i2c_master_cur=&p531_i2c;
	if(I2C_MASTER1==i2c)
	{
		i2c_print_error("p531 only has i2c_master0\n");
		return I2C_ERROR;
	}
#endif

#ifdef HI_I2C1_REGBASE_ADDR
	if(I2C_MASTER0==i2c){
		i2c_master_cur=&v7r2_i2c0;
	}
	else{
		i2c_master_cur=&v7r2_i2c1;

		if(I2C_MASTER1!=i2c){
			i2c_print_error("there are i2c_master0 or i2c_master1,don't have i2c_master%d\n",i2c);
			return I2C_ERROR;
		}
	}

#endif
	slave_num=i2c_master_cur->slave_num;
	if(slave_num>=MAX_SLAVE_NUM){
		i2c_print_error("master has %d slaves,add salve fail!\n",slave_num);
		return I2C_ERROR;
	}

	/*ensure one slave_id can't register a master with muti times*/
	for(idx=0;idx<slave_num;idx++)
	{
		if(slave_id==i2c_master_cur->i2c_slave_list[idx])
		{
			i2c_print_error("the slave has registed!\n");
			return I2C_ERROR;
		}
	}
	i2c_master_cur->i2c_slave_list[slave_num]=slave_id;
	i2c_master_cur->slave_num=slave_num+1;

	return I2C_OK;

}

/*find i2c master for slave*/
static struct balong_i2c_ctrl *find_i2c_ctrl(u8 slave_id)
{
	/*lint -save -e958*/
	struct balong_i2c_ctrl *i2c_master;
	int idx = 0;/*i2c slave index*/
	/*lint -restore*/

#ifdef HI_I2C_REGBASE_ADDR
	i2c_master=&p531_i2c;
	for(idx=0;idx<i2c_master->slave_num;idx++)
	{
		if(i2c_master->i2c_slave_list[idx]==slave_id){
			i2c_print_info("the 0x%x i2c master is %s",slave_id,i2c_master->master_name);
			return i2c_master;
		}
	}
	i2c_print_error("can't find the 0x%x slave,may be it didn't have regitstered!\n",slave_id);
	return NULL;
#endif

#ifdef HI_I2C1_REGBASE_ADDR
	i2c_master=&v7r2_i2c0;
	for(idx=0;idx<i2c_master->slave_num;idx++)
	{
		if(i2c_master->i2c_slave_list[idx]==slave_id){
			i2c_print_info("the 0x%x i2c master is %s",slave_id,i2c_master->master_name);
			return i2c_master;
		}
	}

	i2c_master=&v7r2_i2c1;

	for(idx=0;idx<i2c_master->slave_num;idx++)
	{
		if(i2c_master->i2c_slave_list[idx]==slave_id){
			i2c_print_info("the 0x%x i2c master is %s",slave_id,i2c_master->master_name);
			return i2c_master;
		}
	}
	i2c_print_error("can't find the 0x%x slave,may be it didn't have regitstered!\n",slave_id);
	return NULL;
#endif

}

/*set high high level cycle*/
static void balong_i2c_scl_high(struct balong_i2c_ctrl *i2c_master)
{
	u32 tmp = HI_I2C_SCL_H_NUM;;

	writel(tmp, i2c_master->regs+HI_I2C_SCL_H_OFFSET);
	return ;
}

/* set high low level cycle */
static void balong_i2c_scl_low(struct balong_i2c_ctrl *i2c_master)
{
	u32 tmp=HI_I2C_SCL_L_NUM;

	writel(tmp, i2c_master->regs+HI_I2C_SCL_L_OFFSET);
	return ;
}


/*set i2c ctrl*/
static void balong_i2c_enable(struct balong_i2c_ctrl *i2c_master)
{
	u32 iiccon;

	iiccon = HI_I2C_CTRL_I2CEN;	 	/*I2C ctrl enable*/

	writel(iiccon, i2c_master->regs+HI_I2C_CTRL_OFFSET);
	return ;
}

/*clear all interrupt */
static void balong_i2c_all_mask(struct balong_i2c_ctrl *i2c_master)
{
	unsigned long tmp;
	tmp=HI_I2C_ICR_ALLMASK;
	writel(tmp, i2c_master->regs+HI_I2C_ICR_OFFSET);
	return ;
}

/*clk enable */
static int bsp_i2c_clk_enable(struct balong_i2c_ctrl *i2c_master)
{
	int ret = 0;

	/*1 is enable, 0 is disable , -1 is fail*/
	ret = clk_status(i2c_master->clk);
	if(-1 == ret){
		return I2C_ERROR;
	}
	else if(1 == ret){
		return I2C_OK;
	}

	ret = clk_enable(i2c_master->clk);
	return ret;
}

/*clk disable */
static void bsp_i2c_clk_disable(struct balong_i2c_ctrl *i2c_master)
{
	clk_disable(i2c_master->clk);
}
static s32 balong_i2c_status_check(struct balong_i2c_ctrl *i2c_master)
{
	u32 reg_value;
	s32 timeout=I2C_STATUS_TIMEOUT;

	/*Does bus  transfer over */
	do
	{
		reg_value= readl(i2c_master->regs+HI_I2C_SR_OFFSET);

		/*判断是否收到ACK中断*/
		if(reg_value&HI_I2C_SR_ACKERR){
			i2c_print_error("i2c hasn't receive ack!\n");
		}

		/*判断是否有总线仲裁失败的情况*/
		if(reg_value&HI_I2C_SR_ABITR){
			i2c_print_error("i2c has abitrage ERROR!\n");
		}

	}while((!(reg_value &HI_I2C_SR_INTDONE)) && (--timeout > 0));

	if(0 == timeout)
	{
		i2c_print_error("I2C: Timeout! \n");
		return I2C_ERROR;
	}
	balong_i2c_all_mask(i2c_master);
	return OK;
}

static void balong_i2c_message_start(struct balong_i2c_ctrl *i2c_master,u8 device_id)
{
	/*lint -save -e958*/
	u32 addr=0;
	u32 tmp=0;
	/*lint -restore*/

	if(NULL == i2c_master){
		i2c_print_error("i2c master is null\n");
		return ;
	}

	/*set high high level cycle*/
	balong_i2c_scl_high(i2c_master);

	/* set high low level cycle */
	balong_i2c_scl_low(i2c_master);

	/* clear all interrupt  */
	balong_i2c_all_mask(i2c_master);

	/* set i2c ctrl*/
	balong_i2c_enable(i2c_master);

	/* set the first message ,and put it in the TXR register*/
	addr=(device_id&SLAVE_ID_MASK)<<1;

	writel(addr,i2c_master->regs+HI_I2C_TXR_OFFSET);

	/* send start command and write command */
	tmp=HI_I2C_COM_START|HI_I2C_COM_WRITE;
	writel(tmp,i2c_master->regs+HI_I2C_COM_OFFSET);
	return ;
}
s32 bsp_i2c_byte_data_send(u8 device_id,u8 addr, u8 data)
{
	/*lint -save -e958*/
	struct balong_i2c_ctrl *i2c_master;
	unsigned long flags = 0;
	unsigned long flag = 0;
	s32 ret = I2C_OK;
	/*lint -restore*/

	/*find i2c master*/
	i2c_master=find_i2c_ctrl(device_id);
	if(!i2c_master){
		i2c_print_error("can find i2c master for device %0x\n",device_id);
		return I2C_ERROR;
	}

	/*lock i2c master*/
	/* add lock */
    bsp_ipc_spin_lock_irqsave(IPC_SEM_I2C_SWITHC,flag);
	spin_lock_irqsave(&i2c_master->lock,flags);

	/*clock enable*/
	ret = bsp_i2c_clk_enable(i2c_master);
	if(ret){
		i2c_print_error( "i2c clk enable failed ! \n");
		goto irq_error;
	}


	/*send the first message,include start command and write command */
	balong_i2c_message_start(i2c_master,device_id);

	/*check bus is or not transfer over */
	ret = balong_i2c_status_check(i2c_master);
	if(ret){
		i2c_print_error( "i2c status get error! \n");
		goto clk_error;
	}

	/*set addr in TX Regitster*/
	writel(addr,i2c_master->regs+HI_I2C_TXR_OFFSET);

	/*send write command*/
	writel(HI_I2C_COM_WRITE,i2c_master->regs+HI_I2C_COM_OFFSET);

	/*check bus is or not transfer over */
	ret = balong_i2c_status_check(i2c_master);
	if(ret){
		i2c_print_error( "i2c status get error! \n");
		goto clk_error;
	}

	/*set data in TX register*/
	writel(data,i2c_master->regs+HI_I2C_TXR_OFFSET);

	/*send write command*/
	writel(HI_I2C_COM_WRITE,i2c_master->regs+HI_I2C_COM_OFFSET);

	/*check bus is or not transfer over */
	ret = balong_i2c_status_check(i2c_master);
	if(ret){
		i2c_print_error( "i2c status get error! \n");
		goto clk_error;
	}

	/*send the stop flag ,erase bus*/
	writel(HI_I2C_COM_STOP,i2c_master->regs+HI_I2C_COM_OFFSET);

	/*check bus is or not transfer over */
	ret = balong_i2c_status_check(i2c_master);
	if(ret){
		i2c_print_error( "i2c status get error! \n");
		goto clk_error;
	}

clk_error:
    bsp_i2c_clk_disable(i2c_master);

irq_error:

	spin_unlock_irqrestore(&i2c_master->lock,flags);
    bsp_ipc_spin_unlock_irqrestore(IPC_SEM_I2C_SWITHC,flag);
	return ret;

}


s32 bsp_i2c_word_data_send(u8 device_id,u8 addr, u16 data)
{
	/*lint -save -e958*/
	struct balong_i2c_ctrl *i2c_master;
	unsigned long flags = 0;
	unsigned long flag = 0;
	int ret = 0;
	/*lint -restore*/

	/*find i2c master*/
	i2c_master=find_i2c_ctrl(device_id);
	if(!i2c_master){
		i2c_print_error("can find i2c master for device %0x\n",device_id);
		return I2C_ERROR;
	}

	/*lock i2c master*/
    bsp_ipc_spin_lock_irqsave(IPC_SEM_I2C_SWITHC,flag);
	spin_lock_irqsave(&i2c_master->lock,flags);

	/*clock enable*/
	ret = bsp_i2c_clk_enable(i2c_master);
	if(ret){
		i2c_print_error( "i2c clk enable failed ! \n");
		goto irq_error;
	}

	/*send the first message,include start command and write command */
	balong_i2c_message_start(i2c_master,device_id);

	/*check bus is or not transfer over */
	if(I2C_OK!= balong_i2c_status_check(i2c_master)){

		i2c_print_error( "i2c status get error! \n");
		goto clk_error;
	}

	/*set addr in TX Regitster*/
	writel(addr,i2c_master->regs + HI_I2C_TXR_OFFSET);

	/*send write command*/
	writel(HI_I2C_COM_WRITE,i2c_master->regs+HI_I2C_COM_OFFSET);

	/*check bus is or not transfer over */
	if(I2C_OK!= balong_i2c_status_check(i2c_master)){

		i2c_print_error( "i2c status get error! \n");
		goto clk_error;
	}

	/*set high 8bit data in TX register*/
	writel(data>>8,i2c_master->regs+HI_I2C_TXR_OFFSET);

	/*send write command*/
	writel(HI_I2C_COM_WRITE,i2c_master->regs+HI_I2C_COM_OFFSET);

	/*check bus is or not transfer over */
	if(I2C_OK!= balong_i2c_status_check(i2c_master)){

		i2c_print_error( "i2c status get error! \n");
		goto clk_error;
	}

	/*set low 8bit data in TX register*/
	writel(data&(0xff),i2c_master->regs+HI_I2C_TXR_OFFSET);

	/*send write command*/
	writel(HI_I2C_COM_WRITE,i2c_master->regs+HI_I2C_COM_OFFSET);

	/*check bus is or not transfer over */
	if(I2C_OK!= balong_i2c_status_check(i2c_master)){

		i2c_print_error( "i2c status get error! \n");
		goto clk_error;
	}

	/*send the stop flag ,erase bus*/
	writel(HI_I2C_COM_STOP,i2c_master->regs+HI_I2C_COM_OFFSET);

	/*check bus is or not transfer over */
	if(I2C_OK!= balong_i2c_status_check(i2c_master)){

		i2c_print_error( "i2c status get error! \n");
		goto clk_error;
	}

	bsp_i2c_clk_disable(i2c_master);
	spin_unlock_irqrestore(&i2c_master->lock,flags);
    bsp_ipc_spin_unlock_irqrestore(IPC_SEM_I2C_SWITHC,flag);
	return I2C_OK;

clk_error:
	bsp_i2c_clk_disable(i2c_master);

irq_error:
	spin_unlock_irqrestore(&i2c_master->lock,flags);
    bsp_ipc_spin_unlock_irqrestore(IPC_SEM_I2C_SWITHC,flag);
	return I2C_ERROR;
}


/*lint -save -e958*/
s32 bsp_i2c_byte_data_receive(u8 device_id, u8 addr,  u16 *data)
/*lint -restore*/
{
	struct balong_i2c_ctrl *i2c_master;
	unsigned long flags = 0;
	unsigned long flag = 0;
	int ret = 0;

	/*find i2c master*/
	i2c_master=find_i2c_ctrl(device_id);
	if(!i2c_master){
		i2c_print_error("can find i2c master for device %0x\n",device_id);
		return I2C_ERROR;
	}

	/*lock i2c master*/
    bsp_ipc_spin_lock_irqsave(IPC_SEM_I2C_SWITHC,flag);
	spin_lock_irqsave(&i2c_master->lock,flags);

	/*clock enable*/
	ret = bsp_i2c_clk_enable(i2c_master);
	if(ret){
		i2c_print_error( "i2c clk enable failed ! \n");
		goto irq_error;
	}

	/*send the first message,include start command and write command */
	balong_i2c_message_start(i2c_master,device_id);

	/*check bus is or not transfer over */
	if(I2C_OK!= balong_i2c_status_check(i2c_master)){

		i2c_print_error( "i2c status get error! \n");
		goto clk_error;
	}

	/*send slave register addr into TX register*/
	writel(addr,i2c_master->regs+HI_I2C_TXR_OFFSET);

	/*send write command*/
	writel(HI_I2C_COM_WRITE,i2c_master->regs+HI_I2C_COM_OFFSET);

	/*check bus is or not transfer over */
	if(I2C_OK!= balong_i2c_status_check(i2c_master)){

		i2c_print_error( "i2c status get error! \n");
		goto clk_error;
	}

	/*the flag has changed ,so send restart*/
	writel(device_id<<1|I2C_READ_FLAG,i2c_master->regs+HI_I2C_TXR_OFFSET);

	/*send start & write*/
	writel(HI_I2C_COM_START|HI_I2C_COM_WRITE,i2c_master->regs+HI_I2C_COM_OFFSET);

	/*check bus is or not transfer over */
	if(I2C_OK!= balong_i2c_status_check(i2c_master)){

		i2c_print_error( "i2c status get error! \n");
		goto clk_error;
	}

	/*send read command*/
	writel(HI_I2C_COM_READ|HI_I2C_COM_ACK,i2c_master->regs+HI_I2C_COM_OFFSET);

	/*check bus is or not transfer over */
	if(I2C_OK!= balong_i2c_status_check(i2c_master)){

		i2c_print_error( "i2c status get error! \n");
		goto clk_error;
	}

	/*read the first byte data*/
	*data = (u16)readl(i2c_master->regs+HI_I2C_RXR_OFFSET);

	/*send the stop flag*/
	writel(HI_I2C_COM_STOP,i2c_master->regs+HI_I2C_COM_OFFSET);

	/*check bus is or not transfer over */
	if(I2C_OK!= balong_i2c_status_check(i2c_master)){

		i2c_print_error( "i2c status get error! \n");
		goto clk_error;
	}

	bsp_i2c_clk_disable(i2c_master);
	spin_unlock_irqrestore(&i2c_master->lock,flags);
    bsp_ipc_spin_unlock_irqrestore(IPC_SEM_I2C_SWITHC,flag);
	return I2C_OK;

clk_error:
	bsp_i2c_clk_disable(i2c_master);

irq_error:
	spin_unlock_irqrestore(&i2c_master->lock,flags);
    bsp_ipc_spin_unlock_irqrestore(IPC_SEM_I2C_SWITHC,flag);
	return I2C_ERROR;

}


/*lint -save -e958*/
s32 bsp_i2c_word_data_receive(u8 device_id,u8 addr, u16 *data)
/*lint -restore*/
{
	u32 value1 =0xff;
	u32 value2 =0xff;
	unsigned long flags = 0;
	unsigned long flag = 0;
	int ret = 0;
	struct balong_i2c_ctrl *i2c_master;

	/*find i2c master*/
	i2c_master=find_i2c_ctrl(device_id);
	if(!i2c_master){
		i2c_print_error("can find i2c master for device %0x\n",device_id);
		return I2C_ERROR;
	}

	/*lock i2c master*/
    bsp_ipc_spin_lock_irqsave(IPC_SEM_I2C_SWITHC,flag);
	spin_lock_irqsave(&i2c_master->lock,flags);

	/*clock enable*/
	ret = bsp_i2c_clk_enable(i2c_master);
	if(ret){
		i2c_print_error( "i2c clk enable failed ! \n");
		goto irq_error;
	}

	/*send the first message,include start command and write command */
	balong_i2c_message_start(i2c_master,device_id);

	/*check bus is or not transfer over */
	if(I2C_OK!= balong_i2c_status_check(i2c_master)){

		i2c_print_error( "i2c status get error! \n");
		goto clk_error;
	}

	/*send slave register addr into TX register*/
	writel(addr,i2c_master->regs+HI_I2C_TXR_OFFSET);

	/*send write command*/
	writel(HI_I2C_COM_WRITE,i2c_master->regs+HI_I2C_COM_OFFSET);

	/*check bus is or not transfer over */
	if(I2C_OK!= balong_i2c_status_check(i2c_master)){

		i2c_print_error( "i2c status get error! \n");
		goto clk_error;
	}

	/*send slave addr*/
	writel(device_id<<1|I2C_READ_FLAG,i2c_master->regs+HI_I2C_TXR_OFFSET);

	/*send start & write*/
	writel(HI_I2C_COM_START|HI_I2C_COM_WRITE,i2c_master->regs+HI_I2C_COM_OFFSET);

	/*check bus is or not transfer over */
	if(I2C_OK!= balong_i2c_status_check(i2c_master)){

		i2c_print_error( "i2c status get error! \n");
		goto clk_error;
	}
	/*send read command*/
	writel(HI_I2C_COM_READ,i2c_master->regs+HI_I2C_COM_OFFSET);

	/*check bus is or not transfer over */
	if(I2C_OK!= balong_i2c_status_check(i2c_master)){

		i2c_print_error( "i2c status get error! \n");
		goto clk_error;
	}

	/*read the first byte data*/
	value1=readl(i2c_master->regs+HI_I2C_RXR_OFFSET);

	/*send read command*/
	writel(HI_I2C_COM_READ|HI_I2C_COM_ACK,i2c_master->regs+HI_I2C_COM_OFFSET);

	/*check bus is or not transfer over */
	if(I2C_OK!= balong_i2c_status_check(i2c_master)){

		i2c_print_error( "i2c status get error! \n");
		goto clk_error;
	}

	/*read the second byte data*/
	value2=readl(i2c_master->regs+HI_I2C_RXR_OFFSET);

	/*send the stop flag*/
	writel(HI_I2C_COM_STOP,i2c_master->regs+HI_I2C_COM_OFFSET);

	/*check bus is or not transfer over */
	if(I2C_OK!= balong_i2c_status_check(i2c_master)){

		i2c_print_error( "i2c status get error! \n");
		goto clk_error;
	}

	bsp_i2c_clk_disable(i2c_master);
	spin_unlock_irqrestore(&i2c_master->lock,flags);
    bsp_ipc_spin_unlock_irqrestore(IPC_SEM_I2C_SWITHC,flag);
	*data=(u16)((value1<<8)|value2);
	return I2C_OK;

clk_error:
	bsp_i2c_clk_disable(i2c_master);

irq_error:
	spin_unlock_irqrestore(&i2c_master->lock,flags);
    bsp_ipc_spin_unlock_irqrestore(IPC_SEM_I2C_SWITHC,flag);
	return I2C_ERROR;
}

/*******************************

*       dpm   *

*********************************/
#ifdef CONFIG_CCORE_PM
static int bsp_i2c_dpm_prepare(struct dpm_device *dev)
{
	return I2C_OK;
}/*lint !e715*/
static int bsp_i2c_suspend(struct dpm_device *dev)
{
	return I2C_OK;
}/*lint !e715*/
static int bsp_i2c_resume(struct dpm_device *dev)
{
#if 0
#ifdef HI_I2C_REGBASE_ADDR
		bsp_i2c_clk_enable(&p531_i2c);/*close clk should enable it firstly,ensure with xujingcui*/

		/* set high high level cycle*/
		balong_i2c_scl_high(&p531_i2c);
		/* set high low level cycle */
		balong_i2c_scl_low(&p531_i2c);

		bsp_i2c_clk_disable(&p531_i2c);
#endif

#ifdef HI_I2C1_REGBASE_ADDR
		bsp_i2c_clk_enable(&v7r2_i2c0);/*close clk should enable it firstly,ensure with xujingcui*/

		/* set high high level cycle*/
		balong_i2c_scl_high(&v7r2_i2c0);
		/* set high low level cycle */
		balong_i2c_scl_low(&v7r2_i2c0);

		/* set high high level cycle*/
		balong_i2c_scl_high(&v7r2_i2c1);
		/* set high low level cycle */
		balong_i2c_scl_low(&v7r2_i2c1);
		bsp_i2c_clk_disable(&v7r2_i2c1);

#endif
#endif
	return I2C_OK;
}/*lint !e715*/
static int bsp_i2c_dpm_complete(struct dpm_device *dev)
{
	return I2C_OK;
}/*lint !e715*/

struct dpm_device i2c_dpm_device={
	.device_name = "i2c_dpm",
	.prepare = bsp_i2c_dpm_prepare,
	.suspend = bsp_i2c_suspend,
	.suspend_late = NULL,
	.resume = bsp_i2c_resume,
	.complete = bsp_i2c_dpm_complete,
	.resume_early = NULL,
};

#endif


s32 bsp_i2c_initial(void)
{
	s32 ret=0;

	/*initial variable*/
#ifdef HI_I2C_REGBASE_ADDR
	spin_lock_init(&p531_i2c.lock);
	p531_i2c.regs=HI_I2C_REGBASE_ADDR;
	p531_i2c.master_name="p531_i2c";
	p531_i2c.slave_num=0;

	p531_i2c.clk=clk_get(NULL,"i2c0_clk");/*default p531 use I2C0,if change ensure sync code*/
    if(ret = bsp_i2c_clk_enable(&p531_i2c))/*close clk should enable it firstly,ensure with xujingcui*/
	{
		i2c_print_error("i2c_dpm_device add error\n");
		return ERROR;
	}
	/* set high high level cycle*/
	balong_i2c_scl_high(&p531_i2c);
	/* set high low level cycle */
	balong_i2c_scl_low(&p531_i2c);


	bsp_i2c_clk_disable(&p531_i2c);
#endif

#ifdef HI_I2C1_REGBASE_ADDR
	/*i2c0 initial*/
	spin_lock_init(&v7r2_i2c0.lock);
	v7r2_i2c0.regs=HI_I2C0_REGBASE_ADDR;
	v7r2_i2c0.master_name="v7r2_i2c0";
	v7r2_i2c0.slave_num=0;

	v7r2_i2c0.clk=clk_get(NULL,"i2c0_clk");
	ret = bsp_i2c_clk_enable(&v7r2_i2c0);
	if(ret)/*close clk should enable it firstly,ensure with xujingcui*/
	{
		i2c_print_error("i2c_dpm_device add error\n");
		return ERROR;
	}

	/* set high high level cycle*/
	balong_i2c_scl_high(&v7r2_i2c0);
	/* set high low level cycle */
	balong_i2c_scl_low(&v7r2_i2c0);
	bsp_i2c_clk_disable(&v7r2_i2c0);

	/*i2c1 initial*/
	spin_lock_init(&v7r2_i2c1.lock);
	v7r2_i2c1.regs=HI_I2C1_REGBASE_ADDR;
	v7r2_i2c1.master_name="v7r2_i2c1";
	v7r2_i2c1.slave_num=0;

	v7r2_i2c1.clk=clk_get(NULL,"i2c1_clk");
	ret = bsp_i2c_clk_enable(&v7r2_i2c1);
    if(ret)/*close clk should enable it firstly,ensure with xujingcui*/
	{
		i2c_print_error("i2c_dpm_device add error\n");
		return ERROR;
	}
	/* set high high level cycle*/
	balong_i2c_scl_high(&v7r2_i2c1);
	/* set high low level cycle */
	balong_i2c_scl_low(&v7r2_i2c1);
	/*close clk*/
	bsp_i2c_clk_disable(&v7r2_i2c1);
#endif

#ifdef CONFIG_CCORE_PM
	ret = bsp_device_pm_add(&i2c_dpm_device);
	if(OK!= ret)
	{
		i2c_print_error("i2c_dpm_device add error\n");
		return ERROR;
	}
#endif

	return 0;
}

