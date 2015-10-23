#ifndef __HARDTIMER_BALONG_H__
#define __HARDTIMER_BALONG_H__
#include <osl_types.h>
#include <hi_timer.h>
#include <soc_clk_app.h>
#include <product_config.h>

#ifndef OK
#define OK 0
#endif
#ifndef ERROR
#define ERROR  (-1)
#endif
#ifndef SEM_EMPTY
#define SEM_EMPTY 0
#endif

#ifndef SEM_FULL
#define SEM_FULL 1
#endif
#define  Second_To_Millisecond                               1000

#define MAX_COUNT                                               (0xFFFFFFFF/1000)
#define TCXO_CLK_MAX_LENGTH                      (0xFFFFFFFF/HI_TCXO_CLK) 


#define TIMERS_INTSTATUS(hard_timer_module_addr)                  (hard_timer_module_addr + 0xA0)
#define TIMERS_EOI(hard_timer_module_addr)                        (hard_timer_module_addr+ 0xA4 )
#define TIMERS_RAWINTSTATUS(hard_timer_module_addr)               (hard_timer_module_addr+ 0xA8)
#define TIMER_LOADCOUNT(hard_timer_id_addr)                       (hard_timer_id_addr + HI_TIMER_LOADCOUNT_OFFSET )
#define TIMER_LOADCOUNT_H(hard_timer_id_addr)                     (hard_timer_id_addr + HI_TIMER_LOADCOUNT_H_OFFSET )
#define TIMER_CURRENTVALUE(hard_timer_id_addr)                    (hard_timer_id_addr + HI_TIMER_CURRENTVALUE_OFFSET )
#define TIMER_CURRENTVALUE_H(hard_timer_id_addr)                  (hard_timer_id_addr +  HI_TIMER_CURRENTVALUE_H_OFFSET)
#define TIMER_CONTROLREG(hard_timer_id_addr)                      (hard_timer_id_addr + HI_TIMER_CONTROLREG_OFFSET )
#define TIMER_EOI(hard_timer_id_addr)                             (hard_timer_id_addr +  HI_TIMER_EOI_OFFSET)
#define TIMER_INTSTATUS(hard_timer_id_addr)                       (hard_timer_id_addr + HI_TIMER_INTSTATUS_OFFSET )

#define TIMER_EN_ACK                                 (1<<4)
#define TIMER_INT_MASK                               (1<<2)
#define TIMER_MODE_PERIOD                            (1<<1)
#define HARD_TIMER_ENABLE                             1
#define HARD_TIMER_DISABLE                            0
  

#endif
