
/*lint -save -e537*/
#include <vxWorks.h>
#include <semLib.h>
#include <taskLib.h>
#include <stdio.h>
#include <string.h>
#include "osl_common.h"
#include "osl_irq.h"
#include "DrvInterface.h"
#include "bsp_dump_drv.h"
#include "bsp_memmap.h"
#include "bsp_om.h"
/*lint -restore*/

typedef void (* funcvoid)(void);
extern void cshell_register_hook(funcvoid hook);
extern void cshell_send_data(char* data, int length);

#define __LOG_BUF_LEN	        (DUMP_PRINT_SIZE)
#define DUMP_PRINT_MAGIC        (0x32324554)
#define LOG_BUF_MASK            (__LOG_BUF_LEN -1)
#define LOG_BUF(idx)            (g_dmesg_buff->log_buf[(idx) & LOG_BUF_MASK])
#ifndef WAIT_FOREVER
#define WAIT_FOREVER (-1)
#endif
/*
 * g_dmesg_lock protects log_buf, log_start, log_end, con_start and logged_chars
 * It is also used in interesting ways to provide interlocking in
 * release_console_sem().
 */
static SEM_ID g_dmesg_lock;

typedef struct {
	unsigned magic;
	unsigned log_start;	/* Index into log_buf: next char to be read by syslog() */
	unsigned log_end;	/* Index into log_buf: most-recently-written-char + 1 */
	unsigned logged_chars; /* Number of chars produced since last read+clear operation */
	unsigned log_buf_len;
	char    *log_buf;
} dmesg_buff_t;

typedef struct _dmesg_print
{
    char *buffer1;
    char *buffer2;
    int   size1;
    int   size2;
    int   poff1;
    int   poff2;
}dmesg_print;


dmesg_buff_t*  g_dmesg_buff = NULL;

// print time at newline
#if defined(CONFIG_dmesg_print_TIME)
static int printk_time = 1;
#else
static int printk_time = 0;
#endif

int new_text_line = 1;

#ifdef BSP_CONFIG_HI3630
char* dmesg_buf = (char*)(DUMP_DMESG_ADDR);
#else
char dmesg_buf[DUMP_PRINT_SIZE];
#endif

BSP_BOOL dmesg_enable = BSP_FALSE;

dmesg_print g_dmesg_print;

int  dmesg_get_char(char *out_char);
void dmesg_cshell_hook(void);
/*
 * Return the number of unread characters in the log buffer.
 */
int dmesg_get_len(void)
{
	return (int)g_dmesg_buff->logged_chars;
}

/*
 * Clears the ring-buffer
 */
void dmesg_clear(void)
{
	semTake (g_dmesg_lock, WAIT_FOREVER);

	g_dmesg_buff->logged_chars = 0;
	g_dmesg_buff->log_start = 0;
	g_dmesg_buff->log_end = 0;

	semGive (g_dmesg_lock);
}

int dmesg_clear_pbuff(int ix)
{
    if(1 == ix)
    {
        g_dmesg_print.buffer1 = NULL;
        g_dmesg_print.size1 = 0;
        g_dmesg_print.poff1 = 0;
    }
    else if(2 == ix)
    {
        g_dmesg_print.buffer2 = NULL;
        g_dmesg_print.size2 = 0;
        g_dmesg_print.poff2 = 0;
    }

    return BSP_OK;
}

/*
 * Copy a range of characters from the log buffer.
 */
int log_buf_copy(char *dest, int idx, int len)
{
	int ret, max_len;

	semTake (g_dmesg_lock, WAIT_FOREVER);

	max_len = dmesg_get_len();
	if (idx < 0 || idx >= max_len) {
		ret = -1;
	} else {
		if (len > max_len - idx)
			len = max_len - idx;
		ret = len;
		idx += ((int)g_dmesg_buff->log_end - max_len);
		while (len-- > 0)
			dest[len] = LOG_BUF(idx + len);
	}

	semGive (g_dmesg_lock);
	return ret;
}

static void emit_log_char(char c)
{
	LOG_BUF(g_dmesg_buff->log_end) = c;
	g_dmesg_buff->log_end++;

	if (g_dmesg_buff->log_end >= g_dmesg_buff->log_buf_len)
	{
    	//LOG_BUF(g_dmesg_buff->log_end -1) = 0;
		g_dmesg_buff->log_end = 0;
    }
	if (g_dmesg_buff->logged_chars < g_dmesg_buff->log_buf_len)
		g_dmesg_buff->logged_chars++;
}

/*lint -save -e958*/
static int emit_log_char_time(char c)
{
	int printed_len = 0;

	if (new_text_line)
	{
		new_text_line = 0;

		if (printk_time) {
			/* Follow the token with the time */
			char tbuf[50], *tp;
			int tlen;
			u32 t;

			t = om_timer_get();
            /* coverity[secure_coding] */
			tlen = sprintf(tbuf, "[%ums] ", t/32);

			for (tp = tbuf; tp < tbuf + tlen; tp++)
				emit_log_char(*tp);
			printed_len += tlen;
		}

	}

	emit_log_char(c);
	if (c == '\n')
		new_text_line = 1;

	return printed_len;
}
/*lint -restore*/
static int dmesg_vprint(const char *fmt, va_list args)
{
	int printed_len = 0;
	char *p;

	if(BSP_TRUE != dmesg_enable)
		return 0;

	semTake (g_dmesg_lock, WAIT_FOREVER);
    /*lint -save -e119 -e737*/
	/* Emit the output into the temporary buffer */
	printed_len += vsnprintf(dmesg_buf + printed_len, sizeof(dmesg_buf) - printed_len, fmt, args);
    /*lint -restore*/
	p = dmesg_buf;

	/*
	 * Copy the output into log_buf.  If the caller didn't provide
	 * appropriate log level tags, we insert them here
	 */
	for ( ; *p; p++)
	{
		printed_len += emit_log_char_time(*p);
	}
	semGive (g_dmesg_lock);

	return printed_len;
}

/**
 * printk - print a kernel message
 * @fmt: format string
 *
 * This is printk().  It can be called from any context.  We want it to work.
 *
 */
#if 0
int dmesg_print(const char *fmt, ...)
{
	va_list args;
	int r;

	va_start(args, fmt);
	r = dmesg_vprint(fmt, args);
	va_end(args);

	return r;
}
#endif

/*
int dmesg_read(char* buffer, const unsigned len)
{
	int i = 0;
	char c;

	semTake (g_dmesg_lock, WAIT_FOREVER);

	while ((g_dmesg_buff->log_start != g_dmesg_buff->log_end) && i < len)
	{
		c = LOG_BUF(g_dmesg_buff->log_start);
		g_dmesg_buff->log_start++;
		*buffer = c;
		buffer++;
		i++;
	}
	g_dmesg_buff->logged_chars -= i;
	semGive (g_dmesg_lock);

	return i;
}
*/
/*lint -save -e18*/
int dmesg_write(const char* buffer, const unsigned len)
{
	char* buff = (char*)buffer;

	if(BSP_TRUE != dmesg_enable)
		return 0;

    /* 中断中调用，去掉信号量操作 */
//	(void)semTake (g_dmesg_lock, WAIT_FOREVER);
	while(buff < buffer + len)
	{
		if(printk_time)
		{
			(void)emit_log_char_time(*buff);
		}
		else
		{
			(void)emit_log_char(*buff);
		}
		buff++;
	}
// 	semGive (g_dmesg_lock);
	return buff - buffer;
}
/*lint -restore +e18*/

__inline__ void dmesg_start()
{
	dmesg_enable = BSP_TRUE;
    return;
}

__inline__ void dmesg_stop()
{
	dmesg_enable = BSP_FALSE;
    return;
}

/*lint -save -e64*/
void dmesg_init(void)
{
	g_dmesg_lock = semBCreate (SEM_Q_PRIORITY, (SEM_B_STATE)SEM_FULL);

    memset(dmesg_buf, 0, DUMP_PRINT_SIZE);

	g_dmesg_buff = (dmesg_buff_t*)dmesg_buf;
	//if(DUMP_PRINT_MAGIC != g_dmesg_buff->magic
	//	|| g_dmesg_buff->log_buf_len != __LOG_BUF_LEN - sizeof(dmesg_buff_t)
	//	|| g_dmesg_buff->log_buf != (char*)g_dmesg_buff + sizeof(dmesg_buff_t))
	//{
		g_dmesg_buff->magic = DUMP_PRINT_MAGIC;
		g_dmesg_buff->log_start = 0;
		g_dmesg_buff->log_end = 0;
		g_dmesg_buff->logged_chars = 0;
		g_dmesg_buff->log_buf_len = __LOG_BUF_LEN - sizeof(dmesg_buff_t);
		g_dmesg_buff->log_buf = (char*)g_dmesg_buff + sizeof(dmesg_buff_t);
	//}

    dmesg_clear_pbuff(1);
    dmesg_clear_pbuff(2);
    balongv7r2_uart_register_hook(dmesg_get_char);
    cshell_register_hook(dmesg_cshell_hook);
	dmesg_start();
}
/*lint -restore*/


int dmesg_save(const char* buffer, const unsigned length)
{
    unsigned end = 0;
    unsigned len = 0, chars = 0;
    char* s1, *s2;
    unsigned l1, l2;

	end = g_dmesg_buff->log_end;
	chars = g_dmesg_buff->logged_chars;
    len = g_dmesg_buff->log_buf_len;

	if (chars >= len) {
		s1 = g_dmesg_buff->log_buf + end;
		l1 = len - end;

		s2 = g_dmesg_buff->log_buf;
		l2 = end; /* [false alarm]:屏蔽Fority错误 */
	}
    else {
		s1 = "";
		l1 = 0;

		s2 = g_dmesg_buff->log_buf;
		l2 = chars; /* [false alarm]:屏蔽Fority错误 */
	}

    memcpy((void *)buffer, (void *)s1, (u32)l1);
    memcpy((void *)(buffer+l1), (void *)s2, (u32)l2);

    return 0;
}

void dmesg(void)
{
    unsigned end, chars, len;
    unsigned long lock_key;

    local_irq_save(lock_key);

    if(g_dmesg_buff)
    {
    	end = g_dmesg_buff->log_end;
    	chars = g_dmesg_buff->logged_chars;
        len = g_dmesg_buff->log_buf_len;

    	if (chars >= len) {
            g_dmesg_print.poff1 = 0;
    		g_dmesg_print.buffer1 = g_dmesg_buff->log_buf + end;
    		g_dmesg_print.size1 = (int)(len - end);
            g_dmesg_print.poff2 = 0;
    		g_dmesg_print.buffer2 = g_dmesg_buff->log_buf;
    		g_dmesg_print.size2 = (int)end;
    	}
        else if(chars > 0)
        {
            dmesg_clear_pbuff(1);
            g_dmesg_print.poff2 = 0;
    		g_dmesg_print.buffer2 = g_dmesg_buff->log_buf;
    		g_dmesg_print.size2 = (int)chars;
    	}
        else
        {
            dmesg_clear_pbuff(1);
            dmesg_clear_pbuff(2);
        }
    }
    else
    {
        dmesg_clear_pbuff(1);
        dmesg_clear_pbuff(2);
    }

    local_irq_restore(lock_key);

    return;
}

int dmesg_get_char(char *out_char)
{
    if(g_dmesg_print.buffer1 && g_dmesg_print.size1)
    {
        if(g_dmesg_print.poff1 < g_dmesg_print.size1)
        {
            *out_char = g_dmesg_print.buffer1[g_dmesg_print.poff1++];
            return OK;
        }
        else
        {
            dmesg_clear_pbuff(1);
        }
    }

    if(g_dmesg_print.buffer2 && g_dmesg_print.size2)
    {
        if(g_dmesg_print.poff2 < g_dmesg_print.size2)
        {
            *out_char = g_dmesg_print.buffer2[g_dmesg_print.poff2++];
            return OK;
        }
        else
        {
            dmesg_clear_pbuff(2);
        }
    }

    return ERROR;
}

void dmesg_cshell_hook(void)
{
    if(g_dmesg_print.buffer1 && g_dmesg_print.size1)
    {
        cshell_send_data(g_dmesg_print.buffer1, g_dmesg_print.size1);
        dmesg_clear_pbuff(1);
    }

    if(g_dmesg_print.buffer2 && g_dmesg_print.size2)
    {
        cshell_send_data(g_dmesg_print.buffer2, g_dmesg_print.size2);
        dmesg_clear_pbuff(2);
    }
}


#if 0
void dmesg(void)
{
    int end, chars, len;
    char* s1, *s2;
    int l1, l2;

	end = g_dmesg_buff->log_end;
	chars = g_dmesg_buff->logged_chars;
    len = g_dmesg_buff->log_buf_len;
    printf("0x%x 0x%x 0x%x\n", end, chars, len);

	if (chars >= len) {
		s1 = g_dmesg_buff->log_buf + end;
		l1 = len - end;

		s2 = g_dmesg_buff->log_buf;
		l2 = end;
	}
    else {
		s1 = "";
		l1 = 0;

		s2 = g_dmesg_buff->log_buf;
		l2 = chars;
	}

    printf("%s", s1);
    printf("%s", s2);

    return;
}
#endif


