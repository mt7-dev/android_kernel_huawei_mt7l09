/*
 *  arch/arm/mach-balong/include/mach/uncompress.h
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include <mach/hardware.h>
#include <asm/mach-types.h>

#include <mach/board.h>
#include <mach/balongv7r2_iomap.h>

#define AMBA_UART_DR(base)	(*(volatile unsigned int *)((base) + 0x00))
#define AMBA_UART_LCRH(base)	(*(volatile unsigned int *)((base) + 0x2c))
#define AMBA_UART_CR(base)	(*(volatile unsigned int *)((base) + 0x30))
#define AMBA_UART_LSR(base)	(*(volatile unsigned int *)((base) + 0x14))

/*
 * Return the UART base address
 */
static inline unsigned long get_uart_base(void)
{
	return HI_UART0_REGBASE_ADDR;
}

/*
 * This does not append a newline
 */
static inline void putc(int c)
{
	unsigned long base = get_uart_base();

	while (!(AMBA_UART_LSR(base) & 0x20));

	AMBA_UART_DR(base) = c;
}

static inline void flush(void)
{
}

/*
 * nothing to do
 */
#define arch_decomp_setup()
#define arch_decomp_wdog()
