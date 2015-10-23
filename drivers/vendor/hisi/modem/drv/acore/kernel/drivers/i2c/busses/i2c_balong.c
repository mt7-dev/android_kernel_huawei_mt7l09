/******************************************************************************/
/*  Copyright (C), 2007-2013, Hisilicon Technologies Co., Ltd. */
/******************************************************************************/
/* File name     : i2c_balong.c */
/* Version        : 1.0 */
/* Created       : 2013-03-07*/
/* Last Modified : */
/* Description   :  The C union definition file for the module I2C*/
/* Function List : */
/* History       : */
/* 1 Date        : */
/* Modification  : Create file */
/******************************************************************************/
/*lint --e{537}*/
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/time.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/cpufreq.h>
#include <linux/slab.h>
#include <linux/io.h>
/*lint -save -e322 -e7*/
#include <asm/irq.h>
/*lint -restore*/
#include "product_config.h"
#include "hi_i2c.h"
#include "soc_memmap.h"
#include "i2c_balong.h"
#include "drv_clk.h"
#include "bsp_ipc.h"
#define i2c_arb_bypass 0x3e00
/*complete msg and wake up caller */
/*lint -save -e958*/
static inline void balong_i2c_master_complete(struct balong_i2c *i2c, int ret)
{
	i2c_print_info("master_complete %d\n", ret);
	i2c->msg_ptr = 0;
	i2c->msg = NULL;
	i2c->msg_idx++;
	i2c->msg_num = 0;
	if (ret){
		i2c->msg_idx = (unsigned int)ret;
	}
	wake_up(&i2c->wait);
}

/* set high level cycle*/
static inline void balong_i2c_scl_high(struct balong_i2c *i2c)
{
	unsigned long tmp;
	tmp=HI_I2C_SCL_H_NUM;     /* [false alarm]:ÆÁ±ÎFortify´íÎó */

	/*lint -save -e124 -e516*/
	writel(tmp, i2c->regs+HI_I2C_SCL_H_OFFSET);
	/*lint -restore*/

}

/* set low level cycle */
static inline void balong_i2c_scl_low(struct balong_i2c *i2c)
{
	unsigned long tmp;
	tmp=HI_I2C_SCL_L_NUM;       /* [false alarm]:ÆÁ±ÎFortify´íÎó */

	/*lint -save -e124 -e516*/
	writel(tmp, i2c->regs+HI_I2C_SCL_L_OFFSET);
	/*lint -restore*/
}

/*dis ack err irq*/
static inline void balong_i2c_disable_ack(struct balong_i2c *i2c)
{
	unsigned long tmp;

	/*lint -save -e124 -e516*/
	tmp=readl(i2c->regs+HI_I2C_CTRL_OFFSET);
	writel(tmp& ~HI_I2C_CTRL_ACKEN,i2c->regs+HI_I2C_CTRL_OFFSET);
	/*lint -restore*/
}

/* disable irq */
static inline void balong_i2c_disable_irq(struct balong_i2c *i2c)
{
	unsigned long tmp;

	/*lint -save -e124 -e516*/
	tmp=readl(i2c->regs+HI_I2C_CTRL_OFFSET);
	tmp&=~HI_I2C_CTRL_IRQEN;		/*I2C ctrl disable*/
	tmp&=~HI_I2C_CTRL_ACKEN;		/*ack err interrupt*/
	tmp&=~HI_I2C_CTRL_ARBLOSS;		/*arb loss intterupt disable*/
	tmp&=~HI_I2C_CTRL_DONE;		/*transfer done intterupt disable*/     /* [false alarm]:ÆÁ±ÎFortify´íÎó */

	writel(tmp & ~HI_I2C_CTRL_IRQEN,i2c->regs+HI_I2C_CTRL_OFFSET);
	/*lint -restore*/
}

/*enable i2c ctrl*/
static inline void balong_i2c_set_ctrl(struct balong_i2c *i2c)
{
	unsigned  long iiccon;

	iiccon = (HI_I2C_CTRL_I2CEN | i2c_arb_bypass);	 /*I2C ctrl enable*/
	iiccon|=HI_I2C_CTRL_IRQEN;	/*I2C interrupt enable*/
	iiccon|=HI_I2C_CTRL_ACKEN;	/*I2C ackerr interrupt enable*/
	iiccon|=HI_I2C_CTRL_ARBLOSS;/*arb loss intterupt enable*/
	iiccon|=HI_I2C_CTRL_DONE;	/*transfer done intterupt enable*/    /* [false alarm]:ÆÁ±ÎFortify´íÎó */
	/*lint -save -e124 -e516*/
	writel(iiccon, i2c->regs+HI_I2C_CTRL_OFFSET);
	/*lint -restore*/
}

/*clear all interrupts */
static inline void balong_i2c_all_mask(struct balong_i2c *i2c)
{
	unsigned long tmp;
	tmp=HI_I2C_ICR_ALLMASK;     /* [false alarm]:ÆÁ±ÎFortify´íÎó */

	/*lint -save -e124 -e516*/
	writel(tmp, i2c->regs+HI_I2C_ICR_OFFSET);
	/*lint -restore*/
}


/* check bus status */
static int balong_i2c_transfer_status (struct balong_i2c *i2c)
{
	int timeout=I2C_STATUS_TIMEOUT;
	unsigned int u32RegValue;

	/*lint -save -e124 -e516*/
	/*see if bus transfer done*/
    do{
       	u32RegValue= readl(i2c->regs+HI_I2C_SR_OFFSET);
    }while((!(u32RegValue &HI_I2C_SR_INTDONE)) && (--timeout > 0));
	/*lint -restore*/
   	if(0 == timeout)
   	{
		i2c_print_error("I2C: Timeout! \n");
   		return I2C_ERROR;
    }

    return I2C_OK;
}



/*  enable i2c ctrl£¬send start command and write command */
static inline void balong_i2c_message_start(struct balong_i2c *i2c, struct i2c_msg *msg)
{
	unsigned long addr;
	unsigned long tmp=0;

	/*set transfer speed*/
	balong_i2c_scl_high(i2c);
	balong_i2c_scl_low(i2c);

	/*clear all interrupt */
	balong_i2c_all_mask(i2c);

	/*enable i2c control and enable interrupt */
	balong_i2c_set_ctrl(i2c);

	/* if read ,set the last bit 1,else set it 0 */
	addr=(msg->addr&SLAVE_ID_MASK)<<1;
	if(msg->flags & I2C_M_RD){
		addr|=1;    /* [false alarm]:ÆÁ±ÎFortify´íÎó */
	}

	/*lint -save -e124 -e516*/
	writel(addr,i2c->regs+HI_I2C_TXR_OFFSET);
	ndelay(i2c->tx_setup);

	/*send start and write command */
	tmp=HI_I2C_COM_START|HI_I2C_COM_WRITE;          /* [false alarm]:ÆÁ±ÎFortify´íÎó */
	writel(tmp, i2c->regs+HI_I2C_COM_OFFSET);
	i2c_print_info("ctrl:0x%x\ncom:0x%x\nSR:0x%x\nRx:0x%x\nTx:0x%x\n",readl(i2c->regs+HI_I2C_CTRL_OFFSET),readl(i2c->regs+HI_I2C_COM_OFFSET),readl(i2c->regs+HI_I2C_SR_OFFSET),\
		readl(i2c->regs+HI_I2C_RXR_OFFSET),readl(i2c->regs+HI_I2C_TXR_OFFSET));
	/*lint -restore*/

}

/* send stop command */
static inline void balong_i2c_stop(struct balong_i2c *i2c, int ret)
{
	unsigned long workstate;

	i2c_print_info("current transfer STOP!\n");

	/*clear all interrupts*/
	balong_i2c_all_mask( i2c);

	/*send stop command */
	workstate=HI_I2C_COM_STOP;      /* [false alarm]:ÆÁ±ÎFortify´íÎó */

	/*lint -save -e124 -e516*/
	writel(workstate, i2c->regs+HI_I2C_COM_OFFSET);
	/*lint -restore*/

	balong_i2c_transfer_status(i2c);

	/*setup the work status is "STOP"*/
	i2c->state = STATE_STOP;

	balong_i2c_master_complete(i2c, ret);
	balong_i2c_disable_irq(i2c);

	//clk_disable(i2c->clk);
	return ;

}

/*check to see if the last msg in a transfer  */
static inline int is_lastmsg(struct balong_i2c *i2c)
{
	return i2c->msg_idx >= (i2c->msg_num - 1);
}

/* check to see if the last byte in a msg  */
static inline int is_msglast(struct balong_i2c *i2c)
{
	return i2c->msg_ptr == i2c->msg->len-1;
}


/* check to see if the end in a msg */
static inline int is_msgend(struct balong_i2c *i2c)
{
	return i2c->msg_ptr >= i2c->msg->len;
}

/*read next byte */
 int balong_i2c_irq_nextbyte(struct balong_i2c *i2c,unsigned long workstate)
{

	unsigned long tmp;
    unsigned Datatemp;
	int ret=0;
	unsigned long tmp_read=0;
	unsigned char byte;

	switch(i2c->state){

	case STATE_IDLE:
		i2c_print_error("i2c state is idle!\n");
		goto out;
	case STATE_STOP:
		i2c_print_info("called in STATE_STOP\n");
		balong_i2c_disable_irq(i2c);
		goto out;

	case STATE_START:
		/* last thing we did was send a start condition on the
		 * bus, or started a new i2c message
		 */
		if ((workstate&HI_I2C_SR_ACKERR)&&
		    !(i2c->msg->flags & I2C_M_IGNORE_NAK)) {
			/*ACK was not received .....*/
			i2c_print_error("ack was not received\n");
			balong_i2c_stop(i2c, -ENXIO);
			goto out;
		}

		/* ÉèÖÃI2CµÄ¹¤×÷×´Ì¬  */
		if(i2c->msg->flags&I2C_M_RD)
			i2c->state=STATE_READ;
		else
			i2c->state=STATE_WRITE;

		if (is_lastmsg(i2c) && i2c->msg->len == 0) {
			/*Èç¹ûÊÇÏûÏ¢¶ÓÁÐÖÐµÄ×îºóÒ»ÌõÊý¾Ý£¬²¢ÇÒÃ»ÓÐÒª·¢ËÍµÄ£¬ÔòÍ£Ö¹*/
			balong_i2c_stop(i2c, 0);
			goto out;
		}

		if (i2c->state == STATE_READ)
			goto prepare_read;

		/* fall through to the write state, as we will need to
		 * send a byte as well */
	case STATE_WRITE:
		if (!(i2c->msg->flags & I2C_M_IGNORE_NAK)) {
			if (workstate& HI_I2C_SR_ACKERR) {
				i2c_print_error("WRITE: AckErr!\n");
				balong_i2c_stop(i2c, -ECONNREFUSED);
				goto out;
			}
		}
retry_write:
		if(!is_msgend(i2c)){
			byte=i2c->msg->buf[i2c->msg_ptr++];
			tmp=byte;       /* [false alarm]:ÆÁ±ÎFortify´íÎó */
			/*lint -save -e124 -e516*/
			writel(tmp,i2c->regs+HI_I2C_TXR_OFFSET);
			ndelay(i2c->tx_setup);

			/* send write command*/
			writel((unsigned int)HI_I2C_COM_WRITE, i2c->regs+HI_I2C_COM_OFFSET);
			ndelay(i2c->tx_setup);
			/*lint -restore*/
		}
		else if(!is_lastmsg(i2c)){

			/*we are need go to next message */
			i2c_print_info("\nWRITE: Next Message\n");
			i2c->msg_ptr = 0;
			i2c->msg_idx++;
			i2c->msg++;

			/*check to see if we need to send restart */
			if (i2c->msg->flags & I2C_M_NOSTART) {

				if (i2c->msg->flags & I2C_M_RD) {
 					/*send new start if change send data direction*/
					balong_i2c_stop(i2c, -EINVAL);
				}

				goto retry_write;
			}else{
				/* send restart */
				balong_i2c_message_start(i2c, i2c->msg);
				i2c->state = STATE_START;
			}
		}
		else{

			/*send stop*/
			balong_i2c_stop(i2c, 0);
		}
		break;
	case STATE_READ:
		i2c_print_info("STATE_READ\n");
		/* move data from register to data buffer */

		/*lint -save -e124 -e516*/
		Datatemp = readl(i2c->regs + HI_I2C_RXR_OFFSET);
		/*lint -restore*/
		i2c->msg->buf[i2c->msg_ptr++] = (unsigned char)Datatemp;



 prepare_read:
		if (is_msglast(i2c)) {/*is last byte in a msg*/
			if (is_lastmsg(i2c))/*is the last msg in a transfer*/
				/*if the last byte ,don't send ack */
				tmp_read|=HI_I2C_COM_ACK;
		} else if (is_msgend(i2c)) {
			/* ok, we've read the entire buffer, see if there
			 * is anything else we need to do */
			if (is_lastmsg(i2c)) {
				/* last message, send stop and complete */
				i2c_print_info("READ: Send Stop\n");
				balong_i2c_stop(i2c, 0);
				goto out;
			} else {
				/* go to the next transfer */
				i2c_print_info("READ: Next Transfer\n");

				i2c->msg_ptr = 0;
				i2c->msg_idx++;
				i2c->msg++;

				/*  for test */
				/* check to see if need send restart */
				if (i2c->msg->flags & I2C_M_NOSTART) {

					if (i2c->msg->flags & I2C_M_RD){
						goto prepare_read;
					}
					else{
					/*send new start if change send data direction*/
					balong_i2c_stop(i2c, -EINVAL);
					goto out;
					}
				}else{
					/*send restart*/
					balong_i2c_message_start(i2c, i2c->msg);
                    i2c->state = STATE_START;
					goto out;
				}
				/* end for test */

			}
		}
		/*send read command */
		tmp_read|=HI_I2C_COM_READ;  /* [false alarm]:ÆÁ±ÎFortify´íÎó */
		/*lint -save -e737*/
		udelay(50);
		/*lint -restore*/
		/*lint -save -e124 -e516*/
		writel(tmp_read, i2c->regs+HI_I2C_COM_OFFSET);
		/*lint -restore*/
		break;
	 }

out:

	/*lint -save -e124 -e516*/
	i2c_print_info("ctrl:0x%x\ncom:0x%x\nSR:0x%x\nRx:0x%x\nTx:0x%x\n",readl(i2c->regs+HI_I2C_CTRL_OFFSET),readl(i2c->regs+HI_I2C_COM_OFFSET),readl(i2c->regs+HI_I2C_SR_OFFSET),\
		readl(i2c->regs+HI_I2C_RXR_OFFSET),readl(i2c->regs+HI_I2C_TXR_OFFSET));
	/*lint -restore*/

	return ret;

}

static irqreturn_t balong_i2c_irq(int irqno,void *dev_id)
{
	struct balong_i2c *i2c=dev_id;
	unsigned long status;
	i2c_print_info("balong_i2c_irq\n");

	/*get status register and clear interrupt*/
	/*lint -save -e124 -e516*/
	status=readl(i2c->regs+HI_I2C_SR_OFFSET);
	writel(status&0x7f, i2c->regs+HI_I2C_ICR_OFFSET);
	/*lint -restore*/

	/*check arbtation of bus*/
	if(status &HI_I2C_SR_ABITR)
	{
		i2c_print_error("deal with arbitration loss\n");

		/*lint -save -e124 -e516*/
		writel(0x0, i2c->regs+HI_I2C_COM_OFFSET);
		writel(~HI_I2C_SR_ABITR, i2c->regs+HI_I2C_SR_OFFSET);
		/*lint -restore*/
		return IRQ_HANDLED;

	}

	if(i2c->state==STATE_IDLE)
	{
		goto out;
	}
	/* pretty much this leaves us with the fact that we've
	 * transmitted or received whatever byte we last sent */
	balong_i2c_irq_nextbyte(i2c, status);

out:
	return IRQ_HANDLED;

}

/*check bus status*/
static int balong_i2c_set_master(struct balong_i2c *i2c)
{
	unsigned long workstate;
	int timeout=400;
	while(timeout-->0){
		/*lint -save -e124 -e516*/
		workstate=readl(i2c->regs+HI_I2C_SR_OFFSET);
		/*lint -restore*/
		if(!(workstate&HI_I2C_SR_BUSBUSY))
			return 0;
	}
	i2c_print_error("i2c set master time out !\n");
	return -ETIMEDOUT;
}

static int balong_i2c_doxfer(struct balong_i2c *i2c,struct i2c_msg * msgs, int num)
{
	unsigned long workstate,timeout;
	int spins = 20;
	unsigned long flag_local = 0;
	unsigned long flag = 0;
	int ret;

	if(i2c->suspended)
		return -EIO;

	i2c_print_info("balong_i2c_doxfer!\n");


	/* add lock */
    bsp_ipc_spin_lock_irqsave(IPC_SEM_I2C_SWITHC,flag);
	/*lint -save -e746 -e718*/
	spin_lock_irqsave(&i2c->lock, flag_local);
	/*lint -restore*/
	ret=clk_enable(i2c->clk);
	if(!ret){
		i2c_print_info("enable %sclk OK !!!\n",i2c->clk->name);
	}
	/* get i2c bus  */
	ret=balong_i2c_set_master(i2c);
	if(ret!=0){
		i2c_print_error("cannot get bus (error %d)\n",ret);
		ret=-EAGAIN;
		goto out;
	}


	i2c_print_info("i2c clk enable!\n");
	i2c->msg=msgs;
	i2c->msg_num=(unsigned int)num;
	i2c->msg_ptr=0;
	i2c->msg_idx=0;
	i2c->state=STATE_START;

	balong_i2c_message_start(i2c,msgs);
	spin_unlock_irqrestore(&i2c->lock, flag_local);
    bsp_ipc_spin_unlock_irqrestore(IPC_SEM_I2C_SWITHC,flag);

	/*wait timeout */
	/*lint -save -e665*/
	timeout=(unsigned long)wait_event_timeout(i2c->wait,i2c->msg_num==0,HZ*5);
	/*lint -restore*/
	/* having these next two as dev_err() makes life very
	 * noisy when doing an i2cdetect */
	ret=(int)i2c->msg_idx;
	if (timeout == 0){
		i2c_print_error("timeout\n");
	}
	else if (ret != num){
		i2c_print_error("incomplete xfer (%d)\n", ret);
	}

	/* first, try busy waiting briefly*/
	do {
		/*lint -save -e124 -e516*/
		workstate= readl(i2c->regs + HI_I2C_SR_OFFSET);
		/*lint -restore*/
	} while ((workstate& HI_I2C_SR_START) && --spins);

	/* if that timed out sleep */
	if (!spins) {
		/*lint -save -e124 -e516*/
		workstate= readl(i2c->regs + HI_I2C_SR_OFFSET);
		/*lint -restore*/
	}

	if (workstate& HI_I2C_SR_START)
	{
		i2c_print_info("timeout waiting for bus idle\n");
	}

	clk_disable(i2c->clk);

	return ret;
out:

	clk_disable(i2c->clk);
	spin_unlock_irqrestore(&i2c->lock, flag_local);
    bsp_ipc_spin_unlock_irqrestore(IPC_SEM_I2C_SWITHC,flag);
	return ret;
}
static int balong_i2c_xfer(struct i2c_adapter *adap, struct i2c_msg *msgs,int num)
{
	struct balong_i2c *i2c=(struct balong_i2c *)adap->algo_data;
	int retry;
	int ret;

	for(retry=0;retry<adap->retries;retry++){
		ret=balong_i2c_doxfer(i2c,msgs,num);
		if(ret!=-EAGAIN)
			return ret;
		i2c_print_info("Retrying transmissin(%d)\n",retry);
		/*lint -save -e737*/
		udelay(100);
		/*lint -restore*/
	}
	return -EREMOTEIO;
}

static u32 balong_i2c_func(struct i2c_adapter *adap)
{
	return I2C_FUNC_I2C | I2C_FUNC_SMBUS_EMUL | I2C_FUNC_PROTOCOL_MANGLING;
}

static const struct i2c_algorithm balong_i2c_algorithm = {
	.master_xfer =balong_i2c_xfer,
	.functionality= balong_i2c_func,

};

/*enable i2c ctrl */
static inline int balong_i2c_init(struct balong_i2c *i2c)
{
	unsigned long iicon;

	/*enable i2c contrler */
	iicon=(HI_I2C_CTRL_I2CEN | i2c_arb_bypass);        /* [false alarm]:ÆÁ±ÎFortify´íÎó */
	/*lint -save -e124 -e516*/
	writel(iicon,i2c->regs+HI_I2C_CTRL_OFFSET);
	/*lint -restore*/

	return 0;

}

static int balong_i2c_remove(struct platform_device *pdev)
{
	int ret;
	struct balong_i2c *i2c = platform_get_drvdata(pdev);

    if(NULL == i2c){
		i2c_print_error("i2c is null pointer \n");
		return I2C_ERROR;
	}
	/*lint -save -e718 -e746*/
	ret = i2c_del_adapter(&i2c->adap);/* coverity[dereference] */
	/*lint -restore*/

	free_irq(i2c->irq, i2c);

	clk_put(i2c->clk);

	/*lint -save -e124 -e516*/
	iounmap(i2c->regs);
	/*lint -restore*/

	release_resource(i2c->ioarea);
	kfree(i2c->ioarea);
	kfree(i2c);

	return ret;

}

static int balong_i2c_probe(struct platform_device *pdev)
{
	struct balong_i2c *i2c;
	struct platform_i2c *pdata;
	struct resource *res;
	int ret = 0;

	pdata=pdev->dev.platform_data;
	if(!pdata){
		i2c_print_error("no platform data\n");
		return -EINVAL;
	}

	i2c=kzalloc(sizeof(struct balong_i2c),GFP_KERNEL);
	if(!i2c){
		i2c_print_error("no memory for state\n");
		return -ENOMEM;
	}

	/*initial i2c driver structure*/

	i2c->adap.owner=THIS_MODULE;
	i2c->adap.class=I2C_CLASS_HWMON | I2C_CLASS_SPD;
	i2c->adap.algo=&balong_i2c_algorithm;
	i2c->adap.retries=2;
	i2c->tx_setup=50;
	init_waitqueue_head(&i2c->wait);
	spin_lock_init(&i2c->lock);
	strlcpy(i2c->adap.name,"balong-i2c",sizeof(i2c->adap.name));

	/*find the clock and enable it */
	if(pdev->id == 0){
		i2c->clk=clk_get(NULL,"i2c0_clk");
	}
	else{
		i2c->clk=clk_get(NULL,"i2c1_clk");
	}
	if(IS_ERR(i2c->clk)){
		i2c_print_error("cannot get i2c%d clk!\n",pdev->id);
		goto err_noclk;
	}

	dev_dbg(&pdev->dev,"clock source %p\n",i2c->clk);
	ret = clk_enable(i2c->clk);/*close clk should enable it firstly,ensure with xujingcui*/
    if (ret){
        i2c_print_error("clock enable failed\n");
		goto err_clk_enable;
    }
	/*  map the register */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (res == NULL) {
		i2c_print_error("cannot find IO resource\n");
		ret = -ENOENT;
		goto err_clk;
	}

	i2c->ioarea=request_mem_region(res->start, resource_size(res),pdev->name);

	if(i2c->ioarea==NULL){
		i2c_print_error("cannot request IO \n");
		ret=-ENOENT;
		goto err_clk;

	}

	/*lint -save -e124 -e64*/
	i2c->regs=ioremap(res->start,resource_size(res));

	if(i2c->regs==NULL){
		i2c_print_error("cannot map IO\n");
		ret=-ENXIO;
		goto err_ioarea;
	}

	i2c_print_info("registers %p \n",
		i2c->regs);
	/*lint -restore*/

	/* setup info block for the i2c core */

	i2c->adap.algo_data = i2c;
	i2c->adap.dev.parent = &pdev->dev;
	//i2c->adap.nr = pdev->id;
	/*initialise the i2c controller */

	ret =balong_i2c_init(i2c);
	if (ret != 0)
		goto err_iomap;

	/*find the IRQ for this unit (note,this relies on the init call to
	 * ensure no current IRQs pending)
	 */

	ret = platform_get_irq(pdev, 0);
	if (ret <= 0) {
		i2c_print_error("cannot find IRQ\n");
		goto err_iomap;
	}
	i2c->irq = (unsigned int)ret;

	ret = request_irq(i2c->irq,balong_i2c_irq, IRQF_DISABLED,
			  dev_name(&pdev->dev), i2c);
	if (ret != 0) {
		i2c_print_error("cannot claim IRQ %d\n", i2c->irq);
		goto err_iomap;
	}
	i2c_print_info("INT NUMBER:  %d \n",i2c->irq);


	/*setup i2c transfer speed*/
	balong_i2c_scl_high(i2c);
	balong_i2c_scl_low(i2c);

	i2c->adap.nr = pdata->bus_num;

	/*lint -save -e718 -e746*/
	ret = i2c_add_numbered_adapter(&i2c->adap);
	/*lint -restore*/
	if (ret < 0) {
		i2c_print_error( "failed to add bus to i2c core\n");
		goto err_irq;
	}

	platform_set_drvdata(pdev, i2c);
	clk_disable(i2c->clk);

	/*
	pm_runtime_enable(&pdev->dev);
	pm_runtime_enable(&i2c->adp.dev);
	*/

	i2c_print_error("%s: HIS I2C adapter probe ok!\n", dev_name(&i2c->adap.dev));
	return 0;

 err_irq:
	free_irq(i2c->irq,i2c);

err_iomap:
	/*lint -save -e124*/
	iounmap(i2c->regs);
	/*lint -restore*/


err_ioarea:
	release_resource(i2c->ioarea);
	kfree(i2c->ioarea);

err_clk:
	clk_disable(i2c->clk);
err_clk_enable:
    clk_put(i2c->clk);

err_noclk:
	kfree(i2c);

	return (ret);

}

#ifdef CONFIG_PM
static int balong_i2c_dpm_prepare(struct device *dev)
{
	return I2C_OK;
}

static int balong_i2c_suspend_noirq(struct device *dev)
{
	struct platform_device *pdev=to_platform_device(dev);
	struct balong_i2c *i2c=platform_get_drvdata(pdev);

	if(NULL == i2c){
		i2c_print_error("i2c is null pointer \n");
		return I2C_ERROR;
	}
	i2c->suspended =1;
	return I2C_OK;
}

static int balong_i2c_resume_noirq(struct device *dev)
{
	struct platform_device *pdev=to_platform_device(dev);
	struct balong_i2c *i2c=platform_get_drvdata(pdev);

	if(NULL == i2c){
		i2c_print_error("i2c is null pointer \n");
		return I2C_ERROR;
	}

	i2c->suspended =0;


	return 0;
}

static void balong_i2c_dpm_complete(struct device *dev)
{
	return ;
}
static const struct dev_pm_ops balong_i2c_dev_pm_ops ={
	.prepare = balong_i2c_dpm_prepare,
	.suspend_noirq = balong_i2c_suspend_noirq,
	.resume_noirq = balong_i2c_resume_noirq,
	.complete = balong_i2c_dpm_complete
};

#define BALONG_DEV_PM_OPS (&balong_i2c_dev_pm_ops)
#endif
#ifdef HI_I2C1_REGBASE_ADDR
static struct platform_driver balong_i2c0_driver = {
	.probe		= balong_i2c_probe,
	.remove		= balong_i2c_remove,
	.driver		= {
		.owner	= THIS_MODULE,
		.name	= "balong_i2c_v7r2_0",
#ifdef CONFIG_PM
		.pm		= BALONG_DEV_PM_OPS
#endif
	},
};

static struct platform_driver balong_i2c1_driver = {
	.probe		= balong_i2c_probe,
	.remove		= balong_i2c_remove,
	.driver		= {
		.owner	= THIS_MODULE,
		.name	= "balong_i2c_v7r2_1",
#ifdef CONFIG_PM
		.pm		= BALONG_DEV_PM_OPS
#endif
	},
};
#endif
#ifdef HI_I2C_REGBASE_ADDR
static struct platform_driver balong_i2c_driver={
	.probe		= balong_i2c_probe,
	.remove		= balong_i2c_remove,
	.driver		= {
		.owner	= THIS_MODULE,
		.name	= "balong_i2c_p531",
#ifdef CONFIG_PM
		.pm		= BALONG_DEV_PM_OPS
#endif
	},

};
#endif

static int __init i2c_adapter_balong_init(void)
{
	int ret=0;
#ifdef HI_I2C1_REGBASE_ADDR
	 ret= platform_driver_register(&balong_i2c0_driver);
	 ret|= platform_driver_register(&balong_i2c1_driver);
	 return ret;
#endif
#ifdef HI_I2C_REGBASE_ADDR
	 ret= platform_driver_register(&balong_i2c_driver);
	 return ret;
#endif
	/*lint -save -e527*/
	return 0;
	/*lint -restore*/
}

arch_initcall(i2c_adapter_balong_init);
static void __exit i2c_adapter_balong_exit(void)
{
#ifdef HI_I2C1_REGBASE_ADDR
	platform_driver_unregister(&balong_i2c0_driver);
	platform_driver_unregister(&balong_i2c1_driver);
#endif
#ifdef HI_I2C_REGBASE_ADDR
	platform_driver_unregister(&balong_i2c_driver);
#endif

}

module_exit(i2c_adapter_balong_exit);

MODULE_DESCRIPTION("balong I2C Bus driver");
/*lint -restore*/

