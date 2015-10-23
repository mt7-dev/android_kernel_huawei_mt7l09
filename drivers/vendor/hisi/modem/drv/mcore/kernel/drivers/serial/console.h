
#ifndef __M_CONSOLE_H__
#define __M_CONSOLE_H__

#include <cmsis_os.h>
#include "uart.h"
#include "ring_buffer.h"


struct simple_console {
	struct uart_chan dev;
	osThreadId cmd_process_tid;
	osSemaphoreId cmd_process_sem;
	DECLARE_S_RING_BUFFER(send_buff, 1024);
	DECLARE_S_RING_BUFFER(recv_buff, 128);
	int echo_enabled;
	int enabled;
};

int console_init(void);

int console_xput(char ch, void *cookie);

void console_flush(void);

const struct uart_chan * get_uartchan(void);

#endif
