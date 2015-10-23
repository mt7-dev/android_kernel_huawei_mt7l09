/*----------------------------------------------------------------------------
 *      balongv7r2 m3 pm
 *----------------------------------------------------------------------------
 *      Name:    pm.C
 *      Purpose: RTX example program
 *----------------------------------------------------------------------------
 *      This code is part of balongv7r2 PWR.
 *---------------------------------------------------------------------------*/

#include <cmsis_os.h>
#include <product_config.h>
#include <osl_common.h>
#include <soc_memmap_m3.h>
#include <hi_base.h>
#include <hi_syssc.h>
#include <drv_ipc_enum.h>
#include <ipc_balong.h>
#include <wdt_balong.h>
#include <hisi_mem_repair.h>

__ao_func void modem_subsys_init(void)
{
    u32 tmp;

    /* modem subsystem  */
    writel(0x804, 0x40235050);
    writel(0x18000000, 0x40235064);
    writel(0x10000000, 0x40235094);

    tmp = readl(0x40231380);
    tmp &= 0xFFFFFFF7;
    writel(tmp, 0x40231380);

    do
	{
		tmp = readl(0x40231388);
		tmp = (tmp >> 3) & 0x1;
	}while(tmp);

    do
	{
		tmp = readl(0x40231384);
		tmp = (tmp >> 3) & 0x1;
	}while(tmp);

    /* sec */
	/*writel(0x0, HI_SYSCTRL_BASE_ADDR + HI_SEC_CTRL0_OFFSET);*/
	tmp = readl(HI_SYSCTRL_BASE_ADDR + HI_SEC_CTRL0_OFFSET);
	tmp = tmp & ~((u32)0x1 << 9);  /*nsec-read ok*/
	writel(tmp, HI_SYSCTRL_BASE_ADDR + HI_SEC_CTRL0_OFFSET)
}

/* a9 bbe bbp */
__ao_func void mdm_a9pll_init(void)
{
    u32 tmp = 0;

    // a9 pll: 667
    tmp = readl(HI_SYSCTRL_BASE_ADDR + HI_CRG_A9PLL_CFG1_OFFSET);
    tmp &= 0xFF000000;
    tmp |= 0x00600000;
    writel(tmp, HI_SYSCTRL_BASE_ADDR + HI_CRG_A9PLL_CFG1_OFFSET);
    writel(0x00A04505, HI_SYSCTRL_BASE_ADDR + HI_CRG_A9PLL_CFG0_OFFSET);

    do
	{
		tmp = readl(HI_SYSCTRL_BASE_ADDR + HI_CRG_A9PLL_CFG0_OFFSET);
		tmp = (tmp >> 26) & 0x1;
	}while(!tmp);

    tmp = readl(HI_SYSCTRL_BASE_ADDR + HI_CRG_A9PLL_CFG1_OFFSET);
    tmp |= (0x1<<26);
    writel(tmp, HI_SYSCTRL_BASE_ADDR + HI_CRG_A9PLL_CFG1_OFFSET);
}

__ao_func void mdm_bbepll_init(void)
{
    u32 tmp = 0;

    /* 6 7 8 bbe core :100*/
    tmp = readl(HI_LP_PERI_CRG_REG_ADDR + 0xDC);
    /* first :div [3:0] div=2 */
    tmp &= ~0xF;
    tmp |= 0xF0001;
    writel(tmp, HI_LP_PERI_CRG_REG_ADDR + 0xDC);
    /* bbe sel ccpu pll */
    tmp &= ~0x30;
    tmp |= 0x300010;
    writel(tmp, HI_LP_PERI_CRG_REG_ADDR + 0xDC);
}

__ao_func void mdm_bbppll_init(void)
{
    u32 tmp = 0;

    /* 9 10 11 bbp pll:491*/
    tmp = readl(HI_SYSCTRL_BASE_ADDR + HI_CRG_BBPPLL_CFG0_OFFSET);
    tmp &= 0xFFFFFFFC;
    tmp |= 0x1;
    writel(tmp, HI_SYSCTRL_BASE_ADDR + HI_CRG_BBPPLL_CFG0_OFFSET);

    do
    {
        tmp = readl(HI_SYSCTRL_BASE_ADDR + HI_CRG_BBPPLL_CFG0_OFFSET);
        tmp = (tmp >> 26) & 0x1;
    }while(!tmp);

    tmp = readl(HI_SYSCTRL_BASE_ADDR + HI_CRG_BBPPLL_CFG1_OFFSET);
    tmp |= (0x1<<26);
    writel(tmp, HI_SYSCTRL_BASE_ADDR + HI_CRG_BBPPLL_CFG1_OFFSET);
}

void startup_modem(void)
{
    u32 tmp = 0;

    /* 1 2 mtcmos power up and wait for complete */
    writel(0x1<<1, HI_SYSCTRL_BASE_ADDR + HI_PWR_CTRL6_OFFSET);

    do
	{
		tmp = readl(HI_SYSCTRL_BASE_ADDR + HI_PWR_STAT1_OFFSET);
		tmp = (tmp >> 1) & 0x1;
	}while(!tmp);

    mdm_a9pll_init();

    mdm_bbepll_init();

    mdm_bbppll_init();

    /* 12 a9 bbe ref_clk enable*/
    writel(0x3<<24, HI_SYSCTRL_BASE_ADDR + HI_CRG_CLKEN3_OFFSET);

    /* 13 bus div: 0,0,3*/
    writel(0x40040300, HI_SYSCTRL_BASE_ADDR + HI_CRG_CLKDIV2_OFFSET);

    /* 14 */
    writel(0x40010300, HI_SYSCTRL_BASE_ADDR + HI_CRG_CLKDIV2_OFFSET);

    /* 15 enable ap and L2 clk */
    writel(0x3<<3, HI_SYSCTRL_BASE_ADDR + HI_CRG_CLKEN4_OFFSET);
    /* 16 a9 start addr */
    //writel(MCORE_TEXT_START_ADDR >> 16,0xA8A09C18);
    /* 17 disable iso */
    writel(0x1<<1, HI_SYSCTRL_BASE_ADDR + HI_PWR_CTRL5_OFFSET);
    /* 18 */
    //writel(0x1<<15, HI_SYSCTRL_BASE_ADDR + HI_CRG_SRSTDIS1_OFFSET);
    //writel(0x1<<6, HI_SYSCTRL_BASE_ADDR + HI_CRG_SRSTDIS1_OFFSET);

}

void modem_start_init(void)
{
    modem_subsys_init();
    hisi_ip_mem_repair(MRB_MODEM);
    startup_modem();
}

static void ccpu_start(void)
{
    /* 16 a9 start addr */
#ifdef ROM_COMPRESS
    writel(MCORE_TEXT_START_ADDR_COMPRESSED >> 16,0xA8A09C18);
#else
    writel(MCORE_TEXT_START_ADDR >> 16,0xA8A09C18);
#endif

    /* 18 */
    writel(0x1<<15, HI_SYSCTRL_BASE_ADDR + HI_CRG_SRSTDIS1_OFFSET);
    writel(0x1<<6, HI_SYSCTRL_BASE_ADDR + HI_CRG_SRSTDIS1_OFFSET);
}

static void IntIpcCcpuStartHandler(u32 irq)
{
    int irqlock = 0;

    local_irq_save(irqlock);
    printk("CcpuStart irq=%d\n", get_irq());
    bsp_wdt_enable();
    ccpu_start();
    local_irq_restore(irqlock);
}

void ccpu_start_init(void)
{
	if(bsp_ipc_int_connect(IPC_MCU_INT_SRC_CCPU_START, IntIpcCcpuStartHandler, 0) != OK)
	{
		printk("ipc connect ccpu err\n");
	}

	if(bsp_ipc_int_enable(IPC_MCU_INT_SRC_CCPU_START) != OK)
	{
		printk("ipc enable ccpu err\n");
	}
}


