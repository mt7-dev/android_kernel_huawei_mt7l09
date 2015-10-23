#ifndef _BSP_ONOFF_H_
#define _BSP_ONOFF_H_

#include <osl_types.h>
#include <osl_sem.h>
#include "drv_onoff.h"
#include "power_com.h"

typedef struct _tagPowCtrl
{
    unsigned int    is_init;        /* 1:have initialized*/
    osl_sem_id      pwr_sem;
    stCtrlMsg       curr;
}stPowCtrl;

void power_on_c_status_set(c_power_st_e val);
int bsp_on_off_init( void );

#endif
