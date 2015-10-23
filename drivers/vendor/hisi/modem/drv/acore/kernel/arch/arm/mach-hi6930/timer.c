/*
 *  linux/arch/arm/mach-balongv7r2/timer.c
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
#include <linux/clocksource.h>
#include <linux/clockchips.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/io.h>

#include <mach/timer.h>
#include <asm/mach/time.h>
#include <mach/balongv7r2_iomap.h>

/*
 * These timers are currently always setup to be clocked at 1MHz.
 */
#define TIMER_FREQ_KHZ	(19200)
#define TIMER_RELOAD	(TIMER_FREQ_KHZ * 1000 / HZ)

static void __iomem *clksrc_base;

static cycle_t balong_timer_read(struct clocksource *cs)
{
    unsigned long ctrl;
    ctrl = readl(clksrc_base + TIMER_CTRL);
    ctrl |= TIMER_CTRL_LOCK;/* [false alarm]:误报 */
    writel(ctrl, clksrc_base + TIMER_CTRL);
    return ~readl(clksrc_base + TIMER_VALUE);
}

static struct clocksource clocksource_balong = {
	.name		= "balong clk cs",
	.rating		= 200,
	.read		= balong_timer_read,
	.mask		= CLOCKSOURCE_MASK(32),
	.shift		= 20,
	.flags		= CLOCK_SOURCE_IS_CONTINUOUS,
};

void __init balong_clocksource_init(void __iomem *base)
{
	struct clocksource *cs = &clocksource_balong;

	clksrc_base = base;

	/* setup timer 9 as free-running clocksource */
	writel(0, clksrc_base + TIMER_CTRL);
	writel(0xffffffff, clksrc_base + TIMER_LOAD);	
	writel(TIMER_CTRL_ENABLE | TIMER_CTRL_INTDIS,
		   clksrc_base + TIMER_CTRL);

	cs->mult = clocksource_khz2mult(TIMER_FREQ_KHZ, cs->shift);
	clocksource_register(cs);
}

static void __iomem *clkevt_base;

/*
 * IRQ handler for the timer
 */
static irqreturn_t balong_timer_interrupt(int irq, void *dev_id)
{
    struct clock_event_device *evt = dev_id;
    unsigned long ctrl;

    /* clear the interrupt */
	readl(clkevt_base + TIMER_INTCLR);

	if (CLOCK_EVT_MODE_ONESHOT == evt->mode)
	{
		ctrl = readl(clkevt_base + TIMER_CTRL);
		ctrl &= ~(TIMER_CTRL_ENABLE);/* [false alarm]:误报 */
		writel(ctrl, clkevt_base + TIMER_CTRL);
	}

	evt->event_handler(evt);

	return IRQ_HANDLED;
}

static void balong_timer_set_mode(enum clock_event_mode mode,
	struct clock_event_device *evt)
{
	unsigned long ctrl = 0;

	switch (mode) {
	case CLOCK_EVT_MODE_PERIODIC:
		writel(TIMER_RELOAD, clkevt_base + TIMER_LOAD);
		ctrl |= TIMER_CTRL_PERIODIC | TIMER_CTRL_ENABLE;/* [false alarm]:误报 */
		break;

	case CLOCK_EVT_MODE_ONESHOT:
		/* period set, and timer enabled in 'next_event' hook */
		ctrl = 0;/* [false alarm]:误报 */
		break;

	case CLOCK_EVT_MODE_UNUSED:
	case CLOCK_EVT_MODE_SHUTDOWN:
	default:
		break;
	}

	writel(ctrl, clkevt_base + TIMER_CTRL);
}

static int balong_timer_set_next_event(unsigned long next,
	struct clock_event_device *evt)
{
	writel(next, clkevt_base + TIMER_LOAD);
	writel(TIMER_CTRL_ENABLE | TIMER_CTRL_PERIODIC, clkevt_base + TIMER_CTRL);

	return 0;
}

static struct clock_event_device balong_clockevent = {
	.name				= "balong clk evt",
	.shift				= 32,
	.features       	= CLOCK_EVT_FEAT_PERIODIC | CLOCK_EVT_FEAT_ONESHOT,
	.set_mode			= balong_timer_set_mode,
	.set_next_event		= balong_timer_set_next_event,
	.rating				= 300,
	.cpumask			= cpu_all_mask,
};

static struct irqaction balong_timer_irq = {
	.name		= "balong system timer",
	.flags		= IRQF_DISABLED | IRQF_TIMER | IRQF_IRQPOLL,
	.handler	= balong_timer_interrupt,
	.dev_id		= &balong_clockevent,
};

void __init balong_clockevent_init(void __iomem *base, unsigned int timer_irq)
{
	struct clock_event_device *evt = &balong_clockevent;

	clkevt_base = base;

	evt->irq = timer_irq;
	evt->mult = div_sc(TIMER_FREQ_KHZ, NSEC_PER_MSEC, evt->shift);
	evt->max_delta_ns = clockevent_delta2ns(0xffffffff, evt);
	evt->min_delta_ns  = clockevent_delta2ns(0xf, evt);

	setup_irq(timer_irq, &balong_timer_irq);
	clockevents_register_device(evt);
}

static void __init balongv7r2_timer_init(void)
{    
    balong_clockevent_init((void __iomem *)HI_TIMER_14_REGBASE_VIRT_ADDR, INT_LVL_TIMER14);
    balong_clocksource_init((void __iomem *)HI_TIMER_15_REGBASE_VIRT_ADDR);
}

struct sys_timer balongv7r2_timer = {
	.init = balongv7r2_timer_init
};

