#include <stdarg.h>
#include <hi_uart.h>
#include <osl_types.h>
#include <osl_bio.h>
#include "balongv7r2_uart.h"
#include "bsp_om.h"
/*lint --e{616} */
#ifndef ERROR
#define ERROR -1
#endif
extern AMBA_UART_CHAN * get_amba_chan();
static void uart_put(unsigned ch)
{
    while ((readl((u32)((get_amba_chan())->regs) + UART_REGOFF_USR) & 0x02) == 0); /*lint !e746 !e722 */
	writel(ch, (u32)((get_amba_chan())->regs) + UART_REGOFF_THR);
    (void)dmesg_write((char *)&ch, sizeof(char));
}

static unsigned int hex2asc(int n)
{
    n &= 15;
    if(n > 9){
        return (unsigned int)(('a' - 10) + n);
    } else {
        return (unsigned int)('0' + n);
    }
}

static void xputs(const char *s, void (*xputc)(unsigned n, void *cookie), void *cookie)
{
    while (*s) {
        xputc(*s++, cookie); /*lint !e732 */
    }
}

void xprintf(const char *fmt, va_list ap,
               void (*xputc)(unsigned n, void *cookie),
               void *cookie)
{
    char scratch[16];

    for(;;){
        switch(*fmt){
        case 0:
            va_end(ap);
            return;
        case '%':
            switch(fmt[1]) {
            case 'c': {
                unsigned n = va_arg(ap, unsigned); /*lint !e732 !e713 */
                xputc(n, cookie);
                fmt += 2;
                continue;
            }
            case 'h': {
                unsigned n = va_arg(ap, unsigned); /*lint !e732 !e713 */
                xputc(hex2asc((int)(n >> 12)), cookie);
                xputc(hex2asc((int)(n >> 8)), cookie);
                xputc(hex2asc((int)(n >> 4)), cookie);
                xputc(hex2asc((int)n), cookie);
                fmt += 2;
                continue;
            }
            case 'b': {
                unsigned n = va_arg(ap, unsigned); /*lint !e732 !e713 */
                xputc(hex2asc((int)(n >> 4)), cookie);
                xputc(hex2asc((int)n), cookie);
                fmt += 2;
                continue;
            }                
            case 'p':
            case 'X':
            case 'x': {
                unsigned n = va_arg(ap, unsigned); /*lint !e732 !e713 */
                char *p = scratch + 15;
                *p = 0;
                do {
                    *--p = (char)hex2asc((int)n);
                    n = n >> 4; 
                } while(n != 0);
                while(p > (scratch + 7)) *--p = '0';
                xputs(p, xputc, cookie);
                fmt += 2;
                continue;
            }
            case 'd': {
                int n = va_arg(ap, int); /*lint !e732 !e713 */
                char *p = scratch + 15;
                *p = 0;
                if(n < 0) {
                    xputc('-', cookie);
                    n = -n;
                }
                do {
                    *--p = (n % 10) + '0';
                    n /= 10;
                } while(n != 0);
                xputs(p, xputc, cookie);
                fmt += 2;
                continue;
            }
            case 's': {
                char *s = (char*)va_arg(ap, char*); /*lint !e732 !e713 */
                if(s == 0) s = "(null)";
                xputs(s, xputc, cookie);
                fmt += 2;
                continue;
            }
            }/*lint !e744 */
            xputc(*fmt++, cookie);  /*lint !e732 */
            break;
			/* coverity[unterminated_case] */
        case '\n':
            xputc((u32)'\r', cookie);
			/* coverity[fallthrough] */
        default:
            xputc(*fmt++, cookie);  /*lint !e732 */
        }
    }
}

void printksync(const char *fmt, ...)
{
    va_list ap = 0;

    va_start(ap, fmt);
    xprintf(fmt, ap, (void*)uart_put, 0);
    va_end(ap);
}

#if 0
extern void pm_disable_all_gic(void);
int test_c_printksync()
{
	int i = 0;
	pm_disable_all_gic();
	while(i++ <= 10000);
	printksync("sljglj%X\n",0x1234567);
	return 0;
}
#endif
