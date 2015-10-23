/* sysSerial.c - ARM Integrator board serial device initialization */

/* Copyright 1999-2000 ARM Limited */
/*
 * Copyright (c) 1999, 2000, 2005, 2007 Wind River Systems, Inc.
 *
 * The right to copy, distribute or otherwise make use of this software
 * may be licensed only pursuant to the terms of an applicable Wind River
 * license agreement.
 */

/*******************************************************************/
/*                                              头文件                                              */
/*******************************************************************/
#include <vxWorks.h>
#include <iv.h>
#include <intLib.h>
#include "errno.h"
#include <sysLib.h>
#include "product_config.h"
#include "config.h"
#include "osl_types.h"
#include "soc_interrupts.h"
#include "soc_clk.h"
#include "balongv7r2_uart.h"
#include <bsp_sram.h>
#include <hi_uart.h>

/* 引不到vxworks系统的定义 */
/*lint --e{19, 26, 40, 64, 516, 551 } */
extern FUNCPTR _func_consoleOut; 

AMBA_UART_CHAN * get_amba_chan(void);
/*******************************************************************/
/*                                              宏定义                                              */
/*******************************************************************/
#define UART_XTAL_FREQ              HI_UART_CLK
#define AMBA_UART_CHANNELS_NUM      1


/*******************************************************************/
/*                                              变量定义                                         */
/*******************************************************************/
typedef struct
{
    u32	    vector;
    u32 *	base_addrs;
    u32	    int_level;
} SYS_AMBA_CHAN_PARAS;


/* Local data structures */

static SYS_AMBA_CHAN_PARAS dev_paras[] =
{
	{CCORE_SYS_UART_INTID, (u32*)CCORE_SYS_UART_BASE, CCORE_SYS_UART_INTID}
};

/* Add for USB ACM */
static AMBA_UART_CHAN amba_chan[AMBA_UART_CHANNELS_NUM];

AMBA_UART_CHAN * get_amba_chan(void)
{
	return &amba_chan[0];
}

/*
 * Array of pointers to all serial channels configured in system.
 * See sioChanGet(). It is this array that maps channel pointers
 * to standard device names.  The first entry will become "/tyCo/0",
 * the second "/tyCo/1", and so forth.
 */

static SIO_CHAN * sysSioChans [] =
{
    &amba_chan[0].sio/* /tyCo/0 */
};


/* forward declarations */

#ifdef	INCLUDE_SIO_POLL
static int sysSerialPollConsoleOut (int arg, char *buf, int len);
#endif	/* INCLUDE_SIO_POLL */

/******************************************************************************
*
* sysSerialHwInit - initialize the BSP serial devices to a quiescent state
*
* This routine initializes the BSP serial device descriptors and puts the
* devices in a quiescent state.  It is called from sysHwInit() with
* interrupts locked.
*
* RETURNS: N/A
*
* SEE ALSO: sysHwInit()
*/

void sysSerialHwInit (void)
{
    int i;

#ifndef BSP_CONFIG_HI3630
	dev_paras[0].int_level = ((SRAM_SMALL_SECTIONS * )SRAM_SMALL_SECTIONS_ADDR)->UART_INFORMATION[1].interrupt_num;
	dev_paras[0].base_addrs = (u32*)(((SRAM_SMALL_SECTIONS * )SRAM_SMALL_SECTIONS_ADDR)->UART_INFORMATION[1].base_addr);
	dev_paras[0].vector = ((SRAM_SMALL_SECTIONS * )SRAM_SMALL_SECTIONS_ADDR)->UART_INFORMATION[1].interrupt_num;
#else
    ((SRAM_SMALL_SECTIONS * )SRAM_SMALL_SECTIONS_ADDR)->UART_INFORMATION[1].interrupt_num = dev_paras[0].int_level;
    ((SRAM_SMALL_SECTIONS * )SRAM_SMALL_SECTIONS_ADDR)->UART_INFORMATION[1].base_addr = (u32)dev_paras[0].base_addrs;
#endif

    for (i = 0; i < AMBA_UART_CHANNELS_NUM; i++)
	{
	    amba_chan[i].regs       = dev_paras[i].base_addrs;
	    amba_chan[i].baud_rate  = CONSOLE_BAUD_RATE;
	    amba_chan[i].xtal       = UART_XTAL_FREQ;

	    amba_chan[i].level_rx   = dev_paras[i].int_level;
	    amba_chan[i].level_tx   = dev_paras[i].int_level;

	    /*
	      * Initialize driver functions, getTxChar, putRcvChar and channelMode
	      * and initialize UART
	      */

	    balongv7r2_uart_init(&amba_chan[i]);
	}



#ifdef	INCLUDE_SIO_POLL
    (BSP_VOID)sioIoctl (sysSioChans[SIO_POLL_CONSOLE], SIO_MODE_SET,
	      (void *) SIO_MODE_POLL);
    _func_consoleOut = (FUNCPTR)sysSerialPollConsoleOut;
#endif	/* INCLUDE_SIO_POLL */
}


/******************************************************************************
*
* sysSerialHwInit2 - connect BSP serial device interrupts
*
* This routine connects the BSP serial device interrupts.  It is called from
* sysHwInit2().  Serial device interrupts could not be connected in
* sysSerialHwInit() because the kernel memory allocator was not initialized
* at that point, and intConnect() may call malloc().
*
* RETURNS: N/A
*
* SEE ALSO: sysHwInit2()
*/

void sysSerialHwInit2 (void)
{
    int i;
	for (i = 0; i < AMBA_UART_CHANNELS_NUM; i++)
	{
		(void)intConnect (INUM_TO_IVEC(dev_paras[i].vector),
				   balongv7r2_uart_irq_handler, (int) &amba_chan[i] );
		(void)intEnable ((int)dev_paras[i].int_level);
    }
}

/******************************************************************************
*
* sysSerialChanGet - get the SIO_CHAN device associated with a serial channel
*
* This routine returns a pointer to the SIO_CHAN device associated with
* a specified serial channel.  It is called by usrRoot() to obtain
* pointers when creating the system serial devices '/tyCo/x'.  It is also
* used by the WDB agent to locate its serial channel.
*
* RETURNS: A pointer to the SIO_CHAN structure for the channel, or ERROR
* if the channel is invalid.
*/

SIO_CHAN * sysSerialChanGet(int channel)
{
    if (channel < 0 || channel >= (int)(NELEMENTS(sysSioChans)))
	{
	    return (SIO_CHAN *)-1;
    }
    else
    {
        return (SIO_CHAN*)&amba_chan[channel];
    }
}

/******************************************************************************
*
* sysSerialReset - reset the sio devices to a quiet state
*
* Reset all devices to prevent them from generating interrupts.
*
* This is called from sysToMonitor to shutdown the system gracefully before
* transferring to the boot ROMs.
*
* RETURNS: N/A
*/

void sysSerialReset (void)
{
    int i;

    for (i = 0; i < AMBA_UART_CHANNELS_NUM; i++)
	{
	    /* disable serial interrupts */
	    (void)intDisable ((int)dev_paras[i].int_level);
	}
}

#ifdef	INCLUDE_SIO_POLL
/******************************************************************************
*
* sysSerialPollConsoleOut - poll out routine
*
* This function prints by polling.
*
* RETURNS: bytes sent to console
*/

static int sysSerialPollConsoleOut(int arg, char * buf, int len)
{
    char c;
    int  bytesOut = 0;

    if ((len <= 0) || (buf == NULL))
    {
	    return 0;
    }

    while ((bytesOut < len) && ((c = *buf++) != EOS))
	{
	    while (sioPollOutput (sysSioChans[SIO_POLL_CONSOLE], c) == EAGAIN);
	    bytesOut++;

	    if (c == '\n')
        {
	        while (sioPollOutput (sysSioChans[SIO_POLL_CONSOLE], '\r') == EAGAIN);
        }
	}

    return (bytesOut);
}/*lint !e715 !e818*/
#endif	/* INCLUDE_SIO_POLL */

