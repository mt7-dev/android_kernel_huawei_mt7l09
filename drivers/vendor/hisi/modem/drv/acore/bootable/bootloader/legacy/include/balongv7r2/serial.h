
#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <product_config.h>

/**
 * UART module register define
 */
#define UART_RBR   0x0
#define UART_THR   0x0
#define UART_DLL   0x0
#define UART_IER   0x4
#define UART_DLH   0x4
#define UART_IIR   0x8
#define UART_FCR   0x8
#define UART_LCR   0xC
#define UART_MCR   0x10
#define UART_LSR   0x14
#define UART_FAR   0x70
#define UART_TFR   0x74
#define UART_RFW   0x78
#define UART_USR   0x7C
#define UART_TFL   0x80
#define UART_RFL   0x84
#define UART_HTX   0xA4
#define UART_DMASA 0xA8
#define UART_CPR   0xF4


#define AMBA_UART_MAX_BAUT_RATE 115200
#define AMBA_UART_MIN_BAUT_RATE 1


#define CONFIG_PRIMCELL_CLOCK   48000000
#define CONFIG_BAUDRATE         115200
#define ARM_FREQ                400
#define UART_DELAY_1S           (ARM_FREQ*1000*1000)
#define UART0_BASE      0x90007000  /* UART 0 */
#define UART1_BASE      0x90016000  /* UART 1 */
#define UART2_BASE      0x90017000  /* UART 2 */
#define UART3_BASE      0x90018000  /* UART 3 */

#ifndef CONFIG_FASTBOOT_UART_NUM
#define UART_BASE       UART1_BASE
#else
#if (CONFIG_FASTBOOT_UART_NUM == 0)
#define UART_BASE       UART0_BASE
#elif (CONFIG_FASTBOOT_UART_NUM == 1)
#define UART_BASE       UART1_BASE
#elif (CONFIG_FASTBOOT_UART_NUM == 2)
#define UART_BASE       UART2_BASE
#elif (CONFIG_FASTBOOT_UART_NUM == 3)
#define UART_BASE       UART3_BASE
#endif
#endif
#define UARTLOAD_BASE   UART1_BASE /* for fastboot load */


int serial_init(void);
int uart_getc(void);
void uart_putc(unsigned ch);
void uart_putc_do_nothing(unsigned ch);
int uart_getc_do_nothing(void);

typedef int (*serial_get_fun)(void);
typedef void (*serial_put_fun)(unsigned ch);

extern serial_get_fun uart_getchar;
extern serial_put_fun uart_putchar;

#define b_printf cprintf
#endif
