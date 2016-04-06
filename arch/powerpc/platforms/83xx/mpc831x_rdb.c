

#include <linux/pci.h>
#include <linux/of_platform.h>

#include <asm/time.h>
#include <asm/ipic.h>
#include <asm/udbg.h>
#include <sysdev/fsl_pci.h>

#include "mpc83xx.h"

/*
 * Setup the architecture
 */
static void __init mpc831x_rdb_setup_arch(void)
{
	if (ppc_md.progress)
		ppc_md.progress("mpc831x_rdb_setup_arch()", 0);

	mpc83xx_setup_pci();
	mpc831x_usb_cfg();
}

static const char *board[] __initdata = {
	"MPC8313ERDB",
	"fsl,mpc8315erdb",
	NULL
};

/*
 * Called very early, MMU is off, device-tree isn't unflattened
 */
static int __init mpc831x_rdb_probe(void)
{
	return of_flat_dt_match(of_get_flat_dt_root(), board);
}

machine_device_initcall(mpc831x_rdb, mpc83xx_declare_of_platform_devices);

define_machine(mpc831x_rdb) {
	.name			= "MPC831x RDB",
	.probe			= mpc831x_rdb_probe,
	.setup_arch		= mpc831x_rdb_setup_arch,
	.init_IRQ		= mpc83xx_ipic_init_IRQ,
	.get_irq		= ipic_get_irq,
	.restart		= mpc83xx_restart,
	.time_init		= mpc83xx_time_init,
	.calibrate_decr		= generic_calibrate_decr,
	.progress		= udbg_progress,
};
