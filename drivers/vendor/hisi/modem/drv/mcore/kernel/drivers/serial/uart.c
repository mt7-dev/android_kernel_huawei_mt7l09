
#include <osl_types.h>
#include <osl_bio.h>
#include <hi_uart.h>
#include <irq.h>
#include "console.h"
#include "hi_syscrg.h"
#include "m3_pm.h"
/*lint --e{958} */
static struct uart_chan* g_uart_chan = NULL;

int uart_transmit_char(struct uart_chan* p_uart_chan, char* ch)
{
    u32 status = 0;
    status = readl(p_uart_chan->base_addr + UART_REGOFF_USR);
    if (status & 0x02) /* transmit fifo empty */
    {
		writel(*ch, p_uart_chan->base_addr + UART_REGOFF_THR);/*lint !e732*/
		return 0;
    }
	else
	{
		return -1;
	}
}

int uart_receive_char(struct uart_chan* p_uart_chan, char* ch)
{
    u32 status = 0;
    status = readl(p_uart_chan->base_addr + UART_REGOFF_USR);
    if (status & 0x08) /* transmit fifo empty */
    {
		*ch = (char)readl(p_uart_chan->base_addr + UART_REGOFF_RBR);
		return 0;
    }
	else
	{
		return -1;
	}
}



int uart_transmit_int_enable(struct uart_chan* p_uart_chan)
{
	if (p_uart_chan->mode == SIO_MODE_INT)
	{
		/* enable transmit interrupt */
		writel(readl(p_uart_chan->base_addr + UART_REGOFF_IER)|(0x02), p_uart_chan->base_addr + UART_REGOFF_IER);
		return 0;
	}
	else
	{
		return -1;
	}
}


static void uart_transmit_int_handler(struct uart_chan* p_uart_chan)
{
    int i;
    char ch =0;
    u32 status = 0;

    /* fifo dep :16 bytes */
    for(i = 0; i < 16; i++)
    {
        status = readl(p_uart_chan->base_addr + UART_REGOFF_USR);
        if (status & 0x02) /* transmit fifo empty */
        {
            if ((*p_uart_chan->get_tx_char)(p_uart_chan->get_tx_char_context, &ch) != 0)
            {
				writel(ch, p_uart_chan->base_addr + UART_REGOFF_THR);/*lint !e732*/
            }
            else
            {
				/* disable transmit interrupt */
				writel(readl(p_uart_chan->base_addr + UART_REGOFF_IER)&(~0x02), p_uart_chan->base_addr + UART_REGOFF_IER);
				break;
            }
        }
        else
        {
            break;
        }
    }
	/* read clear */
	readl(p_uart_chan->base_addr + UART_REGOFF_IIR);
}

static void uart_reveive_int_handler(struct uart_chan* p_uart_chan)
{
    int i;
    char ch;
    char status;

    /* fifo dep :16 bytes */
    for(i = 0; i <= 16; i++)
    {
		status = (char)readl(p_uart_chan->base_addr + UART_REGOFF_USR);
        if (status & 0x08) /* reveive data avaliable */
        {
			ch = (char)readl(p_uart_chan->base_addr + UART_REGOFF_RBR);
			(void)(*p_uart_chan->put_rx_char)(p_uart_chan->put_rx_char_context, &ch);
        }
        else
        {
            break;
        }
    }
}

static irqreturn_t uart_int_handler(int irq, void *irq_data)
{
    unsigned int_status = 0;
	struct uart_chan* p_uart_chan = g_uart_chan;

	int_status = readl(p_uart_chan->base_addr + UART_REGOFF_IIR); /* read clear */
    int_status &= 0x0F;

    if (0x02 == int_status)  /* transmit register empty int */
    {
        uart_transmit_int_handler(p_uart_chan);
    }
    else if ((0x04==int_status) || (0x0C==int_status)) /* receive data avaliable int */
    {
        uart_reveive_int_handler(p_uart_chan);
    }
	else
	{
		/* nothing to do */
	}
	return IRQ_HANDLED;
}

int init_uart_reg(const struct uart_chan* p_uart_chan)
{
	unsigned status = 0;
	unsigned i =0;

	if(!p_uart_chan)
		return -1;

	/*clear line status interrupt*/
    status = readl(p_uart_chan->base_addr + UART_REGOFF_LSR);

    /*clear rcv interrupt*/
    status = readl(p_uart_chan->base_addr + UART_REGOFF_RBR);

    /*clear send empty interrupt*/
    status = readl(p_uart_chan->base_addr + UART_REGOFF_IIR);

	/*clear line busy interrupt*/
    status = readl(p_uart_chan->base_addr + UART_REGOFF_USR);

    /*config FIFO:enable FIFO,receive level=00b,send level=00b,reset xfifo and rfifo */
    writel(p_uart_chan->fifo_config, p_uart_chan->base_addr + UART_REGOFF_FCR);

    /*config UART format:data--8bit stop--1bit parity--disable */
    writel(p_uart_chan->line_config, p_uart_chan->base_addr + UART_REGOFF_LCR);

	/*config baudrate: uart must be idle */
    status = readl(p_uart_chan->base_addr + UART_REGOFF_USR);
    for(i = 0; i<1000 && (status & 0x01); i++)/*uart is busy*/
    {
		status = readl(p_uart_chan->base_addr + UART_REGOFF_USR);
    }
    if(status & 0x01)
    {
		//printk(KERN_ERR"config baudrate fail. uart is busy.");
        return -1;
    }

    /*config baudrate: enable access DLL and DLH */
    writel(readl(p_uart_chan->base_addr + UART_REGOFF_LCR) | (1 << 0x07), p_uart_chan->base_addr + UART_REGOFF_LCR);

	/*config baudrate: write DLL, DLH */
    writel((p_uart_chan->clk_freq/(16*p_uart_chan->baudrate)), p_uart_chan->base_addr + UART_REGOFF_DLL);
    writel((p_uart_chan->clk_freq/(16*p_uart_chan->baudrate))>>8, p_uart_chan->base_addr + UART_REGOFF_DLH);

    /*config baudrate: disable access DLL and DLH */
    writel(readl(p_uart_chan->base_addr + UART_REGOFF_LCR) & ~(1 << 0x07), p_uart_chan->base_addr + UART_REGOFF_LCR);

	if (p_uart_chan->highspeed)
	{
	    /* config MCR : rts, auto flow control */
	    writel(0x22, p_uart_chan->base_addr + UART_REGOFF_MCR);

	    /* config highspeed fifo */
	    writel(10, p_uart_chan->base_addr + UART_REGOFF_TXDEPTH);
	}

	return 0;
}

int uart_initilize(struct uart_chan* p_uart_chan)
{
	g_uart_chan = p_uart_chan;

	/* disable interrupt */
    writel(0x0, p_uart_chan->base_addr + UART_REGOFF_IER);

	if(init_uart_reg(p_uart_chan))
		return -1;

	if (SIO_MODE_INT == p_uart_chan->mode)
	{
		//request irq
		(void)request_irq(p_uart_chan->irq, uart_int_handler, 0, "uart irq", p_uart_chan);/*lint !e713*/
    	//enable interrupt
    	writel(0x1, p_uart_chan->base_addr + UART_REGOFF_IER);
	}
	return 0;
}

u32 uart_delay_flag = 0;
u32 uart_dll = 0;
u32 uart_dlh = 0;

/*****************************************************************************
* 函 数 名  :  balongv7r2_uart_suspend
*
* 功能描述  :  串口0挂起
*
* 输入参数  :  无

* 输出参数  :  无
*
* 返 回 值  :  0 : OK
*              1 : ERR
*
* 修改记录  :
*****************************************************************************/
int balongv7r2_m_uart_suspend()
{
    u32 tmp = 0;
    char status;
    struct uart_chan* p_uart_chan = g_uart_chan;

    if((p_uart_chan->base_addr == HI_UART0_REGBASE_ADDR) && pm_mcu_uart_wsrc_flag())
    {
        status = (char)readl(p_uart_chan->base_addr + UART_REGOFF_USR);
        while(status & 0x08)
        {
            (void)readl(p_uart_chan->base_addr + UART_REGOFF_RBR);
            status = (char)readl(p_uart_chan->base_addr + UART_REGOFF_USR);
        };

        /*config baudrate: enable access DLL and DLH */
        tmp = readl(p_uart_chan->base_addr + UART_REGOFF_LCR);
        do
        {
            (void)readl(p_uart_chan->base_addr + UART_REGOFF_RBR);
            writel(tmp | (1 << 0x07), p_uart_chan->base_addr + UART_REGOFF_LCR);
            tmp = readl(p_uart_chan->base_addr + UART_REGOFF_LCR);
        }while(!(tmp & 0x80));

    	/*config baudrate: write DLL, DLH */
        uart_dll = readl(p_uart_chan->base_addr + UART_REGOFF_DLL);
        uart_dlh = readl(p_uart_chan->base_addr + UART_REGOFF_DLH);
        writel(0x1, p_uart_chan->base_addr + UART_REGOFF_DLL);
        writel(0x0, p_uart_chan->base_addr + UART_REGOFF_DLH);

        /*config baudrate: disable access DLL and DLH */
        tmp = readl(p_uart_chan->base_addr + UART_REGOFF_LCR);
        tmp &= ~(1 << 0x07);
        writel(tmp, p_uart_chan->base_addr + UART_REGOFF_LCR);

        tmp = readl(HI_SYSCTRL_BASE_ADDR + HI_CRG_CLK_SEL1_OFFSET);
        tmp |= 0x20;
        writel(tmp, HI_SYSCTRL_BASE_ADDR + HI_CRG_CLK_SEL1_OFFSET);
    }
    else if(p_uart_chan->base_addr != HI_UART0_REGBASE_ADDR)
    {
        /* disable interrupt */
    	const struct uart_chan * dev = get_uartchan();
    	writel(0x0, dev->base_addr + UART_REGOFF_IER);
    }
    else
    {
        writel(p_uart_chan->fifo_config & ~0x1, p_uart_chan->base_addr + UART_REGOFF_FCR);
        writel(0x1 << 1, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_CLKDIS3_OFFSET);
    }

	return 0;
}

/*****************************************************************************
* 函 数 名  :  balongv7r2_uart_resume
*
* 功能描述  :  串口恢复
*
* 输入参数  :  无

* 输出参数  :  无
*
* 返 回 值  :  无
*
* 修改记录  :
*****************************************************************************/
int balongv7r2_m_uart_resume()
{
    u32 tmp = 0;
    unsigned int ch;
    char status;

    struct uart_chan* p_uart_chan = g_uart_chan;

    if((p_uart_chan->base_addr == HI_UART0_REGBASE_ADDR) && pm_mcu_uart_wsrc_flag())
    {
        tmp = readl(HI_SYSCTRL_BASE_ADDR + HI_CRG_CLK_SEL1_OFFSET);
        tmp &= (~0x20);
        writel(tmp, HI_SYSCTRL_BASE_ADDR + HI_CRG_CLK_SEL1_OFFSET);

        status = (char)readl(p_uart_chan->base_addr + UART_REGOFF_USR);
        while(status & 0x08)
        {
            ch = readl(p_uart_chan->base_addr + UART_REGOFF_RBR);
            if(ch == 0x87)
            {
                uart_delay_flag = 1;
            }
            status = (char)readl(p_uart_chan->base_addr + UART_REGOFF_USR);
        };

        /*config baudrate: enable access DLL and DLH */
        tmp = readl(p_uart_chan->base_addr + UART_REGOFF_LCR);
        do
        {
            readl(p_uart_chan->base_addr + UART_REGOFF_RBR);
            writel(tmp | (1 << 0x07), p_uart_chan->base_addr + UART_REGOFF_LCR);
            tmp = readl(p_uart_chan->base_addr + UART_REGOFF_LCR);
        }while(!(tmp & 0x80));

    	/*config baudrate: write DLL, DLH */
        writel(uart_dll, p_uart_chan->base_addr + UART_REGOFF_DLL);
        writel(uart_dlh, p_uart_chan->base_addr + UART_REGOFF_DLH);

        /*config baudrate: disable access DLL and DLH */
        tmp = readl(p_uart_chan->base_addr + UART_REGOFF_LCR);
        tmp &= ~(1 << 0x07);
        writel(tmp, p_uart_chan->base_addr + UART_REGOFF_LCR);
    }
    else if(p_uart_chan->base_addr != HI_UART0_REGBASE_ADDR)
    {
        const struct uart_chan * dev = get_uartchan();
        writel(0x0, dev->base_addr + UART_REGOFF_IER);
    	if(init_uart_reg(dev))
    		return -1;

    	if (SIO_MODE_INT == dev->mode)
    	{
        	writel(UART_REGOFF_RIE, dev->base_addr + UART_REGOFF_IER);
    	}
    }
    else
    {
        writel(0x1 << 1, HI_SYSCRG_BASE_ADDR_VIRT + HI_CRG_CLKEN3_OFFSET);
        writel(p_uart_chan->fifo_config, p_uart_chan->base_addr + UART_REGOFF_FCR);
    }
	return 0;
}