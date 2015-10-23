
#ifndef ___SERIAL__H__ 
#define ___SERIAL__H__ 

#ifndef _ASMLANGUAGE

#define UART_BASE_ADDR    0x90007000
#define UART_INT_NUM      87
#define SIO_MODE_INT 0

#define UART_REGOFF_MCR 0x10
#define UART_REGOFF_TXDEPTH 0x2C

/* Register description OF ARM AMBA UART */
#define UART_REGOFF_DR      0x00         /* UART data register (R/W) */
#define UART_REGOFF_FCR     0x08         /*Fifo Control register(W)*/
#define UART_REGOFF_IIR     0x08         /*Int Status register(R)*/
#define UART_REGOFF_LCR     0x0C         /*UART Line Control register */
#define UART_REGOFF_IER     0x04         /*Int Enable register */
#define UART_REGOFF_USR     0x7C         /*UART Status register */
#define UART_REGOFF_DLL     0x00         /*UART Divisor Low bits*/
#define UART_REGOFF_DLH     0x04         /*UART Divisor High bits*/
#define UART_REGOFF_LSR     0x14         /*UART Line Status Register*/
#define UART_DEF_LSR_TEMT   0x40         /*发送空标志*/

#define UART_DEF_PARITY_NONE    0x00        /* set no parity */
#define UART_DEF_ONE_STOP       0x00        /* set one stop bit */

#define UART_DEF_FIFO_ENABLE    0x01        /* Enable both FIFOs */
#define UART_DEF_WORD_LEN_5     0x00    /* Set UART word lengths */
#define UART_DEF_WORD_LEN_6     0x01
#define UART_DEF_WORD_LEN_7     0x02
#define UART_DEF_WORD_LEN_8     0x03

#define UART_REGOFF_LSIE   0x04         /*Line Status Int Enable*/
#define UART_REGOFF_TIE    0x02         /*Transmitter Int Enable*/
#define UART_REGOFF_RIE    0x01         /*Receiver Int Enable*/

#define UART_DEF_FIFO_OPTS_SET       0x1030  /* FIFO 选项设置*/
#define UART_DEF_FIFO_OPTS_GET       0x1031  /* FIFO 选项状态读取*/

/* 硬件选项配置对应宏*/
#define UART_DEF_LCR_DLS    0x03 /* 数据长度选择 */
#define UART_DEF_LCR_STOP   0x04 /* 停止位个数*/
#define UART_DEF_LCR_PEN    0x08 /* 奇偶校验使能*/
#define UART_DEF_LCR_EPS    0x10 /* 奇偶校验选择*/
#define UART_DEF_LCR_BC     0x40 /* 暂停控制位*/

/* LCR默认值*/
#define UART_DEF_LCR_DEFAULTVALUE  0x00 

/* 设置串行数据流中的每个字符的个数*/
#define UART_DEF_LCR_CS8    0x03 /* 8bit*/
#define UART_DEF_LCR_CS7    0x02 /* 7bit*/
#define UART_DEF_LCR_CS6    0x01 /* 6bit*/
#define UART_DEF_LCR_CS5    0x00 /* 5bit*/

/* 停止位设置 */
/*在LCR bit[1:0]=0 的条件下为1.5 个停止位，否则为2 个停止位*/
#define UART_DEF_LCR_STP2   0x04
/*1个停止位*/
#define UART_DEF_LCR_STP1   0x00 

#define UART_DEF_LCR_PEN    0x08 /* 校验使能*/
#define UART_DEF_LCR_EPS    0x10 /* 校验选择*/

#define UART_DEF_LCR_BC_NORMAL   0x00   /* 暂停控制位 正常*/
#define UART_DEF_LCR_BC_UNNORMAL 0x40   /* 暂停控制位 暂停*/

/* FIFO 控制相关*/
#define UART_DEF_FIFOE  0x1  /* FIFO 使能控制*/
#define UART_DEF_TET    0x30 /* 发送FIFO 的空水线*/
#define UART_DEF_RT     0xc0 /* 接受FIFO 的空水线*/


#define UART_DEF_FIFOE_ENABLE   0x1 /* FIFO 使能*/
#define UART_DEF_FIFOE_DISABLE  0x0 /* FIFO 使能*/

#define UART_DEF_TET_NULL     0x00 /* FIFO 完全空*/
#define UART_DEF_TET_2CHAR    0x10 /* FIFO 中有2 个字符*/
#define UART_DEF_TET_ONEFOUR  0x20 /* FIFO 四分之一满*/
#define UART_DEF_TET_ONETWO   0x30 /* FIFO 二分之一满*/

#define UART_DEF_RT_1CHAR     0x00 /* FIFO 中有1 个字符*/
#define UART_DEF_RT_2CHAR     0xc0 /* FIFO 差2 个字符就满*/
#define UART_DEF_RT_ONEFOUR   0x40 /* FIFO 四分之一满*/
#define UART_DEF_RT_ONETWO    0x80 /* FIFO 二分之一满*/

/* 波特率对应宏*/
#define AMBA_UART_MAX_BAUT_RATE 921600
#define AMBA_UART_MIN_BAUT_RATE 1

struct uart_chan
{
	unsigned	base_addr;			/* uart register base address */
	unsigned	irq;				/* uart int number */
	unsigned	fifo_config;
	unsigned	line_config;
	unsigned	baudrate;
	unsigned	clk_freq;
	unsigned	mode;				/* int or poll mode */
	unsigned	highspeed;			/* is highspeed */

	unsigned   (*get_tx_char)(void* context, char* ch);		/* Tx callback */
	unsigned   (*put_rx_char)(void* context, char* ch);		/* Tx callback */
	void* get_tx_char_context;
	void* put_rx_char_context;	
};


int uart_initilize(struct uart_chan* p_uart_chan);
int uart_transmit_int_enable(struct uart_chan* p_uart_chan);
int uart_transmit_char(struct uart_chan* p_uart_chan, char* ch);


#endif  /* _ASMLANGUAGE */

 
#endif /* ___SERIAL__H__ */

