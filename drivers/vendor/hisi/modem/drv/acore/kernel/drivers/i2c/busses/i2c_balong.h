

#ifndef I2C_BALONG_H
#define I2C_BALONG_H

#include <linux/i2c.h>
#include "bsp_i2c.h"

/* i2c controller state */
enum balong_i2c_state {
	STATE_IDLE,
	STATE_START,
	STATE_READ,
	STATE_WRITE,
	STATE_STOP
};

struct platform_i2c{
	int bus_num;
	unsigned int flags;
	unsigned int slave_addr;
	unsigned long frequency;
	unsigned int sda_delay;
};

struct balong_i2c {
	spinlock_t		lock;
	wait_queue_head_t	wait;		/*在数据发送函数his_doxfer中wait_evnt_timeout,在数据发送完成后通过 master_complete发送唤醒信号*/
	unsigned int		suspended:1;

	struct i2c_msg		*msg;	/*管理收发数据的结构体*/
	unsigned int		msg_num;
	unsigned int		msg_idx;	/*当前正在处理的msg在msg_num中的序列号*/
	unsigned int		msg_ptr;	

	unsigned int		tx_setup;/*数据放入寄存器后需要一段等待时间*/
	unsigned int		irq;

	enum balong_i2c_state	state;	
	unsigned long		clkrate;

	void __iomem		*regs;
	struct clk		*clk;
	struct device		*dev;
	struct resource		*ioarea;
	struct i2c_adapter	adap;

#ifdef CONFIG_CPU_FREQ
	struct notifier_block	freq_transition;
#endif
};

#endif

