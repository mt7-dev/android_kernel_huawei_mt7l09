#include "balongv7r2/chg_boot.h"
#include "balongv7r2/lcd.h"
#include <boot/boot.h>

chg_batt_check_type chg_battery_check_boot()
{
    return CHARGE_BATTERY_GOOD;
}

s32 display_logo( void )
{
    return lcd_power_on();
}

void display_string(unsigned char* string)
{
    lcd_show_string(string);
}

void display_upgrade( void )
{
    return ;
}

void boot_show_battery_bad( void )
{
    display_string("BAD BATTERY");
}

void boot_show_battery_over_tmp()
{
    display_string("OVER HEATED");
}

void boot_show_battery_low( void )
{
    display_string("LOW BATTERY");
}

