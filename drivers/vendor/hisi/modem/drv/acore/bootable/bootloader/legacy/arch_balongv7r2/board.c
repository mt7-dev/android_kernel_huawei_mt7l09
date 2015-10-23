#include <stdarg.h>

#include <boot/bootimg.h>
#include <boot/flash.h>
#include <boot/boot.h>
#include <boot/uart.h>
#include <balongv7r2/config.h>
#include <balongv7r2/parameters.h>
#include <balongv7r2/board.h>
#include <balongv7r2/serial.h>
#include <ios.h>
#include <balongv7r2/spi.h>
#include <bsp_pmu.h>
#include <bsp_coul.h>
#include <product_config.h>
#include <balongv7r2/lcd.h>
#include <balongv7r2/version.h>
#include <bsp_dsp.h>
#include <bsp_hkadc.h>
#include <bsp_shared_ddr.h>
#include <boot/board.h>

#include <ios_list.h>
#include <ddm_phase.h>
#include <hi_syscrg_interface.h>
#include <hi_syssc_interface.h>
#include <bsp_nvim.h>
#include <balongv7r2/i2c.h>
#include <balongv7r2/clk_fastboot.h>
#include "time.h"
#include "tsensor_balong.h"
/*
 * This holds our defaults.
 */
 /*
static struct init_tags SECTION(".args_array")
init_tags = {
	{ tag_size(tag_core), ATAG_CORE },
	{ 1, PAGE_SIZE, 0xff },
	{ tag_size(tag_mem32), ATAG_MEM },
	{ CFG_DDR_SIZE, CFG_DDR_PHYS },
	{ 0, ATAG_NONE }
};
*/

ST_MEMORY_TBL_ST pmemory_product[MEMORY_NUM] =
{
    /*name*/            /*baseaddr*/                                 /*size*/                         /* is necessary*/
    {"acore"       ,DDR_ACORE_ADDR                     ,DDR_ACORE_SIZE                  ,1},
    {"ccore"       ,DDR_MCORE_ADDR                     ,DDR_MCORE_SIZE                  ,1},
    {"share"       ,DDR_SHARED_MEM_ADDR                ,DDR_SHARED_MEM_SIZE             ,1},
    {"AXI"         ,GLOBAL_AXI_MEM_BASE_ADDR           ,HI_SRAM_MEM_SIZE                ,1},
    {"dump"        ,DDR_MNTN_ADDR_VIRT                 ,DDR_MNTN_SIZE                   ,1}
};

extern int timer_init(void);
extern void serial_reset(void);
extern int bsp_coul_init(void);

void console_putc(unsigned ch)
{
	unsigned int cur_time = 0;
	unsigned char i = 7;
	unsigned char t;

	if(uart_putchar)
	{
		uart_putchar(ch);
	}
	if(uart_putchar &&('\n' == ch))
	{
		uart_putchar('[');
		cur_time = get_elapse_ms();
		for(; i > 0; i--)
		{
			t = 0xF & (cur_time >> (4 * i));
			t = (t >= 10) ? (t - 10 + 'A') : (t + '0');
			uart_putchar(t);
		}
		uart_putchar('m');
		uart_putchar('s');
		uart_putchar(']');
	}
}
void sync_init(void)
{
	memset((void*)SHM_MEM_SYNC_ADDR,0,SHM_MEM_SYNC_SIZE);
}

void icc_shared_flag_clear(void)
{
	memset((void *)SHM_MEM_ICC_ADDR, 0, SHM_MEM_ICC_SIZE);
	memset((void *)SRAM_ICC_ADDR, 0, SRAM_ICC_SIZE);
}

extern void bsp_disable_mtcmos(void);
extern void pcie_fastboot_init(void);
#ifndef BSP_CONFIG_EDA
void board_init(void)
{
     	bsp_disable_non_on_clk();
#ifdef BSP_CONFIG_P531_FPGA
	p531_fpga_init();
#endif

	timer_init();

	serial_init();
	if(uart_putchar)
	{
		dprintf_set_putc(/*uart_putchar*/console_putc);
	}
	console_init();

    ddm_phase_boot_init();

	bsp_pmu_init();

	bsp_hkadc_init();

	bsp_save_hw_version();

	ios_list_init();
	ios_init();

	serial_reset();

    (void)flash_init();

    nv_init();

#ifdef CONFIG_DSP
    bsp_dsp_init();
#endif

#ifdef CONFIG_PCIE_CFG
    pcie_fastboot_init();
#endif

    tsensor_init();

    bsp_coul_init();

    bsp_disable_mtcmos();
#if (defined (CONFIG_FB_EMI_BALONG) || defined (CONFIG_FB_SPI_BALONG))
    ddm_phase_boot_score("start lcd_warm_up",__LINE__);
    lcd_warm_up();
    ddm_phase_boot_score("end lcd_warm_up",__LINE__);
#endif

#ifdef DRAM_TEST
	ret = dram_test();
#endif

#ifdef BSP_CONFIG_V7R2_SFT
	writel(0x01,HI_SC_REGBASE_ADDR + 0x70);
	writel(0xFFFFFFFF,HI_SC_REGBASE_ADDR + 0x7C);
#endif
	/* icc核间共享标识清零 */
	icc_shared_flag_clear();
	sync_init();
	cprintf("board_init ok\n");
}
#else
extern void bsp_abb_init(void);

void board_init(void)
{
	int ret = 0;
	timer_init();

	serial_init();
	if(uart_putchar)
	{
		dprintf_set_putc(uart_putchar);
	}
	console_init();

	ddm_phase_boot_init();

	flash_init();

	bsp_pmu_init();

    ios_list_init();;

	ios_init();

    bsp_abb_init();

}
#endif
void board_reboot(void)
{
    hi_syscrg_wdt_clk_enble();
	hi_syssc_wdt_reboot();
}

const char *board_cmdline(void)
{

	return CFG_DEF_CMDLINE;
}

unsigned board_machtype(void)
{
	return CFG_ARCH_TYPE;
}

void board_getvar(const char *name, char *value)
{
     if(!strcmp(name, "memory_num")) {
        num_to_hex8(MEMORY_NUM, value);
     }else if(!memcmp(name, "memory_info_", 12))
     {
        unsigned i = hex2unsigned((char*)name+12);
        if(i<MEMORY_NUM)
        {
            num_to_hex8(pmemory_product[i].base_addr, value);
            num_to_hex8(pmemory_product[i].size,      value+8);
            num_to_hex8(pmemory_product[i].necessary, value+16);
			/* coverity[secure_coding] */
            strcpy(value+24, pmemory_product[i].name);
        }
     }else
     {
     }
}

void keypad_init(void)
{

}

void __aeabi_idiv0(void)
{
}


