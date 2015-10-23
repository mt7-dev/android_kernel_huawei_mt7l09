#ifndef _CHG_BOOT_H_
#define _CHG_BOOT_H_

/* 临时文件，充电代码合入后删除 */

typedef enum
{
    CHARGE_BATTERY_GOOD = 0,/*电池/charge 在位，并且电池是好的 voltage >= 3.0. go on  **/
    CHARGE_BATTERY_ONLY,    /*电池在位(no charge)，并且达到开机门限( >= 3.4v), go on*/
    CHARGE_BATTERY_LOWER,  /* 弱电池(no charger) ==> power off. < 3.4   */
    CHARGE_BATTERY_LOWER_WITH_CHARGE, /* 弱电池(charger) ==> power off. < 3.4   */
    CHARGE_BATTERY_BAD,     /*坏电池: charger ==> Wait  show message, only wait  charge be remove, then power off.  **/
    BATTERY_BAD_WITHOUT_CHARGE,/* 坏电池时拔出charger */
    CHARGE_BATTERY_OVER_TEMP, /* 过温 wait resume */
    CHARGE_BATTERY_MAX
}chg_batt_check_type;

chg_batt_check_type chg_battery_check_boot();
int display_logo(void);
void display_string(unsigned char* string);
void display_upgrade();
void boot_show_battery_bad();
void boot_show_battery_over_tmp();
void boot_show_battery_low();

#endif
