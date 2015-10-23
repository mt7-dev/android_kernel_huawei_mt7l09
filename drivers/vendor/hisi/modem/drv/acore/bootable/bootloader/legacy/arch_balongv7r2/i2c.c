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
#include <balongv7r2/i2c.h>
#include <balongv7r2/gpio.h>
#include "osl_bio.h"
#include "bsp_memmap.h"
#include "hi_base.h"
#include "hi_syscrg.h"
#include "hi_i2c.h"
#include "boot/boot.h"

#define MAX_NAME_LEN 48
#define MAX_SLAVE_NUM 20
#define TEST_SLAVE_ID	0x6b
#define I2C_TEST_PASS 0
#define I2C_TEST_FAILE -1
struct balong_i2c_ctrl {
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
int bsp_i2c_slave_register(enum i2c_master i2c,u32 slave_id)
{
	int slave_num;
	int idx;
	struct balong_i2c_ctrl *i2c_master_cur;

#ifdef HI_I2C_REGBASE_ADDR
	i2c_master_cur=&p531_i2c;
	if(I2C_MASTER1==i2c)
	{
		cprintf("p531 only has i2c_master0\n");
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
			cprintf("there are i2c_master0 or i2c_master1,don't have i2c_master%d\n",i2c);
			return I2C_ERROR;
		}
	}

#endif
	slave_num=i2c_master_cur->slave_num;
	if(slave_num>=MAX_SLAVE_NUM){
		cprintf("master has %d slaves,add salve fail!\n",slave_num);
		return I2C_ERROR;
	}

	/*ensure one slave_id can't register a master with muti times*/
	for(idx=0;idx<slave_num;idx++)
	{
		if(slave_id==i2c_master_cur->i2c_slave_list[idx])
		{
			cprintf("the slave has registed!\n");
			return I2C_ERROR;
		}
	}
	i2c_master_cur->i2c_slave_list[slave_num]=slave_id;
	i2c_master_cur->slave_num=slave_num+1;
	return I2C_OK;
	
}

/*find i2c master for slave*/
struct balong_i2c_ctrl *find_i2c_ctrl(u8 slave_id)
{
	int idx;/*i2c slave index*/
	struct balong_i2c_ctrl *i2c_master;
	
#ifdef HI_I2C_REGBASE_ADDR
	i2c_master=&p531_i2c;
	for(idx=0;idx<i2c_master->slave_num;idx++)
	{
		if(i2c_master->i2c_slave_list[idx]==slave_id){
			//cprintf("the 0x%x i2c master is %s\n",slave_id,i2c_master->master_name);
			return i2c_master;
		}
	}
	cprintf("can't find the 0x%x slave,may be it didn't have regitstered!\n",slave_id);
	return NULL;
#endif
	
#ifdef HI_I2C1_REGBASE_ADDR
	i2c_master=&v7r2_i2c0;
	for(idx=0;idx<i2c_master->slave_num;idx++)
	{
		if(i2c_master->i2c_slave_list[idx]==slave_id){
			//cprintf("the 0x%x i2c master is %s\n",slave_id,i2c_master->master_name);
			return i2c_master;
		}
	}
	
	i2c_master=&v7r2_i2c1;
	
	for(idx=0;idx<i2c_master->slave_num;idx++)
	{
		if(i2c_master->i2c_slave_list[idx]==slave_id){
			//cprintf("the 0x%x i2c master is %s\n",slave_id,i2c_master->master_name);
			return i2c_master;
		}
	}
	cprintf("can't find the 0x%x slave,may be it didn't have regitstered!\n",slave_id);
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

	if(!strcmp(i2c_master->master_name, "v7r2_i2c0"))
		set_hi_crg_clken1_i2c0_clk_en(1);
	else if(!strcmp(i2c_master->master_name, "v7r2_i2c1"))
		set_hi_crg_clken1_i2c1_clk_en(1);
	
	/*I2C ctrl enable*/
	iiccon = readl(i2c_master->regs+HI_I2C_CTRL_OFFSET);
	iiccon |= HI_I2C_CTRL_I2CEN;

	writel(iiccon, i2c_master->regs+HI_I2C_CTRL_OFFSET);
		
	return ;
}

static void balong_i2c_disable(struct balong_i2c_ctrl *i2c_master)
{
	u32 iiccon;

	if(!strcmp(i2c_master->master_name, "v7r2_i2c0"))
		set_hi_crg_clkdis1_i2c0_clk_dis(1);
	else if(!strcmp(i2c_master->master_name, "v7r2_i2c1"))
		set_hi_crg_clkdis1_i2c1_clk_dis(1);
	
	/*I2C ctrl disable*/
	iiccon = readl(i2c_master->regs+HI_I2C_CTRL_OFFSET);
	iiccon &=(~HI_I2C_CTRL_I2CEN);

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
s32 balong_i2c_status_check(struct balong_i2c_ctrl *i2c_master)
{
	u32 reg_value;
	s32 timeout=I2C_STATUS_TIMEOUT;

	
	/*Does bus  transfer over */
	do
	{
		reg_value= readl(i2c_master->regs+HI_I2C_SR_OFFSET);

		/*ÅÐ¶ÏÊÇ·ñÊÕµ½ACKÖÐ¶Ï*/
		if(reg_value&HI_I2C_SR_ACKERR){
				cprintf("i2c hasn't receive ack!status_register is:0x%x\n",reg_value);
		}

		/*ÅÐ¶ÏÊÇ·ñÓÐ×ÜÏßÖÙ²ÃÊ§°ÜµÄÇé¿ö*/
		if(reg_value&HI_I2C_SR_ABITR){
				cprintf("i2c has abitrage ERROR!status_register is:0x%x\n",reg_value);
				return I2C_ERROR;
		}
		
	}while((!(reg_value &HI_I2C_SR_INTDONE)) && (--timeout > 0));

   	 if(0 == timeout)
   	 {
    		cprintf("I2C: Timeout! \n");
       		 return I2C_ERROR;
    	}
	balong_i2c_all_mask(i2c_master);
    	return OK;
}

void balong_i2c_message_start(u8 device_id,struct balong_i2c_ctrl *i2c_master)
{
	u32 addr=0;
	u32 tmp=0;

	if(NULL == i2c_master){
		cprintf("i2c master is null\n");
		return ;
	}

	/*set i2c ctrl*/
	balong_i2c_enable(i2c_master);

	/*set high high level cycle*/
	balong_i2c_scl_high(i2c_master);

	/* set high low level cycle */
	balong_i2c_scl_low(i2c_master);
	
	/* clear all interrupt  */
	balong_i2c_all_mask(i2c_master);

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
	struct balong_i2c_ctrl *i2c_master;
	
	/*find i2c master*/
	i2c_master=find_i2c_ctrl(device_id);
	if(!i2c_master){
		cprintf("can find i2c master for device %0x\n",device_id);
		return I2C_ERROR;
	}
	/*send the first message,include start command and write command */
	balong_i2c_message_start(device_id,i2c_master);

	/*check bus is or not transfer over */
	if(I2C_OK!= balong_i2c_status_check(i2c_master)){
	
		cprintf( "i2c status get error! \n");
		goto error;
	}

	/*set addr in TX Regitster*/
	writel(addr,i2c_master->regs+HI_I2C_TXR_OFFSET);

	/*send write command*/
	writel(HI_I2C_COM_WRITE,i2c_master->regs+HI_I2C_COM_OFFSET);

	/*check bus is or not transfer over */
	if(I2C_OK!= balong_i2c_status_check(i2c_master)){
	
		cprintf( "i2c status get error! \n");
		goto error;
	}
	
	/*set data in TX register*/
	writel(data,i2c_master->regs+HI_I2C_TXR_OFFSET);

	/*send write command*/
	writel(HI_I2C_COM_WRITE,i2c_master->regs+HI_I2C_COM_OFFSET);

	/*check bus is or not transfer over */
	if(I2C_OK!= balong_i2c_status_check(i2c_master)){
	
		cprintf( "i2c status get error! \n");
		goto error;
	}
	
	/*send the stop flag ,erase bus*/
	writel(HI_I2C_COM_STOP,i2c_master->regs+HI_I2C_COM_OFFSET);

	/*check bus is or not transfer over */
	if(I2C_OK!= balong_i2c_status_check(i2c_master)){
	
		cprintf( "i2c status get error! \n");
		goto error;
	}
	balong_i2c_disable(i2c_master);

	return I2C_OK;
error:
	
	return I2C_ERROR;

}

s32 bsp_i2c_word_data_send(u8 device_id,u8 addr, u16 data)
{
	struct balong_i2c_ctrl *i2c_master;
	
	/*find i2c master*/
	i2c_master=find_i2c_ctrl(device_id);
	if(!i2c_master){
		cprintf("can find i2c master for device %0x\n",device_id);
		return I2C_ERROR;
	}
	
	/*send the first message,include start command and write command */
	balong_i2c_message_start(device_id,i2c_master);

	/*check bus is or not transfer over */
	if(I2C_OK!= balong_i2c_status_check(i2c_master)){
	
		cprintf( "i2c status get error! \n");
		goto error;
	}

	/*set addr in TX Regitster*/
	writel(addr,i2c_master->regs + HI_I2C_TXR_OFFSET);

	/*send write command*/
	writel(HI_I2C_COM_WRITE,i2c_master->regs+HI_I2C_COM_OFFSET);

	/*check bus is or not transfer over */
	if(I2C_OK!= balong_i2c_status_check(i2c_master)){
	
		cprintf( "i2c status get error! \n");
		goto error;
	}
	
	/*set high 8bit data in TX register*/
	writel(data>>8,i2c_master->regs+HI_I2C_TXR_OFFSET);

	/*send write command*/
	writel(HI_I2C_COM_WRITE,i2c_master->regs+HI_I2C_COM_OFFSET);

	/*check bus is or not transfer over */
	if(I2C_OK!= balong_i2c_status_check(i2c_master)){
	
		cprintf( "i2c status get error! \n");
		goto error;
	}
	
	/*set low 8bit data in TX register*/
	writel(data&(0xff),i2c_master->regs+HI_I2C_TXR_OFFSET);

	/*send write command*/
	writel(HI_I2C_COM_WRITE,i2c_master->regs+HI_I2C_COM_OFFSET);

	/*check bus is or not transfer over */
	if(I2C_OK!= balong_i2c_status_check(i2c_master)){
	
		cprintf( "i2c status get error! \n");
		goto error;
	}

	/*send the stop flag ,erase bus*/
	writel(HI_I2C_COM_STOP,i2c_master->regs+HI_I2C_COM_OFFSET);

	/*check bus is or not transfer over */
	if(I2C_OK!= balong_i2c_status_check(i2c_master)){
	
		cprintf( "i2c status get error! \n");
		goto error;
	}
	balong_i2c_disable(i2c_master);

	return I2C_OK;
error:
	return I2C_ERROR;
}


s32 bsp_i2c_byte_data_receive(u8 device_id, u8 addr,  u16 *data)
{

	struct balong_i2c_ctrl *i2c_master;
	
	/*find i2c master*/
	i2c_master=find_i2c_ctrl(device_id);
	if(!i2c_master){
		cprintf("can find i2c master for device %0x\n",device_id);
		return I2C_ERROR;
	}

	/*send the first message,include start command and write command */
	balong_i2c_message_start(device_id,i2c_master);

	/*check bus is or not transfer over */
	if(I2C_OK!= balong_i2c_status_check(i2c_master)){
	
		cprintf( "i2c status get error! \n");
		goto error;
	}

	/*send slave register addr into TX register*/
	writel(addr,i2c_master->regs+HI_I2C_TXR_OFFSET);

	/*send write command*/
	writel(HI_I2C_COM_WRITE,i2c_master->regs+HI_I2C_COM_OFFSET);
	
	/*check bus is or not transfer over */
	if(I2C_OK!= balong_i2c_status_check(i2c_master)){
	
		cprintf( "i2c status get error! \n");
		goto error;
	}

	/*the flag has changed ,so send restart*/
	writel(device_id<<1|I2C_READ_FLAG,i2c_master->regs+HI_I2C_TXR_OFFSET);

	/*send start & write*/
	writel(HI_I2C_COM_START|HI_I2C_COM_WRITE,i2c_master->regs+HI_I2C_COM_OFFSET);

	/*check bus is or not transfer over */
	if(I2C_OK!= balong_i2c_status_check(i2c_master)){
	
		cprintf( "i2c status get error! \n");
		goto error;
	}
	
	/*send read command*/
	writel(HI_I2C_COM_READ|HI_I2C_COM_ACK,i2c_master->regs+HI_I2C_COM_OFFSET);

	/*check bus is or not transfer over */
	if(I2C_OK!= balong_i2c_status_check(i2c_master)){
	
		cprintf( "i2c status get error! \n");
		goto error;
	}

	/*read the first byte data*/
	*data=readl(i2c_master->regs+HI_I2C_RXR_OFFSET);

	/*send the stop flag*/
	writel(HI_I2C_COM_STOP,i2c_master->regs+HI_I2C_COM_OFFSET);

	/*check bus is or not transfer over */
	if(I2C_OK!= balong_i2c_status_check(i2c_master)){
	
		cprintf( "i2c status get error! \n");
		goto error;
	}
	
	balong_i2c_disable(i2c_master);
	return I2C_OK;
error:	
	return I2C_ERROR;

}


s32 bsp_i2c_word_data_receive(u8 device_id,u8 addr, u16 *data)
{
	u32 value1;
	u32 value2;
	
	struct balong_i2c_ctrl *i2c_master;
	
	/*find i2c master*/
	i2c_master=find_i2c_ctrl(device_id);
	if(!i2c_master){
		cprintf("can find i2c master for device %0x\n",device_id);
		return I2C_ERROR;
	}

	
	/*send the first message,include start command and write command */
	balong_i2c_message_start(device_id,i2c_master);

	/*check bus is or not transfer over */
	if(I2C_OK!= balong_i2c_status_check(i2c_master)){
	
		cprintf( "i2c status get error!after_message_start \n");
		goto error;
	}

	/*send slave register addr into TX register*/
	writel(addr,i2c_master->regs+HI_I2C_TXR_OFFSET);

	/*send write command*/
	writel(HI_I2C_COM_WRITE,i2c_master->regs+HI_I2C_COM_OFFSET);
	
	/*check bus is or not transfer over */
	if(I2C_OK!= balong_i2c_status_check(i2c_master)){
	
		cprintf( "i2c status get error!after send slave register addr \n");
		goto error;
	}

	/*send slave addr and read commond*/
	writel(device_id<<1|I2C_READ_FLAG,i2c_master->regs+HI_I2C_TXR_OFFSET);

	/*send start & write*/
	writel(HI_I2C_COM_START|HI_I2C_COM_WRITE,i2c_master->regs+HI_I2C_COM_OFFSET);

	/*check bus is or not transfer over */
	if(I2C_OK!= balong_i2c_status_check(i2c_master)){
	
		cprintf( "i2c status get error!after write slave addr and write commond \n");
		goto error;
	}
	/*send read command*/
	writel(HI_I2C_COM_READ,i2c_master->regs+HI_I2C_COM_OFFSET);

	/*check bus is or not transfer over */
	if(I2C_OK!= balong_i2c_status_check(i2c_master)){
	
		cprintf( "i2c status get error!after send read commond 1 \n");
		goto error;
	}

	/*read the first byte data*/
	value1=readl(i2c_master->regs+HI_I2C_RXR_OFFSET);

	/*send read command*/
	writel(HI_I2C_COM_READ|HI_I2C_COM_ACK,i2c_master->regs+HI_I2C_COM_OFFSET);

	/*check bus is or not transfer over */
	if(I2C_OK!= balong_i2c_status_check(i2c_master)){
	
		cprintf( "i2c status get error!after send read commond 2 \n");
		goto error;
	}
	
	/*read the second byte data*/
	value2=readl(i2c_master->regs+HI_I2C_RXR_OFFSET);
	
	/*send the stop flag*/
	writel(HI_I2C_COM_STOP,i2c_master->regs+HI_I2C_COM_OFFSET);

	/*check bus is or not transfer over */
	if(I2C_OK!= balong_i2c_status_check(i2c_master)){
	
		cprintf( "i2c status get error!after send stop \n");
		goto error;
	}

	*data=(u16)((value1<<8)|value2);
	balong_i2c_disable(i2c_master);

	return I2C_OK;
error:	
	return I2C_ERROR;
}
s32 bsp_i2c_initial(void)
{
#ifdef HI_I2C_REGBASE_ADDR
	p531_i2c.regs=HI_I2C_REGBASE_ADDR;
	p531_i2c.master_name="p531_i2c";
	p531_i2c.slave_num=0;
#endif

#ifdef HI_I2C1_REGBASE_ADDR
	v7r2_i2c0.regs=HI_I2C0_REGBASE_ADDR;
	v7r2_i2c0.master_name="v7r2_i2c0";
	v7r2_i2c0.slave_num=0;

	v7r2_i2c1.regs=HI_I2C1_REGBASE_ADDR;
	v7r2_i2c1.master_name="v7r2_i2c1";
	v7r2_i2c1.slave_num=0;	
#endif
	

	return 0;
}
/************************************************************/
/*send byte data from bq24161 03# register*/
/*									*/
/************************************************************/
int debug_i2c_byte_data_send(u8 addr,u8 value)
{
	u8 slave_id = TEST_SLAVE_ID;
	u8 offset = addr;
	u8 data = value;
	u16 back_data=0;
	int ret=0;

	/*send data to register*/
	ret=bsp_i2c_byte_data_send(slave_id, offset, data);
	if(I2C_TEST_PASS!=ret)
	{
		cprintf("TEST FAIL!offset:%x,send data:%x\n",offset,data);
		return I2C_TEST_FAILE;
	}
	/*read the 0x02 register data*/
	ret=bsp_i2c_byte_data_receive(slave_id, offset, &back_data);
	if(I2C_TEST_PASS!=ret)
	{
		cprintf("TEST FAIL!read back:%x,should:%x\n",back_data,data);
		return I2C_TEST_FAILE;
	}

	/*check read data is write data or not*/
	if(data!=back_data)
	{
		cprintf("TEST FAIL!register:%d,data=%x,back_data=%x\n",offset,data,back_data);
		return I2C_TEST_FAILE;
	}
	return I2C_TEST_PASS;

}

int debug_i2c_word_data_send(u8 addr,u16 value)
{
	u8 slave_id = TEST_SLAVE_ID;
	u8 offset = addr;
	u16 data= value;
	u16 back_data=0;
	int ret=0;

	/*send data to 0x02 register*/
	ret=bsp_i2c_word_data_send(slave_id, offset, data);
	if(I2C_TEST_PASS!=ret)
	{
		cprintf("TEST FAIL!data:0x%x,offset:0x%x\n",data,offset);
		return I2C_TEST_FAILE;
	}

	/*read the 0x02 register data*/
	ret=bsp_i2c_word_data_receive(slave_id, offset, &back_data);
	if(I2C_TEST_PASS!=ret)
	{
		cprintf("TEST FAIL!offset:0x%x,backdata:0x%x\n",offset,back_data);
		return I2C_TEST_FAILE;
	}

	/*check read data is write data or not*/
	if(data!=back_data)
	{
		cprintf("TEST FAIL!data=0x%x,back_data:0x%x\n",data,back_data);
		return I2C_TEST_FAILE;
	}

	return I2C_TEST_PASS;
}

int bsp_i2c_debug(void)
{
	int i = 0;	
	int count = 0;
	unsigned int clk0 = 0;
	unsigned int clk1 = 0;

	bsp_i2c_initial();
#ifdef HI_I2C1_REGBASE_ADDR
	clk0 = get_hi_crg_clkstat1_i2c0_clk_status();
	clk1 = get_hi_crg_clkstat1_i2c1_clk_status();
	cprintf("i2c0_clk:%d  i2c1_clk:%d\n",clk0,clk1);
#endif
	bsp_i2c_slave_register(I2C_MASTER1,TEST_SLAVE_ID);

	/*µ¥×Ö½Ú²âÊÔ£¬05¼Ä´æÆ÷Á¬Ðø¶ÁÐ´50´Î*/
	count=0;
	for(i=0;i<50;i++){
		
		if(I2C_TEST_PASS!=debug_i2c_byte_data_send(0x05,0x8a))
		{
			count++;
			cprintf("register offset:0x05,send data:0x8a,times:%d\n\n",i);
		}
	}
	cprintf("test count:%d times,success %d times\n\n",i,i-count);

	/*µ¥×Ö½Ú²âÊÔ£¬02¼Ä´æÆ÷¶ÁÐ´50´Î*/
	count = 0;
	for(i=0;i<50;i++){
		if(I2C_TEST_PASS!=debug_i2c_byte_data_send(0x2,0x8a))
		{
			count++;
			cprintf("register offset:%x,send data:0x8a\n",i);
		}
	}

	cprintf("\ntest count:%d times,success %d times\n\n",i,i-count);


	/*Ë«×Ö½Ú²âÊÔ£¬05¼Ä´æÆ÷¶ÁÐ´50´Î*/
	count=0;
	for(i=0;i<50;i++){
		
		if(I2C_TEST_PASS!=debug_i2c_word_data_send(0x02,0x8a8a))
		{
			count++;
			cprintf("register offset:0x05,send data:0x8a,times:%d\n\n",i);
		}
	}
	cprintf("\ntest count:%d times,success %d times\n\n",i,i-count);

	/*Ë«×Ö½Ú²âÊÔ£¬02¼Ä´æÆ÷¶ÁÐ´50´Î*/
	count = 0;
	for(i=0;i<50;i++){
		if(I2C_TEST_PASS!=debug_i2c_word_data_send(0x05,0x8a8a))
		{
			count++;
			cprintf("register offset:0x02,send data:0x8a8a\n",i);
		}
	}
	cprintf("\ntest count:%d times,success %d times\n\n",i,i-count);

	return 0;
}



