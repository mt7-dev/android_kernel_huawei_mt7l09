/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <boot/boot.h>
#include <boot/flash.h>
#include <boot/board.h>
#include <bootimg.h>
#include <serial.h>
#include <config.h>
#include <balongv7r2/types.h>

#define VERSION "0.9"

#define REQUIRE_SIGNATURE 0

static unsigned uart_rx_length;

static char *uartload_cmdbuf;
static int uartload_cmdlen = 0;
static char *uartload_databuf;
static int uartload_datalen = 0;

static int uartload_mode = 0; /* 0:cmd   1:data */

extern unsigned hex2unsigned(char *x);
extern void num_to_hex8(unsigned n, char *out);
void uartload_poll(void);
void uartload_tx(unsigned ch);
extern char* strncpy(char *dest, const char *src, unsigned int count);
void uartload_chengmode(int mode);
extern void boot_unregister_poll_func(void (*func)(void));
extern int boot_linux_from_flash(void);
extern int boot_vxworks_from_flash(void);

static void uart_tx_status(const char *status)
{
    int i = 0;
    int len = strlen(status);
	for(i = 0; i < len; i++)
	{
		uartload_tx((unsigned) status[i]);
	}
}




static void uartload_rx_data_complete()
{
    uart_tx_status("OKAY\n");
    uartload_chengmode(0);
}

static void uartload_rx_cmd_complete()
{
    if(memcmp(uartload_cmdbuf, "reboot", 6) == 0) {
        uart_tx_status("OKAY\n");
        uartload_chengmode(0);
        mdelay(100);
        board_reboot();
    }

    if(memcmp(uartload_cmdbuf, "download:", 9) == 0) {
        char status[16];
        uart_rx_length = hex2unsigned(uartload_cmdbuf + 9);
        if (uart_rx_length > CFG_USB_DOWNLOAD_SIZE) {
            uart_tx_status("FAILdata too large\n");
			uartload_chengmode(0);
            return;
        }
        dprintf("recv data addr=%x size=%x\n", uartload_cmdbuf, uart_rx_length);
        strncpy(status,"DATA", 4);
        num_to_hex8(uart_rx_length, status + 4);
		status[12] = '\n';
		status[13] = 0;
		dprintf("%s \n", status);
        uart_tx_status(status);
        uartload_chengmode(1);
        return;
    }

    if(memcmp(uartload_cmdbuf, "erase:", 6) == 0){
        struct ptentry *ptn;
        ptn = flash_find_ptn(uartload_cmdbuf + 6);
        if(ptn == 0) {
            uart_tx_status("FAILpartition does not exist\n");
            uartload_chengmode(0);
            return;
        }
        dprintf("erasing '%s'\n", ptn->name);
        if(flash_erase(ptn)) {
            uart_tx_status("FAILfailed to erase partition\n");
            uartload_chengmode(0);
            cprintf(" - FAIL\n");
            return;
        } else {
            dprintf("partition '%s' erased\n", ptn->name);
            cprintf(" - OKAY\n");
        }
        uart_tx_status("OKAY\n");
        uartload_chengmode(0);
        return;
    }

    if(memcmp(uartload_cmdbuf, "flash:", 6) == 0){
        struct ptentry *ptn;
        int extra = 0;
        ptn = flash_find_ptn(uartload_cmdbuf + 6);
        if(uart_rx_length == 0) {
            uart_tx_status("FAILno image downloaded\n");
            uartload_chengmode(0);
            return;
        }
        if(ptn == 0) {
            uart_tx_status("FAILpartition does not exist\n");
            uartload_chengmode(0);
            return;
        }
        if(!strcmp(ptn->name,"boot") || !strcmp(ptn->name,"recovery")) {
            if(memcmp((void*) uartload_databuf, BOOT_MAGIC, BOOT_MAGIC_SIZE)) {
                uart_tx_status("FAILimage is not a boot image\n");
                uartload_chengmode(0);
                return;
            }
        }
        if(ptn->property & DATA_YAFFS) {
            extra = 16;
        } else {
            uart_rx_length = (uart_rx_length + 2047) & (~2047);
        }
        dprintf("writing 0x%x  to '%s'\n",
                ptn->start, ptn->name);
        cprintf("writing '%s' (%d bytes)", ptn->name, uart_rx_length);
        if(flash_write(ptn, extra, (void*) uartload_databuf, uart_rx_length)) {
            uart_tx_status("FAILflash write failure\n");
            uartload_chengmode(0);
            cprintf(" - FAIL\n");
            return;
        } else {
            dprintf("partition '%s' updated\n", ptn->name);
            cprintf(" - OKAY\n");
        }
        uart_tx_status("OKAY\n");
        uartload_chengmode(0);
        return;
    }
	else if(memcmp(uartload_cmdbuf, "oem:", strlen("oem:")) == 0) {
				char *cmd = uartload_cmdbuf;
				cmd += strlen("oem:");

				if (memcmp(uartload_cmdbuf + 4, "boot", 4) == 0) {
                	cprintf("Try to boot kernel from flash...\n");
            		uart_tx_status("OKAY\n");
					boot_unregister_poll_func(uartload_poll);
					if(boot_linux_from_flash()) {
						boot_register_poll_func(uartload_poll);
			            uart_tx_status("FAILinvalid boot image\n");
			            uartload_chengmode(0);
			            return;
			        }
				}
				else if(memcmp(uartload_cmdbuf + 4, "mboot", 5) == 0) {
                	cprintf("Try to boot vxworks from flash...\n");
            		uart_tx_status("OKAY\n");
					boot_unregister_poll_func(uartload_poll);
					if(boot_vxworks_from_flash()) {
						boot_register_poll_func(uartload_poll);
			            uart_tx_status("FAILinvalid vxworks image");
			            uartload_chengmode(0);
			            return;
			        }
				}

		}

    uart_tx_status("FAILinvalid command\n");
    uartload_chengmode(0);
}

void uartload_tx(unsigned ch)
{

    /* Wait until there is space in the FIFO */
    while ((readl(UARTLOAD_BASE + UART_USR) & 0x02) == 0)
    {};

    /* Send the character */
    writel(ch, UARTLOAD_BASE + UART_THR);
}

int uartload_rx(void)
{
	if(!(readl(UARTLOAD_BASE + UART_LSR) & 0x01))
		return -1;
	return readl(UARTLOAD_BASE + UART_THR);
}

void uartload_chengmode(int mode)
{
	uartload_mode = mode;
	if(mode == 0)
	{
		uartload_cmdlen = 0;
	}
	else if(mode == 1)
	{
		uartload_datalen = 0;
	}
}

void uart_rx_cmd(void)
{
	int ch = 0;
	int i = 100;
	while(i--)
	{
		ch = uartload_rx();
		if (ch == -1)
			continue;

		if ((ch == '\r')|| (ch == '\n')) {
			uartload_cmdbuf[uartload_cmdlen] = 0;
			cprintf("%s\n", uartload_cmdbuf);
			uartload_rx_cmd_complete(uartload_cmdbuf);
			break;
		}
		else{
			uartload_cmdbuf[uartload_cmdlen++] = ch;
		}
	}
}

void uart_rx_data(void)
{
	int ch = 0;
	int i = 10000;

	while(i)
	{
		ch = uartload_rx();
		if (ch == -1)
		{
			i--;
			continue;
		}

		uartload_databuf[uartload_datalen++] = ch;

		if (uartload_datalen == (int)uart_rx_length) {
			uartload_rx_data_complete(uartload_databuf);
			break;
		}
	}

}


void uartload_poll(void)
{
	if(uartload_mode == 0)
	{
		uart_rx_cmd();
	}
	else
	{
		uart_rx_data();
	}
}


void uartload_init(void)
{
    //config FIFO:enable FIFO,receive level=00b,send level=00b,reset xfifo and rfifo
    writel(0x07, UARTLOAD_BASE+UART_FCR);

    //config UART format:data--8bit stop--1bit parity--disable
    writel(0x03, UARTLOAD_BASE+UART_LCR);

    //enable access DLL and DLH
    writel(readl(UARTLOAD_BASE+UART_LCR) | (1 << 0x07), UARTLOAD_BASE+UART_LCR);

    writel((CONFIG_PRIMCELL_CLOCK/(16*CONFIG_BAUDRATE)), UARTLOAD_BASE+UART_DLL);
    writel((CONFIG_PRIMCELL_CLOCK/(16*CONFIG_BAUDRATE))>>8, UARTLOAD_BASE+UART_DLH);

    //disable access DLL and DLH
    writel(readl(UARTLOAD_BASE+UART_LCR) & ~(1 << 0x07), UARTLOAD_BASE+UART_LCR);

    //disable interrupt
    writel(0x0, UARTLOAD_BASE+UART_IER);

	uartload_cmdbuf = (char*)CFG_UART_DOWNLOAD_ADDR;
	uartload_databuf = (char*)(CFG_UART_DOWNLOAD_ADDR + 0x40);

    boot_register_poll_func(uartload_poll);
}
