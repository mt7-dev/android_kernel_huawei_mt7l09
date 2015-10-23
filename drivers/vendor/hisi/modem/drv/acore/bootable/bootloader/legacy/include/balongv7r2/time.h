#ifndef __TIME_H__
#define __TIME_H__


void sleep(unsigned int sleep_ms);

int init_system_timer(void);
unsigned int get_timer_value(void);
unsigned int get_passed_ms(unsigned int timer_value_base);
unsigned int get_elapse_ms(void);

int timer_init(void);

#endif
