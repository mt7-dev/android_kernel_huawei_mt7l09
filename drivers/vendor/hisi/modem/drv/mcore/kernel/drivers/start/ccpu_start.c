/*----------------------------------------------------------------------------
 *      balongv7r2 m3 pm
 *----------------------------------------------------------------------------
 *      Name:    pm.C
 *      Purpose: RTX example program
 *----------------------------------------------------------------------------
 *      This code is part of balongv7r2 PWR.
 *---------------------------------------------------------------------------*/

#include <ARMCM3.h>
#include "console.h"
#include "printk.h"

#include "osl_types.h"
#include "osl_bio.h"
#include "osl_irq.h"

#include "m3_pm.h"
#include "drv_ipc_enum.h"
#include "ipc_balong.h"
#include "hi_common.h"
#include "product_config.h"

static void ccpu_start(void)
{
    hi_ccore_reset();
#ifdef ROM_COMPRESS
    hi_ccore_set_entry_addr(MCORE_TEXT_START_ADDR_COMPRESSED);
#else
    hi_ccore_set_entry_addr(MCORE_TEXT_START_ADDR);
#endif
    hi_ccore_active();
}

static void IntIpcCcpuStartHandler(u32 irq)
{
    printk("IntIpcCcpuStartHandler irq = %d\n", get_irq());
    ccpu_start();
    pm_vote(PM_PWR_VOTE_CCORE, 1);
}

void ccpu_start_init(void)
{
	if(bsp_ipc_int_connect(IPC_MCU_INT_SRC_CCPU_START, IntIpcCcpuStartHandler, 0) != OK)
	{
		printk("\r\npm_ipc_init connect acpu error\n");
	}

	if(bsp_ipc_int_enable(IPC_MCU_INT_SRC_CCPU_START) != OK)
	{
		printk("\r\npm_ipc_init enable acpu error\n");
	}
}




