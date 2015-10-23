
#include <product_config.h>
#include <osl_types.h>
#include <osl_bio.h>

#include <soc_memmap.h>
#include <soc_interrupts.h>
#include <bsp_sram.h>
#include <bsp_hardtimer.h>

/*lint --e{537, 718, 746, 958} */
#include <libc.h>
#include <irq.h>

#include "printk.h"
#include "symbol.h"
#include "console.h"

struct simple_console g_console = {
	.cmd_process_tid = NULL,
	.cmd_process_sem = NULL,
	INIT_S_RING_BUFFER_STRUCT(g_console, send_buff), /*lint !e64 !e65 !e665 */
	INIT_S_RING_BUFFER_STRUCT(g_console, recv_buff), /*lint !e64 !e65 !e665 */
	.echo_enabled = 1,
	.enabled = 0
	};
static void  console_cmd_process_thread(void const *argument);
osThreadDef(console_cmd_process_thread, osPriorityBelowNormal, 1, 512); /*lint !e133 */
osSemaphoreDef(cmd_process_sem);

#ifndef min
#define min(a,b)        ((a) < (b) ? (a) : (b))
#endif


/**********************************command***************************************/

typedef int (*call_ptr)(int arg1, int arg2, int arg3, int arg4, int arg5, int arg6);

struct console_call_arg {
	char func_name[50];
	int args[6]; /*lint !e958 */
};

const struct uart_chan * get_uartchan(void)
{
	return &g_console.dev;
}

extern unsigned long kallsyms_lookup_name(const char *name);
int convert_str2int(const char *str)
{
	const char *p = str;
	long val = 0;
	int ext = 0;
	int base = 0;

	while('0' == *p)
	{
		p++;
	}

	if ('x' == *p || 'X' == *p)
	{
		base = 16;
		p++;
	}
	else
	{
		base = 10;
	}

	while(*p != '\0' && *p != ' ' && *p != '\n' && *p != ',' && *p != '\r')
	{
		if (*p >= '0' && *p <= '9')
		{
			ext = -'0';
		}
		else if (*p >= 'A' && *p <= 'F')
		{
			ext = 10 - 'A';
		}
		else if (*p >= 'a' && *p <= 'f')
		{
			ext = 10 - 'a';
		}
		else
		{
			return val;
		}
		val = val * base + *p  + ext;
		p++;
	}

	return val;
}

int console_call(const char* buff)
{
	unsigned i, arg_start, arg_idx, size = 0;
	int code = 0;
	unsigned len = (unsigned)strlen(buff);
	struct console_call_arg call_arg;
	call_ptr paddr = NULL;

	memset(&call_arg, 0, sizeof(call_arg));
	for(i=0, arg_start=0, arg_idx =0; i<len; i++)
	{
		if (buff[i] == ' ' || buff[i] == '\r' || buff[i] == ',')
		{
			if(arg_idx ==0 && i>arg_start)
			{
				size = min(i-arg_start, sizeof(call_arg.func_name)-1);
				memcpy(call_arg.func_name, buff + arg_start, size);
				call_arg.func_name[size] = 0;
				arg_idx++;
			}
			else if (i>arg_start)
			{
				call_arg.args[arg_idx-1] = convert_str2int(buff + arg_start);  /*lint !e661 !e662 */
				arg_idx++;
			}
			else
			{
				;
			}
			arg_start = i+1;
		}
	}

	paddr = (call_ptr)kallsyms_lookup_name(call_arg.func_name);

	if(paddr)
	{
		if('m' == call_arg.func_name[0] && 1 == size && 1 == arg_idx)
		{
			printk("invalid argument, value = -1\n");
			return 0;
		}

		printk("exc: %s(%x, %x, %x, %x, %x, %x) \n"
			, call_arg.func_name
			, call_arg.args[0], call_arg.args[1]	, call_arg.args[2]
			, call_arg.args[3]	, call_arg.args[4]	, call_arg.args[5]
			);

		code = paddr(call_arg.args[0], call_arg.args[1], call_arg.args[2], call_arg.args[3], call_arg.args[4], call_arg.args[5]);

		printk("exc: %s = %d\n", call_arg.func_name, code);
		return 0;
	}
	return 1;
}

static void process_cmd_line(const char* buff)
{
	int error = 0;

	if (buff && buff[0] && buff[0] != '\r')
	{
		error = console_call(buff);
		if(error)
		{
			printk("    unknown command:%s\n", buff);
		}
	}
	printk("m3 # ");
}

/**********************************console***************************************/
int console_xput(char ch, void *cookie)
{
	char nm = 0, i;
	unsigned int time_cur;
	char tch = 0;

	if (ch == '\n')
		{
			s_ring_buffer_xin(&g_console.send_buff, &ch);
			tch = '[';
			s_ring_buffer_xin(&g_console.send_buff, &tch);
			time_cur = bsp_get_slice_value();
			for(i = 1; i <= 8; i++)
			{
				nm = ((time_cur >> (32 - i * 4)) & 0xF);
				tch = nm >= 10 ? nm - 10 + 'A' : nm + '0';
				s_ring_buffer_xin(&g_console.send_buff, &tch);
			}
			tch = ']';
			s_ring_buffer_xin(&g_console.send_buff, &tch);
		}
	else
		s_ring_buffer_xin(&g_console.send_buff, &ch);
	return 0;
}

void console_flush(void)
{
	if (g_console.enabled) {
		uart_transmit_int_enable(&g_console.dev);
		if (! in_interrupt() && s_ring_buffer_wl(&g_console.send_buff) && !(__get_PRIMASK()))
		{
			while(!s_ring_buffer_is_empty(&g_console.send_buff))
			{
				osThreadYield();
			}
		}
	}
}



static void  console_cmd_process_thread(void const *argument)
{
	char buff[128];
	unsigned int size = 0;
    /* coverity[no_escape] */
	for (;;)
	{
		osSemaphoreWait(g_console.cmd_process_sem, osWaitForever);/*lint !e569 */

		size = s_ring_buffer_line_size(&g_console.recv_buff);
		while(size != 0)
		{
			size = min(size, (int)sizeof(buff)-1);
			size = s_ring_buffer_out(&g_console.recv_buff, buff, size);
			buff[size] = 0;
			process_cmd_line(buff);
			size = s_ring_buffer_line_size(&g_console.recv_buff);
		}
	}
}

static unsigned console_get_tx_char(void* context, char* ch)
{
	struct s_ring_buffer* buff = (struct s_ring_buffer*)context;
	return s_ring_buffer_xout(buff, ch);
}

static unsigned console_put_rx_char(void* context, char* ch)
{
	struct s_ring_buffer* buff = (struct s_ring_buffer*)context;
	s_ring_buffer_xin(buff, ch);
	if(g_console.echo_enabled)
	{
		console_xput(*ch, 0);
		if (*ch == '\r')
			console_xput('\n', 0);
		console_flush();
	}
	if (*ch == '\r' || *ch == '\n')
	{
		osSemaphoreRelease(g_console.cmd_process_sem);
	}
	return 1;
}

int console_init(void)
{
	int status =0;

	g_console.echo_enabled = 1;
	g_console.dev.base_addr = HI_UART0_REGBASE_ADDR;
	g_console.dev.irq = M3_UART0_INT;//UART_INT_NUM;
	g_console.dev.fifo_config = 0xB7; /* 使能FIFO,复位接收与发送FIFO，接收发送水线为FIFO-2 */
	g_console.dev.line_config = 0x03;
	g_console.dev.baudrate = 115200;
	g_console.dev.clk_freq = 48000000;
	g_console.dev.mode = SIO_MODE_INT;
	g_console.dev.highspeed = 0;
	g_console.dev.get_tx_char = console_get_tx_char;
	g_console.dev.get_tx_char_context = &g_console.send_buff;
	g_console.dev.put_rx_char = console_put_rx_char;
	g_console.dev.put_rx_char_context = &g_console.recv_buff;

	g_console.dev.base_addr = ((SRAM_SMALL_SECTIONS * )SRAM_SMALL_SECTIONS_ADDR)->UART_INFORMATION[2].base_addr;
	g_console.dev.irq = ((SRAM_SMALL_SECTIONS * )SRAM_SMALL_SECTIONS_ADDR)->UART_INFORMATION[2].interrupt_num;

	status = uart_initilize(&g_console.dev);
	if (status)
	{
		printk("console_init: uart_init init error.\n");
		goto out;
	}
	g_console.enabled = 1;

	printk("Welcome to Balong MCU console, enter 'help' for commands help.\n");
	process_cmd_line(NULL);

	g_console.cmd_process_sem = osSemaphoreCreate(osSemaphore(cmd_process_sem), 1);
	if(NULL == g_console.cmd_process_sem)
	{
		status = -1;
		printk("console_init: create cmd_process_sem semaphore error.\n");
		goto out;
	}

	g_console.cmd_process_tid = osThreadCreate(osThread(console_cmd_process_thread), &g_console.recv_buff);
	if(NULL == g_console.cmd_process_tid)
	{
		status = -1;
		printk("console_init: create thread osThreadCreate  error.\n");
		goto thread_error;
	}
	return status;

thread_error:
	osSemaphoreDelete(g_console.cmd_process_sem);
out:
	return status;

}


