#include <boot/boot.h>
//#include <balongv7r2/config.h>
//#include <balongv7r2/parameters.h>
#include <balongv7r2/serial.h>
//#include <bsp_sram.h>
//#include <product_config.h>

/*lint --e{527, 572} */
serial_get_fun uart_getchar = uart_getc;
serial_put_fun uart_putchar = uart_putc;


int serial_init (void)
{
    //config FIFO:enable FIFO,receive level=00b,send level=00b,reset xfifo and rfifo
    writel(0x07, UART_BASE+UART_FCR);

    //config UART format:data--8bit stop--1bit parity--disable
    writel(0x03, UART_BASE+UART_LCR);

    //enable access DLL and DLH
    writel(readl(UART_BASE+UART_LCR) | (1UL << 0x07), UART_BASE+UART_LCR);
    
    writel((CONFIG_PRIMCELL_CLOCK/(16*CONFIG_BAUDRATE)), UART_BASE+UART_DLL);
    writel((CONFIG_PRIMCELL_CLOCK/(16*CONFIG_BAUDRATE))>>8, UART_BASE+UART_DLH);/*lint !e778 */

    //disable access DLL and DLH
    writel(readl(UART_BASE+UART_LCR) & ~(1 << 0x07), UART_BASE+UART_LCR);

    //disable interrupt
    writel(0x0, UART_BASE+UART_IER);    

    return 0;
}

void uart_putc(unsigned ch)
{
    /* Wait until there is space in the FIFO */
    while (0 == (readl(UART_BASE + UART_USR) & 0x02)); /*lint !e722 */
    /* Send the character */
    writel(ch, UART_BASE + UART_THR);
}

int uart_getc(void)
{
	if(!(readl(UART_BASE + UART_LSR) & 0x01))
		return -1;    
	return (int)readl(UART_BASE + UART_THR);
}
/*lint -save -e438*/
void uart_putc_do_nothing(unsigned ch)
{
	ch = ch;
}
/*lint -restore +e438*/
int uart_getc_do_nothing(void)
{
	return -1;
}

void serial_reset()
{
	//reset fastboot uart fifo
#if(UART_BASE != UART0_BASE)
	writel(0x6U | readl(UART_BASE + UART_FCR), UART_BASE + UART_FCR);
#endif
}