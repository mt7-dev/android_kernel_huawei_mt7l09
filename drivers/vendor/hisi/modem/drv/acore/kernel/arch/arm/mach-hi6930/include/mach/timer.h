#ifndef BALONG_TIMER_H
#define BALONG_TIMER_H

#define TIMER_LOAD	    0x00			
#define TIMER_VALUE	    0x04			
#define TIMER_CTRL	    0x08			
#define TIMER_INTCLR	0x0c

#define TIMER_CTRL_PERIODIC	(1 << 1)	
#define TIMER_CTRL_ENABLE	0x01	
#define TIMER_CTRL_INTDIS   (1 << 2)
#define TIMER_CTRL_LOCK     (1 << 3)

void dw_clocksource_init(void __iomem *);
void dw_timer_clockevents_init(void __iomem *, unsigned int);

extern struct sys_timer balongv7r2_timer;

#endif
