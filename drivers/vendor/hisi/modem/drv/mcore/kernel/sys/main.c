/*----------------------------------------------------------------------------
 *      RL-ARM - RTX
 *----------------------------------------------------------------------------
 *      Name:    RTX_EX1.C
 *      Purpose: Your First RTX example program
 *----------------------------------------------------------------------------
 *
 * Copyright (c) 1999-2009 KEIL, 2009-2013 ARM Germany GmbH
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  - Neither the name of ARM  nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *---------------------------------------------------------------------------*/
#include "product_config.h"
#include <ARMCM3.h>
#include <bsp_ipc.h>
#include "console.h"
#include "printk.h"
#include <icc_balong.h>
#include "m3_pm.h"
#include "m3_cpufreq.h"
#include "pm_api.h"
#include "tsensor_balong.h"
#include "om_balong.h"
#include <bsp_dsp.h>
#include <bsp_hkadc.h>
#include <bsp_nvim.h>
#include <bsp_softtimer.h>
#include "temperature.h"
#include <wdt_balong.h>
#include "ccpu_start.h"
#include <om_balong.h>
#include <socp_balong.h>
#include <m3_dump.h>
#include <bsp_lowpower_mntn.h>

u32 sys_initial = 0;
/*----------------------------------------------------------------------------
 *   Main Thread
 *---------------------------------------------------------------------------*/
int main (void) {
	console_init();

	bsp_ipc_init();

	bsp_icc_init();
	(void)bsp_softtimer_init();
	/* must after ipc icc init */
	pm_init();
	cpufreq_init();
    ccpu_start_init();

    bsp_nvm_init();

    tsensor_init();

    bsp_dsp_init();

    bsp_hkadc_init();

#ifdef CONFIG_TEMPERATURE_PROTECT
    tem_protect_init();
#endif

	socp_m3_init();

	om_log_init();

    /*bsp_rfile_init();*/

	hi6930_wdt_init();
        m3_dump_init();
#ifdef CONFIG_PWC_MNTN_CCORE
	bsp_lowpower_mntn_init();
#endif
	sys_initial = 1;
	printk("\nsys: init ok\n");
	return 0;
}

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
