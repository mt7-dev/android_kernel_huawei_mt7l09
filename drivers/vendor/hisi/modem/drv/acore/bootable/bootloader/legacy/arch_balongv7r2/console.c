#include <boot/boot.h>
#include <balongv7r2/config.h>
#include <balongv7r2/parameters.h>
#include <balongv7r2/serial.h>
#include "hi_uart.h"
#include "../nandwrite/nandwrite.h"
#include "time.h"
#include <bsp_nandc.h>
#include <soc_interrupts_m3.h>
#include "ios_list.h"
#include "efuse.h"
#include "i2c.h"
#include "tsensor_balong.h"
#include <bsp_version.h>
#define MAX_CONSOLE_BUF 512
/*lint --e{746}*/
static char console_buf[MAX_CONSOLE_BUF]={0};
static unsigned console_buf_loc;

extern int boot_linux_from_flash(void);
extern int boot_vxworks_from_flash(void);
extern int boot_recovery_from_flash(void);
extern int boot_rtx_cm3_from_flash(void);
extern void flash_erase_force(char *partition_name);
extern void flash_scan_bad(char *partition_name);
extern void bsp_nand_markbad(char *blockid);
extern void cstest(void);
extern void astest(void);
extern void board_reboot(void);
extern void bsp_show_nand_info(void);
extern void pcie_fastboot_console(char *cmd);

extern unsigned BOOTLOADER_BSS;
extern unsigned BOOTLOADER_BSS_END;

extern unsigned BOOTLOADER_START;
extern unsigned BOOTLOADER_SIZE;

extern unsigned BOOTLOADER_HEAP_ADDR;

void uart_sele_set(char c)
{
	SRAM_SMALL_SECTIONS * sram_mem = ((SRAM_SMALL_SECTIONS * )SRAM_SMALL_SECTIONS_ADDR);/*lint !e958 */
	switch(c)
	{
	case 'k':
	case 'K':
		*(u32*)SHM_MEM_AT_FLAG_ADDR = 0;
		break;
	case 'l':
	case 'L':
		/*A CORE*/
		sram_mem->UART_INFORMATION[0].flag = UART_USE_FLAG;
		sram_mem->UART_INFORMATION[0].base_addr = HI_UART0_REGBASE_ADDR;
		sram_mem->UART_INFORMATION[0].interrupt_num = INT_LVL_UART0;
		/*C CORE*/
		sram_mem->UART_INFORMATION[1].flag = 0;
		sram_mem->UART_INFORMATION[1].base_addr = HI_UART1_REGBASE_ADDR;
		sram_mem->UART_INFORMATION[1].interrupt_num = INT_LVL_UART1;
		/*M CORE*/
		sram_mem->UART_INFORMATION[2].flag = 0;
		sram_mem->UART_INFORMATION[2].base_addr = HI_UART2_REGBASE_ADDR;
		sram_mem->UART_INFORMATION[2].interrupt_num = M3_UART2_INT;
		/*shell*/
		if(HW_VER_PRODUCT_E5379 == bsp_version_get_board_chip_type())
		{
			sram_mem->SRAM_USB_ASHELL = 0;
			writel(PRT_FLAG_EN_MAGIC_M, SHM_MEM_CHSELL_FLAG_ADDR);
		}
		break;
	case 'v':
	case 'V':
		/*A CORE*/
		sram_mem->UART_INFORMATION[0].flag = 0;
		sram_mem->UART_INFORMATION[0].base_addr = HI_UART1_REGBASE_ADDR;
		sram_mem->UART_INFORMATION[0].interrupt_num = INT_LVL_UART1;
		/*C CORE*/
		sram_mem->UART_INFORMATION[1].flag = UART_USE_FLAG;
		sram_mem->UART_INFORMATION[1].base_addr = HI_UART0_REGBASE_ADDR;
		sram_mem->UART_INFORMATION[1].interrupt_num = INT_LVL_UART0;
		/*M CORE*/
		sram_mem->UART_INFORMATION[2].flag = 0;
		sram_mem->UART_INFORMATION[2].base_addr = HI_UART2_REGBASE_ADDR;
		sram_mem->UART_INFORMATION[2].interrupt_num = M3_UART2_INT;
		/*shell*/
		if(HW_VER_PRODUCT_E5379 == bsp_version_get_board_chip_type())
		{
			sram_mem->SRAM_USB_ASHELL = PRT_FLAG_EN_MAGIC_A;
			writel(0, SHM_MEM_CHSELL_FLAG_ADDR);
		}
		break;
	case 'm':
	case 'M':
		/*A CORE*/
		sram_mem->UART_INFORMATION[0].flag = 0;
		sram_mem->UART_INFORMATION[0].base_addr = HI_UART1_REGBASE_ADDR;
		sram_mem->UART_INFORMATION[0].interrupt_num = INT_LVL_UART1;
		/*C CORE*/
		sram_mem->UART_INFORMATION[1].flag = 0;
		sram_mem->UART_INFORMATION[1].base_addr = HI_UART2_REGBASE_ADDR;
		sram_mem->UART_INFORMATION[1].interrupt_num = INT_LVL_UART2;
		/*M CORE*/
		sram_mem->UART_INFORMATION[2].flag = UART_USE_FLAG;
		sram_mem->UART_INFORMATION[2].base_addr = HI_UART0_REGBASE_ADDR;
		sram_mem->UART_INFORMATION[2].interrupt_num = M3_UART0_INT;
		/*shell*/
		if(HW_VER_PRODUCT_E5379 == bsp_version_get_board_chip_type())
		{
			sram_mem->SRAM_USB_ASHELL = PRT_FLAG_EN_MAGIC_A;
			writel(PRT_FLAG_EN_MAGIC_M, SHM_MEM_CHSELL_FLAG_ADDR);
		}
#ifdef CONFIG_FASTBOOT_UART_NUM
#if(CONFIG_FASTBOOT_UART_NUM == 0)
		//uart_getchar = uart_getc_do_nothing;
		uart_putchar = uart_putc_do_nothing;
#endif
#endif
		break;
	default:
		break;
	}
}

void console_rx_cmd_complete(char* cmd)
{
	/*lint --e{718} */
	if(memcmp(cmd, "boot", 4) == 0){
        boot_rtx_cm3_from_flash();
		boot_linux_from_flash();
	}
    else if(memcmp(cmd, "showflash", 9) == 0){
		bsp_show_nand_info();
	}
    else if(memcmp(cmd, "flash_erase_force", 17) == 0){
		flash_erase_force(cmd + 18);
	}
    else if(memcmp(cmd, "flash_scan_bad", 14) == 0){
		flash_scan_bad(cmd + 14);
	}
    else if(memcmp(cmd, "nand_markbad", 12) == 0){
		bsp_nand_markbad(cmd + 13);
	}
    else if(memcmp(cmd, "nand_erase_force", 16) == 0){
		bsp_nand_erase_force(cmd + 17);
	}
	else if(memcmp(cmd, "io", 2) == 0){
		ios_list_echo();
	}
	else if(memcmp(cmd, "jtag1", 5) == 0){
		jtag1_config();
	}
	else if(memcmp(cmd, "chiptem", 7) == 0){
        cprintf("chip temperature is %d.\n ", chip_tem_get());
	}    
	else if(memcmp(cmd, "efuse", 5) == 0){
		efuse_info_show();
	}
	else if(memcmp(cmd, "cboot", 5) == 0){
		/* coverity[check_return] */
		boot_vxworks_from_flash();
	}
	else if(memcmp(cmd, "mboot", 5) == 0){
		boot_rtx_cm3_from_flash();
	}
	else if(memcmp(cmd, "mem", 3) == 0){
		cprintf("    section size:  start          size  \n");
		cprintf("           first: 0x%x    %d\n",&BOOTLOADER_START, &BOOTLOADER_SIZE);

		cprintf("    bss:0x%x -- 0x%x, %d\n",&BOOTLOADER_BSS, &BOOTLOADER_BSS_END
			, (unsigned)(&BOOTLOADER_BSS_END- &BOOTLOADER_BSS));

		cprintf("    heap:0x%x -- 0x%x, %d\n",BOOTLOADER_HEAP_ADDR, get_heap_next()
			, (unsigned)(get_heap_next()- BOOTLOADER_HEAP_ADDR));
	}
	else if(memcmp(cmd, "help", 4) == 0){
		cprintf("    help        print the list.\n");
		cprintf("    boot        boot kernel from flash.\n");
		cprintf("    boot cshell boot with cshell.\n");
		cprintf("    mem         show memory statis.\n");
	}
    else if(memcmp(cmd, "recovery", 8) == 0)
    {
        boot_recovery_from_flash();
    }
	else if(memcmp(cmd, "flash", 5) == 0)
    {
        flash_main();
    }
	else if(memcmp(cmd, "reboot", 6) == 0)
    {
        board_reboot();
    }
	else if(memcmp(cmd, "cstest", 6) == 0)
    {
        cstest();
    }
	else if(memcmp(cmd, "astest", 6) == 0)
    {
        astest();
    }
	else if(memcmp(cmd, "i2cdebug", 8) == 0)
	{
		bsp_i2c_debug();
	}
    else if(memcmp(cmd, "pcie", 4) == 0)
	{
		pcie_fastboot_console(cmd + 5);
	}
	else if((0 == memcmp(cmd, "m", 1)) ||
		(0 == memcmp(cmd, "M", 1)) ||
		(0 == memcmp(cmd, "v", 1)) ||
		(0 == memcmp(cmd, "V", 1)) ||
		(0 == memcmp(cmd, "l", 1)) ||
		(0 == memcmp(cmd, "L", 1)) ||
		(0 == memcmp(cmd, "k", 1)) ||
		(0 == memcmp(cmd, "K", 1)))
	{
		uart_sele_set(cmd[0]);
	}
	else if(*cmd == '\r' || *cmd == '\n'){
	}
	else
	{
		cprintf("    unknown command %s.\n", cmd);
	}
	cprintf("/ # ");
}

void console_flush(void)
{
}

void console_poll_for_single_char()
{
	int ch = 0;

	if(!uart_getchar)
		return;
	ch = uart_getchar();
	if (ch == -1)
		return;

	console_buf[console_buf_loc++] = (char)ch;
	console_buf[console_buf_loc] = 0;
	console_rx_cmd_complete(console_buf);
	console_buf_loc = 0;
	console_buf[console_buf_loc] = 0;
}

void console_poll()
{
	int ch = 0;

	if(!uart_getchar)
		return;
	ch = uart_getchar();
	if (ch == -1)
		return;

	console_putc((unsigned int)ch);

	if (console_buf_loc + 1 >= MAX_CONSOLE_BUF
		|| ch == '\r'
		|| ch == '\n') {
		console_buf[console_buf_loc++] = (char)ch;
		console_buf[console_buf_loc] = 0;
		console_putc('\n');
		console_rx_cmd_complete(console_buf);
		console_buf_loc = 0;
		console_buf[console_buf_loc] = 0;
	}
	else {
		console_buf[console_buf_loc++] = (char)ch;
	}
}

void console_init(void)
{
	memset(console_buf, 0, sizeof(console_buf));
	console_buf_loc = 0;

#ifndef BSP_CONFIG_EDA
	cprintf("\n\n\n");
	cprintf("********************************************************* \n");
	cprintf("FASTBOOT simple console, enter 'help' for commands help.\n");
	cprintf("********************************************************* \n");
#endif
	boot_register_poll_func(console_poll);
}
