#ifndef __POWER_EXCHANGE_H__
#define __POWER_EXCHANGE_H__

#include <power_com.h>

int clear_misc_message(void);
void power_on_wdt_cnt_set( void );
void power_on_reboot_flag_set( power_off_reboot_flag enFlag );
power_off_reboot_flag power_on_reboot_flag_get( void );
void power_on_start_reason_set( power_on_start_reason enReason );
power_on_start_reason power_on_start_reason_get( void );
void power_reboot_cmd_set( power_reboot_cmd cmd );
c_power_st_e power_on_c_status_get(void);

void print_exchange_addr( void );

#endif

